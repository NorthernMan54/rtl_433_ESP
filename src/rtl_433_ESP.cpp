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
#include "ELECHOUSE_CC1101_SRC_DRV.h"
#include "tools/aprintf.h"

// ESP32 doesn't define ICACHE_RAM_ATTR
#ifndef ICACHE_RAM_ATTR
#define ICACHE_RAM_ATTR IRAM_ATTR
#endif

#ifdef DEBUG
#define Debug(x) Serial.print(x)
#define DebugLn(x) Serial.println(x)
#else
#define Debug(x)
#define DebugLn(x)
#endif

#define CC1101_FREQUENCY 433.92

extern "C"
{
// #include "pilight/libs/pilight/protocols/protocol.h"
#include "bitbuffer.h"
#include "pulse_detect.h"
#include "pulse_demod.h"
#include "list.h"
#include "rtl_devices.h"
#include "r_api.h"
#include "r_private.h"
#include "rtl_433.h"
#include "rtl_433_devices.h"
#include "rtl_bridge.h"
}
/*
static protocols_t *used_protocols = nullptr;
*/

bool receiveMode = false;
static unsigned long signalStart = micros();
static unsigned long gapStart = micros();

volatile PulseTrain_t rtl_433_ESP::_pulseTrains[RECEIVER_BUFFER_SIZE];
uint16_t rtl_433_ESP::pulses[MAXPULSESTREAMLENGTH];
boolean rtl_433_ESP::pins[MAXPULSESTREAMLENGTH];

bool rtl_433_ESP::_enabledReceiver;
volatile uint8_t rtl_433_ESP::_actualPulseTrain = 0;
uint8_t rtl_433_ESP::_avaiablePulseTrain = 0;
volatile unsigned long rtl_433_ESP::_lastChange =
    0; // Timestamp of previous edge
volatile uint16_t rtl_433_ESP::_nrpulses = 0;

int16_t rtl_433_ESP::_interrupt = NOT_AN_INTERRUPT;

uint8_t rtl_433_ESP::minrawlen = std::numeric_limits<uint8_t>::max();
uint8_t rtl_433_ESP::maxrawlen = std::numeric_limits<uint8_t>::min();
uint16_t rtl_433_ESP::mingaplen = std::numeric_limits<uint16_t>::max();
uint16_t rtl_433_ESP::maxgaplen = std::numeric_limits<uint16_t>::min();
uint16_t rtl_433_ESP::minpulselen = 50;
uint16_t rtl_433_ESP::maxpulselen = 100000;

unsigned long signalEnd = micros();

r_cfg_t rtl_433_ESP::g_cfg;

void rtl_433_ESP::initReceiver(byte inputPin)
{
  Debug("Pre initReceiver: ");
  DebugLn(ESP.getFreeHeap());
  r_cfg_t *cfg = &g_cfg;

  rtlSetup(cfg);

  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.SetRx(CC1101_FREQUENCY);
  resetReceiver();

  Debug("Post initReceiver: ");
  DebugLn(ESP.getFreeHeap());
}

uint16_t rtl_433_ESP::receivePulseTrain(uint16_t *pulses, boolean *pins)
{
  uint16_t length = nextPulseTrainLength();

  if (length > 0)
  {
    volatile PulseTrain_t &pulseTrain = _pulseTrains[_avaiablePulseTrain];
    _avaiablePulseTrain = (_avaiablePulseTrain + 1) % RECEIVER_BUFFER_SIZE;
    for (uint16_t i = 0; i < length; i++)
    {
      pulses[i] = pulseTrain.pulses[i];
      pins[i] = pulseTrain.pins[i];
    }
    pulseTrain.length = 0;
  }
  return length;
}

uint16_t rtl_433_ESP::nextPulseTrainLength()
{
  return _pulseTrains[_avaiablePulseTrain].length;
}

void ICACHE_RAM_ATTR rtl_433_ESP::interruptHandler()
{
  volatile PulseTrain_t &pulseTrain = _pulseTrains[_actualPulseTrain];
  volatile uint16_t *codes = pulseTrain.pulses;
  volatile boolean *pins = pulseTrain.pins;

  const unsigned long now = micros();
  const unsigned int duration = now - _lastChange;
  // Debug(duration);  Debug(",");

  /* We first do some filtering (same as pilight BPF) */
  if (duration > minpulselen)
  {
    //        if (duration < maxpulselen)
    //        {
    /* All codes are buffered */
    codes[_nrpulses] = (uint16_t)duration;
    if (digitalRead(RF_RECEIVER_GPIO))
    {
      pins[_nrpulses] = true;
      // Debug("+");
    }
    else
    {
      pins[_nrpulses] = false;
      // Debug("-");
    }
    _nrpulses = (uint16_t)((_nrpulses + 1) % MAXPULSESTREAMLENGTH);
    //        }
    _lastChange = now;
  }
}

void rtl_433_ESP::resetReceiver()
{
  for (unsigned int i = 0; i < RECEIVER_BUFFER_SIZE; i++)
  {
    _pulseTrains[i].length = 0;
  }
  _avaiablePulseTrain = 0;
  _actualPulseTrain = 0;
  _nrpulses = 0;

  receiveMode = false;
  signalStart = micros();
}

void rtl_433_ESP::enableReceiver(byte inputPin)
{
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
  }
}

void rtl_433_ESP::disableReceiver() { _enabledReceiver = false; }

void rtl_433_ESP::loop()
{
  if (ELECHOUSE_cc1101.getRssi() > MINRSSI)
  {
    // DebugLn(ELECHOUSE_cc1101.getRssi());
    // enableReceiver(RF_RECEIVER_GPIO);
    if (!receiveMode)
    {
      receiveMode = true;
      signalStart = micros();
      enableReceiver(RF_RECEIVER_GPIO);
    }
    signalEnd = micros();
  }
  // If we received a signal but had a minor drop in strength keep the receiver running for an additional 20,0000
  else if (micros() - signalEnd < 40000 && micros() - signalStart > 30000)
  {
    // skip over signal drop outs
  }
  else
  {
    if (receiveMode)
    {

      // Log.trace(F("Signal length: %d, train: %d, pulses: %d" CR), micros() - signalStart, _actualPulseTrain, _nrpulses);
      receiveMode = false;
      enableReceiver(-1);
      if (_nrpulses > 30 && (micros() - signalStart > 40000))
      {
        DebugLn();

        Debug("Signal length: ");
        Debug(micros() - signalStart);
        Debug(", Gap length: ");
        Debug(signalStart - gapStart);
        Debug(", train: ");
        Debug(_actualPulseTrain);
        Debug(", pulses: ");
        DebugLn(_nrpulses);

        gapStart = micros();
        _pulseTrains[_actualPulseTrain].length = _nrpulses;
        _pulseTrains[_actualPulseTrain].duration = micros() - signalStart;
        _actualPulseTrain = (_actualPulseTrain + 1) % RECEIVER_BUFFER_SIZE;

        _nrpulses = 0;
      }
      else
      {
        // _pulseTrains[_actualPulseTrain].duration = micros() - signalStart;
        // _actualPulseTrain = (_actualPulseTrain + 1) % RECEIVER_BUFFER_SIZE;

        _nrpulses = 0;
      }
    }
  }

  uint16_t length = receivePulseTrain(pulses, pins);

  if (length > 6)
  {
    // bitbuffer_t bitbuffer = {0};

    Debug("RAW (");
    Debug(length);
    Debug("): ");

    pulse_data_t *rtl_pulses = (pulse_data_t *)calloc(1, sizeof(pulse_data_t));

    rtl_pulses->sample_rate = 1.0e6;
    // rtl_pulses->num_pulses = 0;

    for (int i = 0; i < length; i++)
    {
      if (pins[i])
      {
        Debug("+");
        rtl_pulses->gap[rtl_pulses->num_pulses] = pulses[i];
        rtl_pulses->num_pulses++;
      }
      else
      {
        Debug("-");
        rtl_pulses->pulse[rtl_pulses->num_pulses] = pulses[i];
        // rtl_pulses->num_pulses++;
      }
      Debug(pulses[i]);
    }
    DebugLn(" ");
    Debug("Pre run_ook_demods ");
    DebugLn(ESP.getFreeHeap());

    r_cfg_t *cfg = &g_cfg;

    int events = run_ook_demods(&cfg->demod->r_devs, rtl_pulses);
    Debug("Post run_ook_demods ");
    DebugLn(events);
    free(rtl_pulses);
    Debug("Post run_ook_demods ");
    DebugLn(ESP.getFreeHeap());
  }
}

rtl_433_ESP::rtl_433_ESP(int8_t outputPin)
{
  _outputPin = outputPin;
  _callback = nullptr;
  _rawCallback = nullptr;
  _echoEnabled = false;

  if (_outputPin >= 0)
  {
    pinMode((uint8_t)_outputPin, OUTPUT);
    digitalWrite((uint8_t)_outputPin, LOW);
  }
}

void rtl_433_ESP::setCallback(rtl_433_ESPCallBack callback)
{
  _callback = callback;
}

void rtl_433_ESP::setPulseTrainCallBack(PulseTrainCallBack rawCallback)
{
  _rawCallback = rawCallback;
}
