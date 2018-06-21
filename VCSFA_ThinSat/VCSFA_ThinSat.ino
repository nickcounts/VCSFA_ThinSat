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

/*  ┌──────────────────────────────────────────────────┐
 *  │       Disregard everything above this line:      │
 *  │   It is needed to keep CodeSense happy in Xcode  │
 *  └──────────────────────────────────────────────────┘ */





/*  ┌──────────────────────────────────────────────────┐
 *  │  Include Twiggs Space Lab Payload Board Library  │
 *  └──────────────────────────────────────────────────┘ */

#include "TSLPB.h"

/*  ┌──────────────────────────────────────────────────┐
 *  │          Include custom sensor libraries         │
 *  └──────────────────────────────────────────────────┘ */


#include "Adafruit_BNO055.h"
#include "imumaths.h"
#include "Adafruit_BMP280.h"



/*  ┌──────────────────────────────────────────────────┐
 *  │        Define SPI Pins for custom sensors        │
 *  └──────────────────────────────────────────────────┘ */

    /*
     * These BMP_ definitions might not be required?
     * Try commenting them out and see if the program still works
     * They aren't really hurting anything, I just don't know if they
     * could interfere with the actual BMP library?
     */

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11
#define BMP_CS 10


/*  ┌──────────────────────────────────────────────────┐
 *  │   Instantiate Controller Classes and Variables   │
 *  └──────────────────────────────────────────────────┘ */


Adafruit_BMP280 bme; // I2C Constructor
Adafruit_BNO055 bno = Adafruit_BNO055();

TSLPB tslpb;

ThinsatPacket_t missionData;


/*  ┌──────────────────────────────────────────────────┐
 *  │  Setup Function: Run any custom initializations  │
 *  └──────────────────────────────────────────────────┘ */

void setup()
{
    Serial.begin(NSL_BAUD_RATE);
    
    tslpb.begin();
    
    bno.begin();
    delay(250);
    bno.setExtCrystalUse(true);
    
    bme.begin();
    
    
}

/*  ┌──────────────────────────────────────────────────┐
 *  │             Main Flight Software Loop            │
 *  └──────────────────────────────────────────────────┘ */

void loop()
{
    
    /*  ┌──────────────────────────────────────────────────┐
     *  │       Poll Sensors and store in missionData      │
     *  └──────────────────────────────────────────────────┘ */
    
    
    /*  ┌──────────────────────────────────────────────────┐
     *  │          Get BNO Gyro/Mag Data and Store         │
     *  └──────────────────────────────────────────────────┘ */
    
    imu::Quaternion tempQuat = bno.getQuat(); // returns double types
    
    missionData.payloadData.quatw = (int16_t)(tempQuat.w() * 1000); // store as integer
    missionData.payloadData.quatx = (int16_t)(tempQuat.x() * 1000); // convert by divide
    missionData.payloadData.quaty = (int16_t)(tempQuat.y() * 1000); // by 1000
    missionData.payloadData.quatz = (int16_t)(tempQuat.z() * 1000);
    
    imu::Vector<3> magVect;
    magVect = bno.getVector(bno.VECTOR_MAGNETOMETER);


    missionData.payloadData.bnomagy = (int16_t)(magVect[0] * 10); // Stored as integer.
    missionData.payloadData.bnomagz = (int16_t)(magVect[1] * 10); // Convert by dividing by
    missionData.payloadData.bnomagx = (int16_t)(magVect[2] * 10); // 10 (one decimal place)
    
    uint8_t sysCal, gyroCal, accelCal, magCal;
    bno.getCalibration(&sysCal, &gyroCal, &accelCal, &magCal);
    missionData.payloadData.bnoCal =   ((sysCal & 0x3) << 6)   | ((gyroCal & 0x3) << 4) | ((accelCal & 0x3) << 2) | (magCal & 0x3);
    
    /*  ┌──────────────────────────────────────────────────┐
     *  │         Get BME280 Weather Data and Store        │
     *  └──────────────────────────────────────────────────┘ */
    
    missionData.payloadData.bmePres = (unsigned long)(bme.readPressure() * 10);
    missionData.payloadData.bmeTemp = (int16_t)(bme.readTemperature() * 10);
    
    
    /*  ┌──────────────────────────────────────────────────┐
     *  │         Get Payload Health Data and Store        │
     *  └──────────────────────────────────────────────────┘ */
    
    missionData.payloadData.tslVolts   = tslpb.readAnalogSensor(Voltage);
    missionData.payloadData.tslCurrent = tslpb.readAnalogSensor(Current);
    missionData.payloadData.tslTempExt = tslpb.readAnalogSensor(TempExt);

    
    /*  ┌──────────────────────────────────────────────────┐
     *  │        Get TSL Magnetometer Data and Store       │
     *  └──────────────────────────────────────────────────┘ */
    
    missionData.payloadData.tslMagXraw = tslpb.readDigitalSensorRaw(Magnetometer_x);
    missionData.payloadData.tslMagYraw = tslpb.readDigitalSensorRaw(Magnetometer_y);
    missionData.payloadData.tslMagZraw = tslpb.readDigitalSensorRaw(Magnetometer_z);
    
    /*  ┌──────────────────────────────────────────────────┐
     *  │          Get the TSL Solar Sensor Value          │
     *  └──────────────────────────────────────────────────┘ */
    
    missionData.payloadData.solar = tslpb.readAnalogSensor(Solar);
   
    
    /*  ┌──────────────────────────────────────────────────┐
     *  │             Wait for Clear To Send?              │
     *  └──────────────────────────────────────────────────┘ */
    
    while (!tslpb.isClearToSend())
    {
        delay(100);
    }
    
    /*  ┌──────────────────────────────────────────────────┐
     *  │          Push Data to the NSL Mothership         │
     *  └──────────────────────────────────────────────────┘ */
    
    tslpb.pushDataToNSL(missionData);
    
    
    /*  ┌──────────────────────────────────────────────────┐
     *  │    Wait for some predetermined amount of time    │
     *  └──────────────────────────────────────────────────┘ */
    
            /*
             * We could have it wait some fixed amount of time
             * or we could have it try on fixed intervals by
             * looking at the internal timer...
             */
    
    delay(5000);
    
    
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
