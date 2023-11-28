/** \file DLK_MCP2515.h */
/*
 * NAME: DLK_MCP2515.h
 *
 * WHAT:
 *  Header file for DLK_MCP2515 Arduino CAN driver class.
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 *
 * AUTHOR:
 *  D.L. Karmann
 *
 */
#ifndef __DLK_MCP2515_H__
#define __DLK_MCP2515_H__

#include <SPI.h>

#include "Arduino.h"
#include "can.h"
#include "MCP2515.h"

#if defined(ARDUINO_ARCH_RP2040) && defined(ARDUINO_ARCH_MBED_RP2040)
#error "Unsupported MCU"
#endif

#if defined(ARDUINO_ARCH_RP2040) && !defined(ARDUINO_ARCH_MBED_RP2040)
#define PHILHOWER_RP2040    // Philhower Arduino for Raspberry Pi Pico/Pico W
#endif

#define SPI0_NUM    0
#define SPI1_NUM    1

#define SPI_DUMMY_BYTE  0x00
#define FRAME_CNT       4

#ifdef __AVR__              // Nano
    #define MAX_INTS    2
#endif

#ifdef PHILHOWER_RP2040     // Pi Pico
    #define MAX_INTS    4
#endif

#ifdef TEENSYDUINO          // Teensy 3.1
    #define MAX_INTS    4
#endif

#ifdef ESP8266              // ESP8266
    #define MAX_INTS    0   // .usingInterrupt() not supported
#endif

#ifdef ESP32                // ESP32
    #define MAX_INTS    0   // .usingInterrupt() not supported
#endif

/**
 * DLK_MCP2515 Arduino MCP2515 CAN library class. Version: "V1.0.3 9/26/2023"
 */
class DLK_MCP2515
{
    public:
        // Constructor
        /**
         *  A constructor that sets up the DLK_MCP2515 CAN processing code.
         *
         *  \param spi_speed: the speed (bps) of the SPI interface to the MCP2515 device
         *  \param cs_pin: the chip select Arduino pin (~CS) of the SPI interface to the MCP2515 device
         *  \param hw_cs_pin: true if using SPI HW CS (if supported), else false {optional}
         *  \param which_spi: which SPI peripheral to use - \ref SPI0_NUM or \ref SPI1_NUM (always SPI0_NUM for AVR) {optional}
         *
         *  \return None.
         */
        DLK_MCP2515(uint32_t spi_speed, uint8_t cs_pin, bool hw_cs_pin = false, uint8_t which_spi = SPI0_NUM);

        /**
         * Read from specified MCP2515 register.
         *
         * \param reg: the MCP2515 register (0x00 to 0x7d) to read from
         *
         * \return   int8_t = MCP2515 register value
         */
        uint8_t MCP2515_ReadRegister(uint8_t reg);

        /**
         * Read from specified MCP2515 registers.
         *
         * \param reg: the starting MCP2515 register (0x00 to 0x7d) to read from
         * \param values: place to store the MCP2515 registers values read
         * \param cnt: the number of consecutive MCP2515 registers to read from
         *
         *  \return None.
         */
        void MCP2515_ReadRegisters(uint8_t reg, uint8_t values[], uint8_t cnt);

        /**
         * Write specified value to specified MCP2515 register.
         *
         * \param reg: the MCP2515 register (0x00 to 0x7d) to write to
         * \param val: the value to write to the MCP2515 register
         *
         *  \return None.
         */
        void MCP2515_WriteRegister(uint8_t reg, uint8_t val);

        /**
         * Write specified values to starting at specified MCP2515 register.
         *
         * \param reg: the starting MCP2515 register (0x00 to 0x7d) to write to
         * \param vals: the values to write to the MCP2515 registers
         * \param cnt: the number of consecutive MCP2515 registers to write to
         *
         *  \return None.
         */
        void MCP2515_WriteRegisters(uint8_t reg, uint8_t vals[], uint8_t cnt);

        /**
         * Modify specified MCP2515 register with specified mask and specified data.
         *
         * \param reg: the MCP2515 register (0x00 to 0x7d) to modify
         * \param mask: the bit mask of bits in the MCP2515 register to modify
         * \param data: the value to modify in the MCP2515 register
         *
         *  \return None.
         */
        void MCP2515_ModifyRegister(uint8_t reg, uint8_t mask, uint8_t data);

        /**
         * Check if specified MCP2515 register is writable.
         *
         * \param reg: the MCP2515 register (0x00 to 0x7d) to check
         *
         * \return   int8_t
         * \return   MCP2515_FAIL = register not writable in current non-Configuration mode
         * \return   MCP2515_OK = register is writable
         */
        uint8_t MCP2515_CheckRegisterWritable(uint8_t reg);

        /**
         * Reset the MCP2515 device.
         *
         *  \return None.
         */
        void MCP2515_Reset(void);

        /**
         * Read and return the MCP2515 READ STATUS Instruction value.
         *
         * \return   int8_t = the MCP2515 READ STATUS Instruction value
         */
        uint8_t MCP2515_ReadStatus(void);

        /**
         * Read and return the MCP2515 RX STATUS Instruction value.
         *
         * \return   int8_t = the MCP2515 RX STATUS Instruction value
         */
        uint8_t MCP2515_ReadRxStatus(void);

        /**
         * Set specified MCP2515 mode of operation.
         *
         * \param newMode: the MCP2515 mode (MODE_NORMAL, MODE_SLEEP, MODE_LOOPBACK,
         *                 MODE_LISTENONLY, or MODE_CONFIG) to set
         *
         * \return   int8_t
         * \return   MCP2515_FAIL = the MCP2515 failed to change to specified mode
         * \return   MCP2515_OK = the MCP2515 mode setting was successful
         */
        uint8_t MCP2515_SetMode(uint8_t newMode);

        /**
         * Set specified MCP2515 CAN speed.
         *
         * \param canSpeed: the MCP2515 CAN bus speed to set
         *                  (CAN_5KBPS, CAN_10KBPS, CAN_20KBPS, CAN_31K25BPS, CAN_33KBPS,
         *                   CAN_40KBPS, CAN_50KBPS, CAN_80KBPS, CAN_83K3BPS, CAN_95KBPS,
         *                   CAN_100KBPS, CAN_125KBPS, CAN_200KBPS, CAN_250KBPS, CAN_500KBPS,
         *                   or CAN_1000KBPS)
         *
         * \return   int8_t
         * \return   MCP2515_FAIL = the MCP2515 failed to change to configuration mode
         * \return   MCP2515_OK = the MCP2515 CAN speed setting was successful
         */
        uint8_t MCP2515_SetBitrate(uint8_t canSpeed);

        /**
         * Set specified MCP2515 Rx buffer operating mode.
         *
         * \param rx_num: the MCP2515 Rx buffer (RXB0 or RXB1) to set operating mode
         * \param rx_mode: the MCP2515 Rx buffer operating mode to set
         *                  (RXM_M0 - disable all masks/filters to receive any message \n
         *                   or RXM_M0 - receive messages meeting masks/filters criteria)
         *
         * \return   int8_t
         * \return   MCP2515_FAIL = incorrect Rx buffer specified
         * \return   MCP2515_OK = the MCP2515 Rx buffer operating mode setting was successful
         */
        uint8_t MCP2515_SetRxMode(uint8_t rx_num, uint8_t rx_mode);

        /**
         * Set specified MCP2515 Standard Frame Filter values.
         *
         * \param filt_num: the MCP2515 filter (RXF0 to RXF5) to set
         * \param filt_id: the MCP2515 CAN message ID filter to set
         * \param filt_d0: the MCP2515 CAN message Data 0 byte filter to set
         * \param filt_d1: the MCP2515 CAN message Data 1 byte filter to set
         *
         *  \return None.
         *
         *  \note Supports standard 11-bit CAN data frames.
         */
        void MCP2515_SetFilter(uint8_t filt_num, uint16_t filt_id, uint8_t filt_d0, uint8_t filt_d1);

        /**
         * Set specified MCP2515 Extended Frame Filter values.
         *
         * \param filt_num: the MCP2515 filter (RXF0 to RXF5) to set
         * \param filt_id: the MCP2515 CAN message ID filter to set
         *
         *  \return None.
         *
         *  \note Supports extended 29-bit CAN data frames.
         */
        void MCP2515_SetExtFilter(uint8_t filt_num, uint32_t filt_id);

        /**
         * Set specified MCP2515 Standard Frame Mask values.
         *
         * \param mask_num: the MCP2515 mask (RXM0 or RXM1) to set
         * \param mask_id: the MCP2515 CAN message ID mask to set
         * \param mask_d0: the MCP2515 CAN message Data 0 byte mask to set
         * \param mask_d1: the MCP2515 CAN message Data 1 byte mask to set
         *
         *  \return None.
         *
         *  \note Supports standard 11-bit CAN data frames.
         */
        void MCP2515_SetMask(uint8_t mask_num, uint16_t mask_id, uint8_t mask_d0, uint8_t mask_d1);

        /**
         * Set specified MCP2515 Extended Frame Mask value.
         *
         * \param mask_num: the MCP2515 mask (RXM0 or RXM1) to set
         * \param mask_id: the MCP2515 CAN message ID mask to set
         *
         *  \return None.
         *
         *  \note Supports extended 29-bit CAN data frames.
         */
        void MCP2515_SetExtMask(uint8_t mask_num, uint32_t mask_id);

        /**
         * Initialize MCP2515 device.
         *
         * \param canSpeed: the MCP2515 CAN bus speed to use
         *                  (CAN_5KBPS, CAN_10KBPS, CAN_20KBPS, CAN_31K25BPS, CAN_33KBPS,
         *                   CAN_40KBPS, CAN_50KBPS, CAN_80KBPS, CAN_83K3BPS, CAN_95KBPS,
         *                   CAN_100KBPS, CAN_125KBPS, CAN_200KBPS, CAN_250KBPS, CAN_500KBPS,
         *                   or CAN_1000KBPS)
         *
         * \return   MCP2515_FAIL = the MCP2515 failed to change to configuration mode
         * \return   MCP2515_OK = the MCP2515 initialization was successful
         */
        uint8_t MCP2515_Init(uint8_t canSpeed);

        /**
         * Send Standard CAN frame data to MCP2515 for CAN transmission.
         *
         * \param id: the CAN bus ID to use
         * \param len: the number of bytes to send
         * \param can_msg: the CAN message to send
         *
         * \return   MCP2515_FAIL:
         * \return      - 'len' was incorrect
         * \return      - CAN transmission error occurred
         * \return      - CAN transmission failed (i.e. no other CAN device on CAN bus)
         * \return   MCP2515_OK = the MCP2515 CAN transmission was successful
         */
        uint8_t MCP2515_Send(uint32_t id, uint8_t len, uint8_t * can_msg);

        /**
         * Send Extended CAN frame data to MCP2515 for CAN transmission.
         *
         * \param id: the CAN bus ID to use
         * \param len: the number of bytes to send
         * \param can_msg: the CAN message to send
         *
         * \return   MCP2515_FAIL:
         * \return      - 'len' was incorrect
         * \return      - CAN transmission error occurred
         * \return      - CAN transmission failed (i.e. no other CAN device on CAN bus)
         * \return   MCP2515_OK = the MCP2515 CAN transmission was successful
         */
        uint8_t MCP2515_Xsend(uint32_t id, uint8_t len, uint8_t * can_msg);

        /**
         * Send Standard Remote CAN frame to MCP2515 for CAN transmission.
         *
         * \param id: the CAN bus ID to use
         * \param len: the expected number of bytes in response
         *
         * \return   MCP2515_FAIL:
         * \return      - 'len' was incorrect
         * \return      - CAN transmission error occurred
         * \return      - CAN transmission failed (i.e. no other CAN device on CAN bus)
         * \return   MCP2515_OK = the MCP2515 CAN transmission was successful
         */
        uint8_t MCP2515_RtrSend(uint16_t id, uint8_t len);

        /**
         * Send Extended Remote CAN frame to MCP2515 for CAN transmission.
         *
         * \param id: the CAN bus ID to use
         * \param len: the expected number of bytes in response
         *
         * \return   MCP2515_FAIL:
         * \return      - 'len' was incorrect
         * \return      - CAN transmission error occurred
         * \return      - CAN transmission failed (i.e. no other CAN device on CAN bus)
         * \return   MCP2515_OK = the MCP2515 CAN transmission was successful
         */
        uint8_t MCP2515_ExtRtrSend(uint32_t id, uint8_t len);

        /*
            1) Determine if CAN message has been received
                - RX Status[7:6] or Read Status[1:0]  (either will indicate message available)
            2) Determine Rx buffer containing CAN message
            3) Get ID/EID from RXBnSIDH, RXBnSIDL, RXBnEID8, RXBnEID0
            4) Get RTR from RXBnCTRL
            5) Get DLC from RXBnDLC
            6) Get CAN data from RXBnD0 to RXBnD7
            7) Notify MCP2515 that CAN message has been retrieved
               (clear RXnIF in CANINTF register)
        */
        /**
         * Retrieve CAN data from MCP2515 device.
         *
         * \param frame: the place to store the received CAN message
         *
         * \return   MCP2515_NO_RX_MSG = no CAN message available
         * \return   MCP2515_OK = the MCP2515 CAN message retrieval was successful
         */
        uint8_t MCP2515_Recv(CAN_FRAME * frame);

        /**
         * Setup callback for MCP2515 receive interrupts.
         *
         * \param int_pin: the interrupt pin for MCP2515 receive interrupts 
         * \param callback: the application callback function to call on receive interrupts
         *
         * \return   int8_t
         * \return   MCP2515_FAIL = could not attach Rx interrupt 
         * \return   MCP2515_OK = Rx interrupt attachment successful
         *
         *  \note MCP2515_FAIL return due to not a valid interrupt pin \b int_pin or
         *        all supported interrupt instances (2) already in use.
         */
        uint8_t MCP2515_OnRxInterrupt(int int_pin, void (* callback)(CAN_FRAME *));

    private:
        /// the SPI port to use (SPI0_NUM or SPI1_NUM)
        uint8_t WhichSPI;

        /// Pointer to SPI device
#if defined(__AVR__) || defined(TEENSYDUINO) || defined(ESP8266) || defined(ESP32)
        SPIClass * SPI_dev;
#endif
#ifdef PHILHOWER_RP2040
        SPIClassRP2040 * SPI_dev;
#endif

        /// Flag for SPI initialization
#if defined(__AVR__) || defined(TEENSYDUINO) || defined(ESP8266)
        static bool SPI_initted;
#endif
#if defined(PHILHOWER_RP2040) || defined(ESP32)
        static bool SPI0_initted;
        static bool SPI1_initted;
#endif

        /// Chip Select pin number
        uint8_t CS_pin;

        /// HW Chip Select pin usage
        bool HW_CS_pin;

        /// SPI configuration settings
        SPISettings SPI_Settings;

        /// pointers to DLK_MCP2515 class using private static class variables for interrupts usage
#if (MAX_INTS > 0)
        static DLK_MCP2515 * instance1;
#if (MAX_INTS > 1)
        static DLK_MCP2515 * instance2;
#if (MAX_INTS > 2)
        static DLK_MCP2515 * instance3;
        static DLK_MCP2515 * instance4;
#endif
#endif
#endif

        /// storage for received CAN messages
        CAN_FRAME CAN_Frame[FRAME_CNT];

        /// next index for storage for received CAN messages
        uint8_t MsgNdx = 0;

        /// Rx interrupt callback function
        void (* MCP2515_InterruptHandler)(CAN_FRAME *);

        /// Initiate MCP2515 SPI transaction
        inline void MCP2515_StartSPI(void);

        /// Terminate MCP2515 SPI transaction
        inline void MCP2515_EndSPI(void);

        /// Prepare ID field data
        ///  - supports standard 11-bit CAN data frames
        void MCP2515_PrepareId(uint8_t id_data[], uint32_t can_id);

        /// Prepare Extended ID field data
        ///  - supports extended 29-bit CAN data frames
        void MCP2515_PrepareExtId(uint8_t id_data[], uint32_t can_id);

        /// Prepare Standard Frame Mask field data
        ///  - supports standard 11-bit CAN data frames
        void MCP2515_PrepareMask(uint8_t mask_data[], uint16_t mask_id, uint8_t mask_d0, uint8_t mask_d1);

        /// Prepare Extended Frame Mask field data
        ///  - supports extended 29-bit CAN data frames
        void MCP2515_PrepareExtMask(uint8_t mask_data[], uint32_t mask_id);

        /// Prepare Standard Frame Filter field data
        ///  - supports standard 11-bit CAN data frames
        void MCP2515_PrepareFilter(uint8_t filt_data[], uint16_t filt_id, uint8_t filt_d0, uint8_t filt_d1);

        /// Prepare Extended Frame Filter field data
        ///  - supports extended 29-bit CAN data frames
        void MCP2515_PrepareExtFilter(uint8_t filt_data[], uint32_t filt_id);

        /// Send CAN frame to MCP2515 for CAN transmission
        ///  - supports both standard 11-bit and extended 29-bit CAN data frames
        ///  - RTR (Remote Transmission Request) supported
        uint8_t MCP2515_SendMessage(CAN_FRAME * frame);

        /// 1) Determine if CAN message has been received \n
        /// 2) Determine Rx buffer containing CAN message
        uint8_t MCP2515_CheckCAN_Rx(void);

        /// 3) Get ID/EID from RXBnSIDH, RXBnSIDL, RXBnEID8, RXBnEID0 (includes IDE bit)
        uint32_t MCP2515_ReadCAN_ID(uint8_t id_addr);

        /// 3) Get ID/EID from RXBnSIDH, RXBnSIDL, RXBnEID8, RXBnEID0 (includes IDE bit) \n
        /// 4) Get RTR from RXBnCTRL \n
        /// 5) Get DLC from RXBnDLC \n
        /// 6) Get CAN data from RXBnD0 to RXBnD7
        void MCP2515_ReadCAN_Msg(uint8_t rxbn_addr, CAN_FRAME * frame);

        /// MCP2515 Int pin handler
        void MCP2515_HandleInterrupt(void);

        /// MCP2515 Int pin handler
#if (MAX_INTS > 0)
        static void MCP2515_OnInterrupt1(void);
#if (MAX_INTS > 1)
        static void MCP2515_OnInterrupt2(void);
#if (MAX_INTS > 2)
        static void MCP2515_OnInterrupt3(void);
        static void MCP2515_OnInterrupt4(void);
#endif
#endif
#endif
};
#endif  // __DLK_MCP2515_H__

