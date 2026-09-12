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

#include "log.h"
#include "tools/aprintf.h"

// ESP32 doesn't define ICACHE_RAM_ATTR
#ifndef ICACHE_RAM_ATTR
#  define ICACHE_RAM_ATTR IRAM_ATTR
#endif

// RadioLib Library setup

#define RADIOLIB_LOW_LEVEL 1

#include <RadioLib.h>

/*----------------------------- Optional Compiler Definitions -----------------------------*/

#ifndef ONBOARD_LED
// #define ONBOARD_LED -1
#endif

#ifndef MINRSSI
#  define MINRSSI -82 // DB above noise level
#endif

// Workaround for CC1101 transceivers going deaf occasionally
#ifndef NO_DEAF_WORKAROUND
#  define DEAF_WORKAROUND
#endif

// Number of rssi results to collect for average calculation
#ifndef RSSI_SAMPLES
#  define RSSI_SAMPLES 50000
#endif

// Number of RSSI samples collected before enabling reception at startup
#ifndef RSSI_INITIAL_SAMPLES
#  define RSSI_INITIAL_SAMPLES 1000
#endif

//  Amount to add to average RSSI to determine if a signal is present
#ifndef RSSI_THRESHOLD
#  define RSSI_THRESHOLD 9
#endif

// CC1101 OOK receiver settings. These defaults preserve the effective values
// used by rtl_433_ESP before the settings were made configurable.
#ifndef CC1101_RX_BANDWIDTH
#  define CC1101_RX_BANDWIDTH 812.0f
#endif

#ifndef CC1101_AGCCTRL2
#  define CC1101_AGCCTRL2 0xC7
#endif

#ifndef CC1101_AGCCTRL1
#  define CC1101_AGCCTRL1 0x40
#endif

#ifndef CC1101_AGCCTRL0
#  define CC1101_AGCCTRL0 0xB2
#endif

// Enable setting of RSSI Signal threshold based on backgroup signal level
#ifndef DISABLERSSITHRESHOLD
#  define AUTORSSITHRESHOLD true
#endif

// #define AUTOOOKFIX true      // Has shown to be problematic

// Pulse train buffer count
#define RECEIVER_BUFFER_SIZE 2

// #define MAXPULSESTREAMLENGTH 750 // Pulse train buffer size

// Set to false to enable FSK demodulators ( Experimental )
#ifndef OOK_MODULATION
#  define OOK_MODULATION true
#endif

// signals shorter than this are ignored in interrupt handler

// Overridable from build flags. Defaults are unchanged; only the guards are new.
//
// Without a guard the header's #define wins over any -D on the command line, so
// a protocol whose symbol period or frame duration falls below the defaults
// cannot be received without editing the library.
#if OOK_MODULATION
#  ifndef MINIMUM_PULSE_LENGTH
#    define MINIMUM_PULSE_LENGTH 50
#  endif
#  ifndef MINIMUM_SIGNAL_LENGTH
#    define MINIMUM_SIGNAL_LENGTH 40000
#  endif
#else
#  ifndef MINIMUM_PULSE_LENGTH
#    define MINIMUM_PULSE_LENGTH 30
#  endif
#  ifndef MINIMUM_SIGNAL_LENGTH
#    define MINIMUM_SIGNAL_LENGTH 500
#  endif
#endif

// SX127X OOK Reception Floor
#ifndef OOK_FIXED_THRESHOLD
#  define OOK_FIXED_THRESHOLD 15 // Default value after a bit of experimentation
#endif

#ifndef SX127X_RXBANDWIDTH
#  define SX127X_RXBANDWIDTH 250
#endif

/*----------------------------- Predefined board wiring -----------------------------*/

// Uses platformio board definition from
// ~/.platformio/packages/framework-arduinoespressif32/variants/.../pins_arduino.h

#if defined(WIFI_LoRa_32_V2) || defined(WIFI_LoRa_32) // Heltec ESP32 Lora Board or heltec_wifi_lora_32
#  ifndef RF_SX1278
#    define RF_SX1276 "SX1276"
#  endif
#  ifndef RF_MODULE_DIO0
#    define RF_MODULE_DIO0 DIO0
#  endif
#  ifndef RF_MODULE_DIO1
#    define RF_MODULE_DIO1 DIO1
#  endif
#  ifndef RF_MODULE_DIO2
#    define RF_MODULE_DIO2 DIO2
#  endif
#  ifndef RF_MODULE_RST
#    define RF_MODULE_RST RST_LoRa
#  endif
#  ifndef RF_MODULE_CS
#    define RF_MODULE_CS SS
#  endif
#endif

#if defined(ARDUINO_TTGO_LoRa32_v21new) // LILYGO® Disaster-Radio LoRa V2.1_1.6.1
#  ifndef RF_SX1278
#    define RF_SX1276 "SX1276"
#  endif
#  ifndef RF_MODULE_DIO0
#    define RF_MODULE_DIO0 LORA_IRQ
#  endif
#  ifndef RF_MODULE_DIO1
#    define RF_MODULE_DIO1 LORA_D1
#  endif
#  ifndef RF_MODULE_DIO2
#    define RF_MODULE_DIO2 LORA_D2
#  endif
#  ifndef RF_MODULE_RST
#    define RF_MODULE_RST LORA_RST
#  endif
#  ifndef RF_MODULE_CS
#    define RF_MODULE_CS LORA_CS
#  endif
#endif

#ifdef RF_SX1276
#  define RF_MODULE_RECEIVER_GPIO RF_MODULE_DIO2
#  define STR_MODULE              "SX1276"
#  if defined(RF_MODULE_SCK) && defined(RF_MODULE_MISO) && \
      defined(RF_MODULE_MOSI) && defined(RF_MODULE_CS)
#    define RADIO_LIB_MODULE                                                  \
      new Module(RF_MODULE_CS, RF_MODULE_DIO0, RF_MODULE_RST, RF_MODULE_DIO1, \
                 newSPI)
#  else
#    define RADIO_LIB_MODULE \
      new Module(RF_MODULE_CS, RF_MODULE_DIO0, RF_MODULE_RST, RF_MODULE_DIO1)
#  endif
#endif

#ifdef RF_SX1278
#  define STR_MODULE "SX1278"
#  ifndef RF_MODULE_RECEIVER_GPIO
#    define RF_MODULE_RECEIVER_GPIO RF_MODULE_DIO2
#  endif
#  if defined(RF_MODULE_SCK) && defined(RF_MODULE_MISO) && \
      defined(RF_MODULE_MOSI) && defined(RF_MODULE_CS)
#    define RADIO_LIB_MODULE                                                  \
      new Module(RF_MODULE_CS, RF_MODULE_DIO0, RF_MODULE_RST, RF_MODULE_DIO1, \
                 newSPI)
#  else
#    define RADIO_LIB_MODULE                                  \
      new Module(RF_MODULE_CS, RF_MODULE_DIO0, RF_MODULE_RST, \
                 RF_MODULE_DIO1) // defaults from heltec_wifi_lora_32_V2
#  endif
#endif

#ifdef RF_CC1101
#  ifndef RF_MODULE_RECEIVER_GPIO
#    define RF_MODULE_RECEIVER_GPIO RF_MODULE_GDO0
#  endif
#  define STR_MODULE "CC1101"
#  ifndef RF_MODULE_CS
#    define RF_MODULE_CS SS
#  endif
#  if defined(RF_MODULE_SCK) && defined(RF_MODULE_MISO) && \
      defined(RF_MODULE_MOSI) && defined(RF_MODULE_CS)
#    define RADIO_LIB_MODULE \
      new Module(RF_MODULE_CS, RF_MODULE_GDO0, RADIOLIB_NC, RF_MODULE_GDO2, newSPI)
#  else
#    define RADIO_LIB_MODULE \
      new Module(RF_MODULE_CS, RF_MODULE_GDO0, RADIOLIB_NC, RF_MODULE_GDO2)
#  endif

/*
 * Optional second CC1101 sharing the primary radio's SPI bus. The
 * second radio is receive-only and shares the pulse decoder: its captured
 * trains carry centerfreq_hz so a decode can be attributed to the radio (and
 * frequency) it came from. Enable by defining RF_MODULE2_CS and
 * RF_MODULE2_GDO0; the data input the ESP32 listens on defaults to GDO0 and
 * can be moved with RF_MODULE2_RECEIVER_GPIO (the async serial bitstream is
 * mirrored onto GDO2 as well).
 */
#  if defined(RF_MODULE2_CS) && defined(RF_MODULE2_GDO0)
#    if !(defined(RF_MODULE_SCK) && defined(RF_MODULE_MISO) && \
          defined(RF_MODULE_MOSI) && defined(RF_MODULE_CS))
#      error "RF_MODULE2_* needs the shared-SPI RF_MODULE_SCK/MISO/MOSI/CS pin set"
#    endif
#    define RF_DUAL_CC1101
#    ifndef RF_MODULE2_GDO2
#      define RF_MODULE2_GDO2 RADIOLIB_NC
#    endif
#    ifndef RF_MODULE2_RECEIVER_GPIO
#      define RF_MODULE2_RECEIVER_GPIO RF_MODULE2_GDO0
#    endif
#    define RADIO_LIB_MODULE2                                 \
      new Module(RF_MODULE2_CS, RF_MODULE2_GDO0, RADIOLIB_NC, \
                 RF_MODULE2_GDO2, newSPI)
#  endif
#endif

/*-----------------------------  Logging Macros -----------------------------*/

extern volatile bool rtl_433_radio_config_failed;

#ifdef REGOOKFIX_DEBUG
#  define RADIOLIB_STATE(STATEVAR, FUNCTION)                              \
    {                                                                     \
      if ((STATEVAR) == RADIOLIB_ERR_NONE) {                              \
        logprintfLn(LOG_INFO, STR_MODULE " " FUNCTION " - success!");     \
      } else {                                                            \
        logprintfLn(LOG_ERR, STR_MODULE " " FUNCTION " failed, code: %d", \
                    STATEVAR);                                            \
        rtl_433_radio_config_failed = true;                               \
      }                                                                   \
    }
#else
#  define RADIOLIB_STATE(STATEVAR, FUNCTION)                              \
    {                                                                     \
      if ((STATEVAR) != RADIOLIB_ERR_NONE) {                              \
        logprintfLn(LOG_ERR, STR_MODULE " " FUNCTION " failed, code: %d", \
                    STATEVAR);                                            \
        rtl_433_radio_config_failed = true;                               \
      }                                                                   \
    }
#endif

/**
 * message - JSON formatted message from device
 */
typedef void (*rtl_433_ESPCallBack)(char* message);
typedef void (*rtl_433_raw_pulse_cb)(const int* pulse_us, const int* gap_us,
                                     unsigned int num_pulses,
                                     unsigned long duration_us, int rssi);
typedef struct pulse_data pulse_data_t;

typedef std::function<void(const uint16_t* pulses, size_t length)>
    PulseTrainCallBack;

enum class rtl_433_ESPError : uint8_t {
  None = 0,
  OutOfMemory,
  RadioInitialization,
  QueueCreation,
  TaskCreation,
  InvalidArgument,
};

struct rtl_433_ESPStatus {
  unsigned int capturedSignals;
  unsigned int decodedSignals;
  unsigned int decodedMessages;
  unsigned int zeroDecodedSignals;
  unsigned int droppedCaptureBuffers;
  unsigned int droppedDecoderQueue;
  int currentRssi;
  int averageRssi;
  int rssiThreshold;
  bool receiverEnabled;
};

class rtl_433_ESP {
public:
  /**
   * Constructor.
   */
  rtl_433_ESP();

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
   * message - JSON formatted message from device
   */
  void setCallback(rtl_433_ESPCallBack callback, char* messageBuffer,
                   int bufferSize);

  /**
   * Set callback to receive raw pulse train data for each captured signal.
   * Pointers are valid only during callback execution.
   */
  void setRawPulsesCallback(rtl_433_raw_pulse_cb callback);

  /**
   * Set minimum RSSI value for receiver
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
   * inputPin         - Receiver gpio Receiver pin
   * receiveFrequency - Receiver Receive frequency
   */
  static void initReceiver(byte inputPin, float receiveFrequency);

  /** Initialize the receiver and report failure without halting the device. */
  static bool begin(byte inputPin, float receiveFrequency);

  /** Stop reception and release library-owned tasks, queues, and buffers. */
  static void end();

  static bool isInitialized();
  static rtl_433_ESPError lastError();
  static rtl_433_ESPStatus statusSnapshot();

  /**
   * Frequency the primary receiver is tuned to, in MHz. Set by
   * initReceiver(); a caller that retunes the radio behind the library's back
   * (e.g. a transmit/receive handover) must keep it current, because every
   * captured pulse train is stamped with it (pulse_data_t.centerfreq_hz) and
   * the stamp is what attributes a decode to a frequency.
   */
  static float receiveFrequencyMhz;

  /**
   * Serialise access to the SPI bus shared by the radio(s). The receiver task
   * polls RSSI over SPI from its own core; any other context touching a radio
   * register must hold this lock. Returns false if the lock could not be
   * taken within waitMs. Both are safe to call before initReceiver().
   */
  static bool spiTake(uint32_t waitMs);
  static void spiGive();

#ifdef RF_DUAL_CC1101
  /**
   * Initialise the second CC1101 as an additional receive channel. Call after
   * initReceiver() (the shared SPI bus and the receiver task must exist).
   *
   * inputPin         - GPIO carrying the second radio's demodulated bitstream
   * receiveFrequency - receive frequency in MHz
   *
   * Returns false — and leaves the channel disabled, with lastError() set —
   * if the pulse-train buffers cannot be allocated or the radio rejects its
   * configuration. The primary channel is unaffected either way.
   */
  static bool initSecondaryReceiver(byte inputPin, float receiveFrequency);

  /**
   * Retune the second receive channel. Safe while receiving: capture is
   * paused for the duration and any in-progress train is discarded.
   */
  static void setSecondaryFrequency(float receiveFrequency);

  /**
   * Stop only the primary channel's capture (ISR + RSSI polling) for a
   * transmit handover on the primary radio, leaving the second channel
   * listening. resumePrimaryFromTx() restores capture; putting the primary
   * radio itself back into receive mode remains the caller's job.
   */
  static void suspendPrimaryForTx();
  static void resumePrimaryFromTx();

  static int messageCount2;
  static int currentRssi2;
  static int signalRssi2;
  static int rssiThreshold2;
  static int averageRssi2;
#endif

  /**
   * Enable pulse receiver interrupt and logic
   */
  static void enableReceiver();

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
   * Non functional
   */
  static void setDebug(int);

  /**
   * trigger a debug/internal message from the device
   */
  static void getStatus();

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
   * Functions used during testing
   */
#if defined(setBitrate) || defined(setFreqDev) || defined(setRxBW) || \
    defined(CC1101_OOK_TUNING)
  int16_t setFrequency(float);
  int16_t setFrequencyDeviation(float);
  int16_t setBitRate(float);
  int16_t setRxBandwidth(float);
  int16_t receiveDirect();
#  if defined(RF_CC1101)
  int16_t setCC1101Register(uint8_t address, uint8_t value);
  uint8_t getCC1101Register(uint8_t address);
#  endif
#endif

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

  /** Decoder outcomes accumulated since startup (or an explicit test reset). */
  static volatile unsigned int decoderSignals;
  static volatile unsigned int decodedMessages;
  static volatile unsigned int zeroDecodedSignals;
  static volatile unsigned int droppedCaptureBuffers;
  static volatile unsigned int droppedDecoderQueue;

  static uint8_t OokFixedThreshold;

  /*----------------------------- Future features -----------------------------*/
  /**
 * @brief OOK/FSK Modulation
 * true = OOK
 * false = FSK
 * 
 */
  static bool ookModulation;

  /**
   * @brief change receiver to OOK Modulation
   * Experimental feature, currently not functional
   */
  static bool setOOKModulation();

  /**
   * @brief change receiver to FSK Modulation
   * Experimental feature, currently not functional
   *
   */
  static bool setFSKModulation();

private:
  int8_t _outputPin;

  /**
   * interruptHandler is called on every change in the input
   * signal. If RcPilight::initReceiver is called with interrupt <0,
   * you have to call interruptHandler() yourself. (Or use
   * InterruptChain)
   */
  static void interruptHandler();

#ifdef RF_DUAL_CC1101
  /**
   * Edge handler for the second receive channel's data input.
   */
  static void interruptHandler2();
#endif

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
   * Returns: last PulseTrain or 0 if not available
   */
  static bool receivePulseTrain(pulse_data_t* destination);

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

  static void rtl_433_ReceiverTask(void* pvParameters);

  // static r_cfg_t g_cfg;
};

#endif
