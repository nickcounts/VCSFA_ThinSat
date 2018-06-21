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
 * PIN                      | MODE
 * ------------             |-----
 * TSL_ADC                  | Analog Input
 * TSL_MUX_A                | Digital Output
 * TSL_MUX_B                | Digital Output
 * TSL_MUX_C                | Digital Output
 * TSL_SERIAL_STATUS_PIN    | Digital Input
 */
void TSLPB::begin()
{
    Wire.begin();
    InitTSLAnalogSensors();
    InitTSLDigitalSensors();
    pinMode(TSL_SERIAL_STATUS_PIN, INPUT);
    
    }


void TSLPB::InitTSLDigitalSensors(){

    write8bitRegister(IMU_ADDRESS, 29,  0x06);  // (MPU9250_ADDRESS,29,0x06);  // Set accelerometers low pass filter at 5Hz
    write8bitRegister(IMU_ADDRESS, 26,  0x06);  // Set gyroscope low pass filter at 5Hz
    
    write8bitRegister(IMU_ADDRESS, 27,  GYRO_FULL_SCALE_1000_DPS);  // Configure gyroscope range
    write8bitRegister(IMU_ADDRESS, 28,  ACC_FULL_SCALE_16_G); // Configure accelerometers range
    
    write8bitRegister(IMU_ADDRESS, 0x37,0x02);  // Set by pass mode for the magnetometers
    //    write8bitRegister(MAG_ADDRESS, 0x0A,0x16);  // Request continuous magnetometer measurements in 16 bits
    write8bitRegister(MAG_ADDRESS, MPU9250_MAG_REG_CONTROL,(MAG_MODE_16_BIT | MAG_MODE_CONTINUOUS_8HZ));  // Request continuous magnetometer measurements in 16 bits

//    write8bitRegister(IMU_ADDRESS, MPU9250_REG_INT_PIN_BYPASS, MPU9250_PASSTHROUGH_ON);
//    write8bitRegister(MAG_ADDRESS, MPU9250_MAG_REG_CONTROL, 0b10110); ///< Bit pattern for "continuous 16bit reporting"

}

void TSLPB::InitTSLAnalogSensors()
{
    pinMode(TSL_ADC, INPUT);
    pinMode(TSL_MUX_A, OUTPUT);
    pinMode(TSL_MUX_B, OUTPUT);
    pinMode(TSL_MUX_C, OUTPUT);
}

/*!
 * @brief This method returns the raw value from the specified analog sensor.
 *
 * @param[in] sensorName : TSLPB_AnalogSensor_t Sensor Name enum
 *
 * @return a uint16_t containing raw value of the Arduino Pro Mini's ADC.
 *
 * @note    The TSLPB uses a 10-bit Analog-to-Digital Converter. The 6 MSbs of
 *          the return value will always be 0.
 */
uint16_t TSLPB::readAnalogSensor(TSLPB_AnalogSensor_t sensorName)
{
    digitalWrite(TSL_MUX_A, (sensorName >> 2) & 0x1);
    digitalWrite(TSL_MUX_B, (sensorName >> 1) & 0x1);
    digitalWrite(TSL_MUX_C, sensorName & 0x1);
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
    uint16_t i2c_received = 0;  ///< I2C buffer for read function
    uint16_t rawRegValue = 0;   ///< return value, after endian correction
    TSLPB_I2CAddress_t address = getDeviceAddress(sensorName);
    uint8_t status = 0;
    
    
    switch (sensorName) {
        
        case Gyroscope_x:
            read16bitRegister(address, MPU9250_GYRO_XOUT_MSB, i2c_received);
            return i2c_received;
            break;
        case Gyroscope_y:
            read16bitRegister(address, MPU9250_GYRO_YOUT_MSB, i2c_received);
            return i2c_received;
            break;
        case Gyroscope_z:
            read16bitRegister(address, MPU9250_GYRO_ZOUT_MSB, i2c_received);
            return i2c_received;
            break;

        
        case Accelerometer_x:
            read16bitRegister(address, MPU9250_ACCEL_XOUT_MSB, i2c_received);
            return i2c_received;
            break;
        case Accelerometer_y:
            read16bitRegister(address, MPU9250_ACCEL_YOUT_MSB, i2c_received);
            return i2c_received;
            break;
        case Accelerometer_z:
            read16bitRegister(address, MPU9250_ACCEL_ZOUT_MSB, i2c_received);
            return i2c_received;
            break;

        case Magnetometer_x:
            waitForMagReady();
            // Read measurement data (sets DRDY and DOR to 0)
            rawRegValue  =  read8bitRegister(MAG_ADDRESS, MPU9250_MAG_REG_X_DATA_LSB);
            rawRegValue |= (read8bitRegister(MAG_ADDRESS, MPU9250_MAG_REG_X_DATA_MSB) << 8);
            
            // Read ST2 register (required)
            // HOFL shows if overflow. 0 is good, 1 is overflow
            status = read8bitRegister(MAG_ADDRESS, MPU9250_MAG_REG_STATUS_2);
            
            // Set the magnetometer overflow flag
            isMagnetometerOverflow = (bool)((status & MAG_MASK_DATA_OVERFLOW) >> 3);
            
            return rawRegValue;
        case Magnetometer_y:
            waitForMagReady();
            rawRegValue  =  read8bitRegister(MAG_ADDRESS, MPU9250_MAG_REG_Y_DATA_LSB);
            rawRegValue |= (read8bitRegister(MAG_ADDRESS, MPU9250_MAG_REG_Y_DATA_MSB) << 8);
            status       =  read8bitRegister(MAG_ADDRESS, MPU9250_MAG_REG_STATUS_2);
            
            // Set the magnetometer overflow flag
            isMagnetometerOverflow = (bool)((status & MAG_MASK_DATA_OVERFLOW) >> 3);
            
            return rawRegValue;
            
        case Magnetometer_z:
            waitForMagReady();
            rawRegValue  =  read8bitRegister(MAG_ADDRESS, MPU9250_MAG_REG_Z_DATA_LSB);
            rawRegValue |= (read8bitRegister(MAG_ADDRESS, MPU9250_MAG_REG_Z_DATA_MSB) << 8);
            status       =  read8bitRegister(MAG_ADDRESS, MPU9250_MAG_REG_STATUS_2);
            
            // Set the magnetometer overflow flag
            isMagnetometerOverflow = (bool)((status & MAG_MASK_DATA_OVERFLOW) >> 3);
            
            return rawRegValue;
            
            
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
    
    switch (sensorName) {
        case Magnetometer_x:
        case Magnetometer_y:
        case Magnetometer_z:
            return (double)((int16_t)regContents)*MAG_MAX_VALUE_FLOAT/MAG_MAX_BYTE_VALUE;
            break;
            
        case DT1:
        case DT2:
        case DT3:
        case DT4:
        case DT5:
        case DT6:
            if ((bool)(regContents >> LMA_TEMP_REG_SIGN_BIT)) {
                // Has negative bit set. Flip unused MSbs
                regContents |= 0xF800;
            }
            processValue = (double)((int16_t)regContents);
            processValue *= LMA_TEMP_REG_DEGREES_PER_LSB;
            return processValue;
            break;
            
        default:
            break;
    }
    
    
    
}

/*!
 * @brief This API returns the I2C address for the specified sensor as a
 * uint8_t (one byte). If the sensor argument is invalid, returns 0x0
 *
 * @param[in] sensorName : LM75A TSLPBDigitalSensorAddress_t Enum
 *
 * @return I2C Device address as a uint8_t
 */
TSLPB_I2CAddress_t TSLPB::getDeviceAddress(TSLPB_DigitalSensor_t sensorName) {
    switch (sensorName) {
        // All magnetometer params fall through
        case Magnetometer_x:
        case Magnetometer_y:
        case Magnetometer_z:
            return MAG_ADDRESS;
        
        // All remaining IMU params fall through
        case Gyroscope_x:
        case Gyroscope_y:
        case Gyroscope_z:
        case Accelerometer_x:
        case Accelerometer_y:
        case Accelerometer_z:
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


bool TSLPB::write8bitRegister(TSLPB_I2CAddress_t i2cAddress, const uint8_t reg, uint8_t data)
{
    bool result;
    
    Wire.beginTransmission(i2cAddress);
    result =  Wire.write(reg);
    result &= Wire.write(data);
    Wire.endTransmission();
    return result;
}


/*!
 * @brief This private method returns the contents of a single I2C register (1 byte)
 *
 * @param[in]   i2cAddress  TSLPB Digital Sensor Address Enum (a uint8_t I2C address)
 * @param[in]   reg         Register (a uint8_t I2C register)
 *
 * @return      uint8_t     The contents of register reg at I2C device i2cAddress
 *
 */
uint8_t TSLPB::read8bitRegister(TSLPB_I2CAddress_t i2cAddress, const uint8_t reg)
{
    uint8_t regContents;

    Wire.beginTransmission(i2cAddress);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)i2cAddress, (uint8_t)1);
    regContents = Wire.read();
    return regContents;
}


/*!
 * @brief This private method assigns the bit-pattern of a specified I2C
 * register to the variable "response"
 *
 * @param[in]   i2cAddress  TSLPB Digital Sensor Enum (a uint8_t I2C address)
 * @param[in]   reg         LM75A Register Selection Enum (a uint8_t I2C register)
 * @param[out]  response    The variable to which the register contents will be
 *                          assigned
 * @return      true or false read success.
 * @warning     This method DOES NOT implement error checking and always returns TRUE
 */
bool TSLPB::read16bitRegister(TSLPB_I2CAddress_t i2cAddress, const uint8_t reg, uint16_t& response)
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


void TSLPB::sleepUntilClearToSend() { 
    
}


void TSLPB::sleepWithWakeOnSerialReady() {
    // Use SLEEP_MODE_IDLE (lowest power savings, but safest?)
    Serial.flush(); // Prior to entering sleep, just to be safe
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
//    attachInterrupt(digitalPinToInterrupt(TSL_SERIAL_STATUS_PIN), wakeOnSerialReady, NSL_SERIAL_READY);
    sleep_mode();
    sleep_disable();
    detachInterrupt(digitalPinToInterrupt(TSL_SERIAL_STATUS_PIN));
}








void TSLPB::waitForMagReady()
{
    uint32_t startTime = millis();
    byte dataReady = false;
    while (millis() - startTime < TSL_SENSOR_READY_TIMEOUT) { // Timeout value hardcoded
        dataReady = read8bitRegister(MAG_ADDRESS, MPU9250_MAG_REG_STATUS_1);
        dataReady = (dataReady & MAG_MASK_DATA_READY) > 1;
        if (dataReady)
            return;
    }
}

void TSLPB::wakeOnSerialReady() { };

/*!
 * @brief This function sends the user's payload data to NSL Mothership over the
 * serial line. This function expects a ThinsatPacket_t union as an argument.
 * That data type is defined in ThinSat_DataPacket.h, and the contents of the
 * user data structure may be customized.
 *
 * @param[in]   data    A ThinsatPacket_t union.
 *
 * @return      nominal transmission: true or false
 */
bool TSLPB::pushDataToNSL(ThinsatPacket_t data) {
    
    char header[] = NSL_PACKET_HEADER;
    strcpy(data.payloadData.header, header);
    
    int bytesWritten;
    bytesWritten = Serial.write( (char*)data.NSLPacket, sizeof(data.payloadData) );
    if (bytesWritten == sizeof(data.NSLPacket)) {
        return true;
    } else {
        return false;
    }
}



/*!
 * @brief This function returns true if the NSL Mothership is ready to receive
 * data over the serial line.
 *
 * @return      true or false
 */
bool TSLPB::isClearToSend()
{
    if (digitalRead(TSL_SERIAL_STATUS_PIN) == NSL_SERIAL_READY) {
        return true;
    } else {
        return false;
    }
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
 *
 *  You probably noticed the "Voltage", "Current", etc arguments. The TSLPB
 *  driver has two enums that allow the client to call the read methods with
 *  human-readable code, and without worrying about keeping I2C addresses or
 *  managing low-level mux switching.
 *
 *  - ::TSLPB_AnalogSensor_t
 *  - ::TSLPB_DigitalSensor_t
 */

