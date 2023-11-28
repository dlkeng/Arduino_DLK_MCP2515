/* CAN OBD & UDS Simple PID Request
 *
 *  Currently requests PID 0x00 at a 1 second interval and
 *  displays all received CAN traffic to the terminal at 115200.
 *
 *  Written By: Cory J. Fowler  April 5th, 2017
 *
 *  (Disclaimer: Standard IDs are currently UNTESTED against a vehicle)
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
             |        |                  -|PD3[D3]              [A6]|-
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

#include <DLK_MCP2515.h>    // CAN Bus library

#define MCP2515_CS_PIN      10
#define MCP2515_INT_PIN     2
#define SPI_CLOCK           2000000         // 2 Mbps
#define CAN_SPEED           CAN_250KBPS

#define TIMER_EXPIRED(start, interval)  ((millis() - start) >= interval)
#define HEARTBEAT_OFF_INTERVAL  950     // mS
#define HEARTBEAT_ON_INTERVAL   50      // mS
#define TX_INTERVAL         1000        // mS
#define FAST_TX_INTERVAL    10          // mS

#define TX_INTERVAL_PIN     3   // the transmit interval selection pin
                                // 0 = FAST_TX_INTERVAL
                                // 1 = TX_INTERVAL

#define LED_PIN     8       // the heartbeat LED pin

#define LED_ON      HIGH
#define LED_OFF     LOW

// What CAN ID type?  Standard (1) or Extended
#define STANDARD    1

// 7E0/8 = Engine ECM
// 7E1/9 = Transmission ECM

#if STANDARD == 1
#define LISTEN_ID       0x7E1
#define REPLY_ID        0x7E0
#define FUNCTIONAL_ID   0x7DF
#else
#define LISTEN_ID       0x98DAF101
#define REPLY_ID        0x98DA01F1
#define FUNCTIONAL_ID   0x98DB33F1
#endif

// CAN TX Variables
uint32_t PrevTx = 0;
uint32_t InvlTx = 1000;

uint8_t TxData[][8] = 
{
    { 0x02, 0x01, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc },    // [0] supported PIDs
    { 0x02, 0x09, 0x02, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc },    // [1] VIN
    { 0x02, 0x09, 0x04, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc },    // [2] Calibration ID
    { 0x02, 0x09, 0x06, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc },    // [3] CVN
    { 0x02, 0x09, 0x0a, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc },    // [4] ECU Name
    { 0x02, 0x09, 0x0d, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc },    // [5] ESN
};

uint8_t TxFlowCtrl[] = {0x30, 0x00, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc};    // Flow Control Frame

// CAN RX Variables
uint32_t RxID;
uint8_t Dlc;
uint8_t RxBuf[8];
char MsgString[128];                        // Array to store serial string

// CAN Interrupt and Chip Select Pins
#define CAN0_INT    MCP2515_INT_PIN             // Set CAN0 INT to pin 2
DLK_MCP2515 CAN0(SPI_CLOCK, MCP2515_CS_PIN);    // Set CAN0 CS to pin 10

void setup()
{
    // init heartbeat LED
    pinMode(LED_PIN, OUTPUT);
    LED_off();

    // init Tx interval selection input pin
    pinMode(TX_INTERVAL_PIN, INPUT_PULLUP);

    // Configuring pin for /INT input
    pinMode(CAN0_INT, INPUT_PULLUP);

    Serial.begin(115200);

    // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
//    if (CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
    // Initialize MCP2515 running at 8MHz with a baudrate of 250kb/s
    if (CAN0.MCP2515_Init(CAN_SPEED) == MCP2515_OK)
    {
        Serial.println("MCP2515 Initialized Successfully!");
    }
    else
    {
        Serial.println("Error Initializing MCP2515... Permanent failure!  Check your code & connections");
        while (1)
        { ; }
    }

    //
    //  // Allow all Standard IDs
    //  CAN0.init_Mask(0,0x00000000);                // Init first mask...
    //  CAN0.init_Filt(0,0x00000000);                // Init first filter...
    //  CAN0.init_Filt(1,0x00000000);                // Init second filter...
    //  // Allow all Extended IDs
    //  CAN0.init_Mask(1,0x80000000);                // Init second mask...
    //  CAN0.init_Filt(2,0x80000000);                // Init third filter...
    //  CAN0.init_Filt(3,0x80000000);                // Init fourth filter...
    //  CAN0.init_Filt(4,0x80000000);                // Init fifth filter...
    //  CAN0.init_Filt(5,0x80000000);                // Init sixth filter...

#if STANDARD == 1
    // Standard ID Filters
    CAN0.MCP2515_SetMask(0, 0x7F0, 0x00, 0x00);             // Init first mask...
    CAN0.MCP2515_SetFilter(0, FUNCTIONAL_ID, 0x00, 0x00);   // Init first filter...
    CAN0.MCP2515_SetFilter(1, 0x7E1, 0x00, 0x00);           // Init second filter...

    CAN0.MCP2515_SetMask(1, 0x7F0, 0x00, 0x00);             // Init second mask...
    CAN0.MCP2515_SetFilter(2, FUNCTIONAL_ID, 0x00, 0x00);   // Init third filter...
    CAN0.MCP2515_SetFilter(3, 0x7E1, 0x00, 0x00);           // Init fourth filter...
    CAN0.MCP2515_SetFilter(4, FUNCTIONAL_ID, 0x00, 0x00);   // Init fifth filter...
    CAN0.MCP2515_SetFilter(5, 0x7E1, 0x00, 0x00);           // Init sixth filter...
#else
    // Extended ID Filters
    CAN0.MCP2515_SetExtMask(0, 0x10FF0000);                 // Init first mask...
    CAN0.MCP2515_SetExtFilter(0, 0x10DA0000);               // Init first filter...
    CAN0.MCP2515_SetExtFilter(1, 0x10DB0000);               // Init second filter...

    CAN0.MCP2515_SetExtMask(1, 0x10FF0000);                 // Init second mask...
    CAN0.MCP2515_SetExtFilter(2, 0x10DA0000);               // Init third filter...
    CAN0.MCP2515_SetExtFilter(3, 0x10DB0000);               // Init fourth filter...
    CAN0.MCP2515_SetExtFilter(4, 0x10DA0000);               // Init fifth filter...
    CAN0.MCP2515_SetExtFilter(5, 0x10DB0000);               // Init sixth filter...
#endif

    CAN0.MCP2515_SetMode(MODE_NORMAL);                  // Set operation mode to normal so the MCP2515 sends acks to received data.

    // Having problems?  ======================================================
    // If you are not receiving any messages, uncomment the setMode line below
    // to test the wiring between the Ardunio and the protocol controller.
    // The message that this sketch sends should be instantly received.
    // ========================================================================
    //CAN0.setMode(MCP_LOOPBACK);

    Serial.println("Simple CAN OBD-II PID Request");
}

void loop()
{
    static uint8_t dat_ndx = 1;
    CAN_FRAME frame;

    if (!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
    {
        //CAN0.readMsgBuf(&RxID, &Dlc, RxBuf);
        if (CAN0.MCP2515_Recv(&frame) == MCP2515_OK)    // Get CAN data
        {
            // Display received CAN data as we receive it.
            if ((frame.can_id & CAN_EFF_FLAG) == CAN_EFF_FLAG)    // Determine if ID is standard (11 bits) or extended (29 bits)
            {
                sprintf(MsgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (frame.can_id & CAN_EFF_MASK), frame.can_dlc);
            }
            else
            {
                sprintf(MsgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", frame.can_id & CAN_SFF_MASK, frame.can_dlc);
            }

            Serial.print(MsgString);

            if ((frame.can_id & CAN_RTR_FLAG) == CAN_RTR_FLAG)    // Determine if message is a remote request frame.
            {
                sprintf(MsgString, " REMOTE REQUEST FRAME");
                Serial.print(MsgString);
            }
            else
            {
                for (uint8_t i = 0; i < frame.can_dlc; i++)
                {
                    sprintf(MsgString, " 0x%.2X", frame.can_data[i]);
                    Serial.print(MsgString);
                }
            }

            Serial.println();

            if ((frame.can_data[0] & 0xf0) == 0x10)     // was First Frame
            {
                PrevTx = millis();
                if (CAN0.MCP2515_Send(LISTEN_ID, 8, TxFlowCtrl) == MCP2515_OK)
                {
                    Serial.println("Flow Control Message Sent Successfully!");
                }
                else
                {
                    Serial.println("Error Sending Flow Control Message...");
                }
            }
        }
    }

    // Every InvlTx, send a request for a PID
    if (TIMER_EXPIRED(PrevTx, InvlTx))
    {
        PrevTx = millis();
        if (CAN0.MCP2515_Send(FUNCTIONAL_ID, 8, TxData[dat_ndx]) == MCP2515_OK)
        {
            Serial.println("Message Sent Successfully!");
        }
        else
        {
            Serial.println("Error Sending Message...");
        }
        switch (dat_ndx)
        {
            case 0:
                dat_ndx = 1;
                break;
            case 1:
                dat_ndx = 2;
                dat_ndx = 4;
                break;
            case 2:
                dat_ndx = 3;
                break;
            case 3:
                dat_ndx = 4;
                break;
            case 4:
                dat_ndx = 5;
                break;
            case 5:
                dat_ndx = 0;
                dat_ndx = 1;
                break;
            default:
                dat_ndx = 0;
                break;
        }
    }

    if (digitalRead(TX_INTERVAL_PIN))
    {
        InvlTx = TX_INTERVAL;
    }
    else
    {
        InvlTx = FAST_TX_INTERVAL;
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

