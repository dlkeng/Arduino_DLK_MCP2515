/*
 * NAME: CAN_Gateway.ino
 *
 * WHAT:
 *  DLK MCP2515 Arduino Library CAN Controller CAN gateway program.
 *  Provides a bridge from one CAN bus to another CAN bus.
 *
 *  CAN1 <=====> Gateway <=====> CAN2
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
/*   #1           Nano               #2            Nano
   MCP2515 Pin  Arduino Pin        MCP2515 Pin  Arduino Pin
   ---------------------------     ---------------------------
        VCC         5V                  VCC         5V
        GND         GND                 GND         GND
        CS1         D10                 CS2         D9
        SI(MOSI)    D11                 SI(MOSI)    D11
        SO(MISO)    D12                 SO(MISO)    D12
        SCK         D13                 SCK         D13
        INT         D2 (INT1)           INT         D3 (INT2)

                                                      _________________________
                                                     |                         |
                                                    -|TX0[D1]               VIN|-
                                                    -|RX0[D0]               GND|-
         ________      ________                     -|RST                   RST|-
        |        |    |        |                    -|GND                   +5V|-
        |    ~INT|--- |        | ------------------->|PD2[D2]              [A7]|-
        |        |    |    ~INT|-------------------->|PD3[D3]              [A6]|-
        |        |    |        |     DEBUG_PIN <-----|PD4[D4]   [SCL/A5/D19]PC5|-
        |        |    |        |                    -|PD5[D5]   [SDA/A4/D18]PC4|-
        |        |    |        |                    -|PD6[D6]       [A3/D17]PC3|-
        |        |    |        |                    -|PD7[D7]       [A2/D16]PC2|-
        |        |    |        |        LED_HB <-----|PB0[D8]       [A1/D15]PC1|-
        |        |    |     ~CS|<--------------------|PB1[D9]       [A0/D14]PC0|-
        |     ~CS|<---|        | --------------------|PB2[D10]             AREF|-
        |      SI|<---|      SI|<--------------------|PB3[D11]             3.3V|-
        |      SO|----|      SO|-------------------->|PB4[D12]         [D13]PB5|------.
        |        |    |        |                     |         .-----.         |      |
        |        |    |        |                     |_________| USB |_________|      |
        |        |    |        |                               '-----'                |
        |        |    |        |                             Arduino Nano             |
        |        |    |        |                                                      |
        |     SCK|<---|     SCK|<-----------------------------------------------------'
        |________|    |________|       
         MCP2515 #1   MCP2515 #2       
 */

/*    #1          Teensy               #2          Teensy
   MCP2515 Pin  Arduino Pin         MCP2515 Pin  Arduino Pin
   ---------------------------      ---------------------------
        VCC         VIN                  VCC         VIN
        GND         GND                  GND         GND
        CS          D10                  CS          D7
        SI(MOSI)    D11                  SI(MOSI)    D11
        SO(MISO)    D12                  SO(MISO)    D12
        SCK         D13                  SCK         D13
        INT         D1                   INT         D2
                                                        ________      ________
                                                       |        |    |        |
        .--------------------------------------------->|SCK     |--->|SCK     |
        |      _____________________                   |  CAN1  |    |  CAN2  |
        |     |                     |                  |        |    |        |
        '-----|PC5[D13]     [D12]PC7|<-----------------|SO      |<---|SO      |
             -|PD1[D14]     [D11]PC6|----------------->|SI      |--->|SI      |
             -|PC0[D15]     [D10]PC4|----------------->|~CS     |    |        |
             -|PB0[D16]      [D9]PC3|-                 |        |    |        |
             -|PB1[D17]      [D8]PD3|-----> LED_HB     |        |    |        |
             -|PB3[D18]      [D7]PD2|----------------- |        |--->|~CS     |
             -|PB2[D19]      [D6]PD4|-                 |        |    |        |
             -|PD5[D20]      [D5]PD7|-                 |        |    |        |
             -|PD6[D21]     [D4]PA13|-                 |        |    |        |
             -|PC1[D22]     [D3]PA12|-                 |        |    |        |
             -|PC2[D23]      [D2]PD0|<---------------- |        | ---|~INT    |
             -|3.3V         [D1]PB17|<-----------------|~INT    |    |        |
             -|AGND         [D0]PB16|-                 |________|    |________|
             -|VIN               GND|-                 MCP2515 #1    MCP2515 #2
              |       .-----.       |
              |_______| USB |_______|
                      '-----'
                     Teensy 3.1
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
                        |     HSPI_SS - GPIO15|---[D15]--------------->|~CS     |-----------------.
                        |        INT2 - GPIO16|---[D16]<-------|>|-----|~INT    |<--.             |
                        |                     |                        |________|   |             |
                [D17]---|GPIO17               |                        MCP2515 #2   |             |
                [D26]---|GPIO26               |                                     |             |
  DEBUG_PIN <---[D22]---|GPIO22               |              ___                    |             |
                        |                     |             |   |                   |  ________   |
                        |                     |            --- 2K       ________    | |        |  |
                        |                     |             -   |      |        |   '-|~INT    |  |
                        |   VSPI_MISO - GPIO19|---[D19]<--------+-1K---|SO      |<----|SO      |  |
                        |    VSPI_SCK - GPIO18|---[D18]--------------->|SCK     |---->|SCK     |  |
                        |   VSPI_MOSI - GPIO23|---[D23]--------------->|SI      |---->|SI      |  |
                        |     VSPI_SS -  GPIO5|---[D5]---------------->|~CS     |  .->|~CS     |  |
                        |        INT1 - GPIO21|---[D21]<-------|>|-----|~INT    |  |  |        |  |
                        |                     |                        |________|  |  |________|  |
                        |                     |                        MCP2515 #1  |  MCP2515 #2  |
                        |                     |                                    '--------------'
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
                  GPIO26 |  O D26 |         | D22 O  | GPIO22 - DEBUG_PIN
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
                          | O D26             TX2 O | GPIO17
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

#include <DLK_MCP2515.h>    // MCP2515 CAN Bus library

// define following to use Rx interrupts for CAN1 Rx
#define USING_RX1_INTS

// define following to use Rx interrupts for CAN2 Rx
#define USING_RX2_INTS

// specify CS and interrupt pins to use
#ifdef __AVR__
#define LED_PIN             8               // the heartbeat LED pin (LED_BUILTIN is used for SPI SCK)
#define DEBUG_PIN           4
#define MCP2515_CS1_PIN     10
#define MCP2515_CS2_PIN     9
#define MCP2515_INT1_PIN    2               // INT0
#define MCP2515_INT2_PIN    3               // INT1
#endif

// specify CS and interrupt pins to use
#ifdef PHILHOWER_RP2040     // Pi Pico
#define LED_PIN             LED_BUILTIN     // the heartbeat LED pin
#define DEBUG_PIN           4
#define MCP2515_CS1_PIN     17
#define MCP2515_CS2_PIN     13
#define MCP2515_INT1_PIN    20              // INT
#define MCP2515_INT2_PIN    2               // INT
#endif

// specify CS and interrupt pins to use
#ifdef TEENSYDUINO          // Teensy 3.1
#define LED_PIN             8               // the heartbeat LED pin (onboard LED pin is used for SPI SCK)
#define DEBUG_PIN           4
#define MCP2515_CS1_PIN     10
#define MCP2515_CS2_PIN     7
#define MCP2515_INT1_PIN    1               // GPIO/INT
#define MCP2515_INT2_PIN    2               // GPIO/INT
#endif

// specify CS and interrupt pins to use
#ifdef ESP32                // ESP32
#define LED_PIN             LED_BUILTIN     // the heartbeat LED pin (on board)
#define DEBUG_PIN           22              // the Debug pin
#define MCP2515_CS1_PIN     5
#define MCP2515_CS2_PIN     15
#define MCP2515_INT1_PIN    21              // GPIO
#define MCP2515_INT2_PIN    16              // GPIO

#ifdef USING_RX1_INTS
#undef USING_RX1_INTS                       // CAN interrupts not supported for ESP32
#endif
#ifdef USING_RX2_INTS
#undef USING_RX2_INTS                       // CAN interrupts not supported for ESP32
#endif
#endif

#define SPI_CLOCK           2000000         // 2 Mbps
#define CAN_SPEED           CAN_250KBPS

#define TIMER_EXPIRED(start, interval)  ((millis() - start) >= interval)
#define HEARTBEAT_OFF_INTERVAL  950     // mS
#define HEARTBEAT_ON_INTERVAL   50      // mS

#define LED_ON      HIGH
#define LED_OFF     LOW

#define DEBUG_HI()      digitalWrite(DEBUG_PIN, HIGH);
#define DEBUG_LO()      digitalWrite(DEBUG_PIN, LOW);
#define DEBUG_TOGL()    digitalWrite(DEBUG_PIN, !digitalRead(DEBUG_PIN));

#ifdef USING_RX1_INTS
volatile bool Rx1IntFlag = false;
CAN_FRAME * RxCAN1_Frame;
#endif

#ifdef USING_RX2_INTS
volatile bool Rx2IntFlag = false;
CAN_FRAME * RxCAN2_Frame;
#endif

// CAN Interrupts and Chip Selects
#define CAN1_INT    MCP2515_INT1_PIN            // Set CAN1 INT to pin 2
DLK_MCP2515 CAN1(SPI_CLOCK, MCP2515_CS1_PIN);   // Set CAN1 CS to pin 10

#define CAN2_INT    MCP2515_INT2_PIN            // Set CAN2 INT to pin 3
#if defined(PHILHOWER_RP2040)
DLK_MCP2515 CAN2(SPI_CLOCK, MCP2515_CS2_PIN, false, SPI1_NUM);
#else
DLK_MCP2515 CAN2(SPI_CLOCK, MCP2515_CS2_PIN);   // Set CAN2 CS to pin 9
#endif

uint8_t Len = 0;       // length of received buffer
uint8_t Buf[8];        // Buffer to hold up to 8 bytes of data
uint32_t CanID;        // Can message ID

void setup()
{
    // init debug pin
    pinMode(DEBUG_PIN, OUTPUT);
    DEBUG_LO();

    // init heartbeat LED
    pinMode(LED_PIN, OUTPUT);
    LED_off();

    // init CS pin
    pinMode(MCP2515_CS1_PIN, OUTPUT);
    digitalWrite(MCP2515_CS1_PIN, HIGH);

    // init CS pin
    pinMode(MCP2515_CS2_PIN, OUTPUT);
    digitalWrite(MCP2515_CS2_PIN, HIGH);

    Serial.begin(115200);
#ifdef PHILHOWER_RP2040
    delay(2000);                 // allow time for Arduino's serial window to re-connect
    Serial.println("Raspberry Pi Pico MCU");
#endif
#ifdef TEENSYDUINO
    delay(2000);                 // allow time for Arduino's serial window to re-connect
    Serial.println("Teensy MCU");
#endif
#ifdef ESP32
    delay(2000);                 // allow time for Arduino's serial window to re-connect
    Serial.println();
    Serial.println("ESP32 MCU");
#endif
#ifdef __AVR__
    Serial.println("AVR MCU");
#endif

    Serial.println("CAN Bus Gateway");
#if !defined(USING_RX1_INTS) && !defined(USING_RX2_INTS)
    Serial.print(F("NOT "));
#endif
    Serial.println("Using Interrupts");

    delay(50);

    // Initialize CAN1 MCP2515 running at 8MHz with a baudrate of 250kb/s
    if (CAN1.MCP2515_Init(CAN_SPEED) == MCP2515_OK)
    {
        Serial.println("MCP2515 CAN1 Initialized Successfully!");
    }
    else
    {
        Serial.println("Error Initializing MCP2515 CAN1 ...");
//        while (1)
        {
            yield();    // needed by ESP8266 to prevent Soft WDT reset "rst cause:2, boot mode:(3,6)"
        }
    }

    CAN1.MCP2515_SetMode(MODE_NORMAL);  // Set operation mode to normal so the MCP2515 sends acks to received data.

#ifdef USING_RX1_INTS
    // set CAN1 Rx interrupt callback
    if (CAN1.MCP2515_OnRxInterrupt(CAN1_INT, RxInt1Handler) != MCP2515_OK)
    {
        Serial.println("Failed attaching MCP2515 CAN1_INT ...");
        while (1)
        {
            yield();    // needed by ESP8266 to prevent Soft WDT reset "rst cause:2, boot mode:(3,6)"
        }
    }
#endif

    // Initialize CAN2 MCP2515 running at 8MHz with a baudrate of 250kb/s
    if (CAN2.MCP2515_Init(CAN_SPEED) == MCP2515_OK)
    {
        Serial.println("MCP2515 CAN2 Initialized Successfully!");
    }
    else
    {
        Serial.println("Error Initializing MCP2515 CAN2 ...");
        while (1)
        {
            yield();    // needed by ESP8266 to prevent Soft WDT reset "rst cause:2, boot mode:(3,6)"
        }
    }

    CAN2.MCP2515_SetMode(MODE_NORMAL);  // Set operation mode to normal so the MCP2515 sends acks to received data.

#ifdef USING_RX2_INTS
    // set CAN2 Rx interrupt callback
    if (CAN2.MCP2515_OnRxInterrupt(CAN2_INT, RxInt2Handler) != MCP2515_OK)
    {
        Serial.println("Failed attaching MCP2515 CAN2_INT ...");
        while (1)
        {
            yield();    // needed by ESP8266 to prevent Soft WDT reset "rst cause:2, boot mode:(3,6)"
        }
    }
#endif
}

#ifdef USING_RX1_INTS
// Interrupt callback handler for Rx interrupt for CAN1
void RxInt1Handler(CAN_FRAME * frame)
{
    RxCAN1_Frame = frame;    // save reference to CAN1 Rx data

    Rx1IntFlag = true;       // set notification of received CAN1 data
}
#endif

#ifdef USING_RX2_INTS
// Interrupt callback handler for Rx interrupt for CAN2
void RxInt2Handler(CAN_FRAME * frame)
{
    RxCAN2_Frame = frame;    // save reference to CAN2 Rx data

    Rx2IntFlag = true;       // set notification of received CAN2 data
}
#endif

void loop()
{
#if !defined(USING_RX1_INTS) || !defined(USING_RX2_INTS)
    CAN_FRAME frame;
#endif

    bool got_rx = false;
    uint8_t sndStat;

#ifdef USING_RX1_INTS
    if (Rx1IntFlag)      // got CAN Rx interrupt
    {
        Rx1IntFlag = false;

        // Read data, Len: data length, Buf: data buffer
        CanID = RxCAN1_Frame->can_id;
        Len = RxCAN1_Frame->can_dlc;
        memcpy(Buf, RxCAN1_Frame->can_data, Len);
        got_rx = true;
    }
#else
//if (!digitalRead(MCP2515_INT1_PIN))                 // If CAN0_INT pin is low, read receive buffer
{
    // use polling of MCP2515 to determine if CAN data available
    // (could instead poll the MCP2515 interrupt pin to determine if CAN data available
    //  for less SPI bus traffic)
    if (CAN1.MCP2515_Recv(&frame) == MCP2515_OK) // check if data is coming in
    {
        // Read data, Len1: data length, Buf1: data buffer
        CanID = frame.can_id;
        Len = frame.can_dlc;
        memcpy(Buf, frame.can_data, Len);
        got_rx = true;
    }
}
#endif
    if (got_rx)
    {
        Serial.print("Rx CAN ID1: 0x");
        Serial.print(CanID, HEX);       // print the CAN ID in HEX

        Serial.print("    Data Length: "); // Print the length of the received data
        Serial.print(Len);
        Serial.print("    ");

        for (int i = 0; i < Len; i++)    // loop on the incoming data to print each byte
        {
//            Serial.print(Buf[i]);     
            PrintHexByte(Buf[i]);     
            if (i < Len - 1)
            {
                Serial.print(",");      // Separate the numbers for readability
            }
        }
        Serial.println();

        // CAN.MCP2515_Send(msg ID, #of data bytes, data array);
        sndStat = CAN2.MCP2515_Send(CanID, Len, Buf);
        if (sndStat == MCP2515_OK)
        {
            Serial.println("Message Forwarded Successfully to CAN2!");
        }
        else
        {
            Serial.println("Error Forwarding Message to CAN2...");
        }
delay(100);
        got_rx = false;
    }

#ifdef USING_RX2_INTS
    if (Rx2IntFlag)      // got CAN Rx interrupt
    {
        Rx2IntFlag = false;

        // Read data, Len: data length, Buf: data buffer
        CanID = RxCAN2_Frame->can_id;
        Len = RxCAN2_Frame->can_dlc;
        memcpy(Buf, RxCAN2_Frame->can_data, Len);
        got_rx = true;
    }
#else
//if (!digitalRead(MCP2515_INT2_PIN))                 // If CAN1_INT pin is low, read receive buffer
{
    // use polling of MCP2515 to determine if CAN data available
    // (could instead poll the MCP2515 interrupt pin to determine if CAN data available
    //  for less SPI bus traffic)
    if (CAN2.MCP2515_Recv(&frame) == MCP2515_OK) // check if data is coming in
    {
        // Read data, Len: data length, Buf: data buffer
        CanID = frame.can_id;
        Len = frame.can_dlc;
        memcpy(Buf, frame.can_data, Len);
        got_rx = true;
    }
}
#endif
    if (got_rx)
    {
        Serial.print("Rx CAN ID2: 0x");
        Serial.print(CanID, HEX);       // print the CAN ID in HEX

        Serial.print("    Data Length: "); // Print the length of the received data
        Serial.print(Len);
        Serial.print("    ");

        for (int i = 0; i < Len; i++)    // loop on the incoming data to print each byte
        {
//            Serial.print(Buf[i]);     
            PrintHexByte(Buf[i]);     
            if (i < Len - 1)
            {
                Serial.print(",");      // Separate the numbers for readability
            }
        }
        Serial.println();

        // CAN.MCP2515_Send(msg ID, #of data bytes, data array);
        sndStat = CAN1.MCP2515_Send(CanID, Len, Buf);
        if (sndStat == MCP2515_OK)
        {
            Serial.println("Message Forwarded Successfully to CAN1!");
        }
        else
        {
            Serial.println("Error Forwarding Message to CAN1...");
        }
delay(100);
        got_rx = false;
    }

    DoHeartbeat();
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

/*
 * NAME:
 *  void PrintHexByte(uint8_t the_byte)
 *
 * PARAMETERS:
 *  uint8_t the_byte = the byte to print
 *
 * WHAT:
 *  Helper function that prints formatted hex byte value to console.
 *
 * RETURN VALUES:
 *  None.
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
void PrintHexByte(uint8_t the_byte)
{
    if (the_byte < 0x10)
    {
        Serial.print('0');          // leading '0'
    }
    Serial.print(the_byte, HEX);
}

/*
 * NAME:
 *  void Print0xHexByte(uint8_t the_byte)
 *
 * PARAMETERS:
 *  uint8_t the_byte = the byte to print
 *
 * WHAT:
 *  Helper function that prints formatted hex byte value with leading "0x" to console.
 *
 * RETURN VALUES:
 *  None.
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
void Print0xHexByte(uint8_t the_byte)
{
    Serial.print(F("0x"));          // leading "0x"
    PrintHexByte(the_byte);
}

