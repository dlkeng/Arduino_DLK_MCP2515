// From: https://protosupplies.com/product/mcp2515-can-bus-interface-module/
/*
  Exercise the MCP2515 CAN Bus Module (Sending Module Code)
  Requires 2 Arduino compatible boards and 2 CAN bus modules to create a link
  Module connects to power/ground and SPI bus.  Pin 10 used for CS.
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
           ________                      -|RST                   RST|-
          |        |                     -|GND                   +5V|-
          |    ~INT|--------------------->|PD2[D2]              [A7]|-
          |        |  TX_INTERVAL_PIN --->|PD3[D3]              [A6]|-
          |        |                     -|PD3[D3]              [A6]|-
          |        |                     -|PD4[D4]   [SCL/A5/D19]PC5|-
          |        |                     -|PD5[D5]   [SDA/A4/D18]PC4|-
          |        |                     -|PD6[D6]       [A3/D17]PC3|-
          |        |                     -|PD7[D7]       [A2/D16]PC2|-
          |        |         LED_HB <-----|PB0[D8]       [A1/D15]PC1|-
          |        |                     -|PB1[D9]       [A0/D14]PC0|-
          |     ~CS|<---------------------|PB2[D10]             AREF|-
          |      SI|<---------------------|PB3[D11]             3.3V|-
          |      SO|--------------------->|PB4[D12]         [D13]PB5|------.
          |        |                      |         .-----.         |      |
          |        |                      |_________| USB |_________|      |
          |        |                                '-----'                |
          |        |                              Arduino Nano             |
          |        |                                                       |
          |     SCK|<------------------------------------------------------'
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
             |    ~INT|------1K-+-------->|GP20[D20]       [D11]GP11|-
             |        |         |        -|GP21[D21]       [D10]GP10|-
             |        |        2K        -|GND                   GND|-
             |        |         |        -|GP22[D22]         [D9]GP9|-
             |        |        ---       -|RUN               [D8]GP8|-
             |        |         -        -|GP26[D26]         [D7]GP7|-
             |        |                  -|GP27[D27]         [D6]GP6|-
             |        |                  -|GND                   GND|-
             |        |                  -|GP28[D28]         [D5]GP5|-
             |        |                  -|VREF              [D4]GP4|-
             |        |                  -|3V3               [D3]GP3|<------ TX_INTERVAL_PIN
             |        |                  -|3V3_EN            [D2]GP2|-
             |        |                  -|GND                   GND|-
             |     +5V|<------------------|VSYS              [D1]GP1|-
             |     GND|---.              -|VBUS              [D0]GP0|-
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
        INT         D8
                                                                ________
                                                               |        |
        .----------------------------------------------------->|SCK     |
        |      _____________________                           |   CAN  |
        |     |                     |                          |        |
        '-----|PC5[D13]     [D12]PC7|<-------------------------|SO      |
             -|PD1[D14]     [D11]PC6|------------------------->|SI      |
             -|PC0[D15]     [D10]PC4|------------------------->|~CS     |
             -|PB0[D16]      [D9]PC3|-                         |        |
             -|PB1[D17]      [D8]PD3|-----> LED_HB             |        |
             -|PB3[D18]      [D7]PD2|-                         |        |
             -|PB2[D19]      [D6]PD4|-                         |        |
             -|PD5[D20]      [D5]PD7|-                         |        |
             -|PD6[D21]     [D4]PA13|-                         |        |
             -|PC1[D22]     [D3]PA12|<----- TX_INTERVAL_PIN    |        |
             -|PC2[D23]      [D2]PD0|-                         |        |
             -|3.3V         [D1]PB17|<-------------------------|~INT    |
             -|AGND         [D0]PB16|-                         |________|
             -|VIN               GND|-                          MCP2515
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
                      [D0]---|GPIO16 *             |             |      |________|
                      [D3]---|GPIO0 *              |            2K       MCP2515
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
     TX_INTERVAL_PIN | O D0  |         | D1 O | ~CS
                 SCK | O D5  | ESP-12  | D2 O | ~INT
                MISO | O D6  |         | D3 O |
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
                            |        INT2 - GPIO16|---[D16]<-------|>|-----|~INT    |
                            |                     |                        |________|
 TX_INTERVAL_PIN--->[D17]---|GPIO17               |                        MCP2515 #2
                    [D26]---|GPIO26               |                                  
                    [D22]---|GPIO22               |              ___                 
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
                  GPIO26 |  O D26 |         | D22 O  | GPIO22
       VSPI_SCK - GPIO18 |  O D18 |  ESP32  | D21 O  | GPIO21 - ~INT1
      VSPI_MISO - GPIO19 |  O D19 | WROOM32 | D17 O  | GPIO17 - TX_INTERVAL_PIN
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
                   GPIO36 | O VPD .---------. D22 O |
                   GPIO39 | O VN  |         | TX0 O | GPIO1
                          | O D34 |         | RX0 O | GPIO3
                          | O D35 |  ESP32  | D21 O | GPIO21 - ~INT1
                          | O D32 | WROOM32 | D19 O | VSPI_MISO
                          | O D33 |         | D18 O | VSPI_SCK
                          | O D25 |_________|  D5 O | VSPI_SS - ~CS1
                          | O D26             TX2 O | GPIO17 - TX_INTERVAL_PIN
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

#include <DLK_MCP2515.h>    // CAN Bus library

#define LED_ON              HIGH
#define LED_OFF             LOW

#define TX_INTERVAL_PIN     3   // the transmit interval selection pin
                                // 0 = FAST_TX_INTERVAL
                                // 1 = TX_INTERVAL

// specify CS and interrupt pins to use
#ifdef __AVR__
#define LED_PIN             8               // the heartbeat LED pin (LED_BUILTIN is used for SPI SCK)
#if 1
#define MCP2515_CS_PIN      10
#define MCP2515_INT_PIN     2               // INT0
#else
#define MCP2515_CS_PIN      7
#define MCP2515_INT_PIN     4               // GPIO
#endif
#endif

// specify CS and interrupt pins to use
#ifdef PHILHOWER_RP2040     // Pi Pico
#define LED_PIN             LED_BUILTIN     // the heartbeat LED pin
#define MCP2515_CS_PIN      17
#define MCP2515_INT_PIN     20              // INT
#endif

// specify CS and interrupt pins to use
#ifdef TEENSYDUINO          // Teensy 3.1
#define LED_PIN             8               // the heartbeat LED pin (onboard LED pin is used for SPI SCK)
#define MCP2515_CS_PIN      10
#define MCP2515_INT_PIN     1               // GPIO/INT
#endif

// specify CS and interrupt pins to use (if using "D1 mini", can use WeMos Dn number)
#ifdef ESP8266              // ESP8266
#define LED_PIN             LED_BUILTIN     // WeMos D4 - the heartbeat LED pin (on board)
#define MCP2515_CS_PIN      5               // WeMos D1
#define MCP2515_INT_PIN     4               // WeMos D2 - GPIO/INT
#undef TX_INTERVAL_PIN
#define TX_INTERVAL_PIN     16              // WeMos D0 - the transmit interval selection pin
#undef LED_OFF
#undef LED_ON
#define LED_OFF             HIGH            // ESP8266 built-in LED active-low!
#define LED_ON              LOW             // ESP8266 built-in LED active-low!
#endif

// specify CS and interrupt pins to use
#ifdef ESP32                // ESP32
#define LED_PIN             LED_BUILTIN     // the heartbeat LED pin (on board)
#define MCP2515_CS_PIN      5
#define MCP2515_INT_PIN     21              // GPIO
#undef TX_INTERVAL_PIN
#define TX_INTERVAL_PIN     17              // the transmit interval selection pin
#endif

#define SPI_CLOCK           2000000         // 2 Mbps
#define CAN_SPEED           CAN_250KBPS
#define TX_INTERVAL         500             // mS
#define FAST_TX_INTERVAL    10              // mS

#define TIMER_EXPIRED(start, interval)  ((millis() - start) >= interval)
#define HEARTBEAT_OFF_INTERVAL  950     // mS
#define HEARTBEAT_ON_INTERVAL   50      // mS

DLK_MCP2515 CAN(SPI_CLOCK, MCP2515_CS_PIN);

byte Data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };  // CAN Msg 8 byte data

//===============================================================================
//  Initialization
//===============================================================================
void setup()
{
    // init heartbeat LED
    pinMode(LED_PIN, OUTPUT);
    LED_off();

    // init Tx interval selection input pin - need external pull-op on ESP8266
    pinMode(TX_INTERVAL_PIN, INPUT_PULLUP);

    Serial.begin(115200);   // Initialize communication with Serial monitor
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

    delay(50);

    Serial.println("CAN transmitter test");

    delay(50);
#if 0
    pinMode(MCP2515_CS_PIN, OUTPUT);
    digitalWrite(MCP2515_CS_PIN, HIGH);

    SPI.begin();
#else
    // Initialize MCP2515 running at 8MHz with a baudrate of 250kb/s
    if (CAN.MCP2515_Init(CAN_SPEED) == MCP2515_OK)
    {
        Serial.println("MCP2515 Initialized Successfully!");
    }
    else
    {
        Serial.println("Error Initializing MCP2515...");
        while (1)
        {
            yield();    // needed by ESP8266 to prevent Soft WDT reset "rst cause:2, boot mode:(3,6)"
        }
    }

    CAN.MCP2515_SetMode(MODE_NORMAL);   // Normal mode to allow messages to be transmitted
#endif
}

//===============================================================================
//  Main
//===============================================================================
void loop()
{
#if 0
    // take the SS pin low to select the chip:
    digitalWrite(MCP2515_CS_PIN, LOW);

    //  send in the data via SPI:
    SPI.transfer(0xa5);

    // take the SS pin high to de-select the chip:
    digitalWrite(MCP2515_CS_PIN, HIGH);

    delay(100);
#else
    static uint32_t last_tick = 0;
    static uint32_t interval = TX_INTERVAL;
    static byte val = 0;
    byte sndStat;

    if (digitalRead(TX_INTERVAL_PIN))
    {
        interval = TX_INTERVAL;
    }
    else
    {
        interval = FAST_TX_INTERVAL;
    }

    if (TIMER_EXPIRED(last_tick, interval))
    {
        last_tick = millis();

        // Assign some changing dummy values for the 8 bytes of data
        Data[0] = val;
        Data[1] = val + 1;
        Data[2] = val + 2;
        Data[3] = val + 3;
        Data[4] = val + 4;
        Data[5] = val + 5;
        Data[6] = val + 6;
        Data[7] = val + 7;

        //CAN.MCP2515_Send(msg ID, #of data bytes, data array);
        sndStat = CAN.MCP2515_Send(0x01, sizeof(Data), Data);
        if (sndStat == MCP2515_OK)
        {
            Serial.println("Message Sent Successfully!");
        }
        else
        {
            Serial.println("Error Sending Message...");
        }

        if (val++ > 247)
        {
            val = 0;
        }
    }
#endif
    // do Heartbeat
    DoHeartbeat();
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

