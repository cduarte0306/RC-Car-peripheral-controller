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

#define APP_JUMP_TIMEOUT  5000  // 5 seconds

typedef void (*cy_app_entry_t)(void);

/**
 * @brief Main app metadata states
 * 
 */
enum
{
    eAppStateNone = 1,      /**< No app present on Main app sector           */
    eAppStateIncomplete,    /**< App write was started but was not completed */
    eAppStateValid          /**< App has been written and is present         */
};

volatile struct Message
{
    tBlXfer xfer;
    uint8_t* pBuff;    
} msg;

typedef struct __attribute__((__packed__))
{
    uint8_t  appImgState;   /**< State of app image          */
    uint32_t appCrc;        /**< App's CRC32                 */
    uint32_t appStart;      /**< Start address of image      */
    uint32_t appEnd;        /**< End address of image region */
} tMetaData_t;

typedef struct
{
    uint32_t initial_sp;
    uint32_t reset_handler;
} cy_app_vectors_t;


CY_NOINIT static volatile uint32_t bootEntryFlag;

uint32_t firmwareWriteCommand = Bootloader_Noop;
uint32_t firmwareWriteStatus  = Bl_Ok;
volatile uint8_t commsStarted = FALSE;
static volatile uint32 g_ms;

static uint32_t blParseCommand();
static void blJumpToMainApp();
static void blDoUpgrade();
static cystatus BlWriteRow(uint32_t absoluteRow, const uint8_t rowData[CY_ROW_LENGTH]);
static uint8_t BlCheckMetadata();
static cystatus BlWriteMetadata(uint32_t dataSize);
extern uint8_t blCommsPoll(uint8_t** rxBuffer, size_t* len);

void Bootloader_Start()
{
    // Check if we should jump to the image or jump to bootloader mode
    Bootloader_SPI_Start();

    // Verify main app metadata
    vLoggingPrintf(DEBUG_INFO, LOG_PSOC, "Starting flash\r\n");

    (void) CySetTemp();

    if ((bootEntryFlag == BOOTLOADER_ENTRY_MAGIC) || (!BlCheckMetadata()))
    {
        vLoggingPrintf(DEBUG_INFO, LOG_PSOC, "Entering updating stage\r\n");
        blDoUpgrade();
    }

    while (!BlCheckMetadata())
    {
        firmwareWriteStatus = Bl_Err;
        vLoggingPrintf(DEBUG_ERROR, LOG_PSOC, "No valid app image; staying in bootloader\r\n");
        blDoUpgrade();
    }

    bootEntryFlag = 0;
    vLoggingPrintf(DEBUG_INFO, LOG_PSOC, "Jumping to main app\r\n");
    blJumpToMainApp();
}

void SetMessage(const uint8_t* payload, const tBlXfer* xfer)
{
    msg.pBuff = (uint8_t*)payload;
    msg.xfer = *xfer;
    commsStarted   = TRUE;
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
    size_t len = 0;
    uint32_t timeBeg = xGetTimestamp();
    uint32_t dataSize = 0;
    PWM_Start();

    while (firmwareWriteStatus != Bl_Finished)
    {
        if (commsStarted)
        {
            firmwareWriteStatus = blParseCommand();
            dataSize += CY_ROW_LENGTH;
            commsStarted = FALSE;
        }
    }

    if (firmwareWriteStatus != Bl_Finished)
    {
        vLoggingPrintf(DEBUG_INFO, LOG_PSOC, "Failed to update app image\r\n");
    }
    else
    {
        vLoggingPrintf(DEBUG_INFO, LOG_PSOC, "Update finalized\r\n");

        // Wait 5 seconds before jumping to the image
        timeBeg = xGetTimestamp();
        uint8_t countDown = APP_JUMP_TIMEOUT / 1000;
        while(xGetElapsed(timeBeg) < APP_JUMP_TIMEOUT)
        {
            if (!(xGetElapsed(timeBeg) % 1000))
            {
                vLoggingPrintf(DEBUG_INFO, LOG_PSOC, "Jumping to image in %d seconds\r", countDown --);
            }
        }
    }
    BlWriteMetadata(dataSize);
}

static uint32_t blParseCommand()
{
    cystatus ret;
    uint32_t crc = 0;
    uint32_t stateReply = Bl_Ok;
    switch (msg.xfer.cmd)
    {
        case BootLoader_WriteRow:
            crc = xCRC32(msg.pBuff, CY_ROW_LENGTH);
            if (crc == msg.xfer.crc32)
            {
                // Bootloader_SPI_SetResponse((uint8_t)Bl_WriteInProgress);
                ret = BlWriteRow(msg.xfer.row, msg.pBuff);
                if (ret == CYRET_SUCCESS)
                {
                    stateReply = Bl_Ok;
                }
                else
                {
                    vLoggingPrintf(DEBUG_ERROR, LOG_PSOC, "Failed to write row %d\r\n", msg.xfer.row);
                    stateReply = Bl_Err;
                }
            }
            else
            {
                vLoggingPrintf(DEBUG_ERROR, LOG_PSOC, "Invalid CRC32 detected\r\n");
                stateReply = Bl_Err;
            }
            break;
        case Bootloader_Verify_Write:  // Does noithing
            break;
        case Bootloader_Finalize:
            stateReply = Bl_Finished;
            break;
        default:
            vLoggingPrintf(DEBUG_ERROR, LOG_PSOC, "Unknown bootloader command\r\n");
            stateReply = Bl_Err;
            break;
    }

    // Bootloader_SPI_SetResponse((uint8_t)stateReply);  // Stage the reply byte
    return stateReply;
}

static cystatus BlWriteRow(uint32_t absoluteRow, const uint8_t rowData[CY_ROW_LENGTH])
{
    /* absoluteRow is a ROW NUMBER (0-1023), not a byte address - comparing
     * it against APPL_METADATA (a byte address, 0x8000) was always true
     * and never actually rejected anything. CY_FIRST_APP_ROW is the same
     * boundary already expressed in row units. This also keeps the
     * metadata row itself (row 128, [0x8000,0x8100)) out of reach of the
     * normal WriteRow path - it's written only by Bootloader_Finalize. */
    if (absoluteRow < CY_FIRST_APP_ROW)
    {
        return CYRET_BAD_PARAM;
    }

    uint16_t arrayId    = (uint8_t ) (absoluteRow / (CY_FLASH_SECTOR_SIZE / CY_ROW_LENGTH));
    uint16_t rowInArray = (uint16_t) (absoluteRow % (CY_FLASH_SECTOR_SIZE / CY_ROW_LENGTH));
    return CyWriteRowData(arrayId, rowInArray, rowData);
}

static cystatus BlWriteMetadata(uint32_t dataSize)
{
    // Calculate metadata and write it to the flash memory
    uint32_t crc = xCRC32((const uint8_t*)APPL_START_ADDR, dataSize);
    tMetaData_t metadata;
    metadata.appStart = APPL_START_ADDR;
    metadata.appEnd = APPL_START_ADDR + dataSize;
    metadata.appCrc = crc;
    metadata.appImgState = eAppStateValid;
    uint8_t row = APPL_METADATA / CY_ROW_LENGTH;

    return CyWriteRowData(APPL_METADATA, row, (const uint8_t*)&metadata);
}

static uint8_t BlCheckMetadata()
{
    const tMetaData_t* metadata = (const tMetaData_t*)APPL_METADATA;
    if (metadata->appImgState != eAppStateValid)
    {
        vLoggingPrintf(DEBUG_INFO, LOG_PSOC, "Metadata Check | No image detected in sector\r\n");
        return FALSE;
    }

    if ((metadata->appStart != APPL_START_ADDR) ||
        (metadata->appEnd <= metadata->appStart) ||
        (metadata->appEnd > CY_FLASH_SIZE) ||
        ((metadata->appEnd - metadata->appStart) > CY_IMAGE_MAX_SIZE))
    {
        vLoggingPrintf(DEBUG_INFO, LOG_PSOC, "Metadata Check | Invalid image bounds\r\n");
        return FALSE;
    }

    const uint8_t* imgBuff = (const uint8_t*)metadata->appStart;
    uint32_t imageLength = metadata->appEnd - metadata->appStart;
    uint32_t crc = xCRC32(imgBuff, imageLength);
    if (crc != metadata->appCrc)
    {
        vLoggingPrintf(DEBUG_INFO, LOG_PSOC, "Metadata Check | Invalid image CRC detected\r\n");
        return FALSE;
    }

    vLoggingPrintf(DEBUG_INFO, LOG_PSOC, "Metadata Check | Valid image CRC at address: 0x%08lX\r\n",
                   (unsigned long)metadata->appStart);
    return TRUE;
}