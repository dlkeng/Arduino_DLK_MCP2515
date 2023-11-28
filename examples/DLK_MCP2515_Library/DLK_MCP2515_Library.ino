/*
 * NAME: DLK_MCP2515_Library.ino
 *
 * WHAT:
 *  DLK MCP2515 Arduino Library CAN Controller test program.
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 *
 * AUTHOR:
 *  D.L. Karmann
 *
 * MODIFIED:
 *
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
             ________                    -|RST                   RST|-
            |        |                   -|GND                   +5V|-
            |    ~INT|------------------->|PD2[D2]              [A7]|-
            |        |                   -|PD3[D3]              [A6]|-
            |        |                   -|PD4[D4]   [SCL/A5/D19]PC5|-
            |        |                   -|PD5[D5]   [SDA/A4/D18]PC4|-
            |        |                   -|PD6[D6]       [A3/D17]PC3|-----> CAN_DATA_PIN
            |        |                   -|PD7[D7]       [A2/D16]PC2|-
            |        |       LED_HB <-----|PB0[D8]       [A1/D15]PC1|-
            |        |                   -|PB1[D9]       [A0/D14]PC0|------> DEBUG_PIN
            |     ~CS|<-------------------|PB2[D10]             AREF|-
            |      SI|<-------------------|PB3[D11]             3.3V|-
            |      SO|------------------->|PB4[D12]         [D13]PB5|------.
            |        |                    |         .-----.         |      |
            |        |                    |_________| USB |_________|      |
            |        |                              '-----'                |
            |        |                            Arduino Nano             |
            |        |                                                     |
            |     SCK|<----------------------------------------------------'
            |________|               
             MCP2515         
 */
 
/*                Pi Pico
   MCP2515 Pin    Arduino Pin
   ------------------------------
        VCC         VSYS
        GND         GND
        SO(MISO)    D16
        CS          D17
        SCK         D18
        SI(MOSI)    D19
        INT         D20
                                 ___
                                |   |
              ________          2K ---     _________________________
             |        |         |   -     |                         |
             |      SO|------1K-+-------->|GP16[D16]       [D15]GP15|-
             |     ~CS|<------------------|GP17[D17]       [D14]GP14|-
             |        |                  -|GND                   GND|-
             |     SCK|<------------------|GP18[D18]       [D13]GP13|-
             |      SI|<------------------|GP19[D19]       [D12]GP12|-
             |    ~INT|------1K-+-------->|GP20[D20]       [D11]GP11|------> CAN_DATA_PIN
             |        |         |        -|GP21[D21]       [D10]GP10|-
             |        |        2K        -|GND                   GND|-
             |        |         |        -|GP22[D22]         [D9]GP9|-
             |        |        ---       -|RUN               [D8]GP8|-
             |        |         -        -|GP26[D26]         [D7]GP7|-
             |        |                  -|GP27[D27]         [D6]GP6|-
             |        |                  -|GND                   GND|-
             |        |                  -|GP28[D28]         [D5]GP5|-
             |        |                  -|VREF              [D4]GP4|-
             |        |                  -|3V3               [D3]GP3|-
             |        |                  -|3V3_EN            [D2]GP2|-
             |        |                  -|GND                   GND|-
             |     +5V|<------------------|VSYS              [D1]GP1|-
             |     GND|---.              -|VBUS              [D0]GP0|------> DEBUG_PIN
             |        |   |               |         .-----.         | 
             |________|  ---              |_________| USB |_________| LED_HB (onboard)
              MCP2515     -                         '-----'           
                                              Raspberry Pi Pico        
 */

/*                Teensy
   MCP2515 Pin  Arduino Pin
   ------------------------------
        VCC         VIN
        GND         GND
        CS          D10
        SI(MOSI)    D11
        SO(MISO)    D12
        SCK         D13
        INT         D1
                                                                ________
                                                               |        |
                .--------------------------------------------->|SCK     |
                |      _____________________                   |   CAN  |
                |     |                     |                  |        |
                '-----|PC5[D13]     [D12]PC7|<-----------------|SO      |
     DEBUG_PIN <------|PD1[D14]     [D11]PC6|----------------->|SI      |
                     -|PC0[D15]     [D10]PC4|----------------->|~CS     |
                     -|PB0[D16]      [D9]PC3|-                 |        |
  CAN_DATA_PIN <------|PB1[D17]      [D8]PD3|-----> LED_HB     |        |
                     -|PB3[D18]      [D7]PD2|-                 |        |
                     -|PB2[D19]      [D6]PD4|-                 |        |
                     -|PD5[D20]      [D5]PD7|-                 |        |
                     -|PD6[D21]     [D4]PA13|-                 |        |
                     -|PC1[D22]     [D3]PA12|-                 |        |
                     -|PC2[D23]      [D2]PD0|-                 |        |
                     -|3.3V         [D1]PB17|<-----------------|~INT    |
                     -|AGND         [D0]PB16|-                 |________|
                     -|VIN               GND|-                  MCP2515
                      |       .-----.       |
                      |_______| USB |_______|
                              '-----'
                             Teensy 3.1
*/

/*               ESP8266 WeMos D1 Mini
   MCP2515 Pin   Arduino Pin
   ------------------------------
        VCC         +5V
        GND         GND
        CS          D10
        SI(MOSI)    D7
        SO(MISO)    D6
        SCK         D5
        INT         D8
                                  WeMos D1 Mini
                   ___          ESP8266-based WiFi
                   \|/        _____________________
                    |        |                     |
                    |        |                     |
                    '<======>| WiFi                |
                             |                     |          ___
                    /[TX0]<--|TxD/GPIO1            |         |   |
         USB<=====>|         |        UART0        |        --- 2K       ________
         COM        \[RX0]-->|RxD/GPIO3            |         -   |      |        |
                             |          MISO/GPIO12|--[D6]<------+-1K---|SO      |
                             |           SCK/GPIO14|--[D5]------------->|SCK     |
                             |          MOSI/GPIO13|--[D7]------------->|SI      |
                             |                GPIO5|--[D1]------------->|~CS     |
                             |                GPIO4|--[D2]<------+-1K---|~INT    |
    CAN_DATA_PIN <----[D0]---|GPIO16 *             |             |      |________|
       DEBUG_PIN <----[D3]---|GPIO0 *              |            2K       MCP2515
          LED_HB <----[D4]---|UART1_TxD/GPIO2 *    |             | 
                      [A0]-->|ADC          * GPIO15|--[D8]      ---
                             |                     |   |         -
                             |_____________________|   `---WeMos D pins

 * GPIO0 - 10K or 12K pull-up    - Must be Hi at reset, else bootloader mode
 * GPIO2 - 10K or 12K pull-up    - Must be Hi at reset, bootloader SDIO mode
 * GPIO15 - 10K or 12K pull-down - Must be Lo at reset, else SDIO mode
 * GPIO16 - special case - does not have internal pull-up, so do NOT use INPUT_PULLUP (can use use INPUT_PULLDOWN)
   RESET - 10K or 12K pull-up
   other GPIO's - no pull-down or pull-up
                      ________________________
                     /      ___   _   _       \
                     |     | | |_| |_| |      |
                     |     | |                |
                     | O RST .---------. TX O | GPIO1
                     | O A0  |         | RX O | GPIO3
        CAN_DATA_PIN | O D0  |         | D1 O | ~CS
                 SCK | O D5  | ESP-12  | D2 O | ~INT
                MISO | O D6  |         | D3 O | DEBUG_PIN
                MOSI | O D7  |         | D4 O | LED_HB
                     | O D8  |_________|  G O | GND
                     | O 3V3             5V O | +5V
                     |        D1 mini         |
                     \                        |
                      |       .-----.         |
                     =|RST    | USB |         |
                      |_______|-----|_________|
                            WeMos D1 Mini
*/

/*               ESP32
   MCP2515 Pin   Arduino Pin
   ------------------------------
        VCC         +5V
        GND         GND
        CS          D10
        SI(MOSI)    D7
        SO(MISO)    D6
        SCK         D5
        INT         D8
                                    ESP32
                  ___        ESP32-based WiFi/ BLE
                  \|/        _____________________
                   |        |                     |
                   |<======>| WiFi                |
                   '<======>| Bluetooth           |
                            |                     |
                   /[TXD]<--|TxD/GPIO1            |                   
        USB<=====>|         |      UART0          |              ___
        COM        \[RXD]-->|RxD/GPIO3            |             |   |
                            |                     |            --- 2K       ________
                            |                     |             -   |      |        |
                            |   HSPI_MISO - GPIO12|---[D12]<--------+-1K---|SO      |
                            |    HSPI_SCK - GPIO14|---[D14]--------------->|SCK     |
                            |   HSPI_MOSI - GPIO13|---[D13]--------------->|SI      |
                            |     HSPI_SS - GPIO15|---[D15]--------------->|~CS     |
                            |                     |                        |________|
                    [D17]---|GPIO17               |                        MCP2515 #2
   CAN_DATA_PIN <---[D26]---|GPIO26               |                                  
      DEBUG_PIN <---[D22]---|GPIO22               |              ___                 
                            |                     |             |   |                
                            |                     |            --- 2K       ________ 
                            |                     |             -   |      |        |
                            |   VSPI_MISO - GPIO19|---[D19]<--------+-1K---|SO      |
                            |    VSPI_SCK - GPIO18|---[D18]--------------->|SCK     |
                            |   VSPI_MOSI - GPIO23|---[D23]--------------->|SI      |
                            |     VSPI_SS -  GPIO5|---[D5]---------------->|~CS     |
                            |        INT1 - GPIO21|---[D21]<-------|>|-----|~INT    |
                            |                     |                        |________|
                            |                     |                        MCP2515 #1
                            |                     |                                  
                            |               GPIO33|---[D33]
                            |                     |    |
                            |_____________________|    `---D1 D pins
                            LED_HB (onboard - GPIO2)

    GPIO0 - ~45K pull-up (10K)    - Must be Hi at reset, else bootloader mode
    GPIO2 -                       - Connected to on-board LED
    GPIO5 - (10K pull-up)         - Must be Hi at reset
    GPIO15 -                      - Must be Hi at reset
    RESET - 10K pull-up
    other GPIO's - no pull-down or pull-up
                          ___________________________
                         /        _   _   _ _        \
                         |       | |_| |_| | |       |
                         |       |         | |       |
                         |  O RST .---------. TXD O  | GPIO1
                  GPIO36 |  O SVP |         | RXD O  | GPIO3
   CAN_DATA_PIN - GPIO26 |  O D26 |         | D22 O  | GPIO22 - DEBUG_PIN
       VSPI_SCK - GPIO18 |  O D18 |  ESP32  | D21 O  | GPIO21 - ~INT1
      VSPI_MISO - GPIO19 |  O D19 | WROOM32 | D17 O  | GPIO17
      VSPI_MOSI - GPIO23 |  O D23 |         | D16 O  | GPIO16 - ~INT2
 ~CS1 - VSPI_SS -  GPIO5 |  O D5  |_________| GND O  |
                         |  O 3V3             VCC O  |
      HSPI_MOSI - GPIO13 |  O TCK             TDO O  | GPIO15 - HSPI_SS - ~CS2
                         |  O SD3             SDO O  |
                         |          D1 mini          |
                         \       (Inner Pins)        |
                          |         .-----.          |
                         =|RST      | USB |          |
                          |_________|=====|__________|
                              WeMos Mini D1 ESP32
                          ___________________________
                         /        _   _   _ _        \
                         |       | |_| |_| | |       |
                         |       |         | |       |
                         | O GND  .---------.  GND O |
                         | O      |         |  D27 O | GPIO27
                  GPIO39 | O SVN  |         |  D25 O | GPIO25
                  GPIO35 | O D35  |  ESP32  |  D32 O | GPIO32
                  GPIO33 | O D33  | WROOM32 |  TDI O | GPIO12 - HSPI_MISO
                  GPIO34 | O D34  |         |   D4 O | GPIO4
       HSPI_SCK - GPIO14 | O TMS  |_________|   D0 O | GPIO0
                         | O                    D2 O | GPIO2 - LED_HB (onboard)
                         | O SD2               SD1 O |
                         | O CMD               CLK O |
                         |          D1 mini          |
                         \       (Outer Pins)        |
                          |         .-----.          |
                         =|RST      | USB |          |
                          |_________|=====|__________| LED_HB (onboard - GPIO2)
                              WeMos Mini D1 ESP32

                           _________________________
                          |       _   _   _ _       |
                          |      | |_| |_| | |      |
                          |      |         | |      |
                          | O EN              D23 O | VSPI_MOSI
                   GPIO36 | O VPD .---------. D22 O | DEBUG_PIN
                   GPIO39 | O VN  |         | TX0 O | GPIO1
                          | O D34 |         | RX0 O | GPIO3
                          | O D35 |  ESP32  | D21 O | GPIO21 - ~INT1
                          | O D32 | WROOM32 | D19 O | VSPI_MISO
                          | O D33 |         | D18 O | VSPI_SCK
                          | O D25 |_________|  D5 O | VSPI_SS - ~CS1
             CAN_DATA_PIN | O D26             TX2 O | GPIO17
                          | O D27             RX2 O | GPIO16 - ~INT2
                 HSPI_SCK | O D14              D4 O |
                HSPI_MISO | O D12              D2 O | GPIO2 - LED_HB (onboard)
                HSPI_MOSI | O D13             D15 O | HSPI_SS - ~CS2
                          | O GND             GND O |
                          | O VIN             3V3 O |
                          |         .-----.         |
                          | RST/EN  | USB |  BOOT   |
                          |_________|=====|_________|
                                ESP32 DEVKIT V1
*/

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

#include <EEPROM.h>
#include <CommandLine.h>
#include <DLK_MCP2515.h>

// define following to use SPI HW chip-select instead of SW chip-select
#define USE_HW_SPI_CS

// define following to use Rx interrupts instead of polling
#define USING_RX_INTS

#define TITLE_MSG           "DLK MCP2515 Library CAN Controller Testing"

#define LED_ON      HIGH
#define LED_OFF     LOW

#define TIMER_EXPIRED(start, interval)  ((millis() - start) >= interval)
#define HEARTBEAT_OFF_INTERVAL  950     // mS
#define HEARTBEAT_ON_INTERVAL   50      // mS

// specify CS and interrupt pins to use
#ifdef __AVR__
#define LED_PIN             8               // the heartbeat LED pin (LED_BUILTIN is used for SPI SCK)
#define MCP2515_CS_PIN      10
#define MCP2515_INT_PIN     2               // INT0
#define CAN_DATA_PIN        A3              // the CAN data indicator pin
#define DEBUG_PIN           A0              // the Debug pin
#endif

// specify CS and interrupt pins to use
#ifdef PHILHOWER_RP2040     // Pi Pico
#define LED_PIN             LED_BUILTIN     // the heartbeat LED pin
#define MCP2515_CS_PIN      17
#define MCP2515_INT_PIN     20              // INT
#define CAN_DATA_PIN        11              // the CAN data indicator pin
#define DEBUG_PIN           0               // the Debug pin
#endif

// specify CS and interrupt pins to use
#ifdef TEENSYDUINO
#define LED_PIN             8               // the heartbeat LED pin (onboard LED pin is used for SPI SCK)
#define MCP2515_CS_PIN      10
#define MCP2515_INT_PIN     1               // GPIO/INT
#define CAN_DATA_PIN        17              // the CAN data indicator pin
#define DEBUG_PIN           14              // the Debug pin
#endif

// specify CS and interrupt pins to use (if using "D1 mini", can use WeMos Dn number)
#ifdef ESP8266              // ESP8266
#define LED_PIN             LED_BUILTIN     // WeMos D4 - the heartbeat LED pin (on board)
#define MCP2515_CS_PIN      5               // WeMos D1
#define MCP2515_INT_PIN     4               // WeMos D2 - GPIO/INT
#define CAN_DATA_PIN        16              // the CAN data indicator pin
#define DEBUG_PIN           0               // the Debug pin

#undef LED_OFF
#undef LED_ON
#define LED_OFF             HIGH            // ESP8266 built-in LED active-low!
#define LED_ON              LOW             // ESP8266 built-in LED active-low!

#ifdef USING_RX_INTS
#undef USING_RX_INTS                        // CAN interrupts not supported for ESP8266
#endif
#endif

// specify CS and interrupt pins to use
#ifdef ESP32                // ESP32
#define LED_PIN             LED_BUILTIN     // the heartbeat LED pin (on board)
#define MCP2515_CS_PIN      5
#define MCP2515_INT_PIN     21              // GPIO
#define CAN_DATA_PIN        26              // the CAN data indicator pin
#define DEBUG_PIN           22              // the Debug pin

#ifdef USING_RX_INTS
#undef USING_RX_INTS                            // CAN interrupts not supported for ESP32
#endif
#endif

#define CAN_DATA_HI()       digitalWrite(CAN_DATA_PIN, HIGH)
#define CAN_DATA_LO()       digitalWrite(CAN_DATA_PIN, LOW)
#define CAN_DATA_TOGL()     digitalWrite(CAN_DATA_PIN, !digitalRead(CAN_DATA_PIN))

#define DEBUG_HI()          digitalWrite(DEBUG_PIN, HIGH)
#define DEBUG_LO()          digitalWrite(DEBUG_PIN, LOW)
#define DEBUG_TOGL()        digitalWrite(DEBUG_PIN, !digitalRead(DEBUG_PIN))

#define SPI_CLOCK           2000000         // 2 Mbps
#define CAN_MESSAGE_ID      0x008
#define CAN_XMESSAGE_ID     0xabcdef
#define CAN_SPEED           CAN_250KBPS     // default

#define EEPROM_SIZE         256         // not needed for AVR

// EEPROM addresses
#define EE_CAN_MSGS_FORMAT  0x08        // 1 byte
#define EE_CAN_DUPE_MSGS    0x09        // 1 byte
#define EE_CAN_SPEED        0x10        // 1 byte
#define EE_CAN_MODE         0x11        // 1 byte
#define EE_CAN_MASKS        0x12        // 8 bytes
#define EE_CAN_FILTERS      0x20        // 24 bytes

// CAN messages shown format
#define CMSGS_MIN           0
#define CMSGS_CSV           1
#define CMSGS_NORM          2

// CAN duplicate messages shown
#define CDUPE_NO            0
#define CDUPE_YES           1

uint16_t CAN_MsgCnt = 0;
uint8_t CAN_MsgFormat = CMSGS_NORM;
uint8_t CAN_MsgDupes = CDUPE_YES;
uint8_t CAN_Speed = CAN_250KBPS;
uint8_t CAN_Mode = MODE_NORMAL;
uint32_t CAN_Masks[MCP2515_N_MASKS];
uint32_t CAN_Filters[MCP2515_N_FILTERS];

CommandLine CmdLine(Serial);    // setup CommandLine to use standard Arduino Serial and echo on

#if defined(PHILHOWER_RP2040) && defined(USE_HW_SPI_CS)
DLK_MCP2515 Mcp2515(SPI_CLOCK, MCP2515_CS_PIN, true);
#else
DLK_MCP2515 Mcp2515(SPI_CLOCK, MCP2515_CS_PIN);
#endif

#ifdef USING_RX_INTS
#define CAN_FRAME_CNT   4

volatile bool RxIntFlag = false;
void RxIntHandler(CAN_FRAME * frame);
CAN_FRAME * RxCAN_Frame[CAN_FRAME_CNT];
uint8_t LastRxNdx = 0;
uint8_t NextRxNdx = 0;
#endif

CAN_FRAME LastCAN_Frame;

void ShowCAN_Msg(CAN_FRAME * frame);

void setup()
{
    // init heartbeat LED
    pinMode(LED_PIN, OUTPUT);
    LED_off();

    // init CAN data indicator pin
    pinMode(CAN_DATA_PIN, OUTPUT);
    CAN_DATA_LO();

    // init Debug pin
    pinMode(DEBUG_PIN, OUTPUT);
    DEBUG_LO();

    // init MCP2515 Int input pin
    pinMode(MCP2515_INT_PIN, INPUT_PULLUP);

    // setup serial port
    Serial.begin(115200);

    // generate the sign-on banner
//    Serial.println(F(CLS_HOME));
    Serial.println();
#ifdef PHILHOWER_RP2040
    delay(2000);                 // allow time for Arduino's serial window to re-connect
    Serial.println("Raspberry Pi Pico MCU");
#endif
#ifdef TEENSYDUINO
    delay(2000);                 // allow time for Arduino's serial window to re-connect
    Serial.println("Teensy MCU");
#endif
#ifdef ESP8266
    delay(2000);                 // allow time for Arduino's serial window to re-connect
    Serial.println();
    Serial.println("ESP8266 MCU");
#endif
#ifdef ESP32
    delay(2000);                 // allow time for Arduino's serial window to re-connect
    Serial.println();
    Serial.println("ESP32 MCU");
#endif
#ifdef __AVR__
    Serial.println("AVR MCU");
#endif

    Serial.println(F("DLK_MCP2515_Library.ino"));

    Serial.println(F(TITLE_MSG));

#ifndef USING_RX_INTS
    Serial.print(F("NOT "));
#endif
    Serial.println(F("Using Interrupts"));

    Serial.print("Using ");
#if defined(PHILHOWER_RP2040) && defined(USE_HW_SPI_CS)
    Serial.print("HW");
#else
    Serial.print("SW");
#endif
    Serial.println(" SPI CS");

    delay(50);

    // get EEPROM configuration items
    GetConfig();

    // Initialize MCP2515
    if (DoCAN_Init(CAN_Speed) != MCP2515_OK)
    {
        Serial.println(F("MCP2515_Init() failed!"));
        while (1)
        {
            yield();    // needed by ESP8266 to prevent Soft WDT reset "rst cause:2, boot mode:(3,6)"
        }
    }

#ifdef USING_RX_INTS
    // set Rx interrupt callback
    if (Mcp2515.MCP2515_OnRxInterrupt(MCP2515_INT_PIN, RxIntHandler) != MCP2515_OK)
    {
        Serial.println("Failed attaching MCP2515 MCP2515_INT_PIN ...");
        while (1)
        {
            yield();    // needed by ESP8266 to prevent Soft WDT reset "rst cause:2, boot mode:(3,6)"
        }
    }
#endif
}

#ifdef USING_RX_INTS
// Interrupt callback handler for Rx interrupt
void RxIntHandler(CAN_FRAME * frame)
{
    RxCAN_Frame[NextRxNdx] = frame;     // save reference to CAN Rx data
    if (++NextRxNdx == CAN_FRAME_CNT)
    {
        NextRxNdx = 0;
    }

    RxIntFlag = true;       // set notification of received CAN data
}
#endif

void loop()
{
    static bool new_prompt = true;
#ifndef USING_RX_INTS
    CAN_FRAME frame;
#endif

    if (new_prompt)
    {
        new_prompt = false;
        //
        // Print a prompt to the console.
        //
        Serial.println();
        Serial.print(F("-> "));
    }

    new_prompt = CmdLine.DoCmdLine();

#ifdef USING_RX_INTS
    if (RxIntFlag)      // got CAN Rx interrupt
    {
        RxIntFlag = false;
        while (RxCAN_Frame[LastRxNdx] != NULL)
        {
            ++CAN_MsgCnt;
            CAN_DATA_TOGL();
            if ((CAN_MsgDupes == CDUPE_YES) || 
                (!CheckForDupes(RxCAN_Frame[LastRxNdx], &LastCAN_Frame)))
            {
                ShowCAN_Msg(RxCAN_Frame[LastRxNdx]);
            }

            LastCAN_Frame = *RxCAN_Frame[LastRxNdx];    // remember last CAN message - deep copy
            RxCAN_Frame[LastRxNdx] = NULL;
            if (++LastRxNdx == CAN_FRAME_CNT)
            {
                LastRxNdx = 0;
            }
        }
    }
#else
    if (digitalRead(MCP2515_INT_PIN) == LOW)
    {
        if (Mcp2515.MCP2515_Recv(&frame) == MCP2515_OK)
        {
            ++CAN_MsgCnt;
            CAN_DATA_TOGL();
            ShowCAN_Msg(&frame);
        }
    }
#endif


    // do other stuff here

    // do Heartbeat
    DoHeartbeat();
}

// read configurations items from EEPROM
void GetConfig(void)
{
    uint8_t ee_uint8;
    uint32_t ee_uint32;

#if defined(ARDUINO_ARCH_RP2040) || defined(ESP8266) || defined(ESP32)
    EEPROM.begin(EEPROM_SIZE);
#endif

    // get saved CAN messages shown format
    ee_uint8 = EEPROM.read(EE_CAN_MSGS_FORMAT);
    if (ee_uint8 <= CMSGS_NORM)
    {
        CAN_MsgFormat = ee_uint8;
    }

    // get saved CAN messages shown format
    ee_uint8 = EEPROM.read(EE_CAN_DUPE_MSGS);
    if (ee_uint8 <= CDUPE_YES)
    {
        CAN_MsgDupes = ee_uint8;
    }

    // get saved CAN speed
    ee_uint8 = EEPROM.read(EE_CAN_SPEED);
    if ((ee_uint8 >= CAN_5KBPS) && (ee_uint8 <= CAN_1000KBPS))
    {
        CAN_Speed = ee_uint8;
    }

    // get saved CAN mode
    ee_uint8 = EEPROM.read(EE_CAN_MODE);
    switch (ee_uint8)
    {
        case MODE_NORMAL:
        case MODE_SLEEP:
        case MODE_LOOPBACK:
        case MODE_LISTENONLY:
        case MODE_CONFIG:
            CAN_Mode = ee_uint8;
            break;
    }

    // get saved CAN masks
    for (uint8_t i = 0; i < MCP2515_N_MASKS; ++i)
    {
        EEPROM.get(EE_CAN_MASKS + (i * sizeof(uint32_t)), ee_uint32);
        if (ee_uint32 == 0xffffffff)
        {
            CAN_Masks[i] = 0;
        }
        else
        {
            CAN_Masks[i] = ee_uint32;
        }
    }

    // get saved CAN filters
    for (uint8_t i = 0; i < MCP2515_N_FILTERS; ++i)
    {
        EEPROM.get(EE_CAN_FILTERS + (i * sizeof(uint32_t)), ee_uint32);
        if (ee_uint32 == 0xffffffff)
        {
            CAN_Filters[i] = 0;
        }
        else
        {
            CAN_Filters[i] = ee_uint32;
        }
    }
}

// check for duplicate CAN messages
bool CheckForDupes(CAN_FRAME * cf1, CAN_FRAME *cf2)
{
    if (cf1->can_id != cf2->can_id)
    {
        return false;   // not duplicate frame
    }
    if (cf1->can_dlc != cf2->can_dlc)
    {
        return false;   // not duplicate frame
    }
    if (cf1->can_rxb != cf2->can_rxb)
    {
        return false;   // not duplicate frame
    }
    for (uint8_t i = 0; i < cf1->can_dlc; ++i)
    {
        if (cf1->can_data[i] != cf2->can_data[i])
        {
            return false;   // not duplicate frame
        }
    }
    return true;   // duplicate frame
}

// do Heartbeat
void DoHeartbeat(void)
{
    static uint32_t last_HB_tick = 0;
    static uint8_t last_HB_state = false;

    if (last_HB_state)
    {
        if (TIMER_EXPIRED(last_HB_tick, HEARTBEAT_ON_INTERVAL))
        {
            LED_off();      // off
            last_HB_state = false;
            last_HB_tick = millis();
        }
    }
    else
    {
        if (TIMER_EXPIRED(last_HB_tick, HEARTBEAT_OFF_INTERVAL))
        {
            LED_on();       // on
            last_HB_state = true;
            last_HB_tick = millis();
        }
    }
}

void LED_on(void)
{
    digitalWrite(LED_PIN, LED_ON);
}

void LED_off(void)
{
    digitalWrite(LED_PIN, LED_OFF);
}

