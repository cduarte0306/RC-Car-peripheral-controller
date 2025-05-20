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

#include "project.h"

#include "RCUtils.h"
#include "rc_car.h"

#include "FreeRTOS.h"
#include "task.h"


/* Function prototype for a simple task */
void vLEDMonitorTask(void *pvParameters);
void vCommsTask( void* pvParameters );
void vRCTask( void* pvParameters );
void vSpeedMeasureTask( void* pvParameters );


int main(void) 
{
    BaseType_t ret;

    /* Port layer functions that need to be copied into the vector table. */
    extern void xPortPendSVHandler(void);
    extern void xPortSysTickHandler(void);
    extern void vPortSVCHandler(void);
    extern cyisraddress CyRamVectors[];

    /* Install the OS Interrupt Handlers. */
    CyRamVectors[11] = (cyisraddress) vPortSVCHandler;
    CyRamVectors[14] = (cyisraddress) xPortPendSVHandler;
    CyRamVectors[15] = (cyisraddress) xPortSysTickHandler;
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    UART_Debug_Start();
    
    vLoggingPrintf(DEBUG_INFO, LOG_PSOC, " ===============================\r\n\r\n");
    
    /* Create a simple task */
    ret = xTaskCreate(
        vLEDMonitorTask,               /* Task function */
        "led-monitor",                   /* Task name (for debugging) */
        configMINIMAL_STACK_SIZE,  /* Stack size */
        NULL,                      /* Task input parameter */
        1,                         /* Priority */
        NULL                       /* Task handle */
    );
    if (ret != RET_PASS)
    {
        vLoggingPrintfCritical("main | err: init led-monitor fail\r\n");
        CYASSERT(FALSE);
        for (;;);
    }
    
    ret = xTaskCreate(
        vCommsTask,                /* Task function */
        "spi-comms",               /* Task name (for debugging) */
        configMINIMAL_STACK_SIZE,  /* Stack size */
        NULL,                      /* Task input parameter */
        1,                         /* Priority */
        NULL                       /* Task handle */
    );
    if (ret != RET_PASS)
    {
        vLoggingPrintfCritical("main | err: init spi-comms fail\r\n");
        CYASSERT(FALSE);
        for (;;);
    }
    
    ret = xTaskCreate(
        vRCTask,                    /* Task function */
        "rc-task",                 /* Task name (for debugging) */
        configMINIMAL_STACK_SIZE,  /* Stack size */
        NULL,                      /* Task input parameter */
        1,                         /* Priority */
        NULL                       /* Task handle */
    );
    if (ret != RET_PASS)
    {
        vLoggingPrintfCritical("main | err: init rc-task fail\r\n");
        CYASSERT(FALSE);
        for (;;);
    }
    
    ret = xTaskCreate(
        vSpeedMeasureTask,                   /* Task function */
        "speed-read",                 /* Task name (for debugging) */
        configMINIMAL_STACK_SIZE,  /* Stack size */
        NULL,                      /* Task input parameter */
        1,                         /* Priority */
        NULL                       /* Task handle */
    );
    if (ret != RET_PASS)
    {
        vLoggingPrintfCritical("main | err: init speed-read fail\r\n");
        CYASSERT(FALSE);
        for (;;);
    }

    /* Start the scheduler */
    vTaskStartScheduler();

    /* If all is well, we will never reach here as the scheduler will now be running. */
    for(;;)
    {
        /* Infinite loop */
    }
}


/* Simple task to blink an LED */
void vLEDMonitorTask(void *pvParameters)
{
    (void) pvParameters;
    uint8 ledState = pdFALSE;

    for(;;)
    {
        uint8 staticBits = (LED_DR & (uint8)(~LED_MASK));
        LED_DR = staticBits | ((uint8)(ledState << LED_SHIFT) & LED_MASK);
        ledState = !ledState;
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay 500ms
    }
}


void vCommsTask( void* pvParameters )
{
    ( void ) pvParameters;
    
    for(;;)
    {        
        vTaskDelay( 1 );
    }
}


void vRCTask( void* pvParameters )
{
    ( void ) pvParameters;
    BaseType_t ret = RCInit();
    if ( ret != pdPASS )
    {
        vLoggingPrintf(DEBUG_INFO, LOG_RC_CAR, "app: init | err: Could initialize RC car module\r\n");
    }
    
    for(;;)
    {
        RCprocessTelemetry();
        vTaskDelay( 100 );
    }
}


void vSpeedMeasureTask( void* pvParameters )
{
    ( void ) pvParameters;
    
    for(;;)
    {
        RCreadSpeedThread();
    }
}


/* [] END OF FILE */
