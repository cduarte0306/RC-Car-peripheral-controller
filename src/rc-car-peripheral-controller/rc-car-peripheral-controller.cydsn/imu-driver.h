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

#include <cytypes.h>


typedef struct
{
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;

    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;

    int16_t temperature;
} IMU_Data_t;

typedef struct
{
    int16_t mag_x;
    int16_t mag_y;
    int16_t mag_z;
} IMU_Mag_t;


uint8_t IMU_reset(void);
uint8_t IMU_wake(void);
uint8_t IMU_detect(void);
uint8_t IMU_initialize(void);
uint8_t IMU_clearInt(void);
uint8_t IMU_readAll(IMU_Data_t *imuData);
uint8_t IMU_magReady(void);
uint8_t IMU_readMag(IMU_Mag_t  *magData);

/* [] END OF FILE */
