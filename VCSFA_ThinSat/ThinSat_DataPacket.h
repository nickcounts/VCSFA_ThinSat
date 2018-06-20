//
//  ThinSat_DataPacket.h
//  VCSFA_ThinSat
//
//  Created by Nicholas Counts on 6/20/18.
//  Copyright © 2018 Counts Engineering. All rights reserved.
//

#ifndef ThinSat_DataPacket_h
#define ThinSat_DataPacket_h



typedef struct UserDataStruct_t{
    char            header[NSL_PACKET_HEADER_LENGTH];
    int16_t         quatw;      //  1 -  3 (value from -1000 to 1000) 1.000 (unitless)
    int16_t         quatx;      //  4 -  5 (value from -1000 to 1000) 1.000 (unitless)
    int16_t         quaty;      //  6 -  7 (value from -1000 to 1000) 1.000 (unitless)
    int16_t         quatz;      //  8 -  9 (value from -1000 to 1000) 1.000 (unitless)
    int16_t         bnomagx;       // 10 - 11 (value from -20480 to 20470) 2047.0 uT (from BNO)
    int16_t         bnomagy;       // 12 - 13 (value from -20480 to 20470) 2047.0 uT
    int16_t         bnomagz;       // 14 - 15 (value from -20480 to 20470) 2047.0 uT
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
    UserDataStruct_t payloadData;
    byte NSLPacket[sizeof(UserDataStruct_t)];
};

#endif /* ThinSat_DataPacket_h */
