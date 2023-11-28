/** \file can.h */
/*
 * NAME: can.h
 *
 * WHAT:
 *  Header file for CAN defines.
 *
 * SPECIAL CONSIDERATIONS:
 *  Derived from: 'can.h' in "autowp-mcp2515" Arduino CAN library
 *
 * AUTHOR:
 *  D.L. Karmann
 *
 */
#ifndef CAN_H_
#define CAN_H_

/// special address description flags for the CAN_ID
#define CAN_EFF_FLAG 0x80000000UL   // EFF/SFF is set in the MSB
#define CAN_RTR_FLAG 0x40000000UL   // remote transmission request
#define CAN_ERR_FLAG 0x20000000UL   // error message frame

/// valid bits in CAN ID for frame formats
#define CAN_SFF_MASK 0x000007FFUL   // standard frame format (SFF)
#define CAN_EFF_MASK 0x1FFFFFFFUL   // extended frame format (EFF)
#define CAN_ERR_MASK 0x1FFFFFFFUL   // omit EFF, RTR, ERR flags

/**
 * Controller Area Network Identifier structure
 *
 * bit 0-28 : CAN identifier (11/29 bit) \n
 * bit 29   : error message frame flag (0 = data frame, 1 = error message) \n
 * bit 30   : remote transmission request flag (1 = rtr frame) \n
 * bit 31   : frame format flag (0 = standard 11 bit, 1 = extended 29 bit) \n
\code
      31    30    29    28                           11     10                        0 
    --------------------------------------------------------------------------------------
    | EFF | RTR | ERR | ID28 ...................... ID11 | ID10 .................... ID0 |
    --------------------------------------------------------------------------------------
                      |<---------------- EID ----------->|<------------- ID ------------>|

\endcode
 */
typedef uint32_t canid_t;

#define CAN_SFF_ID_BITS     11
#define CAN_EFF_ID_BITS     29

/// CAN payload length and DLC definitions according to ISO 11898-1
#define CAN_MAX_DLC     8
#define CAN_MAX_DLEN    8

/// CAN frame
typedef struct can_frame
{
    /// 32 bit CAN_ID + EFF/RTR/ERR flags
    canid_t can_id;
    /// frame payload length in bytes (0 .. CAN_MAX_DLEN)
    uint8_t can_dlc;
    /// Rx buffer number of received CAN message
    uint8_t can_rxb;
    /// CAN data of received CAN message
    uint8_t can_data[CAN_MAX_DLEN + 1];
} CAN_FRAME;

#endif /* CAN_H_ */
