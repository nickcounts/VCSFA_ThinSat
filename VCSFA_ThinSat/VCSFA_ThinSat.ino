//
// VCSFA_ThinSat
//
// Description of the project
// Developed with [embedXcode](http://embedXcode.weebly.com)
//
// Author 		Nicholas Counts
// 				Counts Engineering
//
// Date			6/11/18 9:56 PM
// Version		<#version#>
//
// Copyright	© Nicholas Counts, 2018
// Licence		<#licence#>
//
// See         ReadMe.txt for references
//


// Core library for code-sense - IDE-based
// !!! Help: http://bit.ly/2AdU7cu
#if defined(ENERGIA) // LaunchPad specific
#include "Energia.h"
#elif defined(TEENSYDUINO) // Teensy specific
#include "Arduino.h"
#elif defined(ESP8266) // ESP8266 specific
#include "Arduino.h"
#elif defined(ARDUINO) // Arduino 1.8 specific
#include "Arduino.h"
#else // error
#error Platform not defined
#endif // end IDE

// Set parameters

// Includes and prototypes to make xCode happy

#include "HardwareSerial.h"
extern HardwareSerial Serial;



//


// Include application, user and local libraries

#include "Adafruit_BNO055.h"
#include "imumaths.h"

#include "Adafruit_BMP280.h"

#include "SparkFunMPU9250-DMP.h"

#include "NSL_ThinSat.h"
#include "TSLPB.h"


// Define structures and classes

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11
#define BMP_CS 10

// Define variables and constants

typedef struct sensorData_t{
    char            header[NSL_PACKET_HEADER_LENGTH];
    int16_t         quatw;      //  1 -  3 (value from -1000 to 1000) 1.000 (unitless)
    int16_t         quatx;      //  4 -  5 (value from -1000 to 1000) 1.000 (unitless)
    int16_t         quaty;      //  6 -  7 (value from -1000 to 1000) 1.000 (unitless)
    int16_t         quatz;      //  8 -  9 (value from -1000 to 1000) 1.000 (unitless)
    int16_t         magx;       // 10 - 11 (value from -20480 to 20470) 2047.0 uT
    int16_t         magy;       // 12 - 13 (value from -20480 to 20470) 2047.0 uT
    int16_t         magz;       // 14 - 15 (value from -20480 to 20470) 2047.0 uT
    uint8_t         bnoCal;     // 16      (sys, gyro, accel, mag) 01010101b
    unsigned long   bmePres;    // 17 - 20 (values from 0 to 1010000) 101000.0 Pa
    int16_t         bmeTemp;    // 21 - 22 (values from -1000 to 1000) 100.0 C
    uint16_t        tslTempExt; // 23 - 24 (10 bits 0-1023) ADC Raw Counts
    uint16_t        tslVolts;   // 25 - 26 (10 bits 0-1023) ADC Raw Counts
    uint16_t        tslCurrent; // 27 - 28 (10 bits 0-1023) ADC Raw Counts
    uint16_t        unused1;    // 29 - 30
    uint16_t        unused2;    // 31 - 32
    uint16_t        unused3;    // 33 - 34
    uint16_t        unused4;    // 35 - 36
    uint8_t         unused5;    // 37 - 38
};

typedef union ThinsatPacket_t {
    sensorData_t sensor;
    byte ThinsatPacket[sizeof(sensorData_t)];
};

Adafruit_BMP280 bme; // I2C Constructor
Adafruit_BNO055 bno = Adafruit_BNO055();

TSLPB tslpb;


sensorData_t mySensorData;


// Prototypes


// Utilities


// Functions


// Add setup code
void setup()
{
    Serial.begin(NSL_BAUD_RATE);
    
    tslpb.begin();
    
    bno.begin();
    delay(250);
    bno.setExtCrystalUse(true);
    
    bme.begin();
    
    
}

// Add loop code
void loop()
{
    /*

     3. Poll Sensors
        * Quaternion data (w, x, y, z): 3 sig figs
        * Magnetometer data (x, y, z): 3-4 sig figs
        * Pressure data: it’s taken in Pascals so we’ll probably need at least
                    (5) sig figs maybe 6 …
        * Temperature: 3 sig figs should be enough, but i’d like to have (1)
                    from TSL and (1) from either BMP or BNO just for
                    comparison’s sake.
     */
    
    imu::Quaternion tempQuat = bno.getQuat(); // returns double types
    
    mySensorData.quatw = (int16_t)(tempQuat.w() * 1000); // store as integer
    mySensorData.quatx = (int16_t)(tempQuat.x() * 1000); // convert by divide
    mySensorData.quaty = (int16_t)(tempQuat.y() * 1000); // by 1000
    mySensorData.quatz = (int16_t)(tempQuat.z() * 1000);
    
    
    imu::Vector<3> magVect = bno.getVector(bno.VECTOR_MAGNETOMETER);

    mySensorData.magy = (int16_t)(magVect[0] * 10); // Stored as integer.
    mySensorData.magz = (int16_t)(magVect[1] * 10); // Convert by dividing by
    mySensorData.magx = (int16_t)(magVect[2] * 10); // 10 (one decimal place)
    
    uint8_t sysCal, gyroCal, accelCal, magCal;
    bno.getCalibration(&sysCal, &gyroCal, &accelCal, &magCal);
    mySensorData.bnoCal =   ((sysCal & 0x3) << 6)   | ((gyroCal & 0x3) << 4) |
                            ((accelCal & 0x3) << 2) | (magCal & 0x3);
    
    mySensorData.tslVolts = tslpb.readAnalogSensor(Voltage);
    mySensorData.tslCurrent = tslpb.readAnalogSensor(Current);
    mySensorData.tslTempExt = tslpb.readAnalogSensor(TempExt);

    mySensorData.bmePres = (unsigned long)(bme.readPressure() * 10);
    mySensorData.bmeTemp = (int16_t)(bme.readTemperature() * 10);
    
    Serial.print("DT1 Raw Count: ");
    Serial.println(tslpb.readTSLDigitalSensorRaw(DT1), BIN);
    
    Serial.print("DT1 With Unit: ");
    Serial.println(tslpb.readTSLDigitalSensor(DT1));

    delay(1000);
    
    /*
     4. Store data
     5. Wait for CTS
        * Interrupt or while() loop?
     6. Push data to NSL board
     7. Deep sleep unitl next time (10 seconds from last poll?)
     8. Back to 3. Poll Sensors
     */
    
}

/*
 1. Power On
 2. Initialize Sensors
 * Initialize Serial connection
 * Initialize BNO 9DOF Sensor
 * Initialize BME Weather Sensor
 3. Poll Sensors
 * Quaternion data (w, x, y, z): 3 sig figs
 * Magnetometer data (x, y, z): 3-4 sig figs
 * Pressure data: it’s taken in Pascals so we’ll probably need at least (5) sig figs maybe 6 …
 * Temperature: 3 sig figs should be enough, but i’d like to have (1) from TSL and (1) from either BMP or BNO just for comparison’s sake.
 4. Store data
 5. Wait for CTS
 * Interrupt or while() loop?
 6. Push data to NSL board
 7. Deep sleep unitl next time (10 seconds from last poll?)
 8. Back to 3. Poll Sensors
 */
