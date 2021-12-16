This is an attempt at creating an Arduino library for use on ESP32 boards with a CC1101 transceiver with the device decoders from the rtl_433 package.  And be available for use with openMQTTGateway as an available module.

Inspiration for this came from the ESPiLight effort.  Kudos to puuu for this, and the awesome package.

The initial port implements only enables a subset of the available modulations and device decoders, and is limited to devices I have access to and can test with.

* One comment about the cc1101 transceiver module, I have found that the receiver is not as sensitive as a rtl_sdr and I get about 1/2 the range. 

## Enabled Pulse Demodulation modules

```
	OOK_PPM :         Pulse Position Modulation
	OOK_PWM :         Pulse Width Modulation
```

### Enabled Device Decoders

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
Registering protocol [17] "BlueLine Innovations Power Cost Monitor"
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
Registering protocol [59] "LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor"
Registering protocol [60] "LaCrosse/ELV/Conrad WS7000/WS2500 weather sensors"
Registering protocol [61] "LaCrosse WS-2310 / WS-3600 Weather Station"
Registering protocol [63] "Markisol, E-Motion, BOFU, Rollerhouse, BF-30x, BF-415 curtain remote"
Registering protocol [64] "Maverick et73"
Registering protocol [66] "Missil ML0757 weather station"
Registering protocol [68] "Nexus, FreeTec NC-7345, NX-3980, Solight TE82S, TFA 30.3209 temperature/humidity sensor"
Registering protocol [70] "Opus/Imagintronix XT300 Soil Moisture"
Registering protocol [71] "Oregon Scientific SL109H Remote Thermal Hygro Sensor"
Registering protocol [73] "Philips outdoor temperature sensor (type AJ3650)"
Registering protocol [74] "Philips outdoor temperature sensor (type AJ7010)"
Registering protocol [75] "Prologue, FreeTec NC-7104, NC-7159-675 temperature sensor"
Registering protocol [76] "Quhwa"
Registering protocol [78] "Rubicson Temperature Sensor"
Registering protocol [79] "Rubicson 48659 Thermometer"
Registering protocol [80] "Conrad S3318P, FreeTec NC-5849-913 temperature humidity sensor"
Registering protocol [81] "Silvercrest Remote Control"
Registering protocol [82] "Skylink HA-434TL motion sensor"
Registering protocol [83] "Wireless Smoke and Heat Detector GS 558"
Registering protocol [84] "Solight TE44/TE66, EMOS E0107T, NX-6876-917"
Registering protocol [85] "Springfield Temperature and Soil Moisture"
Registering protocol [86] "TFA Dostmann 30.3221.02 T/H Outdoor Sensor"
Registering protocol [87] "TFA Drop Rain Gauge 30.3233.01"
Registering protocol [88] "TFA pool temperature sensor"
Registering protocol [89] "TFA-Twin-Plus-30.3049, Conrad KW9010, Ea2 BL999"
Registering protocol [90] "Thermopro TP11 Thermometer"
Registering protocol [91] "Thermopro TP08/TP12/TP20 thermometer"
Registering protocol [93] "TS-FT002 Wireless Ultrasonic Tank Liquid Level Meter With Temperature Sensor"
Registering protocol [94] "Visonic powercode"
Registering protocol [95] "Waveman Switch Transmitter"
Registering protocol [96] "WG-PB12V1 Temperature Sensor"
Registering protocol [97] "WS2032 weather station"
Registering protocol [98] "Hyundai WS SENZOR Remote Temperature Sensor"
Registering protocol [99] "WT0124 Pool Thermometer"
Registering protocol [100] "X10 RF"
Registering protocol [101] "X10 Security"
```

The gaps in the numbers are device decoders disabled by default.

## Wiring and Building the Example

To wire my device, I followed this guide

https://github.com/LSatan/SmartRC-CC1101-Driver-Lib/blob/master/img/Esp32_CC1101.png

But we are recommending CC1101 GDO2 to be connected to ESP32 D27 and GDO0 to be connected to D12, this is different than the LSatan diagram. This is due to the ESP32 using D2 as part of the boot process. 

And to build the example I use platformio and opened the example folder as a new window and built from their.

## Datasheet for the board I'm using

[datasheet](docs/E07-M1101D-TH_Usermanual_EN_v1.30.pdf)

## Projects using the library

* OpenMQTTGateway - https://github.com/1technophile/OpenMQTTGateway
  - Currently available in the development branch

## Roadmap / Next Steps

* [ ] Firmware size tuning
* [ ] Further heap usage tuning
* [x] Find stack leak when running all devices
* [ ] Enable addition pulse demod functions
* [x] Simplify the process to update from the rtl_433 source

## Compile definition options

```
MINRSSI						; Default rssi to enable receiver, defaults to -82
MY_DEVICES					; Only include my personal subset of devices

DEMOD_DEBUG					; enable verbose debugging of signal processing
DEVICE_DEBUG				; Validate fields are mapped to response object ( rtl_433 )
MEMORY_DEBUG				; display heap usage information
RAW_SIGNAL_DEBUG			; display raw received messages
RSSI						; Enable collection of per pulse RSSI Values during signal reception
RTL_DEBUG					; Enable RTL_433 Verbose option ( 0=normal, 1=verbose, 2=verbose decoders, 3=debug decoders, 4=trace decoding. )
SUPPRESS_UNPARSED			; Enable suppression of publishing MQTT messages for unparsed signals, e.g. {model":"unknown","protocol":"signal parsing failed"…
NO_DEAF_WORKAROUND    ; Workaround for issue #16 ( by default the workaround is enabaled )
```

## Porting approach

* Copy subset of files in src/rtl_433/devices without any modification
  - Generated a list of files that used OOK_PPM and OOK_PWM with tools/device.awk
  - Copied into src/rtl_433/devices
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
