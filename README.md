This is an attempt at creating an Arduino library for use on ESP32 boards with a CC1101 transceiver or SX127X Transceivers with the device decoders from the [rtl_433](https://github.com/merbanan/rtl_433) package.  And be available for use with openMQTTGateway as an available module.

Architecture, task ownership, callback context, lifecycle, and resource notes
are documented in [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md). The upstream
refresh procedure is documented in [docs/UPSTREAM.md](docs/UPSTREAM.md).

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

## Enabled Device Decoders from rtl_433

The bundled decoders were refreshed on August 14, 2026 from rtl_433 development
snapshot `25.12-346-g91b2ebdb`, commit [91b2ebdb](https://github.com/merbanan/rtl_433/tree/91b2ebdb42e59798f98b4ebc192f137d2acbf66d).

### OOK Signal Device Decoders

```plaintext
Registering protocol [0] "Amazon Basics Meat Thermometer"
Registering protocol [1] "Acurite 896 Rain Gauge"
Registering protocol [2] "Acurite 609TXC Temperature and Humidity Sensor"
Registering protocol [3] "Acurite 592TXR Temp/Humidity, 592TX Temp, 5n1 Weather Station, 6045 Lightning, 899 Rain, 3N1, Atlas"
Registering protocol [4] "Acurite 986 Refrigerator / Freezer Thermometer"
Registering protocol [5] "Acurite 606TX Temperature Sensor"
Registering protocol [6] "Acurite 00275rm,00276rm Temp/Humidity with optional probe"
Registering protocol [7] "Acurite 590TX Temperature with optional Humidity"
Registering protocol [8] "Acurite Grill/Meat Thermometer 01185M"
Registering protocol [9] "Akhan 100F14 remote keyless entry"
Registering protocol [10] "AlectoV1 Weather Sensor (Alecto WS3500 WS4500 Ventus W155/W044 Oregon)"
Registering protocol [11] "Ambient Weather F007TH, TFA 30.3208.02, SwitchDocLabs F016TH temperature sensor"
Registering protocol [12] "Ambient Weather TX-8300 Temperature/Humidity Sensor"
Registering protocol [13] "Astrostart 2000 Car Remote (-f 372.4M)"
Registering protocol [14] "Atech-WS308 temperature sensor"
Registering protocol [15] "Audiovox PRO-OE3B Car Remote (-f 303.4M)"
Registering protocol [16] "Auriol 4-LD5661/4-LD5972/4-LD6313, Sempre 4-AH0423-4 temperature/rain sensors"
Registering protocol [17] "Auriol AFT 77 B2 temperature sensor"
Registering protocol [18] "Auriol AFW2A1 temperature/humidity sensor"
Registering protocol [19] "Auriol AHFL temperature/humidity sensor"
Registering protocol [20] "Auriol HG02832, HG05124A-DCF, Rubicson 48957 temperature/humidity sensor"
Registering protocol [21] "Auriol HG04641A temperature station"
Registering protocol [22] "Baldr HCS528ARF Pool Thermometer sensor"
Registering protocol [23] "Baldr / RainPoint rain gauge."
Registering protocol [24] "Baldr E0666TH Thermo-Hygrometer"
Registering protocol [25] "Blyss DC5-UK-WH"
Registering protocol [26] "bm5-v2 12V Battery Monitor"
Registering protocol [27] "Brennenstuhl RCS 2044"
Registering protocol [28] "Bresser Thermo-/Hygro-Sensor 3CH"
Registering protocol [29] "Bresser Thermo-/Hygro-Sensor Explore Scientific ST1005H"
Registering protocol [30] "Biltema rain gauge"
Registering protocol [31] "Burnhard BBQ thermometer"
Registering protocol [32] "Calibeur RF-104 Sensor"
Registering protocol [33] "Cardin S466-TX2"
Registering protocol [34] "Celsia CZC1 Thermostat"
Registering protocol [35] "Chrysler Car Remote (-f 315.1M -s 920k)"
Registering protocol [36] "Chuango Security Technology"
Registering protocol [37] "Clipsal CMR113 Cent-a-meter power meter"
Registering protocol [38] "Code Alarm FRDPC2002 Car Remote"
Registering protocol [39] "Companion WTR001 Temperature Sensor"
Registering protocol [40] "Compustar 1WG3R Car Remote"
Registering protocol [41] "Cotech 36-7900 rain gauge"
Registering protocol [42] "Cotech 36-7959, SwitchDocLabs FT020T wireless weather station with USB"
Registering protocol [43] "Cotech FT0203/18-3676 anemometer"
Registering protocol [44] "Dickert MAHS433-01 garage door remote control"
Registering protocol [45] "Digitech XC-0324 / AmbientWeather FT005TH temp/hum sensor"
Registering protocol [46] "Dish remote 6.3"
Registering protocol [47] "DSC Security Contact"
Registering protocol [48] "DSC Security Contact (WS4945)"
Registering protocol [49] "Ecowitt Wireless Outdoor Thermometer WH53/WH0280/WH0281A"
Registering protocol [50] "Eurochron EFTH-800 temperature and humidity sensor"
Registering protocol [51] "Elro DB286A Doorbell"
Registering protocol [52] "ELV EM 1000"
Registering protocol [53] "ELV WS 2000"
Registering protocol [54] "EMOS E6016 weatherstation with DCF77, EMOS E6018 temperature/humidity sensor"
Registering protocol [55] "EMOS E6016 rain gauge"
Registering protocol [56] "EnOcean ERP1"
Registering protocol [57] "ERT Interval Data Message (IDM)"
Registering protocol [58] "ERT Interval Data Message (IDM) for Net Meters"
Registering protocol [59] "ERT Standard Consumption Message (SCM)"
Registering protocol [60] "ESA1000 / ESA2000 Energy Monitor, GIRA Wetterstation"
Registering protocol [61] "Esperanza EWS"
Registering protocol [62] "Esun EN2053 two-channel BBQ thermometer"
Registering protocol [63] "Eurochron temperature and humidity sensor"
Registering protocol [64] "Fine Offset Electronics, WH2, WH5, Telldus Temperature/Humidity/Rain Sensor"
Registering protocol [65] "Agimex Rosenborg 66796 (collides with Fine Offset Electronics WH5) Temperature/Humidity Sensor"
Registering protocol [66] "Fine Offset Electronics, WH0530 Temperature/Rain Sensor"
Registering protocol [67] "Fine Offset WH1050 Weather Station"
Registering protocol [68] "Fine Offset Electronics WH1080/WH3080 Weather Station"
Registering protocol [69] "Florabest FB-TH-1 BBQ Thermometer"
Registering protocol [70] "Ford Car Key"
Registering protocol [71] "FS20 / FHT"
Registering protocol [72] "FT-004-B Temperature Sensor"
Registering protocol [73] "Funkbus / Instafunk (Berker, Gira, Jung)"
Registering protocol [74] "Gasmate BA1008 meat thermometer"
Registering protocol [75] "Geevon TX16-3 outdoor sensor"
Registering protocol [76] "Geevon TX19-1 outdoor sensor"
Registering protocol [77] "Generic wireless motion sensor"
Registering protocol [78] "Generic Remote SC226x EV1527"
Registering protocol [79] "Generic temperature sensor 1"
Registering protocol [80] "GM ABO1502T Car Remote (-f 314.9M)"
Registering protocol [81] "Govee Water Leak Detector H5054, Door Contact Sensor B5023"
Registering protocol [82] "Govee Water Leak Detector H5054"
Registering protocol [83] "RF-T0912 Grill Thermometer"
Registering protocol [84] "Globaltronics QUIGG GT-TMBBQ-05"
Registering protocol [85] "Globaltronics GT-WT-02 Sensor"
Registering protocol [86] "Globaltronics GT-WT-03 Sensor"
Registering protocol [87] "Microchip HCS200/HCS300 KeeLoq Hopping Encoder based remotes"
Registering protocol [88] "Microchip HCS361 KeeLoq Hopping Encoder based remotes (-f 315.1M) (Sync, 833 bit/s)"
Registering protocol [89] "Microchip HCS361 KeeLoq Hopping Encoder based remotes (-f 315.1M) (Sync, 1667 bit/s)"
Registering protocol [90] "Microchip HCS361 KeeLoq Hopping Encoder based remotes (-f 315.1M) (No Sync, 833 bit/s)"
Registering protocol [91] "Microchip HCS361 KeeLoq Hopping Encoder based remotes (-f 315.1M) (No Sync, 1667 bit/s)"
Registering protocol [92] "Microchip HCS361 KeeLoq Hopping Encoder based remotes (-f 315.1M) (PIWM, 2500 bit/s)"
Registering protocol [93] "Microchip HCS361 KeeLoq Hopping Encoder based remotes (-f 315.1M) (PIWM, 5000 bit/s)"
Registering protocol [94] "Microchip HCS362 KeeLoq PWM"
Registering protocol [95] "Microchip HCS362 KeeLoq MC"
Registering protocol [96] "HIDEKI TS04 Temperature, Humidity, Wind and Rain Sensor"
Registering protocol [97] "Holman Industries iWeather WS5029 weather station (older PWM, OOK), BIOWIN 270208"
Registering protocol [98] "Homelead HG9901 (Geevon, Dr.Meter, Royal Gardineer) soil moisture/temp/light level sensor"
Registering protocol [99] "Honeywell Door/Window Sensor, 2Gig DW10/DW11, RE208 repeater"
Registering protocol [100] "Honeywell ActivLink, Wireless Doorbell"
Registering protocol [101] "HT680 Remote control"
Registering protocol [102] "IBIS beacon"
Registering protocol [103] "inFactory, nor-tec, FreeTec NC-3982-913 temperature humidity sensor"
Registering protocol [104] "Inovalley kw9015b, TFA Dostmann 30.3161 (Rain and temperature sensor)"
Registering protocol [105] "Interlogix GE UTC Security Devices"
Registering protocol [106] "Intertechno 433"
Registering protocol [107] "Jasco/GE Choice Alert Security Devices"
Registering protocol [108] "Kedsum Temperature & Humidity Sensor, Pearl NC-7415"
Registering protocol [109] "Kerui PIR / Contact Sensor"
Registering protocol [110] "Kidde RF-SM-DC wireless-interconnect smoke alarm"
Registering protocol [111] "Klimalogg"
Registering protocol [112] "LaCrosse TX Temperature / Humidity Sensor"
Registering protocol [113] "LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth, (TFA, ORIA) sensor"
Registering protocol [114] "LaCrosse Technology TX22U-IT (OOK)"
Registering protocol [115] "LaCrosse/ELV/Conrad WS7000/WS2500 weather sensors"
Registering protocol [116] "LaCrosse WS-2310 / WS-3600 Weather Station"
Registering protocol [117] "LightwaveRF"
Registering protocol [118] "Markisol, E-Motion, BOFU, Rollerhouse, BF-30x, BF-415 curtain remote"
Registering protocol [119] "Martec MPLCD Ceiling Fan Remote"
Registering protocol [120] "Maverick ET73"
Registering protocol [121] "Maverick ET-732/733 BBQ Sensor"
Registering protocol [122] "Mebus 433"
Registering protocol [123] "Linear Megacode Garage/Gate Remotes"
Registering protocol [124] "MIC 6SC2 Car Remote (-f 315.1M)"
Registering protocol [125] "Missil ML0757 weather station"
Registering protocol [126] "Neptune R900 flow meters"
Registering protocol [127] "Template decoder"
Registering protocol [128] "KlikAanKlikUit Wireless Switch"
Registering protocol [129] "Nexa"
Registering protocol [130] "Nexus, FreeTec NC-7345, NX-3980, Solight TE82S, TFA 30.3209 temperature/humidity sensor"
Registering protocol [131] "Nexus, CRX, Prego sauna temperature sensor"
Registering protocol [132] "Nice Flor-s remote control for gates"
Registering protocol [133] "Norgo NGE101"
Registering protocol [134] "Oil Ultrasonic STANDARD ASK"
Registering protocol [135] "Omni Multisensor"
Registering protocol [136] "Opus/Imagintronix XT300 Soil Moisture"
Registering protocol [137] "Oregon Scientific Weather Sensor"
Registering protocol [138] "Oregon Scientific SL109H Remote Thermal Hygro Sensor"
Registering protocol [139] "OSv1 Temperature Sensor"
Registering protocol [140] "Oria WA150KM freezer and fridge thermometer"
Registering protocol [141] "Philips outdoor temperature sensor (type AJ3650)"
Registering protocol [142] "Philips outdoor temperature sensor (type AJ7010)"
Registering protocol [143] "SmartFire Proflame 2 remote control"
Registering protocol [144] "Prologue, FreeTec NC-7104, NC-7159-675 temperature sensor"
Registering protocol [145] "Proove / Nexa / KlikAanKlikUit Wireless Switch"
Registering protocol [146] "Quhwa"
Registering protocol [147] "Radiohead ASK"
Registering protocol [148] "Sensible Living Mini-Plant Moisture Sensor"
Registering protocol [149] "RainPoint soil temperature and moisture sensor"
Registering protocol [150] "RainPoint HCS012ARF Rain Gauge sensor"
Registering protocol [151] "Regency Ceiling Fan Remote (-f 303.75M to 303.96M)"
Registering protocol [152] "Revolt NC-5642 Energy Meter"
Registering protocol [153] "Revolt ZX-7717 power meter"
Registering protocol [154] "RF-tech"
Registering protocol [155] "RfxMeter, RFXPwr"
Registering protocol [156] "Risco 2 Way Agility protocol, Risco PIR/PET Sensor RWX95P"
Registering protocol [157] "Rosstech Digital Control Unit DCU-706/Sundance/Jacuzzi"
Registering protocol [158] "Rubicson, TFA 30.3197 or InFactory PT-310 Temperature Sensor"
Registering protocol [159] "Rubicson 48659 Thermometer"
Registering protocol [160] "Rubicson Pool Thermometer 48942"
Registering protocol [161] "Conrad S3318P, FreeTec NC-5849-913 temperature humidity sensor, ORIA WA50 ST389 temperature sensor"
Registering protocol [162] "Sainlogic SA8, Gevanti SA8 Weather Station"
Registering protocol [163] "Schou 72543 Day Rain Gauge, Motonet MTX Rain, MarQuant Rain Gauge, TFA Dostmann 30.3252.01/47.3006.01 Rain Gauge and Thermometer, ADE WS1907"
Registering protocol [164] "Schrader TPMS"
Registering protocol [165] "Schrader TPMS EG53MA4, Saab, Opel, Vauxhall, Chevrolet"
Registering protocol [166] "Schrader TPMS SMD3MA4 (Subaru)"
Registering protocol [167] "Schrader TPMS MRXNIS315G3, 3039 (Infiniti, Nissan, Renault), aka Redi-Sensor SE10001HP/SE10001HPR"
Registering protocol [168] "Schrader TPMS MRXBC5A4 (BMW)"
Registering protocol [169] "Standard Consumption Message Plus (SCMplus)"
Registering protocol [170] "Security+ (Keyfob)"
Registering protocol [171] "Shenzhen Wale WL-TH6R Temperature & Humidity Sensor"
Registering protocol [172] "Siemens 5WY72XX Car Remote (-f 315.1M)"
Registering protocol [173] "Silvercrest Remote Control"
Registering protocol [174] "SimpliSafe Home Security System (May require disabling automatic gain for KeyPad decodes)"
Registering protocol [175] "Skylink HA-434TL motion sensor"
Registering protocol [176] "Wireless Smoke and Heat Detector GS 558"
Registering protocol [177] "Solight TE44/TE66, EMOS E0107T, NX-6876-917"
Registering protocol [178] "Somfy RTS"
Registering protocol [179] "Springfield Temperature and Soil Moisture"
Registering protocol [180] "Telldus weather station FT0385R sensors"
Registering protocol [181] "TFA Dostmann 30.3221.02 T/H Outdoor Sensor (also 30.3249.02)"
Registering protocol [182] "TFA Drop Rain Gauge 30.3233.01"
Registering protocol [183] "TFA pool temperature sensor"
Registering protocol [184] "TFA-Twin-Plus-30.3049, Conrad KW9010, Ea2 BL999"
Registering protocol [185] "Thermopro TP11 Thermometer"
Registering protocol [186] "ThermoPro TP08/TP12/TP20 thermometer"
Registering protocol [187] "ThermoPro-TX2 temperature sensor"
Registering protocol [188] "ThermoPro TX-2C Thermometer and Humidity sensor"
Registering protocol [189] "Thermor DG950 weather station"
Registering protocol [190] "Thermor A6N 132TX temperature sensor"
Registering protocol [191] "EezTire E618, Carchet TPMS, TST-507 TPMS"
Registering protocol [192] "Gear Hive TPMS sensor"
Registering protocol [193] "GM-Aftermarket TPMS"
Registering protocol [194] "iMars T240 TPMS"
Registering protocol [195] "Jansite TPMS TY-468-eu2 / KKMOON TPMS"
Registering protocol [196] "Schrader Motorcycle TPMS sensor"
Registering protocol [197] "SmarTire TPMS sensor, Aston Martin/Vantage DB9 protocol"
Registering protocol [198] "TRW TPMS OOK OEM and Clone models"
Registering protocol [199] "TyreGuard 400 TPMS"
Registering protocol [200] "TR-502MSV remote smart socket controller"
Registering protocol [201] "TS-FT002 Wireless Ultrasonic Tank Liquid Level Meter With Temperature Sensor"
Registering protocol [202] "Emos TTX201 Temperature Sensor"
Registering protocol [203] "2GIG-KEY2E-345 encrypted keyfob"
Registering protocol [204] "Universal (Reverseable) 24V Fan Controller"
Registering protocol [205] "Vaillant calorMatic VRT340f Central Heating Control"
Registering protocol [206] "Vauno EN8822C"
Registering protocol [207] "Visonic powercode"
Registering protocol [208] "Vivint Door/Window Sensor, V-DW21R-345/V-DW11-345"
Registering protocol [209] "WallarGe CLTX001 Outdoor Temperature Sensor"
Registering protocol [210] "Kingspan/Watchman Plus (Niveau) oil tank monitor"
Registering protocol [211] "Watts WFHT-RF Thermostat"
Registering protocol [212] "Watts WFHT-RF / WFHC-MASTERH&C-RF underfloor heating thermostat"
Registering protocol [213] "Waveman Switch Transmitter"
Registering protocol [214] "WEC-2103 temperature/humidity sensor"
Registering protocol [215] "WG-PB12V1 Temperature Sensor"
Registering protocol [216] "WS2032 weather station"
Registering protocol [217] "Hyundai WS SENZOR Remote Temperature Sensor"
Registering protocol [218] "WT0124 Pool Thermometer"
Registering protocol [219] "WT450, WT260H, WT405H"
Registering protocol [220] "X10 RF"
Registering protocol [221] "X10 Security"
Registering protocol [222] "Yale HSA (Home Security Alarm), YES-Alarmkit"
```

The current OOK registration contains 223 protocol decoders.

### FSK Signal Device Decoders

```plaintext
Registering protocol [0] "Alps FWB1U545 Car Remote"
Registering protocol [1] "Ambient Weather WH31E Thermo-Hygrometer Sensor, EcoWitt WH40/WN20 rain gauge, WS68 weather station"
Registering protocol [2] "ANT and ANT+ devices"
Registering protocol [3] "Apator Metra E-ITN 30 heat cost allocator"
Registering protocol [4] "Apator Metra E-RM 30 water meter"
Registering protocol [5] "Arad/Master Meter Dialog3G water utility meter"
Registering protocol [6] "TBH weather sensor"
Registering protocol [7] "Arexx Multilogger IP-HA90, IP-TH78EXT, TSN-70E"
Registering protocol [8] "Orion Endpoint from Badger Meter, GIF2014W-OSE, water meter, hopping from 904.4 Mhz to 924.6Mhz (-s 1600k)"
Registering protocol [9] "Orion Endpoint from Badger Meter, GIF2020OCECNA, water meter, hopping from 904.4 Mhz to 924.6Mhz (-s 1600k)"
Registering protocol [10] "Badger ORION water meter, 100kbps (-f 916.45M -s 1200k)"
Registering protocol [11] "Bresser Weather Center 5-in-1"
Registering protocol [12] "Bresser Weather Center 6-in-1, 7-in-1 indoor, soil, new 5-in-1, 3-in-1 wind gauge, Froggit WH6000, Ventus C8488A"
Registering protocol [13] "Bresser Weather Center 7-in-1, Air Quality PM2.5/PM10 7009970, CO2 7009977, HCHO/VOC 7009978 sensors"
Registering protocol [14] "Bresser SmartHome Garden soil moisture and water timer valve (Baldr Homgar, RainPoint)"
Registering protocol [15] "Bresser water leakage"
Registering protocol [16] "Bresser lightning"
Registering protocol [17] "Cavius smoke, heat and water detector"
Registering protocol [18] "Cavius Door/Window sensor (-f 868.7M)"
Registering protocol [19] "CED7000 Shot Timer"
Registering protocol [20] "Chamberlain CWPIRC PIR Sensor"
Registering protocol [21] "Continental KR5V2X Car Remote (-f 313.8M -s 1024k)"
Registering protocol [22] "Honda Keyfob KR5V2X/1X (-f 433.6M -s 1024k)"
Registering protocol [23] "Cellular Tracking Technologies LifeTag/PowerTag/HybridTag"
Registering protocol [24] "CurrentCost Current Sensor"
Registering protocol [25] "Danfoss CFR Thermostat"
Registering protocol [26] "DeltaDore X3D devices"
Registering protocol [27] "DirecTV RC66RX Remote Control"
Registering protocol [28] "Eberle Instat 868r1 floor heating thermostat remote"
Registering protocol [29] "Voltcraft EnergyCount 3000 (ec3k)"
Registering protocol [30] "EcoDHOME Smart Socket and MCEE Solar monitor"
Registering protocol [31] "Eco-Eye solar PV/grid current monitor"
Registering protocol [32] "Efergy e2 classic"
Registering protocol [33] "Efergy Optical"
Registering protocol [34] "Elero bidirectional blinds/awning remote (Silent Gliss and others)"
Registering protocol [35] "Elsner Solexa 230V"
Registering protocol [36] "Elster/Honeywell R2S/REXU power meter"
Registering protocol [37] "Elster/Honeywell R2S/REXU power meter, type-2 frames"
Registering protocol [38] "Emax W6, rebrand Altronics x7063/4/x7064A, Optex 990040/50/51, Orium 13093/13123, Infactory FWS-1200, Newentor Q9, Otio 810025, Protmex PT3390A, Jula Marquant 014331/32, TechniSat IMETEO X6 76-4924-00, Weather Station or temperature/humidity sensor"
Registering protocol [39] "emonTx OpenEnergyMonitor"
Registering protocol [40] "EN2058 four probe temperature sensor"
Registering protocol [41] "ESIC EMT7110 power meter"
Registering protocol [42] "Fine Offset Electronics, WH25, WH32, WH32B, WN32B, WH24, WH65, WS69, HP1000, Misol WS2320 Temperature/Humidity/Pressure Sensor"
Registering protocol [43] "Fine Offset Electronics/Ecowitt WH51, WN31, SwitchDoc Labs SM23 Soil Moisture Sensor"
Registering protocol [44] "TFA 30.3151 Weather Station"
Registering protocol [45] "Fine Offset Electronics WH1080/WH3080 Weather Station (FSK)"
Registering protocol [46] "Ambient Weather WH31L (FineOffset WH57) Lightning-Strike sensor"
Registering protocol [47] "Fine Offset Electronics WH43 air quality sensor"
Registering protocol [48] "Fine Offset Electronics WH45 air quality sensor"
Registering protocol [49] "Fine Offset Electronics WH46 air quality sensor"
Registering protocol [50] "Fine Offset Electronics / Ecowitt WH52 Soil Moisture/Temperature/EC Sensor"
Registering protocol [51] "Fine Offset / Ecowitt WH55 water leak sensor"
Registering protocol [52] "Fine Offset Electronics WN34S/L/D, WN38 and Froggit DP150/D35 temperature sensor"
Registering protocol [53] "Fine Offset Electronics WS80 weather station"
Registering protocol [54] "Fine Offset Electronics WS85 weather station"
Registering protocol [55] "Fine Offset Electronics WS90 weather station"
Registering protocol [56] "Flowis flow meters"
Registering protocol [57] "FSL Cricket Scoreboard Controller"
Registering protocol [58] "GE Color Effects"
Registering protocol [59] "GEO minim+ energy monitor"
Registering protocol [60] "Govee Water Leak Detector H5059"
Registering protocol [61] "Govee H5112 Dual-Probe Thermometer"
Registering protocol [62] "Govee Pool/Spa Thermometer H5310"
Registering protocol [63] "Landis & Gyr Gridstream Power Meters 9.6k"
Registering protocol [64] "Landis & Gyr Gridstream Power Meters 19.2k"
Registering protocol [65] "Landis & Gyr Gridstream Power Meters 38.4k"
Registering protocol [66] "Hanwell ML/RL4000-series Radiologger temperature/humidity sensor"
Registering protocol [67] "Microchip HCS200/HCS300 KeeLoq Hopping Encoder based remotes (FSK)"
Registering protocol [68] "AOK Weather Station rebrand Holman Industries iWeather WS5029, Conrad AOK-5056, Optex 990018"
Registering protocol [69] "Holman Industries iWeather WS5029 weather station (older PWM)"
Registering protocol [70] "Honda Car Key"
Registering protocol [71] "Honeywell CM921 Wireless Programmable Room Thermostat"
Registering protocol [72] "Honeywell ActivLink, Wireless Doorbell (FSK)"
Registering protocol [73] "IKEA Sparsnas Energy Meter Monitor"
Registering protocol [74] "Inkbird ITH-20R temperature humidity sensor"
Registering protocol [75] "Insteon"
Registering protocol [76] "LaCrosse Technology View LTV-WSDTH01 Breeze Pro Wind Sensor"
Registering protocol [77] "LaCrosse Technology View LTV-R1, LTV-R3 Rainfall Gauge, LTV-W1/W2 Wind Sensor"
Registering protocol [78] "LaCrosse Technology View LTV-TH Thermo/Hygro Sensor"
Registering protocol [79] "LaCrosse Technology TX22U-IT"
Registering protocol [80] "LaCrosse TX31U-IT, The Weather Channel WS-1910TWC-IT"
Registering protocol [81] "LaCrosse TX34-IT rain gauge"
Registering protocol [82] "LaCrosse TX29IT, TFA Dostmann 30.3159.IT Temperature sensor"
Registering protocol [83] "LaCrosse TX35DTH-IT, TFA Dostmann 30.3155 Temperature/Humidity sensor"
Registering protocol [84] "LaCrosse Technology View LTV-WR1 Multi Sensor"
Registering protocol [85] "LaCrosse WS6868 TX232TH-LCD temperature/humidity sensor"
Registering protocol [86] "LaCrosse WS6868 TX231RW wind/rain sensor"
Registering protocol [87] "Wireless M-Bus, Mode C&T, 100kbps (-f 868.95M -s 1200k)"
Registering protocol [88] "Wireless M-Bus, Mode T, 32.768kbps (-f 868.3M -s 1000k)"
Registering protocol [89] "Wireless M-Bus, Mode S, 32.768kbps (-f 868.3M -s 1000k)"
Registering protocol [90] "Wireless M-Bus, Mode R, 4.8kbps (-f 868.33M)"
Registering protocol [91] "Wireless M-Bus, Mode F, 2.4kbps"
Registering protocol [92] "RADIAN/RADIAN0 meter"
Registering protocol [93] "Marlec Solar iBoost+ sensors"
Registering protocol [94] "Maverick XR-30 BBQ Sensor"
Registering protocol [95] "Maverick XR-50 BBQ Sensor"
Registering protocol [96] "McPower Kinetic battery-less wall switch"
Registering protocol [97] "Mueller Hot Rod water meter"
Registering protocol [98] "NetAtmo temp/hum and wind sensors"
Registering protocol [99] "Nidec Car Remote (-f 313.8M -s 1024k)"
Registering protocol [100] "Oil Ultrasonic SMART FSK"
Registering protocol [101] "Oil Ultrasonic STANDARD FSK"
Registering protocol [102] "Watchman Sonic / Apollo Ultrasonic / Beckett Rocket oil tank monitor"
Registering protocol [103] "Watchman Sonic Advanced / Plus, Tekelek"
Registering protocol [104] "Opel Mokka Car Key"
Registering protocol [105] "Oregon Scientific WMR500 weather station"
Registering protocol [106] "Quinetic"
Registering protocol [107] "RFM69 LowPowerLab Moteino board (-s 1000k)"
Registering protocol [108] "RojaFlex shutter and remote devices"
Registering protocol [109] "Sharp SPC775 weather station"
Registering protocol [110] "Silver Spring Networks mesh endpoint (-s 1600k)"
Registering protocol [111] "SimpliSafe Gen 3 Home Security System"
Registering protocol [112] "Somfy io-homecontrol"
Registering protocol [113] "SRSmith Pool Light Remote Control SRS-2C-TX (-f 915M)"
Registering protocol [114] "Steelmate TPMS"
Registering protocol [115] "TFA Dostmann 14.1504.V2 Radio-controlled grill and meat thermometer"
Registering protocol [116] "TFA Dostmann 30.3196 T/H outdoor sensor"
Registering protocol [117] "TFA Dostmann 30.390X T/H sensors series"
Registering protocol [118] "TFA Marbella Pool Thermometer"
Registering protocol [119] "ThermoPro TP211B Thermometer"
Registering protocol [120] "ThermoPro TP28b Super Long Range Wireless Meat Thermometer for Smoker BBQ Grill"
Registering protocol [121] "ThermoPro Meat Thermometers, TP828B 2 probes with Temp, BBQ Target LO and HI"
Registering protocol [122] "ThermoPro Meat Thermometers, TP829B 4 probes with temp only"
Registering protocol [123] "ThermoPro TempSpike XR TP862b / TP863b Wireless Dual-Probe Meat Thermometer"
Registering protocol [124] "ThermoPro TX-7B Outdoor Thermometer Hygrometer"
Registering protocol [125] "Abarth 124 Spider and Shenzhen EGQ Q85 TPMS"
Registering protocol [126] "Airpuxem TPMS TYH11_EU6_ZQ"
Registering protocol [127] "AVE TPMS"
Registering protocol [128] "BMW Gen4-Gen5 TPMS and Audi TPMS Pressure Alert, multi-brand HUF/Beru, Continental, Schrader/Sensata, Audi"
Registering protocol [129] "BMW Gen2 and Gen3 TPMS"
Registering protocol [130] "Citroen TPMS"
Registering protocol [131] "Elantra2012 TPMS"
Registering protocol [132] "Ford TPMS"
Registering protocol [133] "Honda (TRW PPA-GF33) TPMS"
Registering protocol [134] "Hyundai TPMS (VDO)"
Registering protocol [135] "Jansite TPMS Model TY02S"
Registering protocol [136] "Jansite TPMS Model Solar"
Registering protocol [137] "Jansite TPMS TY588-EU2"
Registering protocol [138] "Jeep TPMS"
Registering protocol [139] "Kia TPMS (-s 1000k)"
Registering protocol [140] "Mercedes Benz Sprinter 4500 TPMS sensor"
Registering protocol [141] "Nissan TPMS"
Registering protocol [142] "PMV-107J (Toyota) TPMS"
Registering protocol [143] "Porsche Boxster/Cayman TPMS"
Registering protocol [144] "Renault TPMS"
Registering protocol [145] "Renault 0435R TPMS"
Registering protocol [146] "Sefis M3 / Careud / Sykik SRTP300 TPMS"
Registering protocol [147] "Toyota TPMS"
Registering protocol [148] "Unbranded SolarTPMS for trucks"
Registering protocol [149] "TRW TPMS FSK OEM and Clone models"
Registering protocol [150] "Typhur Sync Gold meat thermometer probe"
Registering protocol [151] "Vevor Wireless Weather Station 7-in-1"
Registering protocol [152] "Watts Vision thermostat (-f 868.3M)"
```

The current FSK registration contains 153 protocol decoders.
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

## Frequency Configuration

The library supports multiple frequency bands including 433 MHz, 868 MHz, and 915 MHz. The receive frequency can be configured using the `RF_MODULE_FREQUENCY` build flag in your `platformio.ini` file.

### Supported Frequencies

The transceiver modules (CC1101, SX1276, SX1278) support multiple frequency bands depending on the hardware variant:

* **433 MHz** - Default frequency (433.92 MHz), commonly used in Asia and some other regions
* **868 MHz** - European ISM band (e.g., 868.30 MHz for many European sensors)
* **915 MHz** - ISM band used in North America, Australia, and some other regions (915.00 MHz)

### Configuration

To set a custom frequency, add the `RF_MODULE_FREQUENCY` build flag to your environment in `platformio.ini`:

```ini
build_flags = 
  '-DRF_MODULE_FREQUENCY=868.30'  ; Set frequency to 868.30 MHz
```

**Examples:**

```ini
'-DRF_MODULE_FREQUENCY=433.92'  ; 433 MHz (default)
'-DRF_MODULE_FREQUENCY=868.30'  ; 868 MHz (common for EU sensors)
'-DRF_MODULE_FREQUENCY=915.00'  ; 915 MHz (US ISM band)
```

**Note:** Make sure your hardware supports the frequency band you want to use. Many LoRa modules are available in different frequency variants (e.g., 433 MHz, 868 MHz, 915 MHz versions). The frequency must match your hardware's capabilities and the sensors you want to receive.

For a complete example, see the `esp32_heltec_915` environment in [examples/OOK_Receiver/platformio.ini](examples/OOK_Receiver/platformio.ini#L163).

## Callbacks

The library supports two callback interfaces:

* `setCallback(...)` - receives JSON payloads for successfully decoded signals.
* `setRawPulsesCallback(...)` - receives raw pulse/gap timing for every captured signal, including signals that do not match a built-in decoder.

```cpp
typedef void (*rtl_433_raw_pulse_cb)(
    const int* pulse_us,
    const int* gap_us,
    unsigned int num_pulses,
    unsigned long duration_us,
    int rssi
);
```

Registering callbacks:

```cpp
rf.setCallback(rtl_433_Callback, messageBuffer, JSON_MSG_BUFFER);
rf.setRawPulsesCallback(rtl_433_RawCallback);
```

Raw pulse pointers are valid only during the callback. Copy any data you need to keep.

## Wiring and Building the Example

Details are [here](examples/OOK_Receiver/README.md)

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

The RSSI Threshold for signal detection is automatically determined based on the average RSSI signal level received aka RSSI floor level with a delta ( RSSI_THRESHOLD ) added to it. At startup, reception remains disabled while the first `RSSI_INITIAL_SAMPLES` readings are collected, preventing the initial `MINRSSI` value from creating one long false signal. The resulting floor and threshold are logged when reception is enabled. Subsequent averages use `RSSI_SAMPLES`.

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
RF_MODULE_FREQUENCY   ; Set receive frequency in MHz (e.g., 433.92, 868.30, 915.00), defaults to 433.92
RSSI_SAMPLES          ; Number of rssi samples to collect for average calculation, defaults to 50,000
RSSI_INITIAL_SAMPLES  ; Number of startup RSSI samples collected before reception is enabled, defaults to 1,000
RSSI_THRESHOLD        ; Delta applied to average RSSI value to calculate RSSI Signal Threshold, defaults to 9
CC1101_RX_BANDWIDTH   ; CC1101 receive bandwidth in kHz, defaults to 812.0 (812.5 kHz hardware setting)
CC1101_AGCCTRL2       ; CC1101 AGCCTRL2 register value, defaults to 0xC7
CC1101_AGCCTRL1       ; CC1101 AGCCTRL1 register value, defaults to 0x40
CC1101_AGCCTRL0       ; CC1101 AGCCTRL0 register value, defaults to 0xB2
RTL_DEBUG             ; Enable RTL_433 device decoder verbose mode for all device decoders ( 0=normal, 1=verbose, 2=verbose decoders, 3=debug decoders, 4=trace decoding. )
RTL_VERBOSE=##        ; Enable RTL_433 device decoder verbose mode, ## is the decoder # from the appropriate memcpy line in signalDecoder.cpp
VIVINT_SEEDS="…"       ; Optional comma-separated Vivint TXID=hexseed list; omit to discover seeds automatically
OUTPUT_VIVINT_DECODE    ; Include Vivint seed-discovery fields; defaults to 1, set to 0 to omit
VIVINT_MAX_SENSORS      ; Maximum number of tracked Vivint sensors; defaults to 32
VIVINT_CACHED_COUNTERS  ; Number of recent seed-discovery samples retained per sensor; defaults to 8
VIVINT_SEED_DATA_REQUIRED ; Samples required before attempting seed discovery; defaults to 6
RTL_ANALYZER          ; Enable pulse stream analysis ( note is very resource intensive and will not work with other modules )
RTL_ANALYZE=##        ; Enable pulse stream analysis for decoder ##
SIGNAL_RSSI           ; Enable collection of per pulse RSSI Values during signal reception for display in signal debug messages
RF_MODULE_INIT_STATUS ; Display transceiver config during startup
DISABLERSSITHRESHOLD  ; Disable automatic setting of RSSI_THRESHOLD ( legacy behaviour ), and use MINRSSI ( -82 )
OOK_MODULATION        ; Enable OOK Device Decoders, setting to false enables FSK Device Decoders 
```

`DISABLERSSITHRESHOLD` does not disable the RSSI gate. It fixes the gate at
`MINRSSI` (default `-82` dBm). If the receiver reports RSSI values below that
level, no signal will pass the gate and the library may appear silent. Override
`MINRSSI` with a value below the measured idle RSSI, or leave automatic RSSI
threshold selection enabled.

When no signals are reported, first verify radio initialization and register
readback, then observe the idle RSSI and confirm the configured threshold is
below the RSSI reached during a transmission. If the radio's data pin toggles
but the library reports no events, also check `MINIMUM_PULSE_LENGTH` and
`MINIMUM_SIGNAL_LENGTH` against the protocol being received.

CC1101 OOK performance can depend on the receiver module, local noise, and
transmitter frequency error. The bandwidth and AGC registers can be overridden
without modifying the library source. The defaults remain the broad rtl_433_ESP
profile for compatibility.

### Vivint CC1101 tuning results

Testing 345 MHz Vivint sensors at one installation showed that a narrower
162.5 kHz receive bandwidth with `AGCCTRL2=0x84`, `AGCCTRL1=0x40`, and
`AGCCTRL0=0xA0` reduced zero-decode signals and produced more complete signal
captures than the broad default profile. Use these values as a starting point,
not a universal preset: antenna, module crystal tolerance, interference, and
sensor frequency offset can change the best result. The dedicated
[CC1101 tuning example](examples/cc1101_tuning/README.md) can run the search and
comparison suites unattended to validate the profile locally. The complete
method and results are recorded in
[CC1101 Vivint tuning results](docs/CC1101_VIVINT_TUNING_RESULTS.md).

```ini
build_flags =
  -DCC1101_RX_BANDWIDTH=162.5
  -DCC1101_AGCCTRL2=0x84
  -DCC1101_AGCCTRL1=0x40
  -DCC1101_AGCCTRL0=0xA0
```

`LOG_LEVEL` also controls the library's `logprintf` and `alogprintf` output.
Set `-DLOG_LEVEL=0` (ArduinoLog's silent level) to suppress that diagnostic
output completely.

## ESP32 platform support

The decoder task runs on core 0 for single-core ESP32 targets and core 1 for
dual-core targets. ESP32-C5 custom SPI pin configurations use `FSPI`, matching
the ESP32-C3 and ESP32-S3 configuration. PlatformIO support for the C5 depends
on the selected Espressif platform and Arduino core; the library does not
require a repository-wide compiler-warning override or custom linker flags.

### Vivint seed configuration

rtl_433_ESP automatically discovers a Vivint sensor's 16-bit seed after
collecting enough packets with distinct counters. A known seed can instead be
provided at compile time to decrypt packets immediately:

```ini
build_flags =
  '-DVIVINT_SEEDS="0016-0357157=e4d8,0016-0345744=1e35,0016-0357170=c283"'
```

Each entry uses the Vivint `TXID=hexseed` format. Separate multiple sensors
with commas and keep the complete definition single-quoted in `platformio.ini`.
When no matching configured seed is present, automatic discovery remains
enabled.

Seed diagnostics are enabled by default. They report `seed`, `decode_status`,
`seed_data_count`, `seed_data_required`, and `seed_candidate_count`. To omit
these fields from decoded messages, add:

```ini
build_flags =
  '-DOUTPUT_VIVINT_DECODE=0'
```

Seed discovery can be tuned for memory use and confidence. The required sample
count must not exceed the cache size:

```ini
build_flags =
  '-DVIVINT_MAX_SENSORS=32'
  '-DVIVINT_CACHED_COUNTERS=8'
  '-DVIVINT_SEED_DATA_REQUIRED=6'
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
