/*
 * FreeRTOS V202012.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *----------------------------------------------------------*/

#include <project.h>
    
    
#define configASSERT(x)                   CYASSERT(x)

/* Clock related settings */
#define configCPU_CLOCK_HZ			      ( ( unsigned long ) BCLK__BUS_CLK__HZ )
#define configTICK_RATE_HZ			      ( ( TickType_t ) 1000 )
    
#ifndef pdMS_TO_TICKS
    #define pdMS_TO_TICKS(xTimeInMs)      ( (TickType_t) (((xTimeInMs)*((TickType_t) configTICK_RATE_HZ))/((TickType_t) 1000U)) )
#endif
    
#ifndef pdTICKS_TO_MS
    #define pdTICKS_TO_MS(xTicks)         ( (uint32_t) ((xTicks)*1000)/configTICK_RATE_HZ )
#endif

    
/* Debug related settings */
#if defined(DEBUG) && !defined(NDEBUG)
    #define configUSE_MALLOC_FAILED_HOOK      1
    #define configCHECK_FOR_STACK_OVERFLOW    2
#endif
    
/* Stack related settings */
#define configSTACK_DEPTH_TYPE                    uint16_t
#define configMINIMAL_STACK_SIZE	              512u
#define configUSE_PORT_OPTIMISED_TASK_SELECTION   1

/* Task related settings */
#define configMAX_TASK_NAME_LEN		      12
#define configQUEUE_REGISTRY_SIZE		  10
#define configMAX_PRIORITIES		      4
    
#define configUSE_PREEMPTION		      1
#define configUSE_16_BIT_TICKS		      0
#define configIDLE_SHOULD_YIELD		      0
#define configUSE_CO_ROUTINES 		      0
#define configUSE_MUTEXES		          1
#define configUSE_RECURSIVE_MUTEXES		  0
#define configUSE_COUNTING_SEMAPHORES 	  0
#define configUSE_ALTERNATIVE_API 	      0
#define configUSE_TASK_NOTIFICATIONS      1

/* Memory allocation related definitions. */
#define configUSE_NEWLIB_REENTRANT              0
#define configSUPPORT_STATIC_ALLOCATION         0
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configAPPLICATION_ALLOCATED_HEAP        0
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( CYDEV_HEAP_SIZE ) )
    
/* Hook function related definitions. */    
#define configUSE_IDLE_HOOK			        0
#define configUSE_TICK_HOOK			        0
#define configUSE_DAEMON_TASK_STARTUP_HOOK  0
    
/* Co-routine related definitions. */
#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         1

/* Software timer related definitions. */
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               3
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            configMINIMAL_STACK_SIZE
    
/* Run time and task stats gathering related definitions. */
#define configGENERATE_RUN_TIME_STATS           0
#define configUSE_TRACE_FACILITY                1  /* enable `vTaskGetInfo` for `vLoggingPrintf` task info */
#define configUSE_STATS_FORMATTING_FUNCTIONS    0
    
/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet			0
#define INCLUDE_uxTaskPriorityGet			0
#define INCLUDE_vTaskDelete					1
#define INCLUDE_vTaskSuspend				1
#define INCLUDE_xResumeFromISR              0
#define INCLUDE_vTaskDelayUntil				0
#define INCLUDE_vTaskDelay					1
#define INCLUDE_xTaskGetSchedulerState      0
#define INCLUDE_xTaskGetCurrentTaskHandle   1
#define INCLUDE_uxTaskGetStackHighWaterMark 1
#define INCLUDE_xTaskGetIdleTaskHandle      0
#define INCLUDE_eTaskGetState               0
#define INCLUDE_xEventGroupSetBitFromISR    0
#define INCLUDE_xTimerPendFunctionCall      0
#define INCLUDE_xTaskAbortDelay             0
#define INCLUDE_xTaskGetHandle              1
#define INCLUDE_xTaskResumeFromISR          0

/* FreeRTOS+CLI defs */
#define configCOMMAND_INT_MAX_OUTPUT_SIZE   1u

/**
 * Configure the number of priority bits. This is normally
 * __NVIC_PRIO_BITS but PSoC Creator beta 5 contained a larger
 * value for the priority than is implemented in the hardware so
 * set it here to what the data sheet describes.
 */
#define configPRIO_BITS       3        /* 8 priority levels */

/* The lowest priority. */
#define configKERNEL_INTERRUPT_PRIORITY 	( 7 << (8 - configPRIO_BITS) )

/* Priority 5, or 160 as only the top three bits are implemented. */
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( 5 << (8 - configPRIO_BITS) )


//#define traceTASK_CREATE(pxNewTCB)                 vLoggingPrintfCritical("[ %lu ] sys: %s:%u | task <%s> created | free mem: %li\r\n", xGetTimestamp(), __FILENAME__, __LINE__, pxNewTCB->pcTaskName, xPortGetFreeHeapSize())
//#define traceTASK_DELETE(pxTCB)                    vLoggingPrintfCritical("[ %lu ] sys: %s:%u | task <%s> deleted | free mem: %li\r\n", xGetTimestamp(), __FILENAME__, __LINE__, pxTCB->pcTaskName, xPortGetFreeHeapSize())
//#define traceTASK_NOTIFY_TAKE_BLOCK(uxIndexToWait) vLoggingPrintfCritical("[ %lu ] sys: %s:%u | NOTIFY_TAKE %u\r\n", xGetTimestamp(), __FILENAME__, __LINE__, uxIndexToWait)
//#define traceTASK_CREATE(pxNewTCB);                vLoggingPrintfCritical("[ %lu ] sys: %s:%u | task <%s> created\r\n", xGetTimestamp(), __FILENAME__, __LINE__, pxNewTCB->pcTaskName)
//#define traceTASK_SWITCHED_IN()                    vLoggingPrintfCritical("[ %lu ] sys: %s:%u | task <%s> enter\r\n", xGetTimestamp(), __FILENAME__, __LINE__, pxCurrentTCB->pcTaskName)
//#define traceTASK_SWITCHED_OUT()                   vLoggingPrintfCritical("[ %lu ] sys: %s:%u | task <%s> exit\r\n", xGetTimestamp(), __FILENAME__, __LINE__, pxCurrentTCB->pcTaskName)

//#define traceQUEUE_SEND( pxQueue )                 vLoggingPrintfCritical("[ %lu ] sys: queue send 0x%08X\r\n", xGetTimestamp(), pxQueue)
//#define traceQUEUE_SEND_FAILED( pxQueue )          vLoggingPrintfCritical("[ %lu ] sys: queue send fail 0x%08X\r\n", xGetTimestamp(), pxQueue)
//#define traceBLOCKING_ON_QUEUE_SEND( pxQueue )     vLoggingPrintfCritical("[ %lu ] sys: queue send block 0x%08X\r\n", xGetTimestamp(), pxQueue)

//#define traceQUEUE_RECEIVE( pxQueue )              vLoggingPrintfCritical("[ %lu ] sys: queue receive 0x%08X\r\n", xGetTimestamp(), pxQueue)
//#define traceQUEUE_RECEIVE_FAILED( pxQueue )       vLoggingPrintfCritical("[ %lu ] sys: queue receive fail 0x%08X\r\n", xGetTimestamp(), pxQueue)
//#define traceBLOCKING_ON_QUEUE_RECEIVE( pxQueue )  vLoggingPrintfCritical("[ %lu ] sys: queue receive block 0x%08X\r\n", xGetTimestamp(), pxQueue)



#endif /* FREERTOS_CONFIG_H */
