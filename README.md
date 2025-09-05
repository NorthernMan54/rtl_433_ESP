This is an attempt at creating an Arduino library for use on ESP32 boards with a CC1101 transceiver or SX127X Transceivers with the device decoders from the [rtl_433](https://github.com/merbanan/rtl_433) package.  And be available for use with openMQTTGateway as an available module.

Inspiration for this came from the ESPiLight effort.  Kudos to puuu for this, and the awesome package.

This is the [LILYGO® LoRa32 V2.1_1.6.1 433Mhz](https://www.lilygo.cc/products/lora3?variant=42476923879605) board running [OpenMQTTGateway](https://github.com/1technophile/OpenMQTTGateway/blob/master/README.md).
![image](https://user-images.githubusercontent.com/19808920/212491102-3ffd719b-9cb5-4494-b74f-49e7947c6887.png)

The initial port implements only enables a subset of the available modulations and device decoders, and is limited to devices I have access to and can test with.

* One comment about the transceiver modules, I have found that the receiver is not as sensitive as a rtl_sdr and I only get about 1/2 the range or less.  And having the antenna oriented into a vertical position does improve reception.

## Enabled rtl_433 Demodulation modules

### OOK

```plaintext
OOK_MC_ZEROBIT
OOK_PULSE_DMC
OOK_PULSE_MANCHESTER_ZEROBIT
OOK_PULSE_NRZS
OOK_PULSE_PCM
OOK_PULSE_PIWM_DC
OOK_PULSE_PPM
OOK_PULSE_PWM
OOK_PULSE_PWM_OSV1
OOK_PULSE_RZ
```

### FSK

```plaintext
FSK_PULSE_MANCHESTER_ZEROBIT
FSK_PULSE_PCM
FSK_PULSE_PWM
```

Please note that FSK or OOK demodulation mode can not be used simultaneously, due the usage of the receiver chipset's signal demodulator.

## Enabled Device Decoders from rtl_433 release 25.02

The current implementation is based on this version of rtl_433 - https://github.com/merbanan/rtl_433/tree/e37c0d78ad2985cd8b1f7a5d6ad09be5acf066c6

### OOK Signal Device Decoders

```plaintext
Registering protocol [0] "Amazon Basics Meat Thermometer"
Registering protocol [1] "Acurite Grill/Meat Thermometer 01185M"
Registering protocol [2] "Akhan 100F14 remote keyless entry"
Registering protocol [3] "Ambient Weather F007TH, TFA 30.3208.02, SwitchDocLabs F016TH temperature sensor"
Registering protocol [4] "Ambient Weather TX-8300 Temperature/Humidity Sensor"
Registering protocol [5] "Atech-WS308 temperature sensor"
Registering protocol [6] "Auriol 4-LD5661/4-LD5972/4-LD6313 temperature/rain sensors"
Registering protocol [7] "Auriol AFT 77 B2 temperature sensor"
Registering protocol [8] "Auriol AFW2A1 temperature/humidity sensor"
Registering protocol [9] "Auriol AHFL temperature/humidity sensor"
Registering protocol [10] "Auriol HG02832, HG05124A-DCF, Rubicson 48957 temperature/humidity sensor"
Registering protocol [11] "Baldr / RainPoint rain gauge."
Registering protocol [12] "Blyss DC5-UK-WH"
Registering protocol [13] "Brennenstuhl RCS 2044"
Registering protocol [14] "Bresser Thermo-/Hygro-Sensor 3CH"
Registering protocol [15] "Bresser Thermo-/Hygro-Sensor Explore Scientific ST1005H"
Registering protocol [16] "Biltema rain gauge"
Registering protocol [17] "Burnhard BBQ thermometer"
Registering protocol [18] "Cardin S466-TX2"
Registering protocol [19] "Celsia CZC1 Thermostat"
Registering protocol [20] "Chuango Security Technology"
Registering protocol [21] "Clipsal CMR113 Cent-a-meter power meter"
Registering protocol [22] "Companion WTR001 Temperature Sensor"
Registering protocol [23] "Cotech 36-7959, SwitchDocLabs FT020T wireless weather station with USB"
Registering protocol [24] "Digitech XC-0324 / AmbientWeather FT005TH temp/hum sensor"
Registering protocol [25] "Dish remote 6.3"
Registering protocol [26] "Ecowitt Wireless Outdoor Thermometer WH53/WH0280/WH0281A"
Registering protocol [27] "Elro DB286A Doorbell"
Registering protocol [28] "EMOS E6016 weatherstation with DCF77"
Registering protocol [29] "EMOS E6016 rain gauge"
Registering protocol [30] "EnOcean ERP1"
Registering protocol [31] "ERT Interval Data Message (IDM)"
Registering protocol [32] "ERT Standard Consumption Message (SCM)"
Registering protocol [33] "Esperanza EWS"
Registering protocol [34] "Eurochron temperature and humidity sensor"
Registering protocol [35] "Fine Offset WH1050 Weather Station"
Registering protocol [36] "Fine Offset Electronics WH1080/WH3080 Weather Station"
Registering protocol [37] "Ford Car Key"
Registering protocol [38] "FS20 / FHT"
Registering protocol [39] "FT-004-B Temperature Sensor"
Registering protocol [40] "Gasmate BA1008 meat thermometer"
Registering protocol [41] "Geevon TX16-3 outdoor sensor"
Registering protocol [42] "Generic wireless motion sensor"
Registering protocol [43] "Generic Remote SC226x EV1527"
Registering protocol [44] "Generic temperature sensor 1"
Registering protocol [45] "Govee Water Leak Detector H5054, Door Contact Sensor B5023"
Registering protocol [46] "Globaltronics QUIGG GT-TMBBQ-05"
Registering protocol [47] "Globaltronics GT-WT-02 Sensor"
Registering protocol [48] "Globaltronics GT-WT-03 Sensor"
Registering protocol [49] "Microchip HCS200/HCS300 KeeLoq Hopping Encoder based remotes"
Registering protocol [50] "Homelead HG9901 (Geevon, Dr.Meter, Royal Gardineer) soil moisture/temp/light level sensor"
Registering protocol [51] "Honeywell Door/Window Sensor, 2Gig DW10/DW11, RE208 repeater"
Registering protocol [52] "Honeywell ActivLink, Wireless Doorbell"
Registering protocol [53] "HT680 Remote control"
Registering protocol [54] "IBIS beacon"
Registering protocol [55] "inFactory, nor-tec, FreeTec NC-3982-913 temperature humidity sensor"
Registering protocol [56] "Interlogix GE UTC Security Devices"
Registering protocol [57] "Intertechno 433"
Registering protocol [58] "Jasco/GE Choice Alert Security Devices"
Registering protocol [59] "Kedsum Temperature & Humidity Sensor, Pearl NC-7415"
Registering protocol [60] "Kerui PIR / Contact Sensor"
Registering protocol [61] "Klimalogg"
Registering protocol [62] "LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth, (TFA, ORIA) sensor"
Registering protocol [63] "LaCrosse/ELV/Conrad WS7000/WS2500 weather sensors"
Registering protocol [64] "LaCrosse WS-2310 / WS-3600 Weather Station"
Registering protocol [65] "LightwaveRF"
Registering protocol [66] "Markisol, E-Motion, BOFU, Rollerhouse, BF-30x, BF-415 curtain remote"
Registering protocol [67] "Maverick ET73"
Registering protocol [68] "Maverick ET-732/733 BBQ Sensor"
Registering protocol [69] "Linear Megacode Garage/Gate Remotes"
Registering protocol [70] "Missil ML0757 weather station"
Registering protocol [71] "Neptune R900 flow meters"
Registering protocol [72] "Template decoder"
Registering protocol [73] "KlikAanKlikUit Wireless Switch"
Registering protocol [74] "Nexa"
Registering protocol [75] "Nexus, FreeTec NC-7345, NX-3980, Solight TE82S, TFA 30.3209 temperature/humidity sensor"
Registering protocol [76] "Nice Flor-s remote control for gates"
Registering protocol [77] "Norgo NGE101"
Registering protocol [78] "Opus/Imagintronix XT300 Soil Moisture"
Registering protocol [79] "Oregon Scientific Weather Sensor"
Registering protocol [80] "Oregon Scientific SL109H Remote Thermal Hygro Sensor"
Registering protocol [81] "OSv1 Temperature Sensor"
Registering protocol [82] "Philips outdoor temperature sensor (type AJ3650)"
Registering protocol [83] "Philips outdoor temperature sensor (type AJ7010)"
Registering protocol [84] "SmartFire Proflame 2 remote control"
Registering protocol [85] "Prologue, FreeTec NC-7104, NC-7159-675 temperature sensor"
Registering protocol [86] "Proove / Nexa / KlikAanKlikUit Wireless Switch"
Registering protocol [87] "Quhwa"
Registering protocol [88] "Radiohead ASK"
Registering protocol [89] "RainPoint soil temperature and moisture sensor"
Registering protocol [90] "RainPoint HCS012ARF Rain Gauge sensor"
Registering protocol [91] "Regency Ceiling Fan Remote (-f 303.75M to 303.96M)"
Registering protocol [92] "Revolt NC-5642 Energy Meter"
Registering protocol [93] "Revolt ZX-7717 power meter"
Registering protocol [94] "RF-tech"
Registering protocol [95] "Risco 2 Way Agility protocol, Risco PIR/PET Sensor RWX95P"
Registering protocol [96] "Rosstech Digital Control Unit DCU-706/Sundance/Jacuzzi"
Registering protocol [97] "Rubicson, TFA 30.3197 or InFactory PT-310 Temperature Sensor"
Registering protocol [98] "Rubicson 48659 Thermometer"
Registering protocol [99] "Rubicson Pool Thermometer 48942"
Registering protocol [100] "Conrad S3318P, FreeTec NC-5849-913 temperature humidity sensor, ORIA WA50 ST389 temperature sensor"
Registering protocol [101] "Schou 72543 Day Rain Gauge, Motonet MTX Rain, MarQuant Rain Gauge, TFA Dostmann 30.3252.01/47.3006.01 Rain Gauge and Thermometer, ADE WS1907"
Registering protocol [102] "Schrader TPMS"
Registering protocol [103] "Standard Consumption Message Plus (SCMplus)"
Registering protocol [104] "Security+ (Keyfob)"
Registering protocol [105] "Silvercrest Remote Control"
Registering protocol [106] "Wireless Smoke and Heat Detector GS 558"
Registering protocol [107] "Solight TE44/TE66, EMOS E0107T, NX-6876-917"
Registering protocol [108] "Somfy RTS"
Registering protocol [109] "Springfield Temperature and Soil Moisture"
Registering protocol [110] "Telldus weather station FT0385R sensors"
Registering protocol [111] "TFA Dostmann 30.3221.02 T/H Outdoor Sensor (also 30.3249.02)"
Registering protocol [112] "TFA pool temperature sensor"
Registering protocol [113] "Thermopro TP11 Thermometer"
Registering protocol [114] "ThermoPro TP08/TP12/TP20 thermometer"
Registering protocol [115] "ThermoPro-TX2 temperature sensor"
Registering protocol [116] "ThermoPro TX-2C Thermometer and Humidity sensor"
Registering protocol [117] "Thermor DG950 weather station"
Registering protocol [118] "EezTire E618, Carchet TPMS, TST-507 TPMS"
Registering protocol [119] "GM-Aftermarket TPMS"
Registering protocol [120] "TyreGuard 400 TPMS"
Registering protocol [121] "TS-FT002 Wireless Ultrasonic Tank Liquid Level Meter With Temperature Sensor"
Registering protocol [122] "Emos TTX201 Temperature Sensor"
Registering protocol [123] "Vaillant calorMatic VRT340f Central Heating Control"
Registering protocol [124] "Vauno EN8822C"
Registering protocol [125] "Visonic powercode"
Registering protocol [126] "Watts WFHT-RF Thermostat"
Registering protocol [127] "Waveman Switch Transmitter"
Registering protocol [128] "WEC-2103 temperature/humidity sensor"
Registering protocol [129] "WG-PB12V1 Temperature Sensor"
Registering protocol [130] "WS2032 weather station"
Registering protocol [131] "Hyundai WS SENZOR Remote Temperature Sensor"
Registering protocol [132] "WT450, WT260H, WT405H"
Registering protocol [133] "X10 Security"
Registering protocol [134] "Yale HSA (Home Security Alarm), YES-Alarmkit"
```

The gaps in the numbers are device decoders disabled by default.

### FSK Signal Device Decoders

```plaintext
Registering protocol [0] "Ambient Weather WH31E Thermo-Hygrometer Sensor, EcoWitt WH40 rain gauge, WS68 weather station"
Registering protocol [1] "ANT and ANT+ devices"
Registering protocol [2] "Apator Metra E-RM 30"
Registering protocol [3] "Arad/Master Meter Dialog3G water utility meter"
Registering protocol [4] "TBH weather sensor"
Registering protocol [5] "Arexx Multilogger IP-HA90, IP-TH78EXT, TSN-70E"
Registering protocol [6] "Bresser Weather Center 5-in-1"
Registering protocol [7] "Bresser Weather Center 6-in-1, 7-in-1 indoor, soil, new 5-in-1, 3-in-1 wind gauge, Froggit WH6000, Ventus C8488A"
Registering protocol [8] "Bresser Weather Center 7-in-1, Air Quality PM2.5/PM10 7009970, CO2 7009977, HCHO/VOC 7009978 sensors"
Registering protocol [9] "Bresser water leakage"
Registering protocol [10] "Bresser lightning"
Registering protocol [11] "Cavius smoke, heat and water detector"
Registering protocol [12] "CED7000 Shot Timer"
Registering protocol [13] "Chamberlain CWPIRC PIR Sensor"
Registering protocol [14] "CurrentCost Current Sensor"
Registering protocol [15] "DeltaDore X3D devices"
Registering protocol [16] "DirecTV RC66RX Remote Control"
Registering protocol [17] "EcoDHOME Smart Socket and MCEE Solar monitor"
Registering protocol [18] "Efergy e2 classic"
Registering protocol [19] "Efergy Optical"
Registering protocol [20] "Emax W6, rebrand Altronics x7063/4, Optex 990040/50/51, Orium 13093/13123, Infactory FWS-1200, Newentor Q9, Otio 810025, Protmex PT3390A, Jula Marquant 014331/32, TechniSat IMETEO X6 76-4924-00, Weather Station or temperature/humidity sensor"
Registering protocol [21] "emonTx OpenEnergyMonitor"
Registering protocol [22] "ESIC EMT7110 power meter"
Registering protocol [23] "Ambient Weather WH31L (FineOffset WH57) Lightning-Strike sensor"
Registering protocol [24] "Fine Offset Electronics WH45 air quality sensor"
Registering protocol [25] "Fine Offset Electronics WH46 air quality sensor"
Registering protocol [26] "Fine Offset / Ecowitt WH55 water leak sensor"
Registering protocol [27] "Fine Offset Electronics WN34S/L/D and Froggit DP150/D35 temperature sensor"
Registering protocol [28] "Fine Offset Electronics WS80 weather station"
Registering protocol [29] "Fine Offset Electronics WS90 weather station"
Registering protocol [30] "Flowis flow meters"
Registering protocol [31] "GE Color Effects"
Registering protocol [32] "GEO minim+ energy monitor"
Registering protocol [33] "Honda Car Key"
Registering protocol [34] "Honeywell CM921 Wireless Programmable Room Thermostat"
Registering protocol [35] "IKEA Sparsnas Energy Meter Monitor"
Registering protocol [36] "Inkbird ITH-20R temperature humidity sensor"
Registering protocol [37] "Insteon"
Registering protocol [38] "LaCrosse Technology View LTV-WSDTH01 Breeze Pro Wind Sensor"
Registering protocol [39] "LaCrosse Technology View LTV-R1, LTV-R3 Rainfall Gauge, LTV-W1/W2 Wind Sensor"
Registering protocol [40] "LaCrosse Technology View LTV-TH Thermo/Hygro Sensor"
Registering protocol [41] "LaCrosse TX31U-IT, The Weather Channel WS-1910TWC-IT"
Registering protocol [42] "LaCrosse TX34-IT rain gauge"
Registering protocol [43] "LaCrosse TX29IT, TFA Dostmann 30.3159.IT Temperature sensor"
Registering protocol [44] "LaCrosse Technology View LTV-WR1 Multi Sensor"
Registering protocol [45] "Marlec Solar iBoost+ sensors"
Registering protocol [46] "Maverick XR-30 BBQ Sensor"
Registering protocol [47] "Maverick XR-50 BBQ Sensor"
Registering protocol [48] "Mueller Hot Rod water meter"
Registering protocol [49] "Oil Ultrasonic SMART FSK"
Registering protocol [50] "Oil Ultrasonic STANDARD FSK"
Registering protocol [51] "Watchman Sonic / Apollo Ultrasonic / Beckett Rocket oil tank monitor"
Registering protocol [52] "Watchman Sonic Advanced / Plus, Tekelek"
Registering protocol [53] "Quinetic"
Registering protocol [54] "RojaFlex shutter and remote devices"
Registering protocol [55] "Sharp SPC775 weather station"
Registering protocol [56] "SimpliSafe Gen 3 Home Security System"
Registering protocol [57] "Somfy io-homecontrol"
Registering protocol [58] "SRSmith Pool Light Remote Control SRS-2C-TX (-f 915M)"
Registering protocol [59] "Steelmate TPMS"
Registering protocol [60] "TFA Dostmann 14.1504.V2 Radio-controlled grill and meat thermometer"
Registering protocol [61] "TFA Marbella Pool Thermometer"
Registering protocol [62] "ThermoPro TP28b Super Long Range Wireless Meat Thermometer for Smoker BBQ Grill"
Registering protocol [63] "ThermoPro TX-7B Outdoor Thermometer Hygrometer"
Registering protocol [64] "Abarth 124 Spider TPMS"
Registering protocol [65] "AVE TPMS"
Registering protocol [66] "BMW Gen4-Gen5 TPMS and Audi TPMS Pressure Alert, multi-brand HUF/Beru, Continental, Schrader/Sensata, Audi"
Registering protocol [67] "Citroen TPMS"
Registering protocol [68] "Elantra2012 TPMS"
Registering protocol [69] "Ford TPMS"
Registering protocol [70] "Hyundai TPMS (VDO)"
Registering protocol [71] "Jansite TPMS Model TY02S"
Registering protocol [72] "Jansite TPMS Model Solar"
Registering protocol [73] "Kia TPMS (-s 1000k)"
Registering protocol [74] "Nissan TPMS"
Registering protocol [75] "PMV-107J (Toyota) TPMS"
Registering protocol [76] "Porsche Boxster/Cayman TPMS"
Registering protocol [77] "Renault TPMS"
Registering protocol [78] "Renault 0435R TPMS"
Registering protocol [79] "Toyota TPMS"
Registering protocol [80] "Unbranded SolarTPMS for trucks"
Registering protocol [81] "Vevor Wireless Weather Station 7-in-1"
```
## Sensors I use as part of Testing

These are the sensors that are part of my personal collection, and can confirm that they work correctly.  Other sensors devices are on a best effort basis as I have no method to test and confirm functionality.

```
Acurite Tower - Acurite 592TXR Temp/Humidity, 592TX Temp, 5n1 Weather Station, 6045 Lightning, 899 Rain, 3N1, Atlas
LaCrosse-TX141Bv3 - LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth, (TFA, ORIA) sensor
Acurite-986 - Acurite 986 Refrigerator / Freezer Thermometer
Skylink_HA-434TL_motion - Skylink HA-434TL motion sensor
FineOffSet WH51
```

## Transceiver Modules Supported

### CC1101 433 Mhz Transceiver Module

Datasheet for the CC1101 board I'm using

[CC1101 datasheet](docs/E07-M1101D-TH_Usermanual_EN_v1.30.pdf)

### SX127X - Heltec WiFi LoRa 32v2 and LILYGO® LoRa32

This is the [Heltec SX127X](https://heltec.org/project/wifi-lora-32v2/) board I used for development. I used the 433 Mhz Board with the SX127X chipset.

This is the [LILYGO® LoRa32 V2.1_1.6.1 433](https://www.lilygo.cc/products/lora3?variant=42476923879605) board I used for development. I used the 433 Mhz Board with the SX127X chipset.

When your purchasing one of these, please watch out for similar boards that do not use the SX127X chipset.  These others boards will not work, as the required receiver mode is not available in the chipset.

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

* Acurite 5n1 to Weather Services Forwarder - https://github.com/shchuko/acurite-bridge

## Roadmap / Next Steps

* [x] Firmware size tuning
* [x] Further heap usage tuning
* [x] Find stack leak when running all devices
* [x] Enable additional pulse demod functions
* [x] Simplify the process to update from the rtl_433 source

# Signal detection and reception approach

To determne that a signal is available for reception, the library watches the current RSSI reported by the transceiver module and when it crosses a predetermined RSSI threshold it enables the signal receiver function.   End of signal is determined when the signal drops below the predetermined RSSI threshold for a minimum of 150,000 micro seconds.

## RSSI Threshold Automatic Setting

The RSSI Threshold for signal detection is automatically determined based on the average RSSI signal level received aka RSSI floor level with a delta ( RSSI_THRESHOLD ) added to it.  The average RSSI signal level is calculated over RSSI_SAMPLES.

## SX127X OOK RSSI FIXED Threshold

For background see section 2.1.3.2. of SX127X Data sheet

To tune the SX127X OOK RSSI FIXED Threshold two values are used to determine if the threshold needs to be increased or decreased.  The first is the noise received between signals.  If the number of noise bits received between signals is greater than 100, then the threshold is incremented.  Second is the unparsed signals.  If an unparsed signal is received, but it has less than 20 pulses, the threshold is decremented.

The first approach is what is recommended in the SX127X datasheet, and the second is a control to lower the threshold if it is too high and incomplete signals are received.

# Compile definition options

```plaintext
DEMOD_DEBUG           ; enable verbose debugging of signal processing
DEVICE_DEBUG          ; Validate fields are mapped to response object ( rtl_433 )
MEMORY_DEBUG          ; display heap usage information
RESOURCE_DEBUG        : Monitor HEAP and STACK usage and report large jumps
MY_DEVICES            ; Only include my personal subset of devices
NO_DEAF_WORKAROUND    ; Workaround for issue #16 ( by default the workaround is enabled )
PUBLISH_UNPARSED      ; Enable publishing of MQTT messages for unparsed signals, e.g. {model":"unknown","protocol":"signal parsing failed"…
RAW_SIGNAL_DEBUG      ; display raw received messages
RSSI_SAMPLES          ; Number of rssi samples to collect for average calculation, defaults to 50,000
RSSI_THRESHOLD        ; Delta applied to average RSSI value to calculate RSSI Signal Threshold, defaults to 9
RTL_DEBUG             ; Enable RTL_433 device decoder verbose mode for all device decoders ( 0=normal, 1=verbose, 2=verbose decoders, 3=debug decoders, 4=trace decoding. )
RTL_VERBOSE=##        ; Enable RTL_433 device decoder verbose mode, ## is the decoder # from the appropriate memcpy line in signalDecoder.cpp
RTL_ANALYZER          ; Enable pulse stream analysis ( note is very resource intensive and will not work with other modules )
RTL_ANALYZE=##        ; Enable pulse stream analysis for decoder ##
SIGNAL_RSSI           ; Enable collection of per pulse RSSI Values during signal reception for display in signal debug messages
RF_MODULE_INIT_STATUS ; Display transceiver config during startup
DISABLERSSITHRESHOLD  ; Disable automatic setting of RSSI_THRESHOLD ( legacy behaviour ), and use MINRSSI ( -82 )
OOK_MODULATION        ; Enable OOK Device Decoders, setting to false enables FSK Device Decoders 
```

## RF Module Wiring

```plaintext
ONBOARD_LED           ; GPIO pin to toggle during signal reception ( Typically onboard LED )
```

### SX1276 Module Options

```plaintext
RF_SX1276             ; Enable support for SX1276 Transceiver
OOK_FIXED_THRESHOLD   ; Initial OOK threshold ( See 2.1.3.2. of datasheet ), defaults to 90
AUTOOOKFIX            ; Set to enable automatic setting of OOK_FIXED_THRESHOLD based on noise level between signals
```

### SX1276 Module Wiring ( Required if not using standard configuration )

```plaintext
RF_MODULE_CS          ; SX1276 SPI Chip select
RF_MODULE_DIO0        ; SX1276 DIO0 PIN
RF_MODULE_RST         ; SX1276 RST PIN
RF_MODULE_DIO1        ; SX1276 DIO1 PIN
```

### SX1278 Module Options

```plaintext
RF_SX1278 - Enable support for SX1276
OOK_FIXED_THRESHOLD   ; Initial OOK threshold ( See 2.1.3.2. of datasheet ), defaults to 90
AUTOOOKFIX            ; Set to enable automatic setting of OOK_FIXED_THRESHOLD based on noise level between signals
```

### SX1278 Module Wiring ( Required if not using standard configuration )

```plaintext
RF_MODULE_CS          ; SX1278 SPI Chip select
RF_MODULE_DIO0        ; SX1278 DIO0 PIN
RF_MODULE_RST         ; SX1278 RST PIN
RF_MODULE_DIO1        ; SX1278 DIO1 PIN
```

### CC1101 Module Options

```plaintext
RF_CC1101             ; Enable support for CC1101 Transceiver
NO_DEAF_WORKAROUND    ; Workaround for issue #16 ( by default the workaround is enabaled )
```

### CC1101 Module Wiring

```plaintext
RF_MODULE_CS          ; CC1101 SPI Chip select
RF_MODULE_GDO0        ; CC1101 GDOO PIN
RF_MODULE_GDO2        ; CC1101 GDO2 PIN
```

## RF Module SPI Wiring ( Required if not using standard configuration )

When using a non standard SPI configuration ( Standard config is SCK - 18, MISO - 19, MOSI - 23, CS - 5)

```plaintext
RF_MODULE_SCK         ; SPI Clock
RF_MODULE_MISO        ; SPI Serial Output
RF_MODULE_MOSI        ; SPI Serial Input
RF_MODULE_CS          ; SPI Chip select
```

## Porting approach

Copying src files

```plaintext
Copying rtl_433/src abuf.c to src/rtl_433
Copying rtl_433/src bitbuffer.c to src/rtl_433
Copying rtl_433/src compat_time.c to src/rtl_433
Copying rtl_433/src data.c to src/rtl_433
Copying rtl_433/src decoder_util.c to src/rtl_433
Copying rtl_433/src list.c to src/rtl_433
Copying rtl_433/src logger.c to src/rtl_433
Copying rtl_433/src output_log.c to src/rtl_433
Copying rtl_433/src pulse_data.c to src/rtl_433
Copying rtl_433/src r_util.c to src/rtl_433
Copying rtl_433/src util.c to src/rtl_433
```

These src files need copying and updating

```shell
cp ../rtl_433/src/pulse_analyzer.c ../src/rtl_433
cp ../rtl_433/src/pulse_slicer.c ../src/rtl_433
cp ../rtl_433/src/r_api.c ../src/rtl_433
```

Copying include files

```plaintext
Copying rtl_433/include abuf.h to include
Copying rtl_433/include bitbuffer.h to include
Copying rtl_433/include compat_time.h to include
Copying rtl_433/include decoder.h to include
Copying rtl_433/include decoder_util.h to include
Copying rtl_433/include fatal.h to include
Copying rtl_433/include list.h to include
Copying rtl_433/include logger.h to include
Copying rtl_433/include optparse.h to include
Copying rtl_433/include output_log.h to include
Copying rtl_433/include pulse_analyzer.h to include
Copying rtl_433/include pulse_detect.h to include
Copying rtl_433/include pulse_slicer.h to include
Copying rtl_433/include r_api.h to include
Copying rtl_433/include r_device.h to include
Copying rtl_433/include r_util.h to include
Copying rtl_433/include rfraw.h to include
Copying rtl_433/include util.h to include
```

These include files need copying and updating

```shell
cp ../rtl_433/include/data.h ../include
cp ../rtl_433/include/pulse_data.h ../include
cp ../rtl_433/include/r_private.h ../include
cp ../rtl_433/include/rtl_433.h ../include
```

## Codebase conflicts

* ESPiLight and rtl_433 conflict on silvercrest

```plaintext
.platformio/packages/toolchain-xtensa32/bin/../lib/gcc/xtensa-esp32-elf/5.2.0/../../../../xtensa-esp32-elf/bin/ld: Warning: size of symbol `silvercrest' changed from 4 in .pio/build/rtl_433-9e0770/libb28/libESPiLight.a(protocol.c.o) to 76 in .pio/build/rtl_433-9e0770/lib395/librtl_433_ESP.a(silvercrest.c.o)
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
* [Using low-cost wireless sensors in the unlicensed bands](https://lwn.net/Articles/921497/)

February 2023
* [OpenMQTTGateway Connects Many Things to Your Home Automation](https://www.youtube.com/watch?v=_gdXR1uklaY)

January 2024
* [LilyGO 433MHz Lora Board - Sensordaten per openMQTTGateway an HA übertragen](https://www.youtube.com/watch?v=KbRNd1DyRbg)
