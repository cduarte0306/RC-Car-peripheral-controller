#include "bootloader_spi.h"
#include <project.h>

#include "RCUtils.h"
#include "MotorBootloaderDefs.h"

#define DMA_SPI_RX_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_SPI_TX_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_SPI_RX_DST_BASE (CYDEV_SRAM_BASE)
#define SPI_RX_BUFFER_SIZE  1024

typedef struct __attribute__((__packed__))
{
    uint8_t status;
} spiTransactionStruct;

typedef struct __attribute__((__packed__))
{
    uint8_t len;
} tSpiRxHdr;

static uint8_t txReady = FALSE;
static uint8_t rxChannel;
static uint8 rxTD;

volatile static uint8_t msgReady = FALSE;
volatile static uint8_t rxLen = 0;

volatile static uint8_t rxBuffer[SPI_RX_BUFFER_SIZE              ] = { 0 };
volatile static uint8_t txBuffer[sizeof(tCmdHdr)] = { 0 };

static uint8_t configRxDMA(void);

CY_ISR(txHandler)
{
    
}

CY_ISR(end_of_message_handler)
{
    *end_of_message_INTC_CLR_PD = end_of_message__INTC_MASK;

    /* Disable first for a clean, deterministic restart: a message shorter
     * than the max transfer size (the normal case - messages vary in size)
     * leaves the channel still enabled and waiting for more bytes that will
     * never arrive for THIS message, so it must be stopped before being
     * re-armed for the next one. */
    CyDmaChDisable(rxChannel);

    /* TD_INC_DST_ADR advances the TD's OWN stored destination address as
     * bytes arrive - CyDmaChSetInitialTd() alone does not restore it, so
     * without this, every subsequent message keeps writing further past
     * rxBuffer instead of restarting at the beginning. */
    CyDmaTdSetAddress(rxTD, LO16((uint32)SPIS_RXDATA_PTR), LO16((uint32)rxBuffer));

    CyDmaChSetInitialTd(rxChannel, rxTD);
    CyDmaChEnable(rxChannel, 1);

    msgReady = TRUE;    /* a full message is ready for blCommsPoll() to consume */
    txReady = FALSE;
}

void Bootloader_SPI_Start()
{
    // Configure rx and tx interrupts
    end_of_message_Start();
    end_of_message_StartEx(end_of_message_handler);
 
    tx_interrupt_Start();
    tx_interrupt_StartEx(txHandler);

    CHECK(configRxDMA());
}

/**
 * @brief Configures the DMA channel to receive data from the SPI RX FIFO.
 *
 * Moves bytes from SPIS_RXDATA_PTR → rxBuffer[] automatically.
 * Restarts when the entire spiTransactionStruct has been received.
 */
static uint8_t configRxDMA(void)
{
    cystatus ret;

    /* Initialize RX DMA channel */
    rxChannel = DMA_SPI_RX_DmaInitialize(
        1,     // 1 byte per burst
        1,     // 1 request per burst
        HI16(DMA_SPI_RX_SRC_BASE),  // High 16 bits of source base (peripheral)
        HI16(CYDEV_SRAM_BASE));     // High 16 bits of destination base (SRAM)
    if (rxChannel == DMA_INVALID_CHANNEL)
    {
        return RET_FAIL;
    }

    /* Allocate a Transfer Descriptor (TD) */
    rxTD = CyDmaTdAllocate();
    if (rxTD == CY_DMA_INVALID_TD)
    {
        return RET_FAIL;
    }

    /* Configure the TD
     *
     * transferCount is the MAX a single message can be, not the actual
     * message size - messages vary in size and the real length is
     * self-described by the tHdr.len the sender embeds in the data itself.
     * The DMA just captures however many bytes actually arrive (up to this
     * cap) before end_of_message_handler fires and resets it for the next
     * message. */
    ret = CyDmaTdSetConfiguration(
        rxTD,
        SPI_RX_BUFFER_SIZE,                       // Bytes per transfer (max)
        CY_DMA_DISABLE_TD,                        // One-shot mode, stop after completion
        DMA_SPI_RX__TD_TERMOUT_EN | TD_INC_DST_ADR); // Increment destination, assert TERMOUT
    if (ret != CYRET_SUCCESS)
    {
        return RET_FAIL;
    }

    /* Set source and destination addresses */
    ret = CyDmaTdSetAddress(
        rxTD,
        LO16((uint32)SPIS_RXDATA_PTR),           // Source: SPI RX register
        LO16((uint32)rxBuffer));                 // Destination: RX buffer
    if (ret != CYRET_SUCCESS)
    {
        return RET_FAIL;
    }

    /* Assign the TD to the channel */
    ret = CyDmaChSetInitialTd(rxChannel, rxTD);
    if (ret != CYRET_SUCCESS)
    {
        return RET_FAIL;
    }

    /* Clear any pending DMA requests before enabling */
    ret = CyDmaClearPendingDrq(rxChannel);
    if (ret != CYRET_SUCCESS)
    {
        return RET_FAIL;
    }

    /* Enable the RX DMA channel */
    ret = CyDmaChEnable(rxChannel, 1);
    if (ret != CYRET_SUCCESS)
    {
        return RET_FAIL;
    }

    return RET_PASS;
}

uint8 blCommsPoll(uint8_t** pBuf, size_t* len)
{
    ASSERT(pBuf != NULL);
    if (!msgReady) return FALSE;

    *pBuf = (uint8_t*) rxBuffer;
    if (len != NULL)
    {
        /* Upper bound only (the DMA's max transfer size) - the actual
         * message length is the tHdr.len the caller reads out of *pBuf
         * itself, since messages vary in size and aren't tracked here. */
        *len = SPI_RX_BUFFER_SIZE;
    }

    msgReady = FALSE;   /* consumed */
    return TRUE;
}

void Bootloader_SPI_SetResponse(uint8_t status)
{
    tCmdHdr* buff = (tCmdHdr*) txBuffer;
    buff->status = status;
    txReady = TRUE;
}