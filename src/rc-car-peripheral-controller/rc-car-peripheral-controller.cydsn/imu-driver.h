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


// Int bitmasks
#define INT_STATUS_FIFO_OVERFLOW  (1 << 6)
#define INT_STATUS_I2C_MST        (1 << 5)
#define INT_STATUS_PLL_RDY        (1 << 4)
#define INT_STATUS_DMP            (1 << 1)
#define INT_STATUS_DATA_RDY       (1 << 0)


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
    float mag_x_uT;
    float mag_y_uT;
    float mag_z_uT;
} IMU_Mag_t;


uint8_t IMU_reset(void);
uint8_t IMU_wake(void);
uint8_t IMU_detect(void);
uint8_t IMU_initialize(void);
uint8_t IMU_clearInt(uint8_t* imuIntStatus);
uint8_t IMU_readAll(IMU_Data_t *imuData);
uint8_t IMU_magReady(void);
uint8_t IMU_readMag(IMU_Mag_t  *magData);

/* [] END OF FILE */
