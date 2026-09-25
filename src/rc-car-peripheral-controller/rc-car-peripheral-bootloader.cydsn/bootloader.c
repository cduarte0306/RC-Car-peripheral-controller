#include "bootloader.h"
#include "RCUtils.h"
#include "logging.h"

#include <project.h>

#include "MotorBootloaderDefs.h"
#include "bootloader_spi.h"

#include "cytypes.h"
#include "cydevice.h"      /* CYDEV_SRAM_BASE / CYDEV_SRAM_SIZE */
#include "core_cm3.h"       /* NVIC, SCB */
#include "cmsis_gcc.h"      /* __set_MSP, __disable_irq */

#define APP_JUMP_TIMEOUT  5000  // 2 seconds

typedef void (*cy_app_entry_t)(void);

typedef struct
{
    uint32_t initial_sp;
    uint32_t reset_handler;
} cy_app_vectors_t;

CY_NOINIT static volatile uint32_t bootEntryFlag;
volatile uint8_t commsStarted = FALSE;

static volatile uint32 g_ms;

static uint8_t blParseCommand(const uint8_t* pData, uint16_t len);
static void blJumpToMainApp();
static void blDoUpgrade();
cystatus BlWriteRow(uint32_t absoluteRow, const uint8_t rowData[CY_ROW_LENGTH]);
extern uint8_t blCommsPoll(uint8_t** rxBuffer, size_t* len);

void Bootloader_Start()
{
    // Check if we should jump to the image or jump to bootloader mode
    Bootloader_SPI_Start();
    CyFlash_Start();
    (void) CySetTemp();

    if (bootEntryFlag == BOOTLOADER_ENTRY_MAGIC)
    {
        vLoggingPrintf(DEBUG_INFO, LOG_PSOC, "Entering updating stage\r\n");
        blDoUpgrade();
    }
    vLoggingPrintf(DEBUG_INFO, LOG_PSOC, "Jumping to main app\r\n");
    blJumpToMainApp();
}

inline void SetCommsStarted()
{
    commsStarted = TRUE;
}

static void blJumpToMainApp()
{
    const cy_app_vectors_t *appVectors = (const cy_app_vectors_t *)APPL_START_ADDR;

    /* UART_Debug_TX_STS_COMPLETE is a one-shot, read-to-clear event bit, not
     * a state bit - vLoggingPrintf()'s own internal FIFO polling already
     * reads this same TXSTATUS_REG and silently clears it as a side effect,
     * so waiting on TX_STS_COMPLETE hangs forever (the event already fired
     * and was already consumed before we ever get here). Poll
     * TX_STS_FIFO_EMPTY instead - it reflects live FIFO occupancy on every
     * read rather than a one-shot latch - and bound the wait so a bootloader
     * never hangs forever on an unexpected hardware status bit.
     *
     * This MUST run before __disable_irq() below: the bound relies on g_ms,
     * which only advances via the SysTick ISR, so timing out here after
     * interrupts are already disabled would never actually time out. */
    uint32_t txFlushDeadline = g_ms + 10u;
    while (((UART_Debug_TXSTATUS_REG & UART_Debug_TX_STS_FIFO_EMPTY) == 0u) && (g_ms < txFlushDeadline))
    {
        /* wait for the last byte to leave the FIFO, but don't hang forever */
    }

    // Stop peripherals and interrupts
    __disable_irq();

    SPIS_ClearFIFO();
    SPIS_ClearRxBuffer();
    SPIS_ClearTxBuffer();
    SPIS_Stop();
    PWM_Stop();
    UART_Debug_Stop();

    for (uint32_t i = 0u; i < (sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0])); i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFFu;   /* disable every external interrupt */
        NVIC->ICPR[i] = 0xFFFFFFFFu;   /* and clear anything left pending */
    }

    __set_MSP(appVectors->initial_sp);
    /* reset_handler already has the Thumb bit (bit 0) set - it's a plain
     * function pointer as written into RomVectors[] by the compiler. */
    ((cy_app_entry_t)appVectors->reset_handler)();

    for (;;) { /* unreachable */ }
}

static void blDoUpgrade()
{
    uint8_t* rxPointer = NULL;
    size_t len = 0;
    uint8_t ret = Bl_Ok;
    uint32_t timeBeg = xGetTimestamp();
    while ((xGetElapsed(timeBeg) < APP_JUMP_TIMEOUT) && (ret != Bl_Finished))
    {
        if (blCommsPoll(&rxPointer, &len))
        {
            CHECK(rxPointer != NULL);
            const tHdr* hdr = (const tHdr*)rxPointer;
            ret = blParseCommand((const uint8_t*)(hdr + 1), hdr->len);
            Bootloader_SPI_SetResponse(ret);
        }
    }

    if (ret != Bl_Finished)
    {
        vLoggingPrintf(DEBUG_INFO, LOG_PSOC, "Failed to update app image\r\n");
    }
    else
    {
        vLoggingPrintf(DEBUG_INFO, LOG_PSOC, "Update finalized\r\n");
    }
}

static uint8_t blParseCommand(const uint8_t* pData, uint16_t len)
{
    CHECK(pData != NULL);
    const tCmdHdr* hdr = (const tCmdHdr*)pData;
    static uint8_t verifyStatus = FALSE;
    cystatus ret;
    uint32_t crc = 0;
    switch (hdr->cmd)
    {
        case BootLoader_WriteRow:
            crc = xCRC32((uint8_t*)(hdr + 1), CY_ROW_LENGTH);
            if (crc == hdr->crc32)
            {
                ret = BlWriteRow(hdr->row, (uint8_t*)(hdr + 1));
                if (ret == CYRET_SUCCESS)
                {
                    return Bl_WriteInProgress;
                }
            }
            else
            {
                vLoggingPrintf(DEBUG_ERROR, LOG_PSOC, "Invalid CRC32 detected\r\n");
                return Bl_Err;
            }
            verifyStatus = crc == hdr->crc32;
            break;
        case Bootloader_Verify_Write:
            break;
        case Bootloader_Finalize:
            return Bl_Finished;
        default:
            break;
    }

    if (ret != CYRET_SUCCESS)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_PSOC, "Could not execute operation: %d\r\n", hdr->cmd);
        return Bl_Err;
    }

    return Bl_Ok;
}

cystatus BlWriteRow(uint32_t absoluteRow, const uint8_t rowData[CY_ROW_LENGTH])
{
    if (absoluteRow < APPL_METADATA)
    {
        return CYRET_BAD_PARAM;
    }

    uint16_t arrayId    = (uint8_t ) (absoluteRow / (CY_FLASH_SECTOR_SIZE / CY_ROW_LENGTH));
    uint16_t rowInArray = (uint16_t) (absoluteRow % (CY_FLASH_SECTOR_SIZE / CY_ROW_LENGTH));
    return CyWriteRowData(arrayId, rowInArray, rowData);
}