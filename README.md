This is an attempt at creating an Arduino library for use on ESP32 boards with a CC1101 transceiver or SX127X Transceivers with the device decoders from the rtl_433 package.  And be available for use with openMQTTGateway as an available module.

Inspiration for this came from the ESPiLight effort.  Kudos to puuu for this, and the awesome package.

This is the [LILYGO® LoRa32 V2.1_1.6.1 433Mhz](https://www.lilygo.cc/products/lora3?variant=42476923879605) board running [OpenMQTTGateway](https://github.com/1technophile/OpenMQTTGateway/blob/master/README.md).
![image](https://user-images.githubusercontent.com/19808920/212491102-3ffd719b-9cb5-4494-b74f-49e7947c6887.png)


The initial port implements only enables a subset of the available modulations and device decoders, and is limited to devices I have access to and can test with.

* One comment about the cc1101 transceiver module, I have found that the receiver is not as sensitive as a rtl_sdr and I get about 1/2 the range.

## Enabled rtl_433 Demodulation modules

```
	OOK_PPM                      :         Pulse Position Modulation
	OOK_PWM                      :         Pulse Width Modulation
	OOK_PULSE_MANCHESTER_ZEROBIT :         Pulse Manchester Zero Bit
```

## Enabled Device Decoders

```
Registering protocol [2] "Acurite 609TXC Temperature and Humidity Sensor"
Registering protocol [3] "Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas"
Registering protocol [4] "Acurite 986 Refrigerator / Freezer Thermometer"
Registering protocol [5] "Acurite 606TX Temperature Sensor"
Registering protocol [6] "Acurite 00275rm,00276rm Temp/Humidity with optional probe"
Registering protocol [7] "Acurite 590TX Temperature with optional Humidity"
Registering protocol [8] "Acurite Grill/Meat Thermometer 01185M"
Registering protocol [9] "Akhan 100F14 remote keyless entry"
Registering protocol [10] "AlectoV1 Weather Sensor (Alecto WS3500 WS4500 Ventus W155/W044 Oregon)"
Registering protocol [11] "Ambient Weather TX-8300 Temperature/Humidity Sensor"
Registering protocol [13] "Auriol AFT 77 B2 temperature sensor"
Registering protocol [14] "Auriol AFW2A1 temperature/humidity sensor"
Registering protocol [15] "Auriol AHFL temperature/humidity sensor"
Registering protocol [16] "Auriol HG02832, HG05124A-DCF, Rubicson 48957 temperature/humidity sensor"
Registering protocol [18] "Blyss DC5-UK-WH"
Registering protocol [20] "Bresser Thermo-/Hygro-Sensor 3CH"
Registering protocol [22] "Burnhard BBQ thermometer"
Registering protocol [23] "Calibeur RF-104 Sensor"
Registering protocol [24] "Cardin S466-TX2"
Registering protocol [25] "Chuango Security Technology"
Registering protocol [26] "Companion WTR001 Temperature Sensor"
Registering protocol [29] "Ecowitt Wireless Outdoor Thermometer WH53/WH0280/WH0281A"
Registering protocol [30] "Eurochron EFTH-800 temperature and humidity sensor"
Registering protocol [34] "Esperanza EWS"
Registering protocol [36] "Fine Offset Electronics, WH2, WH5, Telldus Temperature/Humidity/Rain Sensor"
Registering protocol [37] "Fine Offset Electronics, WH0530 Temperature/Rain Sensor"
Registering protocol [38] "Fine Offset WH1050 Weather Station"
Registering protocol [39] "Fine Offset Electronics WH1080/WH3080 Weather Station"
Registering protocol [41] "FT-004-B Temperature Sensor"
Registering protocol [42] "Generic wireless motion sensor"
Registering protocol [43] "Generic Remote SC226x EV1527"
Registering protocol [44] "Generic temperature sensor 1"
Registering protocol [45] "Govee Water Leak Dectector H5054, Door Contact Sensor B5023"
Registering protocol [46] "Globaltronics QUIGG GT-TMBBQ-05"
Registering protocol [47] "Globaltronics GT-WT-02 Sensor"
Registering protocol [48] "Globaltronics GT-WT-03 Sensor"
Registering protocol [49] "Microchip HCS200 KeeLoq Hopping Encoder based remotes"
Registering protocol [50] "Honeywell ActivLink, Wireless Doorbell"
Registering protocol [51] "HT680 Remote control"
Registering protocol [52] "inFactory, nor-tec, FreeTec NC-3982-913 temperature humidity sensor"
Registering protocol [54] "Interlogix GE UTC Security Devices"
Registering protocol [56] "Kedsum Temperature & Humidity Sensor, Pearl NC-7415"
Registering protocol [57] "Kerui PIR / Contact Sensor"
Registering protocol [58] "LaCrosse TX Temperature / Humidity Sensor"
Registering protocol [59] "LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth, (TFA, ORIA) sensor"
Registering protocol [60] "LaCrosse/ELV/Conrad WS7000/WS2500 weather sensors"
Registering protocol [61] "LaCrosse WS-2310 / WS-3600 Weather Station"
Registering protocol [63] "Markisol, E-Motion, BOFU, Rollerhouse, BF-30x, BF-415 curtain remote"
Registering protocol [64] "Maverick et73"
Registering protocol [66] "Missil ML0757 weather station"
Registering protocol [68] "Nexus, FreeTec NC-7345, NX-3980, Solight TE82S, TFA 30.3209 temperature/humidity sensor"
Registering protocol [70] "Opus/Imagintronix XT300 Soil Moisture"
Registering protocol [71] "Oregon Scientific Weather Sensor"
Registering protocol [72] "Oregon Scientific SL109H Remote Thermal Hygro Sensor"
Registering protocol [74] "Philips outdoor temperature sensor (type AJ3650)"
Registering protocol [75] "Philips outdoor temperature sensor (type AJ7010)"
Registering protocol [76] "Prologue, FreeTec NC-7104, NC-7159-675 temperature sensor"
Registering protocol [77] "Quhwa"
Registering protocol [79] "Rubicson Temperature Sensor"
Registering protocol [80] "Rubicson 48659 Thermometer"
Registering protocol [81] "Conrad S3318P, FreeTec NC-5849-913 temperature humidity sensor"
Registering protocol [82] "Silvercrest Remote Control"
Registering protocol [83] "Skylink HA-434TL motion sensor"
Registering protocol [84] "Wireless Smoke and Heat Detector GS 558"
Registering protocol [85] "Solight TE44/TE66, EMOS E0107T, NX-6876-917"
Registering protocol [86] "Springfield Temperature and Soil Moisture"
Registering protocol [87] "TFA Dostmann 30.3221.02 T/H Outdoor Sensor"
Registering protocol [88] "TFA Drop Rain Gauge 30.3233.01"
Registering protocol [89] "TFA pool temperature sensor"
Registering protocol [90] "TFA-Twin-Plus-30.3049, Conrad KW9010, Ea2 BL999"
Registering protocol [91] "Thermopro TP11 Thermometer"
Registering protocol [92] "Thermopro TP08/TP12/TP20 thermometer"
Registering protocol [94] "TS-FT002 Wireless Ultrasonic Tank Liquid Level Meter With Temperature Sensor"
Registering protocol [95] "Visonic powercode"
Registering protocol [96] "Waveman Switch Transmitter"
Registering protocol [97] "WG-PB12V1 Temperature Sensor"
Registering protocol [98] "WS2032 weather station"
Registering protocol [99] "Hyundai WS SENZOR Remote Temperature Sensor"
Registering protocol [100] "WT0124 Pool Thermometer"
Registering protocol [101] "X10 RF"
Registering protocol [102] "X10 Security"
```

The gaps in the numbers are device decoders disabled by default.

## Transceiver Modules Supported

### CC1101 433 Mhz Transceiver Module

Datasheet for the CC1101 board I'm using

[CC1101 datasheet](docs/E07-M1101D-TH_Usermanual_EN_v1.30.pdf)

### SX127X - Heltec WiFi LoRa 32 and LILYGO® LoRa32

This is the [Heltec SX127X](https://heltec.org/project/wifi-lora-32/) board I used for development. I used the 433 Mhz Board

This is the [LILYGO® LoRa32 V2.1_1.6.1 433](https://www.lilygo.cc/products/lora3?variant=42476923879605) board I used for development. I used the 433 Mhz Board

### SX1278 - external to ESP32 DOIT DevKit V1

This is the [AI-Thinker R01 - SX1278](https://docs.ai-thinker.com/en/lora/man) module I used for development. I used the Ra-01 433 Mhz Board

This is the [ESP32 DOIT DevKit V1](https://docs.platformio.org/en/latest/boards/espressif32/esp32doit-devkit-v1.html) board I used for development.

Wiring for ESP32 DOIT DevKit V1 and AI-Thinker R01 - SX1278

![image](https://github.com/diepeterpan/rtl_433_ESP/blob/master/docs/Ai-Thinker-Ra-01-Schematic-Diagram.png)

## Wiring and Building the Example

Details are [here](example/OOK_Receiver/README.md)

## Projects using the library

* OpenMQTTGateway - https://github.com/1technophile/OpenMQTTGateway
  - Currently available in the production branch

## Roadmap / Next Steps

* [x] Firmware size tuning
* [x] Further heap usage tuning
* [x] Find stack leak when running all devices
* [ ] Enable additional pulse demod functions
* [x] Simplify the process to update from the rtl_433 source

# Signal detection and reception approach

To determne that a signal is available for reception, the library watches the current RSSI reported by the transceiver module and when it crosses a predetermined RSSI threshold it enables the signal receiver function.   End of signal is determined when the signal drops below the predetermined RSSI threshold for a minimum of 150,000 micro seconds.

## RSSI Threshold Automatic Setting

The RSSI Threshold for signal detection is automatically determined based on the average RSSI signal level received aka RSSI floor level with a delta ( RSSI_THRESHOLD ) added to it.  The average RSSI signal level is calculated over RSSI_SAMPLES.

## SX127X OOK RSSI FIXED Threshold

For background see section 2.1.3.2. of SX127X Data sheet

To tune the SX127X OOK RSSI FIXED Threshold two values are used to determine if the threhold needs to be increased or decreased.  The first is the noise recevied between signals.  If the number of noise bits received between signals is greater than 100, then the threshold is incremented.  Second is the unparsed signals.  If an unparsed signal is received, but it has less than 20 pulses, the threhold is decremented.

The first approach is what is recommended in the SX127X datasheet, and the second is a control to lower the threshold if it is too high and incomplete signals are received.

# Compile definition options

```
DEMOD_DEBUG					  ; enable verbose debugging of signal processing
DEVICE_DEBUG				  ; Validate fields are mapped to response object ( rtl_433 )
MEMORY_DEBUG				  ; display heap usage information
MY_DEVICES					  ; Only include my personal subset of devices
NO_DEAF_WORKAROUND    ; Workaround for issue #16 ( by default the workaround is enabaled )
PUBLISH_UNPARSED		  ; Enable publishing of MQTT messages for unparsed signals, e.g. {model":"unknown","protocol":"signal parsing failed"…
RAW_SIGNAL_DEBUG		  ; display raw received messages
RSSI_SAMPLES          ; Number of rssi samples to collect for average calculation, defaults to 50,000
RSSI_THRESHOLD        ; Delta applied to average RSSI value to calculate RSSI Signal Threshold, defaults to 9
RTL_DEBUG					    ; Enable RTL_433 device decoder verbose mode for all device decoders ( 0=normal, 1=verbose, 2=verbose decoders, 3=debug decoders, 4=trace decoding. )
RTL_VERBOSE=##        ; Enable RTL_433 device decoder verbose mode, ## is the decoder # from the appropriate memcpy line in rtl_433_ESP.cpp
SIGNAL_RSSI					  ; Enable collection of per pulse RSSI Values during signal reception for display in signal debug messages
RF_MODULE_INIT_STATUS ; Display transceiver config during startup
DISABLERSSITHRESHOLD  ; Disable automatic setting of RSSI_THRESHOLD ( legacy behaviour ), and use MINRSSI ( -82 )
```

## RF Module Wiring

ONBOARD_LED           ; GPIO pin to toggle during signal reception ( Typically onboard LED )

### SX1276 Module Options

RF_SX1276             ; Enable support for SX1276 Transceiver
OOK_FIXED_THRESHOLD   ; Inital OOK threshold ( See 2.1.3.2. of datasheet ), defaults to 90
AUTOOOKFIX            ; Set to enable automatic setting of OOK_FIXED_THRESHOLD based on noise level between signals

### SX1276 Module Wiring ( Required if not using standard configuraton )

RF_MODULE_CS          ; SX1276 SPI Chip select
RF_MODULE_DIO0        ; SX1276 DIO0 PIN
RF_MODULE_RST         ; SX1276 RST PIN
RF_MODULE_DIO1        ; SX1276 DIO1 PIN

### SX1278 Module Options

RF_SX1278 - Enable support for SX1276
OOK_FIXED_THRESHOLD   ; Inital OOK threshold ( See 2.1.3.2. of datasheet ), defaults to 90
AUTOOOKFIX            ; Set to enable automatic setting of OOK_FIXED_THRESHOLD based on noise level between signals

### SX1278 Module Wiring ( Required if not using standard configuraton )

RF_MODULE_CS          ; SX1278 SPI Chip select
RF_MODULE_DIO0        ; SX1278 DIO0 PIN
RF_MODULE_RST         ; SX1278 RST PIN
RF_MODULE_DIO1        ; SX1278 DIO1 PIN

### CC1101 Module Options

RF_CC1101             ; Enable support for CC1101 Transceiver
NO_DEAF_WORKAROUND    ; Workaround for issue #16 ( by default the workaround is enabaled )

### CC1101 Module Wiring

RF_MODULE_CS          ; CC1101 SPI Chip select
RF_MODULE_GDO0        ; CC1101 GDOO PIN
RF_MODULE_GDO2        ; CC1101 GDO2 PIN

## RF Module SPI Wiring ( Required if not using standard configuraton )

When using a non standard SPI configuration ( Standard config is SCK - 18, MISO - 19, MOSI - 23, CS - 5)

RF_MODULE_SCK         ; SPI Clock
RF_MODULE_MISO        ; SPI Serial Output
RF_MODULE_MOSI        ; SPI Serial Input
RF_MODULE_CS          ; SPI Chip select

## Porting approach

* Copy subset of files in src/rtl_433/devices without any modification
  - Generated a list of files that used OOK_PPM and OOK_PWM with tools/device.awk
  - Copied into src/rtl_433/devices
  - Manually added oregon_scientific.c to devices
  - Marked newkaku.c, nexa.c, and proove.c as unavailable as the device decoder triggers a stack overflow
  - Based on the reduced list, updated include/rtl_433_devices.h, src/rtl_433_ESP.cpp, and src/rtl_433_ESP.h

* Copy of only used files in src/rtl_433, with minimal modifications
- abuf.c 			- No changes made
- bitbuffer.c		- No changes made
- data.c			- Defined out unneeded functions ( #ifndef ESP32 )
- decoder_util.c 	- No changes made
- list.c			- No changes made
- pulse_demod.c		- Move 'bitbuffer_t bits' to class level
- r_api.c			- Significant tuning and tweaking applied
- r_util.c			- No changes made
- util.c			- No changes made

* include directory is a copy
- abuf.h			- No changes made
- am_analyze.h		- No changes made
- baseband.h		- No changes made
- bitbuffer.h		- No changes made
- compat_time.h		- No changes made
- data.h			- Added '#define _POSIX_HOST_NAME_MAX  128'
- decoder.h			- No changes made
- decoder_util.h	- No changes made
- fatal.h			- No changes made
- fileformat.h		- No changes made
- list.h			- No changes made
- log.h				- Not from rtl_433.h
- optparse.h		- No changes made
- pulse_demod.h		- No changes made
- pulse_detect.h	- Adjusted structures to reduce size
- r_api.h			- No changes made
- r_device.h		- Adjusted structures to reduce size
- r_private.h		- Adjusted structures to reduce size
- r_util.h			- No changes made
- rtl_433.h			- Adjusted structures to reduce size
- rtl_433_devices.h	- Only added my skylink device
- rtl_devices.h		- Not needed ??
- samp_grab.h		- No changes made
- term_ctl.h		- No changes made
- util.h			- No changes made

## Codebase conflicts

* ESPiLight and rtl_433 conflict on silvercrest

```
.platformio/packages/toolchain-xtensa32/bin/../lib/gcc/xtensa-esp32-elf/5.2.0/../../../../xtensa-esp32-elf/bin/ld: Warning: size of symbol `silvercrest' changed from 4 in .pio/build/rtl_433-9e0770/libb28/libESPiLight.a(protocol.c.o) to 76 in .pio/build/rtl_433-9e0770/lib395/librtl_433_ESP.a(silvercrest.c.o)
```

## Memory footprint

### OpenMQTTGateway

Build definitions

```
  '-Dota_password=""'
  '-DMQTT_SERVER="192.168.1.12"'
;  '-DMDNS_SD=true'
  '-DESPWifiManualSetup=true'
  '-DGateway_Name="cc1101-ec20dc"'    ; 9e0770  ; ec20dc
  '-DOMG_VERSION="$PIOENV"'
  '-DSERIAL_BAUD=921600'
 ; '-DZgatewayRF="RF"'
  '-DZgatewayRTL_433="rtl_433"'
 ; '-DZgatewayPilight="Pilight"'
  '-DZradioCC1101="CC1101"'
;  '-DZsensorBH1750="BH1750"'
;  '-DZsensorBME280="BME280"'
;  '-DLOG_LEVEL=LOG_LEVEL_TRACE'
;  '-DMEMORY_DEBUG=true'
;  '-DDEMOD_DEBUG=true'
; '-DRTL_DEBUG=4'           ; rtl_433 verbose mode
;  '-DRAW_SIGNAL_DEBUG=true'
;  '-DSUPPRESS_UNPARSED=true'
  '-DRF_EMITTER_GPIO=12'
  '-DRF_RECEIVER_GPIO=27'
;  '-DMY_DEVICES=true'
  '-UZmqttDiscovery'
```

* Version 0.0.1

```
Advanced Memory Usage is available via "PlatformIO Home > Project Inspect"
RAM:   [==        ]  23.1% (used 75776 bytes from 327680 bytes)
Flash: [========  ]  79.1% (used 1036818 bytes from 1310720 bytes)
```

* Version 2

Cleaned up device decoder list, ( OOK PPM and PWM only)

```
Advanced Memory Usage is available via "PlatformIO Home > Project Inspect"
RAM:   [==        ]  20.6% (used 67344 bytes from 327680 bytes)
Flash: [=======   ]  70.4% (used 922990 bytes from 1310720 bytes)
```

## Supporting Additional Devices etc

As support for devices is limited to what we have personally available, donations for additional devices is encouraged.

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=6V3MNYYR446EG)

## Thanks

* Tks to the rtl_433 team for a great codebase
* @diepeterpan for testing and showing that the OOK_PULSE_MANCHESTER_ZEROBIT decoder works, and adding support for Oregon-CM160 ( oregon_scientific.c)

## Media Mentions

January 2023
* [Hackaday - ARDUINO LIBRARY BRINGS RTL_433 TO THE ESP32](https://hackaday.com/2023/01/13/arduino-library-brings-rtl_433-to-the-esp32)
* [CNX Software - 433 MHz is not dead! Using an ESP32 board with LoRa module to talk to 433 MHz sensors](https://www.cnx-software.com/2023/01/14/esp32-board-with-lora-433-mhz-sensors/)
* [RTL_433 PORTED TO ESP32 MICROCONTROLLERS WITH CC1101 OR SX127X TRANSCEIVER CHIPS](https://www.rtl-sdr.com/rtl_433-ported-to-esp32-microcontrollers-with-cc1101-or-sx127x-transceiver-chips/)
* [RTL 433 ON ESP32 DEVICE - MQTT HOME ASSISTANT](https://youtube.com/watch?v=H-JXWbWjJYE&feature=shares)
