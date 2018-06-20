/**
 *  @file   MPU9250_REGS.h
 *  @author Nicholas Counts
 *  @date   06/19/18
 *  @brief  Register map and configuration data for the MPU9250 IMU on the
 *          TSLPB V3
 *
 */

 /* 2018 Counts Engineering */

#ifndef MPU9250_REGS_h
#define MPU9250_REGS_h


#define     GYRO_FULL_SCALE_250_DPS     0x00  ///< Gyroscope range parameter
#define     GYRO_FULL_SCALE_500_DPS     0x08  ///< Gyroscope range parameter
#define     GYRO_FULL_SCALE_1000_DPS    0x10  ///< Gyroscope range parameter
#define     GYRO_FULL_SCALE_2000_DPS    0x18  ///< Gyroscope range parameter

#define     ACC_FULL_SCALE_2_G          0x00  ///< Accelerometer range parameter
#define     ACC_FULL_SCALE_4_G          0x08  ///< Accelerometer range parameter
#define     ACC_FULL_SCALE_8_G          0x10  ///< Accelerometer range parameter
#define     ACC_FULL_SCALE_16_G         0x18  ///< Accelerometer range parameter

#define     MAG_MAX_BYTE_VALUE          0x7FF8  ///< Max register for scaling
#define     MAG_MAX_VALUE_FLOAT         4912    ///< in units of uT for scaling



enum MPU9250_TEMP_REGISTER_t
{
    MPU9250_TEMP_OUT_MSB =        0x41,
    MPU9250_TEMP_OUT_LSB =        0x42
};

enum
{
    MPU9250_ACCEL_XOUT_MSB      = 0x3B,
    MPU9250_ACCEL_XOUT_LSB      = 0x3C,
    MPU9250_ACCEL_YOUT_MSB      = 0x3D,
    MPU9250_ACCEL_YOUT_LSB      = 0x3E,
    MPU9250_ACCEL_ZOUT_MSB      = 0x3F,
    MPU9250_ACCEL_ZOUT_LSB      = 0x40,
    
    MPU9250_ACCEL_SELF_TEST_X   = 0x0D,
    MPU9250_ACCEL_SELF_TEST_Y   = 0x0E,
    MPU9250_ACCEL_SELF_TEST_Z   = 0x0F
    
}MPU9250_ACCEL_REGISTER_t;



enum MPU9250_GYRO_REGISTER_t
{
    MPU9250_GYRO_XOUT_MSB =       0x43,
    MPU9250_GYRO_XOUT_LSB =       0x44,
    MPU9250_GYRO_YOUT_MSB =       0x45,
    MPU9250_GYRO_YOUT_LSB =       0x46,
    MPU9250_GYRO_ZOUT_MSB =       0x47,
    MPU9250_GYRO_ZOUT_LSB =       0x48,
    
    MPU9250_GYRO_SELF_TEST_X =  0x00,
    MPU9250_GYRO_SELF_TEST_Y =  0x01,
    MPU9250_GYRO_SELF_TEST_Z =  0x02
    
};



enum MPU9250_MAG_REGISTER_t
{
    MPU9250_MAG_REG_DEVICE_ID     = 0x00,   ///< READ: Device ID
    MPU9250_MAG_REG_INFORMATION   = 0x01,   ///< READ: Information
    MPU9250_MAG_REG_STATUS_1      = 0x02,   ///< READ: Data status
    MPU9250_MAG_REG_X_DATA_LSB    = 0x03,   ///< READ: X-axis data (LSB)
    MPU9250_MAG_REG_X_DATA_MSB    = 0x04,   ///< READ: X-axis data (MSB)
    MPU9250_MAG_REG_Y_DATA_LSB    = 0x05,   ///< READ: Y-axis data (LSB)
    MPU9250_MAG_REG_Y_DATA_MSB    = 0x06,   ///< READ: Y-axis data (MSB)
    MPU9250_MAG_REG_Z_DATA_LSB    = 0x07,   ///< READ: Z-axis data (LSB)
    MPU9250_MAG_REG_Z_DATA_MSB    = 0x08,   ///< READ: Z-axis data (MLSB)
    MPU9250_MAG_REG_STATUS_2      = 0x09,   ///< READ: Data Status
    MPU9250_MAG_REG_CONTROL       = 0x0A,   ///< READ/WRITE: Mode Setting
    MPU9250_MAG_REG_SELF_TEST     = 0x0C,   ///< READ/WRITE:
    MPU9250_MAG_REG_I2C_DISABLE   = 0x0F,   ///< READ/WRITE:
    MPU9250_MAG_REG_X_SENSITIVITY = 0x10,   ///< READ:
    MPU9250_MAG_REG_Y_SENSITIVITY = 0x11,   ///< READ:
    MPU9250_MAG_REG_Z_SENSITIVITY = 0x12    ///< READ:
};

enum MPU9250_MAG_CONTROL_t
{
    MPU9250_MAG_STATUS_1_DATA_READY_BIT = 0x00,
    MPU9250_REG_INT_PIN_BYPASS          = 0x37, ///< READ/WRITE: Allow passthrough mode
    MPU9250_PASSTHROUGH_ON              = 0x02, ///< When asserted, the i2c_master interface pins go into ‘bypass mode’ when the i2c master interface is disabled. The pins will float high due to the internal pull-up if not enabled and the i2c master interface is disabled.
    MPU9250_PASSTHROUGH_OFF             = 0x00, ///< & with current register
    MAG_MODE_SINGLE_MEAS                = 0b0001, ///< Single Measurement Mode
    MAG_MODE_CONTINUOUS_8HZ             = 0b0010, ///< Continuous register update mode (8 Hz)
    MAG_MODE_CONTINUOUS_100HZ           = 0b0011, ///< Continuous register update mode (100 Hz)
    MAG_MODE_POWER_DOWN                 = 0b0000, ///< Low power standby mode
    MAG_MODE_SELF_TEST                  = 0b1000, ///< Perform a self test with internal magnetic field generator
    MAG_MODE_BITMASK                    = 0x0F, ///< bit mask for mode-setting register
    MAG_MODE_14_BIT                     = 0x00, ///< bit 4 off for 14-bit output
    MAG_MODE_16_BIT                     = 0x10, ///< bit 4 on for 16-bit output
    MAG_MASK_DATA_OVERRUN               = 0x02, ///< ST1 bit mask for data overrun
    MAG_MASK_DATA_READY                 = 0x01, ///< ST1 bit mask for data ready
    MAG_MASK_DATA_OVERFLOW              = 0x08, ///< ST2 bit mask for "Magnetic sensor overflow occurred" - true if true
    MAG_MASK_DATA_BIT_RESOLUTION        = 0x10, ///< ST2 bit mask: 0 if 14-bit output, 1 if 16-bit output
    
};

#endif /* MPU9250_REGS_h */
