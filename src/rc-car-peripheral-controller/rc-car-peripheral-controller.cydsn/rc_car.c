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

#include "rc_car.h"
#include "RCUtils.h"
#include "spi_controller.h"

#include <project.h>


#define RD_SPEED_DATA   (speed_msb_Status << 8U) | ( speed_lsb_Status )


static regMapType regMap[ REG_END ];
static uint32_t   speed_count;


static void readTelemetry(void);


void RCInit(void)
{
    SPI_controller_start();
    start_Control = pdTRUE;
}


void RCprocessTelemetry(void)
{
    readTelemetry();
}


void RCreadSpeedThread(void)
{
    speed_count = RD_SPEED_DATA;
    
    start_Control = pdFALSE;
    vTaskDelay(100);  // Delay 100ms (10Hz sampling rate)
    start_Control = pdTRUE;
}


regMapType* getRegRef(void)
{
    return regMap;
}


static void readTelemetry(void)
{
    regMap[REG_SPEED].data.u32 = speed_count;
}


/* [] END OF FILE */
