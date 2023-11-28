/** \file DLK_MCP2515.cpp */
/*
 * NAME: DLK_MCP2515.cpp
 *
 * WHAT:
 *  Arduino MCP2515 CAN library functions for MCP2515 CAN SPI operations.
 *
 * SPECIAL CONSIDERATIONS:
 *  None
 *
 * AUTHOR:
 *  D.L. Karmann
 *
 * MODIFIED:
 *
 */

#include <SPI.h>
#include "DLK_MCP2515.h"

// outside of DLK_MCP2515 class

// define a non-class pointer to DLK_MCP2515 class using private static class variable
#if (MAX_INTS > 0)
DLK_MCP2515 * DLK_MCP2515::instance1;
#if (MAX_INTS > 1)
DLK_MCP2515 * DLK_MCP2515::instance2;
#if (MAX_INTS > 2)
DLK_MCP2515 * DLK_MCP2515::instance3;
DLK_MCP2515 * DLK_MCP2515::instance4;
#endif
#endif
#endif

// private static class variable must be initialized outside of class
#if defined(__AVR__) || defined(TEENSYDUINO) || defined(ESP8266)
bool DLK_MCP2515::SPI_initted = false;
#endif

#if defined(PHILHOWER_RP2040) || defined(ESP32)
bool DLK_MCP2515::SPI0_initted = false;
bool DLK_MCP2515::SPI1_initted = false;
#endif

#if defined(ESP32)
SPIClass SPIH = SPIClass(HSPI); // SPI1 appears to be used somewhere and crashes ESP32 code if used here!!!
#endif

// DLK_MCP2515 Class members

// Constructor
DLK_MCP2515::DLK_MCP2515(uint32_t spi_speed, uint8_t cs_pin, bool hw_cs_pin, uint8_t which_spi)
{
    CS_pin = cs_pin;
    HW_CS_pin = hw_cs_pin;
    WhichSPI = which_spi;

#if defined(__AVR__) || defined(TEENSYDUINO) || defined(ESP8266) || defined(ESP32)
    HW_CS_pin = false;
#endif

    if (HW_CS_pin)
    {
        // the following SPI mode is OK for SW CS
        // the following SPI mode is needed by HW CS for continuous CS assertion during all bytes in a transaction
        SPISettings spi_settings(spi_speed, MSBFIRST, SPI_MODE3);
        SPI_Settings = spi_settings;
    }
    else    // SW CS
    {
        // the following SPI mode is OK for SW CS
        // but for HW CS, with this SPI mode, the CS is only asserted separately for each byte in the transaction
        SPISettings spi_settings(spi_speed, MSBFIRST, SPI_MODE0);
        SPI_Settings = spi_settings;
    }

#if (MAX_INTS > 0)
    instance1 = nullptr;
#if (MAX_INTS > 1)
    instance2 = nullptr;
#if (MAX_INTS > 2)
    instance3 = nullptr;
    instance4 = nullptr;
#endif
#endif
#endif
}

#if 1
// Initiate MCP2515 SPI transaction
inline void DLK_MCP2515::MCP2515_StartSPI(void)
{
    SPI_dev->beginTransaction(SPI_Settings);
    if (!HW_CS_pin)
    {
        digitalWrite(CS_pin, LOW);
    }
}

// Terminate MCP2515 SPI transaction
inline void DLK_MCP2515::MCP2515_EndSPI(void)
{
    if (!HW_CS_pin)
    {
        digitalWrite(CS_pin, HIGH);
    }
    SPI_dev->endTransaction();
}

// Read from specified MCP2515 register
uint8_t DLK_MCP2515::MCP2515_ReadRegister(uint8_t reg)
{
    uint8_t ret;

    MCP2515_StartSPI();
#if defined(__AVR__) || defined(TEENSYDUINO) || defined(ESP8266) || defined(ESP32)
    SPI_dev->transfer(MCP2515_READ);
    SPI_dev->transfer(reg);
    ret = SPI_dev->transfer(SPI_DUMMY_BYTE);
#else
    uint8_t spi_txdata[3];
    uint8_t spi_rxdata[3];

    spi_txdata[0] = MCP2515_READ;
    spi_txdata[1] = reg;
    spi_txdata[2] = SPI_DUMMY_BYTE;
    SPI_dev->transfer(spi_txdata, spi_rxdata, sizeof(spi_txdata));
    ret = spi_rxdata[2];
#endif
    MCP2515_EndSPI();

    return ret;
}

// Read from specified MCP2515 registers
void DLK_MCP2515::MCP2515_ReadRegisters(uint8_t reg, uint8_t values[], uint8_t cnt)
{
    MCP2515_StartSPI();
#if defined(__AVR__) || defined(TEENSYDUINO) || defined(ESP8266) || defined(ESP32)
    SPI_dev->transfer(MCP2515_READ);
    SPI_dev->transfer(reg);
    memset(values, SPI_DUMMY_BYTE, cnt);    // optional

    // MCP2515 has auto-increment of address-pointer
    SPI_dev->transfer(values, cnt);
#else

    uint8_t spi_txdata[11];
    uint8_t spi_rxdata[11];

    spi_txdata[0] = MCP2515_READ;
    spi_txdata[1] = reg;
    memset(&spi_txdata[2], SPI_DUMMY_BYTE, cnt);    // optional
    SPI_dev->transfer(spi_txdata, spi_rxdata, cnt + 2);
    memcpy(values, &spi_rxdata[2], cnt);
#endif
    MCP2515_EndSPI();
}

// Write specified value to specified MCP2515 register
void DLK_MCP2515::MCP2515_WriteRegister(uint8_t reg, uint8_t val)
{
    MCP2515_StartSPI();
#if defined(__AVR__) || defined(TEENSYDUINO) || defined(ESP8266) || defined(ESP32)
    SPI_dev->transfer(MCP2515_WRITE);
    SPI_dev->transfer(reg);
    SPI_dev->transfer(val);
#else
    uint8_t spi_data[3];

    spi_data[0] = MCP2515_WRITE;
    spi_data[1] = reg;
    spi_data[2] = val;
    SPI_dev->transfer(spi_data, nullptr, sizeof(spi_data));
#endif
    MCP2515_EndSPI();
}

// Write specified values to starting at specified MCP2515 register
void DLK_MCP2515::MCP2515_WriteRegisters(uint8_t reg, uint8_t vals[], uint8_t cnt)
{
    if (cnt)
    {
        MCP2515_StartSPI();
#if defined(__AVR__) || defined(TEENSYDUINO) || defined(ESP8266) || defined(ESP32)
        SPI_dev->transfer(MCP2515_WRITE);
        SPI_dev->transfer(reg);
        SPI_dev->transfer(vals, cnt);
#else
        uint8_t spi_data[11];

        spi_data[0] = MCP2515_WRITE;
        spi_data[1] = reg;
        memcpy(&spi_data[2], vals, cnt);
        SPI_dev->transfer(spi_data, nullptr, cnt + 2);
#endif
        MCP2515_EndSPI();
    }
}

// Modify specified MCP2515 register with specified mask and specified data
void DLK_MCP2515::MCP2515_ModifyRegister(uint8_t reg, uint8_t mask, uint8_t data)
{
    MCP2515_StartSPI();
#if defined(__AVR__) || defined(TEENSYDUINO) || defined(ESP8266) || defined(ESP32)
    SPI_dev->transfer(MCP2515_BITMOD);
    SPI_dev->transfer(reg);
    SPI_dev->transfer(mask);
    SPI_dev->transfer(data);
#else
    uint8_t spi_data[4];

    spi_data[0] = MCP2515_BITMOD;
    spi_data[1] = reg;
    spi_data[2] = mask;
    spi_data[3] = data;
    SPI_dev->transfer(spi_data, nullptr, sizeof(spi_data));
#endif
    MCP2515_EndSPI();
}

// Check if specified MCP2515 register is writable
uint8_t DLK_MCP2515::MCP2515_CheckRegisterWritable(uint8_t reg)
{
    uint8_t mode;

    mode = MCP2515_ReadRegister(MCP2515_CANCTRL) & MODE_MASK;
    if (mode == MODE_CONFIG)
    {
        return MCP2515_OK;      // all registers writable in Configuration mode
    }
    else    // not in Configuration mode
    {
        switch (reg)
        {
            case MCP2515_RXF0SIDH:
            case MCP2515_RXF0SIDL:
            case MCP2515_RXF0EID8:
            case MCP2515_RXF0EID0:
            case MCP2515_RXF1SIDH:
            case MCP2515_RXF1SIDL:
            case MCP2515_RXF1EID8:
            case MCP2515_RXF1EID0:
            case MCP2515_RXF2SIDH:
            case MCP2515_RXF2SIDL:
            case MCP2515_RXF2EID8:
            case MCP2515_RXF2EID0:
            case MCP2515_TXRTSCTRL:
            case MCP2515_RXF3SIDH:
            case MCP2515_RXF3SIDL:
            case MCP2515_RXF3EID8:
            case MCP2515_RXF3EID0:
            case MCP2515_RXF4SIDH:
            case MCP2515_RXF4SIDL:
            case MCP2515_RXF4EID8:
            case MCP2515_RXF4EID0:
            case MCP2515_RXF5SIDH:
            case MCP2515_RXF5SIDL:
            case MCP2515_RXF5EID8:
            case MCP2515_RXF5EID0:
            case MCP2515_RXM0SIDH:
            case MCP2515_RXM0SIDL:
            case MCP2515_RXM0EID8:
            case MCP2515_RXM0EID0:
            case MCP2515_RXM1SIDH:
            case MCP2515_RXM1SIDL:
            case MCP2515_RXM1EID8:
            case MCP2515_RXM1EID0:
            case MCP2515_CNF3:
            case MCP2515_CNF2:
            case MCP2515_CNF1:
                return MCP2515_FAIL;    // registers not writable in non-Configuration mode
        }
    }
    return MCP2515_OK;      // all other registers writable in non-Configuration mode
}

// Reset MCP2515
void DLK_MCP2515::MCP2515_Reset(void)
{
    MCP2515_StartSPI();
    SPI_dev->transfer(MCP2515_RESET);
    MCP2515_EndSPI();
    delay(10);
}

// Read and return MCP2515 Status
uint8_t DLK_MCP2515::MCP2515_ReadStatus(void)
{
    uint8_t ret;

    MCP2515_StartSPI();
#if defined(__AVR__) || defined(TEENSYDUINO) || defined(ESP8266) || defined(ESP32)
    SPI_dev->transfer(MCP2515_READ_STATUS);
    ret = SPI_dev->transfer(SPI_DUMMY_BYTE);
#else
    uint8_t spi_txdata[2];
    uint8_t spi_rxdata[2];

    spi_txdata[0] = MCP2515_READ_STATUS;
    spi_txdata[1] = SPI_DUMMY_BYTE;
    SPI_dev->transfer(spi_txdata, spi_rxdata, sizeof(spi_txdata));
    ret = spi_rxdata[1];
#endif
    MCP2515_EndSPI();

    return ret;
}

// Read and return MCP2515 Rx Status
uint8_t DLK_MCP2515::MCP2515_ReadRxStatus(void)
{
    uint8_t ret;

    MCP2515_StartSPI();
#if defined(__AVR__) || defined(TEENSYDUINO) || defined(ESP8266) || defined(ESP32)
    SPI_dev->transfer(MCP2515_RX_STATUS);
    ret = SPI_dev->transfer(SPI_DUMMY_BYTE);
#else
    uint8_t spi_txdata[2];
    uint8_t spi_rxdata[2];

    spi_txdata[0] = MCP2515_RX_STATUS;
    spi_txdata[1] = SPI_DUMMY_BYTE;
    SPI_dev->transfer(spi_txdata, spi_rxdata, sizeof(spi_txdata));
    ret = spi_rxdata[1];
#endif
    MCP2515_EndSPI();

    return ret;
}

// Set specified MCP2515 mode of operation
uint8_t DLK_MCP2515::MCP2515_SetMode(uint8_t newMode)
{
    uint8_t wakeIntEnabled;

    // If the chip is asleep and we want to change mode, then a manual wake needs to be done
    // This is done by setting the wake up interrupt flag
    // This undocumented trick was found at:
    //      https://github.com/mkleemann/can/blob/master/can_sleep_mcp2515.c
    if (((MCP2515_ReadRegister(MCP2515_CANCTRL) & MODE_MASK) == MODE_SLEEP) &&
         (newMode != MODE_SLEEP))
    {
        // Make sure wake interrupt is enabled
        wakeIntEnabled = (MCP2515_ReadRegister(MCP2515_CANINTE) & MCP2515_WAKIF);
        if (!wakeIntEnabled)
        {
            MCP2515_ModifyRegister(MCP2515_CANINTE, MCP2515_WAKIF, MCP2515_WAKIF);
        }

        // Set wake flag (this does the actual waking up)
        MCP2515_ModifyRegister(MCP2515_CANINTF, MCP2515_WAKIF, MCP2515_WAKIF);

        // Wait for the chip to exit SLEEP and enter LISTENONLY mode.

        // If the chip is not connected to a CAN bus (or the bus has no other powered nodes)
        // it will sometimes trigger the wake interrupt as soon as it's put to sleep, but it
        // will stay in SLEEP mode instead of automatically switching to LISTENONLY mode.
        // In this situation the mode needs to be manually set to LISTENONLY.
        MCP2515_ModifyRegister(MCP2515_CANCTRL, MODE_MASK, MODE_LISTENONLY);
        if ((MCP2515_ReadRegister(MCP2515_CANCTRL) & MODE_MASK) != MODE_LISTENONLY)
        {
            return MCP2515_SET_MODE_FAIL;
        }

        // Turn wake interrupt back off if it was originally off
        if (!wakeIntEnabled)
        {
            MCP2515_ModifyRegister(MCP2515_CANINTE, MCP2515_WAKIF, 0);
        }

        // Clear wake flag
        MCP2515_ModifyRegister(MCP2515_CANINTF, MCP2515_WAKIF, 0);
    }

    if (newMode == MODE_SLEEP)
    {
        // enable wake interrupt
        MCP2515_ModifyRegister(MCP2515_CANINTE, MCP2515_WAKIF, MCP2515_WAKIF);
    }
    else
    {
        // disable wake interrupt
        MCP2515_ModifyRegister(MCP2515_CANINTE, MCP2515_WAKIF, 0);
    }

    // try multiple times - if need be
    for (uint8_t i = 0; i < 10; ++i)
    {
        MCP2515_ModifyRegister(MCP2515_CANCTRL, MODE_MASK, newMode);
        delay(1);           // allow mode change to propagate! (especially at lower CAN speeds)

        if ((MCP2515_ReadRegister(MCP2515_CANCTRL) & MODE_MASK) == newMode)
        {
            return MCP2515_OK;
        }
    }
    return MCP2515_SET_MODE_FAIL;
}

// Set specified MCP2515 CAN speed
uint8_t DLK_MCP2515::MCP2515_SetBitrate(uint8_t canSpeed)
{
    uint8_t mode;
    uint8_t rslt;
    uint8_t cfg1;
    uint8_t cfg2;
    uint8_t cfg3;

    mode = MCP2515_ReadRegister(MCP2515_CANCTRL) & MODE_MASK;   // get mode
    rslt = MCP2515_SetMode(MODE_CONFIG);    // must be in Configuration mode to write CNFn registers
    if (rslt != MCP2515_OK)
    {
        return rslt;    // could not set Configuration mode
    }

    switch (canSpeed)   // assume 8 Mhz MCP2515 clock
    {
        case (CAN_5KBPS):               //   5KBPS
            cfg1 = MCP_8MHz_5kBPS_CFG1;
            cfg2 = MCP_8MHz_5kBPS_CFG2;
            cfg3 = MCP_8MHz_5kBPS_CFG3;
            break;

        case (CAN_10KBPS):              //  10KBPS
            cfg1 = MCP_8MHz_10kBPS_CFG1;
            cfg2 = MCP_8MHz_10kBPS_CFG2;
            cfg3 = MCP_8MHz_10kBPS_CFG3;
            break;

        case (CAN_20KBPS):              //  20KBPS
            cfg1 = MCP_8MHz_20kBPS_CFG1;
            cfg2 = MCP_8MHz_20kBPS_CFG2;
            cfg3 = MCP_8MHz_20kBPS_CFG3;
            break;

        case (CAN_31K25BPS):            //  31.25KBPS
            cfg1 = MCP_8MHz_31k25BPS_CFG1;
            cfg2 = MCP_8MHz_31k25BPS_CFG2;
            cfg3 = MCP_8MHz_31k25BPS_CFG3;
            break;

        case (CAN_33KBPS):              //  33.333KBPS
            cfg1 = MCP_8MHz_33k3BPS_CFG1;
            cfg2 = MCP_8MHz_33k3BPS_CFG2;
            cfg3 = MCP_8MHz_33k3BPS_CFG3;
            break;

        case (CAN_40KBPS):              //  40Kbps
            cfg1 = MCP_8MHz_40kBPS_CFG1;
            cfg2 = MCP_8MHz_40kBPS_CFG2;
            cfg3 = MCP_8MHz_40kBPS_CFG3;
            break;

        case (CAN_50KBPS):              //  50Kbps
            cfg1 = MCP_8MHz_50kBPS_CFG1;
            cfg2 = MCP_8MHz_50kBPS_CFG2;
            cfg3 = MCP_8MHz_50kBPS_CFG3;
            break;

        case (CAN_80KBPS):              //  80Kbps
            cfg1 = MCP_8MHz_80kBPS_CFG1;
            cfg2 = MCP_8MHz_80kBPS_CFG2;
            cfg3 = MCP_8MHz_80kBPS_CFG3;
            break;

        case (CAN_100KBPS):             // 100Kbps
            cfg1 = MCP_8MHz_100kBPS_CFG1;
            cfg2 = MCP_8MHz_100kBPS_CFG2;
            cfg3 = MCP_8MHz_100kBPS_CFG3;
            break;

        case (CAN_125KBPS):             // 125Kbps
            cfg1 = MCP_8MHz_125kBPS_CFG1;
            cfg2 = MCP_8MHz_125kBPS_CFG2;
            cfg3 = MCP_8MHz_125kBPS_CFG3;
            break;

        case (CAN_200KBPS):             // 200Kbps
            cfg1 = MCP_8MHz_200kBPS_CFG1;
            cfg2 = MCP_8MHz_200kBPS_CFG2;
            cfg3 = MCP_8MHz_200kBPS_CFG3;
            break;

        case (CAN_250KBPS):             // 250Kbps
            cfg1 = MCP_8MHz_250kBPS_CFG1;
            cfg2 = MCP_8MHz_250kBPS_CFG2;
            cfg3 = MCP_8MHz_250kBPS_CFG3;
            break;

        case (CAN_500KBPS):             // 500Kbps
            cfg1 = MCP_8MHz_500kBPS_CFG1;
            cfg2 = MCP_8MHz_500kBPS_CFG2;
            cfg3 = MCP_8MHz_500kBPS_CFG3;
            break;

        case (CAN_1000KBPS):            //   1Mbps
            cfg1 = MCP_8MHz_1000kBPS_CFG1;
            cfg2 = MCP_8MHz_1000kBPS_CFG2;
            cfg3 = MCP_8MHz_1000kBPS_CFG3;
            break;

        default:
            return MCP2515_FAIL;
            break;
    }

    MCP2515_WriteRegister(MCP2515_CNF1, cfg1);
    MCP2515_WriteRegister(MCP2515_CNF2, cfg2);
    MCP2515_WriteRegister(MCP2515_CNF3, cfg3);
//    MCP2515_WriteRegister(MCP2515_CNF3, cfg3 & 0x7f);   // ensure CLKOUT != SOF signal
    // Note: When CLKEN bit is enabled in CANCTRL register (as is default after reset), the
    //       SOF bit in CNF3 register enables outputting SOF signal instead of CLKOUT signal.

    MCP2515_SetMode(mode);          // restore mode
    return rslt;
}

// Set specified MCP2515 Rx operating mode
uint8_t DLK_MCP2515::MCP2515_SetRxMode(uint8_t rx_num, uint8_t rx_mode)
{
    switch (rx_num)
    {
        case RXB0:
            MCP2515_ModifyRegister(MCP2515_RXB0CTRL, RXM_MASK, rx_mode);
            break;
        case RXB1:
            MCP2515_ModifyRegister(MCP2515_RXB1CTRL, RXM_MASK, rx_mode);
            break;
        default:
            return MCP2515_FAIL;
            break;
    }
    return MCP2515_OK;
}

// Prepare Standard Frame Filter field data 
//  - supports standard 11-bit CAN data frames
void DLK_MCP2515::MCP2515_PrepareFilter(uint8_t filt_data[], uint16_t filt_id, uint8_t filt_d0, uint8_t filt_d1)
{
    uint32_t canid;

    canid = (filt_id & CAN_SFF_MASK);
    canid |= ((uint32_t)filt_d0 << 16);           // Data0
    canid |= ((uint32_t)filt_d1 << 24);           // Data1
    MCP2515_PrepareId(filt_data, canid);
}

// Prepare Extended Frame Filter field data 
//  - supports extended 29-bit CAN data frames
void DLK_MCP2515::MCP2515_PrepareExtFilter(uint8_t filt_data[], uint32_t filt_id)
{
    MCP2515_PrepareExtId(filt_data, filt_id);
}

// Set specified MCP2515 Standard Frame Filter values
//  - supports standard 11-bit CAN data frames
void DLK_MCP2515::MCP2515_SetFilter(uint8_t filt_num, uint16_t filt_id, uint8_t filt_d0, uint8_t filt_d1)
{
    const uint8_t filtregs[MCP2515_N_FILTERS] = 
    {
        MCP2515_RXF0SIDH, MCP2515_RXF1SIDH, MCP2515_RXF2SIDH,
        MCP2515_RXF3SIDH, MCP2515_RXF4SIDH, MCP2515_RXF5SIDH
    };
    uint8_t mode;
    uint8_t filt_data[4];

    MCP2515_PrepareFilter(filt_data, filt_id, filt_d0, filt_d1);

    mode = MCP2515_ReadRegister(MCP2515_CANCTRL) & MODE_MASK;   // get mode
    MCP2515_SetMode(MODE_CONFIG);       // must be in Configuration mode to write Filter registers

    MCP2515_WriteRegisters(filtregs[filt_num], filt_data, 4);  // RXFnSIDH, RXFnSIDL, RXFnEID8, RXFnEID0

    MCP2515_SetMode(mode);          // restore mode
}

// Set specified MCP2515 Extended Frame Filter values
//  - supports extended 29-bit CAN data frames
void DLK_MCP2515::MCP2515_SetExtFilter(uint8_t filt_num, uint32_t filt_id)
{
    const uint8_t filtregs[MCP2515_N_FILTERS] = 
    {
        MCP2515_RXF0SIDH, MCP2515_RXF1SIDH, MCP2515_RXF2SIDH,
        MCP2515_RXF3SIDH, MCP2515_RXF4SIDH, MCP2515_RXF5SIDH
    };
    uint8_t mode;
    uint8_t filt_data[4];

    MCP2515_PrepareExtFilter(filt_data, filt_id);

    mode = MCP2515_ReadRegister(MCP2515_CANCTRL) & MODE_MASK;   // get mode
    MCP2515_SetMode(MODE_CONFIG);       // must be in Configuration mode to write Filter registers

    MCP2515_WriteRegisters(filtregs[filt_num], filt_data, 4);  // RXFnSIDH, RXFnSIDL, RXFnEID8, RXFnEID0

    MCP2515_SetMode(mode);          // restore mode
}

// Prepare Standard Frame Mask field data 
//  - supports standard 11-bit CAN data frames
void DLK_MCP2515::MCP2515_PrepareMask(uint8_t mask_data[], uint16_t mask_id, uint8_t mask_d0, uint8_t mask_d1)
{
    uint32_t canid;

    canid = (mask_id & CAN_SFF_MASK);
    canid |= ((uint32_t)mask_d0 << 16);           // Data0
    canid |= ((uint32_t)mask_d1 << 24);           // Data1
    MCP2515_PrepareId(mask_data, canid);
}

// Prepare Extended Frame Mask field data 
//  - supports extended 29-bit CAN data frames
void DLK_MCP2515::MCP2515_PrepareExtMask(uint8_t mask_data[], uint32_t mask_id)
{
    MCP2515_PrepareExtId(mask_data, mask_id);
}

// Set specified MCP2515 Standard Frame Mask values
//  - supports standard 11-bit CAN data frames
void DLK_MCP2515::MCP2515_SetMask(uint8_t mask_num, uint16_t mask_id, uint8_t mask_d0, uint8_t mask_d1)
{
    const uint8_t maskregs[MCP2515_N_FILTERS] = { MCP2515_RXM0SIDH, MCP2515_RXM1SIDH };
    uint8_t mode;
    uint8_t mask_data[4];

    MCP2515_PrepareMask(mask_data, mask_id, mask_d0, mask_d1);

    mode = MCP2515_ReadRegister(MCP2515_CANCTRL) & MODE_MASK;   // get mode
    MCP2515_SetMode(MODE_CONFIG);       // must be in Configuration mode to write Mask registers

    MCP2515_WriteRegisters(maskregs[mask_num], mask_data, 4);  // RXMnSIDH, RXMnSIDL, RXMnEID8, RXMnEID0

    MCP2515_SetMode(mode);          // restore mode
}

// Set specified MCP2515 Extended Frame Mask value
//  - supports extended 29-bit CAN data frames
void DLK_MCP2515::MCP2515_SetExtMask(uint8_t mask_num, uint32_t mask_id)
{
    const uint8_t maskregs[MCP2515_N_FILTERS] = { MCP2515_RXM0SIDH, MCP2515_RXM1SIDH };
    uint8_t mode;
    uint8_t mask_data[4];

    MCP2515_PrepareExtMask(mask_data, mask_id);

    mode = MCP2515_ReadRegister(MCP2515_CANCTRL) & MODE_MASK;   // get mode
    MCP2515_SetMode(MODE_CONFIG);       // must be in Configuration mode to write Filter registers

    MCP2515_WriteRegisters(maskregs[mask_num], mask_data, 4);  // RXMnSIDH, RXMnSIDL, RXMnEID8, RXMnEID0

    MCP2515_SetMode(mode);          // restore mode
}

// Initialize MCP2515
uint8_t DLK_MCP2515::MCP2515_Init(uint8_t canSpeed)
{
    uint8_t rslt;

   // setup SPI
#if defined(__AVR__) || defined(TEENSYDUINO) || defined(ESP8266)
    SPI_dev = &SPI;

    if (!SPI_initted)
    {
        SPI_dev->begin();
        SPI_initted = true;
    }
#endif

#ifdef PHILHOWER_RP2040
    if (WhichSPI == SPI1_NUM)
    {
        SPI_dev = &SPI1;        // SPI1
    }
    else    // SPI0_NUM
    {
        SPI_dev = &SPI;         // SPI0
    }

    if (SPI_dev == &SPI)
    {
        if (!SPI0_initted)
        {
            if (HW_CS_pin)
            {
                if (!SPI_dev->setCS(CS_pin))
                {
                    return MCP2515_FAIL;    // invalid CS pin for SPI port
                }
            }
            SPI_dev->begin(HW_CS_pin);
            SPI0_initted = true;
        }
    }
    else if (SPI_dev == &SPI1)
    {
        if (!SPI1_initted)
        {
            if (HW_CS_pin)
            {
                if (!SPI_dev->setCS(CS_pin))
                {
                    return MCP2515_FAIL;    // invalid CS pin for SPI port
                }
            }
            SPI_dev->begin(HW_CS_pin);
            SPI1_initted = true;
        }
    }
#endif

#ifdef ESP32
    if (WhichSPI == SPI1_NUM)
    {
        SPI_dev = &SPIH;        // HSPI
    }
    else    // SPI0_NUM
    {
        SPI_dev = &SPI;         // VSPI
    }

    if (SPI_dev == &SPI)
    {
        if (!SPI0_initted)
        {
            SPI_dev->begin(SCK, MISO, MOSI, CS_pin);
            SPI0_initted = true;
        }
    }
    else if (SPI_dev == &SPIH)
    {
        if (!SPI1_initted)
        {
            SPI_dev->begin();   // use default HSPI SPI pins
            SPI1_initted = true;
        }
    }
#endif

    if (!HW_CS_pin)
    {
        pinMode(CS_pin, OUTPUT);
        digitalWrite(CS_pin, HIGH);
    }

    MCP2515_Reset();

    rslt = MCP2515_SetBitrate(canSpeed);
    if (rslt != MCP2515_OK)
    {
        return rslt;    // setting bit rate mode change failed
    }

    rslt = MCP2515_SetMode(MODE_NORMAL);
//    rslt = MCP2515_SetMode(MODE_LOOPBACK);  // for communications testing
    if (rslt != MCP2515_OK)
    {
        return rslt;
    }

    // enable Rx rollover from RXB0 to RXB1 when RXB0 is full
    MCP2515_ModifyRegister(MCP2515_RXB0CTRL, BUKT_BIT, BUKT_BIT);

#if 0   // testing
    // disable all Rx filters to receive any CAN message in either Rx Buffer
    MCP2515_SetRxMode(RXB0, RXM_M3);
    MCP2515_SetRxMode(RXB1, RXM_M3);
#else   // recommended
    // unmask all Rx filters to receive any CAN message in either Rx Buffer
    for (uint8_t i = RXM0; i <= RXM1; ++i)
    {
        MCP2515_SetMask(i, MASK_ANY_ID, MASK_ANY_DATA, MASK_ANY_DATA);
    }

    // also ensure NO extended frame filters are enabled
    for (uint8_t i = RXF0; i <= RXF5; ++i)
    {
        MCP2515_SetFilter(i, 0, 0, 0);
    }
#endif

    // enable MCP2515 Rx interrupts (even if not using interrupt ISR - allows polling interrupt pin)
    MCP2515_ModifyRegister(MCP2515_CANINTE, (MCP2515_RX1IF | MCP2515_RX0IF), (MCP2515_RX1IF | MCP2515_RX0IF));

    return rslt;
}

// Send Standard CAN data to MCP2515 for CAN transmission
uint8_t DLK_MCP2515::MCP2515_Send(uint32_t id, uint8_t len, uint8_t * can_msg)
{
    CAN_FRAME can_data;

    // accumulate items into CAN frame
    can_data.can_id = id;
    can_data.can_dlc = len;
    memcpy(can_data.can_data, can_msg, len);

    return MCP2515_SendMessage(&can_data);
}

// Send Extended CAN data to MCP2515 for CAN transmission
uint8_t DLK_MCP2515::MCP2515_Xsend(uint32_t id, uint8_t len, uint8_t * can_msg)
{
    CAN_FRAME can_data;

    // accumulate items into CAN frame
    can_data.can_id = id;
    can_data.can_id |= CAN_EFF_FLAG;    // indicate to use Extended frame
    can_data.can_dlc = len;
    memcpy(can_data.can_data, can_msg, len);

    return MCP2515_SendMessage(&can_data);
}

// Send Remote CAN frame to MCP2515 for CAN transmission
uint8_t DLK_MCP2515::MCP2515_RtrSend(uint16_t id, uint8_t len)
{
    CAN_FRAME can_data;

    // accumulate items into CAN frame
    can_data.can_id = id;
    can_data.can_id |= CAN_RTR_FLAG;    // indicate to use Remote frame
    can_data.can_dlc = len;             // indicate length of expected response data

    return MCP2515_SendMessage(&can_data);
}

// Send Extended Remote CAN frame to MCP2515 for CAN transmission
uint8_t DLK_MCP2515::MCP2515_ExtRtrSend(uint32_t id, uint8_t len)
{
    CAN_FRAME can_data;

    // accumulate items into CAN frame
    can_data.can_id = id;
    can_data.can_id |= CAN_EFF_FLAG;    // indicate to use Extended frame
    can_data.can_id |= CAN_RTR_FLAG;    // indicate to use Remote frame
    can_data.can_dlc = len;             // indicate length of expected response data

    return MCP2515_SendMessage(&can_data);
}

// Send specified CAN frame to MCP2515 for CAN transmission
//  - supports both standard 11-bit and extended 29-bit CAN data frames
//  - RTR (Remote Transmission Request) supported
uint8_t DLK_MCP2515::MCP2515_SendMessage(CAN_FRAME * frame)
{
    const uint8_t ctrlregs[MCP2515_N_TXBUFFERS] = { MCP2515_TXB0CTRL, MCP2515_TXB1CTRL, MCP2515_TXB2CTRL };
    uint16_t cnt = 0;
    uint8_t id_data[4];
    uint8_t rslt;
    uint8_t txb;

    if (frame->can_dlc > CAN_MAX_MESSAGE_LENGTH)
    {
        return MCP2515_FAIL;
    }

   /* MCP2515 Registers for Transmitting CAN Frame
    --------------------------------------------------------------------------------------
    | TXBnCTRL | TXBnSIDH | TXBnSIDL | TXBnEID8 | TXBnEID0 | TXBnDLC | TXBnD0 ... TXBnD7 |
    --------------------------------------------------------------------------------------
               |<---------------- ID/EID ----------------->|<- DLC ->|<----- Data ------>|
   */

    // find first non-pending Tx buffer
    for (uint8_t i = 0; i < MCP2515_N_TXBUFFERS; ++i)
    {
        rslt = MCP2515_ReadRegister(ctrlregs[i]);
        if ((rslt & TXB_TXREQ_BIT) == 0)
        {
            txb = ctrlregs[i];      // found non-pending Tx buffer
            break;
        }
    }
    if ((rslt & TXB_TXREQ_BIT) != 0)    // all Tx buffers were pending
    {
        txb = ctrlregs[0];          // TX0 buffer - forced
    }

    // ensure no pending MCP2515 CAN data transmission for this Tx buffer
    rslt = MCP2515_ReadRegister(txb);
    if ((rslt & TXB_TXREQ_BIT) != 0)
    {
        // force stop pending MCP2515 CAN data transmission
        MCP2515_ModifyRegister(txb, TXB_TXREQ_BIT, 0);
        delay(1);
    }

    // prepare and populate ID fields
    if (frame->can_id & CAN_EFF_FLAG)
    {
        MCP2515_PrepareExtId(id_data, frame->can_id);   // Extended frame
    }
    else
    {
        MCP2515_PrepareId(id_data, frame->can_id);      // Standard frame
    }
    MCP2515_WriteRegisters(txb + MCP2515_BUF_SIDH, id_data, sizeof(id_data));

    // populate DLC field
    if (frame->can_id & CAN_RTR_FLAG)
    {
        // indicate is RTR message
        MCP2515_WriteRegister(txb + MCP2515_BUF_DLC, MCP2515_RTR_MASK | frame->can_dlc);
    }
    else    // non-RTR message
    {
        MCP2515_WriteRegister(txb + MCP2515_BUF_DLC, frame->can_dlc);
    }

    // populate CAN data fields
    MCP2515_WriteRegisters(txb + MCP2515_BUF_DATA0, frame->can_data, frame->can_dlc);

    // initiate MCP2515 CAN data transmission
    MCP2515_ModifyRegister(txb, TXB_TXREQ_BIT, TXB_TXREQ_BIT);
    // Alternative: Send RTS Instruction for specified Tx buffer
    //              i.e.
    //              MCP2515_StartSPI();
    //              SPI_dev->transfer((MCP2515_RTS_TX0 - 1) + (1 << ((txb >> 4) - 3)));    // RTS Instruction
    //              MCP2515_EndSPI();

    // check for errors
    rslt = MCP2515_ReadRegister(txb);
    if ((rslt & (TXB_ABTF_BIT | TXB_MLOA_BIT | TXB_TXERR_BIT)) != 0)
    {
        return MCP2515_FAIL;
    }

    // Note: In order for the MCP2515 to consider a CAN data transmission to be
    //       complete, it *must* see a dominant (low) ACK response from some other
    //       CAN device on the attached CAN bus!
    //       This other device(s) *must* be operating at the same CAN speed as this
    //       sending CAN device!
    //       If no responding CAN device is on the CAN bus when the MCP2515 does a
    //       transmission, the MCP2515 will continuously forever repeat the transmission 
    //       until it gets a response or the MCU aborts the transmission. This is considered
    //       to be an "Acknowledge Error" and results in a TXERR being set in the TBnCTRL[3]
    //       register for the Tx buffer being transmitted. Error counts will also be
    //       indicated in the Transmit Error Counter (TEC) and associated error and warning
    //       bits in the EFLG register.

    while (1)
    {
        // check for transmission complete
        rslt = MCP2515_ReadRegister(txb);
        if ((rslt & TXB_TXREQ_BIT) == 0)
        {
            break;
        }
        delayMicroseconds(10);
        if (++cnt >= 25000)
        {
            // stop failed MCP2515 CAN data transmission
            MCP2515_ModifyRegister(txb, TXB_TXREQ_BIT, 0);

            return MCP2515_FAIL;
        }
    }
    // clear TX0 buffer empty interrupt
    MCP2515_ModifyRegister(MCP2515_CANINTF, MCP2515_TX0IF, 0);

    return MCP2515_OK;
}

// Prepare ID field data 
//  - supports standard 11-bit CAN data frames
void DLK_MCP2515::MCP2515_PrepareId(uint8_t id_data[], uint32_t can_id)
{
    uint32_t canid;

    canid = (can_id & CAN_SFF_MASK);
    id_data[MCP2515_SIDH] = (canid >> 3);
    id_data[MCP2515_SIDL] = ((canid & 0x07 ) << 5);

    canid = (can_id & 0xffff0000);
    id_data[MCP2515_EID8] = canid >> 16;    // Data0
    id_data[MCP2515_EID0] = canid >> 24;    // Data1
}

// Prepare Extended ID field data 
//  - supports extended 29-bit CAN data frames
void DLK_MCP2515::MCP2515_PrepareExtId(uint8_t id_data[], uint32_t can_id)
{
    uint32_t canid;

    canid = (can_id & CAN_EFF_MASK);

    id_data[MCP2515_EID0] = (canid & 0xFF);
    id_data[MCP2515_EID8] = (canid >> 8);

    canid = (can_id >> 16);
    id_data[MCP2515_SIDL] = (canid & 0x03);
    id_data[MCP2515_SIDL] += ((canid & 0x1C) << 3);
    id_data[MCP2515_SIDL] |= MCP2515_RXB_IDE;
    id_data[MCP2515_SIDH] = (canid >> 5 );
}

// Receive CAN data from MCP2515
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
uint8_t DLK_MCP2515::MCP2515_Recv(CAN_FRAME * frame)
{
    uint8_t status;

    // 1) Determine if CAN message has been received
    // 2) Determine Rx buffer containing CAN message
    status = MCP2515_CheckCAN_Rx();
    switch (status)
    {
        case MCP2515_NO_RX_MSG:
            return status;

        case RXM_RXB0_MSG:
            // 3) Get ID/EID from RXB0SIDH, RXB0SIDL, RXB0EID8, RXB0EID0
            // 4) Get RTR from RXB0CTRL
            // 5) Get DLC from RXB0DLC
            // 6) Get CAN data from RXB0D0 to RXB0D7
            MCP2515_ReadCAN_Msg(MCP2515_RXB0CTRL, frame);
            // 7) Notify MCP2515 that CAN message has been retrieved
            MCP2515_ModifyRegister(MCP2515_CANINTF, MCP2515_RX0IF, 0);
            frame->can_rxb = RXB0;
            break;

        case RXM_RXB1_MSG:
            // 3) Get ID/EID from RXB1SIDH, RXB1SIDL, RXB1EID8, RXB1EID0
            // 4) Get RTR from RXB1CTRL
            // 5) Get DLC from RXB1DLC
            // 6) Get CAN data from RXB1D0 to RXB1D7
            MCP2515_ReadCAN_Msg(MCP2515_RXB1CTRL, frame);
            // 7) Notify MCP2515 that CAN message has been retrieved
            MCP2515_ModifyRegister(MCP2515_CANINTF, MCP2515_RX1IF, 0);
            frame->can_rxb = RXB1;
            break;
    }

    return MCP2515_OK;
}

// 1) Determine if CAN message has been received
// 2) Determine Rx buffer containing CAN message
uint8_t DLK_MCP2515::MCP2515_CheckCAN_Rx(void)
{
    uint8_t status;

#if 1
    status = MCP2515_ReadRxStatus() & RXMS_MASK;      // get RX Status
    if (status == RXM_NO_MSG)
    {
        return MCP2515_NO_RX_MSG;
    }
    else if (status == RXM_RXBOTH_MSG)
    {
        return RXM_RXB0_MSG;    // indicate RXB0 when messages in both Rx buffers
    }
    return status;
#else
    status = MCP2515_ReadStatus() & (STAT_RX1IF | STAT_RX0IF);      // get Read Status
    if (status == 0)
    {
        return MCP2515_NO_RX_MSG;
    }
    else if (status & STAT_RX1IF)
    {
        return RXM_RXB0_MSG;
    }
    else if (status & STAT_RX0IF)
    {
        return RXM_RXB1_MSG;
    }
    else
    {
        return RXM_RXB0_MSG;    // indicate RXB0 when messages in both Rx buffers
    }
#endif
}

// 3) Get ID/EID from RXBnSIDH, RXBnSIDL, RXBnEID8, RXBnEID0 (includes IDE bit)
uint32_t DLK_MCP2515::MCP2515_ReadCAN_ID(uint8_t id_addr)
{
    uint8_t id_buf[4];
    uint32_t id;

    // get ID data
    MCP2515_ReadRegisters(id_addr, id_buf, sizeof(id_buf));  // RXBnSIDH, RXBnSIDL, RXBnEID8, RXBnEID0

    // accumulate ID
    id = (id_buf[MCP2515_SIDH] << 3) + (id_buf[MCP2515_SIDL] >> 5);

    // accumulate Extended frame ID
    if (id_buf[MCP2515_SIDL] & MCP2515_RXB_IDE)
    {
        id = (id << 2) + (id_buf[MCP2515_SIDL] & 0x03);
        id = (id << 8) + id_buf[MCP2515_EID8];
        id = (id << 8) + id_buf[MCP2515_EID0];
        id |= CAN_EFF_FLAG;             // merge in indication is Extended frame
    }

    return id;
}

// 3) Get ID/EID from RXBnSIDH, RXBnSIDL, RXBnEID8, RXBnEID0 (includes IDE bit)
// 4) Get RTR from RXBnCTRL (placed into frame->can_id)
// 5) Get DLC from RXBnDLC
// 6) Get CAN data from RXBnD0 to RXBnD7
void DLK_MCP2515::MCP2515_ReadCAN_Msg(uint8_t rxbn_addr, CAN_FRAME * frame)
{
    uint8_t can_rtr;

    // 3) Get ID/EID from RXBnSIDH, RXBnSIDL, RXBnEID8, RXBnEID0
    frame->can_id = MCP2515_ReadCAN_ID(rxbn_addr + MCP2515_BUF_SIDH);

    // 4) Get RTR from RXBnCTRL
    can_rtr = MCP2515_ReadRegister(rxbn_addr);
    if (can_rtr & RXRTR_BIT)
    {
        frame->can_id |= CAN_RTR_FLAG;      // merge in indication was standard RTR request
    }

    // 5) Get DLC (and extended RTR) from RXBnDLC
    frame->can_dlc = MCP2515_ReadRegister(rxbn_addr + MCP2515_BUF_DLC);
    if ((frame->can_id & CAN_EFF_FLAG) && (frame->can_dlc & MCP2515_RTR_MASK))
    {
        frame->can_id |= CAN_RTR_FLAG;      // merge in indication was extended RTR request
    }
    frame->can_dlc &= MCP2515_DLC_MASK;

    // 6) Get CAN data from RXBnD0 to RXBnD7
    MCP2515_ReadRegisters(rxbn_addr + MCP2515_BUF_DATA0, frame->can_data, frame->can_dlc);
}

// Setup callback for MCP2515 receive interrupts
// Note: Handling of interrupt callbacks from inside a C++ class is tricky!
//       The interrupt handler attached to the interrupt pin *must* be a static function
//       (or a non-class function outside of the class)!
//       To call the application callback, the 'this' pointer to the current
//       class object must be saved and in the interrupt handler is used to then call
//       a class function that is then able to call the application callback function.
//       See: https://www.onetransistor.eu/2019/05/arduino-class-interrupts-and-callbacks.html
//       See: https://github.com/adafruit/Adafruit_MCP2515
//       Note: This is only supported with 4 instances with interrupts!
uint8_t DLK_MCP2515::MCP2515_OnRxInterrupt(int int_pin, void (* callback)(CAN_FRAME *))
{
    if (digitalPinToInterrupt(int_pin) == NOT_AN_INTERRUPT)
    {
        return MCP2515_INVALID_INT;     // not a valid interrupt pin!
    }

#if (MAX_INTS > 0)
    // init MCP2515 Int input pin
    pinMode(int_pin, INPUT_PULLUP);

    // notify SPI driver that SPI operations will be occurring inside an interrupt handler
    SPI_dev->usingInterrupt(digitalPinToInterrupt(int_pin));

    // disable MCP2515 Rx interrupts
    MCP2515_ModifyRegister(MCP2515_CANINTE, (MCP2515_RX1IF | MCP2515_RX0IF), 0);

    MsgNdx = 0;

    if (instance1 == nullptr)      // available
    {
        instance1 = this;
        // attach MCP2515 Interrupt pin interrupt to static interrupt handler function
        attachInterrupt(digitalPinToInterrupt(int_pin), MCP2515_OnInterrupt1, LOW);
    }
#if (MAX_INTS > 1)
    else if (instance2 == nullptr) // available
    {
        instance2 = this;
        // attach MCP2515 Interrupt pin interrupt to static interrupt handler function
        attachInterrupt(digitalPinToInterrupt(int_pin), MCP2515_OnInterrupt2, LOW);
    }
#if (MAX_INTS > 2)
    else if (instance3 == nullptr) // available
    {
        instance3 = this;
        // attach MCP2515 Interrupt pin interrupt to static interrupt handler function
        attachInterrupt(digitalPinToInterrupt(int_pin), MCP2515_OnInterrupt3, LOW);
    }
    else if (instance4 == nullptr) // available
    {
        instance4 = this;
        // attach MCP2515 Interrupt pin interrupt to static interrupt handler function
        attachInterrupt(digitalPinToInterrupt(int_pin), MCP2515_OnInterrupt4, LOW);
    }
#endif
#endif
    else
#endif
    {
        return MCP2515_NO_AVAIL_INTS;   // all supported interrupt instances already in use!
    }

    // save the interrupt callback function
    MCP2515_InterruptHandler = callback;

    // clear any pending MCP2515 interrupts
    MCP2515_WriteRegister(MCP2515_CANINTF, 0);

    // enable MCP2515 Rx interrupts
    MCP2515_ModifyRegister(MCP2515_CANINTE, (MCP2515_RX1IF | MCP2515_RX0IF), (MCP2515_RX1IF | MCP2515_RX0IF));

    return MCP2515_OK;
}

#if (MAX_INTS > 0)
// This *must* be a static function to be used in attachInterrupt()
void DLK_MCP2515::MCP2515_OnInterrupt1(void)
{
    instance1->MCP2515_HandleInterrupt();    // invoke class-based handler
}

#if (MAX_INTS > 1)
// This *must* be a static function to be used in attachInterrupt()
void DLK_MCP2515::MCP2515_OnInterrupt2(void)
{
    instance2->MCP2515_HandleInterrupt();    // invoke class-based handler
}

#if (MAX_INTS > 2)
// This *must* be a static function to be used in attachInterrupt()
void DLK_MCP2515::MCP2515_OnInterrupt3(void)
{
    instance3->MCP2515_HandleInterrupt();    // invoke class-based handler
}

// This *must* be a static function to be used in attachInterrupt()
void DLK_MCP2515::MCP2515_OnInterrupt4(void)
{
    instance4->MCP2515_HandleInterrupt();    // invoke class-based handler
}
#endif
#endif
#endif

// MCP2515 common class-based Int pin handler
void DLK_MCP2515::MCP2515_HandleInterrupt(void)
{
    uint8_t ints;

    ints = MCP2515_ReadRegister(MCP2515_CANINTF);
    if (ints & (0xff & ~(MCP2515_RX1IF | MCP2515_RX0IF)))   // non-Rx interrupt
    {
        MCP2515_ModifyRegister(MCP2515_CANINTF, 0xfc, 0);   // discard/clear non-Rx interrupts
    }

    if (ints & (MCP2515_RX1IF | MCP2515_RX0IF))         // Rx interrupt
    {
        // receive CAN data from MCP2515
        if (MCP2515_Recv(&CAN_Frame[MsgNdx]) == MCP2515_OK)
        {
            // call application callback
            MCP2515_InterruptHandler(&CAN_Frame[MsgNdx]);

            if (++MsgNdx >= FRAME_CNT)
            {
                MsgNdx = 0;
            }
        }
    }
}
#endif

