/**
 *  @file   ThinSat_DataPacket.h
 *  @author Nicholas Counts
 *  @date   06/20/18
 *  @brief  Defines the custom data structure used to store the user's payload
 *          data, and the union that is used to transmit the data to the NSL
 *          Mothership.
 *
 */

/* 2018 Counts Engineering */


#ifndef ThinSat_DataPacket_h
#define ThinSat_DataPacket_h


/*!
 * @brief   A user-customizable structure to hold any data the user intends to
 *          send back to Earth.
 *
 * @note    This is a sample UserDataStruct_t. It was developed for the VCSFA
 *          ThinSat custom payload. Some of the fields are for external sensors
 *          and some of the fields are for TSLPB sensors.
 *
 * @note    We recommend adding comments that show the expected ranges and units
 *          of any data being put into a field. This will ensure that you can
 *          translate the data later.
 *
 * @warning The struct must be NSL_PACKET_SIZE bytes in total size.
 *          The first member must always be called "header" and have a size of
 *          NSL_PACKET_HEADER_LENGTH
 *
 */
typedef struct UserDataStruct_t{
    char            header[NSL_PACKET_HEADER_LENGTH];
    int16_t         quatw;      ///<  1 -  3 (value from -1000 to 1000) 1.000 (unitless)
    int16_t         quatx;      ///<  4 -  5 (value from -1000 to 1000) 1.000 (unitless)
    int16_t         quaty;      ///<  6 -  7 (value from -1000 to 1000) 1.000 (unitless)
    int16_t         quatz;      ///<  8 -  9 (value from -1000 to 1000) 1.000 (unitless)
    int16_t         bnomagx;    ///< 10 - 11 (value from -20480 to 20470) 2047.0 uT (from BNO)
    int16_t         bnomagy;    ///< 12 - 13 (value from -20480 to 20470) 2047.0 uT
    int16_t         bnomagz;    ///< 14 - 15 (value from -20480 to 20470) 2047.0 uT
    uint8_t         bnoCal;     ///< 16      (sys, gyro, accel, mag) 01010101b
    unsigned long   bmePres;    ///< 17 - 20 (values from 0 to 1010000) 101000.0 Pa
    int16_t         bmeTemp;    ///< 21 - 22 (values from -1000 to 1000) 100.0 C
    uint16_t        tslTempExt; ///< 23 - 24 (10 bits 0-1023) ADC Raw Counts
    uint16_t        tslVolts;   ///< 25 - 26 (10 bits 0-1023) ADC Raw Counts
    uint16_t        tslCurrent; ///< 27 - 28 (10 bits 0-1023) ADC Raw Counts
    int16_t         tslMagXraw; ///< 29 - 30 Raw value (2's compliment form) -0x7FF8 to 0x7FF8
    int16_t         tslMagYraw; ///< 31 - 32 Raw value (2's compliment form) -0x7FF8 to 0x7FF8
    int16_t         tslMagZraw; ///< 33 - 34 Raw value (2's compliment form) -0x7FF8 to 0x7FF8
    uint8_t         unused5;    ///< 35      Left over for status bits?
};


/*!
 * @brief   A union of the UserDataStruct_t payloadData and a byte array that is
 *          used to send the user's mission data to the NSL Mothership.
 *
 * @warning DO NOT MODIFY THIS UNION UNLESS YOU REALLY REALLY KNOW WHAT YOU ARE
 *          DOING. This datatype is used in the public method
 *          TSLPB::pushDataToNSL(ThinsatPacket_t data) and changing this union
 *          may break that functionality.
 */
typedef union ThinsatPacket_t {
    UserDataStruct_t payloadData;
    byte NSLPacket[sizeof(UserDataStruct_t)];
};

#endif /* ThinSat_DataPacket_h */
