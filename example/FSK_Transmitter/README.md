Sample program to create a FSK signal to test decoder logic and settings.  Based on WH51 Soil Moisture Sensor Signal

Sends a FSK packet at the rate of about 1 every second or so

This is what it looks like in the receiver

```
N: Received message : {"model":"Fineoffset-WH51","id":"006b58","battery_ok":0.77778,"battery_mV":1400,"moisture":36,"boost":3,"ad_raw":210,"mic":"CRC","protocol":"Fine Offset Electronics/ECOWITT WH51, SwitchDoc Labs SM23 Soil Moisture Sensor","rssi":-74,"duration":11000}
```

Original WH51 signal via rtl_433

time : 2021-04-06 13:21:09
model : Fineoffset-WH51 ID : 00ca1d
Battery level: 1.111 Battery : 1700 mV Moisture : 0 % Transmission boost: 0 AD raw : 20 Integrity : CRC
Analyzing pulses...
Total count: 48, width: 10.26 ms ( 2564 S)
Pulse width distribution:
[ 0] count: 1, width: 0 us [0;0] ( 0 S)
[ 1] count: 38, width: 56 us [56;68] ( 14 S)
[ 2] count: 3, width: 116 us [116;120] ( 29 S)
[ 3] count: 3, width: 172 us [172;176] ( 43 S)
[ 4] count: 1, width: 404 us [404;404] ( 101 S)
[ 5] count: 1, width: 288 us [288;288] ( 72 S)
[ 6] count: 1, width: 1392 us [1392;1392] ( 348 S)
Gap width distribution:
[ 0] count: 33, width: 56 us [48;60] ( 14 S)
[ 1] count: 6, width: 172 us [172;176] ( 43 S)
[ 2] count: 2, width: 464 us [464;468] ( 116 S)
[ 3] count: 3, width: 112 us [112;116] ( 28 S)
[ 4] count: 2, width: 256 us [232;280] ( 64 S)
[ 5] count: 1, width: 348 us [348;348] ( 87 S)
Pulse period distribution:
[ 0] count: 1, width: 48 us [48;48] ( 12 S)
[ 1] count: 27, width: 116 us [116;120] ( 29 S)
[ 2] count: 9, width: 232 us [232;232] ( 58 S)
[ 3] count: 3, width: 172 us [172;176] ( 43 S)
[ 4] count: 2, width: 576 us [520;636] ( 144 S)
[ 5] count: 3, width: 308 us [292;340] ( 77 S)
[ 6] count: 1, width: 872 us [872;872] ( 218 S)
[ 7] count: 1, width: 1448 us [1448;1448] ( 362 S)
Pulse timing distribution:
[ 0] count: 1, width: 0 us [0;0] ( 0 S)
[ 1] count: 71, width: 56 us [48;68] ( 14 S)
[ 2] count: 6, width: 116 us [112;120] ( 29 S)
[ 3] count: 9, width: 172 us [172;176] ( 43 S)
[ 4] count: 3, width: 444 us [404;468] ( 111 S)
[ 5] count: 3, width: 264 us [232;288] ( 66 S)
[ 6] count: 1, width: 1392 us [1392;1392] ( 348 S)
[ 7] count: 1, width: 348 us [348;348] ( 87 S)
[ 8] count: 1, width: 0 us [0;0] ( 0 S)
Level estimates [high, low]: 15873, 7
RSSI: -0.1 dB SNR: 33.6 dB Noise: -33.7 dB
Frequency offsets [F1, F2]: 7357, -7395 (+28.1 kHz, -28.2 kHz)
Guessing modulation: Pulse Code Modulation (Not Return to Zero)
Attempting demodulation... short_width: 56, long_width: 56, reset_limit: 57344, sync_width: 0
Use a flex decoder with -X 'n=name,m=FSK_PCM,s=56,l=56,r=57344'
pulse_demod_pcm(): Analyzer Device
bitbuffer:: Number of rows: 1
[00] {177} 55 55 55 55 55 51 6e a2 88 06 50 e8 8b f8 07 c0 a7 ff ff fb 9a 20 80

Noting that the bit buffer seems to be out of sync by 1 bit. 55 <<1 = AA (Sync),
55 55 55 55 5 = AA AA AA AA A - sync
5 51 6e a = AA 2D D4 - pre
etc

Given that this actually has a well defined packet
Data format:
00 01 02 03 04 05 06 07 08 09 10 11 12 13
aa aa aa 2d d4 51 00 6b 58 6e 7f 24 f8 d2 ff ff ff 3c 28 8
FF II II II TB YY MM ZA AA XX XX XX CC SS
Sync: aa aa aa ...
Preamble: 2d d4 - actually preamble[] = {0xAA, 0x2D, 0xD4};
FF: Family code 0x51 (ECOWITT/FineOffset WH51)
IIIIII: ID (3 bytes)
T: Transmission period boost: highest 3 bits set to 111 on moisture change and decremented each transmission;
if T = 0 period is 70 sec, if T > 0 period is 10 sec
B: Battery voltage: lowest 5 bits are battery voltage * 10 (e.g. 0x0c = 12 = 1.2V). Transmitter works down to 0.7V (0x07)
YY: ? Fixed: 0x7f
MM: Moisture percentage 0%-100% (0x00-0x64) MM = (AD - 70) / (450 - 70)
Z: ? Fixed: leftmost 7 bit 1111 100
AAA: 9 bit AD value MSB byte[07] & 0x01, LSB byte[08]
XXXXXX: ? Fixed: 0xff 0xff 0xff
CC: CRC of the preceding 12 bytes (Polynomial 0x31, Initial value 0x00, Input not reflected, Result not reflected)
SS: Sum of the preceding 13 bytes % 256

It should be simple enough to set up the parameters in the normal packet TX code. I will do it tonight - I can verify against RTL_433 using the SDR, the Misol receiver and ESP.
Hopefully, the transmitter (WH51) will be stable enough clock wise to use Packet Rx mode on the CC101. I have not tried to date but wanted to start with the RTL_433 approach.


## Tks to @AusGunno
