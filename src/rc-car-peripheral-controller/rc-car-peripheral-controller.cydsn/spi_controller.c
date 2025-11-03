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

#include <project.h>

#define WRITE_TRANSACTION 1U
#define READ_TRANSACTION  0U
#define STORE_TD_CFG_ONCMPLT        (1u)


typedef struct __attribute__((__packed__))
{
    uint8_t transactionType;
    uint8_t reg;
    val_type_t data;
    uint8_t ack;
} spiTransactionStruct;


volatile static uint8_t rxBuffer[sizeof(spiTransactionStruct)    ] = { 0 };
volatile static uint8_t txBuffer[sizeof(spiTransactionStruct) + 1] = { 0 };
volatile static uint8_t rxStatus;
volatile static uint8_t txStatus;

volatile static uint8_t bufferIndexRx = 0;
volatile static uint8_t bufferIndexTx = 0;

volatile regMapType* regMap = NULL;
uint8_t retRegStatus;


CY_ISR(rxHandler)
{
    uint8 tmpStatus;
    tmpStatus = SPIS_GET_STATUS_RX(SPIS_swStatusRx);
    SPIS_swStatusRx = tmpStatus;
    
    while((SPIS_swStatusRx & SPIS_STS_RX_FIFO_NOT_EMPTY) != 0u)
    {
        uint8 rxData = CY_GET_REG8(SPIS_RXDATA_PTR);

        /* Move data from the FIFO to the Buffer */
        rxBuffer[bufferIndexRx] = rxData;

        /* Set next pointer. */
        bufferIndexRx++;
        if(bufferIndexRx >= sizeof(spiTransactionStruct))
        {
            bufferIndexRx = 0u;
        }

        tmpStatus = SPIS_GET_STATUS_RX(SPIS_swStatusRx);
        SPIS_swStatusRx = tmpStatus;
    }

    *rx_interrupt_INTC_CLR_PD = rx_interrupt__INTC_MASK;
}


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
    
    spiTransactionStruct *rx = (spiTransactionStruct*)rxBuffer;
    spiTransactionStruct *tx = (spiTransactionStruct*)txBuffer;

    if (rx->transactionType == WRITE_TRANSACTION) {
        if (regMap && regMap->regType == READ_WRITE)
            regMap->data.u32 = rx->data.u32;
        tx->transactionType = WRITE_TRANSACTION;
        tx->reg  = rx->reg;
        retRegStatus = rdReg(rx->reg, &val);
        if (retRegStatus)
        {
            tx->data.u32 = val.data.u32;    
            tx->ack = TRUE;
        }
        else
        {
            tx->data.u32 = READ_TRANSACTION;
            tx->reg= 0;
            tx->data.u32 = 0;
            tx->ack = FALSE;
        }
    }
    else
    {
        tx->data.u32 = READ_TRANSACTION;
        tx->reg= 0;
        tx->data.u32 = 0;
        tx->ack = FALSE;
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
}


/**
 * @brief Initializes the SPI controller
 * 
 * @return uint8_t RET_PASS on success, RET_FAIL on failure
 */
uint8_t SPI_controller_start(void)
{    
    vLoggingPrintf(DEBUG_INFO, LOG_SPI, "app: SPI_controller_start | Initializing SPI controller\r\n");
    
    rx_interrupt_Start();
    rx_interrupt_StartEx(rxHandler);

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

    if (regMap == NULL)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_SPI, "app: SPI_controller_start | err: Could not read register map\r\n");
        return RET_FAIL;
    }
    
    vLoggingPrintf(DEBUG_INFO, LOG_SPI, "app: SPI_controller_start | SPI controller initialized\r\n");
    return RET_PASS;
}


/* [] END OF FILE */
