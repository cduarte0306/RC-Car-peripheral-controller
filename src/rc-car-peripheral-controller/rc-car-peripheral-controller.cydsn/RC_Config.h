#ifndef LP_UTIL_CONFIG_H
#define LP_UTIL_CONFIG_H

    /**
     * Debug setting for the project. Available settings are:
     *   DEBUG_NONE  - No debugging
     *   DEBUG_CRIT  - Critical events only, like asserts or exceptions
     *   DEBUG_ERROR - Module specific errors
     *   DEBUG_WARN  - Module specific warnings
     *   DEBUG_INFO  - Module specific status updates or state changes; Manual triggers
     *   DEBUG_DEBUG - Temporary and extremely verbose printouts
     *   DEBUG_TRACE - Verbose logs/transcription of things; Automatic triggers
     */
    #if defined(DEBUG)
        #define GLOBAL_DEBUG_LEVEL             DEBUG_DEBUG
    #else
        #define GLOBAL_DEBUG_LEVEL             DEBUG_ERROR
    #endif


    /* ============================================= */
    /*         Define log type enables here          */
    #define LOG_PSOC                       (uint8[2]) {1u, DEBUG_INFO}
    #define LOG_RC_CAR                     (uint8[2]) {1u, DEBUG_INFO}
    #define LOG_SPI_CAR                    (uint8[2]) {1u, DEBUG_INFO}
    #define LOG_CLI                        (uint8[2]) {1u, DEBUG_INFO}
    #define LOG_EEPROM                     (uint8[2]) {1u, DEBUG_INFO}
    #define LOG_APP                        (uint8[2]) {1u, DEBUG_INFO}
    #define LOG_APP_EEPROM                 (uint8[2]) {1u, DEBUG_INFO}
    #define LOG_UNIT_INFO                  (uint8[2]) {1u, DEBUG_WARN}
    #define LOG_MAIN                       (uint8[2]) {1u, DEBUG_INFO}
    #define LOG_ALARM                      (uint8[2]) {1u, DEBUG_WARN}
    #define LOG_ADC                        (uint8[2]) {1u, DEBUG_INFO}
    #define LOG_IO_CTRL                    (uint8[2]) {1u, DEBUG_INFO}
    /* ============================================= */

    
    /* CONFIG_INCLUDE_FREERTOS = 1: Make use of the FreeRTOS library */
    #define CONFIG_INCLUDE_FREERTOS        (1u)

    /* CONFIG_INCLUDE_LWIP = 1: Make use of the lwip library. */
    #define CONFIG_INCLUDE_LWIP            (0u)

    /* CONFIG_INCLUDE_KSZ8851SNL = 1: Make use of the KSZ8851SNL library. Defaults to
     * the lwip implementation if available, otherwise uses the FreeRTOS implementation.
     */
    #define CONFIG_INCLUDE_KSZ8851SNL      (0u)

    /* CONFIG_INCLUDE_UART = 1: Make use of the PSoC UART component to print data to. */
    #define CONFIG_INCLUDE_UART            (1u)

    /* CONFIG_INCLUDE_TIMESTAMP = 1: Make use of the PSoC counter component for printing timestamp data */
    #define CONFIG_INCLUDE_TIMESTAMP       (1u)

    /* CONFIG_INCLUDE_RAND = 1: Make use of the PSoC PRS component for random number generation */
    #define CONFIG_INCLUDE_RAND            (0u)

    /* CONFIG_INCLUDE_CRC = 1: Compile CRC calc related functions */
    #define CONFIG_INCLUDE_CRC             (1u)
        
    /* CONFIG_USE_UNIT_INFO = 1: Enable EEPROM use for tracking for boot count, 
     * watchdog reset count, saving serial number, toring critical log messages
     * prior to a psoc reset. 
     *
     * Resources:
     * - FLASH:   7008 bytes
     * - EEPROM: `CONFIG_NUM_CRIT_MSGS`*4 + 4 rows
     * 
     * \note: Set `CONFIG_NUM_CRIT_MSGS` to 0u to disable crit_msgs. */
    #define CONFIG_USE_UNIT_INFO            ( 1u)
    #define CONFIG_NUM_CRIT_MSGS            ( 0u)
    
    /* UART CLI bindings */
    #if CONFIG_INCLUDE_UART == 1u
    #if __has_include("UART_Debug.h")
        #include "UART_Debug.h"
    #endif
        #define CY_UART_UART_H
        #define UART_INCLUDE_OK
        #define UART_H                   "UART_Debug.h"
        #define _UART_TXSTATUS_REG       UART_Debug_TXSTATUS_REG
        #define _UART_TX_STS_FIFO_EMPTY  UART_Debug_TX_STS_FIFO_EMPTY
        #define _UART_PutString          UART_Debug_PutString
        #define _UART_TXDATA_AUX_CTL_REG UART_Debug_TXDATA_AUX_CTL_REG
        #define _UART_TX_FIFO_CLR        UART_Debug_TX_FIFO_CLR
    #endif
    
    #if CONFIG_INCLUDE_FREERTOS == 1u
        #define FREERTOS_INCLUDE_OK
    #endif
    
    #if ( CONFIG_INCLUDE_TIMESTAMP == 1 )
        #if ( __has_include("TREG0.h") && __has_include("TREG1.h") && __has_include("TREG2.h") && __has_include("TREG3.h") )
            #define TIMESTAMP_INCLUDE_OK
        #else
            #error CONFIG_INCLUDE_TIMESTAMP enabled but xGetTimestamp() is unavailable because CY_STATUS_REG_TREG[0:3]_H is not defined.
    #endif
#endif

#endif  /* LP_UTIL_CONFIG_H */
