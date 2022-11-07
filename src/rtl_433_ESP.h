/*
  rtl_433_ESP - 433.92 MHz protocols library for ESP32

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

#ifndef rtl_433_ESP_H
#define rtl_433_ESP_H

#include <Arduino.h>
#include <functional>
#include "rtl_433.h"

#ifndef ONBOARD_LED
#define ONBOARD_LED -1
#endif

#ifndef MINRSSI
#define MINRSSI -82 // DB above noise level
#endif

#ifndef NO_DEAF_WORKAROUND
#define DEAF_WORKAROUND
#endif

#ifndef RSSI_SAMPLES // Number of rssi results to collect for average calculation
#define RSSI_SAMPLES 50000
#endif

#ifndef RSSI_THRESHOLD //  Amount to add to average RSSI to determine if a signal is present
#define RSSI_THRESHOLD 9
#endif

#ifndef DISABLERSSITHRESHOLD
#define AUTORSSITHRESHOLD true // Enable setting of RSSI Signal Threshodl based on backgroup signal level
#endif

// #define AUTOOOKFIX true      // Has shown to be problematic

#define RECEIVER_BUFFER_SIZE 2 // Pulse train buffer count
// #define MAXPULSESTREAMLENGTH 750 // Pulse train buffer size
#define MINIMUM_PULSE_LENGTH 50 // signals shorter than this are ignored in interupt handler

// SX127X OOK Reception Floor
#ifndef OOK_FIXED_THRESHOLD
#define OOK_FIXED_THRESHOLD 90 // Default value after a bit of experimentation
#endif

#ifdef WIFI_LoRa_32_V2 // Heltec ESP32 Lora Board
#define RF_SX1278 "SX1278"
#endif

#ifdef RF_SX1276
#define RF_MODULE_RECEIVER_GPIO RF_MODULE_DIO2
#define STR_MODULE "SX1276"
#if defined(RF_MODULE_SCK) && defined(RF_MODULE_MISO) && defined(RF_MODULE_MOSI) && defined(RF_MODULE_CS)
#define RADIO_LIB_MODULE new Module(RF_MODULE_CS, RF_MODULE_DIO0, RF_MODULE_RST, RF_MODULE_DIO1, newSPI)
#else
#define RADIO_LIB_MODULE new Module(RF_MODULE_CS, RF_MODULE_DIO0, RF_MODULE_RST, RF_MODULE_DIO1)
#endif
#endif

#ifdef RF_SX1278
#define STR_MODULE "SX1278"
#ifndef RF_MODULE_DIO2
#define RF_MODULE_RECEIVER_GPIO DIO2
#else
#define RF_MODULE_RECEIVER_GPIO RF_MODULE_DIO2
#endif
#if defined(RF_MODULE_SCK) && defined(RF_MODULE_MISO) && defined(RF_MODULE_MOSI) && defined(RF_MODULE_CS)
#define RADIO_LIB_MODULE new Module(RF_MODULE_CS, RF_MODULE_DIO0, RF_MODULE_RST, RF_MODULE_DIO1, newSPI)
#else
#define RADIO_LIB_MODULE new Module(SS, DIO0, RST_LoRa, DIO1) // defaults from heltec_wifi_lora_32_V2
#endif
#endif

#ifdef RF_CC1101
#define RF_MODULE_RECEIVER_GPIO RF_MODULE_GDO0
#define STR_MODULE "CC1101"
#if defined(RF_MODULE_SCK) && defined(RF_MODULE_MISO) && defined(RF_MODULE_MOSI) && defined(RF_MODULE_CS)
#define RADIO_LIB_MODULE new Module(RF_MODULE_CS, RF_MODULE_GDO0, RADIOLIB_NC, RF_MODULE_GDO2, newSPI)
#else
#define RADIO_LIB_MODULE new Module(SS, RF_MODULE_GDO0, RADIOLIB_NC, RF_MODULE_GDO2)
#endif
#endif

#ifdef REGOOKFIX_DEBUG
#define RADIOLIB_STATE(STATEVAR, FUNCTION)                                         \
  {                                                                                \
    if ((STATEVAR) == RADIOLIB_ERR_NONE)                                           \
    {                                                                              \
      logprintfLn(LOG_INFO, STR_MODULE " " FUNCTION " - success!");                \
    }                                                                              \
    else                                                                           \
    {                                                                              \
      logprintfLn(LOG_ERR, STR_MODULE " " FUNCTION " failed, code: %d", STATEVAR); \
      while (true)                                                                 \
        ;                                                                          \
    }                                                                              \
  }
#else
#define RADIOLIB_STATE(STATEVAR, FUNCTION)                                         \
  {                                                                                \
    if ((STATEVAR) != RADIOLIB_ERR_NONE)                                           \
    {                                                                              \
      logprintfLn(LOG_ERR, STR_MODULE " " FUNCTION " failed, code: %d", STATEVAR); \
      while (true)                                                                 \
        ;                                                                          \
    }                                                                              \
  }
#endif
/**
 * message - JSON formated message from device
 */
typedef void (*rtl_433_ESPCallBack)(char *message);

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
   * (char *message)
   * message - JSON formated message from device
   */
  void setCallback(rtl_433_ESPCallBack callback, char *messageBuffer, int bufferSize);

  /**
   * Set minumum RSSI value for receiver
   */
  void setRSSIThreshold(int);

#if defined(RF_SX1276) || defined(RF_SX1278)
  /**
   * Set setOOKThreshold
   *
   */

  void setOOKThreshold(int);
#endif

  /**
   * Initialise receiver
   *
   * inputPin         - CC1101 gpio Receiver pin
   * receiveFrequency - CC1101 Receive frequency
   */
  static void initReceiver(byte inputPin, float receiveFrequency);

  /**
   * Enable Receiver. No need to call enableReceiver() after initReceiver().
   */
  static void enableReceiver(byte);

  /**
   * Disable decoding. You can re-enable decoding by calling enableReceiver();
   */
  static void disableReceiver();

  /**
   * For SX127x transceiver module, Optimizing the OOK Floor Threshold
   */
  static void calibrateOokFixedThreshold();

  /**
   * set rtl_433 device debug level
   */
  static void setDebug(int);

  /**
   * trigger a debug/internal message from the device
   */
  static void getStatus(int);

  static void getModuleStatus();

  /**
   * Number of messages received since most recent device startup
   */
  static int messageCount;

  /**
   * Current rssi from cc1101, updated at start of every loop
   */
  static int currentRssi;

  /**
   * rssi from start of current signal
   */
  static int signalRssi;

  /**
   * Minimum rssi value to start signal receive process
   */
  static int rssiThreshold;

  static int rssiThresholdDelta;

  static int averageRssi;

  /**
   * rtlDebug
   * 0=normal
   * 1=verbose
   * 2=verbose decoders
   * 3=debug decoders
   * 4=trace decoding
   */
  static int rtlVerbose;

  // Variables for auto calibrate function

  static int totalSignals;
  static int ignoredSignals;
  static int unparsedSignals;

  static uint8_t OokFixedThreshold;

private:
  int8_t _outputPin;

  /**
   * interruptHandler is called on every change in the input
   * signal. If RcPilight::initReceiver is called with interrupt <0,
   * you have to call interruptHandler() yourself. (Or use
   * InterruptChain)
   */
  static void interruptHandler();

  /**
   * interruptHandler used to calibrate OOK floor threshold
   */
  static void calibrateOokFixedThresholdHandler();

  /**
   * Quasi-reset. Called when the current edge is too long or short.
   * reset "promotes" the current edge as being the first edge of a new
   * sequence.
   */
  static void resetReceiver();

  static int _getRSSI();

  /**
   * Get last received PulseTrain.
   * Returns: last PulseTrain or 0 if not avaiable
   */
  static int receivePulseTrain();

  /**
   * _enabledReceiver: If true, monitoring and decoding is enabled.
   * If false, interruptHandler will return immediately.
   */
  static bool _enabledReceiver;
  // static volatile pulse_data_t _pulseTrains[];
  static volatile uint8_t _actualPulseTrain;
  static uint8_t _avaiablePulseTrain;
  static volatile unsigned long _lastChange;
  static volatile int16_t _nrpulses;
  static int16_t _interrupt;

  static void rtl_433_ReceiverTask(void *pvParameters);

  // static r_cfg_t g_cfg;
};

#endif
