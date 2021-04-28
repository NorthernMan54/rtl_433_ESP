This is an attempt at creating an Arduino library for use on ESP32 boards with a CC1101 transceiver with the device decoders from the rtl_433 package.  And be available for use with openMQTTGateway as an available module.

Inspiration for this came from the ESPiLight effort.  Kudos to @puuu for this, and the awesome package.

The initial port implements only enables a subset of the available modulations and device decoders, and is limited to devices I have access to and can test with.

## Enabled Pulse Demodulation modules

```
	OOK_PPM :         Pulse Position Modulation ( ASK )
	OOK_PWM :         Pulse Width Modulation ( ASK )
  FSK_PULSE_PCM:    Frequency Shift Keying ( 2-FSK )
```

## Receiving different signal modulations

Unlike the rtl-sdr device which passes the raw RF signal to rtl_433 for processing and demodulation, the CC1101 Transceiver performs signal demodulation on the device itself, so recepition of different modulations is limited to the currently selected modulation scheme.  Simultaneous reception of signals encoded with different modulation schemes is not feasible with a CC1101 Transceiver.

### Enabled Device Decoders

### ASK/OOK devices supported

```
  "Acurite 896 Rain Gauge"
  "Acurite 609TXC Temperature and Humidity Sensor"
  "Acurite 592TXR Temp/Humidity
  "Acurite 986 Refrigerator / Freezer Thermometer"
  "Acurite 606TX Temperature Sensor"
  "Acurite 00275rm
  "Acurite 590TX Temperature with optional Humidity"
  "Akhan 100F14 remote keyless entry"
  "AlectoV1 Weather Sensor (Alecto WS3500 WS4500 Ventus W155/W044 Oregon)"
  "Ambient Weather TX-8300 Temperature/Humidity Sensor"
  "Auriol AFW2A1 temperature/humidity sensor"
  "Auriol AHFL temperature/humidity sensor"
  "Auriol HG02832
  "BlueLine Power Monitor"
  "Blyss DC5-UK-WH"
  "Brennenstuhl RCS 2044"
  "Bresser Thermo-/Hygro-Sensor 3CH"
  "Biltema rain gauge"
  "Burnhard BBQ thermometer"
  "Calibeur RF-104 Sensor"
  "Cardin S466-TX2"
  "Chuango Security Technology"
  "Companion WTR001 Temperature Sensor"
  "Digitech XC-0324 temperature sensor"
  "Dish remote 6.3"
  "Ecowitt Wireless Outdoor Thermometer WH53/WH0280/WH0281A"
  "Eurochron EFTH-800 temperature and humidity sensor"
  "Elro DB286A Doorbell"
  "ELV EM 1000"
  "ELV WS 2000"
  "Esperanza EWS"
  "Eurochron temperature and humidity sensor"
  "Fine Offset Electronics
  "Fine Offset Electronics
  "Fine Offset WH1050 Weather Station"
  "Fine Offset Electronics WH1080/WH3080 Weather Station"
  "FS20"
  "FT-004-B Temperature Sensor"
  "Generic wireless motion sensor"
  "Generic Remote SC226x EV1527"
  "Generic temperature sensor 1"
  "Globaltronics QUIGG GT-TMBBQ-05"
  "Globaltronics GT-WT-02 Sensor"
  "Globaltronics GT-WT-03 Sensor"
  "Microchip HCS200 KeeLoq Hopping Encoder based remotes"
  "Honeywell ActivLink
  "HT680 Remote control"
  "inFactory
  "Inovalley kw9015b
  "Interlogix GE UTC Security Devices"
  "Intertechno 433"
  "Kedsum Temperature & Humidity Sensor
  "Kerui PIR / Contact Sensor"
  "LaCrosse TX Temperature / Humidity Sensor"
  "LaCrosse TX141-Bv2
  "LaCrosse/ELV/Conrad WS7000/WS2500 weather sensors"
  "LaCrosse WS-2310 / WS-3600 Weather Station"
  "LightwaveRF"
  "Maverick et73"
  "Mebus 433"
  "Missil ML0757 weather station"
  "Template decoder"
  "Nexus
  "Nice Flor-s remote control for gates"
  "Opus/Imagintronix XT300 Soil Moisture"
  "Oregon Scientific SL109H Remote Thermal Hygro Sensor"
  "OSv1 Temperature Sensor"
  "Philips outdoor temperature sensor (type AJ3650)"
  "Philips outdoor temperature sensor (type AJ7010)"
  "Prologue
  "Quhwa"
  "RF-tech"
  "Rubicson Temperature Sensor"
  "Rubicson 48659 Thermometer"
  "Conrad S3318P
  "Silvercrest Remote Control"
  "Skylink HA-434TL motion sensor"
  "Wireless Smoke and Heat Detector GS 558"
  "Solight TE44/TE66
  "Springfield Temperature and Soil Moisture"
  "TFA Dostmann 30.3221.02 T/H Outdoor Sensor"
  "TFA Drop Rain Gauge 30.3233.01"
  "TFA pool temperature sensor"
  "TFA-Twin-Plus-30.3049
  "Thermopro TP11 Thermometer"
  "Thermopro TP08/TP12/TP20 thermometer"
  "ThermoPro-TX2 temperature sensor"
  "TS-FT002 Wireless Ultrasonic Tank Liquid Level Meter With Temperature Sensor"
  "Visonic powercode"
  "Waveman Switch Transmitter"
  "WG-PB12V1 Temperature Sensor"
  "WS2032 weather station"
  "Hyundai WS SENZOR Remote Temperature Sensor"
  "WT0124 Pool Thermometer"
  "X10 RF"
  "X10 Security"
```

### FSK Devices Supported

```
  "Ambient Weather WH31E Thermo-Hygrometer Sensor
  "TBH weather sensor"
  "Bresser Weather Center 5-in-1"
  "Bresser Weather Center 6-in-1
  "Bresser Weather Center 7-in-1"
  "Danfoss CFR Thermostat"
  "DirecTV RC66RX Remote Control"
  "EcoDHOME Smart Socket and MCEE Solar monitor"
  "emonTx OpenEnergyMonitor"
  "ESIC EMT7110 power meter"
  "Fine Offset Electronics
  "Fine Offset Electronics/ECOWITT WH51 Soil Moisture Sensor"
  "Fine Offset Electronics WH1080/WH3080 Weather Station (FSK)"
  "Holman Industries iWeather WS5029 weather station (newer PCM)"
  "IKEA Sparsnas Energy Meter Monitor"
  "LaCrosse Technology View LTV-WSDTH01 Breeze Pro Wind Sensor"
  "LaCrosse Technology View LTV-R1 Rainfall Gauge"
  "LaCrosse Technology View LTV-TH Thermo/Hygro Sensor"
  "LaCrosse TX29IT
  "LaCrosse TX35DTH-IT
  "LaCrosse Technology View LTV-WR1 Multi Sensor"
  "TFA Marbella Pool Thermometer"
```

## Wiring and Building the Example

This is the diagram I'm using with my devices.

![diagram](docs/CC1101 with BME280 and BH1750.png)

In my units that I have built with the openMQTTGateway firmware I have included a BME280 Temperature Sensor and a BH1750 Light LUX Sensor.  The two sensors are purely optional and you do not need to include them in your builds.

And to build the example I use platformio and opened the example folder as a new window and built from their.

## Projects using the library

* OpenMQTTGateway - https://github.com/1technophile/OpenMQTTGateway
  - ASK Support is currently available in the production branch

## Roadmap / Next Steps

* [ ] Firmware size tuning
* [ ] Further heap usage tuning
* [x] Find stack leak when running all devices
* [ ] Enable addition pulse demod functions
* [ ] Enable FSK enabled devices
* [ ] Add support for SX1276 SX1278 ESP32 LoRa 868MHz/915MHz/433MH based receiver boards

## Compile definition options

```
MY_DEVICES					; Only include my personal subset of devices

CC1101_GDO0         ; gpio pin that is connected to GDO0 on the CC1101
CC1101_GDO2         ; gpio pin that is connected to GDO2 on the CC1101
DEMOD_DEBUG					; enable verbose debugging of signal processing
DEVICE_DEBUG				; Validate fields are mapped to response object ( rtl_433 )
MEMORY_DEBUG				; display heap usage information
RAW_SIGNAL_DEBUG			; display raw received messages
RSSI						; Enable collection of per pulse RSSI Values during signal reception
RTL_DEBUG					; Enable RTL_433 Verbose option ( 0=normal, 1=verbose, 2=verbose decoders, 3=debug decoders, 4=trace decoding. )
SUPPRESS_UNPARSED			; Enable suppression of publishing MQTT messages for unparsed signals, e.g. {model":"unknown","protocol":"signal parsing failed"…
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
  '-DCC1101_GDO0=2'         
  '-DCC1101_GDO2=4'
;  '-DMY_DEVICES=true'
  '-UZmqttDiscovery'
```

* Version 1

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
