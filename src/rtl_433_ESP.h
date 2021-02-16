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

#ifndef rtl_433_ESP_H
#define rtl_433_ESP_H

#include <Arduino.h>
#include <functional>
#include "rtl_433.h"

#ifndef CC1101_FREQUENCY
#define CC1101_FREQUENCY 433.92
#endif

#ifndef MINRSSI
#define MINRSSI -78 // DB above noise level
#endif

#define RF_RECEIVER_GPIO 4

#define RECEIVER_BUFFER_SIZE 3  // Pulse train buffer count
#define MAXPULSESTREAMLENGTH 750 // Pulse train buffer size
#define MINIMUM_PULSE_LENGTH 50  // signals shorter than this are ignored in interupt handler

typedef struct RTL433PulseTrain_t
{
  uint16_t pulses[MAXPULSESTREAMLENGTH];
  boolean pins[MAXPULSESTREAMLENGTH];
#ifdef RSSI
  int rssi[MAXPULSESTREAMLENGTH];
#endif
  uint16_t length;
  unsigned long duration;
  int signalRssi;
} RTL433PulseTrain_t;

/**
  * (char *name, char *message, unsigned int modulation)
   * name - name of device as defined in device signal decoder
   * message - JSON formated message from device
   * modulation - Type of signal modulation ( r_device.h )
   */
typedef void (*rtl_433_ESPCallBack)(char *name, char *message, unsigned int modulation);

typedef std::function<void(const uint16_t *pulses, size_t length)>
    PulseTrainCallBack;

class rtl_433_ESP
{
public:
  /**
   * Constructor.
   */
  rtl_433_ESP(int8_t outputPin);

  /**
   * Parse pulse train and fire callback
   */
  size_t parsePulseTrain(uint16_t *pulses, uint8_t length);

  /**
   * Monitor receiver for signals and enable / disable signal decoder
   */
  void loop();

  /**
   * Set message received callback function
   * 
   * callback      - message received function callback
   * messageBuffer - message received buffer
   * bufferSize    - size of message received buffer
   * 
   * callback function signature
   * 
   * (char *name, char *message, unsigned int modulation)
   * name - name of device as defined in device signal decoder
   * message - JSON formated message from device
   * modulation - Type of signal modulation ( r_device.h )
   */
  void setCallback(rtl_433_ESPCallBack callback, char *messageBuffer, int bufferSize);

 /**
   * Set minumum RSSI value for receiver
   */
  void setMinimumRSSI(int);
  
  /**
   * Initialise receiver
   * 
   * inputPin         - CC1101 gpio Receiver pin
   * receiveFrequency - CC1101 Receive frequency
   */
  static void initReceiver(byte inputPin, float receiveFrequency);

  /**
   * Get last received PulseTrain.
   * Returns: last PulseTrain or 0 if not avaiable
   */
 volatile RTL433PulseTrain_t* receivePulseTrain();

  /**
   * Check if new PulseTrain avaiable.
   * Returns: 0 if no new PulseTrain avaiable
   */
  static uint16_t nextPulseTrainLength();

  /**
   * Enable Receiver. No need to call enableReceiver() after initReceiver().
   */
  static void enableReceiver(byte);

  /**
   * Disable decoding. You can re-enable decoding by calling enableReceiver();
   */
  static void disableReceiver();

  /**
   * interruptHandler is called on every change in the input
   * signal. If RcPilight::initReceiver is called with interrupt <0,
   * you have to call interruptHandler() yourself. (Or use
   * InterruptChain)
   */
  static void interruptHandler();

private:
  int8_t _outputPin;

  /**
   * Quasi-reset. Called when the current edge is too long or short.
   * reset "promotes" the current edge as being the first edge of a new
   * sequence.
   */
  static void resetReceiver();

  /**
   * Internal functions
   */
  static bool _enabledReceiver; // If true, monitoring and decoding is
                                // enabled. If false, interruptHandler will
                                // return immediately.
  static volatile RTL433PulseTrain_t _pulseTrains[];
  static volatile uint8_t _actualPulseTrain;
  static uint8_t _avaiablePulseTrain;
  static volatile unsigned long _lastChange; // Timestamp of previous edge
  static volatile uint16_t _nrpulses;
  static int16_t _interrupt;
  // static volatile bool receiveMode;
  // static volatile unsigned long signalStart

  // rtl_433

  static r_cfg_t g_cfg;
};

#ifdef MY_DEVICES
#define NUMOFDEVICES 4
#else
#define NUMOFDEVICES 178
#endif

#endif
