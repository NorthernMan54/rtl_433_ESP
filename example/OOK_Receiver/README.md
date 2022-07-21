This is a sample implementation of the rtl_433_ESP library with the CC1101 or the SX127X (Heltec) Transceiver Module.

# Building and implementing the sample

To build and deploy the sample I used Visual Studio Code and PlatformIO.  This development environment will pull in all the required libraries as part of the build.

1 - Clone the rtl_433_ESP library to a local directory

 git clone https://github.com/NorthernMan54/rtl_433_ESP

2 - Start Visual Studio Code, and 'Open Folder' from the `rtl_433_ESP/example/OOK_Receiver` directory

3 - Select and build either the `esp32_cc1101` or `esp32_heltec` environment

# Transceiver Module Wiring

## CC1101 Transceiver Module

| CC1101 PIN | ESP32 GPIO | Compiler Definition | Notes |
| ---------- | ---------- | ------------------- | ----- |
| N/A | 2 | ONBOARD_LED | My ESP32 board had this prewired |
| GDO0 | 22 | RF_MODULE_GDO0 | |
| GDO2 | 4 | RF_MODULE_GDO2 | |
| CSN | 5 | RF_MODULE_CS | |
| MOSI | 23 | N/A | This is the default SPI MOSI connection |
| MISO | 19 | N/A | This is the default SPI MISO connection |
| SCK | 18 | N/A | This is the default SPI MISO connection |

## SX127X (Heltec) Transceiver Module

For the sample I had purchased a Heltec LoRa 32 433-510 Module, which has the SX127X transceiver module included on the board.

For reference I found that the board had the following connections

| SX127X (Heltec) PIN | ESP32 GPIO | Compiler Definition | Notes |
| ---------- | ---------- | ------------------- | ----- |
| N/A | 25 | ONBOARD_LED | |
| DIO0 | 26 | RF_MODULE_DIO0 | |
| DIO1 | 35 | RF_MODULE_DIO1 | |
| DIO2 | 34 | RF_MODULE_DIO2 | |
| RST | 14 | RF_MODULE_RST | |
| CS | 18 | RF_MODULE_CS | |
| MOSI | 27 | RF_MODULE_MOSI | |
| MISO | 19 | RF_MODULE_MISO | |
| SCK | 5 | RF_MODULE_SCK | |
