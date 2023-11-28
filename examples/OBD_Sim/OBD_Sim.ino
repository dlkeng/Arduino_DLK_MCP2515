/* CAN OBD & UDS Simulator
 *
 *  Currently replies to some general OBD requests
 *  Place holders exist for what I've been able to locate on the Internet
 *  Does not currently support UDS correctly, just placeholders with mode descriptions
 *
 *  Written By: Cory J. Fowler  December 20th, 2016
 *
 *  Note:
 *   The MCP2515 interrupt pin is polled for received CAN messages, but the
 *   DLK_MCP2515 CAN Bus library is not using Rx interrupts.
 */
/*                Nano
   MCP2515 Pin  Arduino Pin
    ------------------------------
        VCC         5V
        GND         GND
        CS          D10
        SI(MOSI)    D11
        SO(MISO)    D12
        SCK         D13
        INT         D2
                                           _________________________
                                          |                         |
                                         -|TX0[D1]               VIN|-
                                         -|RX0[D0]               GND|-
              ________                   -|RST                   RST|-
             |        |                  -|GND                   +5V|-
             |    ~INT|------------------>|PD2[D2]              [A7]|-
             |        |   MOD_ID_PIN ---->|PD3[D3]              [A6]|-
             |        |                  -|PD4[D4]   [SCL/A5/D19]PC5|-
             |        |                  -|PD5[D5]   [SDA/A4/D18]PC4|-
             |        |                  -|PD6[D6]       [A3/D17]PC3|-
             |        |                  -|PD7[D7]       [A2/D16]PC2|-
             |        |                  -|PB0[D8]       [A1/D15]PC1|-
             |        |      LED_HB <-----|PB1[D9]       [A0/D14]PC0|-
             |     ~CS|<------------------|PB2[D10]             AREF|-
             |      SI|<------------------|PB3[D11]             3.3V|-
             |      SO|------------------>|PB4[D12]         [D13]PB5|------.
             |        |                   |         .-----.         |      |
             |        |                   |_________| USB |_________|      |
             |        |                             '-----'                |
             |        |                           Arduino Nano             |
             |        |                                                    |
             |     SCK|<---------------------------------------------------'
             |________|
              MCP2515
 */

#include <DLK_MCP2515.h>    // MCP2515 CAN Bus library

#define MCP2515_CS_PIN      10
#define MCP2515_INT_PIN     2
#define SPI_CLOCK           2000000         // 2 Mbps
#define CAN_SPEED           CAN_250KBPS

#define MODULE_ID_PIN       3   // the module ID selection pin
                                // 0 = 7E0/8 = Engine ECM
                                // 1 = 7E1/9 = Transmission ECM

#define TIMER_EXPIRED(start, interval)  ((millis() - start) >= interval)
#define HEARTBEAT_OFF_INTERVAL  950     // mS
#define HEARTBEAT_ON_INTERVAL   50      // mS

#define LED_PIN             8   // the heartbeat LED pin

#define LED_ON      HIGH
#define LED_OFF     LOW

//#define EXTRAS

#define PAD 0x00

// What CAN ID type?  Standard (1) or Extended
#define STANDARD    1

// 7E0/8 = Engine ECM
// 7E1/9 = Transmission ECM
#define ENGINE_ECM       0x7E0
#define XMISSN_ECM       0x7E1

#if STANDARD == 1
#define FUNCTIONAL_ID   0x7DF
#else   // Extended
#define REPLY_ID        0x18DAF101
#define LISTEN_ID       0x18DA01F1
#define FUNCTIONAL_ID   0x18DB33F1
#endif
#define RESPONSE_BIT    0x40

// ISO-TP items
#define TP_SF       0x00    // ISO-TP Single Frame
#define TP_FF       0x10    // ISO-TP First Frame
#define TP_CF       0x20    // ISO-TP Consecutive Frame
#define TP_FC       0x30    // ISO-TP Flow Control Frame
#define CONT        0       // Continue
#define WAIT        1       // Wait
#define ABORT       2       // Overflow/Abort
#define PADDING     0xcc

// CAN RX Variables
unsigned long rxId;
uint8_t dlc;
uint8_t TxMsgBuf[80];

bool ClearedDTCs = false;
uint16_t Listen_ID;
uint16_t Reply_ID;

// CAN Interrupt and Chip Select
#define CAN0_INT    MCP2515_INT_PIN             // Set CAN0 INT to pin 2
DLK_MCP2515 CAN0(SPI_CLOCK, MCP2515_CS_PIN);    // Set CAN0 CS to pin 10

void setup()
{
    // init heartbeat LED
    pinMode(LED_PIN, OUTPUT);
    LED_off();

    // Configuring pin for /INT input
    pinMode(CAN0_INT, INPUT_PULLUP);

    // init module ID selection input pin
    pinMode(MODULE_ID_PIN, INPUT_PULLUP);

    Serial.begin(115200);

    // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
//    if (CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_20MHZ) == CAN_OK)
    // Initialize MCP2515 running at 8MHz with a baudrate of 250kb/s
    if (CAN0.MCP2515_Init(CAN_SPEED) == MCP2515_OK)
    {
        Serial.println("MCP2515 Initialized Successfully!");
    }
    else
    {
        Serial.println("Error Initializing MCP2515...");
        while (1)
        { ; }
    }

    if (digitalRead(MODULE_ID_PIN))     // 1 = 7E1/9 = Transmission ECM
    {
        Listen_ID = XMISSN_ECM;
    }
    else    // 0 = 7E0/8 = Engine ECM
    {
        Listen_ID = ENGINE_ECM;
    }
    Reply_ID = (Listen_ID | 0x08);

#if STANDARD == 1
    // Standard ID Filters
    CAN0.MCP2515_SetMask(0, 0x7F0, 0x00, 0x00);             // Init first mask...
    CAN0.MCP2515_SetFilter(0, FUNCTIONAL_ID, 0x00, 0x00);   // Init first filter...
    CAN0.MCP2515_SetFilter(1, Listen_ID, 0x00, 0x00);       // Init second filter...

    CAN0.MCP2515_SetMask(1, 0x7F0, 0x00, 0x00);             // Init second mask...
    CAN0.MCP2515_SetFilter(2, FUNCTIONAL_ID, 0x00, 0x00);   // Init third filter...
    CAN0.MCP2515_SetFilter(3, Listen_ID, 0x00, 0x00);       // Init fourth filter...
    CAN0.MCP2515_SetFilter(4, FUNCTIONAL_ID, 0x00, 0x00);   // Init fifth filter...
    CAN0.MCP2515_SetFilter(5, Listen_ID, 0x00, 0x00);       // Init sixth filter...
#else
    // Extended ID Filters
    CAN0.MCP2515_SetExtMask(0, 0x10FFFF00);             // Init first mask...
    CAN0.MCP2515_SetExtFilter(0, 0x10DB3300);           // Init first filter...
    CAN0.MCP2515_SetExtFilter(1, 0x10DA0100);           // Init second filter...

    CAN0.MCP2515_SetExtMask(1, 0x10FFFF00);             // Init second mask...
    CAN0.MCP2515_SetExtFilter(2, 0x10DB3300);           // Init third filter...
    CAN0.MCP2515_SetExtFilter(3, 0x10DA0100);           // Init fourth filter...
    CAN0.MCP2515_SetExtFilter(4, 0x10DB3300);           // Init fifth filter...
    CAN0.MCP2515_SetExtFilter(5, 0x10DA0100);           // Init sixth filter...
#endif

    CAN0.MCP2515_SetMode(MODE_NORMAL);                  // Set operation mode to normal so the MCP2515 sends acks to received data.

    Serial.println("OBD-II CAN Simulator");
}

void loop()
{
    CAN_FRAME frame;

    if (!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
    {
        //CAN0.readMsgBuf(&rxId, &dlc, rxBuf);        
        if (CAN0.MCP2515_Recv(&frame) == MCP2515_OK)    // Get CAN data
        {
            // First request from most adapters...
            if (frame.can_id == FUNCTIONAL_ID)
            {
                if (frame.can_dlc >= 3)
                {
                    obdReq(frame.can_data);
                }
                else
                {
                    Serial.println("Invalid OBD-II Message!");
                }
            }
        }
    }

    DoHeartbeat();
}

// Data |  0  |   1  |  2  | 3 | 4 | 5 | 6 | 7 |
//      | len | mode | pid |   |   |   |   |   |
void obdReq(uint8_t * data)
{
    uint8_t len = 0;
    uint8_t mode = data[1] & 0x0F;      // Service ID / mode
    uint8_t pid = data[2];              // Parameter ID
    bool tx = false;

    memset(TxMsgBuf, 0xcc, 8);

char msgstring[64];

sprintf(msgstring, "Mode: $%02X, PID:$%02X", mode, pid);
Serial.println(msgstring);

    //=============================================================================
    // MODE $01 - Show current data
    //=============================================================================
    if (mode == 0x01)
    {
        if (pid == 0x00)        // Supported PIDs 01-20
        {
            TxMsgBuf[0] = 0x80;
            TxMsgBuf[1] = 0x38;
            TxMsgBuf[2] = 0x00;
            TxMsgBuf[3] = 0x01;
            len = 4;
            tx = true;
            // 06 41 00 80 38 00 01 00
        }
        else if (pid == 0x01)   // Monitor status since DTs cleared
        {
            bool MIL = true;
            uint8_t DTC = 5;

            TxMsgBuf[0] = (MIL << 7) | (DTC & 0x7F);
            TxMsgBuf[1] = 0x07;
            TxMsgBuf[2] = 0xFF;
            TxMsgBuf[3] = 0x00;
            len = 4;
            tx = true;
            // 06 41 01 85 07 FF 00 00
        }
#ifdef EXTRAS
        else if (pid == 0x02)    // Freeze DTC
        {
        }
#endif
        else if (pid == 0x03)    // Fuel system status
        {
            TxMsgBuf[0] = 0xFA;
            len = 1;
            tx = true;
            // 03 41 03 FA 00 00 00 00 
        }
        else if (pid == 0x04)    // Calculated engine load
        {
            TxMsgBuf[0] = 0x6A;     // % = 100/255 * 0x6a = 41.5686%
            len = 1;
            tx = true;
        }
        else if (pid == 0x05)    // Engine coolant temperature
        {
            TxMsgBuf[0] = 0xFA;
            len = 1;
            tx = true;
        }
#ifdef EXTRAS
        else if (pid == 0x06)    // Short term fuel trim - Bank 1
        {
        }
        else if (pid == 0x07)    // Long term fuel trim - Bank 1
        {
        }
        else if (pid == 0x08)    // Short term fuel trim - Bank 2
        {
        }
        else if (pid == 0x09)    // Long term fuel trim - Bank 2
        {
        }
        else if (pid == 0x0A)    // Fuel pressure (gauge)
        {
        }
#endif
        else if (pid == 0x0B)    // Intake manifold absolute pressure
        {
            TxMsgBuf[0] = 0x64;
            len = 1;
            tx = true;
        }
        else if (pid == 0x0C)    // Engine RPM
        {
            TxMsgBuf[0] = 0x9C;
            TxMsgBuf[1] = 0x40;
            len = 2;
            tx = true;
        }
        else if (pid == 0x0D)    // Vehicle speed
        {
            TxMsgBuf[0] = 0xFA;
            len = 1;
            tx = true;
        }
#ifdef EXTRAS
        else if (pid == 0x0E)    // Timing advance
        {
        }
#endif
        else if (pid == 0x0F)    // Intake air temperature
        {
            TxMsgBuf[0] = 0xFA;
            len = 1;
            tx = true;
        }
#ifdef EXTRAS
        else if (pid == 0x10)    // MAF air flow rate
        {
        }
#endif
        else if (pid == 0x11)    // Throttle position
        {
            TxMsgBuf[0] = 0xFA;
            len = 1;
            tx = true;
        }
#ifdef EXTRAS
        else if (pid == 0x12)    // Commanded secondary air status
        {
        }
        else if (pid == 0x13)    // Oxygen sensors present (in 2 banks)
        {
        }
        else if (pid == 0x14)    // Oxygen Sensor 1 (Voltage & Trim)
        {
        }
        else if (pid == 0x15)    // Oxygen Sensor 2 (Voltage & Trim)
        {
        }
        else if (pid == 0x16)    // Oxygen Sensor 3 (Voltage & Trim)
        {
        }
        else if (pid == 0x17)    // Oxygen Sensor 4 (Voltage & Trim)
        {
        }
        else if (pid == 0x18)    // Oxygen Sensor 5 (Voltage & Trim)
        {
        }
        else if (pid == 0x19)    // Oxygen Sensor 6 (Voltage & Trim)
        {
        }
        else if (pid == 0x1A)    // Oxygen Sensor 7 (Voltage & Trim)
        {
        }
        else if (pid == 0x1B)    // Oxygen Sensor 8 (Voltage & Trim)
        {
        }
        else if (pid == 0x1C)    // OBD standards this vehicle conforms to
        {
        }
        else if (pid == 0x1D)    // Oxygen sensors present (in 4 banks)
        {
        }
        else if (pid == 0x1E)    // Auxiliary input status
        {
        }
        else if (pid == 0x1F)    // Run time since engine start
        {
        }
#endif
        else if (pid == 0x20)    // Supported PIDs 21-40
        {
            TxMsgBuf[0] = 0x80;
            TxMsgBuf[1] = 0x00;
            TxMsgBuf[2] = 0x00;
            TxMsgBuf[3] = 0x01;
            len = 4;
            tx = true;
        }
        else if (pid == 0x21)    // Distance traveled with MIL on
        {
            TxMsgBuf[0] = 0x00;
            TxMsgBuf[1] = 0x23;
            len = 2;
            tx = true;
        }
#ifdef EXTRAS
        else if (pid == 0x22)    // Fuel rail pressure (Relative to Manifold Vacuum)
        {
        }
        else if (pid == 0x23)    // Fuel rail gauge pressure (diesel or gasoline direct injection)
        {
        }
        else if (pid == 0x24)    // Oxygen Sensor 1 (Fuel to Air & Voltage)
        {
        }
        else if (pid == 0x25)    // Oxygen Sensor 2 (Fuel to Air & Voltage)
        {
        }
        else if (pid == 0x26)    // Oxygen Sensor 3 (Fuel to Air & Voltage)
        {
        }
        else if (pid == 0x27)    // Oxygen Sensor 4 (Fuel to Air & Voltage)
        {
        }
        else if (pid == 0x28)    // Oxygen Sensor 5 (Fuel to Air & Voltage)
        {
        }
        else if (pid == 0x29)    // Oxygen Sensor 6 (Fuel to Air & Voltage)
        {
        }
        else if (pid == 0x2A)    // Oxygen Sensor 7 (Fuel to Air & Voltage)
        {
        }
        else if (pid == 0x2B)    // Oxygen Sensor 8 (Fuel to Air & Voltage)
        {
        }
        else if (pid == 0x2C)    // Commanded EGR
        {
        }
        else if (pid == 0x2D)    // EGR Error
        {
        }
        else if (pid == 0x2E)    // Commanded evaporative purge
        {
        }
        else if (pid == 0x2F)    // Fuel tank level input
        {
        }
        else if (pid == 0x30)    // Warm-ups since codes cleared
        {
        }
        else if (pid == 0x31)    // Distance traveled since codes cleared
        {
        }
        else if (pid == 0x32)    // Evap. System Vapor Pressure
        {
        }
        else if (pid == 0x33)    // Absolute Barometric Pressure
        {
        }
        else if (pid == 0x34)    // Oxygen Sensor 1 (Fuel to Air & Current)
        {
        }
        else if (pid == 0x35)    // Oxygen Sensor 2 (Fuel to Air & Current)
        {
        }
        else if (pid == 0x36)    // Oxygen Sensor 3 (Fuel to Air & Current)
        {
        }
        else if (pid == 0x37)    // Oxygen Sensor 4 (Fuel to Air & Current)
        {
        }
        else if (pid == 0x38)    // Oxygen Sensor 5 (Fuel to Air & Current)
        {
        }
        else if (pid == 0x39)    // Oxygen Sensor 6 (Fuel to Air & Current)
        {
        }
        else if (pid == 0x3A)    // Oxygen Sensor 7 (Fuel to Air & Current)
        {
        }
        else if (pid == 0x3B)    // Oxygen Sensor 8 (Fuel to Air & Current)
        {
        }
        else if (pid == 0x3C)    // Catalyst Temperature: Bank 1, Sensor 1
        {
        }
        else if (pid == 0x3D)    // Catalyst Temperature: Bank 2, Sensor 1
        {
        }
        else if (pid == 0x3E)    // Catalyst Temperature: Bank 1, Sensor 2
        {
        }
        else if (pid == 0x3F)    // Catalyst Temperature: Bank 2, Sensor 2
        {
        }
#endif
        else if (pid == 0x40)    // Supported PIDs 41-60
        {
            TxMsgBuf[0] = 0x00;
            TxMsgBuf[1] = 0x08;
            TxMsgBuf[2] = 0x00;
            TxMsgBuf[3] = 0x0D;
            len = 4;
            tx = true;
        }
#ifdef EXTRAS
        else if (pid == 0x41)    // Monitor status this drive cycle
        {
        }
        else if (pid == 0x42)    // Control module voltage
        {
        }
        else if (pid == 0x43)    // Absolute load value
        {
        }
        else if (pid == 0x44)    // Fuel-Air commanded equivalence ratio
        {
        }
        else if (pid == 0x45)    // Relative throttle position
        {
        }
        else if (pid == 0x46)    // Ambient air temperature
        {
        }
        else if (pid == 0x47)    // Absolute throttle position B
        {
        }
        else if (pid == 0x48)    // Absolute throttle position C
        {
        }
        else if (pid == 0x49)    // Accelerator pedal position D
        {
        }
        else if (pid == 0x4A)    // Accelerator pedal position E
        {
        }
        else if (pid == 0x4B)    // Accelerator pedal position F
        {
        }
        else if (pid == 0x4C)    // Commanded throttle actuator
        {
        }
#endif
        else if (pid == 0x4D)    // Time run with MIL on
        {
            TxMsgBuf[0] = 0x00;
            TxMsgBuf[1] = 0x3C;
            len = 2;
            tx = true;
        }
#ifdef EXTRAS
        else if (pid == 0x4E)    // Time since troble codes cleared
        {
        }
        else if (pid == 0x4F)    // Time since trouble codes cleared
        {
        }
        else if (pid == 0x50)    // Maximum value for Fuel-Air equivalence ratio, oxygen sensor voltage, oxygen sensro current, and intake manifold absolute-pressure
        {
        }
        else if (pid == 0x51)    // Fuel Type
        {
        }
        else if (pid == 0x52)    // Ethanol Fuel %
        {
        }
        else if (pid == 0x53)    // Absolute evap system vapor pressure
        {
        }
        else if (pid == 0x54)    // Evap system vapor pressure
        {
        }
        else if (pid == 0x55)    // Short term secondary oxygen sensor trim, A: bank 1, B: bank 3
        {
        }
        else if (pid == 0x56)    // Long term secondary oxygen sensor trim, A: bank 1, B: bank 3
        {
        }
        else if (pid == 0x57)    // Short term secondary oxygen sensor trim, A: bank 2, B: bank 4
        {
        }
        else if (pid == 0x58)    // Long term secondary oxygen sensor trim, A: bank 2, B: bank 4
        {
        }
        else if (pid == 0x59)    // Fuel rail absolute pressure
        {
        }
        else if (pid == 0x5A)    // Relative accelerator pedal position
        {
        }
        else if (pid == 0x5B)    // Hybrid battery pack remaining life
        {
        }
#endif
        else if (pid == 0x5C)    // Engine oil Temperature
        {
            TxMsgBuf[0] = 0x1E;
            len = 1;
            tx = true;
        }
        else if (pid == 0x5D)    // Fuel injection timing
        {
            TxMsgBuf[0] = 0x61;
            TxMsgBuf[1] = 0x80;
            len = 2;
            tx = true;
        }
        else if (pid == 0x5E)    // Engine fuel rate
        {
            TxMsgBuf[0] = 0x07;
            TxMsgBuf[1] = 0xD0;
            len = 2;
            tx = true;
        }
#ifdef EXTRAS
        else if (pid == 0x5F)    // Emissions requirements to which vehicle is designed
        {
        }
#endif
        else if (pid == 0x60)    // Supported PIDs 61-80
        {
            TxMsgBuf[0] = 0x00;
            TxMsgBuf[1] = 0x00;
            TxMsgBuf[2] = 0x00;
            TxMsgBuf[3] = 0x01;
            len = 4;
            tx = true;
        }
#ifdef EXTRAS
        else if (pid == 0x61)    // Driver's demand engine - percent torque
        {
        }
        else if (pid == 0x62)    // Actual engine - percent torque
        {
        }
        else if (pid == 0x63)    // Engine reference torque
        {
        }
        else if (pid == 0x64)    // Engine percent torque data
        {
        }
        else if (pid == 0x65)    // Auxiliary input / output supported
        {
        }
        else if (pid == 0x66)    // Mas air flow sensor
        {
        }
        else if (pid == 0x67)    // Engine coolant temperature
        {
        }
        else if (pid == 0x68)    // Intake air temperature sensor
        {
        }
        else if (pid == 0x69)    // Commanded EGR and EGR error
        {
        }
        else if (pid == 0x6A)    // Commanded Diesel intake air flow control and relative intake air flow position
        {
        }
        else if (pid == 0x6B)    // Exhaust gas recirculation temperature
        {
        }
        else if (pid == 0x6C)    // Commanded throttle actuator control and relative throttle position
        {
        }
        else if (pid == 0x6D)    // Fuel pressure control system
        {
        }
        else if (pid == 0x6E)    // Injection pressure control system
        {
        }
        else if (pid == 0x6F)    // Turbocharger compressor inlet pressure
        {
        }
        else if (pid == 0x70)    // Boost pressure control
        {
        }
        else if (pid == 0x71)    // Variable Geometry turbo sensor
        {
        }
        else if (pid == 0x72)    // Wastegate control
        {
        }
        else if (pid == 0x73)    // Exhaust pressure
        {
        }
        else if (pid == 0x74)    // Turbocharger RPM
        {
        }
        else if (pid == 0x75)    // Turbocharger temperature
        {
        }
        else if (pid == 0x76)    // Turbocharger temperature
        {
        }
        else if (pid == 0x77)    // Charge air cooler temperature (CACT)
        {
        }
        else if (pid == 0x78)    // Exhaust Gas Temperature (EGT) bank 1
        {
        }
        else if (pid == 0x79)    // Exhaust Gas Temperature (EGT) bank 2
        {
        }
        else if (pid == 0x7A)    // Diesel particulate filter (DPF)
        {
        }
        else if (pid == 0x7B)    // Diesel particulate filter (DPF)
        {
        }
        else if (pid == 0x7C)    // Diesel particulate filter (DPF) temperature
        {
        }
        else if (pid == 0x7D)    // NOx NTE control area status
        {
        }
        else if (pid == 0x7E)    // PM NTE control area status
        {
        }
        else if (pid == 0x7F)    // Engine run time
        {
        }
#endif
        else if (pid == 0x80)    // Supported PIDs 81-A0
        {
            TxMsgBuf[0] = 0x00;
            TxMsgBuf[1] = 0x00;
            TxMsgBuf[2] = 0x00;
            TxMsgBuf[3] = 0x01;
            len = 4;
            tx = true;
        }
#ifdef EXTRAS
        else if (pid == 0x81)    // Engine run time for Auxiliary Emissions Control Device (AECD)
        {
        }
        else if (pid == 0x82)    // Engine run time for Auxiliary Emissions Control Device (AECD)
        {
        }
        else if (pid == 0x83)    // NOx sensor
        {
        }
        else if (pid == 0x84)    // Manifold surface temperature
        {
        }
        else if (pid == 0x85)    // NOx reqgent system
        {
        }
        else if (pid == 0x86)    // Particulate Matter (PM) sensor
        {
        }
        else if (pid == 0x87)    // Intake manifold absolute pressure
        {
        }
#endif
        else if (pid == 0xA0)    // Supported PIDs A1-C0
        {
            TxMsgBuf[0] = 0x00;
            TxMsgBuf[1] = 0x00;
            TxMsgBuf[2] = 0x00;
            TxMsgBuf[3] = 0x01;
            len = 4;
            tx = true;
        }
        else if (pid == 0xC0)    // Supported PIDs C1-E0
        {
            TxMsgBuf[0] = 0x00;
            TxMsgBuf[1] = 0x00;
            TxMsgBuf[2] = 0x00;
            TxMsgBuf[3] = 0x01;
            len = 4;
            tx = true;
        }
        else if (pid == 0xE0)    // Supported PIDs E1-FF?
        {
            TxMsgBuf[0] = 0x00;
            TxMsgBuf[1] = 0x00;
            TxMsgBuf[2] = 0x00;
            TxMsgBuf[3] = 0x00;
            len = 4;
            tx = true;
        }
        else
        {
            unsupported(mode, pid);
        }
    }

    //=============================================================================
    // MODE $02 - Show freeze frame data
    //=============================================================================
    else if (mode == 0x02)
    {
        unsupported(mode, pid);
    }

    //=============================================================================
    // MODE $03 - Show stored DTCs
    //=============================================================================
    else if (mode == 0x03)
    {
        //                         U00BA        P0011      B0013       B1045       B2031
        const uint8_t DTCs[] = { 0xC0, 0xBA, 0x00, 0x11, 0x80, 0x13, 0x90, 0x45, 0xA0, 0x31 };
        if (!ClearedDTCs)
        {
            pid = 0x05;         // DTC count
            iso_tp(mode, pid, sizeof(DTCs), DTCs);
        }
        else
        {
            const uint8_t none = 0x00;
            pid = 0x00;         // dummy
            iso_tp(mode, pid, 1, &none);
        }
    }

    //=============================================================================
    // MODE $04 - Clear DTCs and stored values
    //=============================================================================
    else if (mode == 0x04)
    {
        // Need to clear DTCs.  We just acknowledge the command for now.
        len = 0;
        ClearedDTCs = true;
        tx = true;
    }

    //=============================================================================
    // MODE $05 - Test Results, oxygen sensor monitoring (non CAN only)
    //=============================================================================
    else if (mode == 0x05)
    {
        unsupported(mode, pid);
    }

    //=============================================================================
    // MODE $06 - Test Results, On-Board Monitoring (Oxygen sensor monitoring for CAN only)
    //=============================================================================
    else if (mode == 0x06)
    {
        if (pid == 0x00)        // Supported PIDs 01-20
        {
            TxMsgBuf[0] = 0x00;
            TxMsgBuf[1] = 0x00;
            TxMsgBuf[2] = 0x00;
            TxMsgBuf[3] = 0x00;
            len = 4;
            tx = true;
        }
        else
        {
            unsupported(mode, pid);
        }
    }

    //=============================================================================
    // MODE $07 - Show pending DTCs (Detected during current or last driving cycle)
    //=============================================================================
    else if (mode == 0x07)
    {
        //                         U00BA        P0011      B0013       B1045       B2031
        const uint8_t DTCs[] = { 0xC0, 0xBA, 0x00, 0x11, 0x80, 0x13, 0x90, 0x45, 0xA0, 0x31 };
        if (!ClearedDTCs)
        {
            pid = 0x05;         // DTC count
            iso_tp(mode, pid, sizeof(DTCs), DTCs);
        }
        else
        {
            const uint8_t none = 0x00;
            pid = 0x00;         // dummy
            iso_tp(mode, pid, 1, &none);
        }
    }

    //=============================================================================
    // MODE $08 - Control operation of on-board component/system
    //=============================================================================
    else if (mode == 0x08)
    {
        unsupported(mode, pid);
    }

    //=============================================================================
    // MODE $09 - Request vehicle information
    //=============================================================================
    else if (mode == 0x09)
    {
        if (pid == 0x00)        // Supported PIDs 01-20
        {
            TxMsgBuf[0] = 0x54;
            TxMsgBuf[1] = 0x40;
            TxMsgBuf[2] = 0x00;
            TxMsgBuf[3] = 0x00;
            len = 4;
            tx = true;
        }
#ifdef EXTRAS
        else if (pid == 0x01)    // VIN message count for PID 02. (Only for ISO 9141-2, ISO 14230-4 and SAE J1850.)
        {
        }
#endif
        else if (pid == 0x02)    // VIN (17 to 20 Bytes) Uses ISO-TP
        {
            const uint8_t VIN[] = { "1ZVBP8AM7D5220181" };

            TxMsgBuf[0] = 0x01;
            memcpy(&TxMsgBuf[1], VIN, sizeof(VIN));
            iso_tp(mode, pid, 1 + sizeof(VIN) - 1, TxMsgBuf);
        }
#ifdef EXTRAS
        else if (pid == 0x03)    // Calibration ID message count for PID 04. (Only for ISO 9141-2, ISO 14230-4 and SAE J1850.)
        {
        }
#endif
        else if (pid == 0x04)    // Calibration ID
        {
            const uint8_t CID[] = { "Arduino OBDIIsimQRST" };

            TxMsgBuf[0] = 0x01;
            memcpy(&TxMsgBuf[1], CID, sizeof(CID));
            iso_tp(mode, pid, 1 + sizeof(CID) - 1, TxMsgBuf);
        }
#ifdef EXTRAS
        else if (pid == 0x05)    // Calibration Verification Number (CVN) message count for PID 06. (Only for ISO 9141-2, ISO 14230-4 and SAE J1850.)
        {
        }
#endif
        else if (pid == 0x06)    // CVN
        {
            const uint8_t CVN[] = { "BBB\"CCC" };

            TxMsgBuf[0] = 0x01;
            TxMsgBuf[1] = 0x11;
            memcpy(&TxMsgBuf[2], CVN, sizeof(CVN));
            iso_tp(mode, pid, 2 + sizeof(CVN) - 1, TxMsgBuf);
        }
#ifdef EXTRAS
        else if (pid == 0x07)    // In-use performance tracking message count for PID 08 and 0B. (Only for ISO 9141-2, ISO 14230-4 and SAE J1850.)
        {
        }
        else if (pid == 0x08)    // In-use performance tracking for spark ignition vehicles.
        {
        }
        else if (pid == 0x09)    // ECU name message count for PID 0A.
        {
        }
#endif
        else if (pid == 0x0A)    // ECM Name
        {
            const uint8_t ECMname[] = { "ACM" "\x00" "-ArduinoOBDIIsim" };

            TxMsgBuf[0] = 0x01;
            memcpy(&TxMsgBuf[1], ECMname, sizeof(ECMname));
            iso_tp(mode, pid, 1 + sizeof(ECMname) - 1, TxMsgBuf);
        }
#ifdef EXTRAS
        else if (pid == 0x0B)    // In-use performance tracking for compression ignition vehicles.
        {
        }
        else if (pid == 0x0C)    // ESN message count for PID 0D.
        {
        }
#endif
        else if (pid == 0x0D)    // ESN
        {
            const uint8_t ESN[] = { "Arduino-OBDIIsim" };

            TxMsgBuf[0] = 0x01;
            memcpy(&TxMsgBuf[1], ESN, sizeof(ESN));
            iso_tp(mode, pid, 1 + sizeof(ESN) - 1, TxMsgBuf);
        }
        else
        {
            unsupported(mode, pid);
        }
    }

    //=============================================================================
    // MODE $0A - Show permanent DTCs
    //=============================================================================
    else if (mode == 0x0A)
    {
        //                         U00BA        P0011      B0013       B1045       B2031
        const uint8_t DTCs[] = { 0xC0, 0xBA, 0x00, 0x11, 0x80, 0x13, 0x90, 0x45, 0xA0, 0x31 };
        pid = 0x05;         // DTC count
        iso_tp(mode, pid, sizeof(DTCs), DTCs);
    }

    // UDS Modes: Diagonstic and Communications Management =======================================
    //=============================================================================
    // MODE $10 - Diagnostic Session Control
    //=============================================================================
#ifdef EXTRAS
    else if (mode == 0x10)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    //=============================================================================
    // MODE $11 - ECU Reset
    //=============================================================================
    else if (mode == 0x11)
    {
        TxMsgBuf[0] = mode;
        iso_tp(mode, pid, 1, TxMsgBuf);
    }

    //=============================================================================
    // MODE $27 - Security Access
    //=============================================================================
    else if (mode == 0x27)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    //=============================================================================
    // MODE $28 - Communication Control
    //=============================================================================
    else if (mode == 0x28)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    //=============================================================================
    // MODE $3E - Tester Present
    //=============================================================================
    else if (mode == 0x3E)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    //=============================================================================
    // MODE $83 - Access Timing Parameters
    //=============================================================================
    else if (mode == 0x83)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    //=============================================================================
    // MODE $84 - Secured Data Transmission
    //=============================================================================
    else if (mode == 0x84)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    //=============================================================================
    // MODE $85 - Control DTC Sentings
    //=============================================================================
    else if (mode == 0x85)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    //=============================================================================
    // MODE $86 - Response On Event
    //=============================================================================
    else if (mode == 0x86)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    //=============================================================================
    // MODE $87 - Link Control
    //=============================================================================
    else if (mode == 0x87)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    // UDS Modes: Data Transmission ==============================================================
    //=============================================================================
    // MODE $22 - Read Data By Identifier
    //=============================================================================
    else if (mode == 0x22)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    //=============================================================================
    // MODE $23 - Read Memory By Address
    //=============================================================================
    else if (mode == 0x23)
    {
        TxMsgBuf[0] = mode;
        iso_tp(mode, pid, 1, TxMsgBuf);
    }

    //=============================================================================
    // MODE $24 - Read Scaling Data By Identifier
    //=============================================================================
    else if (mode == 0x24)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    //=============================================================================
    // MODE $2A - Read Data By Periodic Identifier
    //=============================================================================
    else if (mode == 0x2A)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    //=============================================================================
    // MODE $2C - Dynamically Define Data Identifier
    //=============================================================================
    else if (mode == 0x2C)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    //=============================================================================
    // MODE $2E - Write Data By Identifier
    //=============================================================================
    else if (mode == 0x2E)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    //=============================================================================
    // MODE $3D - Write Memory By Address
    //=============================================================================
    else if (mode == 0x3D)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    // UDS Modes: Stored Data Transmission =======================================================
    //=============================================================================
    // MODE $14 - Clear Diagnostic Information
    //=============================================================================
    else if (mode == 0x14)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    //=============================================================================
    // MODE $19 - Read DTC Information
    //=============================================================================
    else if (mode == 0x19)
    {
        TxMsgBuf[0] = mode;
        iso_tp(mode, pid, 1, TxMsgBuf);
    }

    // UDS Modes: Input Output Control ===========================================================
    //=============================================================================
    // MODE $2F - Input Output Control By Identifier
    //=============================================================================
    else if (mode == 0x2F)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    // UDS Modes: Remote Activation of Routine ===================================================
    //=============================================================================
    // MODE $31 - Routine Control
    //=============================================================================
    else if (mode == 0x2F)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    // UDS Modes: Upload / Download ==============================================================
    //=============================================================================
    // MODE $34 - Request Download
    //=============================================================================
    else if (mode == 0x34)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    //=============================================================================
    // MODE $35 - Request Upload
    //=============================================================================
    else if (mode == 0x35)
    {
        TxMsgBuf[0] = mode;
        iso_tp(mode, pid, 1, TxMsgBuf);
    }

    //=============================================================================
    // MODE $36 - Transfer Data
    //=============================================================================
    else if (mode == 0x36)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    //=============================================================================
    // MODE $37 - Request Transfer Exit
    //=============================================================================
    else if (mode == 0x37)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }

    //=============================================================================
    // MODE $38 - Request File Transfer
    //=============================================================================
    else if (mode == 0x38)
    {
        TxMsgBuf[0] = mode;
        TxMsgBuf[1] = 0x00;
        iso_tp(mode, pid, 2, TxMsgBuf);
    }
#endif
    else
    {
        unsupported(mode, pid);
    }

    if (tx)
    {
        iso_tp(mode, pid, len, TxMsgBuf);
    }
}

// Generic debug serial output
void unsupported(uint8_t mode, uint8_t pid)
{
    negAck(mode, 0x12);
    unsupportedPrint(mode, pid);
}

// Generic debug serial output
void negAck(uint8_t mode, uint8_t reason)
{
    const uint8_t txData[] = { 0x03, 0x7F, mode, reason, PAD, PAD, PAD, PAD };

    CAN0.MCP2515_Send(Reply_ID, 8, (uint8_t *)txData);
}

// Generic debug serial output
void unsupportedPrint(uint8_t mode, uint8_t pid)
{
    char msgstring[64];

    sprintf(msgstring, "Mode $%02X: Unsupported PID $%02X requested!", mode, pid);
    Serial.println(msgstring);
}

// Blocking example of ISO transport (ISO-TP i.e. ISO 15765-2)
// See: https://en.wikipedia.org/wiki/ISO_15765-2
void iso_tp(uint8_t mode, uint8_t pid, int len, const uint8_t * data)
{
    CAN_FRAME frame;
    uint8_t tpData[8];
    int offset = 0;
    uint8_t index = 0;

    len += 2;
    if (len < 8)
    {
        // Single frame
Serial.println("Sending SF");

        if (len == 2)
        {
            tpData[0] = TP_SF | (len - 1);
            tpData[1] = (uint8_t)(RESPONSE_BIT | mode);
            tpData[2] = data[offset];
        }
        else
        {
            tpData[0] = TP_SF | len;
            tpData[1] = (uint8_t)(RESPONSE_BIT | mode);
            tpData[2] = pid;
        }

        for (uint8_t i = 3; i < 8; ++i)
        {
            if (i <= len)
            {
                tpData[i] = data[offset++];
            }
            else
            {
                tpData[i] = PADDING;
            }
        }
        CAN0.MCP2515_Send(Reply_ID, 8, tpData);
        return;
    }


    // First frame
Serial.println("Sending FF");
    tpData[0] = TP_FF | ((len >> 8) & 0x0F);
    tpData[1] = 0x00FF & len;
    tpData[2] = (uint8_t)(RESPONSE_BIT | mode);
    tpData[3] = pid;
    for (uint8_t i = 4; i < 8; i++)
    {
        tpData[i] = data[offset++];
    }
    CAN0.MCP2515_Send(Reply_ID, 8, tpData);
    index++;        // We sent a packet so increase our index.

    bool not_done = true;
    unsigned long sepPrev = millis();
    uint8_t sepInvl = 0;
    uint8_t frames = 0;
    bool lockout = false;

    while (not_done)
    {
        // Need to wait for flow frame
        if (!digitalRead(CAN0_INT))
        {
            // CAN0.readMsgBuf(&rxId, &dlc, rxBuf);
            if (CAN0.MCP2515_Recv(&frame) == MCP2515_OK)
            {
                if ((frame.can_id == Listen_ID) && ((frame.can_data[0] & 0xF0) == TP_FC))
                {
Serial.println("Received FC");
                    if ((frame.can_data[0] & 0x0F) == CONT)
                    {
                        // Continue
                        frames = frame.can_data[1];
                        sepInvl = frame.can_data[2];
                        lockout = true;
                    }
                    else if ((frame.can_data[0] & 0x0F) == WAIT)
                    {
                        // Wait
                        lockout = false;
                        delay(frame.can_data[2]);
                    }
                    else if ((frame.can_data[0] & 0x0F) == ABORT)
                    {
                        // Abort
                        not_done = false;
                        return;
                    }
                }
            }
        }

        if (((millis() - sepPrev) >= sepInvl) && lockout)
        {
            sepPrev = millis();
Serial.println("Sending CF");
            {
                char msgstring[32];

                sprintf(msgstring, "Offset: 0x%04X\tLen: 0x%04X", offset, (len - 2));
                Serial.println(msgstring);
            }

            tpData[0] = TP_CF | index++;         // consecutive frame
            for (uint8_t i = 1; i < 8; i++)
            {
                if (offset != (len - 2))
                {
                    tpData[i] = data[offset++];
                }
                else
                {
                    tpData[i] = PADDING;
                }
            }

            // Do consecutive frames (CF) as instructed via flow frame
            CAN0.MCP2515_Send(Reply_ID, 8, tpData);

            if (frames-- == 1)
            {
                lockout = false;
            }
        }

        if (offset == (len - 2))
        {
            not_done = false;
        }

        // Timeout
        if ((millis() - sepPrev) >= 1000)
        {
            not_done = false;
        }
    }
}

/*
 * NAME:
 *  void DoHeartbeat(void)
 *
 * PARAMETERS:
 *  None.
 *
 * WHAT:
 *  Do Heartbeat operation.
 *
 * RETURN VALUES:
 *  None.
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
void DoHeartbeat(void)
{
    static uint32_t last_HB_tick = 0;
    static bool last_HB_state = false;

    if (last_HB_state)
    {
        if (TIMER_EXPIRED(last_HB_tick, HEARTBEAT_ON_INTERVAL))
        {
            LED_off();
            last_HB_state = false;
            last_HB_tick = millis();
        }
    }
    else
    {
        if (TIMER_EXPIRED(last_HB_tick, HEARTBEAT_OFF_INTERVAL))
        {
            LED_on();
            last_HB_state = true;
            last_HB_tick = millis();
        }
    }
}

/*
 * NAME:
 *  void LED_on(void)
 *
 * PARAMETERS:
 *  None.
 *
 * WHAT:
 *  Turn on-board LED on.
 *
 * RETURN VALUES:
 *  None.
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
void LED_on(void)
{
    digitalWrite(LED_PIN, LED_ON);
}

/*
 * NAME:
 *  void LED_off(void)
 *
 * PARAMETERS:
 *  None.
 *
 * WHAT:
 *  Turn on-board LED off.
 *
 * RETURN VALUES:
 *  None.
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
void LED_off(void)
{
    digitalWrite(LED_PIN, LED_OFF);
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/

