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

static float integral = 0.0;
static pid_t pid = {
    .kp = 1,
    .ki = 0,
    .kd = 0
};

static float doCruiseControl(uint32_t speed);


void MotorCtrlInit(void)
{
    PWM_Motor_Start();
    PWM_Motor_WriteCompare(0);
}


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


void MotorCtrlMonitor(uint32_t speed)
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


static float doCruiseControl(uint32_t speed)
{
    int err = speedSetPoint - speed;
    uint32_t timeNow = xGetTimestamp();
    
    float tDelta = (float)(timeNow - lastTime) / 1000.0f;  // Convert ms to seconds if necessary
    float proportional = pid.kp * (float)err;
    integral += pid.ki * (float)err * tDelta;
    
    float controlOutput = proportional + integral;
    
    lastTime = timeNow;
    
    return controlOutput;
}


/* [] END OF FILE */
