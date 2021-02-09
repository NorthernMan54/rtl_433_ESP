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
#define MINRSSI -75
#endif

#define RF_RECEIVER_GPIO 4

#ifndef RECEIVER_BUFFER_SIZE
#define RECEIVER_BUFFER_SIZE 10
#endif

#ifndef MAXPULSESTREAMLENGTH
#define MAXPULSESTREAMLENGTH 1024
#endif

#define MAX_PULSE_TYPES 16

enum PilightRepeatStatus_t
{
  FIRST,
  INVALID,
  VALID,
  KNOWN
};

typedef struct PulseTrain_t
{
  uint16_t pulses[MAXPULSESTREAMLENGTH];
  boolean pins[MAXPULSESTREAMLENGTH];
  uint16_t length;
  unsigned long duration;
} PulseTrain_t;

typedef std::function<void(const String &protocol, const String &message,
                           int status, size_t repeats, const String &deviceID)>
    rtl_433_ESPCallBack;
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
   * Transmit pulse train
   */
  void sendPulseTrain(const uint16_t *pulses, size_t length,
                      size_t repeats = 10);

  /**
   * Transmit Pilight json message
   * repeats of 0 means repeats as defined in protocol.
   */
  int send(const String &protocol, const String &json, size_t repeats = 0);

  /**
   * Parse pulse train and fire callback
   */
  size_t parsePulseTrain(uint16_t *pulses, uint8_t length);

  /**
   * Process receiver queue and fire callback
   */
  void loop();

  void setCallback(rtl_433_ESPCallBack callback);
  void setPulseTrainCallBack(PulseTrainCallBack rawCallback);

  /**
   * If set to true, the receiver will temporarely be disabled when sending.
   */
  void setEchoEnabled(bool enabled);

  /**
   * Initialise receiver
   */
  static void initReceiver(byte inputPin);

  /**
   * Get last received PulseTrain.
   * Returns: length of PulseTrain or 0 if not avaiable
   */
  static uint16_t receivePulseTrain(uint16_t *pulses, boolean *pins);

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

  /**
   * Limit the available protocols.
   *
   * This gets a json array of the protocol names that should be activated.
   * If the array is empty, the filter gets reset.
   */
  static void limitProtocols(const String &protos);

  /**
   * Return a json array containing all the available protocols.
   */
  static String availableProtocols();

  /**
   * Return an json array containing all the currently enabled protocols
   */
  static String enabledProtocols();

  /**
   * Set pilight error output Print class (default is Serial)
   */
  static void setErrorOutput(Print &output);

  static uint8_t minrawlen;
  static uint8_t maxrawlen;
  static uint16_t mingaplen;
  static uint16_t maxgaplen;
  static uint16_t minpulselen;
  static uint16_t maxpulselen;

  static String pulseTrainToString(const uint16_t *pulses, size_t length);
  static int stringToPulseTrain(const String &data, uint16_t *pulses,
                                size_t maxlength);
  static int stringToRepeats(const String &data);

  static int createPulseTrain(uint16_t *pulses, const String &protocol_id,
                              const String &json);

  /**
   * Error return codes for send() and createPulseTrain()
   */
  static const int ERROR_UNAVAILABLE_PROTOCOL = 0;
  static const int ERROR_INVALID_PILIGHT_MSG = -1;
  static const int ERROR_INVALID_JSON = -2;
  static const int ERROR_NO_OUTPUT_PIN = -3;

  /**
   * Error return codes for stringToPulseTrain()
   */
  static const int ERROR_INVALID_PULSETRAIN_MSG_C = -1;
  static const int ERROR_INVALID_PULSETRAIN_MSG_P = -2;
  static const int ERROR_INVALID_PULSETRAIN_MSG_END = -3;
  static const int ERROR_INVALID_PULSETRAIN_MSG_TYPE = -4;
  static const int ERROR_INVALID_PULSETRAIN_MSG_R = -5;

private:
  rtl_433_ESPCallBack _callback;
  PulseTrainCallBack _rawCallback;
  int8_t _outputPin;
  bool _echoEnabled;

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
  static volatile PulseTrain_t _pulseTrains[];
  static volatile uint8_t _actualPulseTrain;
  static uint8_t _avaiablePulseTrain;
  static volatile unsigned long _lastChange; // Timestamp of previous edge
  static volatile uint16_t _nrpulses;
  static int16_t _interrupt;
  static volatile bool receiveMode;
  static volatile unsigned long signalStart;

  static uint16_t pulses[MAXPULSESTREAMLENGTH];
  static boolean pins[MAXPULSESTREAMLENGTH];

  // rtl_433

  static r_cfg_t g_cfg;
};

#endif
