
// These examples are from the Electronics Cookbook by Simon Monk
//https://github.com/LSatan/SmartRC-CC1101-Driver-Lib
// mod by Little_S@tan

// WH51 FSK Sample

/*[00] {177} 55 55 55 55 55 51 6e a2 88 06 50 e8 8b f8 07 c0 a7 ff ff fb 9a 20 80

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
*/

/** @fn int prologue_callback(r_device *decoder, bitbuffer_t *bitbuffer)
Prologue sensor protocol,
also FreeTec NC-7104 sensor for FreeTec Weatherstation NC-7102,
and Pearl NC-7159-675.
The sensor can be bought at Clas Ohlson.

The sensor sends 36 bits 7 times, before the first packet there is a sync pulse.
The packets are ppm modulated (distance coding) with a pulse of ~500 us
followed by a short gap of ~2000 us for a 0 bit or a long ~4000 us gap for a
1 bit, the sync gap is ~9000 us.

The data is grouped in 9 nibbles

    [type] [id0] [id1] [flags] [temp0] [temp1] [temp2] [humi0] [humi1]

- type: 4 bit fixed 1001 (9) or 0110 (5)
- id: 8 bit a random id that is generated when the sensor starts, could include battery status
  the same batteries often generate the same id
- flags(3): is 0 when the battery is low, otherwise 1 (ok), first reading always says low
- flags(2): is 1 when the sensor sends a reading when pressing the button on the sensor
- flags(1,0): the channel number that can be set by the sensor (1, 2, 3, X)
- temp: 12 bit signed scaled by 10
- humi: 8 bit always 11001100 (0xCC) if no humidity sensor is available

rtl_433 verbose debug

pulse_demod_ppm(): Prologue, FreeTec NC-7104, NC-7159-675 temperature sensor
bitbuffer:: Number of rows: 8 
[00] {38} 15 86 43 24 40 : 00010101 10000110 01000011 00100100 010000
[01] {36} 56 19 0c 91 00 : 01010110 00011001 00001100 10010001 0000
[02] {36} 56 19 0c 91 00 : 01010110 00011001 00001100 10010001 0000
[03] {36} 56 19 0c 91 00 : 01010110 00011001 00001100 10010001 0000
[04] {36} 56 19 0c 91 00 : 01010110 00011001 00001100 10010001 0000
[05] {36} 56 19 0c 91 00 : 01010110 00011001 00001100 10010001 0000
[06] {36} 56 19 0c 91 00 : 01010110 00011001 00001100 10010001 0000
[07] { 0}  

5 61 90 C9 10

*/

#include <Arduino.h>
#include "ELECHOUSE_CC1101_SRC_DRV.h"

int gdo0;

#ifndef SERIAL_BAUD
#define SERIAL_BAUD 921600
#endif

const int fsk_len = 17;
byte fsk_buffer[fsk_len] = {0x51, 0x00, 0x6B, 0x58, 0x6E, 0x7F, 0x24, 0xF8, 0xD2, 0xFF, 0xFF, 0xFF, 0x3C, 0x28, 0x80, 0x00, 0x00};

void setup()
{

#ifdef ESP32
  gdo0 = CC1101_GDO0; // for esp32! GDO0 on GPIO pin 2.
#define ONBOARD_LED 2
#elif ESP8266
  gdo0 = 5; // for esp8266! GDO0 on pin 5 = D1.
#else
  gdo0 = 6; // for Arduino! GDO0 on pin 6.
#endif

  Serial.begin(SERIAL_BAUD);
  delay(1000);
  Serial.println("Sample ASK/OOK and FSK Transmitter");

  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, LOW);

  if (ELECHOUSE_cc1101.getCC1101())
  { // Check the CC1101 Spi connection.
    Serial.println("Connection OK");
  }
  else
  {
    Serial.println("Connection Error");
  }

  ELECHOUSE_cc1101.Init(); // must be set to initialize the cc1101!
  Serial.println("Tx Mode");
}

void loop()
{
  ELECHOUSE_cc1101.SetTx();
  digitalWrite(ONBOARD_LED, HIGH);
  ELECHOUSE_cc1101.SendData(fsk_buffer, fsk_len, 1000);
  Serial.print(millis()/1000);
  Serial.print(" FSK Buffer: (");
  Serial.print(fsk_len);
  Serial.print(") ");
  for (int i = 0; i < fsk_len; i++)
  {
    Serial.print(fsk_buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
  ELECHOUSE_cc1101.SetRx();
  digitalWrite(ONBOARD_LED, LOW);
  delay(2000); // 15 seconds
  // ASK/OOK Transmit
  
}