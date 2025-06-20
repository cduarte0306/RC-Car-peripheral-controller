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
#include "vers.h"

#include <project.h>


#define RD_SPEED_DATA   (speed_msb_Status << 8U) | ( speed_lsb_Status )
#define SEL_MAX         (3U)


static uint32_t lastSpeed = 0;
static regMapType regMap[ REG_WR_END ];
static uint32_t   speed_count;

static volatile uint32_t rightDistance = 0.0;
static volatile uint32_t leftDistance  = 0.0;
static volatile uint32_t frontDistance = 0.0;

static volatile uint8_t sensorSel = 0;

static void readTelemetry(void);


CY_ISR(ultrasonic_handler_left)
{
    leftDistance = (CY_GET_REG32(Timer_echo_left_PERIOD_LSB_PTR)) - (CY_GET_REG32(Timer_echo_left_CAPTURE_LSB_PTR));
    Timer_echo_left_ReadStatusRegister();
}


CY_ISR(ultrasonic_handler_right)
{
    rightDistance = (CY_GET_REG32(Timer_echo_right_PERIOD_LSB_PTR)) - (CY_GET_REG32(Timer_echo_right_CAPTURE_LSB_PTR));
    Timer_echo_right_ReadStatusRegister();
}


CY_ISR(ultrasonic_handler_front)
{
    frontDistance = (CY_GET_REG32(Timer_echo_front_PERIOD_LSB_PTR)) - (CY_GET_REG32(Timer_echo_front_CAPTURE_LSB_PTR));
    Timer_echo_front_ReadStatusRegister();
}


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
    
    PWM_trig_Start();
    
    Timer_echo_left_Start();
    Timer_echo_right_Start();
    Timer_echo_front_Start();
    
    // Set the version in the registers
    getVers(&regMap[REG_VER_MAJOR].data.u8, &regMap[REG_VER_MINOR].data.u8, &regMap[REG_VER_BUILD].data.u8);
    
    regMap[REG_NOOP].data.u32 = 0;
    
    encoder_counter_Start();
    
    isr_left_echo_StartEx(ultrasonic_handler_left);
    isr_right_echo_StartEx(ultrasonic_handler_right);
    isr_front_echo_StartEx(ultrasonic_handler_front);
    
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
    MotorCtrlSetOnOffState(regMap[REG_MOTOR_ONOFF_STATE].data.u8);
    MotorCtrlsetSpeedSetPoint(regMap[REG_SPEED_SETPOINT].data.u32);
    MotorCtrlSetState(regMap[REG_SET_MOTOR_CTRL_STATUS].data.u32);
    
    MotrorCtrlProcess(regMap[REG_SPEED].data.u32);

}


/**
 * @brief Reads the speed data from the RC car
 * 
 */
void RcReadSpeedThread(void)
{
    regMap[REG_SPEED].data.u32 = encoder_counter_ReadCounter();
    encoder_counter_WriteCounter(0);
    vTaskDelay(100);
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
    regMap[REG_SPEED        ].data.u32 = speed_count;
    regMap[REG_LEFT_DISTANCE].data.u32 = leftDistance / 58;
}


/* [] END OF FILE */
