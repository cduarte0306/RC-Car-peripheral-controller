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
#include "app_cli.h"
#include "spi_controller.h"

#include "FreeRTOS.h"
#include "task.h"


/* Function prototype for a simple task */
void vLEDMonitorTask(void *pvParameters);
void vCommsTask( void* pvParameters );
void vRCTask( void* pvParameters );
void vSpeedMeasureTask( void* pvParameters );
void vCliTask( void* pvParameters );

xTaskHandle rc_car_handle   = NULL;
xTaskHandle cli_handle      = NULL;
xTaskHandle spi_coms_handle = NULL;
xTaskHandle led_handle      = NULL;
xTaskHandle speed_rd_handle = NULL;


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
        &led_handle                /* Task handle */
    );
    if (ret != RET_PASS)
    {
        vLoggingPrintfCritical("main | err: init led-monitor fail\r\n");
        CYASSERT(FALSE);
        for (;;);
    }
    
    vLoggingPrintf(DEBUG_ERROR, LOG_RC_CAR,"main | init led-monitor\r\n");
    
    ret = xTaskCreate(
        vCommsTask,                /* Task function */
        "spi-comms",               /* Task name (for debugging) */
        configMINIMAL_STACK_SIZE,  /* Stack size */
        NULL,                      /* Task input parameter */
        1,                         /* Priority */
        &spi_coms_handle           /* Task handle */
    );
    if (ret != RET_PASS)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_RC_CAR, "main | err: init spi-comms fail\r\n");
        CYASSERT(FALSE);
        for (;;);
    }
    
    vLoggingPrintf(DEBUG_ERROR, LOG_RC_CAR,"main | init spi-comms\r\n");
    
    ret = xTaskCreate(
        vRCTask,                    /* Task function */
        "rc-task",                 /* Task name (for debugging) */
        configMINIMAL_STACK_SIZE,  /* Stack size */
        NULL,                      /* Task input parameter */
        1,                         /* Priority */
        &rc_car_handle             /* Task handle */
    );
    if (ret != RET_PASS)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_RC_CAR, "main | err: init rc-task fail\r\n");
        CYASSERT(FALSE);
        for (;;);
    }
    
    vLoggingPrintf(DEBUG_INFO, LOG_RC_CAR,"main | init rc-task\r\n");
    
    ret = xTaskCreate(
        vSpeedMeasureTask,                   /* Task function */
        "speed-read",                 /* Task name (for debugging) */
        configMINIMAL_STACK_SIZE,  /* Stack size */
        NULL,                      /* Task input parameter */
        1,                         /* Priority */
        &speed_rd_handle           /* Task handle */
    );
    if (ret != RET_PASS)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_RC_CAR, "main | err: init speed-read fail\r\n");
        CYASSERT(FALSE);
        for (;;);
    }
    
    vLoggingPrintf(DEBUG_INFO, LOG_RC_CAR,"main | init speed-read\r\n");
    
    ret = xTaskCreate(
        vCliTask,                  /* Task function */
        "app-cli",                 /* Task name (for debugging) */
        configMINIMAL_STACK_SIZE,  /* Stack size */
        NULL,                      /* Task input parameter */
        1,                         /* Priority */
        &cli_handle                /* Task handle */
    );
    if (ret != RET_PASS)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_RC_CAR, "main | err: app-cli\r\n");
        CYASSERT(FALSE);
        for (;;);
    }
    
    vLoggingPrintf(DEBUG_INFO, LOG_RC_CAR, "main | init app-cli\\r\n");

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
    uint8_t connectionStatus = pdFALSE;
    for(;;)
    {
//        connectionStatus = SPIGetConnectionStatus();  // Read the SPI connection status
//        uint8 staticBits = (LED_DR & (uint8)(~LED_MASK));
//        LED_DR = staticBits | ((uint8)(ledState << LED_SHIFT) & LED_MASK);
//
//        if ( connectionStatus)
//        {
//            ledState = !ledState;
//        }
//        else
//        {
//            ledState = 0;
//        }
        
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
    uint8_t ret;
    
    ret = SPI_controller_start();
    if ( ret != pdPASS )
    {
        vLoggingPrintf(DEBUG_INFO, LOG_RC_CAR, "app: init | err: Could initialize SPI controller\r\n");
    }

    ret = RCInit();
    if ( ret != pdPASS )
    {
        vLoggingPrintf(DEBUG_INFO, LOG_RC_CAR, "app: init | err: Could initialize RC car module\r\n");
    }
    
    for(;;)
    {
        RcProcess();
        vTaskDelay( 1 );
    }
}


void vSpeedMeasureTask( void* pvParameters )
{
    ( void ) pvParameters;
    
    for(;;)
    {
        RcReadSpeedThread();
    }
}


void vCliTask( void* pvParameters )
{
    ( void ) pvParameters;
    
    BaseType_t ret = APP_CLI_init();
    if (!ret)
    {
        vLoggingPrintf(DEBUG_INFO, LOG_RC_CAR, "app: APP_CLI_init | err: Could initialize app CLI\r\n");
        vTaskDelete(cli_handle);
    }

    for(;;)
    {
        APP_CLI_update();
        vTaskDelay(1);
    }
}


/* [] END OF FILE */
