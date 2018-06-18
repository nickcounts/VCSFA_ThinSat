//
//  TSLPB.h
//  Index
//
//  Created by Nicholas Counts on 6/12/18.
//  Copyright © 2018 Counts Engineering. All rights reserved.
//

#ifndef TSLPB_h
#define TSLPB_h


#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Wire.h"
#include "SparkFunMPU9250-DMP.h"


#define SERIAL_BUSY_LINE 4          // Serial Busy Line

#define TSL_ADC A7      // ADC reading the MUX_Output
#define TSL_MUX_A 7     // Mux A - TSLPB pin
#define TSL_MUX_B 8     // Mux B - TSLPB pin
#define TSL_MUX_C 9     // Mux C - TSLPB pin
#define TSL_MUX_RESPONSE_TIME 10 // 10 miliseconds to change

enum TSLAnalogSensorMux
{
    TSL_SERIAL_READY=0,
    TSL_SERIAL_BUSY
};

typedef enum
{
    Solar=0,
    IR,
    TempInt,
    TempExt,
    Current,
    Voltage
} TSLAnalogSensor_t;

//Mux
//Mux_In_1   000 (Solar Sensor)
//Mux_In_2   001 (IR)
//Mux_In_3   010 (Temp Int)
//Mux_In_4   011 (Temp Ext)
//Mux_In_5   100 (Current)
//Mux_In_6   101 (Vcc)


/*!
 * @brief TSLPB Digital Sensor Address Enums
 */
typedef enum
{
    DT1_ADDRESS = 0x4A,     // LM75A
    DT2_ADDRESS = 0x4C,     // LM75A
    DT3_ADDRESS = 0x4D,     // LM75A
    DT4_ADDRESS = 0x48,     // LM75A
    DT5_ADDRESS = 0x49,     // LM75A
    DT6_ADDRESS = 0x4B,     // LM75A
    IMU_ADDRESS = 0x69      // MPU-9250
    
} TSLPBDigitalSensorAddress_t;

/*!
 * @brief TSLPB Digital Sensor selection Enums
 */
typedef enum
{
    DT1 = 0,            // LM75A
    DT2,                // LM75A
    DT3,                // LM75A
    DT4,                // LM75A
    DT5,                // LM75A
    DT6,                // LM75A
    Accelerometer,      // MPU-9250
    Gyroscope,          // MPU-9250
    Magnetometer,       // MPU-9250
    IMU_Internal_Temp   // MPU-9250
} TSLPBDigitalSensor_t;


/*!
 * @defgroup LMA75A SENSOR API
 * @{*/
#define LMA_TEMP_REG_UNUSED_LSBS        5
#define LMA_TEMP_REG_SIGN_BIT           9
#define LMA_TEMP_REG_DEGREES_PER_LSB    0.125

/*!
 * @brief LM75A Register Selection Enum
 */
typedef enum
{
    LM75A_TEMPERATURE   = 0x0, // Read only. 0x5 C per LSB. d15=MSB, D7=LSB.
    LM75A_CONFIGURATION = 0x1, // Read/Write.
    LM75A_T_HYST        = 0x2, // Read/Write.
    LM75A_T_OS          = 0x3, // Read/Write.
    LM75A_PRODUCT_ID    = 0x7  // Read only.
} LM75A_REG;

typedef enum
{
    LMA_TEMP_REG_BYTES  = 2
} LMA75A_REG_INFO;


class TSLPB
{
    
public:
    void begin();
    uint8_t  readAnalogSensor(TSLAnalogSensor_t sensor);        // Get the raw ADC count
    uint16_t readDigitalSensorRaw(TSLPBDigitalSensor_t sensor); // Get the raw count
    double   readDigitalSensor(TSLPBDigitalSensor_t sensor);    // Get the process value
    void readAccelData(int16_t *);  /* ±2g, ±4g, ±8g, or ±16g */
    void readGyroData(int16_t *);   /* ±250, ±500, ±1000, or ±2000 °/s */
    void readMagData(int16_t *);    /* ±4800 μT */
    
    
    
private:
    uint8_t read8bitRegister (TSLPBDigitalSensorAddress_t i2cAddress, const uint8_t reg);
    bool    read16bitRegister(TSLPBDigitalSensorAddress_t i2cAddress, const uint8_t reg, uint16_t& response);
    void    writeByte(uint8_t, uint8_t, uint8_t);
    void    InitTSLAnalogSensors();
    TSLPBDigitalSensorAddress_t getDeviceAddress(TSLPBDigitalSensor_t sensorName);
};



#endif /* TSLPB_h */
