/*
   RadioLib CC1101 Settings Example

   This example shows how to change all the properties of RF69 radio.
   RadioLib currently supports the following settings:
    - pins (SPI slave select, digital IO 0, digital IO 1)
    - carrier frequency
    - bit rate
    - receiver bandwidth
    - allowed frequency deviation
    - output power during transmission
    - sync word

    For default module settings, see the wiki page
    https://github.com/jgromes/RadioLib/wiki/Default-configuration#cc1101

    For full API reference, see the GitHub Pages
    https://jgromes.github.io/RadioLib/
*/

// ------------------------------------------------------------------------------------------------

#define LOG_EMERG 0
#define LOG_ALERT 1
#define LOG_CRIT 2
#define LOG_ERR 3
#define LOG_WARNING 4
#define LOG_NOTICE 5
#define LOG_INFO 6
#define LOG_DEBUG 7

#define LOG_STACK 255

#include <stdio.h>

#define logprintf(prio, args...)       \
  {                                    \
    printf("rtl_433_ESP(%d): ", prio); \
    printf(args);                      \
  }
#define logprintfLn(prio, args...)     \
  {                                    \
    printf("rtl_433_ESP(%d): ", prio); \
    printf(args);                      \
    printf("\n");                      \
  }
#define alogprintf(prio, args...) \
  {                               \
    printf(args);                 \
  }
#define alogprintfLn(prio, args...) \
  {                                 \
    printf(args);                   \
    printf("\n");                   \
  }

// ------------------------------------------------------------------------------------------------

// include the library

#define RADIOLIB_LOW_LEVEL
#define RADIOLIB_DEBUG
#define RADIOLIB_VERBOSE

#include <RadioLib.h>

// CC1101 has the following connections:
// CS pin:    10
// GDO0 pin:  2
// RST pin:   unused
// GDO2 pin:  3 (optional)

#define CC1101_CSN 5
#define CC1101_GDO2 4  // Receive
#define CC1101_GDO0 22 // Transmit

#define VSPI_MISO 19
#define VSPI_MOSI 23
#define VSPI_SCLK 18
#define VSPI_SS CC1101_CSN // NRF24_CE( I tried swapping these too - no joy)

// SPIClass newSPI(VSPI);
// RadioA radioA = new Module(CC1101_CSN, CC1101_GDO0, RADIOLIB_NC, CC1101_GDO2, newSPI);

CC1101 radio = new Module(CC1101_CSN, CC1101_GDO2, RADIOLIB_NC, CC1101_GDO0);

// or using RadioShield
// https://github.com/jgromes/RadioShield
// CC1101 radio3 = RadioShield.ModuleB;

// ESP32 doesn't define ICACHE_RAM_ATTR
#ifndef ICACHE_RAM_ATTR
#define ICACHE_RAM_ATTR IRAM_ATTR
#endif

#ifndef FREQUENCY
#define FREQUENCY 433.92
#endif

int receiverGpio = CC1101_GDO2;

void ICACHE_RAM_ATTR interruptHandler()
{

  if (!digitalRead(receiverGpio))
  {
    digitalWrite(ONBOARD_LED, LOW);
  }
  else
  {
    digitalWrite(ONBOARD_LED, HIGH);
  }
}

void setup()
{
  Serial.begin(921600);
  Serial.println(F("[CC1101] ... "));
  Serial.println(F("[CC1101] ... "));
  // initialize CC1101 with default settings
  Serial.print(F("[CC1101] Initializing ... "));
  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("success!"));
  }
  else
  {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true)
      ;
  }

  state = radio.setOOK(true);
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("setOOK - success!"));
  }
  else
  {
    Serial.print(F("setOOK failed, code "));
    Serial.println(state);
    while (true)
      ;
  }

  state = radio.receiveDirectAsync();
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("receiveDirect - success!"));
  }
  else
  {
    Serial.print(F("receiveDirect failed, code "));
    Serial.println(state);
    while (true)
      ;
  }

  state = radio.setCrcFiltering(false);

  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("setCrcFiltering - success!"));
  }
  else
  {
    Serial.print(F("setCrcFiltering failed, code "));
    Serial.println(state);
    while (true)
      ;
  }

  elechouseSetup();

  state = radio.setOOK(true);
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("setOOK - success!"));
  }
  else
  {
    Serial.print(F("setOOK failed, code "));
    Serial.println(state);
    while (true)
      ;
  }

  // -----------------------------------------------

  // radio.setDirectAction(interruptHandler);

  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, LOW);

  getCC1101Status();
  alogprintfLn(LOG_INFO, "Signal %d dbm", getRSSI());
}

int minimumRssi = -65;
int signalRssi;
int currentRssi;
boolean receiveMode = false;
unsigned long signalStart = micros();
unsigned long signalEnd;
unsigned long _lastChange;
int totalRssi = 0;
int rssiCount = 0;

int previousRssi = getRSSI();

void loop()
{
  
  currentRssi = getRSSI();
//  alogprintfLn(LOG_INFO, "%d ", currentRssi);
  // alogprintf(LOG_INFO, ".");

  rssiCount++;
  totalRssi += currentRssi;

  if (rssiCount > 50000)
  {
    alogprintfLn(LOG_INFO, "\nAverage Signal %d dbm", totalRssi / rssiCount);
    totalRssi = 0;
    rssiCount = 0;
  }

  if (currentRssi > minimumRssi)
  {
    // alogprintfLn(LOG_INFO, "Signal %d dbm", currentRssi);
    if (!receiveMode)
    {
      alogprintfLn(LOG_INFO, "Average Signal Start %d dbm, count %d", totalRssi / rssiCount, rssiCount);
      totalRssi = 0;
      rssiCount = 0;
      receiveMode = true;
      signalStart = micros();
      // enableReceiver(receiverGpio);
      digitalWrite(ONBOARD_LED, HIGH);
      signalRssi = currentRssi;
      _lastChange = micros();
      // alogprintfLn(LOG_INFO, "Signal start %d dbm", currentRssi);
    }
    signalEnd = micros();
  }
  else
  {
    if (receiveMode)
    {
      alogprintfLn(LOG_INFO, "Average Signal End %d dbm, count %d", totalRssi / rssiCount, rssiCount);
      totalRssi = 0;
      rssiCount = 0;
      digitalWrite(ONBOARD_LED, LOW);
      receiveMode = false;

      unsigned long length = micros() - signalStart;

      if (length > 9999)
      {
        Serial.println();
        Serial.print(F("signalRssi "));
        Serial.print(signalRssi);
        Serial.print(F(" dbm, "));

        Serial.print(F("length "));
        Serial.println(micros() - signalStart);
      }
    }
  }
  previousRssi = currentRssi;
}

void getCC1101Status()
{
  alogprintfLn(LOG_INFO, "----- CC1101 Status -----");
  alogprintfLn(LOG_INFO, "CC1101_MDMCFG1: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_MDMCFG1));
  alogprintfLn(LOG_INFO, "CC1101_MDMCFG2: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_MDMCFG2));
  alogprintfLn(LOG_INFO, "CC1101_MDMCFG3: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_MDMCFG3));
  alogprintfLn(LOG_INFO, "CC1101_MDMCFG4: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_MDMCFG4));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_DEVIATN: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_DEVIATN));
  alogprintfLn(LOG_INFO, "CC1101_AGCCTRL0: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_AGCCTRL0));
  alogprintfLn(LOG_INFO, "CC1101_AGCCTRL1: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_AGCCTRL1));
  alogprintfLn(LOG_INFO, "CC1101_AGCCTRL2: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_AGCCTRL2));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_IOCFG0: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_IOCFG0));
  alogprintfLn(LOG_INFO, "CC1101_IOCFG1: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_IOCFG1));
  alogprintfLn(LOG_INFO, "CC1101_IOCFG2: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_IOCFG2));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_FIFOTHR: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_FIFOTHR));
  alogprintfLn(LOG_INFO, "CC1101_SYNC0: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_SYNC0));
  alogprintfLn(LOG_INFO, "CC1101_SYNC1: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_SYNC1));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_PKTLEN: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_PKTLEN));
  alogprintfLn(LOG_INFO, "CC1101_PKTCTRL0: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_PKTCTRL0));
  alogprintfLn(LOG_INFO, "CC1101_PKTCTRL1: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_PKTCTRL1));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_ADDR: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_ADDR));
  alogprintfLn(LOG_INFO, "CC1101_CHANNR: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_CHANNR));
  alogprintfLn(LOG_INFO, "CC1101_FSCTRL0: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_FSCTRL0));
  alogprintfLn(LOG_INFO, "CC1101_FSCTRL1: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_FSCTRL1));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_FREQ0: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_FREQ0));
  alogprintfLn(LOG_INFO, "CC1101_FREQ1: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_FREQ1));
  alogprintfLn(LOG_INFO, "CC1101_FREQ2: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_FREQ2));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_MCSM0: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_MCSM0));
  alogprintfLn(LOG_INFO, "CC1101_MCSM1: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_MCSM1));
  alogprintfLn(LOG_INFO, "CC1101_MCSM2: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_MCSM2));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_FOCCFG: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_FOCCFG));

  alogprintfLn(LOG_INFO, "CC1101_BSCFG: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_BSCFG));
  alogprintfLn(LOG_INFO, "CC1101_WOREVT0: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_WOREVT0));
  alogprintfLn(LOG_INFO, "CC1101_WOREVT1: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_WOREVT1));
  alogprintfLn(LOG_INFO, "CC1101_WORCTRL: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_WORCTRL));
  alogprintfLn(LOG_INFO, "CC1101_FREND0: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_FREND0));
  alogprintfLn(LOG_INFO, "CC1101_FREND1: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_FREND1));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_FSCAL0: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_FSCAL0));
  alogprintfLn(LOG_INFO, "CC1101_FSCAL1: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_FSCAL1));
  alogprintfLn(LOG_INFO, "CC1101_FSCAL2: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_FSCAL2));
  alogprintfLn(LOG_INFO, "CC1101_FSCAL3: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_FSCAL3));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_RCCTRL0: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_RCCTRL0));
  alogprintfLn(LOG_INFO, "CC1101_RCCTRL1: 0x%.2x", radio.SPIreadRegister(RADIOLIB_CC1101_REG_RCCTRL1));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_PARTNUM: 0x%.2x", radio.SPIgetRegValue(RADIOLIB_CC1101_REG_PARTNUM));
  alogprintfLn(LOG_INFO, "CC1101_VERSION: 0x%.2x", radio.SPIgetRegValue(RADIOLIB_CC1101_REG_VERSION));
  alogprintfLn(LOG_INFO, "CC1101_MARCSTATE: 0x%.2x", radio.SPIgetRegValue(RADIOLIB_CC1101_REG_MARCSTATE));
  alogprintfLn(LOG_INFO, "CC1101_PKTSTATUS: 0x%.2x", radio.SPIgetRegValue(RADIOLIB_CC1101_REG_PKTSTATUS));
  alogprintfLn(LOG_INFO, "CC1101_RXBYTES: 0x%.2x", radio.SPIgetRegValue(RADIOLIB_CC1101_REG_RXBYTES));
  // alogprintfLn(LOG_INFO, "CC1101_STATUS: 0x%.2x", ELECHOUSE_cc1101.getStatus());
  alogprintfLn(LOG_INFO, "----- CC1101 Status -----");
}

void elechouseSetup()
{
  radio.SPIsendCommand(RADIOLIB_CC1101_CMD_RESET);
  radio.SPIsendCommand(RADIOLIB_CC1101_CMD_IDLE);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_FSCTRL1, 0x06);

  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_IOCFG2, 0x0D);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_IOCFG0, 0x0D);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_PKTCTRL0, 0x32);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_MDMCFG3, 0x93);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_MDMCFG4, 7 + 0); // m4RxBw
  radio.setFrequency(FREQUENCY);

  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_MDMCFG1, 0x02);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_MDMCFG0, 0xF8);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_CHANNR, 0); // chan
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_DEVIATN, 0x47);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_FREND1, 0x56);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_MCSM0, 0x18);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_FOCCFG, 0x16);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_BSCFG, 0x1C);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_AGCCTRL2, 0xC7);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_AGCCTRL1, 0x00);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_AGCCTRL0, 0xB2);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_FSCAL3, 0xE9);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_FSCAL2, 0x2A);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_FSCAL1, 0x00);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_FSCAL0, 0x1F);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_FSTEST, 0x59);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_TEST2, 0x81);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_TEST1, 0x35);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_TEST0, 0x09);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_PKTCTRL1, 0x04);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_ADDR, 0x00);
  radio.SPIwriteRegister(RADIOLIB_CC1101_REG_PKTLEN, 0x00);
  radio.SPIsendCommand(RADIOLIB_CC1101_CMD_RX);
}

/****************************************************************
 *FUNCTION NAME:RSSI Level
 *FUNCTION     :Calculating the RSSI Level
 *INPUT        :none
 *OUTPUT       :none
 ****************************************************************/
int getRSSI(void)
{
  int rssi;
  int rawRssi = radio.SPIgetRegValue(RADIOLIB_CC1101_REG_RSSI);
  if (rawRssi >= 128)
  {
    rssi = ((rawRssi - 256) / 2) - 74;
  }
  else
  {
    rssi = (rawRssi / 2) - 74;
  }
  return rssi;
}