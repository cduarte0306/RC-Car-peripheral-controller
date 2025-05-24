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

#ifndef RC_CAR_H
#define RC_CAR_H

#include "types.h"
#include <cytypes.h>

#define READ_ONLY   0u
#define READ_WRITE  1u
    
    
typedef enum
{
    REG_NOOP,
    REG_VER_MAJOR,
    REG_VER_MINOR,
    REG_VER_BUILD,
    REG_SPEED,
    REG_FRONT_DISTANCE,
    REG_LEFT_DISTANCE,
    REG_RIGHT_DISTANCE,
    REG_RO_END
} registerEnumsReadOnly;

typedef enum
{
    REG_SET_MOTOR_STATUS = REG_RO_END,
    REG_SPEED_SETPOINT,
    REG_PID_P,
    REG_PID_I,
    REG_PID_D,
    REG_WR_END,
} registerEnumReadWrites;

typedef struct 
{
    uint8 regType;
    val_type_t data;
} regMapType;
    
    
uint8_t RCInit(void);
void RcProcess(void);
void RcReadSpeedThread( void );
regMapType* getRegRef(void);
    
    
#endif

/* [] END OF FILE */
