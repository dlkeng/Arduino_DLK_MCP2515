/** \file MCP2515.h */
/*
 * NAME: MCP2515.h
 *
 * WHAT:
 *  Header file for MCP2515 defines.
 *
 * SPECIAL CONSIDERATIONS:
 *  Derived from: DFRobot_MCP2515.h in DFRobot_MCP2515 library
 *
 * AUTHOR:
 *  D.L. Karmann
 *
 */
#ifndef __MCP2515_H__
#define __MCP2515_H__

#if 1

// ID indexes
#define MCP2515_SIDH            0
#define MCP2515_SIDL            1
#define MCP2515_EID8            2
#define MCP2515_EID0            3

// Buffer offsets
#define MCP2515_BUF_SIDH        1
#define MCP2515_BUF_SIDL        2
#define MCP2515_BUF_EID8        3
#define MCP2515_BUF_EID0        4
#define MCP2515_BUF_DLC         5
#define MCP2515_BUF_DATA0       6

#define MCP2515_N_RXBUFFERS     2
#define MCP2515_N_TXBUFFERS     3
#define MCP2515_N_MASKS         2
#define MCP2515_N_FILTERS       6

/// IDE: Extended Identifier Flag bit (RXBnSIDL: RECEIVE BUFFER n STANDARD IDENTIFIER REGISTER LOW)
#define MCP2515_RXB_IDE         0x08

/// DLC[3:0]: Data Length Code bits (RXBnDLC: RECEIVE BUFFER n DATA LENGTH CODE REGISTER)
#define MCP2515_DLC_MASK        0x0F

/// RTR: Remote Transmission Request bit (TXBnDLC: TRANSMIT BUFFER n DATA LENGTH CODE REGISTER) \n
/// RTR: Extended Frame Remote Transmission Request bit (RXBnDLC: RECEIVE BUFFER n DATA LENGTH CODE REGISTER)
#define MCP2515_RTR_MASK        0x40

#define MCP2515_RXB_RX_ANY      0x60
#define MCP2515_RXB_RX_EXT      0x40
#define MCP2515_RXB_RX_STD      0x20
#define MCP2515_RXB_RX_STDEXT   0x00
#define MCP2515_RXB_RX_MASK     0x60
#define MCP2515_RXB_BUKT_MASK   (1 << 2)

#define MCP2515_TXB_TXBUFE_M    0x80
#define MCP2515_TXB_ABTF_M      0x40
#define MCP2515_TXB_MLOA_M      0x20
#define MCP2515_TXB_TXERR_M     0x10
#define MCP2515_TXB_TXREQ_M     0x08
#define MCP2515_TXB_TXIE_M      0x04
#define MCP2515_TXB_TXP10_M     0x03

#define MCP2515_TXB_RTR_M       0x40
#define MCP2515_RXB_IDE_M       0x08
#define MCP2515_RXB_RTR_M       0x40

#define MCP2515_STAT_RXIF_MASK  (0x03)
#define MCP2515_STAT_RX0IF      (1 << 0)
#define MCP2515_STAT_RX1IF      (1 << 1)

#define MCP2515_EFLG_RX1OVR     (1 << 7)
#define MCP2515_EFLG_RX0OVR     (1 << 6)
#define MCP2515_EFLG_TXBO       (1 << 5)
#define MCP2515_EFLG_TXEP       (1 << 4)
#define MCP2515_EFLG_RXEP       (1 << 3)
#define MCP2515_EFLG_TXWAR      (1 << 2)
#define MCP2515_EFLG_RXWAR      (1 << 1)
#define MCP2515_EFLG_EWARN      (1 << 0)
#define MCP2515_EFLG_ERRORMASK  (0xF8)

#if 1   // Registers Addresses
#define MCP2515_RXF0SIDH        0x00
#define MCP2515_RXF0SIDL        0x01
#define MCP2515_RXF0EID8        0x02
#define MCP2515_RXF0EID0        0x03
#define MCP2515_RXF1SIDH        0x04
#define MCP2515_RXF1SIDL        0x05
#define MCP2515_RXF1EID8        0x06
#define MCP2515_RXF1EID0        0x07
#define MCP2515_RXF2SIDH        0x08
#define MCP2515_RXF2SIDL        0x09
#define MCP2515_RXF2EID8        0x0A
#define MCP2515_RXF2EID0        0x0B
#define MCP2515_BFPCTRL         0x0C
#define MCP2515_TXRTSCTRL       0x0D
#define MCP2515_CANSTAT         0x0E
#define MCP2515_CANCTRL         0x0F

#define MCP2515_RXF3SIDH        0x10
#define MCP2515_RXF3SIDL        0x11
#define MCP2515_RXF3EID8        0x12
#define MCP2515_RXF3EID0        0x13
#define MCP2515_RXF4SIDH        0x14
#define MCP2515_RXF4SIDL        0x15
#define MCP2515_RXF4EID8        0x16
#define MCP2515_RXF4EID0        0x17
#define MCP2515_RXF5SIDH        0x18
#define MCP2515_RXF5SIDL        0x19
#define MCP2515_RXF5EID8        0x1A
#define MCP2515_RXF5EID0        0x1B
#define MCP2515_TEC             0x1C
#define MCP2515_REC             0x1D

#define MCP2515_RXM0SIDH        0x20
#define MCP2515_RXM0SIDL        0x21
#define MCP2515_RXM0EID8        0x22
#define MCP2515_RXM0EID0        0x23
#define MCP2515_RXM1SIDH        0x24
#define MCP2515_RXM1SIDL        0x25
#define MCP2515_RXM1EID8        0x26
#define MCP2515_RXM1EID0        0x27
#define MCP2515_CNF3            0x28
#define MCP2515_CNF2            0x29
#define MCP2515_CNF1            0x2A
#define MCP2515_CANINTE         0x2B
#define MCP2515_CANINTF         0x2C
#define MCP2515_EFLG            0x2D

#define MCP2515_TXB0CTRL        0x30
#define MCP2515_TXB0SIDH        0x31
#define MCP2515_TXB0SIDL        0x32
#define MCP2515_TXB0EID8        0x33
#define MCP2515_TXB0EID0        0x34
#define MCP2515_TXB0DLC         0x35
#define MCP2515_TXB0D0          0x36
#define MCP2515_TXB0D1          0x37
#define MCP2515_TXB0D2          0x38
#define MCP2515_TXB0D3          0x39
#define MCP2515_TXB0D4          0x3A
#define MCP2515_TXB0D5          0x3B
#define MCP2515_TXB0D6          0x3C
#define MCP2515_TXB0D7          0x3D

#define MCP2515_TXB1CTRL        0x40
#define MCP2515_TXB1SIDH        0x41
#define MCP2515_TXB1SIDL        0x42
#define MCP2515_TXB1EID8        0x43
#define MCP2515_TXB1EID0        0x44
#define MCP2515_TXB1DLC         0x45
#define MCP2515_TXB1D0          0x46
#define MCP2515_TXB1D1          0x47
#define MCP2515_TXB1D2          0x48
#define MCP2515_TXB1D3          0x49
#define MCP2515_TXB1D4          0x4A
#define MCP2515_TXB1D5          0x4B
#define MCP2515_TXB1D6          0x4C
#define MCP2515_TXB1D7          0x4D

#define MCP2515_TXB2CTRL        0x50
#define MCP2515_TXB2SIDH        0x51
#define MCP2515_TXB2SIDL        0x52
#define MCP2515_TXB2EID8        0x53
#define MCP2515_TXB2EID0        0x54
#define MCP2515_TXB2DLC         0x55
#define MCP2515_TXB2D0          0x56
#define MCP2515_TXB2D1          0x57
#define MCP2515_TXB2D2          0x58
#define MCP2515_TXB2D3          0x59
#define MCP2515_TXB2D4          0x5A
#define MCP2515_TXB2D5          0x5B
#define MCP2515_TXB2D6          0x5C
#define MCP2515_TXB2D7          0x5D

#define MCP2515_RXB0CTRL        0x60
#define MCP2515_RXB0SIDH        0x61
#define MCP2515_RXB0SIDL        0x62
#define MCP2515_RXB0EID8        0x63
#define MCP2515_RXB0EID0        0x64
#define MCP2515_RXB0DLC         0x65
#define MCP2515_RXB0D0          0x66
#define MCP2515_RXB0D1          0x67
#define MCP2515_RXB0D2          0x68
#define MCP2515_RXB0D3          0x69
#define MCP2515_RXB0D4          0x6A
#define MCP2515_RXB0D5          0x6B
#define MCP2515_RXB0D6          0x6C
#define MCP2515_RXB0D7          0x6D

#define MCP2515_RXB1CTRL        0x70
#define MCP2515_RXB1SIDH        0x71
#define MCP2515_RXB1SIDL        0x72
#define MCP2515_RXB1EID8        0x73
#define MCP2515_RXB1EID0        0x74
#define MCP2515_RXB1DLC         0x75
#define MCP2515_RXB1D0          0x76
#define MCP2515_RXB1D1          0x77
#define MCP2515_RXB1D2          0x78
#define MCP2515_RXB1D3          0x79
#define MCP2515_RXB1D4          0x7A
#define MCP2515_RXB1D5          0x7B
#define MCP2515_RXB1D6          0x7C
#define MCP2515_RXB1D7          0x7D
#endif

// CANINTE: CAN INTERRUPT ENABLE REGISTER
// CANINTF: CAN INTERRUPT FLAG REGISTER
#define MCP2515_TX_INT          0x1C
#define MCP2515_TX01_INT        0x0C
#define MCP2515_RX_INT          0x03
#define MCP2515_NO_INT          0x00

#define MCP2515_TX01_MASK       0x14
#define MCP2515_TX_MASK         0x54

#if 1   // MCP2515 Instruction Set
#define MCP2515_WRITE           0x02
#define MCP2515_READ            0x03
#define MCP2515_BITMOD          0x05
#define MCP2515_READ_STATUS     0xA0
#define MCP2515_RX_STATUS       0xB0
#define MCP2515_RESET           0xC0

#define MCP2515_LOAD_TX0H       0x40
#define MCP2515_LOAD_TX0        0x41
#define MCP2515_LOAD_TX1H       0x42
#define MCP2515_LOAD_TX1        0x43
#define MCP2515_LOAD_TX2H       0x44
#define MCP2515_LOAD_TX2        0x45

#define MCP2515_RTS_TX0         0x81
#define MCP2515_RTS_TX1         0x82
#define MCP2515_RTS_TX2         0x84
#define MCP2515_RTS_ALL         0x87

#define MCP2515_READ_RX0H       0x90
#define MCP2515_READ_RX0        0x92
#define MCP2515_READ_RX1H       0x94
#define MCP2515_READ_RX1        0x96
#endif

// MCP2515 STATUS INSTRUCTION
#define STAT_TX2IF      0x80
#define STAT_TX2REQ     0x40
#define STAT_TX1IF      0x20
#define STAT_TX1REQ     0x10
#define STAT_TX0IF      0x08
#define STAT_TX0REQ     0x04
#define STAT_RX1IF      0x02
#define STAT_RX0IF      0x01

// MCP2515 RX STATUS INSTRUCTION
#define RXM_NO_MSG      0x00
#define RXM_RXB0_MSG    0x40
#define RXM_RXB1_MSG    0x80
#define RXM_RXBOTH_MSG  0xc0
#define RXMS_MASK       RXM_RXBOTH_MSG

#define MTYP_STD_DATA   0x00
#define MTYP_STD_REM    0x08
#define MTYP_EXT_DATA   0x10
#define MTYP_EXT_REM    0x18
#define MTYP_MASK       MTYP_EXT_REM

#define FM_RXF0         0x00
#define FM_RXF1         0x01
#define FM_RXF2         0x02
#define FM_RXF3         0x03
#define FM_RXF4         0x04
#define FM_RXF5         0x05
#define FM_RXF0_RXB1    0x06
#define FM_RXF1_RXB1    0x07
#define FM_MASK         FM_RXF1_RXB1

// OPMOD[2:0]: Operation Mode bits (CANSTAT: CAN STATUS REGISTER)
// REQOP[2:0]: Request Operation Mode bits (CANSTAT: CAN STATUS REGISTER)
#define MODE_NORMAL     0x00
#define MODE_SLEEP      0x20
#define MODE_LOOPBACK   0x40
#define MODE_LISTENONLY 0x60
#define MODE_CONFIG     0x80
#define MODE_POWERUP    MODE_CONFIG
#define MODE_MASK       0xE0

// ICOD[2:0]: Interrupt Flag Code bits (CANSTAT: CAN STATUS REGISTER)
#define NO_INT          0x00
#define ERROR_INT       0x02
#define WAKE_UP_INT     0x04
#define TXB0_INT        0x06
#define TXB1_INT        0x08
#define TXB2_INT        0x0a
#define RXB0_INT        0x0c
#define RXB1_INT        0x0e
#define INT_MASK        RXB1_INT

/// ABAT: Abort All Pending Transmissions bit (CANCTRL: CAN CONTROL REGISTER)
#define ABORT_TX        0x10

/// OSM: One-Shot Mode bit (CANCTRL: CAN CONTROL REGISTER)
#define MODE_ONESHOT    0x08

// CLKEN: CLKOUT Pin Enable bit (CANCTRL: CAN CONTROL REGISTER)
#define CLKOUT_ENABLE   0x04
#define CLKOUT_DISABLE  0x00

// CLKPRE[1:0]: CLKOUT Pin Prescaler bits (CANCTRL: CAN CONTROL REGISTER)
#define CLKOUT_PS1      0x00       // System Clock/1
#define CLKOUT_PS2      0x01       // System Clock/2
#define CLKOUT_PS4      0x02       // System Clock/4
#define CLKOUT_PS8      0x03       // System Clock/8
#define CLKOUT_MASK     CLKOUT_PS8

// BFPCTRL: RXnBF PIN CONTROL AND STATUS REGISTER
#define B1BFS_BIT       0x20
#define B0BFS_BIT       0x10
#define B1BFE_BIT       0x08
#define B0BFE_BIT       0x04
#define B1BFM_BIT       0x02
#define B0BFM_BIT       0x01

// TXRTSCTRL: TXnRTS PIN CONTROL AND STATUS REGISTER
#define B2RTS_BIT       0x20
#define B1RTS_BIT       0x10
#define B0RTS_BIT       0x08
#define B2RTSM_BIT      0x04
#define B1RTSM_BIT      0x02
#define B0RTSM_BIT      0x01

// TXBnCTRL: TRANSMIT BUFFER n CONTROL REGISTER
#define TXB_ABTF_BIT    0x40
#define TXB_MLOA_BIT    0x20
#define TXB_TXERR_BIT   0x10
#define TXB_TXREQ_BIT   0x08
#define TXB_TXP10_MASK  0x03

// TXP[1:0]: Transmit Buffer Priority bits (TXBnCTRL: TRANSMIT BUFFER n CONTROL REGISTER)
#define TXP_P3          0x03
#define TXP_P2          0x02
#define TXP_P1          0x01
#define TXP_P0          0x00
#define TXP_MASK        TXP_P3

// RXM[1:0]: Receive Buffer Operating mode bits (RXBnCTRL: RECEIVE BUFFER n CONTROL REGISTER)
#define RXM_M3          0x60        // disable all masks/filters to receive any message
#define RXM_M0          0x00        // receive messages meeting masks/filters criteria
#define RXM_MASK        RXM_M3

// RXRTR: Received Remote Transfer Request bit (RXB0CTRL: RECEIVE BUFFER 0 CONTROL REGISTER)
#define RXRTR_BIT       0x08

// BUKT: Rollover Enable bit (RXBnCTRL: RECEIVE BUFFER n CONTROL REGISTER)
#define BUKT_BIT        0x04

// BUKT1: Rollover Enable bit copy (RXB0CTRL: RECEIVE BUFFER 0 CONTROL REGISTER)
#define BUKT1_BIT       0x02

// FILHIT0: Filter Hit bit (RXB0CTRL: RECEIVE BUFFER 0 CONTROL REGISTER)
#define FILHIT0_BIT     0x01

// FILHIT[2:0]: Filter Hit bits (RXB1CTRL: RECEIVE BUFFER 1 CONTROL REGISTER)
#define FILHIT_RXF5     0x05
#define FILHIT_RXF4     0x04
#define FILHIT_RXF3     0x03
#define FILHIT_RXF2     0x02
#define FILHIT_RXF1     0x01
#define FILHIT_RXF0     0x00
#define FILHIT_MASK     0x07

// SJW[1:0]: Synchronization Jump Width Length bits (CNF1: CONFIGURATION REGISTER 1)
#define SJW1            0x00
#define SJW2            0x40
#define SJW3            0x80
#define SJW4            0xC0
#define SJW_MASK        SJW4

/// BRP[5:0]: Baud Rate Prescaler bits (CNF1: CONFIGURATION REGISTER 1)
#define BRP_BITS        0x3f

/// BTLMODE: PS2 Bit Time Length bit (CNF2: CONFIGURATION REGISTER 2)
#define BTLMODE         0x80

// SAM: Sample Point Configuration bit (CNF2: CONFIGURATION REGISTER 2)
#define SAMPLE_1X       0x00
#define SAMPLE_3X       0x40

/// PHSEG1: PS1 Length bits (CNF2: CONFIGURATION REGISTER 2)
#define PHSEG1_BITS     0x38

/// PRSEG: Propagation Segment Length bits (CNF2: CONFIGURATION REGISTER 2)
#define PRSEG_BITS      0x07

// SOF: Start-of-Frame signal bit (CNF3: CONFIGURATION REGISTER 3)
#define SOF_ENABLE      0x80
#define SOF_DISABLE     0x00

// WAKFIL: Wake-up Filter bit (CNF3: CONFIGURATION REGISTER 3)
#define WAKFIL_ENABLE   0x40
#define WAKFIL_DISABLE  0x00

/// PHSEG2: PS2 Length bits (CNF3: CONFIGURATION REGISTER 3)
#define PHSEG2_BITS     0x07

// CANINTF: CAN INTERRUPT FLAG REGISTER
#define MCP2515_RX0IF   0x01
#define MCP2515_RX1IF   0x02
#define MCP2515_TX0IF   0x04
#define MCP2515_TX1IF   0x08
#define MCP2515_TX2IF   0x10
#define MCP2515_ERRIF   0x20
#define MCP2515_WAKIF   0x40
#define MCP2515_MERRF   0x80

#define MCP2515_RXBUF_0         MCP2515_RXB0SIDH
#define MCP2515_RXBUF_1         MCP2515_RXB1SIDH

#define MCP2515_OK              0
#define MCP2515_FAIL            1
#define MCP2515_ALLTXBUSY       2
#define MCP2515_NO_RX_MSG       3
#define MCP2515_SET_MODE_FAIL   4
#define MCP2515_INVALID_INT     5
#define MCP2515_NO_AVAIL_INTS   6

#define CAN_STDID               0
#define CAN_EXTID               1

// Tx Buffer numbers
#define TXB0                    0
#define TXB1                    1
#define TXB2                    2

// Rx Buffer numbers
#define RXB0                    0
#define RXB1                    1

// Rx Mask numbers
#define RXM0                    0
#define RXM1                    1

// Rx Filter numbers
#define RXF0                    0
#define RXF1                    1
#define RXF2                    2
#define RXF3                    3
#define RXF4                    4
#define RXF5                    5

#if 1   // CAN bus speeds
#define CAN_5KBPS               1
#define CAN_10KBPS              2
#define CAN_20KBPS              3
#define CAN_31K25BPS            4
#define CAN_33KBPS              5
#define CAN_40KBPS              6
#define CAN_50KBPS              7
#define CAN_80KBPS              8
#define CAN_83K3BPS             9
#define CAN_95KBPS              10
#define CAN_100KBPS             11
#define CAN_125KBPS             12
#define CAN_200KBPS             13
#define CAN_250KBPS             14
#define CAN_500KBPS             15
#define CAN_1000KBPS            16
#endif

#define CAN_OK                  0
#define CAN_FAILINIT            1
#define CAN_FAILTX              2
#define CAN_MSGAVAIL            3
#define CAN_NOMSG               4
#define CAN_CTRLERROR           5
#define CAN_GETTXBFTIMEOUT      6
#define CAN_SENDMSGTIMEOUT      7
#define CAN_FAIL                0xff

#define CAN_MAX_MESSAGE_LENGTH (8)

#define MASK_ANY_ID      0x00000000
#define MASK_ANY_DATA    0x00

#if 1   // 8 MHz Configuration registers settings based on CAN bus speed
#define MCP_8MHz_1000kBPS_CFG1  (0x00)
#define MCP_8MHz_1000kBPS_CFG2  (0x80)
#define MCP_8MHz_1000kBPS_CFG3  (0x80)

#define MCP_8MHz_500kBPS_CFG1   (0x00)
#define MCP_8MHz_500kBPS_CFG2   (0x90)
#define MCP_8MHz_500kBPS_CFG3   (0x82)

#define MCP_8MHz_250kBPS_CFG1   (0x00)
#define MCP_8MHz_250kBPS_CFG2   (0xB1)
#define MCP_8MHz_250kBPS_CFG3   (0x85)

#define MCP_8MHz_200kBPS_CFG1   (0x00)
#define MCP_8MHz_200kBPS_CFG2   (0xB4)
#define MCP_8MHz_200kBPS_CFG3   (0x86)

#define MCP_8MHz_125kBPS_CFG1   (0x01)
#define MCP_8MHz_125kBPS_CFG2   (0xB1)
#define MCP_8MHz_125kBPS_CFG3   (0x85)

#define MCP_8MHz_100kBPS_CFG1   (0x01)
#define MCP_8MHz_100kBPS_CFG2   (0xB4)
#define MCP_8MHz_100kBPS_CFG3   (0x86)

#define MCP_8MHz_80kBPS_CFG1    (0x01)
#define MCP_8MHz_80kBPS_CFG2    (0xBF)
#define MCP_8MHz_80kBPS_CFG3    (0x87)

#define MCP_8MHz_50kBPS_CFG1    (0x03)
#define MCP_8MHz_50kBPS_CFG2    (0xB4)
#define MCP_8MHz_50kBPS_CFG3    (0x86)

#define MCP_8MHz_40kBPS_CFG1    (0x03)
#define MCP_8MHz_40kBPS_CFG2    (0xBF)
#define MCP_8MHz_40kBPS_CFG3    (0x87)

#define MCP_8MHz_33k3BPS_CFG1   (0x47)
#define MCP_8MHz_33k3BPS_CFG2   (0xE2)
#define MCP_8MHz_33k3BPS_CFG3   (0x85)

#define MCP_8MHz_31k25BPS_CFG1  (0x07)
#define MCP_8MHz_31k25BPS_CFG2  (0xA4)
#define MCP_8MHz_31k25BPS_CFG3  (0x84)

#define MCP_8MHz_20kBPS_CFG1    (0x07)
#define MCP_8MHz_20kBPS_CFG2    (0xBF)
#define MCP_8MHz_20kBPS_CFG3    (0x87)

#define MCP_8MHz_10kBPS_CFG1    (0x0F)
#define MCP_8MHz_10kBPS_CFG2    (0xBF)
#define MCP_8MHz_10kBPS_CFG3    (0x87)

#define MCP_8MHz_5kBPS_CFG1     (0x1F)
#define MCP_8MHz_5kBPS_CFG2     (0xBF)
#define MCP_8MHz_5kBPS_CFG3     (0x87)
#endif

#endif

#endif  // __MCP2515_H__
