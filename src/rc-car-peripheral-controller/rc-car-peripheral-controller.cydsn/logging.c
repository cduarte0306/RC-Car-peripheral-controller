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
#include "logging.h"
#include "app_utils.h"

#include <stdio.h>
#include <stdarg.h>

#if defined ( FREERTOS_INCLUDE_OK )
    #include "FreeRTOS.h"
    #include "semphr.h" 
#endif


static uint32 count_assert = 0;


#if defined ( UART_INCLUDE_OK )
    #define MAX_PRINTF_LEN    255
    static char buffer[MAX_PRINTF_LEN];
#else
    #if (CONFIG_USE_CRIT_MSG == 1u)
        #define MAX_PRINTF_LEN    UNIT_INFO_CRIT_MSG_LEN
        static char buffer[MAX_PRINTF_LEN];
    #endif
#endif


#if defined ( UART_INCLUDE_OK )
    #if defined ( FREERTOS_INCLUDE_OK )
        #include "semphr.h"    
        SemaphoreHandle_t uart_mutex = NULL;
        
        /**
        * @brief Initializes the UART mutex
        * 
        * If this is not called then task suspension will be used.
        * That will pause all tasks instead of the specific ones
        * that are waiting on the UART resource
        */
        void init_shared_uart(void)
        {
            uart_mutex = xSemaphoreCreateMutex();   
        }
        
        
        /**
        * @brief Prints raw strings to UART in a multi-tasking safe manner
        * 
        * If `uart_mutex` is not initialized via `init_shared_uart`, it will
        * suspend ALL tasks to claim the UART resource.
        *
        * @param[in] buffer String to print
        */
        void SHARED_UART_PutString(const char *buffer)
        {
            uint32 time_start = xGetTimestamp();
 
            while ( UART_TxPending() )
            {
                if ( xGetElapsed(time_start) > 100u )
                {
                    UART_ClearTx();
                    break;
                }
            }
            
            if ( uart_mutex == NULL )       
            {       
                vTaskSuspendAll(); {
                    UART_PutString(buffer);
                } xTaskResumeAll();       
            }                             
            else                          
            {
                xSemaphoreTake(uart_mutex, (TickType_t) portMAX_DELAY); {
                    UART_PutString(buffer);
                } xSemaphoreGive(uart_mutex);
            }
        }
    #endif  /* FREERTOS_INCLUDE_OK */

    /**
     * @brief Prints text to UART. NOT meant to be used in critical sections.
     * 
     * Unlike vLogggingPrintf, this doesn't print timestamp info
     *
     * @param[in] format String to print, along with encoded formatting
     * @param[in] ... Arbitrary series of values to be printed as indicated in the string formatting
     */
    void vPrintf(const char *format, ... )
    {
        va_list args;
        uint8 i = 0;
        
        /* Avoid accessing NULL data */
        if (strlen(format) == 0)
        {
            return;
        }
        
        memset(buffer, 0, sizeof(buffer));
        
        va_start(args, format);
        vsnprintf(&buffer[i], sizeof(buffer), format, args);
        va_end(args);

        SHARED_UART_PutString(buffer);
    }
    
    
    /**
     * @brief Prints text to UART. NOT meant to be used in critical sections.
     *
     * @param[in] format String to print, along with encoded formatting
     * @param[in] ... Arbitrary series of values to be printed as indicated in the string formatting
     */
    void vLoggingPrintf(uint8 debug_level, uint8 log_type[2], const char *format, ... )
    {
        va_list args;
        uint8 i = 0;
                    
        /* Global Debug level check */
        if (debug_level < GLOBAL_DEBUG_LEVEL)
        {
            return;   
        }
        
        /* Log type check */
        if (log_type[0] == 0u)
        {
            return;   
        }
        
        if (debug_level < log_type[1])
        {
            return;   
        }
        
        /* Avoid accessing NULL data */
        if (strlen(format) == 0)
        {
            return;
        }

        /* Check if timestamping is possible */ 
        #if !defined( TIMESTAMP_INCLUDE_OK )
            #warning `xGetTimestamp` is unavailable! `LoggingPringf` will not display valid timestamps.
        #endif
        
        memset(buffer, 0, sizeof(buffer));
        
        /* If the text is indented or has blank space, don't write timestamp / task info at the line */
        if (isspace((uint8) format[0]) == 0)
        {
            /* Append timestamp */
            i += snprintf(&buffer[i], sizeof(buffer) - i, "[ %lu ] ", xGetTimestamp());
            
            /* Append task name name */
            #if defined( FREERTOS_INCLUDE_OK )
                #if (configUSE_TRACE_FACILITY == 1u)
                    TaskHandle_t task_handle = xTaskGetCurrentTaskHandle();
                    TaskStatus_t task_status;
                    
                    if (task_handle != NULL)
                    {   
                        vTaskGetInfo(task_handle, &task_status, pdFALSE, eRunning);
                        i += snprintf(&buffer[i], sizeof(buffer) - i, "<%s> ", task_status.pcTaskName);
                    }
                #else
                    #warning `vTaskGetInfo` is unavailable because FreeRTOS option `configUSE_TRACE_FACILITY` is disabled. `LoggingPringf` will not display thread info.
                #endif
            #endif
        }

        va_start(args, format);
        vsnprintf(&buffer[i], sizeof(buffer), format, args);
        va_end(args);
        
        SHARED_UART_PutString(buffer);
    }


    /**
     * @brief Prints text to UART. Meant to be used in critical sections.
     *
     * @param[in] format String to print, along with encoded formatting
     * @param[in] ... Arbitrary series of values to be printed as indicated in the string formatting
     */
    void vLoggingPrintfCritical(const char *format, ... )
    {
        va_list args;

        memset(buffer, 0, sizeof(buffer));
        
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        
#if defined ( FREERTOS_INCLUDE_OK )
        vTaskSuspendAll(); {     
#endif
            UART_ClearTx();
            UART_PutString(buffer);
            while (UART_TxPending());
            
            /* Give time for the UART to print everything */
            CyDelay(1);
#if defined ( FREERTOS_INCLUDE_OK )
        } xTaskResumeAll();    
#endif
    }
    
    
    /**
     * @brief Prints text to UART and also saves the output to EEPROM
     *
     * @param[in] format String to print, along with encoded formatting
     * @param[in] ... Arbitrary series of values to be printed as indicated in the string formatting
     */
    void vLoggingPrintfSave(const char *format, ... )
    {
        va_list args;

        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        
        (void) UNIT_INFO_write_crit_msg(buffer, sizeof(buffer));
        
        UART_PutString(buffer);
        while (UART_TxPending());
    }
#endif  /* ifdef UART_INCLUDE_OK */


/**
 * @brief Increaments assertion count
 */
void LOGGING_inc_count_assert(void)
{
    count_assert++;    
}


/**
 * @brief Gets assertion count
 *
 * @return uint8 Assertion count
 */
uint32 LOGGING_get_count_assert(void)
{
    return count_assert;
}


/* [] END OF FILE */
