/*
 * NAME: Multiple_CAN.ino
 *
 * WHAT:
 *  DLK MCP2515 Arduino Library CAN Controller multiple CAN devices test program.
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

/*                  Nano                               Nano
 #1 MCP2515 Pin  Arduino Pin        #2 MCP2515 Pin  Arduino Pin
 ------------------------------     ------------------------------
        VCC         5V                     VCC         5V
        GND         GND                    GND         GND
        CS1         D10                    CS2         D9
        SI(MOSI)    D11                    SI(MOSI)    D11
        SO(MISO)    D12                    SO(MISO)    D12
        SCK         D13                    SCK         D13
        INT         D2 (INT1)              INT         D3 (INT2)

                    Nano                               Nano
 #3 MCP2515 Pin  Arduino Pin        #4 MCP2515 Pin  Arduino Pin
 ------------------------------     ------------------------------
        VCC         5V                     VCC         5V
        GND         GND                    GND         GND
        CS3         D7                     CS4         D6
        SI(MOSI)    D11                    SI(MOSI)    D11
        SO(MISO)    D12                    SO(MISO)    D12
        SCK         D13                    SCK         D13
        INT         D4                     INT         D5
                                                                    _________________________
                                                                   |                         |
                                                                  -|TX0[D1]               VIN|-
                                                                  -|RX0[D0]               GND|-
  ________      ________      ________      ________              -|RST                   RST|-
 |        |    |        |    |        |    |        |             -|GND                   +5V|-
 |    ~INT|----|        | -- |        | -- |        | ------------>|PD2[D2]              [A7]|-
 |        |    |    ~INT| -- |        | -- |        | ------------>|PD3[D3]              [A6]|-
 |        |    |        |    |    ~INT|--- |        | ------------>|PD4[D4]   [SCL/A5/D19]PC5|-
 |        |    |        |    |        |    |    ~INT|------------->|PD5[D5]   [SDA/A4/D18]PC4|-
 |        |    |        |    |        |    |     ~CS|<-------------|PD6[D6]       [A3/D17]PC3|---> CAN_DATA_PIN
 |        |    |        |    |     ~CS|<---|        | -------------|PD7[D7]       [A2/D16]PC2|-
 |        |    |        |    |        |    |        |   LED_HB <---|PB0[D8]       [A1/D15]PC1|-
 |        |    |     ~CS|<---|        | -- |        | -------------|PB1[D9]       [A0/D14]PC0|---> DEBUG_PIN
 |     ~CS|<---|        | -- |        | -- |        | -------------|PB2[D10]             AREF|-
 |      SI|<---|      SI|<---|      SI|<---|      SI|<-------------|PB3[D11]             3.3V|-
 |      SO|----|      SO|----|      SO|----|      SO|------------->|PB4[D12]         [D13]PB5|----.
 |        |    |        |    |        |    |        |              |         .-----.         |    |
 |        |    |        |    |        |    |        |              |_________| USB |_________|    |
 | CAN0a  |    | CAN0b  |    | CAN0c  |    | CAN0d  |                        '-----'              |
 |        |    |        |    |        |    |        |                      Arduino Nano           |
 |        |    |        |    |        |    |        |                                             |
 |     SCK|<-- |     SCK|<-  |     SCK|<-- |     SCK|<--------------------------------------------'
 |________|    |________|    |________|    |________|
  MCP2515 #1   MCP2515 #2     MCP2515 #3   MCP2515 #4
 */

/*                Pi Pico SPI0  Pi Pico SPI1
   MCP2515 Pin    Arduino Pin   Arduino Pin
   -------------------------------------------
        VCC         VSYS          VSYS
        GND         GND           GND
        SO(MISO)    D16           D12
        CS          D17           D13
        SCK         D18           D14
        SI(MOSI)    D19           D15
        INT         D20           D2
                     ___
                    |   |
  ________          2K ---         _________________________                           ________
 |  SPI0  |         |   -         |                         |                         |  SPI1  |
 |      SO|------1K-+------------>|GP16[D16]       [D15]GP15|------------------------>|SI      |
 |   ~CS0A|<----------------------|GP17[D17]       [D14]GP14|------------------------>|SCK     |
 |        |                      -|GND                   GND|-                        |        |
 |     SCK|<----------------------|GP18[D18]       [D13]GP13|------------------------>|~CS1A   |
 |      SI|<----------------------|GP19[D19]       [D12]GP12|<---------------+-1K-----|SO      |
 |  ~INT0A|------1K-+------------>|GP20[D20]       [D11]GP11|------------.   |        |        |
 |        |         |   CS0D <----|GP21[D21]       [D10]GP10|----> CS1C  |  2K        |        |
 |        |        2K            -|GND                   GND|-           |   |        |        |
 |        |         |            -|GP22[D22]         [D9]GP9|----> CS1B  |  ---       | CAN1a  |
 |        |        ---           -|RUN               [D8]GP8|<---- INT1B V   -        |        |
 | CAN0a  |         -  INT0D ---->|GP26[D26]         [D7]GP7|-        CAN_DATA_PIN    |        |
 |        |                      -|GP27[D27]         [D6]GP6|----> CS1C      ___      |        |                  -|GND                   GND|-    __          |        |
 |        |                      -|GP28[D28]         [D5]GP5|----> CS0C     |   |     |        |
 |        |                      -|VREF              [D4]GP4|<---- INT1C   --- 2K     |        |
 |        |                      -|3V3               [D3]GP3|<---- INT1D    -   |     |        |
 |        |                      -|3V3_EN            [D2]GP2|<------------------+-1K--|~INT1A  |
 |        |                      -|GND                   GND|-                        |________|
 |     +5V|<----------------------|VSYS              [D1]GP1|----> CS0B               MCP2515 #2
 |     GND|---.                  -|VBUS              [D0]GP0|----> DEBUG_PIN
 |        |   |                   |         .-----.         |
 |________|  ---                  |_________| USB |_________| LED_HB (onboard)
 MCP2515 #1   -                             '-----'
                                      Raspberry Pi Pico
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

      #3          Teensy               #4          Teensy
   MCP2515 Pin  Arduino Pin         MCP2515 Pin  Arduino Pin
   ---------------------------      ---------------------------
        VCC         VIN                  VCC         VIN
        GND         GND                  GND         GND
        CS          D6                   CS          D5
        SI(MOSI)    D11                  SI(MOSI)    D11
        SO(MISO)    D12                  SO(MISO)    D12
        SCK         D13                  SCK         D13
        INT         D3                   INT         D4
                                                        ________      ________      ________      ________
                                                       |        |    |        |    |        |    |        |
             .---------------------------------------->|SCK     |--->|SCK     |--->|SCK     |--->|SCK     |
             |     _____________________               |  CAN1  |    |  CAN2  |    |  CAN3  |    |  CAN4  |
             |    |                     |              |        |    |        |    |        |    |        |
             '----|PC5[D13]     [D12]PC7|<-------------|SO      |<---|SO      |<---|SO      |<---|SO      |
    DEBUG_PIN <---|PD1[D14]     [D11]PC6|------------->|SI      |--->|SI      |--->|SI      |--->|SI      |
                 -|PC0[D15]     [D10]PC4|------------->|~CS     |    |        |    |        |    |        |
                 -|PB0[D16]      [D9]PC3|-             |        |    |        |    |        |    |        |
 CAN_DATA_PIN <---|PB1[D17]      [D8]PD3|---> LED_HB   |        |    |        |    |        |    |        |
                 -|PB3[D18]      [D7]PD2|------------- |        |--->|~CS     |    |        |    |        |
                 -|PB2[D19]      [D6]PD4|------------- |        | -- |        |--->|~CS     |    |        |
                 -|PD5[D20]      [D5]PD7|------------- |        | -- |        | -- |        |--->|~CS     |
                 -|PD6[D21]     [D4]PA13|<------------ |        | -- |        | -- |        | ---|~INT    |
                 -|PC1[D22]     [D3]PA12|<------------ |        | -- |        | ---|~INT    |    |        |
                 -|PC2[D23]      [D2]PD0|<------------ |        | ---|~INT    |    |        |    |        |
                 -|3.3V         [D1]PB17|<-------------|~INT    |    |        |    |        |    |        |
                 -|AGND         [D0]PB16|-             |________|    |________|    |________|    |________|
                 -|VIN               GND|-             MCP2515 #1    MCP2515 #2    MCP2515 #2    MCP2515 #4
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
                        |     HSPI_SS - GPIO15|---[D15]--------------->|~CS     |
                        |        INT3 - GPIO16|---[D16]<-------|>|-----|~INT    |
                        |                     |                        |________|
                [D17]---|GPIO17               |                        MCP2515 #3
                [D26]---|GPIO26               |              ___
  DEBUG_PIN <---[D22]---|GPIO22               |             |   |
                        |                     |            --- 2K       ________       ________
                        |                     |             -   |      |        |     |        |
                        |   VSPI_MISO - GPIO19|---[D19]<--------+-1K---|SO      |<----|SO      |
                        |    VSPI_SCK - GPIO18|---[D18]--------------->|SCK     |---->|SCK     |
                        |   VSPI_MOSI - GPIO23|---[D23]--------------->|SI      |---->|SI      |
                        |    VSPI_SS1 -  GPIO5|---[D5]---------------->|~CS     |     |        |
                        |        INT1 - GPIO21|---[D21]<-------|>|-----|~INT    |     |        |
                        |                     |                        |________|     |        |
                        |                     |                        MCP2515 #1     |        |
                        |    VSPI_SS2 - GPIO32|---[D32]------------------------------>|~CS     |
                        |        INT2 - GPIO33|---[D33]<----------------------|>|-----|~INT    |
                        |                     |    |                                  |________|
                        |_____________________|    `---D1 D pins                      MCP2515 #2
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
            ~CS2 - GPIO32 | O D32 | WROOM32 | D19 O | VSPI_MISO
           ~INT2 - GPIO33 | O D33 |         | D18 O | VSPI_SCK
                          | O D25 |_________|  D5 O | VSPI_SS1 - ~CS1
    CAN_DATA_PIN - GPIO26 | O D26             TX2 O | GPIO17
                          | O D27             RX2 O | GPIO16 - ~INT3
                 HSPI_SCK | O D14              D4 O |
                HSPI_MISO | O D12              D2 O | GPIO2 - LED_HB (onboard)
                HSPI_MOSI | O D13             D15 O | HSPI_SS - ~CS3
                          | O GND             GND O |
                          | O VIN             3V3 O |
                          |         .-----.         |
                          | RST/EN  | USB |  BOOT   |
                          |_________|=====|_________|
                                ESP32 DEVKIT V1
*/

#include <SPI.h>

#include <DLK_MCP2515.h>

#define SPI_CLOCK           2000000             // 2 Mbps
#define CAN_SPEED           CAN_250KBPS

// specify CS and interrupt pins to use
#ifdef __AVR__
    #define LED_PIN             8           // the heartbeat LED pin (LED_BUILTIN is used for SPI SCK)
    #define CAN_DATA_PIN        A3          // the CAN data indicator pin
    #define DEBUG_PIN           A0          // the Debug pin

    // Limiting to 4 CAN devices is arbitrary - for testing

    // define following to use SPI0 CAN I/F A
//    #define USE_CAN0_A

    // define following to use SPI0 CAN I/F B
//    #define USE_CAN0_B

    // define following to use SPI0 CAN I/F C
    #define USE_CAN0_C

    // define following to use SPI0 CAN I/F D
    #define USE_CAN0_D

    #ifdef USE_CAN0_A
        // define following to use Rx interrupts for CAN0a Rx instead of polling
//        #define USING_RX0A_INTS

        #ifdef USING_RX0A_INTS
            #define MCP2515_INT0A_PIN_SPI0  2           // INT0
        #else
            #define MCP2515_INT0A_PIN_SPI0  2           // GPIO
        #endif
        #define MCP2515_CS0A_PIN_SPI0       10

        DLK_MCP2515 CAN0a(SPI_CLOCK, MCP2515_CS0A_PIN_SPI0);
    #endif

    #ifdef USE_CAN0_B
        // define following to use Rx interrupts for CAN0b Rx instead of polling
//        #define USING_RX0B_INTS

        #ifdef USING_RX0B_INTS
            #define MCP2515_INT0B_PIN_SPI0  3           // INT1
        #else
            #define MCP2515_INT0B_PIN_SPI0  3           // GPIO
        #endif
        #define MCP2515_CS0B_PIN_SPI0       9

        DLK_MCP2515 CAN0b(SPI_CLOCK, MCP2515_CS0B_PIN_SPI0);
    #endif

    #ifdef USE_CAN0_C
        // define following to use Rx interrupts for CAN0c Rx instead of polling
//        #define USING_RX0C_INTS

        #ifdef USING_RX0C_INTS
            #define MCP2515_INT0C_PIN_SPI0  3           // INT1
        #else
            #define MCP2515_INT0C_PIN_SPI0  4           // GPIO
        #endif
        #define MCP2515_CS0C_PIN_SPI0       7

        DLK_MCP2515 CAN0c(SPI_CLOCK, MCP2515_CS0C_PIN_SPI0);
    #endif

    #ifdef USE_CAN0_D
        // define following to use Rx interrupts for CAN0d Rx instead of polling
//        #define USING_RX0D_INTS

        #ifdef USING_RX0D_INTS
            #define MCP2515_INT0D_PIN_SPI0  2           // INT0
        #else
            #define MCP2515_INT0D_PIN_SPI0  5           // GPIO
        #endif
        #define MCP2515_CS0D_PIN_SPI0       6

        DLK_MCP2515 CAN0d(SPI_CLOCK, MCP2515_CS0D_PIN_SPI0);
    #endif

#if (defined(MCP2515_INT0A_PIN_SPI0)                        &&  \
     ((MCP2515_INT0A_PIN_SPI0 == MCP2515_INT0B_PIN_SPI0)    ||  \
      (MCP2515_INT0A_PIN_SPI0 == MCP2515_INT0C_PIN_SPI0)    ||  \
      (MCP2515_INT0A_PIN_SPI0 == MCP2515_INT0D_PIN_SPI0)))
    #error "Interrupt Pin MCP2515_INT0A_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_INT0B_PIN_SPI0)                        &&  \
     ((MCP2515_INT0B_PIN_SPI0 == MCP2515_INT0C_PIN_SPI0)    ||  \
      (MCP2515_INT0B_PIN_SPI0 == MCP2515_INT0D_PIN_SPI0)))
    #error "Interrupt Pin MCP2515_INT0B_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_INT0C_PIN_SPI0)                        &&  \
     (MCP2515_INT0C_PIN_SPI0 == MCP2515_INT0D_PIN_SPI0))
    #error "Interrupt Pin MCP2515_INT0C_PIN_SPI0 Conflict!"
#endif

#if (defined(MCP2515_CS0A_PIN_SPI0)                         &&  \
     ((MCP2515_CS0A_PIN_SPI0 == MCP2515_CS0B_PIN_SPI0)      ||  \
      (MCP2515_CS0A_PIN_SPI0 == MCP2515_CS0C_PIN_SPI0)      ||  \
      (MCP2515_CS0A_PIN_SPI0 == MCP2515_CS0D_PIN_SPI0)))
    #error "Chip Select Pin MCP2515_CS0A_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_CS0B_PIN_SPI0)                         &&  \
     ((MCP2515_CS0B_PIN_SPI0 == MCP2515_CS0C_PIN_SPI0)      ||  \
      (MCP2515_CS0B_PIN_SPI0 == MCP2515_CS0D_PIN_SPI0)))
    #error "Chip Select Pin MCP2515_CS0B_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_CS0C_PIN_SPI0)                         &&  \
      (MCP2515_CS0C_PIN_SPI0 == MCP2515_CS0D_PIN_SPI0))
    #error "Chip Select Pin MCP2515_CS0C_PIN_SPI0 Conflict!"
#endif
#endif

// specify CS and interrupt pins to use
#ifdef PHILHOWER_RP2040         // Pi Pico
    #define LED_PIN             LED_BUILTIN     // the heartbeat LED pin
    #define CAN_DATA_PIN        11              // the CAN data indicator pin
    #define DEBUG_PIN           0               // the Debug pin

    // specify which SPI(s) to use (for Pi Pico, not AVR)

    // define following to use SPI0
    #define USE_SPI0

    // define following to use SPI1
    #define USE_SPI1

    // Limiting to 4 CAN devices per SPI port is arbitrary - for testing

    #ifdef USE_SPI0
        // define following to use SPI0 HW chip-select instead of SW chip-select
        // Note: With HW CS, only 1 SPI device supported per SPI port.
        #define USE_HW_SPI0_CS

        // define following to use SPI0 CAN I/F A
        #define USE_CAN0_A

        // define following to use SPI0 CAN I/F B
//        #define USE_CAN0_B

        // define following to use SPI0 CAN I/F C
//        #define USE_CAN0_C

        // define following to use SPI0 CAN I/F D
//        #define USE_CAN0_D

        #ifdef USE_CAN0_A
            // define following to use Rx interrupts for CAN0a Rx instead of polling
            #define USING_RX0A_INTS

            #define MCP2515_INT0A_PIN_SPI0      20          // INT/GPIO

            #define MCP2515_CS0A_PIN_SPI0       17

            #ifdef USE_HW_SPI0_CS
                DLK_MCP2515 CAN0a(SPI_CLOCK, MCP2515_CS0A_PIN_SPI0, true, SPI0_NUM);
            #else
                DLK_MCP2515 CAN0a(SPI_CLOCK, MCP2515_CS0A_PIN_SPI0, false, SPI0_NUM);
            #endif
        #endif

        #ifdef USE_CAN0_B
            // define following to use Rx interrupts for CAN0b Rx instead of polling
//            #define USING_RX0B_INTS

            #define MCP2515_INT0B_PIN_SPI0      21          // INT/GPIO

            #define MCP2515_CS0B_PIN_SPI0       1

            #ifdef USE_HW_SPI0_CS
                DLK_MCP2515 CAN0b(SPI_CLOCK, MCP2515_CS0B_PIN_SPI0, true, SPI0_NUM);
            #else
                DLK_MCP2515 CAN0b(SPI_CLOCK, MCP2515_CS0B_PIN_SPI0, false, SPI0_NUM);
            #endif
        #endif

        #ifdef USE_CAN0_C
            // define following to use Rx interrupts for CAN0c Rx instead of polling
//            #define USING_RX0C_INTS

            #define MCP2515_INT0C_PIN_SPI0      22          // INT/GPIO

            #define MCP2515_CS0C_PIN_SPI0       5

            #ifdef USE_HW_SPI0_CS
                DLK_MCP2515 CAN0c(SPI_CLOCK, MCP2515_CS0C_PIN_SPI0, true, SPI0_NUM);
            #else
                DLK_MCP2515 CAN0c(SPI_CLOCK, MCP2515_CS0C_PIN_SPI0, false, SPI0_NUM);
            #endif
        #endif

        #ifdef USE_CAN0_D
            // define following to use Rx interrupts for CAN0d Rx instead of polling
//            #define USING_RX0D_INTS

            #define MCP2515_INT0D_PIN_SPI0      26          // INT/GPIO

            #define MCP2515_CS0D_PIN_SPI0       21

            #ifdef USE_HW_SPI0_CS
                DLK_MCP2515 CAN0d(SPI_CLOCK, MCP2515_CS0D_PIN_SPI0, true, SPI0_NUM);
            #else
                DLK_MCP2515 CAN0d(SPI_CLOCK, MCP2515_CS0D_PIN_SPI0, false, SPI0_NUM);
            #endif
        #endif
    #endif

    #ifdef USE_SPI1
        // define following to use SPI1 HW chip-select instead of SW chip-select
        // Note: With HW CS, only 1 SPI device supported per SPI port.
        #define USE_HW_SPI1_CS

        // define following to use SPI1 CAN I/F A
        #define USE_CAN1_A

        // define following to use SPI1 CAN I/F B
//        #define USE_CAN1_B

        // define following to use SPI1 CAN I/F C
//        #define USE_CAN1_C

        // define following to use SPI1 CAN I/F D
//        #define USE_CAN1_D

        #ifdef USE_CAN1_A
            // define following to use Rx interrupts for CAN1a Rx instead of polling
            #define USING_RX1A_INTS

            #define MCP2515_INT1A_PIN_SPI1      2           // INT/GPIO

            #define MCP2515_CS1A_PIN_SPI1       13

            #ifdef USE_HW_SPI1_CS
                DLK_MCP2515 CAN1a(SPI_CLOCK, MCP2515_CS1A_PIN_SPI1, true, SPI1_NUM);
            #else
                DLK_MCP2515 CAN1a(SPI_CLOCK, MCP2515_CS1A_PIN_SPI1, false, SPI1_NUM);
            #endif
        #endif

        #ifdef USE_CAN1_B
            // define following to use Rx interrupts for CAN1b Rx instead of polling
            #define USING_RX1B_INTS

            #define MCP2515_INT1B_PIN_SPI1      8           // INT/GPIO

            #define MCP2515_CS1B_PIN_SPI1       9

            #ifdef USE_HW_SPI1_CS
                DLK_MCP2515 CAN1b(SPI_CLOCK, MCP2515_CS1B_PIN_SPI1, true, SPI1_NUM);
            #else
                DLK_MCP2515 CAN1b(SPI_CLOCK, MCP2515_CS1B_PIN_SPI1, false, SPI1_NUM);
            #endif
        #endif

        #ifdef USE_CAN1_C
            // define following to use Rx interrupts for CAN1c Rx instead of polling
            #define USING_RX1C_INTS

            #define MCP2515_INT1C_PIN_SPI1      4           // INT/GPIO

            #define MCP2515_CS1C_PIN_SPI1       6

            #ifdef USE_HW_SPI1_CS
                DLK_MCP2515 CAN1c(SPI_CLOCK, MCP2515_CS1C_PIN_SPI1, true, SPI1_NUM);
            #else
                DLK_MCP2515 CAN1c(SPI_CLOCK, MCP2515_CS1C_PIN_SPI1, false, SPI1_NUM);
            #endif
        #endif

        #ifdef USE_CAN1_D
            // define following to use Rx interrupts for CAN1d Rx instead of polling
            #define USING_RX1D_INTS

            #define MCP2515_INT1D_PIN_SPI1      3           // INT/GPIO

            #define MCP2515_CS1D_PIN_SPI1       10

            #ifdef USE_HW_SPI1_CS
                DLK_MCP2515 CAN1d(SPI_CLOCK, MCP2515_CS1D_PIN_SPI1, true, SPI1_NUM);
            #else
                DLK_MCP2515 CAN1d(SPI_CLOCK, MCP2515_CS1D_PIN_SPI1, false, SPI1_NUM);
            #endif
        #endif
    #endif

#if defined(USE_HW_SPI0_CS) && (defined(USE_CAN0_B) || defined(USE_CAN0_C) || defined(USE_CAN0_D))
    #error "SPI0 HW CS Conflict!"
#endif

#if defined(USE_HW_SPI1_CS) && (defined(USE_CAN1_B) || defined(USE_CAN1_C) || defined(USE_CAN1_D))
    #error "SPI1 HW CS Conflict!"
#endif

#if (defined(MCP2515_INT0A_PIN_SPI0)                        &&  \
     ((MCP2515_INT0A_PIN_SPI0 == MCP2515_INT0B_PIN_SPI0)    ||  \
      (MCP2515_INT0A_PIN_SPI0 == MCP2515_INT0C_PIN_SPI0)    ||  \
      (MCP2515_INT0A_PIN_SPI0 == MCP2515_INT0D_PIN_SPI0)    ||  \
      (MCP2515_INT0A_PIN_SPI0 == MCP2515_INT1A_PIN_SPI1)    ||  \
      (MCP2515_INT0A_PIN_SPI0 == MCP2515_INT1B_PIN_SPI1)    ||  \
      (MCP2515_INT0A_PIN_SPI0 == MCP2515_INT1C_PIN_SPI1)    ||  \
      (MCP2515_INT0A_PIN_SPI0 == MCP2515_INT1D_PIN_SPI1)))
    #error "Interrupt Pin MCP2515_INT0A_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_INT0B_PIN_SPI0)                        &&  \
     ((MCP2515_INT0B_PIN_SPI0 == MCP2515_INT0C_PIN_SPI0)    ||  \
      (MCP2515_INT0B_PIN_SPI0 == MCP2515_INT0D_PIN_SPI0)    ||  \
      (MCP2515_INT0B_PIN_SPI0 == MCP2515_INT1A_PIN_SPI1)    ||  \
      (MCP2515_INT0B_PIN_SPI0 == MCP2515_INT1B_PIN_SPI1)    ||  \
      (MCP2515_INT0B_PIN_SPI0 == MCP2515_INT1C_PIN_SPI1)    ||  \
      (MCP2515_INT0B_PIN_SPI0 == MCP2515_INT1D_PIN_SPI1)))
    #error "Interrupt Pin MCP2515_INT0B_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_INT0C_PIN_SPI0)                        &&  \
     ((MCP2515_INT0C_PIN_SPI0 == MCP2515_INT0D_PIN_SPI0)    ||  \
      (MCP2515_INT0C_PIN_SPI0 == MCP2515_INT1A_PIN_SPI1)    ||  \
      (MCP2515_INT0C_PIN_SPI0 == MCP2515_INT1B_PIN_SPI1)    ||  \
      (MCP2515_INT0C_PIN_SPI0 == MCP2515_INT1C_PIN_SPI1)    ||  \
      (MCP2515_INT0C_PIN_SPI0 == MCP2515_INT1D_PIN_SPI1)))
    #error "Interrupt Pin MCP2515_INT0C_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_INT0D_PIN_SPI0)                        &&  \
     ((MCP2515_INT0D_PIN_SPI0 == MCP2515_INT1A_PIN_SPI1)    ||  \
      (MCP2515_INT0D_PIN_SPI0 == MCP2515_INT1B_PIN_SPI1)    ||  \
      (MCP2515_INT0D_PIN_SPI0 == MCP2515_INT1C_PIN_SPI1)    ||  \
      (MCP2515_INT0D_PIN_SPI0 == MCP2515_INT1D_PIN_SPI1)))
    #error "Interrupt Pin MCP2515_INT0D_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_INT1A_PIN_SPI1)                        &&  \
     ((MCP2515_INT1A_PIN_SPI1 == MCP2515_INT1B_PIN_SPI1)    ||  \
      (MCP2515_INT1A_PIN_SPI1 == MCP2515_INT1C_PIN_SPI1)    ||  \
      (MCP2515_INT1A_PIN_SPI1 == MCP2515_INT1D_PIN_SPI1)))
    #error "Interrupt Pin MCP2515_INT1A_PIN_SPI1 Conflict!"
#endif
#if (defined(MCP2515_INT1B_PIN_SPI1)                        &&  \
     ((MCP2515_INT1B_PIN_SPI1 == MCP2515_INT1C_PIN_SPI1)    ||  \
      (MCP2515_INT1B_PIN_SPI1 == MCP2515_INT1D_PIN_SPI1)))
    #error "Interrupt Pin MCP2515_INT1B_PIN_SPI1 Conflict!"
#endif
#if (defined(MCP2515_INT1C_PIN_SPI1)                        &&  \
     (MCP2515_INT1C_PIN_SPI1 == MCP2515_INT1D_PIN_SPI1))
    #error "Interrupt Pin MCP2515_INT1C_PIN_SPI1 Conflict!"
#endif

#if (defined(MCP2515_CS0A_PIN_SPI0)                         &&  \
     ((MCP2515_CS0A_PIN_SPI0 == MCP2515_CS0B_PIN_SPI0)      ||  \
      (MCP2515_CS0A_PIN_SPI0 == MCP2515_CS0C_PIN_SPI0)      ||  \
      (MCP2515_CS0A_PIN_SPI0 == MCP2515_CS0D_PIN_SPI0)      ||  \
      (MCP2515_CS0A_PIN_SPI0 == MCP2515_CS1A_PIN_SPI1)      ||  \
      (MCP2515_CS0A_PIN_SPI0 == MCP2515_CS1B_PIN_SPI1)      ||  \
      (MCP2515_CS0A_PIN_SPI0 == MCP2515_CS1C_PIN_SPI1)      ||  \
      (MCP2515_CS0A_PIN_SPI0 == MCP2515_CS1D_PIN_SPI1)))
    #error "Chip Select Pin MCP2515_CS0A_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_CS0B_PIN_SPI0)                         &&  \
     ((MCP2515_CS0B_PIN_SPI0 == MCP2515_CS0C_PIN_SPI0)      ||  \
      (MCP2515_CS0B_PIN_SPI0 == MCP2515_CS0D_PIN_SPI0)      ||  \
      (MCP2515_CS0B_PIN_SPI0 == MCP2515_CS1A_PIN_SPI1)      ||  \
      (MCP2515_CS0B_PIN_SPI0 == MCP2515_CS1B_PIN_SPI1)      ||  \
      (MCP2515_CS0B_PIN_SPI0 == MCP2515_CS1C_PIN_SPI1)      ||  \
      (MCP2515_CS0B_PIN_SPI0 == MCP2515_CS1D_PIN_SPI1)))
    #error "Chip Select Pin MCP2515_CS0B_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_CS0C_PIN_SPI0)                         &&  \
     ((MCP2515_CS0C_PIN_SPI0 == MCP2515_CS0D_PIN_SPI0)      ||  \
      (MCP2515_CS0C_PIN_SPI0 == MCP2515_CS1A_PIN_SPI1)      ||  \
      (MCP2515_CS0C_PIN_SPI0 == MCP2515_CS1B_PIN_SPI1)      ||  \
      (MCP2515_CS0C_PIN_SPI0 == MCP2515_CS1C_PIN_SPI1)      ||  \
      (MCP2515_CS0C_PIN_SPI0 == MCP2515_CS1D_PIN_SPI1)))
    #error "Chip Select Pin MCP2515_CS0C_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_CS0D_PIN_SPI0)                         &&  \
     ((MCP2515_CS0D_PIN_SPI0 == MCP2515_CS1A_PIN_SPI1)      ||  \
      (MCP2515_CS0D_PIN_SPI0 == MCP2515_CS1B_PIN_SPI1)      ||  \
      (MCP2515_CS0D_PIN_SPI0 == MCP2515_CS1C_PIN_SPI1)      ||  \
      (MCP2515_CS0D_PIN_SPI0 == MCP2515_CS1D_PIN_SPI1)))
    #error "Chip Select Pin MCP2515_CS0D_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_CS1A_PIN_SPI1)                         &&  \
     ((MCP2515_CS1A_PIN_SPI1 == MCP2515_CS1B_PIN_SPI1)      ||  \
      (MCP2515_CS1A_PIN_SPI1 == MCP2515_CS1C_PIN_SPI1)      ||  \
      (MCP2515_CS1A_PIN_SPI1 == MCP2515_CS1D_PIN_SPI1)))
    #error "Chip Select Pin MCP2515_CS1A_PIN_SPI1 Conflict!"
#endif
#if (defined(MCP2515_CS1B_PIN_SPI1)                         &&  \
     ((MCP2515_CS1B_PIN_SPI1 == MCP2515_CS1C_PIN_SPI1)      ||  \
      (MCP2515_CS1B_PIN_SPI1 == MCP2515_CS1D_PIN_SPI1)))
    #error "Chip Select Pin MCP2515_CS1B_PIN_SPI1 Conflict!"
#endif
#if (defined(MCP2515_CS1C_PIN_SPI1)                         &&  \
      (MCP2515_CS1C_PIN_SPI1 == MCP2515_CS1D_PIN_SPI1))
    #error "Chip Select Pin MCP2515_CS1B_PIN_SPI1 Conflict!"
#endif
#endif

// specify CS and interrupt pins to use
#ifdef TEENSYDUINO              // Teensy 3.1
    #define LED_PIN             8               // the heartbeat LED pin (onboard LED pin is used for SPI SCK)
    #define CAN_DATA_PIN        17              // the CAN data indicator pin
    #define DEBUG_PIN           14              // the Debug pin

    // Limiting to 4 CAN devices is arbitrary - for testing

    // define following to use SPI0 CAN I/F A
    #define USE_CAN0_A

    // define following to use SPI0 CAN I/F B
    #define USE_CAN0_B

    // define following to use SPI0 CAN I/F C
//    #define USE_CAN0_C

    // define following to use SPI0 CAN I/F D
//    #define USE_CAN0_D

    #ifdef USE_CAN0_A
        // define following to use Rx interrupts for CAN0a Rx instead of polling
        #define USING_RX0A_INTS

        #define MCP2515_INT0A_PIN_SPI0      1           // INT/GPIO

        #define MCP2515_CS0A_PIN_SPI0       10

        DLK_MCP2515 CAN0a(SPI_CLOCK, MCP2515_CS0A_PIN_SPI0);
    #endif

    #ifdef USE_CAN0_B
        // define following to use Rx interrupts for CAN0b Rx instead of polling
        #define USING_RX0B_INTS

        #define MCP2515_INT0B_PIN_SPI0      2           // INT/GPIO

        #define MCP2515_CS0B_PIN_SPI0       7

        DLK_MCP2515 CAN0b(SPI_CLOCK, MCP2515_CS0B_PIN_SPI0);
    #endif

    #ifdef USE_CAN0_C
        // define following to use Rx interrupts for CAN0c Rx instead of polling
//        #define USING_RX0C_INTS

        #define MCP2515_INT0C_PIN_SPI0      3           // INT/GPIO

        #define MCP2515_CS0C_PIN_SPI0       6

        DLK_MCP2515 CAN0c(SPI_CLOCK, MCP2515_CS0C_PIN_SPI0);
    #endif

    #ifdef USE_CAN0_D
        // define following to use Rx interrupts for CAN0d Rx instead of polling
//        #define USING_RX0D_INTS

        #define MCP2515_INT0D_PIN_SPI0      4           // INT/GPIO

        #define MCP2515_CS0D_PIN_SPI0       5

        DLK_MCP2515 CAN0d(SPI_CLOCK, MCP2515_CS0D_PIN_SPI0);
    #endif

#if (defined(MCP2515_INT0A_PIN_SPI0)                        &&  \
     ((MCP2515_INT0A_PIN_SPI0 == MCP2515_INT0B_PIN_SPI0)    ||  \
      (MCP2515_INT0A_PIN_SPI0 == MCP2515_INT0C_PIN_SPI0)    ||  \
      (MCP2515_INT0A_PIN_SPI0 == MCP2515_INT0D_PIN_SPI0)))
    #error "Interrupt Pin MCP2515_INT0A_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_INT0B_PIN_SPI0)                        &&  \
     ((MCP2515_INT0B_PIN_SPI0 == MCP2515_INT0C_PIN_SPI0)    ||  \
      (MCP2515_INT0B_PIN_SPI0 == MCP2515_INT0D_PIN_SPI0)))
    #error "Interrupt Pin MCP2515_INT0B_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_INT0C_PIN_SPI0)                        &&  \
     (MCP2515_INT0C_PIN_SPI0 == MCP2515_INT0D_PIN_SPI0))
    #error "Interrupt Pin MCP2515_INT0C_PIN_SPI0 Conflict!"
#endif

#if (defined(MCP2515_CS0A_PIN_SPI0)                         &&  \
     ((MCP2515_CS0A_PIN_SPI0 == MCP2515_CS0B_PIN_SPI0)      ||  \
      (MCP2515_CS0A_PIN_SPI0 == MCP2515_CS0C_PIN_SPI0)      ||  \
      (MCP2515_CS0A_PIN_SPI0 == MCP2515_CS0D_PIN_SPI0)))
    #error "Chip Select Pin MCP2515_CS0A_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_CS0B_PIN_SPI0)                         &&  \
     ((MCP2515_CS0B_PIN_SPI0 == MCP2515_CS0C_PIN_SPI0)      ||  \
      (MCP2515_CS0B_PIN_SPI0 == MCP2515_CS0D_PIN_SPI0)))
    #error "Chip Select Pin MCP2515_CS0B_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_CS0C_PIN_SPI0)                         &&  \
      (MCP2515_CS0C_PIN_SPI0 == MCP2515_CS0D_PIN_SPI0))
    #error "Chip Select Pin MCP2515_CS0C_PIN_SPI0 Conflict!"
#endif
#endif

// specify CS and interrupt pins to use
#ifdef ESP32                    // ESP32
    #define LED_PIN             LED_BUILTIN     // the heartbeat LED pin (on board)
    #define CAN_DATA_PIN        26              // the CAN data indicator pin
    #define DEBUG_PIN           22              // the Debug pin

    // specify which SPI(s) to use

    // define following to use SPI0
    #define USE_SPI0

    // define following to use SPI1
    #define USE_SPI1

    // Limiting to 4 CAN devices per SPI port is arbitrary - for testing

    #ifdef USE_SPI0
        // define following to use SPI0 CAN I/F A
        #define USE_CAN0_A

        // define following to use SPI0 CAN I/F B
        #define USE_CAN0_B

        // define following to use SPI0 CAN I/F C
//        #define USE_CAN0_C

        // define following to use SPI0 CAN I/F D
//        #define USE_CAN0_D

        #ifdef USE_CAN0_A
            #define MCP2515_INT0A_PIN_SPI0      21          // GPIO

            #define MCP2515_CS0A_PIN_SPI0       5

            DLK_MCP2515 CAN0a(SPI_CLOCK, MCP2515_CS0A_PIN_SPI0, false, SPI0_NUM);
        #endif

        #ifdef USE_CAN0_B
            #define MCP2515_INT0B_PIN_SPI0      33          // GPIO

            #define MCP2515_CS0B_PIN_SPI0       32

            DLK_MCP2515 CAN0b(SPI_CLOCK, MCP2515_CS0B_PIN_SPI0, false, SPI0_NUM);
        #endif

        #ifdef USE_CAN0_C
            #define MCP2515_INT0C_PIN_SPI0      3           // GPIO

            #define MCP2515_CS0C_PIN_SPI0       6

            DLK_MCP2515 CAN0c(SPI_CLOCK, MCP2515_CS0C_PIN_SPI0, false, SPI0_NUM);
        #endif

        #ifdef USE_CAN0_D
            #define MCP2515_INT0D_PIN_SPI0      4           // GPIO

            #define MCP2515_CS0D_PIN_SPI0       5

            DLK_MCP2515 CAN0d(SPI_CLOCK, MCP2515_CS0D_PIN_SPI0, false, SPI0_NUM);
        #endif
    #endif

    #ifdef USE_SPI1
        // define following to use SPI1 CAN I/F A
        #define USE_CAN1_A

        // define following to use SPI1 CAN I/F B
//        #define USE_CAN1_B

        // define following to use SPI1 CAN I/F C
//        #define USE_CAN1_C

        // define following to use SPI1 CAN I/F D
//        #define USE_CAN1_D

        #ifdef USE_CAN1_A
            #define MCP2515_INT1A_PIN_SPI1      16          // GPIO

            #define MCP2515_CS1A_PIN_SPI1       15

            DLK_MCP2515 CAN1a(SPI_CLOCK, MCP2515_CS1A_PIN_SPI1, false, SPI1_NUM);
        #endif

        #ifdef USE_CAN1_B
            #define MCP2515_INT1B_PIN_SPI1      8           // GPIO

            #define MCP2515_CS1B_PIN_SPI1       9

            DLK_MCP2515 CAN1b(SPI_CLOCK, MCP2515_CS1B_PIN_SPI1, false, SPI1_NUM);
        #endif

        #ifdef USE_CAN1_C
            #define MCP2515_INT1C_PIN_SPI1      4           // GPIO

            #define MCP2515_CS1C_PIN_SPI1       6

            DLK_MCP2515 CAN1c(SPI_CLOCK, MCP2515_CS1C_PIN_SPI1, false, SPI1_NUM);
        #endif

        #ifdef USE_CAN1_D
            #define MCP2515_INT1D_PIN_SPI1      3           // GPIO

            #define MCP2515_CS1D_PIN_SPI1       10

            DLK_MCP2515 CAN1d(SPI_CLOCK, MCP2515_CS1D_PIN_SPI1, false, SPI1_NUM);
        #endif
    #endif

#if (defined(MCP2515_INT0A_PIN_SPI0)                        &&  \
     ((MCP2515_INT0A_PIN_SPI0 == MCP2515_INT0B_PIN_SPI0)    ||  \
      (MCP2515_INT0A_PIN_SPI0 == MCP2515_INT0C_PIN_SPI0)    ||  \
      (MCP2515_INT0A_PIN_SPI0 == MCP2515_INT0D_PIN_SPI0)    ||  \
      (MCP2515_INT0A_PIN_SPI0 == MCP2515_INT1A_PIN_SPI1)    ||  \
      (MCP2515_INT0A_PIN_SPI0 == MCP2515_INT1B_PIN_SPI1)    ||  \
      (MCP2515_INT0A_PIN_SPI0 == MCP2515_INT1C_PIN_SPI1)    ||  \
      (MCP2515_INT0A_PIN_SPI0 == MCP2515_INT1D_PIN_SPI1)))
    #error "Interrupt Pin MCP2515_INT0A_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_INT0B_PIN_SPI0)                        &&  \
     ((MCP2515_INT0B_PIN_SPI0 == MCP2515_INT0C_PIN_SPI0)    ||  \
      (MCP2515_INT0B_PIN_SPI0 == MCP2515_INT0D_PIN_SPI0)    ||  \
      (MCP2515_INT0B_PIN_SPI0 == MCP2515_INT1A_PIN_SPI1)    ||  \
      (MCP2515_INT0B_PIN_SPI0 == MCP2515_INT1B_PIN_SPI1)    ||  \
      (MCP2515_INT0B_PIN_SPI0 == MCP2515_INT1C_PIN_SPI1)    ||  \
      (MCP2515_INT0B_PIN_SPI0 == MCP2515_INT1D_PIN_SPI1)))
    #error "Interrupt Pin MCP2515_INT0B_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_INT0C_PIN_SPI0)                        &&  \
     ((MCP2515_INT0C_PIN_SPI0 == MCP2515_INT0D_PIN_SPI0)    ||  \
      (MCP2515_INT0C_PIN_SPI0 == MCP2515_INT1A_PIN_SPI1)    ||  \
      (MCP2515_INT0C_PIN_SPI0 == MCP2515_INT1B_PIN_SPI1)    ||  \
      (MCP2515_INT0C_PIN_SPI0 == MCP2515_INT1C_PIN_SPI1)    ||  \
      (MCP2515_INT0C_PIN_SPI0 == MCP2515_INT1D_PIN_SPI1)))
    #error "Interrupt Pin MCP2515_INT0C_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_INT0D_PIN_SPI0)                        &&  \
     ((MCP2515_INT0D_PIN_SPI0 == MCP2515_INT1A_PIN_SPI1)    ||  \
      (MCP2515_INT0D_PIN_SPI0 == MCP2515_INT1B_PIN_SPI1)    ||  \
      (MCP2515_INT0D_PIN_SPI0 == MCP2515_INT1C_PIN_SPI1)    ||  \
      (MCP2515_INT0D_PIN_SPI0 == MCP2515_INT1D_PIN_SPI1)))
    #error "Interrupt Pin MCP2515_INT0D_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_INT1A_PIN_SPI1)                        &&  \
     ((MCP2515_INT1A_PIN_SPI1 == MCP2515_INT1B_PIN_SPI1)    ||  \
      (MCP2515_INT1A_PIN_SPI1 == MCP2515_INT1C_PIN_SPI1)    ||  \
      (MCP2515_INT1A_PIN_SPI1 == MCP2515_INT1D_PIN_SPI1)))
    #error "Interrupt Pin MCP2515_INT1A_PIN_SPI1 Conflict!"
#endif
#if (defined(MCP2515_INT1B_PIN_SPI1)                        &&  \
     ((MCP2515_INT1B_PIN_SPI1 == MCP2515_INT1C_PIN_SPI1)    ||  \
      (MCP2515_INT1B_PIN_SPI1 == MCP2515_INT1D_PIN_SPI1)))
    #error "Interrupt Pin MCP2515_INT1B_PIN_SPI1 Conflict!"
#endif
#if (defined(MCP2515_INT1C_PIN_SPI1)                        &&  \
     (MCP2515_INT1C_PIN_SPI1 == MCP2515_INT1D_PIN_SPI1))
    #error "Interrupt Pin MCP2515_INT1C_PIN_SPI1 Conflict!"
#endif

#if (defined(MCP2515_CS0A_PIN_SPI0)                         &&  \
     ((MCP2515_CS0A_PIN_SPI0 == MCP2515_CS0B_PIN_SPI0)      ||  \
      (MCP2515_CS0A_PIN_SPI0 == MCP2515_CS0C_PIN_SPI0)      ||  \
      (MCP2515_CS0A_PIN_SPI0 == MCP2515_CS0D_PIN_SPI0)      ||  \
      (MCP2515_CS0A_PIN_SPI0 == MCP2515_CS1A_PIN_SPI1)      ||  \
      (MCP2515_CS0A_PIN_SPI0 == MCP2515_CS1B_PIN_SPI1)      ||  \
      (MCP2515_CS0A_PIN_SPI0 == MCP2515_CS1C_PIN_SPI1)      ||  \
      (MCP2515_CS0A_PIN_SPI0 == MCP2515_CS1D_PIN_SPI1)))
    #error "Chip Select Pin MCP2515_CS0A_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_CS0B_PIN_SPI0)                         &&  \
     ((MCP2515_CS0B_PIN_SPI0 == MCP2515_CS0C_PIN_SPI0)      ||  \
      (MCP2515_CS0B_PIN_SPI0 == MCP2515_CS0D_PIN_SPI0)      ||  \
      (MCP2515_CS0B_PIN_SPI0 == MCP2515_CS1A_PIN_SPI1)      ||  \
      (MCP2515_CS0B_PIN_SPI0 == MCP2515_CS1B_PIN_SPI1)      ||  \
      (MCP2515_CS0B_PIN_SPI0 == MCP2515_CS1C_PIN_SPI1)      ||  \
      (MCP2515_CS0B_PIN_SPI0 == MCP2515_CS1D_PIN_SPI1)))
    #error "Chip Select Pin MCP2515_CS0B_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_CS0C_PIN_SPI0)                         &&  \
     ((MCP2515_CS0C_PIN_SPI0 == MCP2515_CS0D_PIN_SPI0)      ||  \
      (MCP2515_CS0C_PIN_SPI0 == MCP2515_CS1A_PIN_SPI1)      ||  \
      (MCP2515_CS0C_PIN_SPI0 == MCP2515_CS1B_PIN_SPI1)      ||  \
      (MCP2515_CS0C_PIN_SPI0 == MCP2515_CS1C_PIN_SPI1)      ||  \
      (MCP2515_CS0C_PIN_SPI0 == MCP2515_CS1D_PIN_SPI1)))
    #error "Chip Select Pin MCP2515_CS0C_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_CS0D_PIN_SPI0)                         &&  \
     ((MCP2515_CS0D_PIN_SPI0 == MCP2515_CS1A_PIN_SPI1)      ||  \
      (MCP2515_CS0D_PIN_SPI0 == MCP2515_CS1B_PIN_SPI1)      ||  \
      (MCP2515_CS0D_PIN_SPI0 == MCP2515_CS1C_PIN_SPI1)      ||  \
      (MCP2515_CS0D_PIN_SPI0 == MCP2515_CS1D_PIN_SPI1)))
    #error "Chip Select Pin MCP2515_CS0D_PIN_SPI0 Conflict!"
#endif
#if (defined(MCP2515_CS1A_PIN_SPI1)                         &&  \
     ((MCP2515_CS1A_PIN_SPI1 == MCP2515_CS1B_PIN_SPI1)      ||  \
      (MCP2515_CS1A_PIN_SPI1 == MCP2515_CS1C_PIN_SPI1)      ||  \
      (MCP2515_CS1A_PIN_SPI1 == MCP2515_CS1D_PIN_SPI1)))
    #error "Chip Select Pin MCP2515_CS1A_PIN_SPI1 Conflict!"
#endif
#if (defined(MCP2515_CS1B_PIN_SPI1)                         &&  \
     ((MCP2515_CS1B_PIN_SPI1 == MCP2515_CS1C_PIN_SPI1)      ||  \
      (MCP2515_CS1B_PIN_SPI1 == MCP2515_CS1D_PIN_SPI1)))
    #error "Chip Select Pin MCP2515_CS1B_PIN_SPI1 Conflict!"
#endif
#if (defined(MCP2515_CS1C_PIN_SPI1)                         &&  \
      (MCP2515_CS1C_PIN_SPI1 == MCP2515_CS1D_PIN_SPI1))
    #error "Chip Select Pin MCP2515_CS1B_PIN_SPI1 Conflict!"
#endif
#endif

#define LED_ON      HIGH
#define LED_OFF     LOW

#define CAN_DATA_HI()       digitalWrite(CAN_DATA_PIN, HIGH)
#define CAN_DATA_LO()       digitalWrite(CAN_DATA_PIN, LOW)
#define CAN_DATA_TOGL()     digitalWrite(CAN_DATA_PIN, !digitalRead(CAN_DATA_PIN))

#define DEBUG_HI()          digitalWrite(DEBUG_PIN, HIGH)
#define DEBUG_LO()          digitalWrite(DEBUG_PIN, LOW)
#define DEBUG_TOGL()        digitalWrite(DEBUG_PIN, !digitalRead(DEBUG_PIN))

#define TIMER_EXPIRED(start, interval)  ((millis() - start) >= interval)
#define HEARTBEAT_OFF_INTERVAL  950     // mS
#define HEARTBEAT_ON_INTERVAL   50      // mS

#if defined(USING_RX0A_INTS) || defined(USING_RX0B_INTS) || defined(USING_RX0C_INTS) || defined(USING_RX0D_INTS)
volatile bool Rx1IntFlag = false;
CAN_FRAME * RxCAN1_Frame;

volatile bool Rx2IntFlag = false;
CAN_FRAME * RxCAN2_Frame;

volatile bool Rx3IntFlag = false;
CAN_FRAME * RxCAN3_Frame;

volatile bool Rx4IntFlag = false;
CAN_FRAME * RxCAN4_Frame;
#endif

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

    Serial.begin(115200);   // Initialize communication with Serial monitor
    Serial.println("Multiple_CAN.ino");
    Serial.println("Multiple SPI/CAN test");

//Serial.print("Vspi->InstanceCount: ");
//Serial.println(SPIClass::InstanceCount);

    // show features used/not used
#ifdef PHILHOWER_RP2040
    delay(2000);                 // allow time for Arduino's serial window to re-connect

    Serial.println("Raspberry Pi Pico MCU");

    #ifdef USE_SPI0
        Serial.println("Using SPI0");

        Serial.print("Using ");
        #ifdef USE_HW_SPI0_CS
            Serial.print("HW");
        #else
            Serial.print("SW");
        #endif
        Serial.println(" SPI0 CS");

        #ifdef USE_CAN0_A
            // init CS pin
            pinMode(MCP2515_CS0A_PIN_SPI0, OUTPUT);
            digitalWrite(MCP2515_CS0A_PIN_SPI0, HIGH);

            Serial.print("CAN0a ");
            #ifndef USING_RX0A_INTS
                Serial.print("NOT ");
            #endif
            Serial.println("Using SPI0 Rx Interrupts");
        #endif

        #ifdef USE_CAN0_B
            // init CS pin
            pinMode(MCP2515_CS0B_PIN_SPI0, OUTPUT);
            digitalWrite(MCP2515_CS0B_PIN_SPI0, HIGH);

            Serial.print("CAN0b ");
            #ifndef USING_RX0B_INTS
                Serial.print("NOT ");
            #endif
            Serial.println("Using SPI0 Rx Interrupts");
        #endif

        #ifdef USE_CAN0_C
            // init CS pin
            pinMode(MCP2515_CS0C_PIN_SPI0, OUTPUT);
            digitalWrite(MCP2515_CS0C_PIN_SPI0, HIGH);

            Serial.print("CAN0c ");
            #ifndef USING_RX0C_INTS
                Serial.print("NOT ");
            #endif
            Serial.println("Using SPI0 Rx Interrupts");
        #endif

        #ifdef USE_CAN0_D
            // init CS pin
            pinMode(MCP2515_CS0D_PIN_SPI0, OUTPUT);
            digitalWrite(MCP2515_CS0D_PIN_SPI0, HIGH);

            Serial.print("CAN0d ");
            #ifndef USING_RX0D_INTS
                Serial.print("NOT ");
            #endif
            Serial.println("Using SPI0 Rx Interrupts");
        #endif
    #endif

    #ifdef USE_SPI1
        Serial.println("Using SPI1");

        Serial.print("Using ");
        #ifdef USE_HW_SPI1_CS
            Serial.print("HW");
        #else
            Serial.print("SW");
        #endif
        Serial.println(" SPI1 CS");

        #ifdef USE_CAN1_A
            // init CS pin
            pinMode(MCP2515_CS1A_PIN_SPI1, OUTPUT);
            digitalWrite(MCP2515_CS1A_PIN_SPI1, HIGH);

            Serial.print("CAN1a ");
            #ifndef USING_RX1A_INTS
                Serial.print("NOT ");
            #endif
            Serial.println("Using SPI1 Rx Interrupts");
        #endif

        #ifdef USE_CAN1_B
            // init CS pin
            pinMode(MCP2515_CS1B_PIN_SPI1, OUTPUT);
            digitalWrite(MCP2515_CS1B_PIN_SPI1, HIGH);

            Serial.print("CAN1b ");
            #ifndef USING_RX1B_INTS
                Serial.print("NOT ");
            #endif
            Serial.println("Using SPI1 Rx Interrupts");
        #endif

        #ifdef USE_CAN1_C
            // init CS pin
            pinMode(MCP2515_CS1C_PIN_SPI1, OUTPUT);
            digitalWrite(MCP2515_CS1C_PIN_SPI1, HIGH);

            Serial.print("CAN1c ");
            #ifndef USING_RX1C_INTS
                Serial.print("NOT ");
            #endif
            Serial.println("Using SPI1 Rx Interrupts");
        #endif

        #ifdef USE_CAN1_D
            // init CS pin
            pinMode(MCP2515_CS1D_PIN_SPI1, OUTPUT);
            digitalWrite(MCP2515_CS1D_PIN_SPI1, HIGH);

            Serial.print("CAN1d ");
            #ifndef USING_RX1D_INTS
                Serial.print("NOT ");
            #endif
            Serial.println("Using SPI1 Rx Interrupts");
        #endif
    #endif
#endif

#ifdef TEENSYDUINO
    delay(2000);                 // allow time for Arduino's serial window to re-connect
    Serial.println("Teensy MCU");

    Serial.println("Using SW SPI CS");

    #ifdef USE_CAN0_A
        // init CS pin
        pinMode(MCP2515_CS0A_PIN_SPI0, OUTPUT);
        digitalWrite(MCP2515_CS0A_PIN_SPI0, HIGH);

        Serial.print("CAN0a ");
        #ifndef USING_RX0A_INTS
            Serial.print("NOT ");
        #endif
        Serial.println("Using SPI Rx Interrupts");
    #endif

    #ifdef USE_CAN0_B
        // init CS pin
        pinMode(MCP2515_CS0B_PIN_SPI0, OUTPUT);
        digitalWrite(MCP2515_CS0B_PIN_SPI0, HIGH);

        Serial.print("CAN0b ");
        #ifndef USING_RX0B_INTS
            Serial.print("NOT ");
        #endif
        Serial.println("Using SPI Rx Interrupts");
    #endif

    #ifdef USE_CAN0_C
        // init CS pin
        pinMode(MCP2515_CS0C_PIN_SPI0, OUTPUT);
        digitalWrite(MCP2515_CS0C_PIN_SPI0, HIGH);

        Serial.print("CAN0c ");
        #ifndef USING_RX0C_INTS
            Serial.print("NOT ");
        #endif
        Serial.println("Using SPI Rx Interrupts");
    #endif

    #ifdef USE_CAN0_D
        // init CS pin
        pinMode(MCP2515_CS0D_PIN_SPI0, OUTPUT);
        digitalWrite(MCP2515_CS0D_PIN_SPI0, HIGH);

        Serial.print("CAN0d ");
        #ifndef USING_RX0D_INTS
            Serial.print("NOT ");
        #endif
        Serial.println("Using SPI Rx Interrupts");
    #endif
#endif

#ifdef ESP32
    delay(2000);                 // allow time for Arduino's serial window to re-connect

    Serial.println();
    Serial.println("ESP32 MCU");

    #ifdef USE_SPI0
        Serial.println("Using SPI0");

        Serial.println("Using SW SPI CS");

        #ifdef USE_CAN0_A
            // init CS pin
            pinMode(MCP2515_CS0A_PIN_SPI0, OUTPUT);
            digitalWrite(MCP2515_CS0A_PIN_SPI0, HIGH);

            Serial.print("CAN0a ");
            #ifndef USING_RX0A_INTS
                Serial.print("NOT ");
            #endif
            Serial.println("Using SPI Rx Interrupts");
        #endif

        #ifdef USE_CAN0_B
            // init CS pin
            pinMode(MCP2515_CS0B_PIN_SPI0, OUTPUT);
            digitalWrite(MCP2515_CS0B_PIN_SPI0, HIGH);

            Serial.print("CAN0b ");
            #ifndef USING_RX0B_INTS
                Serial.print("NOT ");
            #endif
            Serial.println("Using SPI Rx Interrupts");
        #endif

        #ifdef USE_CAN0_C
            // init CS pin
            pinMode(MCP2515_CS0C_PIN_SPI0, OUTPUT);
            digitalWrite(MCP2515_CS0C_PIN_SPI0, HIGH);

            Serial.print("CAN0c ");
            #ifndef USING_RX0C_INTS
                Serial.print("NOT ");
            #endif
            Serial.println("Using SPI Rx Interrupts");
        #endif

        #ifdef USE_CAN0_D
            // init CS pin
            pinMode(MCP2515_CS0D_PIN_SPI0, OUTPUT);
            digitalWrite(MCP2515_CS0D_PIN_SPI0, HIGH);

            Serial.print("CAN0d ");
            #ifndef USING_RX0D_INTS
                Serial.print("NOT ");
            #endif
            Serial.println("Using SPI Rx Interrupts");
        #endif
    #endif

    #ifdef USE_SPI1
        Serial.println("Using SPI1");       // actually "SPIH" from DLK_MCP2515_Library

        Serial.println("Using SW SPI CS");

        #ifdef USE_CAN1_A
            // init CS pin
            pinMode(MCP2515_CS1A_PIN_SPI1, OUTPUT);
            digitalWrite(MCP2515_CS1A_PIN_SPI1, HIGH);

            Serial.print("CAN1a ");
            #ifndef USING_RX1A_INTS
                Serial.print("NOT ");
            #endif
            Serial.println("Using SPI1 Rx Interrupts");
        #endif

        #ifdef USE_CAN1_B
            // init CS pin
            pinMode(MCP2515_CS1B_PIN_SPI1, OUTPUT);
            digitalWrite(MCP2515_CS1B_PIN_SPI1, HIGH);

            Serial.print("CAN1b ");
            #ifndef USING_RX1B_INTS
                Serial.print("NOT ");
            #endif
            Serial.println("Using SPI1 Rx Interrupts");
        #endif

        #ifdef USE_CAN1_C
            // init CS pin
            pinMode(MCP2515_CS1C_PIN_SPI1, OUTPUT);
            digitalWrite(MCP2515_CS1C_PIN_SPI1, HIGH);

            Serial.print("CAN1c ");
            #ifndef USING_RX1C_INTS
                Serial.print("NOT ");
            #endif
            Serial.println("Using SPI1 Rx Interrupts");
        #endif

        #ifdef USE_CAN1_D
            // init CS pin
            pinMode(MCP2515_CS1D_PIN_SPI1, OUTPUT);
            digitalWrite(MCP2515_CS1D_PIN_SPI1, HIGH);

            Serial.print("CAN1d ");
            #ifndef USING_RX1D_INTS
                Serial.print("NOT ");
            #endif
            Serial.println("Using SPI1 Rx Interrupts");
        #endif
    #endif
#endif

#ifdef __AVR__
    Serial.println("AVR MCU");

    Serial.println("Using SW SPI CS");

    #ifdef USE_CAN0_A
        // init CS pin
        pinMode(MCP2515_CS0A_PIN_SPI0, OUTPUT);
        digitalWrite(MCP2515_CS0A_PIN_SPI0, HIGH);

        Serial.print("CAN0a ");
        #ifndef USING_RX0A_INTS
            Serial.print("NOT ");
        #endif
        Serial.println("Using SPI Rx Interrupts");
    #endif

    #ifdef USE_CAN0_B
        // init CS pin
        pinMode(MCP2515_CS0B_PIN_SPI0, OUTPUT);
        digitalWrite(MCP2515_CS0B_PIN_SPI0, HIGH);

        Serial.print("CAN0b ");
        #ifndef USING_RX0B_INTS
            Serial.print("NOT ");
        #endif
        Serial.println("Using SPI Rx Interrupts");
    #endif

    #ifdef USE_CAN0_C
        // init CS pin
        pinMode(MCP2515_CS0C_PIN_SPI0, OUTPUT);
        digitalWrite(MCP2515_CS0C_PIN_SPI0, HIGH);

        Serial.print("CAN0c ");
        #ifndef USING_RX0C_INTS
            Serial.print("NOT ");
        #endif
        Serial.println("Using SPI Rx Interrupts");
    #endif

    #ifdef USE_CAN0_D
        // init CS pin
        pinMode(MCP2515_CS0D_PIN_SPI0, OUTPUT);
        digitalWrite(MCP2515_CS0D_PIN_SPI0, HIGH);

        Serial.print("CAN0d ");
        #ifndef USING_RX0D_INTS
            Serial.print("NOT ");
        #endif
        Serial.println("Using SPI Rx Interrupts");
    #endif
#endif

    delay(50);

    // initialize MCP2515 CAN
#ifdef PHILHOWER_RP2040     // Pi Pico
    #ifdef USE_SPI0
        #ifdef USE_CAN0_A
            // Initialize MCP2515
            #ifdef USING_RX0A_INTS
                CAN_Setup(&CAN0a, MCP2515_INT0A_PIN_SPI0, "CAN0a", true);
            #else
                CAN_Setup(&CAN0a, MCP2515_INT0A_PIN_SPI0, "CAN0a", false);
            #endif
        #endif

        #ifdef USE_CAN0_B
            // Initialize MCP2515
            #ifdef USING_RX0B_INTS
                CAN_Setup(&CAN0b, MCP2515_INT0B_PIN_SPI0, "CAN0b", true);
            #else
                CAN_Setup(&CAN0b, MCP2515_INT0B_PIN_SPI0, "CAN0b", false);
            #endif
        #endif

        #ifdef USE_CAN0_C
            // Initialize MCP2515
            #ifdef USING_RX0C_INTS
                CAN_Setup(&CAN0c, MCP2515_INT0C_PIN_SPI0, "CAN0c", true);
            #else
                CAN_Setup(&CAN0c, MCP2515_INT0C_PIN_SPI0, "CAN0c", false);
            #endif
        #endif

        #ifdef USE_CAN0_D
            // Initialize MCP2515
            #ifdef USING_RX0D_INTS
                CAN_Setup(&CAN0d, MCP2515_INT0D_PIN_SPI0, "CAN0d", true);
            #else
                CAN_Setup(&CAN0d, MCP2515_INT0D_PIN_SPI0, "CAN0d", false);
            #endif
        #endif
    #endif

    #ifdef USE_SPI1
        #ifdef USE_CAN1_A
            // Initialize MCP2515
            #ifdef USING_RX1A_INTS
                CAN_Setup(&CAN1a, MCP2515_INT1A_PIN_SPI1, "CAN1a", true);
            #else
                CAN_Setup(&CAN1a, MCP2515_INT1A_PIN_SPI1, "CAN1a", false);
            #endif
        #endif

        #ifdef USE_CAN1_B
            // Initialize MCP2515
            #ifdef USING_RX1B_INTS
                CAN_Setup(&CAN1b, MCP2515_INT1B_PIN_SPI1, "CAN1b", true);
            #else
                CAN_Setup(&CAN1b, MCP2515_INT1B_PIN_SPI1, "CAN1b", false);
            #endif
        #endif

        #ifdef USE_CAN1_C
            // Initialize MCP2515
            #ifdef USING_RX1C_INTS
                CAN_Setup(&CAN1c, MCP2515_INT1C_PIN_SPI1, "CAN1c", true);
            #else
                CAN_Setup(&CAN1c, MCP2515_INT1C_PIN_SPI1, "CAN1c", false);
            #endif
        #endif

        #ifdef USE_CAN1_D
            // Initialize MCP2515
            #ifdef USING_RX1D_INTS
                CAN_Setup(&CAN1d, MCP2515_INT1D_PIN_SPI1, "CAN1d", true);
            #else
                CAN_Setup(&CAN1d, MCP2515_INT1D_PIN_SPI1, "CAN1d", false);
            #endif
        #endif
    #endif
#endif

#if defined(__AVR__) || defined(TEENSYDUINO)
    #ifdef USE_CAN0_A
        // Initialize MCP2515
        #ifdef USING_RX0A_INTS
            CAN_Setup(&CAN0a, MCP2515_INT0A_PIN_SPI0, "CAN0a", true);
        #else
            CAN_Setup(&CAN0a, MCP2515_INT0A_PIN_SPI0, "CAN0a", false);
        #endif
    #endif

    #ifdef USE_CAN0_B
        // Initialize MCP2515
        #ifdef USING_RX0B_INTS
            CAN_Setup(&CAN0b, MCP2515_INT0B_PIN_SPI0, "CAN0b", true);
        #else
            CAN_Setup(&CAN0b, MCP2515_INT0B_PIN_SPI0, "CAN0b", false);
        #endif
    #endif

    #ifdef USE_CAN0_C
        // Initialize MCP2515
        #ifdef USING_RX0C_INTS
            CAN_Setup(&CAN0c, MCP2515_INT0C_PIN_SPI0, "CAN0c", true);
        #else
            CAN_Setup(&CAN0c, MCP2515_INT0C_PIN_SPI0, "CAN0c", false);
        #endif
    #endif

    #ifdef USE_CAN0_D
        // Initialize MCP2515
        #ifdef USING_RX0D_INTS
            CAN_Setup(&CAN0d, MCP2515_INT0D_PIN_SPI0, "CAN0d", true);
        #else
            CAN_Setup(&CAN0d, MCP2515_INT0D_PIN_SPI0, "CAN0d", false);
        #endif
    #endif
#endif

#ifdef ESP32     // ESP32
    #ifdef USE_SPI0
        #ifdef USE_CAN0_A
            // Initialize MCP2515
            CAN_Setup(&CAN0a, MCP2515_INT0A_PIN_SPI0, "CAN0a", false);
        #endif

        #ifdef USE_CAN0_B
            // Initialize MCP2515
            CAN_Setup(&CAN0b, MCP2515_INT0B_PIN_SPI0, "CAN0b", false);
        #endif

        #ifdef USE_CAN0_C
            // Initialize MCP2515
            CAN_Setup(&CAN0c, MCP2515_INT0C_PIN_SPI0, "CAN0c", false);
        #endif

        #ifdef USE_CAN0_D
            // Initialize MCP2515
            CAN_Setup(&CAN0d, MCP2515_INT0D_PIN_SPI0, "CAN0d", false);
        #endif
    #endif

    #ifdef USE_SPI1
        #ifdef USE_CAN1_A
            // Initialize MCP2515
            CAN_Setup(&CAN1a, MCP2515_INT1A_PIN_SPI1, "CAN1a", false);
        #endif

        #ifdef USE_CAN1_B
            // Initialize MCP2515
            CAN_Setup(&CAN1b, MCP2515_INT1B_PIN_SPI1, "CAN1b", false);
            #endif

        #ifdef USE_CAN1_C
            // Initialize MCP2515
            CAN_Setup(&CAN1c, MCP2515_INT1C_PIN_SPI1, "CAN1c", false);
        #endif

        #ifdef USE_CAN1_D
            // Initialize MCP2515
            CAN_Setup(&CAN1d, MCP2515_INT1D_PIN_SPI1, "CAN1d", false);
        #endif
    #endif
#endif

DEBUG_TOGL();
}

void loop()
{
    CAN_FRAME frame;

    // check for CAN Rx interrupts
#if defined(USING_RX0A_INTS) || defined(USING_RX0B_INTS) || defined(USING_RX0C_INTS) || defined(USING_RX0D_INTS)
    if (Rx1IntFlag)      // got CAN Rx interrupt
    {
        Rx1IntFlag = false;
        frame = *RxCAN1_Frame;
        RecvCanData(&frame, "Rx1");
    }

    if (Rx2IntFlag)      // got CAN Rx interrupt
    {
        Rx2IntFlag = false;
        frame = *RxCAN2_Frame;
        RecvCanData(&frame, "Rx2");
    }

    if (Rx3IntFlag)      // got CAN Rx interrupt
    {
        Rx3IntFlag = false;
        frame = *RxCAN3_Frame;
        RecvCanData(&frame, "Rx3");
    }

    if (Rx4IntFlag)      // got CAN Rx interrupt
    {
        Rx4IntFlag = false;
        frame = *RxCAN4_Frame;
        RecvCanData(&frame, "Rx4");
    }
#endif

     // process CAN receiving and sending
#ifdef USE_CAN0_A
    #ifndef USING_RX0A_INTS
        if (digitalRead(MCP2515_INT0A_PIN_SPI0) == LOW)
        {
            if (CAN0a.MCP2515_Recv(&frame) == MCP2515_OK)
            {
                RecvCanData(&frame, "CAN0a");
            }
        }
    #endif
    SendCanData(&CAN0a, 0, "CAN0a");
#endif

#ifdef USE_CAN0_B
    #ifndef USING_RX0B_INTS
        if (digitalRead(MCP2515_INT0B_PIN_SPI0) == LOW)
        {
            if (CAN0b.MCP2515_Recv(&frame) == MCP2515_OK)
            {
                RecvCanData(&frame, "CAN0b");
            }
        }
    #endif
    SendCanData(&CAN0b, 16, "CAN0b");
#endif

#ifdef USE_CAN0_C
    #ifndef USING_RX0C_INTS
        if (digitalRead(MCP2515_INT0C_PIN_SPI0) == LOW)
        {
            if (CAN0c.MCP2515_Recv(&frame) == MCP2515_OK)
            {
                RecvCanData(&frame, "CAN0c");
            }
        }
    #endif
    SendCanData(&CAN0c, 32, "CAN0c");
#endif

#ifdef USE_CAN0_D
    #ifndef USING_RX0D_INTS
        if (digitalRead(MCP2515_INT0D_PIN_SPI0) == LOW)
        {
            if (CAN0d.MCP2515_Recv(&frame) == MCP2515_OK)
            {
                RecvCanData(&frame, "CAN0d");
            }
        }
    #endif
    SendCanData(&CAN0d, 48, "CAN0d");
#endif

#ifdef USE_CAN1_A
    #ifndef USING_RX1A_INTS
        if (digitalRead(MCP2515_INT1A_PIN_SPI1) == LOW)
        {
            if (CAN1a.MCP2515_Recv(&frame) == MCP2515_OK)
            {
                RecvCanData(&frame, "CAN1a");
            }
        }
    #endif
    SendCanData(&CAN1a, 64, "CAN1a");
#endif

#ifdef USE_CAN1_B
    #ifndef USING_RX1B_INTS
        if (digitalRead(MCP2515_INT1B_PIN_SPI1) == LOW)
        {
            if (CAN1b.MCP2515_Recv(&frame) == MCP2515_OK)
            {
                RecvCanData(&frame, "CAN1b");
            }
        }
    #endif
    SendCanData(&CAN0b, 80, "CAN1b");
#endif

#ifdef USE_CAN1_C
    #ifndef USING_RX1C_INTS
        if (digitalRead(MCP2515_INT1C_PIN_SPI1) == LOW)
        {
            if (CAN1c.MCP2515_Recv(&frame) == MCP2515_OK)
            {
                RecvCanData(&frame, "CAN1c");
            }
        }
    #endif
    SendCanData(&CAN0c, 96, "CAN1c");
#endif

#ifdef USE_CAN1_D
    #ifndef USING_RX1D_INTS
        if (digitalRead(MCP2515_INT1D_PIN_SPI1) == LOW)
        {
            if (CAN1d.MCP2515_Recv(&frame) == MCP2515_OK)
            {
                RecvCanData(&frame, "CAN1d");
            }
        }
    #endif
    SendCanData(&CAN0d, 112, "CAN1d");
#endif

    // do Heartbeat
    DoHeartbeat();
}

void SendCanData(DLK_MCP2515 * mcp, uint8_t val, const char * title)
{
    static uint32_t last_tick = 0;
    static uint32_t interval = 2000;
    static uint8_t last_val = 99;
    uint8_t data[8];

    if (TIMER_EXPIRED(last_tick, interval))
    {
        if (val == last_val)    // force taking turns sending
        {
            return;
        }

        last_tick = millis();
        last_val = val;

        // Assign some values for the 8 bytes of data
        data[0] = val;
        data[1] = val + 1;
        data[2] = val + 2;
        data[3] = val + 3;
        data[4] = val + 4;
        data[5] = val + 5;
        data[6] = val + 6;
        data[7] = val + 7;

        Serial.print(title);
        //mcp->MCP2515_Send(msg ID, #of data bytes, data array);
DEBUG_TOGL();
        if (mcp->MCP2515_Send(0x700 + val, sizeof(data), data) == MCP2515_OK)
        {
//DEBUG_TOGL();
            Serial.println(": Message Sent Successfully!");
        }
        else
        {
            Serial.println(": Error Sending Message...");
        }
    }
}

void RecvCanData(CAN_FRAME * frame, const char * title)
{
    uint8_t len = 0;    // length of received buffer
    uint8_t buf[8];     // Buffer to hold up to 8 bytes of data
    uint32_t canID;     // Can message ID

static uint32_t last_toggle = 0;
if (TIMER_EXPIRED(last_toggle, 100))
{
    CAN_DATA_TOGL();
    last_toggle = millis();
}
    // Read data, len: data length, buf: data buffer
    canID = frame->can_id;
    len = frame->can_dlc;
    memcpy(buf, frame->can_data, len);

    Serial.print(title);
    Serial.print(": CAN ID: 0x");
    Serial.print(canID, HEX);       // print the CAN ID in HEX

    Serial.print("    Data Length: "); // Print the length of the received data
    Serial.print(len);
    Serial.print("    ");

    for (int i = 0; i < len; i++)    // loop on the incoming data to print each byte
    {
        PrintHexByte(buf[i]);
        if (i < len - 1)
        {
            Serial.print(",");      // Separate the numbers for readability
        }
    }
    Serial.println();
}

#if defined(USING_RX0A_INTS) || defined(USING_RX0B_INTS) || defined(USING_RX0C_INTS) || defined(USING_RX0D_INTS)
// Interrupt callback handler for Rx interrupt for 1st CAN interrupt
void RxInt1Handler(CAN_FRAME * frame)
{
    RxCAN1_Frame = frame;    // save reference to CAN Rx data

    Rx1IntFlag = true;       // set notification of received CAN data
}

// Interrupt callback handler for Rx interrupt for 2nd CAN interrupt
void RxInt2Handler(CAN_FRAME * frame)
{
    RxCAN2_Frame = frame;    // save reference to CAN Rx data

    Rx2IntFlag = true;       // set notification of received CAN data
}

// Interrupt callback handler for Rx interrupt for 3rd CAN interrupt
void RxInt3Handler(CAN_FRAME * frame)
{
    RxCAN3_Frame = frame;    // save reference to CAN Rx data

    Rx3IntFlag = true;       // set notification of received CAN data
}

// Interrupt callback handler for Rx interrupt for 4th CAN interrupt
void RxInt4Handler(CAN_FRAME * frame)
{
    RxCAN4_Frame = frame;    // save reference to CAN Rx data

    Rx4IntFlag = true;       // set notification of received CAN data
}
#endif

void CAN_Setup(DLK_MCP2515 * mcp, int int_pin, const char * title, bool use_int)
{
    static uint8_t can_int_cnt = 0;
    uint8_t rslt;

DEBUG_TOGL();
    // Initialize MCP2515 running at 8MHz with a baudrate of 250kb/s
    rslt = mcp->MCP2515_Init(CAN_SPEED);
    if (rslt != MCP2515_OK)
    {
        Serial.print(title);
        Serial.print(": [");
        Serial.print(rslt);
        Serial.println("] Error Initializing MCP2515...");
        while (1)
        {
            yield();    // needed by ESP8266 to prevent Soft WDT reset "rst cause:2, boot mode:(3,6)"
        }
    }

    Serial.print(title);

    if (use_int)
    {
#if defined(USING_RX0A_INTS) || defined(USING_RX0B_INTS) || defined(USING_RX0C_INTS) || defined(USING_RX0D_INTS)
        // set CAN Rx interrupt callback
        switch (can_int_cnt)
        {
            case 0:
                rslt = mcp->MCP2515_OnRxInterrupt(int_pin, RxInt1Handler);
                if (rslt != MCP2515_OK)
                {
                    Serial.print(": Failed attaching MCP2515 ");
                    Serial.print(title);
                    Serial.println("_INT ...");
                    while (1)
                    {
                        yield();    // needed by ESP8266 to prevent Soft WDT reset "rst cause:2, boot mode:(3,6)"
                    }
                }
                break;
            case 1:
                rslt = mcp->MCP2515_OnRxInterrupt(int_pin, RxInt2Handler);
                if (rslt != MCP2515_OK)
                {
                    Serial.print(": Failed attaching MCP2515 ");
                    Serial.print(title);
                    Serial.println("_INT ...");
                    while (1)
                    {
                        yield();    // needed by ESP8266 to prevent Soft WDT reset "rst cause:2, boot mode:(3,6)"
                    }
                }
                break;
#if (MAX_INTS > 2)
            case 2:
                rslt = mcp->MCP2515_OnRxInterrupt(int_pin, RxInt3Handler);
                if (rslt != MCP2515_OK)
                {
                    Serial.print(": Failed attaching MCP2515 ");
                    Serial.print(title);
                    Serial.println("_INT ...");
                    while (1)
                    {
                        yield();    // needed by ESP8266 to prevent Soft WDT reset "rst cause:2, boot mode:(3,6)"
                    }
                }
                break;
            case 3:
                rslt = mcp->MCP2515_OnRxInterrupt(int_pin, RxInt4Handler);
                if (rslt != MCP2515_OK)
                {
                    Serial.print(": Failed attaching MCP2515 ");
                    Serial.print(title);
                    Serial.println("_INT ...");
                    while (1)
                    {
                        yield();    // needed by ESP8266 to prevent Soft WDT reset "rst cause:2, boot mode:(3,6)"
                    }
                }
                break;
#endif
            default:
                Serial.println(": No more available MCP2515 CAN interrupts ...");
                while (1)
                {
                    yield();    // needed by ESP8266 to prevent Soft WDT reset "rst cause:2, boot mode:(3,6)"
                }
                break;
        }
#endif
        ++can_int_cnt;
    }
    else
    {
        // init MCP2515 Int input pin
        pinMode(int_pin, INPUT_PULLUP);
    }

    Serial.println(": MCP2515 Initialized Successfully!");
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

