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
    REG_SPEED,
    REG_FRONT_DISTANCE,
    REG_LEFT_DISTANCE,
    REG_RIGHT_DISTANCE,
    REG_END
} registerEnums;

typedef struct 
{
    uint8 regType;
    val_type_t data;
} regMapType;
    
    
uint8_t RCInit(void);
void RCprocessTelemetry(void);
void RCreadSpeedThread( void );
regMapType* getRegRef(void);
    
    
#endif

/* [] END OF FILE */
