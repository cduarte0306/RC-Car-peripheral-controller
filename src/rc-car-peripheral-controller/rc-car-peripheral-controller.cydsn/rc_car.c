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
#include "imu-driver.h"

#include <project.h>


#define RD_SPEED_DATA   (speed_msb_Status << 8U) | ( speed_lsb_Status )
#define SEL_MAX         (3U)


static uint32_t lastSpeed = 0;
static regMapType regMap[ REG_WR_END ];
static uint32_t   speed_count;

static uint8_t imuPresent = FALSE;

static volatile uint32_t rightDistance = 0.0;
static volatile uint32_t leftDistance  = 0.0;
static volatile uint32_t frontDistance = 0.0;

static volatile uint8_t imuDataReady = FALSE;

static volatile uint8_t sensorSel = 0;

static void readTelemetry(void);
static void initIMU(void);
static void readIMU(void);


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


CY_ISR(imu_handler)
{
    if(imuDataReady != TRUE)
        imuDataReady = TRUE;
    imu_interrupt_ClearPending();
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
    
    I2C_Start();
    I2C_MasterClearStatus();
    
    // Set the version in the registers
    getVers(&regMap[REG_VER_MAJOR].data.u8, &regMap[REG_VER_MINOR].data.u8, &regMap[REG_VER_BUILD].data.u8);
    
    regMap[REG_NOOP].data.u32 = 0;
    
    encoder_counter_Start();
    
    isr_left_echo_StartEx(ultrasonic_handler_left);
    isr_right_echo_StartEx(ultrasonic_handler_right);
    isr_front_echo_StartEx(ultrasonic_handler_front);
    
    initIMU();
    
    imu_interrupt_StartEx(imu_handler);
    
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
    IMU_Data_t imuData;
    
    regMap[REG_SPEED         ].data.u32 = speed_count;
    regMap[REG_LEFT_DISTANCE ].data.u32 = leftDistance  / 58;
    regMap[REG_RIGHT_DISTANCE].data.u32 = rightDistance / 58;
    regMap[REG_FRONT_DISTANCE].data.u32 = frontDistance / 58;
    
    readIMU();
}


static void readIMU(void)
{
    if(!imuDataReady)
    {
        return; // IMU not ready
    }
    
    vLoggingPrintf(DEBUG_INFO, LOG_RC_CAR, "app: readIMU | reading IMU...\r\n");
    
    IMU_Data_t imuData;
    IMU_readAll(&imuData);
    
    regMap[REG_ACCEL_X     ].data.f32 = imuData.accel_x / 2048.0f;       // g
    regMap[REG_ACCEL_Y     ].data.f32 = imuData.accel_y / 2048.0f;       // g
    regMap[REG_ACCEL_Z     ].data.f32 = imuData.accel_z / 2048.0f;       // g
    regMap[REG_GYRO_X      ].data.f32 = imuData.gyro_x / 16.4f;          // °/s
    regMap[REG_GYRO_Y      ].data.f32 = imuData.gyro_y / 16.4f;          // °/s
    regMap[REG_GYRO_Z      ].data.f32 = imuData.gyro_z / 16.4f;          // °/s
    regMap[REG_TEMPERATURE ].data.f32 = (imuData.temperature / 333.87f) + 21.0f; // °C
    
    imuDataReady = FALSE;
}


static void initIMU(void)
{
    uint8_t ret;
    
    vLoggingPrintf(DEBUG_INFO, LOG_RC_CAR, "app: initIMU | Initializng IMU...\r\n");
    vLoggingPrintf(DEBUG_INFO, LOG_RC_CAR, "app: initIMU | Resetting IMU...\r\n");
    
    // Initialize the IMU
    ret = IMU_reset();
    if (!ret)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_RC_CAR, "app: initIMU | err: Could not reset IMU\r\n");
        return;
    }
    
    vTaskDelay(100);  // Need to wait 100ms before resetting IMU
    vLoggingPrintf(DEBUG_INFO, LOG_RC_CAR, "app: initIMU | Waking IMU...\r\n");
    
    ret = IMU_wake();
    if (!ret)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_RC_CAR, "app: initIMU | err: Could not wake IMU\r\n");
        return;
    }
    
    vTaskDelay(30);  // Let the Unit wake up
    
    vLoggingPrintf(DEBUG_INFO, LOG_RC_CAR, "app: initIMU | Checking IMU ID...\r\n");
    ret = IMU_detect();
    if (!ret)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_RC_CAR, "app: initIMU | err: Could not detect IMU\r\n");
        return;
    }
    
    vLoggingPrintf(DEBUG_INFO, LOG_RC_CAR, "app: initIMU | Initializing registers...\r\n");
    ret = IMU_initialize();
    if (!ret)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_RC_CAR, "app: initIMU | err: Could not initialize IMU\r\n");
        return;
    }
    
    // Initialization Complete
    vLoggingPrintf(DEBUG_INFO, LOG_RC_CAR, "IMU_init | Initialization successful\r\n");
    
    imuPresent = TRUE;
}


/* [] END OF FILE */
