/**
 *  @file   NSL_ThinSat.h
 *  @author Nicholas Counts
 *  @date   06/12/18
 *  @brief  Function prototypes, includes, and definitions for NSL to TSLPB
 *          Arduino interface
 *
 */

 /* 2018 Counts Engineering */

#ifndef NSL_ThinSat_h
#define NSL_ThinSat_h


#define NSL_PACKET_SIZE 38                      ///< Total bytes in the TSL Payload Packet
#define NSL_PACKET_HEADER_LENGTH 3              ///< Total Bytes
#define NSL_PACKET_HEADER {0x50, 0x50, 0x50}    ///< The 3 byte preabmpe to NSL Payload Packets
#define NSL_BAUD_RATE 38400         ///< From ETSat_Payload_ICD_v5.9.pdf page 10

#define NSL_SERIAL_ACK  {0xAA, 0x05, 0x00}
#define NSL_SERIAL_NAK  {0xAA, 0x05, 0xFF}

#define NSL_SERIAL_READY    LOW     ///< The NSL Mothership is able to receive a payload data packet.
#define NSL_SERIAL_BUSY     HIGH    ///< The NSL Mothership is unable to receive a payload data packet.

//typedef enum
//{
//    NSL_SERIAL_READY    = LOW,  ///< The NSL Mothership is able to receive a payload data packet.
//    NSL_SERIAL_BUSY     = HIGH  ///< The NSL Mothership is unable to receive a payload data packet.
//
//} NSLSerialStatus_t;



#endif /* NSL_ThinSat_h */
