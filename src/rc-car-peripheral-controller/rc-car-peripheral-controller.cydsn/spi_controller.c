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
#define READ_TRANACTION   2U


/* DMA Configuration for DMA_TX */
#define DMA_TX_BYTES_PER_BURST      (1u)
#define DMA_TX_REQUEST_PER_BURST    (1u)
#define DMA_TX_SRC_BASE             (CYDEV_SRAM_BASE)
#define DMA_TX_DST_BASE             (CYDEV_PERIPH_BASE)

/* DMA Configuration for DMA_RX */
#define DMA_RX_BYTES_PER_BURST      (1u)
#define DMA_RX_REQUEST_PER_BURST    (1u)
#define DMA_RX_SRC_BASE             (CYDEV_PERIPH_BASE)
#define DMA_RX_DST_BASE             (CYDEV_SRAM_BASE)

#define BUFFER_SIZE                 (8u)
#define STORE_TD_CFG_ONCMPLT        (1u)


typedef struct
{
    uint8_t transactionType;
    uint8 reg;
    val_type_t data;
    uint8_t ack;
} spiTransactionStruct;


volatile static uint8_t rxBuffer[sizeof(spiTransactionStruct)];
volatile static uint8_t txBuffer[sizeof(spiTransactionStruct)];
volatile static uint8_t rxStatus;
volatile static uint8_t txStatus;

volatile static uint8_t bufferIndexRx = 0;
volatile static uint8_t bufferIndexTx = 0;

volatile regMapType* regMap = NULL;

static uint8 txChannel;
static uint8 txTD;


static uint8_t configDMA( void );


CY_ISR(spi_rx_handler)
{
    rxStatus = SPIS_GET_STATUS_RX(SPIS_swStatusRx);
    if ( rxStatus & SPIS_STS_RX_FIFO_NOT_EMPTY )
    {
        rxBuffer[ bufferIndexRx ++ ] = CY_GET_REG8(SPIS_RXDATA_PTR);
        if ( bufferIndexRx >= sizeof(spiTransactionStruct) )
        {
            bufferIndexRx = 0;    
        }    

        spiTransactionStruct* rx = (spiTransactionStruct*)rxBuffer;
        if( rx->transactionType == WRITE_TRANSACTION )
        {
            if (regMap->regType == READ_WRITE)
            {
                regMap->data.u32 = rx->data.u32;
            }

            spiTransactionStruct* buff = (spiTransactionStruct*)txBuffer;
            buff->ack = TRUE;
        }
        else if ( rx->transactionType == READ_TRANACTION )
        {
            spiTransactionStruct* buff = (spiTransactionStruct*)txBuffer;
           
            // Place the data in the tx buffer
            buff->data.u32 = txBuffer[rx->data.u32];
        }
        
        SPIS_RX_STATUS_MASK_REG &= ((uint8)~SPIS_STS_RX_FIFO_NOT_EMPTY);
    }

    *spi_rx_interrupt_INTC_CLR_PD = spi_rx_interrupt__INTC_MASK;
}


CY_ISR(end_of_message_handler)
{
    spiTransactionStruct* buff = (spiTransactionStruct*)txBuffer;
    buff->ack = FALSE;

    *end_of_message_INTC_CLR_PD = end_of_message__INTC_MASK;
}


void SPI_controller_start(void)
{
    spi_rx_interrupt_Start();
    spi_rx_interrupt_StartEx( spi_rx_handler );
        
    end_of_message_Start();
    end_of_message_StartEx(end_of_message_handler); 
    
    SPIS_Start();
    
    SPIS_ClearFIFO();
    SPIS_ClearRxBuffer();
    SPIS_ClearTxBuffer();
    
    regMap = getRegRef();
    CYASSERT(regMap != NULL);
    
    // Configure DMA for transmission
    memset((uint8*)&txBuffer, 0, sizeof(txBuffer));
    CYASSERT(configDMA);
}


static uint8_t configDMA(void)
{
    cystatus ret;
    
    txChannel = DMA_SPI_TX_DmaInitialize(DMA_TX_BYTES_PER_BURST, DMA_TX_REQUEST_PER_BURST, 
                                        HI16(DMA_TX_SRC_BASE), HI16(DMA_TX_DST_BASE));

    txTD = CyDmaTdAllocate();

    ret = CyDmaTdSetConfiguration(txTD, (BUFFER_SIZE-1), CY_DMA_DISABLE_TD, TD_INC_SRC_ADR);
    if (ret != CYRET_SUCCESS)
    {   
        return RET_FAIL;
    }
    
    ret = CyDmaTdSetAddress(txTD, LO16((uint32) txBuffer), LO16((uint32) SPIS_TXDATA_PTR));
    if (ret != CYRET_SUCCESS)
    {   
        return RET_FAIL;
    }
    
    ret = CyDmaChSetInitialTd(txChannel, txTD);
    if (ret != CYRET_SUCCESS)
    {   
        return RET_FAIL;
    }
    
    ret = CyDmaClearPendingDrq(txChannel);
    if (ret != CYRET_SUCCESS)
    {
        return RET_FAIL;
    }
    
    /* Enable the DMA */
    ret = CyDmaChEnable(txChannel, 1);
    if (ret != CYRET_SUCCESS)
    {
        return RET_FAIL;
    }
    
    return RET_PASS;
}

/* [] END OF FILE */
