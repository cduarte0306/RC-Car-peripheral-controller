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

#include "motor_driver.h"
#include "RCUtils.h"


static uint8_t state = MANUAL;
static uint32_t lastTime = 0;
static uint32_t speedSetPoint = 0;
static uint8_t motor_on_off_state = 0;

static float integral = 0.0;
static pid_t pid = {
    .kp = 1,
    .ki = 0,
    .kd = 0
};

static float doCruiseControl(uint32_t speed);


/**
 * @brief Initializes the motor controller by starting the PWM and setting the initial compare value.
 */
void MotorCtrlInit(void)
{
    PWM_Motor_Start();
    PWM_Motor_WriteCompare(0);
    
    vLoggingPrintf(DEBUG_INFO, LOG_MOTOR, "app: MotorCtrlInit | Motor initialized\r\n");
}


/**
 * @brief Sets the PID parameters for the motor controller.
 * 
 * @param pid_ Pointer to the pid_t structure containing the PID parameters.
 * @return uint8_t RET_PASS on success, RET_FAIL on failure.
 */
uint8 MotorCtrlSetPid(pid_t* pid_)
{
    if (pid_ == NULL)
    {
        return RET_FAIL;
    }

    if (pid_->kp != pid.kp) pid.kp = pid_->kp;
    if (pid_->ki != pid.ki) pid.ki = pid_->ki;
    if (pid_->kd != pid.kd) pid.kd = pid_->kd;
    
    return RET_PASS;
}


/**
 * @brief Monitors the motor control and adjusts the speed based on the current state.
 * 
 * @param speed Speed of the motor to be controlled.
 */
void MotrorCtrlProcess(uint32_t speed)
{
    doCruiseControl(speed);

    switch(state)
    {
        case MANUAL:
            break;
        
        case AUTOMATIC:
        {
            float output = doCruiseControl(speed);
            CY_SET_REG16(PWM_Motor_COMPARE1_LSB_PTR, (uint16)output);
            break;
        }       
    }
}


/**
 * @brief Sets the state of the motor controller.
 * 
 * @param state_ The new state to set.
 */
void MotorCtrlSetState(uint8_t state_)
{
    if (state_ == state)
    {
        return;
    }
    
    state = state_;
    motor_sel_Control = state;
    vLoggingPrintf(DEBUG_INFO, LOG_MOTOR, "app: MotorCtrlSetState | Configured motor state to %u\r\n", state);
}


/**
 * @brief Sets the speed set point for the motor controller.
 * 
 * @param speedSetPoint_ The desired speed set point.
 * @return uint8_t RET_PASS on success, RET_FAIL on failure.
 */
uint8 MotorCtrlsetSpeedSetPoint(uint32_t speedSetPoint_)
{
    if (speedSetPoint_ == speedSetPoint)
    {
        return RET_PASS;
    }
    
    speedSetPoint = speedSetPoint_;
    
    vLoggingPrintf(DEBUG_INFO, LOG_MOTOR, "app: MotorCtrlSetState | Configured motor speed to %lu\r\n", speedSetPoint);
    return RET_PASS;
}


/**
 * @brief Sets the on/off state of the motor.
 * 
 * @param onOffState The desired state (MOTOR_ON or MOTOR_OFF).
 * @return uint8_t RET_PASS on success, RET_FAIL on failure.
 */
uint8 MotorCtrlSetOnOffState(uint8_t onOffState)
{
    if (onOffState == motor_on_off_state)
    {
        return RET_PASS;
    }

    if (onOffState)
    {
        CY_SET_REG16(PWM_Motor_COMPARE1_LSB_PTR, 0);
        PWM_Motor_Start();
        vLoggingPrintf(DEBUG_INFO, LOG_MOTOR, "app: MotorCtrlSetOnOffState | Motor started\r\n");
    }
    else
    {
        PWM_Motor_Stop();
        vLoggingPrintf(DEBUG_INFO, LOG_MOTOR, "app: MotorCtrlSetOnOffState | Motor stopped\r\n");
    }
    
    return RET_PASS;
}


/**
 * @brief Performs cruise control calculations based on the current speed and set point.
 * 
 * @param speed Current speed of the motor.
 * @return float Control output for the motor.
 */
static float doCruiseControl(uint32_t speed)
{
    int err = speedSetPoint - speed;
    uint32_t timeNow = xGetTimestamp();
    
    float tDelta = (float)(timeNow - lastTime) / 1000.0f;  // Convert ms to seconds
    float proportional = pid.kp * (float)err;
    integral += pid.ki * (float)err * tDelta;
    
    float controlOutput = proportional + integral;
    
    lastTime = timeNow;
    
    return controlOutput;
}


/* [] END OF FILE */
