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
#include "imu-driver.h"
#include "RCUtils.h"

#define IMU_ADDRESS 0x68  // Device address

// Register definitions
//======================================================
// MPU-9250 Register Map (with descriptive comments)
//======================================================

// Self-Test Registers
#define REG_SELF_TEST_X_GYRO       0x00    // Gyroscope self-test for X-axis
#define REG_SELF_TEST_Y_GYRO       0x01    // Gyroscope self-test for Y-axis
#define REG_SELF_TEST_Z_GYRO       0x02    // Gyroscope self-test for Z-axis
#define REG_SELF_TEST_X_ACCEL      0x0D    // Accelerometer self-test for X-axis
#define REG_SELF_TEST_Y_ACCEL      0x0E    // Accelerometer self-test for Y-axis
#define REG_SELF_TEST_Z_ACCEL      0x0F    // Accelerometer self-test for Z-axis

// Gyroscope Offset Registers
#define REG_XG_OFFSET_H            0x13    // Gyro X-axis offset high byte
#define REG_XG_OFFSET_L            0x14    // Gyro X-axis offset low byte
#define REG_YG_OFFSET_H            0x15    // Gyro Y-axis offset high byte
#define REG_YG_OFFSET_L            0x16    // Gyro Y-axis offset low byte
#define REG_ZG_OFFSET_H            0x17    // Gyro Z-axis offset high byte
#define REG_ZG_OFFSET_L            0x18    // Gyro Z-axis offset low byte

// Sampling & Configuration
#define REG_SMPLRT_DIV             0x19    // Sample rate divider
#define REG_CONFIG                 0x1A    // General configuration (DLPF, EXT_SYNC)
#define REG_GYRO_CONFIG            0x1B    // Gyroscope configuration (FSR, self-test)
#define REG_ACCEL_CONFIG           0x1C    // Accelerometer configuration (FSR, self-test)
#define REG_ACCEL_CONFIG2          0x1D    // Accelerometer configuration 2 (DLPF)
#define REG_LP_ACCEL_ODR           0x1E    // Low-power accelerometer ODR
#define REG_WOM_THR                0x1F    // Wake-on-motion threshold

// FIFO and I2C Master
#define REG_FIFO_EN                0x23    // FIFO enable for sensor data
#define REG_I2C_MST_CTRL           0x24    // I2C Master control
#define REG_I2C_SLV0_ADDR          0x25    // I2C slave 0 address
#define REG_I2C_SLV0_REG           0x26    // I2C slave 0 register
#define REG_I2C_SLV0_CTRL          0x27    // I2C slave 0 control
#define REG_I2C_SLV1_ADDR          0x28    // I2C slave 1 address
#define REG_I2C_SLV1_REG           0x29    // I2C slave 1 register
#define REG_I2C_SLV1_CTRL          0x2A    // I2C slave 1 control
#define REG_I2C_SLV2_ADDR          0x2B    // I2C slave 2 address
#define REG_I2C_SLV2_REG           0x2C    // I2C slave 2 register
#define REG_I2C_SLV2_CTRL          0x2D    // I2C slave 2 control
#define REG_I2C_SLV3_ADDR          0x2E    // I2C slave 3 address
#define REG_I2C_SLV3_REG           0x2F    // I2C slave 3 register
#define REG_I2C_SLV3_CTRL          0x30    // I2C slave 3 control
#define REG_I2C_SLV4_ADDR          0x31    // I2C slave 4 address
#define REG_I2C_SLV4_REG           0x32    // I2C slave 4 register
#define REG_I2C_SLV4_DO            0x33    // I2C slave 4 data out
#define REG_I2C_SLV4_CTRL          0x34    // I2C slave 4 control
#define REG_I2C_SLV4_DI            0x35    // I2C slave 4 data in
#define REG_I2C_MST_STATUS         0x36    // I2C master status

// Interrupt Configuration
#define REG_INT_PIN_CFG            0x37    // Interrupt pin configuration
#define REG_INT_ENABLE             0x38    // Interrupt enable (data ready, FIFO, etc.)
#define REG_INT_STATUS             0x3A    // Interrupt status flags (cleared on read)

// Sensor Data Registers
#define REG_ACCEL_XOUT_H           0x3B    // Accelerometer X-axis high byte
#define REG_ACCEL_XOUT_L           0x3C    // Accelerometer X-axis low byte
#define REG_ACCEL_YOUT_H           0x3D    // Accelerometer Y-axis high byte
#define REG_ACCEL_YOUT_L           0x3E    // Accelerometer Y-axis low byte
#define REG_ACCEL_ZOUT_H           0x3F    // Accelerometer Z-axis high byte
#define REG_ACCEL_ZOUT_L           0x40    // Accelerometer Z-axis low byte

#define REG_TEMP_OUT_H             0x41    // Temperature high byte
#define REG_TEMP_OUT_L             0x42    // Temperature low byte

#define REG_GYRO_XOUT_H            0x43    // Gyroscope X-axis high byte
#define REG_GYRO_XOUT_L            0x44    // Gyroscope X-axis low byte
#define REG_GYRO_YOUT_H            0x45    // Gyroscope Y-axis high byte
#define REG_GYRO_YOUT_L            0x46    // Gyroscope Y-axis low byte
#define REG_GYRO_ZOUT_H            0x47    // Gyroscope Z-axis high byte
#define REG_GYRO_ZOUT_L            0x48    // Gyroscope Z-axis low byte

// External Sensor Data (from AK8963 magnetometer or I2C slaves)
#define REG_EXT_SENS_DATA_00       0x49    // External sensor data byte 0
#define REG_EXT_SENS_DATA_01       0x4A    // External sensor data byte 1
#define REG_EXT_SENS_DATA_02       0x4B    // External sensor data byte 2
#define REG_EXT_SENS_DATA_03       0x4C    // External sensor data byte 3
#define REG_EXT_SENS_DATA_04       0x4D    // External sensor data byte 4
#define REG_EXT_SENS_DATA_05       0x4E    // External sensor data byte 5
#define REG_EXT_SENS_DATA_06       0x4F    // External sensor data byte 6
#define REG_EXT_SENS_DATA_07       0x50    // External sensor data byte 7
#define REG_EXT_SENS_DATA_08       0x51    // External sensor data byte 8
#define REG_EXT_SENS_DATA_09       0x52    // External sensor data byte 9
#define REG_EXT_SENS_DATA_10       0x53    // External sensor data byte 10
#define REG_EXT_SENS_DATA_11       0x54    // External sensor data byte 11
#define REG_EXT_SENS_DATA_12       0x55    // External sensor data byte 12
#define REG_EXT_SENS_DATA_13       0x56    // External sensor data byte 13
#define REG_EXT_SENS_DATA_14       0x57    // External sensor data byte 14
#define REG_EXT_SENS_DATA_15       0x58    // External sensor data byte 15
#define REG_EXT_SENS_DATA_16       0x59    // External sensor data byte 16
#define REG_EXT_SENS_DATA_17       0x5A    // External sensor data byte 17
#define REG_EXT_SENS_DATA_18       0x5B    // External sensor data byte 18
#define REG_EXT_SENS_DATA_19       0x5C    // External sensor data byte 19
#define REG_EXT_SENS_DATA_20       0x5D    // External sensor data byte 20
#define REG_EXT_SENS_DATA_21       0x5E    // External sensor data byte 21
#define REG_EXT_SENS_DATA_22       0x5F    // External sensor data byte 22
#define REG_EXT_SENS_DATA_23       0x60    // External sensor data byte 23

// I2C Slave Data Out
#define REG_I2C_SLV0_DO            0x63    // I2C slave 0 data out
#define REG_I2C_SLV1_DO            0x64    // I2C slave 1 data out
#define REG_I2C_SLV2_DO            0x65    // I2C slave 2 data out
#define REG_I2C_SLV3_DO            0x66    // I2C slave 3 data out

// Control and Reset
#define REG_I2C_MST_DELAY_CTRL     0x67    // I2C master delay control
#define REG_SIGNAL_PATH_RESET      0x68    // Signal path reset
#define REG_MOT_DETECT_CTRL        0x69    // Motion detection control
#define REG_USER_CTRL              0x6A    // User control (FIFO, I2C master enable)

// Power Management
#define REG_PWR_MGMT_1             0x6B    // Power management 1 (sleep, clock select)
#define REG_PWR_MGMT_2             0x6C    // Power management 2 (sensor enable)

// FIFO Control
#define REG_FIFO_COUNTH            0x72    // FIFO count high byte
#define REG_FIFO_COUNTL            0x73    // FIFO count low byte
#define REG_FIFO_R_W               0x74    // FIFO read/write

// Identification
#define REG_WHO_AM_I               0x75    // Device ID (should return 0x71 for MPU-9250)

// Accelerometer Offset Registers
#define REG_XA_OFFSET_H            0x77    // Accel X-axis offset high byte
#define REG_XA_OFFSET_L            0x78    // Accel X-axis offset low byte
#define REG_YA_OFFSET_H            0x7A    // Accel Y-axis offset high byte
#define REG_YA_OFFSET_L            0x7B    // Accel Y-axis offset low byte
#define REG_ZA_OFFSET_H            0x7D    // Accel Z-axis offset high byte
#define REG_ZA_OFFSET_L            0x7E    // Accel Z-axis offset low byte

#define IMU_ID  (0x71)  // Expected IMU ID


static uint8_t buffer[32] = {0};


uint8_t IMU_detect(void)
{
    uint8 ret = RET_FAIL;
    
    buffer[0] = REG_WHO_AM_I;
    ret = I2C_MasterWriteBuf(IMU_ADDRESS, (uint8 *) &buffer[0], sizeof(buffer[0]), I2C_MODE_NO_STOP);
    if (ret != I2C_MSTR_NO_ERROR)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_IMU, "app: IMU_detect | err: Failed to read I2C\r\n");
        
        I2C_MasterClearReadBuf();
        I2C_MasterClearWriteBuf();
        return RET_FAIL;
    }
    
    while ((I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT) == FALSE);
    
    buffer[0] = 0;
    buffer[1] = 0;
    ret = I2C_MasterReadBuf(IMU_ADDRESS, buffer, 1, I2C_MODE_REPEAT_START);
    if (ret != I2C_MSTR_NO_ERROR)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_IMU, "app: IMU_detect | err: Failed to read I2C\r\n");
        return RET_FAIL;
    }
    
    while ((I2C_MasterStatus() & I2C_MSTAT_RD_CMPLT) == FALSE);
    vLoggingPrintf(DEBUG_INFO, LOG_IMU, "app: IMU_detect | Device ID: %lu\r\n", buffer[0]);
    ret = buffer[0] == IMU_ID;
    buffer[0] = 0;
    return ret;
}


uint8_t IMU_reset(void)
{
    uint8 ret = RET_FAIL;
    
    // Write to the power management register
    buffer[0] = REG_PWR_MGMT_1;
    buffer[1] = (1U << 7U);  // Set the reset bit
    
    ret = I2C_MasterWriteBuf(IMU_ADDRESS, buffer, 2, I2C_MODE_COMPLETE_XFER);
    if (ret != I2C_MSTR_NO_ERROR)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_IMU, "app: IMU_init | err: Could not reset unit IMU\r\n");
        return RET_FAIL;
    }
    
    while ((I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT) == FALSE);
    return RET_PASS;
}


uint8_t IMU_wake(void)
{
    uint8 ret = RET_FAIL;
    
    //Write to clear SLEEP (bit 6 = 0) and select PLL as clock (bits [2:0] = 001)
    buffer[0] = REG_PWR_MGMT_1;
    buffer[1] = (1 << 0);
    
    // Check if the device is connected
    ret = I2C_MasterWriteBuf(IMU_ADDRESS, buffer, 2, I2C_MODE_COMPLETE_XFER);
    if (ret != I2C_MSTR_NO_ERROR)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_IMU, "app: %s | err: Failed to write I2C\r\n", __FUNCTION__);
        return RET_FAIL;
    }
    
    while ((I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT) == FALSE);
    return RET_PASS;
}


uint8_t IMU_initialize(void)
{
    uint8_t ret = RET_FAIL;

    //===========================================
    // 1. Verify WHO_AM_I (Device ID = 0x71)
    //===========================================
    buffer[0] = REG_WHO_AM_I;
    ret = I2C_MasterWriteBuf(IMU_ADDRESS, buffer, 1, I2C_MODE_NO_STOP);
    if (ret != I2C_MSTR_NO_ERROR)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_IMU, "IMU_init | Failed to write WHO_AM_I\r\n");
        return RET_FAIL;
    }
    while ((I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT) == FALSE);

    ret = I2C_MasterReadBuf(IMU_ADDRESS, buffer, 1, I2C_MODE_REPEAT_START);
    if (ret != I2C_MSTR_NO_ERROR)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_IMU, "IMU_init | Failed to read WHO_AM_I\r\n");
        return RET_FAIL;
    }
    while ((I2C_MasterStatus() & I2C_MSTAT_RD_CMPLT) == FALSE);

    if (buffer[0] != 0x71)  // Corrected to actual WHO_AM_I value
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_IMU, "IMU_init | Invalid WHO_AM_I: 0x%02X\r\n", buffer[0]);
        return RET_FAIL;
    }

    //===========================================
    // 2. Configure Gyroscope (±2000°/s)
    //===========================================
    buffer[0] = REG_GYRO_CONFIG;
    buffer[1] = (3 << 3);  // FS_SEL=11 -> ±2000°/s
    ret = I2C_MasterWriteBuf(IMU_ADDRESS, buffer, 2, I2C_MODE_COMPLETE_XFER);
    if (ret != I2C_MSTR_NO_ERROR)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_IMU, "IMU_init | Failed to write GYRO_CONFIG\r\n");
        return RET_FAIL;
    }
    while ((I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT) == FALSE);

    //===========================================
    // 3. Configure Accelerometer (±16g)
    //===========================================
    buffer[0] = REG_ACCEL_CONFIG;
    buffer[1] = (3 << 3);  // AFS_SEL=11 -> ±16g
    ret = I2C_MasterWriteBuf(IMU_ADDRESS, buffer, 2, I2C_MODE_COMPLETE_XFER);
    if (ret != I2C_MSTR_NO_ERROR)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_IMU, "IMU_init | Failed to write ACCEL_CONFIG\r\n");
        return RET_FAIL;
    }
    while ((I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT) == FALSE);

    //===========================================
    // 4. Configure Accelerometer DLPF
    //===========================================
    buffer[0] = REG_ACCEL_CONFIG2;
    buffer[1] = (0 << 3) | (3);  // FCHOICE_B=0, DLPF=3
    ret = I2C_MasterWriteBuf(IMU_ADDRESS, buffer, 2, I2C_MODE_COMPLETE_XFER);
    if (ret != I2C_MSTR_NO_ERROR)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_IMU, "IMU_init | Failed to write ACCEL_CONFIG2\r\n");
        return RET_FAIL;
    }
    while ((I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT) == FALSE);

    //===========================================
    // 5. Configure Gyroscope DLPF
    //===========================================
    buffer[0] = REG_CONFIG;
    buffer[1] = 0x03;  // Gyro DLPF = 44.8Hz
    ret = I2C_MasterWriteBuf(IMU_ADDRESS, buffer, 2, I2C_MODE_COMPLETE_XFER);
    if (ret != I2C_MSTR_NO_ERROR)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_IMU, "IMU_init | Failed to write CONFIG\r\n");
        return RET_FAIL;
    }
    while ((I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT) == FALSE);

    //===========================================
    // 6. Set Sample Rate Divider
    //===========================================
    buffer[0] = REG_SMPLRT_DIV;
    buffer[1] = 9;  // 100Hz sample rate
    ret = I2C_MasterWriteBuf(IMU_ADDRESS, buffer, 2, I2C_MODE_COMPLETE_XFER);
    if (ret != I2C_MSTR_NO_ERROR)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_IMU, "IMU_init | Failed to write SMPLRT_DIV\r\n");
        return RET_FAIL;
    }
    while ((I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT) == FALSE);

    //===========================================
    // 7. Enable All Axes
    //===========================================
    buffer[0] = REG_PWR_MGMT_2;
    buffer[1] = 0x00;
    ret = I2C_MasterWriteBuf(IMU_ADDRESS, buffer, 2, I2C_MODE_COMPLETE_XFER);
    if (ret != I2C_MSTR_NO_ERROR)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_IMU, "IMU_init | Failed to write PWR_MGMT_2\r\n");
        return RET_FAIL;
    }
    while ((I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT) == FALSE);

    //===========================================
    // 8. Configure Interrupt Pin
    //===========================================
    buffer[0] = REG_INT_PIN_CFG;
    buffer[1] = (1 << 7) | (0 << 6) | (1 << 5) | (1 << 4);  // Push-pull, active low
    ret = I2C_MasterWriteBuf(IMU_ADDRESS, buffer, 2, I2C_MODE_COMPLETE_XFER);
    if (ret != I2C_MSTR_NO_ERROR)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_IMU, "IMU_init | Failed to write INT_PIN_CFG\r\n");
        return RET_FAIL;
    }
    while ((I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT) == FALSE);

    //===========================================
    // 9. Enable Data Ready Interrupt
    //===========================================
    buffer[0] = REG_INT_ENABLE;
    buffer[1] = (1 << 0);  // Enable data-ready interrupt
    ret = I2C_MasterWriteBuf(IMU_ADDRESS, buffer, 2, I2C_MODE_COMPLETE_XFER);
    if (ret != I2C_MSTR_NO_ERROR)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_IMU, "IMU_init | Failed to write INT_ENABLE\r\n");
        return RET_FAIL;
    }
    while ((I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT) == FALSE);

    
    return RET_PASS;
}


uint8_t IMU_readAll(IMU_Data_t *data)
{
    if(!data)
        return RET_FAIL;
    
    uint8_t ret = RET_FAIL;

    // Start register: ACCEL_XOUT_H (0x3B), read 14 bytes
    buffer[0] = REG_ACCEL_XOUT_H;
    ret = I2C_MasterWriteBuf(IMU_ADDRESS, buffer, 1, I2C_MODE_NO_STOP);
    if (ret != I2C_MSTR_NO_ERROR)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_RC_CAR, "IMU_readAll | Failed to write start reg\r\n");
        return RET_FAIL;
    }
    while ((I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT) == FALSE);

    ret = I2C_MasterReadBuf(IMU_ADDRESS, buffer, 14, I2C_MODE_REPEAT_START);
    if (ret != I2C_MSTR_NO_ERROR)
    {
        vLoggingPrintf(DEBUG_ERROR, LOG_RC_CAR, "IMU_readAll | Failed to read sensor data\r\n");
        return RET_FAIL;
    }
    while ((I2C_MasterStatus() & I2C_MSTAT_RD_CMPLT) == FALSE);

    // Combine high and low bytes (signed 16-bit values)
    data->accel_x     = (int16_t)((buffer[0]  << 8) | buffer[1]);
    data->accel_y     = (int16_t)((buffer[2]  << 8) | buffer[3]);
    data->accel_z     = (int16_t)((buffer[4]  << 8) | buffer[5]);
    data->temperature = (int16_t)((buffer[6]  << 8) | buffer[7]);
    data->gyro_x      = (int16_t)((buffer[8]  << 8) | buffer[9]);
    data->gyro_y      = (int16_t)((buffer[10] << 8) | buffer[11]);
    data->gyro_z      = (int16_t)((buffer[12] << 8) | buffer[13]);

    return RET_PASS;
}


/* [] END OF FILE */
