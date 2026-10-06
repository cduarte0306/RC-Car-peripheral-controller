#include "bootloader_spi.h"
#include "bootloader.h"
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

volatile static uint8_t txReady = FALSE;
static uint8_t rxChannel;
static uint8 rxTD;
extern uint8_t firmwareWriteStatus;

volatile static uint8_t pwmSet = FALSE;
volatile static uint8_t rxLen = 0;
volatile static uint16_t rxBytesReceived = 0;
volatile static uint16_t bufferIndexTx = 0;
static uint8_t txBufferBytes = 0;
volatile uint8_t retRegStatus;

volatile static uint8_t rxBuffer[SPI_RX_BUFFER_SIZE ] = { 0 };
volatile static uint8_t txBuffer[sizeof(tBlXfer) + 1] = { 0 };

CY_ISR(txHandler)
{
    // Read current TX status
    uint8_t status = SPIS_TX_STATUS_REG;

    // Fill TX FIFO until it's full or we've sent the whole struct
    while (status & SPIS_STS_TX_FIFO_NOT_FULL &&
           (bufferIndexTx < sizeof(tBlXfer)))
    {
        CY_SET_REG8(SPIS_TXDATA_PTR, txBuffer[txBufferBytes++]); // Place byte in TX Buffer
        status = SPIS_TX_STATUS_REG;  // refresh inside loop]]
    }

    // Clear pending flag at the end
    *tx_interrupt_INTC_CLR_PD = tx_interrupt__INTC_MASK;
}

CY_ISR(rxHandler)
{
    // Read current TX status
    uint8_t status = SPIS_RX_STATUS_REG;

    while ((status & SPIS_STS_RX_FIFO_NOT_EMPTY) &&
           (rxBytesReceived < sizeof(rxBuffer)))
    {
        rxBuffer[rxBytesReceived ++] = CY_GET_REG8(SPIS_RXDATA_PTR);
        status = SPIS_RX_STATUS_REG;
    }

    // Clear pending flag at the end
    *rx_interrupt_INTC_CLR_PD = rx_interrupt__INTC_MASK;
}

CY_ISR(end_of_message_handler)
{
    bufferIndexTx = 0;
    *end_of_message_INTC_CLR_PD = end_of_message__INTC_MASK;

    // Reset the received byte counter
    rxBytesReceived = 0;

    // Clear the previous buffer and place the first 4 bytes (size of HW FIFO) in
    SPIS_ClearFIFO();

    // Only set the message ready if not a Noop
    uint8_t msgReady = FALSE;
    tBlXfer* hdr = ((tBlXfer*)rxBuffer);
    tBlXfer* tx =  ((tBlXfer*)txBuffer);
    tx->cmd = hdr->cmd;

    if (hdr->cmd != Bootloader_Noop &&
        hdr->cmd != BootLoader_Ping &&
        pwmSet == FALSE)
    {
        CY_SET_REG16(PWM_PERIOD_LSB_PTR, (uint16)127);
        CY_SET_REG16(PWM_COMPARE1_LSB_PTR, 63);
        pwmSet = TRUE;
    }

    switch (hdr->cmd)
    {
        case Bootloader_Noop:
            break;
        case BootLoader_Ping:
            tx->status = Bl_Ping;
            break;
        case Bootloader_Verify_Write:
            tx->status = firmwareWriteStatus;
            break;
        case BootLoader_WriteRow:
            firmwareWriteStatus = Bl_WriteInProgress;  // Let the server know a write will now take place
        default: msgReady = TRUE;
    }

    if (msgReady)
    {
        SetMessage(rxBuffer + sizeof(tBlXfer), hdr);
    }

    // Place data in the FIFO
    CY_SET_REG8(SPIS_TXDATA_PTR, txBuffer[1]);
    CY_SET_REG8(SPIS_TXDATA_PTR, txBuffer[2]);
    CY_SET_REG8(SPIS_TXDATA_PTR, txBuffer[3]);
    CY_SET_REG8(SPIS_TXDATA_PTR, txBuffer[4]);
    txBufferBytes = 5;  // 4 preloaded bytes into FIFO
    txReady = FALSE;
}

void Bootloader_SPI_Start()
{
    // Configure rx and tx interrupts
    tx_interrupt_Start();
    tx_interrupt_StartEx(txHandler);

    rx_interrupt_Start();
    rx_interrupt_StartEx(rxHandler);

    end_of_message_Start();
    end_of_message_StartEx(end_of_message_handler);
    // CHECK(configRxDMA());
}

void Bootloader_SPI_SetResponse(uint8_t status)
{
    tBlXfer* buff = (tBlXfer*) txBuffer;
    buff->status = status;
    buff->crc32 = xCRC32((uint8_t*)txBuffer, sizeof(tBlXfer) - sizeof(buff->crc32));
}