/*
  rtl_433_ESP - pilight 433.92 MHz protocols library for Arduino
  Copyright (c) 2016 Puuu.  All right reserved.

  Project home: https://github.com/puuu/rtl_433_ESP/
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 3 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with library. If not, see <http://www.gnu.org/licenses/>
*/

#include <rtl_433_ESP.h>

#define RADIOLIB_LOW_LEVEL

#include <SPI.h>
#include <RadioLib.h>

// SPIClass newSPI(VSPI);
#ifdef RF_SX1276
SX1276 radio = new Module(RF_MODULE_CS, RF_MODULE_IRQ1, RF_MODULE_RST, RF_MODULE_IRQ2, newSPI);
#define STR_MODULE "SX1276"
#endif

#ifdef RF_SX1278
SX1278 radio = new Module(RF_MODULE_CS, RF_MODULE_IRQ1, RF_MODULE_RST, RF_MODULE_IRQ2, newSPI);
#define STR_MODULE "SX1278"
#endif

#ifdef RF_CC1101
CC1101 radio = new Module(RF_MODULE_CS, RF_MODULE_IRQ1, RADIOLIB_NC, RF_MODULE_IRQ2);
#define STR_MODULE "CC1101"
#endif

Module *_mod = radio.getMod();

#include "tools/aprintf.h"
#include "log.h"

// ESP32 doesn't define ICACHE_RAM_ATTR
#ifndef ICACHE_RAM_ATTR
#define ICACHE_RAM_ATTR IRAM_ATTR
#endif

extern "C"
{
#include "bitbuffer.h"
#include "pulse_detect.h"
#include "pulse_demod.h"
#include "list.h"
// #include "rtl_devices.h"
#include "r_api.h"
#include "r_private.h"
#include "rtl_433.h"
#include "rtl_433_devices.h"
#include "fatal.h"
  // #include "decoder.h"
}

/**
 * Is the receiver currently receving a signal
 */
static bool receiveMode = false;

/**
 * Timestamp in micros for start of current signal
 */
static unsigned long signalStart = micros();

/**
 * Timestamp in micros for end of most recent signal aka start of current gap
 */
static unsigned long gapStart = micros();

pulse_data_t *_pulseTrains;

int rtl_433_ESP::messageCount = 0;
int rtl_433_ESP::currentRssi = 0;
int rtl_433_ESP::signalRssi = 0;
int rtl_433_ESP::minimumRssi = MINRSSI;
bool rtl_433_ESP::_enabledReceiver = false;
volatile uint8_t rtl_433_ESP::_actualPulseTrain = 0;
uint8_t rtl_433_ESP::_avaiablePulseTrain = 0;
volatile unsigned long rtl_433_ESP::_lastChange = 0; // Timestamp of previous edge
int rtl_433_ESP::rtlVerbose = 0;
volatile int16_t rtl_433_ESP::_nrpulses = -1;

#ifdef DEAF_WORKAROUND
int deafWorkaround = 0;
#endif

int16_t rtl_433_ESP::_interrupt = NOT_AN_INTERRUPT;
static byte receiverGpio = -1;

static unsigned long signalEnd = micros();

r_cfg_t rtl_433_ESP::g_cfg;

void rtl_433_ESP::rtlSetup(r_cfg_t *cfg)
{
  unsigned i;
#ifdef MEMORY_DEBUG
  logprintfLn(LOG_INFO, "sizeof(*cfg->demod) %d", sizeof(*cfg->demod));
#endif
  r_init_cfg(cfg);
#ifdef MEMORY_DEBUG
  logprintfLn(LOG_INFO, "sizeof(*cfg->demod) %d", sizeof(*cfg->demod));
#endif
  cfg->conversion_mode = CONVERT_SI; // Default all output to Celcius
  cfg->num_r_devices = NUMOFDEVICES;
  cfg->devices = (r_device *)calloc(cfg->num_r_devices, sizeof(r_device));
  if (!cfg->devices)
    FATAL_CALLOC("cfg->devices");

#ifndef MY_DEVICES
  // This is a generated fragment from tools/update_rtl_433_devices.sh

  memcpy(&cfg->devices[0], &acurite_rain_896, sizeof(r_device));
  memcpy(&cfg->devices[1], &acurite_th, sizeof(r_device));
  memcpy(&cfg->devices[2], &acurite_txr, sizeof(r_device));
  memcpy(&cfg->devices[3], &acurite_986, sizeof(r_device));
  memcpy(&cfg->devices[4], &acurite_606, sizeof(r_device));
  memcpy(&cfg->devices[5], &acurite_00275rm, sizeof(r_device));
  memcpy(&cfg->devices[6], &acurite_590tx, sizeof(r_device));
  memcpy(&cfg->devices[7], &acurite_01185m, sizeof(r_device));
  memcpy(&cfg->devices[8], &akhan_100F14, sizeof(r_device));
  memcpy(&cfg->devices[9], &alectov1, sizeof(r_device));
  memcpy(&cfg->devices[10], &ambientweather_tx8300, sizeof(r_device));
  memcpy(&cfg->devices[11], &auriol_4ld5661, sizeof(r_device));
  memcpy(&cfg->devices[12], &auriol_aft77b2, sizeof(r_device));
  memcpy(&cfg->devices[13], &auriol_afw2a1, sizeof(r_device));
  memcpy(&cfg->devices[14], &auriol_ahfl, sizeof(r_device));
  memcpy(&cfg->devices[15], &auriol_hg02832, sizeof(r_device));
  memcpy(&cfg->devices[16], &blueline, sizeof(r_device));
  memcpy(&cfg->devices[17], &blyss, sizeof(r_device));
  memcpy(&cfg->devices[18], &brennenstuhl_rcs_2044, sizeof(r_device));
  memcpy(&cfg->devices[19], &bresser_3ch, sizeof(r_device));
  memcpy(&cfg->devices[20], &bt_rain, sizeof(r_device));
  memcpy(&cfg->devices[21], &burnhardbbq, sizeof(r_device));
  memcpy(&cfg->devices[22], &calibeur_RF104, sizeof(r_device));
  memcpy(&cfg->devices[23], &cardin, sizeof(r_device));
  memcpy(&cfg->devices[24], &chuango, sizeof(r_device));
  memcpy(&cfg->devices[25], &companion_wtr001, sizeof(r_device));
  memcpy(&cfg->devices[26], &digitech_xc0324, sizeof(r_device));
  memcpy(&cfg->devices[27], &dish_remote_6_3, sizeof(r_device));
  memcpy(&cfg->devices[28], &ecowitt, sizeof(r_device));
  memcpy(&cfg->devices[29], &eurochron_efth800, sizeof(r_device));
  memcpy(&cfg->devices[30], &elro_db286a, sizeof(r_device));
  memcpy(&cfg->devices[31], &elv_em1000, sizeof(r_device));
  memcpy(&cfg->devices[32], &elv_ws2000, sizeof(r_device));
  memcpy(&cfg->devices[33], &esperanza_ews, sizeof(r_device));
  memcpy(&cfg->devices[34], &eurochron, sizeof(r_device));
  memcpy(&cfg->devices[35], &fineoffset_WH2, sizeof(r_device));
  memcpy(&cfg->devices[36], &fineoffset_WH0530, sizeof(r_device));
  memcpy(&cfg->devices[37], &fineoffset_wh1050, sizeof(r_device));
  memcpy(&cfg->devices[38], &fineoffset_wh1080, sizeof(r_device));
  memcpy(&cfg->devices[39], &fs20, sizeof(r_device));
  memcpy(&cfg->devices[40], &ft004b, sizeof(r_device));
  memcpy(&cfg->devices[41], &generic_motion, sizeof(r_device));
  memcpy(&cfg->devices[42], &generic_remote, sizeof(r_device));
  memcpy(&cfg->devices[43], &generic_temperature_sensor, sizeof(r_device));
  memcpy(&cfg->devices[44], &govee, sizeof(r_device));
  memcpy(&cfg->devices[45], &gt_tmbbq05, sizeof(r_device));
  memcpy(&cfg->devices[46], &gt_wt_02, sizeof(r_device));
  memcpy(&cfg->devices[47], &gt_wt_03, sizeof(r_device));
  memcpy(&cfg->devices[48], &hcs200, sizeof(r_device));
  memcpy(&cfg->devices[49], &honeywell_wdb, sizeof(r_device));
  memcpy(&cfg->devices[50], &ht680, sizeof(r_device));
  memcpy(&cfg->devices[51], &infactory, sizeof(r_device));
  memcpy(&cfg->devices[52], &kw9015b, sizeof(r_device));
  memcpy(&cfg->devices[53], &interlogix, sizeof(r_device));
  memcpy(&cfg->devices[54], &intertechno, sizeof(r_device));
  memcpy(&cfg->devices[55], &kedsum, sizeof(r_device));
  memcpy(&cfg->devices[56], &kerui, sizeof(r_device));
  memcpy(&cfg->devices[57], &lacrossetx, sizeof(r_device));
  memcpy(&cfg->devices[58], &lacrosse_tx141x, sizeof(r_device));
  memcpy(&cfg->devices[59], &lacrosse_ws7000, sizeof(r_device));
  memcpy(&cfg->devices[60], &lacrossews, sizeof(r_device));
  memcpy(&cfg->devices[61], &lightwave_rf, sizeof(r_device));
  memcpy(&cfg->devices[62], &markisol, sizeof(r_device));
  memcpy(&cfg->devices[63], &maverick_et73, sizeof(r_device));
  memcpy(&cfg->devices[64], &mebus433, sizeof(r_device));
  memcpy(&cfg->devices[65], &missil_ml0757, sizeof(r_device));
  memcpy(&cfg->devices[66], &new_template, sizeof(r_device));
  memcpy(&cfg->devices[67], &nexus, sizeof(r_device));
  memcpy(&cfg->devices[68], &nice_flor_s, sizeof(r_device));
  memcpy(&cfg->devices[69], &opus_xt300, sizeof(r_device));
  memcpy(&cfg->devices[70], &oregon_scientific_sl109h, sizeof(r_device));
  memcpy(&cfg->devices[71], &oregon_scientific_v1, sizeof(r_device));
  memcpy(&cfg->devices[72], &philips_aj3650, sizeof(r_device));
  memcpy(&cfg->devices[73], &philips_aj7010, sizeof(r_device));
  memcpy(&cfg->devices[74], &prologue, sizeof(r_device));
  memcpy(&cfg->devices[75], &quhwa, sizeof(r_device));
  memcpy(&cfg->devices[76], &rftech, sizeof(r_device));
  memcpy(&cfg->devices[77], &rubicson, sizeof(r_device));
  memcpy(&cfg->devices[78], &rubicson_48659, sizeof(r_device));
  memcpy(&cfg->devices[79], &s3318p, sizeof(r_device));
  memcpy(&cfg->devices[80], &silvercrest, sizeof(r_device));
  memcpy(&cfg->devices[81], &skylink_motion, sizeof(r_device));
  memcpy(&cfg->devices[82], &smoke_gs558, sizeof(r_device));
  memcpy(&cfg->devices[83], &solight_te44, sizeof(r_device));
  memcpy(&cfg->devices[84], &springfield, sizeof(r_device));
  memcpy(&cfg->devices[85], &tfa_30_3221, sizeof(r_device));
  memcpy(&cfg->devices[86], &tfa_drop_303233, sizeof(r_device));
  memcpy(&cfg->devices[87], &tfa_pool_thermometer, sizeof(r_device));
  memcpy(&cfg->devices[88], &tfa_twin_plus_303049, sizeof(r_device));
  memcpy(&cfg->devices[89], &thermopro_tp11, sizeof(r_device));
  memcpy(&cfg->devices[90], &thermopro_tp12, sizeof(r_device));
  memcpy(&cfg->devices[91], &thermopro_tx2, sizeof(r_device));
  memcpy(&cfg->devices[92], &ts_ft002, sizeof(r_device));
  memcpy(&cfg->devices[93], &visonic_powercode, sizeof(r_device));
  memcpy(&cfg->devices[94], &waveman, sizeof(r_device));
  memcpy(&cfg->devices[95], &wg_pb12v1, sizeof(r_device));
  memcpy(&cfg->devices[96], &ws2032, sizeof(r_device));
  memcpy(&cfg->devices[97], &wssensor, sizeof(r_device));
  memcpy(&cfg->devices[98], &wt1024, sizeof(r_device));
  memcpy(&cfg->devices[99], &X10_RF, sizeof(r_device));
  memcpy(&cfg->devices[100], &x10_sec, sizeof(r_device));

  // end of fragement

#else
  memcpy(&cfg->devices[0], &skylink_motion, sizeof(r_device));
  memcpy(&cfg->devices[1], &prologue, sizeof(r_device));
  memcpy(&cfg->devices[2], &acurite_986, sizeof(r_device));
  memcpy(&cfg->devices[3], &philips_aj3650, sizeof(r_device));
  memcpy(&cfg->devices[4], &lacrosse_tx141x, sizeof(r_device));
#endif

#ifdef RTL_FLEX
  // This option is non-functional. The flex decoder is too resource intensive for an ESP32, and needs the ESP32 stack set to 32768 in order for the flex_callback to execute
  // Tested with -DRTL_FLEX="n=Sonoff-PIR3-RF,m=OOK_PWM,s=300,l=860,r=7492,g=868,t=50,y=0,bits>=24,repeats>=5,invert,get=@0:{20}:id,get=@20:{4}:motion:[0:true ],unique"

  r_device *flex_device;
  flex_device = flex_create_device(RTL_FLEX);
  memcpy(&cfg->devices[101], &flex_device, sizeof(r_device));
  register_protocol(cfg, flex_device, NULL);
  alogprintfLn(LOG_INFO, "Flex Decoder enabled: %s", RTL_FLEX);
#endif

// logprintfLn(LOG_INFO, "Location of r_devices: %p", (void *)&r_devices);
// logprintfLn(LOG_INFO, "Location of cfg: %p", (void *)&cfg);
// logprintfLn(LOG_INFO, "cfg size %d", sizeof(r_cfg_t));
// logprintfLn(LOG_INFO, "Location of cfg->devices: %p", (void *)&cfg->devices);
#ifdef MEMORY_DEBUG
  logprintfLn(LOG_INFO, "size of bitbuffer: %d", sizeof(bitbuffer_t));
  logprintfLn(LOG_INFO, "size of pulse_data: %d", sizeof(pulse_data_t));
#endif

  int numberEnabled = 0;

  for (i = 0; i < cfg->num_r_devices; i++)
  {
    cfg->devices[i].protocol_num = i + 1;
    // These pulse demods have been tested (85), ymmv for the others
    if (cfg->devices[i].modulation == OOK_PULSE_PPM || cfg->devices[i].modulation == OOK_PULSE_PWM)
    {
      numberEnabled++;
    }
    else
    {
      cfg->devices[i].disabled = 1;
    }
  }
#ifdef DEMOD_DEBUG
  logprintfLn(LOG_INFO, "# of device(s) configured %d", cfg->num_r_devices);
  logprintfLn(LOG_INFO, "ssizeof(r_device): %d", sizeof(r_device));
  logprintfLn(LOG_INFO, "cfg->devices size: %d", sizeof(r_device) * cfg->num_r_devices);
  logprintfLn(LOG_INFO, "# of device(s) enabled %d", numberEnabled);

#endif
#ifdef RTL_DEBUG
  cfg->verbosity = RTL_DEBUG; // 0=normal, 1=verbose, 2=verbose decoders, 3=debug decoders, 4=trace decoding.
#else
  cfg->verbosity = rtlVerbose; // 0=normal, 1=verbose, 2=verbose decoders, 3=debug decoders, 4=trace decoding.
#endif
#ifdef RTL_VERBOSE
  cfg->devices[RTL_VERBOSE].verbose = 4;
  logprintfLn(LOG_INFO, "%s Log Level %d", cfg->devices[RTL_VERBOSE].name, cfg->devices[RTL_VERBOSE].verbose);
#endif
  register_all_protocols(cfg, 0);
}

void rtl_433_ESP::initReceiver(byte inputPin, float receiveFrequency)
{
  receiverGpio = inputPin;
#ifdef MEMORY_DEBUG
  logprintfLn(LOG_INFO, "Pre initReceiver: %d", ESP.getFreeHeap());
#endif
#ifdef DEMOD_DEBUG
  logprintfLn(LOG_INFO, STR_MODULE " gpio receive pin: %d", inputPin);
  logprintfLn(LOG_INFO, STR_MODULE " receive frequency: %f", receiveFrequency);
#endif
  r_cfg_t *cfg = &g_cfg;

  rtlSetup(cfg);

// ESP32 can use VSPI, but heltec uses MOSI=27, MISO=19, SCK=5, CS=18
#if defined(RF_MODULE_SCK) && defined(RF_MODULE_MISO) && defined(RF_MODULE_MOSI) && defined(RF_MODULE_CS)
  newSPI.begin(RF_MODULE_SCK, RF_MODULE_MISO, RF_MODULE_MOSI, RF_MODULE_CS);
#else
//  newSPI.begin();
#endif
#ifdef RF_CC1101
  int state = radio.begin();
#else
  int state = radio.beginFSK();
#endif
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F(STR_MODULE " radio.begin() success!"));
  }
  else
  {
    Serial.print(F(STR_MODULE " failed initialization, code "));
    Serial.println(state);
    while (true)
      ;
  }

  /*
    state = radio.setOOK(true);
      state = radio.setFrequency(433.92);
    state = radio.setBitRate(20);
    state = radio.setFrequencyDeviation(100);
    state = radio.setRxBandwidth(200);
    state = radio.setCRC(false);
    // state = radio.variablePacketLengthMode(SX127X_MAX_PACKET_LENGTH_FSK);
    // state = radio.setOokThresholdType(SX127X_OOK_THRESH_AVERAGE);

    // the default RadioLib FSK sync word is 0x12AD
    // radio.setDirectSyncWord(0x555512AD, 32);
    state = radio.setDirectSyncWord(0x01, 8);
    */

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

#ifndef RF_CC1101
  state = radio.setDataShapingOOK(1);
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("setDataShapingOOK - success!"));
  }
  else
  {
    Serial.print(F("setDataShapingOOK failed, code "));
    Serial.println(state);
    while (true)
      ;
  }

  state = radio.setOokPeakThresholdDecrement(RADIOLIB_SX127X_OOK_PEAK_THRESH_DEC_1_2_CHIP);
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("setOokPeakThresholdDecrement - success!"));
  }
  else
  {
    Serial.print(F("setOokPeakThresholdDecrement failed, code "));
    Serial.println(state);
    while (true)
      ;
  }

  state = radio.setOokThresholdType(RADIOLIB_SX127X_OOK_THRESH_PEAK);
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("setOokThresholdType - success!"));
  }
  else
  {
    Serial.print(F("setOokThresholdType failed, code "));
    Serial.println(state);
    while (true)
      ;
  }
  state = radio.setOokFixedOrFloorThreshold(0x0C);
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("setOokFixedOrFloorThreshold - success!"));
  }
  else
  {
    Serial.print(F("setOokFixedOrFloorThreshold failed, code "));
    Serial.println(state);
    while (true)
      ;
  }

  state = radio.setPreambleLength(0);
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("setPreambleLength - success!"));
  }
  else
  {
    Serial.print(F("setPreambleLength failed, code "));
    Serial.println(state);
    while (true)
      ;
  }

  state = _mod->SPIsetRegValue(RADIOLIB_SX127X_REG_PREAMBLE_DETECT, RADIOLIB_SX127X_PREAMBLE_DETECTOR_OFF);
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("PREAMBLE_DETECT - success!"));
  }
  else
  {
    Serial.print(F("PREAMBLE_DETECT failed, code "));
    Serial.println(state);
    while (true)
      ;
  }
#endif

  radio.setFrequency(receiveFrequency);
  resetReceiver();
  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, LOW);
#ifdef MEMORY_DEBUG
  logprintfLn(LOG_INFO, "Post initReceiver: %d", ESP.getFreeHeap());
#endif

#ifndef RF_CC1101
  state = radio.setBitRate(32.768);
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("setBitRate - success!"));
  }
  else
  {
    Serial.print(F("setBitRate failed, code "));
    Serial.println(state);
    while (true)
      ;
  }
  state = radio.setFrequencyDeviation(100);
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("setFrequencyDeviation - success!"));
  }
  else
  {
    Serial.print(F("setFrequencyDeviation failed, code "));
    Serial.println(state);
    while (true)
      ;
  }
  state = radio.setRxBandwidth(200);
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("setRxBandwidth - success!"));
  }
  else
  {
    Serial.print(F("setRxBandwidth failed, code "));
    Serial.println(state);
    while (true)
      ;
  }

#endif

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

#ifndef RF_CC1101

  state = radio.setDirectSyncWord(0, 0);
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("setDirectSyncWord - success!"));
  }
  else
  {
    Serial.print(F("setDirectSyncWord failed, code "));
    Serial.println(state);
    while (true)
      ;
  }

  state = radio.disableBitSync();
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("disableBitSync - success!"));
  }
  else
  {
    Serial.print(F("disableBitSync failed, code "));
    Serial.println(state);
    while (true)
      ;
  }
#endif

#if defined(RF_SX1276) || defined(RF_SX1278)
  state = radio.receiveDirect();
#else
  state = radio.receiveDirectAsync();
#endif
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
#if defined(RF_SX1276) || defined(RF_SX1278)
  getSX127xStatus();
#else
  getCC1101Status();
  // alogprintfLn(LOG_INFO, "CC1101_STATUS: %d", getRSSI());
#endif
}

int rtl_433_ESP::receivePulseTrain()
{

  if (_pulseTrains[_avaiablePulseTrain].num_pulses > 0)
  {
    uint8_t _currentTrain = _avaiablePulseTrain;
    _avaiablePulseTrain = (_avaiablePulseTrain + 1) % RECEIVER_BUFFER_SIZE;
    return _currentTrain;
  }
  return -1;
}

void ICACHE_RAM_ATTR rtl_433_ESP::interruptHandler()
{
  if (!_enabledReceiver)
  {
    return;
  }
  volatile pulse_data_t &pulseTrain = _pulseTrains[_actualPulseTrain];
  volatile int *pulse = pulseTrain.pulse;
  volatile int *gap = pulseTrain.gap;
#ifdef RSSI
  volatile int *rssi = pulseTrain.rssi;
#endif

  const unsigned long now = micros();
  const unsigned int duration = now - _lastChange;

  /* We first do some filtering (same as pilight BPF) */
  if (duration > MINIMUM_PULSE_LENGTH && currentRssi > minimumRssi)
  {
#ifdef RSSI
    rssi[_nrpulses] = currentRssi;
#endif
    if (!digitalRead(receiverGpio))
    {
      if (_nrpulses > -1)
      {
        pulse[_nrpulses] = duration;
      }
      //      _nrpulses = (uint16_t)((_nrpulses + 1) % PD_MAX_PULSES);
    }
    else
    {
      if (_nrpulses > -1)
      {
        gap[_nrpulses] = duration;
      }
      _nrpulses = (uint16_t)((_nrpulses + 1) % PD_MAX_PULSES);
    }
    _lastChange = now;
  }
}

void rtl_433_ESP::resetReceiver()
{
  for (unsigned int i = 0; i < RECEIVER_BUFFER_SIZE; i++)
  {
    _pulseTrains[i].num_pulses = 0;
  }
  _avaiablePulseTrain = 0;
  _actualPulseTrain = 0;
  _nrpulses = -1;

  receiveMode = false;
  signalStart = micros();
}

void rtl_433_ESP::enableReceiver(byte inputPin)
{
  pinMode(inputPin, INPUT);
  int16_t interrupt = digitalPinToInterrupt(inputPin);
  if (_interrupt == interrupt)
  {
    return;
  }
  if (_interrupt >= 0)
  {
    detachInterrupt((uint8_t)_interrupt);
  }
  _interrupt = interrupt;

  if (interrupt >= 0)
  {
    attachInterrupt((uint8_t)interrupt, interruptHandler, CHANGE);
    _enabledReceiver = true;
  }
}

void rtl_433_ESP::disableReceiver() { _enabledReceiver = false; }

int totalRssi = 0;
int rssiCount = 0;

void rtl_433_ESP::loop()
{
  if (_enabledReceiver)
  {

#ifdef RF_CC1101
#ifdef DEAF_WORKAROUND
    deafWorkaround++; // workaround for a deaf CC1101, see issue #16
    if (deafWorkaround > 2000)
    {
      deafWorkaround = 0;
      // radio.SetRx(); // set Receive on
      radio.SPIsendCommand(RADIOLIB_CC1101_CMD_IDLE); // set Receive on
      radio.SPIsendCommand(RADIOLIB_CC1101_CMD_RX);   // set Receive on

    } // workaround for a deaf CC1101
#endif
#endif

    currentRssi = getRSSI();

    rssiCount++;
    totalRssi += currentRssi;

    if (rssiCount > 500)
    {
      alogprintfLn(LOG_INFO, "\nAverage Signal %d dbm", totalRssi / rssiCount);
      totalRssi = 0;
      rssiCount = 0;
    }

    if (currentRssi > minimumRssi)
    {
      if (!receiveMode)
      {
        receiveMode = true;
        signalStart = micros();
        enableReceiver(receiverGpio);
        digitalWrite(ONBOARD_LED, HIGH);
        signalRssi = currentRssi;
        _lastChange = micros();
      }
      signalEnd = micros();
    }
    // If we received a signal but had a minor drop in strength keep the receiver running for an additional 20,0000
    else if ((micros() - signalEnd < 40000 && micros() - signalStart > 30000) || (micros() - signalEnd < 10000))
    {
      // skip over signal drop outs
    }
    else
    {
      if (receiveMode)
      {
        digitalWrite(ONBOARD_LED, LOW);
        receiveMode = false;
        enableReceiver(-1);
        if (_nrpulses > PD_MIN_PULSES && (micros() - signalStart > 40000))
        {
#ifdef DEMOD_DEBUG
          logprintf(LOG_INFO, "Signal length: %lu", micros() - signalStart);
          alogprintf(LOG_INFO, ", Gap length: %lu", signalStart - gapStart);
          alogprintf(LOG_INFO, ", Signal RSSI: %d", signalRssi);
          alogprintf(LOG_INFO, ", train: %d", _actualPulseTrain);
          alogprintf(LOG_INFO, ", messageCount: %d", messageCount);
          alogprintfLn(LOG_INFO, ", pulses: %d", _nrpulses);
#endif
          messageCount++;

          gapStart = micros();
          _pulseTrains[_actualPulseTrain].num_pulses = _nrpulses;
          _pulseTrains[_actualPulseTrain].signalDuration = micros() - signalStart;
          _pulseTrains[_actualPulseTrain].signalRssi = signalRssi;
          _actualPulseTrain = (_actualPulseTrain + 1) % RECEIVER_BUFFER_SIZE;

          _nrpulses = -1;
        }
        else
        {
#ifdef DEMOD_DEBUG
          if (micros() - signalStart > 1000)
          {
            logprintf(LOG_INFO, "Ignored Signal length: %lu", micros() - signalStart);
            alogprintf(LOG_INFO, ", Gap length: %lu", signalStart - gapStart);
            alogprintf(LOG_INFO, ", Signal RSSI: %d", signalRssi);
            alogprintf(LOG_INFO, ", Current RSSI: %d", currentRssi);
            alogprintfLn(LOG_INFO, ", pulses: %d", _nrpulses);
            gapStart = micros();
          }
#endif
          _nrpulses = -1;
        }
      }
    }

    int _receiveTrain = receivePulseTrain();

    pulse_data_t *rtl_pulses = (pulse_data_t *)calloc(1, sizeof(pulse_data_t));
    memcpy(rtl_pulses, (char *)&_pulseTrains[_receiveTrain], sizeof(pulse_data_t));
    _pulseTrains[_receiveTrain].num_pulses = 0; // Make pulse train available for next train

    if (_receiveTrain != -1 && rtl_pulses->num_pulses > 30)
    {
#ifdef MEMORY_DEBUG
      unsigned long signalProcessingStart = micros();
#endif
      rtl_pulses->sample_rate = 1.0e6;
#ifdef RAW_SIGNAL_DEBUG
      logprintf(LOG_INFO, "RAW (%lu): ", rtl_pulses->signalDuration);
      for (int i = 0; i < rtl_pulses->num_pulses; i++)
      {
        alogprintf(LOG_INFO, "+%d", rtl_pulses->pulse[i]);
        alogprintf(LOG_INFO, "-%d", rtl_pulses->gap[i]);
#ifdef RSSI
        alogprintf(LOG_INFO, "(%d)", rtl_pulses->rssi[i]);
#endif
      }
      alogprintfLn(LOG_INFO, " ");
#endif
#ifdef MEMORY_DEBUG
      logprintfLn(LOG_INFO, "Pre run_ook_demods: %d", ESP.getFreeHeap());
#endif
      r_cfg_t *cfg = &g_cfg;
      cfg->demod->pulse_data = rtl_pulses;
      int events = run_ook_demods(&cfg->demod->r_devs, rtl_pulses);
      if (events == 0)
      {
#ifdef PUBLISH_UNPARSED
        logprintf(LOG_INFO, "Unparsed Signal length: %lu", rtl_pulses->signalDuration);
        alogprintf(LOG_INFO, ", Signal RSSI: %d", rtl_pulses->signalRssi);
        alogprintf(LOG_INFO, ", train: %d", _actualPulseTrain);
        alogprintf(LOG_INFO, ", messageCount: %d", messageCount);
        alogprintfLn(LOG_INFO, ", pulses: %d", rtl_pulses->num_pulses);

        logprintf(LOG_INFO, "RAW (%d): ", rtl_pulses->signalDuration);
#ifndef RAW_SIGNAL_DEBUG
        for (int i = 0; i < rtl_pulses->num_pulses; i++)
        {
          alogprintf(LOG_INFO, "+%d", rtl_pulses->pulse[i]);
          alogprintf(LOG_INFO, "-%d", rtl_pulses->gap[i]);
#ifdef RSSI
          alogprintf(LOG_INFO, "(%d)", rtl_pulses->rssi[i]);
#endif
        }
        alogprintfLn(LOG_INFO, " ");
#endif

        // Send a note saying unparsed signal signal received

        data_t *data;
        /* clang-format off */
  data = data_make(
                "model", "",      DATA_STRING,  "unknown",
                "protocol", "",   DATA_STRING,  "signal parsing failed",
                "duration", "",   DATA_INT,     rtl_pulses->signalDuration,
                "rssi", "", DATA_INT,     rtl_pulses->signalRssi,
                "pulses", "",     DATA_INT,     rtl_pulses->num_pulses,
                "train", "",      DATA_INT,     _actualPulseTrain,
                "messageCount", "", DATA_INT,   messageCount,
                "_enabledReceiver", "", DATA_INT, _enabledReceiver,
                "receiveMode", "", DATA_INT,    receiveMode,
                "currentRssi", "", DATA_INT,    currentRssi,
                "minimumRssi", "", DATA_INT,    minimumRssi,
                NULL);
        /* clang-format on */

        r_cfg_t *cfg = &g_cfg;
        data_print_jsons(data, cfg->messageBuffer, cfg->bufferSize);
        (cfg->callback)(cfg->messageBuffer);
        data_free(data);
#endif
      }

      // free(rtl_pulses);

#ifdef MEMORY_DEBUG
      logprintfLn(LOG_INFO, "Signal processing time: %lu", micros() - signalProcessingStart);
      logprintfLn(LOG_INFO, "Post run_ook_demods memory %d", ESP.getFreeHeap());
#endif
#ifdef DEMOD_DEBUG
      logprintfLn(LOG_INFO, "# of messages decoded %d", events);
#endif
      if (events > 0)
      {
        alogprintfLn(LOG_INFO, " ");
      }
#if defined(MEMORY_DEBUG) || defined(DEMOD_DEBUG) || defined(RAW_SIGNAL_DEBUG) || defined(PUBLISH_UNPARSED)
      else
      {
        alogprintfLn(LOG_INFO, " ");
      }
#endif
    }
    free(rtl_pulses);
  }
}

rtl_433_ESP::rtl_433_ESP(int8_t outputPin)
{
  _outputPin = outputPin;

  if (_outputPin >= 0)
  {
    pinMode((uint8_t)_outputPin, OUTPUT);
    digitalWrite((uint8_t)_outputPin, LOW);
  }

  _pulseTrains = (pulse_data_t *)calloc(RECEIVER_BUFFER_SIZE, sizeof(pulse_data_t));
}

void rtl_433_ESP::setCallback(rtl_433_ESPCallBack callback, char *messageBuffer, int bufferSize)
{
  r_cfg_t *cfg = &g_cfg;
  cfg->callback = callback;
  cfg->messageBuffer = messageBuffer;
  cfg->bufferSize = bufferSize;
  // logprintfLn(LOG_INFO, "setCallback location: %p", cfg->callback);
}

void rtl_433_ESP::setMinimumRSSI(int newRssi)
{
  minimumRssi = newRssi;
  logprintfLn(LOG_INFO, "Setting minimum RSSI to: %d", minimumRssi);
}

void rtl_433_ESP::setDebug(int debug)
{
  rtlVerbose = debug;
  logprintfLn(LOG_INFO, "Setting rtl_433 debug to: %d", rtlVerbose);
}

void rtl_433_ESP::getStatus(int status)
{
  alogprintfLn(LOG_INFO, " ");
  logprintf(LOG_INFO, "Status Message: Gap length: %lu", signalStart - gapStart);
  alogprintf(LOG_INFO, ", Signal RSSI: %d", signalRssi);
  alogprintf(LOG_INFO, ", train: %d", _actualPulseTrain);
  alogprintf(LOG_INFO, ", messageCount: %d", messageCount);
  alogprintf(LOG_INFO, ", _enabledReceiver: %d", _enabledReceiver);
  alogprintf(LOG_INFO, ", receiveMode: %d", receiveMode);
  alogprintf(LOG_INFO, ", currentRssi: %d", currentRssi);
  alogprintf(LOG_INFO, ", minimumRssi: %d", minimumRssi);
  alogprintf(LOG_INFO, ", StackHighWaterMark: %d", uxTaskGetStackHighWaterMark(NULL));
  alogprintfLn(LOG_INFO, ", pulses: %d", _nrpulses);

  data_t *data;

  /* clang-format off */
  data = data_make(
                "model", "",      DATA_STRING,  "status",
                "protocol", "",   DATA_STRING,  "debug",
                "debug", "",      DATA_INT,     rtlVerbose,
                "duration", "",   DATA_INT,     micros() - signalStart,
                "Gap length", "", DATA_INT,     (signalStart - gapStart),
                "rssi", "", DATA_INT,     signalRssi,
                "train", "", DATA_INT,          _actualPulseTrain,
                "messageCount", "", DATA_INT,   messageCount,
                "_enabledReceiver", "", DATA_INT, _enabledReceiver,
                "receiveMode", "", DATA_INT,    receiveMode,
                "currentRssi", "", DATA_INT,    currentRssi,
                "minimumRssi", "", DATA_INT,    minimumRssi,
                "messageCount", "", DATA_INT,   messageCount,
                "pulses", "", DATA_INT,         _nrpulses,
                "StackHighWaterMark", "", DATA_INT, uxTaskGetStackHighWaterMark(NULL),
                "freeMem", "", DATA_INT,        ESP.getFreeHeap(),
                NULL);
  /* clang-format on */
  r_cfg_t *cfg = &g_cfg;

  data_print_jsons(data, cfg->messageBuffer, cfg->bufferSize);
  (cfg->callback)(cfg->messageBuffer);
  data_free(data);
}

// Replacement RSSI function for the one in RadioLib that doesn't work in async mode

/****************************************************************
 *FUNCTION NAME:RSSI Level
 *FUNCTION     :Calculating the RSSI Level
 *INPUT        :none
 *OUTPUT       :none
 ****************************************************************/
int rtl_433_ESP::getRSSI(void)
{
  int rssi;

#ifdef RF_CC1101
  int rawRssi = radio.SPIgetRegValue(RADIOLIB_CC1101_REG_RSSI);
  if (rawRssi >= 128)
  {
    rssi = ((rawRssi - 256) / 2) - 74;
  }
  else
  {
    rssi = (rawRssi / 2) - 74;
  }
#else
  int rawRssi = _mod->SPIgetRegValue(RADIOLIB_SX127X_REG_RSSI_VALUE_FSK);
  rssi = -(rawRssi / 2);
#endif
  return rssi;
}

void rtl_433_ESP::getCC1101Status()
{
  alogprintfLn(LOG_INFO, "----- CC1101 Status -----");
  alogprintfLn(LOG_INFO, "CC1101_MDMCFG1: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_MDMCFG1));
  alogprintfLn(LOG_INFO, "CC1101_MDMCFG2: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_MDMCFG2));
  alogprintfLn(LOG_INFO, "CC1101_MDMCFG3: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_MDMCFG3));
  alogprintfLn(LOG_INFO, "CC1101_MDMCFG4: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_MDMCFG4));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_DEVIATN: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_DEVIATN));
  alogprintfLn(LOG_INFO, "CC1101_AGCCTRL0: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_AGCCTRL0));
  alogprintfLn(LOG_INFO, "CC1101_AGCCTRL1: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_AGCCTRL1));
  alogprintfLn(LOG_INFO, "CC1101_AGCCTRL2: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_AGCCTRL2));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_IOCFG0: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_IOCFG0));
  alogprintfLn(LOG_INFO, "CC1101_IOCFG1: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_IOCFG1));
  alogprintfLn(LOG_INFO, "CC1101_IOCFG2: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_IOCFG2));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_FIFOTHR: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_FIFOTHR));
  alogprintfLn(LOG_INFO, "CC1101_SYNC0: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_SYNC0));
  alogprintfLn(LOG_INFO, "CC1101_SYNC1: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_SYNC1));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_PKTLEN: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_PKTLEN));
  alogprintfLn(LOG_INFO, "CC1101_PKTCTRL0: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_PKTCTRL0));
  alogprintfLn(LOG_INFO, "CC1101_PKTCTRL1: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_PKTCTRL1));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_ADDR: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_ADDR));
  alogprintfLn(LOG_INFO, "CC1101_CHANNR: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_CHANNR));
  alogprintfLn(LOG_INFO, "CC1101_FSCTRL0: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_FSCTRL0));
  alogprintfLn(LOG_INFO, "CC1101_FSCTRL1: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_FSCTRL1));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_FREQ0: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_FREQ0));
  alogprintfLn(LOG_INFO, "CC1101_FREQ1: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_FREQ1));
  alogprintfLn(LOG_INFO, "CC1101_FREQ2: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_FREQ2));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_MCSM0: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_MCSM0));
  alogprintfLn(LOG_INFO, "CC1101_MCSM1: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_MCSM1));
  alogprintfLn(LOG_INFO, "CC1101_MCSM2: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_MCSM2));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_FOCCFG: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_FOCCFG));

  alogprintfLn(LOG_INFO, "CC1101_BSCFG: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_BSCFG));
  alogprintfLn(LOG_INFO, "CC1101_WOREVT0: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_WOREVT0));
  alogprintfLn(LOG_INFO, "CC1101_WOREVT1: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_WOREVT1));
  alogprintfLn(LOG_INFO, "CC1101_WORCTRL: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_WORCTRL));
  alogprintfLn(LOG_INFO, "CC1101_FREND0: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_FREND0));
  alogprintfLn(LOG_INFO, "CC1101_FREND1: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_FREND1));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_FSCAL0: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_FSCAL0));
  alogprintfLn(LOG_INFO, "CC1101_FSCAL1: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_FSCAL1));
  alogprintfLn(LOG_INFO, "CC1101_FSCAL2: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_FSCAL2));
  alogprintfLn(LOG_INFO, "CC1101_FSCAL3: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_FSCAL3));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_RCCTRL0: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_RCCTRL0));
  alogprintfLn(LOG_INFO, "CC1101_RCCTRL1: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_CC1101_REG_RCCTRL1));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_PARTNUM: 0x%.2x", _mod->SPIgetRegValue(RADIOLIB_CC1101_REG_PARTNUM));
  alogprintfLn(LOG_INFO, "CC1101_VERSION: 0x%.2x", _mod->SPIgetRegValue(RADIOLIB_CC1101_REG_VERSION));
  alogprintfLn(LOG_INFO, "CC1101_MARCSTATE: 0x%.2x", _mod->SPIgetRegValue(RADIOLIB_CC1101_REG_MARCSTATE));
  alogprintfLn(LOG_INFO, "CC1101_PKTSTATUS: 0x%.2x", _mod->SPIgetRegValue(RADIOLIB_CC1101_REG_PKTSTATUS));
  alogprintfLn(LOG_INFO, "CC1101_RXBYTES: 0x%.2x", _mod->SPIgetRegValue(RADIOLIB_CC1101_REG_RXBYTES));

  alogprintfLn(LOG_INFO, "----- CC1101 Status -----");
}

void rtl_433_ESP::getSX127xStatus()
{
  alogprintfLn(LOG_INFO, "----- SX127x Status -----");

  alogprintfLn(LOG_INFO, "RegOpMode: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_SX127X_REG_OP_MODE));
  alogprintfLn(LOG_INFO, "RegPacketConfig1: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_SX127X_REG_PACKET_CONFIG_2));
  alogprintfLn(LOG_INFO, "RegPacketConfig2: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_SX127X_REG_PACKET_CONFIG_2));
  alogprintfLn(LOG_INFO, "RegBitrateMsb: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_SX127X_REG_BITRATE_MSB));
  alogprintfLn(LOG_INFO, "RegBitrateLsb: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_SX127X_REG_BITRATE_LSB));
  alogprintfLn(LOG_INFO, "RegRxBw: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_SX127X_REG_RX_BW));
  alogprintfLn(LOG_INFO, "RegAfcBw: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_SX127X_REG_AFC_BW));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "RegOokPeak: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_SX127X_REG_OOK_PEAK));
  alogprintfLn(LOG_INFO, "RegOokFix: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_SX127X_REG_OOK_FIX));
  alogprintfLn(LOG_INFO, "RegOokAvg: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_SX127X_REG_OOK_AVG));

  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "RegDioMapping1: 0x%.2x", _mod->SPIreadRegister(RADIOLIB_SX127X_REG_DIO_MAPPING_1));

  alogprintfLn(LOG_INFO, "----- SX127x Status -----");
}