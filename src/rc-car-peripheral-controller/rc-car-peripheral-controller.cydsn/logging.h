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
#ifndef LOGGING_H
#define LOGGING_H

    
#include "RC_Config.h"
    
    
#if defined( UART_INCLUDE_OK )
    #include <string.h>
    #include <cytypes.h>

    #include UART_H

    /* For ensuring all pending UART Tx gets printed before continuing */
    #if !( defined(_UART_TxPending) )
        #if !( defined(_UART_TXSTATUS_REG) )
            #error-Please define `_UART_TXSTATUS_REG` as an alias to `<NAME>_TXSTATUS_REG` in 'LPUtilConfig.h'
        #else
            #if !( defined(UART_TXSTATUS_REG) )
                #define UART_TXSTATUS_REG _UART_TXSTATUS_REG
            #endif
        #endif

        #if !( defined(_UART_TX_STS_FIFO_EMPTY) )
            #error Please define `_UART_TX_STS_FIFO_EMPTY` as an alias to `<NAME>_TX_STS_FIFO_EMPTY` in 'LPUtilConfig.h'
        #else
            #if !( defined(UART_TX_STS_FIFO_EMPTY) )
                #define UART_TX_STS_FIFO_EMPTY _UART_TX_STS_FIFO_EMPTY
            #endif
        #endif

        #if !( defined(_UART_TXDATA_AUX_CTL_REG) )
            #error-Please define `_UART_TXDATA_AUX_CTL_REG` as an alias to `<NAME>_TXDATA_AUX_CTL_REG` in 'LPUtilConfig.h'
        #else
            #if !( defined(UART_TXDATA_AUX_CTL_REG) )
                #define UART_TXDATA_AUX_CTL_REG _UART_TXDATA_AUX_CTL_REG
            #endif
        #endif

        #if !( defined(_UART_TX_FIFO_CLR) )
            #error-Please define `_UART_TX_FIFO_CLR` as an alias to `<NAME>_TX_FIFO_CLR` in 'LPUtilConfig.h'
        #else
            #if !( defined(UART_TX_FIFO_CLR) )
                #define UART_TX_FIFO_CLR _UART_TX_FIFO_CLR
            #endif
        #endif

        #define UART_TxPending()    ( (_UART_TXSTATUS_REG & _UART_TX_STS_FIFO_EMPTY) != 0u )

        #define UART_ClearTx() ({                                    \
            uint8 int_state = CyEnterCriticalSection();              \
                                                                     \
            /* Clear the HW FIFO */                                  \
            _UART_TXDATA_AUX_CTL_REG |= (uint8)  _UART_TX_FIFO_CLR;  \
            _UART_TXDATA_AUX_CTL_REG &= (uint8) ~_UART_TX_FIFO_CLR;  \
                                                                     \
            CyExitCriticalSection(int_state);                        \
        })
    #endif

    /* Prints text to UART. This is to be used to avoid competing FreeRTOS tasks from
     *   accessing the same UART resource. */
    #if !( defined(SHARED_UART_PutString) )
        #if !( defined(_UART_PutString) )
            #error Please define `_UART_PutString` as an alias to `<NAME>_PutString` in 'LPUtilConfig.h'
         #else
            #if !( defined(UART_PutString) )
                #define UART_PutString _UART_PutString
            #endif
        #endif

        #if (CONFIG_INCLUDE_FREERTOS == 1u)
            void init_shared_uart(void);
            void SHARED_UART_PutString(const char *buffer);
        #else
            #define init_shared_uart()
            #define SHARED_UART_PutString(buffer)  UART_PutString(buffer);
        #endif
    #endif

    void vPrintf(const char *format, ... );
    void vLoggingPrintf(uint8 debug_level, uint8 log_type[2], const char *format, ... );
    void vLoggingPrintfSave(const char *format, ... );
    void vLoggingPrintfCritical(const char *format, ... );
#else
    #define init_shared_uart()
    #define SHARED_UART_PutString(buffer)

    #define vPrintf(x, ...)
    #define vLoggingPrintf(dl, lt, x, ...)
    #define vLoggingPrintfSave(x, ...);
    #define vLoggingPrintfCritical(x, ...)
#endif


#include <CyLib.h>
#if defined(CYASSERT)
    #undef CYASSERT
#endif


void LOGGING_inc_count_assert(void);
uint32 LOGGING_get_count_assert(void);


#define ASSERT(x)  \
if ( ! (x) )       \
{                  \
    vLoggingPrintfCritical( "psoc: assert triggered @ %s:%u\r\n", __FILENAME__, __LINE__ ); \
    LOGGING_inc_count_assert(); \
}

#define ASSERT_RET(x, ret)  \
{                           \
    if ( !( x ) )           \
    {                       \
        vLoggingPrintfCritical("psoc: assert triggered @ %s:%u\r\n", __FILENAME__, __LINE__);  \
        LOGGING_inc_count_assert(); \
        return ret;         \
    }                       \
}


#define ASSERT_DO(x, op) \
{                        \
    if ( !( x ) )        \
    {                    \
        vLoggingPrintfCritical("psoc: assert triggered @ %s:%u\r\n", __FILENAME__, __LINE__);  \
        LOGGING_inc_count_assert(); \
        op;              \
    }                    \
}


#define CYASSERT_RET  ASSERT_RET
#define CYASSERT      ASSERT


#define PRINTF_BIN_FMT  "%c%c%c%c%c%c%c%c"
#define PRINTF_BIN(byte)  \
  (byte & 0x80 ? '1' : '0'),  \
  (byte & 0x40 ? '1' : '0'),  \
  (byte & 0x20 ? '1' : '0'),  \
  (byte & 0x10 ? '1' : '0'),  \
  (byte & 0x08 ? '1' : '0'),  \
  (byte & 0x04 ? '1' : '0'),  \
  (byte & 0x02 ? '1' : '0'),  \
  (byte & 0x01 ? '1' : '0')

#define DEBUG_NONE    7u  // No logging
#define DEBUG_CRIT    6u  // Critical events only, like asserts or exceptions; Gets saved to EEPROM if `CONFIG_USE_CRIT_MSG` is enabled
#define DEBUG_ERROR   5u  // Module specific errors
#define DEBUG_WARN    4u  // Module specific warnings
#define DEBUG_INFO    3u  // Module specific status updates or state changes; Manual triggers
#define DEBUG_DEBUG   2u  // Temporary and extremely verbose printouts
#define DEBUG_TRACE   1u  // Verbose logs/transcription of things; Automatic triggers
#define DEBUG_ALL     0u


#endif /* LOGGING_H */
/* [] END OF FILE */
