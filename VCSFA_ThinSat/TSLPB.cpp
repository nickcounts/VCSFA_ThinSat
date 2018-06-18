/**
 *  @file TSLPB.cpp
 *  @author Nicholas Counts
 *  @date 06/12/18
 *  @brief Implementation of TSLPB interface for Arduino
 *
 *
 */

 /* 2018 Counts Engineering */

#include "TSLPB.h"

/*!
 * @brief Initializes the TSLPB, starts the I2C bus, and configures the pins
 * needed for reading the TSLPB analog sensors.
 *
 * Call this function in the setup() function as follows:
 * @code
 *  void setup() {
 *      tslpb.begin();
 *  }
 * @endcode
 *
 * @note This function changes the state of 4 I/O pins:
 * PIN         | MODE
 * ------------|-----
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


uint8_t TSLPB::readAnalogSensor(TSLPB_AnalogSensor_t sensor)
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
 * @param[in] sensorName : TSLPB_DigitalSensor_t Sensor Name Enum
 *
 * @return a uint16_t containing the bit pattern from the sensor's register.
 */
uint16_t TSLPB::readDigitalSensorRaw(TSLPB_DigitalSensor_t sensorName)
{
    uint16_t rawRegValue = 0;
    TSLPB_DigitalSensorAddress_t address = getDeviceAddress(sensorName);
    uint16_t i2c_received = 0;
    
    switch (sensorName) {
        
        case Gyroscope:
            break;
        case Magnetometer:
            break;
        case Accelerometer:
            break;
        case IMU_Internal_Temp:
            break;
            
        // Let any DT# sensor fall through to the common LM75A routine
        case DT1 :
        case DT2 :
        case DT3 :
        case DT4 :
        case DT5 :
        case DT6 :
        {
            read16bitRegister(address, LM75A_TEMPERATURE, i2c_received);
            
            rawRegValue = i2c_received >> LMA_TEMP_REG_UNUSED_LSBS;
            
            return rawRegValue;
        }
            
        default:
            // Bad address passed. Return a dummy value
            return 0;
            break;
    }
    
    return 0;
            

}

/*!
 * @brief This API returns the process from the specified sensor as a
 * double-precision floating point value in the appropriate units for the
 * sensor.
 *
 * @param[in] sensorName    TSLPB_DigitalSensor_t Sensor Name Selection Enum
 *
 * @return  a value in the appropriate units for the sensor as a double precision
 *          floating point value.
 */
double  TSLPB::readDigitalSensor(TSLPB_DigitalSensor_t sensorName)
{
    double processValue;
    uint16_t regContents = readDigitalSensorRaw(sensorName);
    
    if ((bool)(regContents >> LMA_TEMP_REG_SIGN_BIT)) {
        // Has negative bit set. Flip unused MSbs
        regContents |= 0xF800;
    }
    
    processValue = (double)((int16_t)regContents);
    processValue *= LMA_TEMP_REG_DEGREES_PER_LSB;
    
    return processValue;
}

/*!
 * @brief This API returns the I2C address for the specified sensor as a
 * uint8_t (one byte). If the sensor argument is invalid, returns 0x0
 *
 * @param[in] sensorName : LM75A TSLPBDigitalSensorAddress_t Enum
 *
 * @return I2C Device address as a uint8_t
 */
TSLPB_DigitalSensorAddress_t TSLPB::getDeviceAddress(TSLPB_DigitalSensor_t sensorName) {
    switch (sensorName) {
        case Gyroscope:
        case Magnetometer:
        case Accelerometer:
        case IMU_Internal_Temp:
            return IMU_ADDRESS;
            
        case DT1 :
            return DT1_ADDRESS;
        case DT2 :
            return DT2_ADDRESS;
        case DT3 :
            return DT3_ADDRESS;
        case DT4 :
            return DT4_ADDRESS;
        case DT5 :
            return DT5_ADDRESS;
        case DT6 :
            return DT6_ADDRESS;
        default:
            // Bad address passed. Return a dummy value
            return 0;
            break;
    }
}


uint8_t TSLPB::read8bitRegister(TSLPB_DigitalSensorAddress_t i2cAddress, const uint8_t reg)
{
    uint8_t result;

    Wire.beginTransmission(i2cAddress);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)i2cAddress, (uint8_t)1);
    result = Wire.read();
    return result;
}

/*!
 * @brief This private method assigns the bit-pattern of a specified I2C
 * register to the variable "response"
 *
 * @note This method DOES NOT implement error checking and always returns TRUE;
 *
 * @param[in]   i2cAddress  TSLPB Digital Sensor Enum (a uint8_t I2C address)
 * @param[in]   reg         LM75A Register Selection Enum (a uint8_t I2C register)
 * @param[out]  response    The variable to which the register contents will be
 *                          assigned
 * @return true or false read success. Only returns true in this implementation
 */
bool TSLPB::read16bitRegister(TSLPB_DigitalSensorAddress_t i2cAddress, const uint8_t reg, uint16_t& response)
{
    uint8_t result;
    
    Wire.beginTransmission(i2cAddress); // Start with address
    Wire.write(reg);                // Set temperature register
    //    Wire.endTransmission();   // Discard succeed/fail status
    
    uint8_t bytesRead = Wire.requestFrom(i2cAddress, 2);

    Wire.endTransmission();
    
    uint8_t part1 = Wire.read();
    uint8_t part2 = Wire.read();
 
    response = (part1 << 8) | part2;
    return true;
}

/**
 * @mainpage Twiggs Space Lab Payload Board Driver
 *
 *  TSLPB is a driver class that can be instantiated and used to access the
 *  sensors and devices on the TSLPB V3 for the ThinSat program.
 *
 *  The driver sets up all the input and output pins required for accessing the
 *  analog sensors, and provides methods for reading both the analog and digital
 *  sensors.
 *
 * @section Basic Usage
 *
 *  You will need to do the following to use this library:
 *      1. Include tslbp.h in your program.
 *      2. Instantiate a TSLPB object
 *      3. Run the TSLPB::begin() method
 *
 *  Once these steps are complete, you may call any of the public methods to
 *  interact with the TSL Payload Board.
 *
 * @subsection Example
 *
 * @code
 *  #include "TSLPB.h"
 *
 *  TSLBP tslpb;
 *
 *  void setup() {
 *      tslpb.begin();
 *  }
 *
 *  void loop() {
 *      uint16_t tslVolts   = tslpb.readAnalogSensor(Voltage);
 *      uint16_t tslCurrent = tslpb.readAnalogSensor(Current);
 *      uint16_t tslTempExt = tslpb.readAnalogSensor(TempExt);
 *
 *      uint16_t tslDT1Raw  = tslpb.readTSLDigitalSensorRaw(DT1);
 *      double   tslDT1C    = tslpb.readTSLDigitalSensor(DT1);
 *  }
 *
 * @endcode
 */
