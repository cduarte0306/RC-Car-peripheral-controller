/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "spi_controller.h"
#include "types.h"
#include "rc_car.h"
#include "RCUtils.h"
#include "FreeRTOS.h"
#include "timers.h"

#include <project.h>


#define DMA_SPI_RX_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_SPI_TX_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_SPI_RX_DST_BASE (CYDEV_SRAM_BASE)


enum
{
    READ_TRANSACTION,          // Basic read transaction
    STAGE_RD_WRT_TRANSACTION,  // Stages a byte from the register map in the read buffer
    WRITE_REG_TRANSACTION      // Writes to selected register in register map
};

typedef struct __attribute__((__packed__))
{
    uint8_t transactionType;
    uint8_t reg;
    val_type_t data;
    uint8_t ack;
} spiTransactionStruct;

static uint8_t rxChannel;
static uint8 rxTD;

volatile static uint8_t rxBuffer[sizeof(spiTransactionStruct)    ] = { 0 };
volatile static uint8_t txBuffer[sizeof(spiTransactionStruct) + 1] = { 0 };
volatile static uint8_t rxStatus;
volatile static uint8_t txStatus;

volatile static uint8_t bufferIndexRx = 0;
volatile static uint8_t bufferIndexTx = 0;

volatile static uint16_t connectionTimer = SPI_CONNECTION_TIMEOUT;
volatile static uint8_t connectionFlag = pdFALSE;
TimerHandle_t xTimer;


volatile regMapType* regMap = NULL;
uint8_t retRegStatus;

static uint8_t configRxDMA(void);


static void vTimerCallback(TimerHandle_t xTimer);


CY_ISR(txHandler)
{
    // Read current TX status
    uint8_t status = SPIS_TX_STATUS_REG;

    // Fill TX FIFO until it's full or we've sent the whole struct
    while (retRegStatus &&(status & SPIS_STS_TX_FIFO_NOT_FULL) &&
           (bufferIndexTx < sizeof(spiTransactionStruct)))
    {
        CY_SET_REG8(SPIS_TXDATA_PTR, txBuffer[bufferIndexTx++]);
        status = SPIS_TX_STATUS_REG;  // refresh inside loop
    }

    // Clear pending flag at the end
    *tx_interrupt_INTC_CLR_PD = tx_interrupt__INTC_MASK;
}


CY_ISR(end_of_message_handler)
{
    end_of_message_ClearPending();
    regMapType val;
    uint8_t ret;
    
    spiTransactionStruct *rx = (spiTransactionStruct*)rxBuffer;
    spiTransactionStruct *tx = (spiTransactionStruct*)txBuffer;
    
    switch(rx->transactionType)
    {
        case STAGE_RD_WRT_TRANSACTION:
        {
            tx->transactionType = STAGE_RD_WRT_TRANSACTION;
            tx->reg  = rx->reg;
            retRegStatus = rdReg(rx->reg, &val);
            if (retRegStatus)
            {
                tx->data.u32 = val.data.u32;    
                tx->ack = TRUE;
                connectionFlag = pdTRUE;  // Mark connection as true
                connectionTimer = 0;      // Reset the counter
            }
            else
            {
                tx->data.u32 = READ_TRANSACTION;
                tx->reg= 0;
                tx->data.u32 = 0;
                tx->ack = FALSE;
            }
            break;
        }
        case READ_TRANSACTION:
            tx->data.u32 = READ_TRANSACTION;
            tx->reg= 0;
            tx->data.u32 = 0;
            tx->ack = FALSE;
            break;
        case WRITE_REG_TRANSACTION:
            val.data.u32 = tx->data.u32;
            ret = wrtReg(tx->reg, &val);
            if (!ret)
            {
                tx->ack = FALSE;
            }

            tx->data.u32 = 0;
            tx->ack = TRUE;
            break;
        default: break;
    }

    bufferIndexRx = 0;
    bufferIndexTx = 0;

    // Clear the previous buffer and place the first 4 bytes (size of HW FIFO) in
    SPIS_ClearFIFO();

    // Place data in the FIFO
    CY_SET_REG8(SPIS_TXDATA_PTR, txBuffer[1]);
    CY_SET_REG8(SPIS_TXDATA_PTR, txBuffer[2]);
    CY_SET_REG8(SPIS_TXDATA_PTR, txBuffer[3]);
    CY_SET_REG8(SPIS_TXDATA_PTR, txBuffer[4]);
    bufferIndexTx = 5;
    
    CyDmaChSetInitialTd(rxChannel, rxTD);
    CyDmaChEnable(rxChannel, 1);
}


/**
 * @brief Initializes the SPI controller
 * 
 * @return uint8_t RET_PASS on success, RET_FAIL on failure
 */
uint8_t SPI_controller_start(void)
{
    vLoggingPrintf(DEBUG_INFO, LOG_SPI, "app: SPI_controller_start | Initializing SPI controller\r\n");
    
    uint8_t ret;
    tx_interrupt_Start();
    tx_interrupt_StartEx(txHandler);

    end_of_message_Start();
    end_of_message_StartEx(end_of_message_handler); 
    
    SPIS_Start();
    SPIS_Start();
    SPIS_ClearFIFO();
    SPIS_ClearRxBuffer();
    SPIS_ClearTxBuffer();
    
    bufferIndexTx = 0;
    while ((SPIS_ReadTxStatus() & SPIS_STS_TX_FIFO_NOT_FULL) &&
           (bufferIndexTx < sizeof(spiTransactionStruct)))
    {
        CY_SET_REG8(SPIS_TXDATA_PTR, txBuffer[bufferIndexTx++]);
    }
    
    ret = configRxDMA();
    if (!ret)
    {
        vLoggingPrintf(DEBUG_INFO, LOG_SPI, "app: SPI_controller_start | err: Could not start DMA\r\n");
    }
    
    // Start timer
    xTimer = xTimerCreate(
        "connection timer",  // A descriptive name for the timer
        pdMS_TO_TICKS(1000), // Timer period: 1000 milliseconds
        pdTRUE,              // Auto-reload: Yes (timer restarts after expiring)
        (void *) 1,          // Timer ID: A value to identify this timer
        &vTimerCallback      // The function to call when the timer expires
    );
    if (xTimer == NULL)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_SPI, "app: SPI_controller_start | Failed to create software timer\r\n");
        return RET_FAIL;
    }
    
    vLoggingPrintf(DEBUG_INFO, LOG_SPI, "app: SPI_controller_start | SPI controller initialized\r\n");
    return RET_PASS;
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
        1,   // 1 request per burst
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

    /* Configure the TD */
    ret = CyDmaTdSetConfiguration(
        rxTD,
        sizeof(spiTransactionStruct),             // Bytes per transfer
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


uint8_t SPIGetConnectionStatus(void)
{
    return connectionFlag;
}


static void vTimerCallback(TimerHandle_t xTimer)
{
    (void) xTimer;
    
    // Increase the timeout
    if (connectionTimer <= SPI_CONNECTION_TIMEOUT)
    {
        connectionTimer ++;
    }
    else
    {
        connectionFlag = pdFALSE;  // Clear the connection flag
    }
}



/* [] END OF FILE */
