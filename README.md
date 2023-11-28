# Arduino DLK_MCP2515 Library
Arduino library for available MCP2515 standalone CAN controller modules.

[image](./docs/mcp2515-2.jpg)

DLK_MCP2515(spi_speed, cs_pin, hw_cs_pin = false, which_spi = SPI0_NUM);

SPIs | Ints | SW_CS | HW_CS
:---:|:----:|:-----:|------------------------------------
  2  |   4  |  many |  2 (1-each SPI port, Pi Pico only)     


AVR Nano
------------
 - 1 SPI port with SW CS only
 - 2 INT interrupt pins

Scenarios:
----------
 - up to 4 SPI devices with interrupts (limited by INT pins)
 - many SPI devices without interrupts (limited by GPIO pins for SW CS)


Pi Pico
--------------
 - 2 SPI ports optionally supporting HW CS or SW CS
 - many GPIO interrupt pins

Scenarios:
----------
SPI0:
 - up to 4 SPI devices with interrupts (total limited by DLK_MCP2515 Library)
 - 1 SPI device only with HW CS
 - many SPI devices without interrupts (limited by GPIO pins for SW CS)

SPI1:
 - up to 4 SPI devices with interrupts (total limited by DLK_MCP2515 Library)
 - 1 SPI device only with HW CS
 - many SPI devices without interrupts (limited by GPIO pins for SW CS)


Teensy 3.1
--------------
 - 1 SPI port with SW CS only
 - many GPIO interrupt pins

Scenarios:
----------
 - up to 4 SPI devices with interrupts (total limited by DLK_MCP2515 Library)
 - many SPI devices without interrupts (limited by GPIO pins for SW CS)


ESP8266
--------------
 - 1 SPI port with SW CS only
 - NO GPIO interrupt pins

Scenarios:
----------
 - No SPI devices with interrupts
 - 1 SPI device without interrupts (limited by GPIO pins for SW CS)


ESP32
--------------
 - 2 SPI ports with SW CS only
 - NO GPIO interrupt pins

Scenarios:
----------
 - No SPI devices with interrupts
 - many SPI devices without interrupts (limited by GPIO pins for SW CS)
