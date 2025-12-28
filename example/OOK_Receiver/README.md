This is a sample implementation of the rtl_433_ESP library with the CC1101 or the SX127X (Heltec) Transceiver Module.

# Building and implementing the sample

To build and deploy the sample I used Visual Studio Code and PlatformIO.  This development environment will pull in all the required libraries as part of the build.

1 - Clone the rtl_433_ESP library to a local directory

 git clone https://github.com/NorthernMan54/rtl_433_ESP

2 - Start Visual Studio Code, and 'Open Folder' from the `rtl_433_ESP/example/OOK_Receiver` directory

3 - Select and build either the `esp32_cc1101`, `esp32_heltec` or `esp32doitv1_r01` environment

# Configuring Frequency

The library defaults to 433.92 MHz, but supports multiple frequencies including 868 MHz and 915 MHz bands. To configure a different frequency, modify the `build_flags` in your environment within `platformio.ini`:

```ini
build_flags = 
  '-DRF_MODULE_FREQUENCY=868.30'  ; Configure for 868.30 MHz
```

**Common Frequencies:**
* `433.92` - Default, 433 MHz ISM band (Asia and other regions)
* `868.30` - European ISM band (many EU sensors operate at this frequency)
* `915.00` - ISM band (North America, Australia, and other regions)

**Important:** Ensure your hardware module supports the frequency you want to use. Many LoRa/transceiver modules are sold in specific frequency variants (433 MHz, 868 MHz, or 915 MHz). Using a frequency outside your hardware's range will not work.

See the `esp32_heltec_915` environment in `platformio.ini` for a complete 915 MHz configuration example.

# Transceiver Module Wiring

## CC1101 Transceiver Module

| CC1101 PIN | ESP32 GPIO | Compiler Definition | Notes |
| ---------- | ---------- | ------------------- | ----- |
| N/A | 2 | ONBOARD_LED | My ESP32 board had this prewired |
| GDO0 | 22 | RF_MODULE_GDO0 | |
| GDO2 | 4 | RF_MODULE_GDO2 | |
| CSN | 5 | RF_MODULE_CS | This is the default SPI CS connection |
| MOSI | 23 | N/A | This is the default SPI MOSI connection |
| MISO | 19 | N/A | This is the default SPI MISO connection |
| SCK | 18 | N/A | This is the default SPI SCK connection |

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

## SX127X (AI Thinker Ra01) Transceiver Module

For the sample I had purchased a AI Thinker Ra01 Module, which has the SX1278 transceiver module which I connected to a ESP32 DOIT DevKit V1.

For reference I used the following connections

| SX127X (Ra01) PIN | ESP32 GPIO | Compiler Definition | Notes |
| ---------- | ---------- | ------------------- | ----- |
| N/A | 2 | ONBOARD_LED | |
| DIO0 | 15 | RF_MODULE_DIO0 | |
| DIO1 | 4 | RF_MODULE_DIO1 | |
| DIO2 | 16 | RF_MODULE_DIO2 | |
| RST | 17 | RF_MODULE_RST | |
| CS | 5 | RF_MODULE_CS | |
| MOSI | 23 | RF_MODULE_MOSI | |
| MISO | 19 | RF_MODULE_MISO | |
| SCK | 18 | RF_MODULE_SCK | |

## SX127X (LilyGo) Transceiver Module

For the sample I had purchased a TTGO LoRa32-OLED v2.1.6 433Mhz Module, which has the SX127X transceiver module included on the board.

# Sample Outputs

## CC1101 Device

```
�N:  
N: ****** setup ******
rtl_433_ESP(6): CC1101 gpio receive pin: 22
rtl_433_ESP(6): CC1101 receive frequency: 433.920013
rtl_433_ESP(6): # of device(s) configured 101
rtl_433_ESP(6): ssizeof(r_device): 108
rtl_433_ESP(6): cfg->devices size: 10908
rtl_433_ESP(6): # of device(s) enabled 100
M       CC1101
rtl_433_ESP(6): CC1101 radio.begin() success!
rtl_433_ESP(6): CC1101 setOOK - success!
rtl_433_ESP(6): CC1101 setCrcFiltering - success!
rtl_433_ESP(6): CC1101 receiveDirect - success!
----- CC1101 Status -----
CC1101_MDMCFG1: 0x02
CC1101_MDMCFG2: 0x32
CC1101_MDMCFG3: 0x83
CC1101_MDMCFG4: 0xa7
-------------------------
CC1101_DEVIATN: 0x14
CC1101_AGCCTRL0: 0x91
CC1101_AGCCTRL1: 0x40
CC1101_AGCCTRL2: 0x03
-------------------------
CC1101_IOCFG0: 0x0d
CC1101_IOCFG1: 0x2e
CC1101_IOCFG2: 0x29
-------------------------
CC1101_FIFOTHR: 0x07
CC1101_SYNC0: 0xad
CC1101_SYNC1: 0x12
-------------------------
CC1101_PKTLEN: 0xff
CC1101_PKTCTRL0: 0x32
CC1101_PKTCTRL1: 0x04
-------------------------
CC1101_ADDR: 0x00
CC1101_CHANNR: 0x00
CC1101_FSCTRL0: 0x00
CC1101_FSCTRL1: 0x0f
-------------------------
CC1101_FREQ0: 0x71
CC1101_FREQ1: 0xb0
CC1101_FREQ2: 0x10
-------------------------
CC1101_MCSM0: 0x14
CC1101_MCSM1: 0x30
CC1101_MCSM2: 0x07
-------------------------
CC1101_FOCCFG: 0x76
CC1101_BSCFG: 0x6c
CC1101_WOREVT0: 0x6b
CC1101_WOREVT1: 0x87
CC1101_WORCTRL: 0xf8
CC1101_FREND0: 0x11
CC1101_FREND1: 0x56
-------------------------
CC1101_FSCAL0: 0x0d
CC1101_FSCAL1: 0x3b
CC1101_FSCAL2: 0x0c
CC1101_FSCAL3: 0xaa
-------------------------
CC1101_RCCTRL0: 0x00
CC1101_RCCTRL1: 0x41
-------------------------
CC1101_PARTNUM: 0x00
CC1101_VERSION: 0x14
CC1101_MARCSTATE: 0x0d
CC1101_PKTSTATUS: 0xe0
CC1101_RXBYTES: 0x00
----- CC1101 Status -----
N: ****** setup complete ******
rtl_433_ESP(6): Signal length: 220996, Gap length: 6248856, Signal RSSI: -57, train: 0, messageCount: 0, pulses: 169
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-57,"duration":220996}
N: Received message : {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-57,"duration":220996}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-57,"duration":220996}
N: Received message : {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-57,"duration":220996}
rtl_433_ESP(6): # of messages decoded 2
 
rtl_433_ESP(6): Ignored Signal length: 161039, Gap length: 2207276, Signal RSSI: -79, Current RSSI: -87, pulses: 6
rtl_433_ESP(6): Signal length: 439000, Gap length: 4780726, Signal RSSI: -29, train: 1, messageCount: 1, pulses: 222
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":24.7,"humidity":84,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-29,"duration":439000}
N: Received message : {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":24.7,"humidity":84,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-29,"duration":439000}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":24.7,"humidity":84,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-29,"duration":439000}
N: Received message : {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":24.7,"humidity":84,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-29,"duration":439000}
rtl_433_ESP(6): # of messages decoded 2
 
rtl_433_ESP(6): Signal length: 215000, Gap length: 8389809, Signal RSSI: -55, train: 0, messageCount: 2, pulses: 162
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-55,"duration":215000}
N: Received message : {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-55,"duration":215000}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-55,"duration":215000}
N: Received message : {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-55,"duration":215000}
rtl_433_ESP(6): # of messages decoded 2
```

## Heltec SX127X Transceiver

```
���������������������������������������������������������������������������������������������������������������������N:  
N: ****** setup ******
rtl_433_ESP(6): SX1278 gpio receive pin: 34
rtl_433_ESP(6): SX1278 receive frequency: 433.920013
rtl_433_ESP(6): # of device(s) configured 101
rtl_433_ESP(6): ssizeof(r_device): 108
rtl_433_ESP(6): cfg->devices size: 10908
rtl_433_ESP(6): # of device(s) enabled 100
rtl_433_ESP(7): Enabling rtl_433_DecoderTask
rtl_433_ESP(6): SX1278 SPI Config SCK: 5, MISO: 19, MOSI: 27, CS: 18
M       SX127x
rtl_433_ESP(6): SX1278 radio.begin() success!
rtl_433_ESP(6): SX1278 setOOK - success!
rtl_433_ESP(6): SX1278 setDataShapingOOK - success!
rtl_433_ESP(6): SX1278 setOokThresholdType - success!
rtl_433_ESP(6): SX1278 setOokPeakThresholdDecrement - success!
rtl_433_ESP(6): SX1278 setOokFixedOrFloorThreshold - success!
rtl_433_ESP(6): SX1278 setRSSIConfig - success!
rtl_433_ESP(6): SX1278 PREAMBLE_DETECT - success!
rtl_433_ESP(6): SX1278 setBitRate - success!
rtl_433_ESP(6): SX1278 setRxBandwidth - success!
rtl_433_ESP(6): SX1278 setCrcFiltering - success!
rtl_433_ESP(6): SX1278 setDirectSyncWord - success!
rtl_433_ESP(6): SX1278 disableBitSync - success!
rtl_433_ESP(6): SX1278 receiveDirect - success!
----- SX127x Status -----
RegOpMode: 0x2c
RegPacketConfig1: 0x00
RegPacketConfig2: 0x00
RegBitrateMsb: 0x03
RegBitrateLsb: 0xd0
RegRxBw: 0x01
RegAfcBw: 0x02
-------------------------
RegOokPeak: 0x08
RegOokFix: 0x50
RegOokAvg: 0x72
-------------------------
RegLna: 0x20
RegRxConfig: 0x08
RegRssiConfig: 0x07
-------------------------
RegDioMapping1: 0x00
----- SX127x Status -----
rtl_433_ESP(3): SX1278 Enabling rtl_433_ReceiverTask
N: ****** setup complete ******
rtl_433_ESP(6): Signal length: 382996, Gap length: 1619894, Signal RSSI: -51, train: 0, messageCount: 0, pulses: 294
rtl_433_ESP(6): data_output {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"No","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-51,"duration":382996}
N: Received message : {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"No","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-51,"duration":382996}
rtl_433_ESP(6): data_output {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"No","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-51,"duration":382996}
N: Received message : {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"No","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-51,"duration":382996}
rtl_433_ESP(6): # of messages decoded 2
 
rtl_433_ESP(7): RegOokFix Threshold Adjust noise count 176, RegOokFix 0x51
rtl_433_ESP(6): Signal length: 513996, Gap length: 11653283, Signal RSSI: -49, train: 1, messageCount: 1, pulses: 477
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":24.6,"humidity":84,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-49,"duration":513996}
N: Received message : {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":24.6,"humidity":84,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-49,"duration":513996}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":24.6,"humidity":84,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-49,"duration":513996}
N: Received message : {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":24.6,"humidity":84,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-49,"duration":513996}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":24.6,"humidity":84,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-49,"duration":513996}
N: Received message : {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":24.6,"humidity":84,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-49,"duration":513996}
rtl_433_ESP(6): data_output {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"Yes","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-49,"duration":513996}
N: Received message : {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"Yes","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-49,"duration":513996}
rtl_433_ESP(6): data_output {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"Yes","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-49,"duration":513996}
N: Received message : {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"Yes","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-49,"duration":513996}
rtl_433_ESP(6): # of messages decoded 5
 
rtl_433_ESP(6): Signal length: 121996, Gap length: 1093746, Signal RSSI: -72, train: 0, messageCount: 2, pulses: 182
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-72,"duration":121996}
N: Received message : {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-72,"duration":121996}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-72,"duration":121996}
N: Received message : {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-72,"duration":121996}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-72,"duration":121996}
N: Received message : {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-72,"duration":121996}
rtl_433_ESP(6): # of messages decoded 3
 
rtl_433_ESP(6): Signal length: 670996, Gap length: 10037719, Signal RSSI: -55, train: 1, messageCount: 3, pulses: 813
rtl_433_ESP(6): data_output {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"Yes","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-55,"duration":670996}
N: Received message : {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"Yes","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-55,"duration":670996}
rtl_433_ESP(6): data_output {"model":"LaCrosse-TX141W","id":168901,"channel":0,"battery_ok":1,"wind_avg_km_h":2.7,"wind_dir_deg":0,"test":0,"mic":"CRC","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-55,"duration":670996}
N: Received message : {"model":"LaCrosse-TX141W","id":168901,"channel":0,"battery_ok":1,"wind_avg_km_h":2.7,"wind_dir_deg":0,"test":0,"mic":"CRC","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-55,"duration":670996}
rtl_433_ESP(6): # of messages decoded 2
```

## LilyGo SX127X Module

```
N: ****** setup ******
rtl_433_ESP(6): SX1278 gpio receive pin: 32
rtl_433_ESP(6): SX1278 receive frequency: 433.920013
rtl_433_ESP(6): # of device(s) configured 101
rtl_433_ESP(6): ssizeof(r_device): 108
rtl_433_ESP(6): cfg->devices size: 10908
rtl_433_ESP(6): # of device(s) enabled 100
M       SX127x
----- SX127x Status -----
RegOpMode: 0x24
RegPacketConfig1: 0x00
RegPacketConfig2: 0x00
RegBitrateMsb: 0x03
RegBitrateLsb: 0xd0
RegRxBw: 0x01
RegAfcBw: 0x02
-------------------------
RegOokPeak: 0x08
RegOokFix: 0x50
RegOokAvg: 0x72
-------------------------
RegLna: 0x23
RegRxConfig: 0x08
RegRssiConfig: 0x07
-------------------------
RegDioMapping1: 0x00
----- SX127x Status -----
N: ****** setup complete ******
rtl_433_ESP(6): Signal length: 180996, Gap length: 2825872, Signal RSSI: -66, train: 0, messageCount: 0, pulses: 182
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":1.3,"humidity":94,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-66,"duration":180996}
N: Received message : {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":1.3,"humidity":94,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-66,"duration":180996}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":1.3,"humidity":94,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-66,"duration":180996}
N: Received message : {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":1.3,"humidity":94,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-66,"duration":180996}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":1.3,"humidity":94,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-66,"duration":180996}
N: Received message : {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":1.3,"humidity":94,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-66,"duration":180996}
rtl_433_ESP(6): # of messages decoded 3
rtl_433_ESP(6): Signal length: 182000, Gap length: 1508260, Signal RSSI: -63, train: 1, messageCount: 1, pulses: 182
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":0.9,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-63,"duration":182000}
N: Received message : {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":0.9,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-63,"duration":182000}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":0.9,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-63,"duration":182000}
N: Received message : {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":0.9,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-63,"duration":182000}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":0.9,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-63,"duration":182000}
N: Received message : {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":0.9,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-63,"duration":182000}
rtl_433_ESP(6): # of messages decoded 3
```

## AI Thinker Ra01 SX127X Transceiver using ESP32 DoIT DevKit V1

```
N:
N: ****** setup ******
rtl_433_ESP(6): SX1278 gpio receive pin: 16
rtl_433_ESP(6): SX1278 receive frequency: 433.920013
rtl_433_ESP(6): # of device(s) configured 102
rtl_433_ESP(6): ssizeof(r_device): 108
rtl_433_ESP(6): cfg->devices size: 11016
rtl_433_ESP(6): # of device(s) enabled 101
M       SX127x
----- SX127x Status -----
RegOpMode: 0x2c
RegPacketConfig1: 0x00
RegPacketConfig2: 0x00
RegBitrateMsb: 0x03
RegBitrateLsb: 0xd0
RegRxBw: 0x01
RegAfcBw: 0x02
-------------------------
RegOokPeak: 0x08
RegOokFix: 0x50
RegOokAvg: 0x72
-------------------------
RegLna: 0x20
RegRxConfig: 0x08
RegRssiConfig: 0x07
-------------------------
RegDioMapping1: 0x00
----- SX127x Status -----
N: ****** setup complete ******
N: Received message : {"model":"Oregon-CM160","id":7,"power_W":499,"protocol":"Oregon Scientific Weather Sensor","rssi":-65,"duration":129000}
N: Received message : {"model":"Oregon-CM160","id":7,"power_W":483,"protocol":"Oregon Scientific Weather Sensor","rssi":-65,"duration":130000}
N: Received message : {"model":"Oregon-CM160","id":7,"power_W":483,"protocol":"Oregon Scientific Weather Sensor","rssi":-69,"duration":130000}
N: Received message : {"model":"Oregon-CM160","id":7,"power_W":418,"protocol":"Oregon Scientific Weather Sensor","rssi":-61,"duration":161002}
N: Received message : {"model":"Oregon-CM160","id":7,"power_W":418,"protocol":"Oregon Scientific Weather Sensor","rssi":-63,"duration":129000}
N: Received message : {"model":"Oregon-CM160","id":7,"power_W":418,"protocol":"Oregon Scientific Weather Sensor","rssi":-61,"duration":129000}
N: Received message : {"model":"Oregon-CM160","id":7,"power_W":418,"protocol":"Oregon Scientific Weather Sensor","rssi":-61,"duration":130000}
N: Received message : {"model":"Oregon-CM180i","id":10096,"battery_ok":1,"power1_W":16,"power2_W":37448,"power3_W":209,"energy_kWh":330890.03125,"sequence":0,"protocol":"Oregon Scientific Weather Sensor","rssi":-62,"duration":129000}
```
