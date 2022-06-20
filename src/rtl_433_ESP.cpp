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


  Project Structure

  rtl_433_ESP - Main Class
  decoder.cpp - Wrapper and interface for the rtl_433 classes
  receiver.cpp - Wrapper and interface for RadioLib
  rtl_433 - subset of rtl_433 package

*/

#include <rtl_433_ESP.h>

#define RADIOLIB_LOW_LEVEL

#include <RadioLib.h>
#include "decoder.h"

#if defined(RF_MODULE_SCK) && defined(RF_MODULE_MISO) && defined(RF_MODULE_MOSI) && defined(RF_MODULE_CS)
#include <SPI.h>
SPIClass newSPI(VSPI);
#endif

#ifdef RF_SX1276
SX1276 radio = RADIO_LIB_MODULE;
#endif

#ifdef RF_SX1278
SX1278 radio = RADIO_LIB_MODULE;
#endif

#ifdef RF_CC1101
CC1101 radio = RADIO_LIB_MODULE;
#endif

Module *_mod = radio.getMod();

#include "tools/aprintf.h"
#include "log.h"

// ESP32 doesn't define ICACHE_RAM_ATTR
#ifndef ICACHE_RAM_ATTR
#define ICACHE_RAM_ATTR IRAM_ATTR
#endif


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

// r_cfg_t rtl_433_ESP::g_cfg;

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

  rtlSetup();

// ESP32 can use VSPI, but heltec uses MOSI=27, MISO=19, SCK=5, CS=18
#if defined(RF_MODULE_SCK) && defined(RF_MODULE_MISO) && defined(RF_MODULE_MOSI) && defined(RF_MODULE_CS)
  logprintfLn(LOG_INFO, STR_MODULE " SPI Config SCK: %d, MISO: %d, MOSI: %d, CS: %d", RF_MODULE_SCK, RF_MODULE_MISO, RF_MODULE_MOSI, RF_MODULE_CS);
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
    logprintfLn(LOG_INFO, STR_MODULE " radio.begin() success!");
  }
  else
  {
    logprintfLn(LOG_ERR, STR_MODULE " failed initialization, code: %d", state);
    while (true)
      ;
  }

  state = radio.setOOK(true);
  if (state == RADIOLIB_ERR_NONE)
  {
    logprintfLn(LOG_INFO, STR_MODULE " setOOK - success!");
  }
  else
  {
    logprintfLn(LOG_ERR, STR_MODULE " setOOK failed, code: %d", state);
    while (true)
      ;
  }

#ifndef RF_CC1101
  state = radio.setDataShapingOOK(2);     // Default 0 ( 0, 1, 2 )
  if (state == RADIOLIB_ERR_NONE)
  {
    logprintfLn(LOG_INFO, STR_MODULE " setDataShapingOOK - success!");
  }
  else
  {
    logprintfLn(LOG_ERR, STR_MODULE " setDataShapingOOK failed, code: %d", state);
    while (true)
      ;
  }

  state = radio.setOokThresholdType(RADIOLIB_SX127X_OOK_THRESH_PEAK);   // Peak is default
  if (state == RADIOLIB_ERR_NONE)
  {
    logprintfLn(LOG_INFO, STR_MODULE " setOokThresholdType - success!");
  }
  else
  {
    logprintfLn(LOG_ERR, STR_MODULE " setOokThresholdType failed, code: %d", state);
    while (true)
      ;
  }

  state = radio.setOokPeakThresholdDecrement(RADIOLIB_SX127X_OOK_PEAK_THRESH_DEC_1_8_CHIP);   // default
  if (state == RADIOLIB_ERR_NONE)
  {
    logprintfLn(LOG_INFO, STR_MODULE " setOokPeakThresholdDecrement - success!");
  }
  else
  {
    logprintfLn(LOG_ERR, STR_MODULE " setOokPeakThresholdDecrement failed, code: %d", state);
    while (true)
      ;
  }

  state = radio.setOokFixedOrFloorThreshold(RADIOLIB_SX127X_OOK_FIXED_THRESHOLD);           // Default
  if (state == RADIOLIB_ERR_NONE)
  {
    logprintfLn(LOG_INFO, STR_MODULE " setOokFixedOrFloorThreshold - success!");
  }
  else
  {
    logprintfLn(LOG_ERR, STR_MODULE " setOokFixedOrFloorThreshold failed, code: %d", state);
    while (true)
      ;
  }

  state = radio.setRSSIConfig(RADIOLIB_SX127X_RSSI_SMOOTHING_SAMPLES_256);  // Default 8 ( 2, 4, 8, 16, 32, 64, 128, 256)
  if (state == RADIOLIB_ERR_NONE)
  {
    logprintfLn(LOG_INFO, STR_MODULE " setRSSIConfig - success!");
  }
  else
  {
    logprintfLn(LOG_ERR, STR_MODULE " setRSSIConfig failed, code: %d", state);
    while (true)
      ;
  }

/*

  state = radio.setPreambleLength(0);                 // Default 
  if (state == RADIOLIB_ERR_NONE)
  {
    logprintfLn(LOG_INFO, STR_MODULE " setPreambleLength - success!");
  }
  else
  {
    logprintfLn(LOG_ERR, STR_MODULE " setPreambleLength failed, code: %d", state);
    while (true)
      ;
  }

  */

  state = _mod->SPIsetRegValue(RADIOLIB_SX127X_REG_PREAMBLE_DETECT, RADIOLIB_SX127X_PREAMBLE_DETECTOR_OFF);
  if (state == RADIOLIB_ERR_NONE)
  {
    logprintfLn(LOG_INFO, STR_MODULE " PREAMBLE_DETECT - success!");
  }
  else
  {
    logprintfLn(LOG_ERR, STR_MODULE " PREAMBLE_DETECT failed, code: %d", state);
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
    logprintfLn(LOG_INFO, STR_MODULE " setBitRate - success!");
  }
  else
  {
    logprintfLn(LOG_ERR, STR_MODULE " setBitRate failed, code: %d", state);
    while (true)
      ;
  }

  /* 
  state = radio.setFrequencyDeviation(100);
  if (state == RADIOLIB_ERR_NONE)
  {
    logprintfLn(LOG_INFO, STR_MODULE " setFrequencyDeviation - success!");
  }
  else
  {
    logprintfLn(LOG_ERR, STR_MODULE " setFrequencyDeviation failed, code: %d", state);
    while (true)
      ;
  }

  */

  state = radio.setRxBandwidth(250);    // Maximum
  if (state == RADIOLIB_ERR_NONE)
  {
    logprintfLn(LOG_INFO, STR_MODULE " setRxBandwidth - success!");
  }
  else
  {
    logprintfLn(LOG_ERR, STR_MODULE " setRxBandwidth failed, code: %d", state);
    while (true)
      ;
  }

#endif

  state = radio.setCrcFiltering(false);

  if (state == RADIOLIB_ERR_NONE)
  {
    logprintfLn(LOG_INFO, STR_MODULE " setCrcFiltering - success!");
  }
  else
  {
    logprintfLn(LOG_ERR, STR_MODULE " setCrcFiltering failed, code: %d", state);
    while (true)
      ;
  }

#ifndef RF_CC1101

  state = radio.setDirectSyncWord(0, 0);      // Disable
  if (state == RADIOLIB_ERR_NONE)
  {
    logprintfLn(LOG_INFO, STR_MODULE " setDirectSyncWord - success!");
  }
  else
  {
    logprintfLn(LOG_ERR, STR_MODULE " setDirectSyncWord failed, code: %d", state);
    while (true)
      ;
  }

  state = radio.disableBitSync();
  if (state == RADIOLIB_ERR_NONE)
  {
    logprintfLn(LOG_INFO, STR_MODULE " disableBitSync - success!");
  }
  else
  {
    logprintfLn(LOG_ERR, STR_MODULE " disableBitSync failed, code: %d", state);
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
    logprintfLn(LOG_INFO, STR_MODULE " receiveDirect - success!");
  }
  else
  {
    logprintfLn(LOG_ERR, STR_MODULE " receiveDirect failed, code: %d", state);
    while (true)
      ;
  }
  getModuleStatus();
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

  // Getting this type of data from SC127x / LaCrosse -334+619 -327+0-326 +333-321+0-327
  if (duration > MINIMUM_PULSE_LENGTH)  // if (duration > MINIMUM_PULSE_LENGTH && currentRssi > minimumRssi)
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
      if (pulse[_nrpulses] > 0) // Did we collect a + pulse ?
      {
        if (_nrpulses > -1)
        {
          gap[_nrpulses] = duration;
        }
        _nrpulses = (uint16_t)((_nrpulses + 1) % PD_MAX_PULSES);
      }
      else if (_nrpulses > 1)
      { // Have we received any data ?
        // We received a random positive blib
        gap[_nrpulses - 1] += duration;
      }
      else
      {
        if (_nrpulses > -1)
        {
          gap[_nrpulses] = duration;
        }
        _nrpulses = (uint16_t)((_nrpulses + 1) % PD_MAX_PULSES);
      }
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

#ifdef AVERAGE_RSSI
    rssiCount++;
    totalRssi += currentRssi;

    if (rssiCount > AVERAGE_RSSI)
    {
      alogprintfLn(LOG_INFO, "\nAverage Signal %d dbm", totalRssi / rssiCount);
      totalRssi = 0;
      rssiCount = 0;
    }
#endif

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
    else if ((micros() - signalEnd < 40000 && micros() - signalStart > 30000) || (micros() - signalEnd < 150000))
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
      processSignal(rtl_pulses);
    }
    free(rtl_pulses);
  }
}

rtl_433_ESPCallBack _callback;        // TODO: Use global object
char *_messageBuffer; 
int _bufferSize;

void rtl_433_ESP::setCallback(rtl_433_ESPCallBack callback, char *messageBuffer, int bufferSize)
{
  // logprintfLn(LOG_DEBUG, "rtl_433_ESP::setCallback location: %p", callback);
  _callback = callback;
  _messageBuffer = messageBuffer;
  _bufferSize = bufferSize;
  _setCallback(callback, messageBuffer, bufferSize);
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

void rtl_433_ESP::setMinimumRSSI(int newRssi)
{
  minimumRssi = newRssi;
  logprintfLn(LOG_INFO, "Setting minimum RSSI to: %d", minimumRssi);
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
  // r_cfg_t *cfg = &g_cfg;

  // data_print_jsons(data, cfg->messageBuffer, cfg->bufferSize);
  // (cfg->callback)(cfg->messageBuffer);
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

void rtl_433_ESP::getModuleStatus() {
  RF_MODULE_GETSTATUS;
}

void rtl_433_ESP::getCC1101Status()
{
#ifdef RF_CC1101
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

  alogprintfLn(LOG_INFO, "----- CC1101 Status -----");
#endif
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