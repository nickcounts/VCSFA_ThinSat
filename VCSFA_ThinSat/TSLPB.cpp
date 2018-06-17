//
//  TSLPB.cpp
//  Index
//
//  Created by Nicholas Counts on 6/12/18.
//  Copyright © 2018 Counts Engineering. All rights reserved.
//

#include "TSLPB.h"

/*!
 * @brief Initializes the TSLPB, starts the I2C bus, and configures the pins
 * needed for reading the TSLPB analog sensors.
 *
 * @note This function changes the state of 4 I/O pins:
 * PIN         | MODE
 * ------------|-------------------
 * TSL_ADC     | Analog Input
 * TSL_MUX_A   | Digital Output
 * TSL_MUX_B   | Digital Output
 * TSL_MUX_C   | Digital Output
 */
void TSLPB::begin()
{
    Wire.begin();
    InitTSLAnalogSensors();
}



void TSLPB::InitTSLAnalogSensors()
{
    pinMode(TSL_ADC, INPUT);
    pinMode(TSL_MUX_A, OUTPUT);
    pinMode(TSL_MUX_B, OUTPUT);
    pinMode(TSL_MUX_C, OUTPUT);
}


uint8_t TSLPB::readAnalogSensor(TSLAnalogSensor_t sensor)
{
    digitalWrite(TSL_MUX_A, (sensor >> 2) & 0x1);
    digitalWrite(TSL_MUX_B, (sensor >> 1) & 0x1);
    digitalWrite(TSL_MUX_C, sensor & 0x1);
    delay(TSL_MUX_RESPONSE_TIME);
    
    return(analogRead(TSL_ADC));
}

/*!
 * @brief This API returns the raw value from the specified sensor. Handles
 * endiannes and discarding unused bits.
 *
 * @param[in] sensor : LM75A Register Selection Enum
 *
 * @return a uint16_t containing the bit pattern from the sensor's register.
 */
uint16_t TSLPB::readDigitalSensorRaw(TSLPBDigitalSensor_t sensor)
{
    uint16_t rawRegValue = 0;
    
    switch (sensor) {
        // Let any DT# sensor fall through to the common LM75A routine
        case DT1 :
        case DT2 :
        case DT3 :
        case DT4 :
        case DT5 :
        case DT6 :
            {
                uint16_t i2c_received = 0;
                
                read16bitRegister(sensor, LM75A_TEMPERATURE, i2c_received);
                
                rawRegValue = i2c_received >> LMA_TEMP_REG_UNUSED_LSBS;
                
                return rawRegValue;
            }
            
            break;
        case IMU:
            
            break;
            
        default:
            break;
    }
}

/*!
 * @brief This API returns the process from the specified sensor as a
 * double-precision floating point value in the appropriate units for the
 * sensor.
 *
 * @param[in] sensor : LM75A Register Selection Enum
 *
 * @return a value in the appropriate units for the sensor.
 */
double  TSLPB::readDigitalSensor(TSLPBDigitalSensor_t sensor)
{
    double processValue;
    uint16_t regContents = readDigitalSensorRaw(sensor);
    
    if ((bool)(regContents >> LMA_TEMP_REG_SIGN_BIT)) {
        // Has negative bit set. Flip unused MSbs
        regContents |= 0xF800;
    }
    
    processValue = (double)((int16_t)regContents);
    processValue *= LMA_TEMP_REG_DEGREES_PER_LSB;
    
    return processValue;
}

uint8_t TSLPB::read8bitRegister(TSLPBDigitalSensor_t sensor, const uint8_t reg)
{
    uint8_t result;

    Wire.beginTransmission(sensor);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(sensor, (uint8_t)1);
    result = Wire.read();
    return result;
}

/*!
 * @brief This private method assigns the bit-pattern of a specified I2C
 * register to the variable "response"
 *
 * @note This method DOES NOT implement error checking and always returns TRUE;
 *
 * @param[in] sensor    TSLPB Digital Sensor Enum (a uint8_t I2C address)
 * @param[in] reg       LM75A Register Selection Enum (a uint8_t I2C register)
 * @param[in] response  The variable to which the register contents will be
 *                      assigned
 *
 * @return true or false read success. Only returns true in this implementation
 */
bool TSLPB::read16bitRegister(TSLPBDigitalSensor_t sensor, const uint8_t reg, uint16_t& response)
{
    uint8_t result;
    
    Wire.beginTransmission(sensor); // Start with address
    Wire.write(reg);                // Set temperature register
    //    Wire.endTransmission();   // Discard succeed/fail status
    
    uint8_t bytesRead = Wire.requestFrom(sensor, 2);

    Wire.endTransmission();
    
    uint8_t part1 = Wire.read();
    uint8_t part2 = Wire.read();
 
    response = (part1 << 8) | part2;
    return true;
}

