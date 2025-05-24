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

#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <project.h>
    
    
typedef struct
{
    float kp;
    float ki;
    float kd;
} pid_t;

enum
{
    MANUAL,
    AUTOMATIC
};
    

void MotorCtrlInit(void);
void MotorCtrlSetState(uint8_t state_);
uint8 MotorCtrlsetSpeedSetPoint(uint32_t speedSetPoint_);
uint8 setSpeedSetPoint(uint32_t speedSetPoint_);
uint8 MotorCtrlSetPid(pid_t* pid_);
void MotorCtrlMonitor(uint32_t speed);
    
#endif

/* [] END OF FILE */
