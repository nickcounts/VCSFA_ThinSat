//
//  NSL_ThinSat.h
//  Index
//
//  Created by Nicholas Counts on 6/12/18.
//  Copyright © 2018 Counts Engineering. All rights reserved.
//

#ifndef NSL_ThinSat_h
#define NSL_ThinSat_h



#define NSL_PACKET_SIZE 38  // Total bytes in the TSL Payload Packet
#define NSL_PACKET_HEADER_LENGTH 3  // Total Bytes
#define NSL_PACKET_HEADER {0x50, 0x50, 0x50}



#define NSL_BAUD_RATE 38400         // From ETSat_Payload_ICD_v5.9.pdf page 10

#define NSL_SERIAL_READY    LOW
#define NSL_SERIAL_BUSY     HIGH

enum NSLSerialStatus
{
    READY=0,
    BUSY
};


#endif /* NSL_ThinSat_h */
