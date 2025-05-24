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
#include "motor_driver.h"

#include <project.h>


#define RD_SPEED_DATA   (speed_msb_Status << 8U) | ( speed_lsb_Status )


static regMapType regMap[ REG_WR_END ];
static uint32_t   speed_count;


static void readTelemetry(void);


/**
 * @brief Initializes the RC car components
 * 
 * @return uint8_t RET_PASS on success, RET_FAIL on failure
 */
uint8_t RCInit(void)
{
    uint8 ret;
    vLoggingPrintf(DEBUG_INFO, LOG_RC_CAR, "app: init | Initializing RC car\r\n");
    
    ret = SPI_controller_start();
    if (!ret)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_SPI, "app: RCInit | err: Could not configure SPI\r\n");
    }
    
    MotorCtrlInit();
    
    for (uint8 idx = REG_NOOP; idx < REG_RO_END; idx++)
    {
        regMap[idx].regType = READ_ONLY;
    }
    
    for (uint8 idx = REG_RO_END; idx < REG_WR_END; idx++)
    {
        regMap[idx].regType = READ_WRITE;
    }
    
    start_Control = pdTRUE;
    vLoggingPrintf(DEBUG_INFO, LOG_RC_CAR, "app: init | RC Car initialized\r\n");
    return RET_PASS;
}


/**
 * @brief Processes telemetry data from the RC car
 * 
 */
void RcProcess(void)
{
    readTelemetry();
    
    // Process the values in the registers
    MotorCtrlsetSpeedSetPoint(regMap[REG_SPEED_SETPOINT].data.u32);
    MotorCtrlSetState(regMap[REG_SET_MOTOR_STATUS].data.u32);
    
    MotrorCtrlProcess(regMap[REG_SPEED].data.u32);
}


/**
 * @brief Reads the speed data from the RC car
 * 
 */
void RcReadSpeedThread(void)
{
    speed_count = RD_SPEED_DATA;
    
    start_Control = pdFALSE;
    vTaskDelay(100);  // Delay 100ms (10Hz sampling rate)
    start_Control = pdTRUE;
}


/**
 * @brief Gets the register map reference
 * 
 * @return regMapType* Pointer to the register map
 */
regMapType* getRegRef(void)
{
    return regMap;
}


/**
 * @brief Reads telemetry data from the RC car
 * 
 */
static void readTelemetry(void)
{
    regMap[REG_SPEED].data.u32 = speed_count;
}


/* [] END OF FILE */
