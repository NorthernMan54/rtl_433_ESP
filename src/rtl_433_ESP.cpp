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

#include <rtl_433_ESP.h>

#include "receiver.h"
#include "signalDecoder.h"

/*----------------------------- Transceiver SPI Connections -----------------------------*/

#if defined(RF_MODULE_SCK) && defined(RF_MODULE_MISO) && \
    defined(RF_MODULE_MOSI) && defined(RF_MODULE_CS)
#  include <SPI.h>
#  if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
SPIClass newSPI(FSPI);
#  else
SPIClass newSPI(VSPI);
#  endif
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

#if defined(RF_SX1276) || defined(RF_SX1278)
uint8_t rtl_433_ESP::OokFixedThreshold = OOK_FIXED_THRESHOLD;
#endif

Module* _mod = radio.getMod();

/*----------------------------- rtl_433_ESP Internals -----------------------------*/

#define rtl_433_ReceiverTask_Stack    2048
#define rtl_433_ReceiverTask_Priority 2
#define rtl_433_ReceiverTask_Core     0

/*----------------------------- Initialize variables -----------------------------*/

/**
 * Is the receiver currently receiving a signal
 */
static bool receiveMode = false;

/**
 * Timestamp in micros for start of current signal
 */
static unsigned long signalStart = micros();

/**
 * Timestamp in micros for end of most recent message aka start of current gap
 */
static unsigned long gapStart = micros();

/**
 * Timestamp in micros for end of most recent signal
 *
 */
static unsigned long signalEnd = micros();

pulse_data_t* _pulseTrains;

int rtl_433_ESP::messageCount = 0;
int rtl_433_ESP::currentRssi = 0;
int rtl_433_ESP::signalRssi = 0;
int rtl_433_ESP::rssiThreshold = MINRSSI;
bool rtl_433_ESP::_enabledReceiver = false;
volatile uint8_t rtl_433_ESP::_actualPulseTrain = 0;
uint8_t rtl_433_ESP::_avaiablePulseTrain = 0;
volatile unsigned long rtl_433_ESP::_lastChange = 0; // Timestamp of previous edge
int rtl_433_ESP::rtlVerbose = 0;
volatile int16_t rtl_433_ESP::_nrpulses;

// Variables for OOK Threshold auto calibrate function

int rtl_433_ESP::totalSignals = 0;
int rtl_433_ESP::ignoredSignals = 0;
int rtl_433_ESP::unparsedSignals = 0;
int signalRatio = 0;

// RSSI Threshold and average calculation

int rtl_433_ESP::averageRssi = 0;
int rtl_433_ESP::rssiThresholdDelta = RSSI_THRESHOLD;

bool rtl_433_ESP::ookModulation = OOK_MODULATION; // Defaults to true

int _totalRssi = 0;
int _rssiCount = 0;

int _noiseCount = 0; // Count of ticks while receiver is disabled

#ifdef DEAF_WORKAROUND
unsigned long _deafWorkaround = millis();
#endif

int16_t rtl_433_ESP::_interrupt = NOT_AN_INTERRUPT;
static byte receiverGpio = -1;

static TaskHandle_t rtl_433_ReceiverHandle;

/*----------------------------- End of variable initialization -----------------------------*/

rtl_433_ESP::rtl_433_ESP() {
  _pulseTrains = (pulse_data_t*)heap_caps_calloc(
      RECEIVER_BUFFER_SIZE, sizeof(pulse_data_t), MALLOC_CAP_INTERNAL);
}

/**
 * @brief Initialize Transceiver and rtl_433 decoders
 * 
 * @param inputPin - GPIO of receiver
 * @param receiveFrequency - receive frequency
 */
void rtl_433_ESP::initReceiver(byte inputPin, float receiveFrequency) {
#if defined(RF_SX1276) || defined(RF_SX1278)
  radio.reset();
#endif

  receiverGpio = digitalPinToInterrupt(inputPin);
#ifdef MEMORY_DEBUG
  logprintfLn(LOG_INFO, "Pre initReceiver: %d", ESP.getFreeHeap());
#endif
#ifdef DEMOD_DEBUG
  logprintfLn(LOG_INFO, STR_MODULE " gpio receive pin: %d", inputPin);
  logprintfLn(LOG_INFO, STR_MODULE " receive frequency: %f", receiveFrequency);
#endif

  rtlSetup();

#ifdef MEMORY_DEBUG
  logprintfLn(LOG_INFO, "Post rtlSetup: %d", ESP.getFreeHeap());
#endif

// ESP32 defaults to VSPI, but heltec uses MOSI=27, MISO=19, SCK=5, CS=18
#if defined(RF_MODULE_SCK) && defined(RF_MODULE_MISO) && defined(RF_MODULE_MOSI) && defined(RF_MODULE_CS)
#  ifdef RF_MODULE_INIT_STATUS
  logprintfLn(LOG_INFO, STR_MODULE " SPI Config SCK: %d, MISO: %d, MOSI: %d, CS: %d", RF_MODULE_SCK, RF_MODULE_MISO, RF_MODULE_MOSI, RF_MODULE_CS);
#  endif
  newSPI.begin(RF_MODULE_SCK, RF_MODULE_MISO, RF_MODULE_MOSI, RF_MODULE_CS);
#endif

  /*----------------------------- Initialize Transceiver -----------------------------*/

#ifdef RF_CC1101
  int state = radio.begin();
#else
  int state = radio.beginFSK();
#endif
  RADIOLIB_STATE(state, "radio.begin()");

  radio.setFrequency(receiveFrequency);
  resetReceiver();
#ifdef ONBOARD_LED
  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, LOW);
#endif

  if (ookModulation) {
    state = radio.setOOK(true);
    RADIOLIB_STATE(state, "setOOK");
  } else {
    state = radio.setOOK(false);
    RADIOLIB_STATE(state, "setFSK");
  }

  state = radio.setCrcFiltering(false);
  RADIOLIB_STATE(state, "setCrcFiltering");

#ifdef RF_CC1101
  if (ookModulation) {
    // set mode to standby
    radio.SPIsendCommand(RADIOLIB_CC1101_CMD_IDLE);

    state = radio.SPIsetRegValue(RADIOLIB_CC1101_REG_PKTLEN, 0);
    RADIOLIB_STATE(state, "set PKTLEN");

    // Settings borrowed from lsatan

    state = radio.SPIsetRegValue(RADIOLIB_CC1101_REG_AGCCTRL2, 0xc7);
    RADIOLIB_STATE(state, "set AGCCTRL2");

    state = radio.SPIsetRegValue(RADIOLIB_CC1101_REG_MDMCFG3, 0x93); // Data rate
    RADIOLIB_STATE(state, "set MDMCFG3");

    state = radio.SPIsetRegValue(RADIOLIB_CC1101_REG_MDMCFG4, 0x07); // Bandwidth
    RADIOLIB_STATE(state, "set MDMCFG4");
  } else {
    // From https://github.com/matthias-bs/BresserWeatherSensorReceiver/issues/41#issuecomment-1458166772
    // radio.begin(868.3, 17.24, 40, 270, 10, 32);
    // carrier frequency:                   868.3 MHz
    // bit rate:                            17.24 kbps
    // frequency deviation:                 40 kHz
    // Rx bandwidth:                        270.0 kHz (CC1101) / 250 kHz (SX1276)
    // output power:                        10 dBm
    // preamble length:                     32 bits

    state = radio.setFrequencyDeviation(40); //
    RADIOLIB_STATE(state, "setFrequencyDeviation");

    state = radio.setBitRate(17.24);
    RADIOLIB_STATE(state, "setBitRate");

    state = radio.setRxBandwidth(270); // Sweet spot found from testing
    RADIOLIB_STATE(state, "setRxBandwidth");
  }
  state = radio.disableSyncWordFiltering(false);
  RADIOLIB_STATE(state, "disableSyncWordFiltering");
#endif

#if defined(RF_SX1276) || defined(RF_SX1278)
  if (ookModulation) {
    state = radio.setDataShapingOOK(2); // Default 0 ( 0, 1, 2 )
    RADIOLIB_STATE(state, "setDataShapingOOK");

    state = radio.setOokThresholdType(
        RADIOLIB_SX127X_OOK_THRESH_PEAK); // Peak is default
    RADIOLIB_STATE(state, "OOK Thresh PEAK");

    state = radio.setOokPeakThresholdDecrement(
        RADIOLIB_SX127X_OOK_PEAK_THRESH_DEC_1_1_CHIP); // default
    RADIOLIB_STATE(state, "OOK PEAK Thresh Decrement");

    state = radio.setOokPeakThresholdStep(
        RADIOLIB_SX127X_OOK_PEAK_THRESH_STEP_0_5_DB); // default
    RADIOLIB_STATE(state, "Ook Peak Threshold Step");

    state = radio.setOokFixedOrFloorThreshold(
        OokFixedThreshold); // Default 0x0C RADIOLIB_SX127X_OOK_FIXED_THRESHOLD
    RADIOLIB_STATE(state, "OokFixedThreshold");

    state = radio.setBitRate(1.2);
    RADIOLIB_STATE(state, "setBitRate");

    state = radio.setRxBandwidth(SX127X_RXBANDWIDTH); // Lowering to 125 lowered number of received signals
    RADIOLIB_STATE(state, "setRxBandwidth");

  } else {
    // From https://github.com/matthias-bs/BresserWeatherSensorReceiver/issues/41#issuecomment-1458166772
    // radio.begin(868.3, 17.24, 40, 270, 10, 32);
    // carrier frequency:                   868.3 MHz
    // bit rate:                            17.24 kbps
    // frequency deviation:                 40 kHz
    // Rx bandwidth:                        270.0 kHz (CC1101) / 250 kHz (SX1276)
    // output power:                        10 dBm
    // preamble length:                     32 bits

    state = radio.setFrequencyDeviation(40); //
    RADIOLIB_STATE(state, "setFrequencyDeviation");

    state = radio.setBitRate(17.24);
    RADIOLIB_STATE(state, "setBitRate");

    state = radio.setRxBandwidth(
        83); // Lowering to 125 lowered number of received signals
    RADIOLIB_STATE(state, "setRxBandwidth");
  }
  state = radio.setRSSIConfig(RADIOLIB_SX127X_RSSI_SMOOTHING_SAMPLES_2, RADIOLIB_SX127X_OOK_AVERAGE_OFFSET_0_DB); // Default 8 ( 2, 4, 8, 16, 32,
  // 64, 128, 256)
  RADIOLIB_STATE(state, "RSSI Smoothing");

  state = _mod->SPIsetRegValue(RADIOLIB_SX127X_REG_PREAMBLE_DETECT,
                               RADIOLIB_SX127X_PREAMBLE_DETECTOR_OFF);
  RADIOLIB_STATE(state, "preamble detect off");

  state = radio.setDirectSyncWord(0, 0); // Disable
  RADIOLIB_STATE(state, "setDirectSyncWord");
  if (ookModulation) {
    state = radio.disableBitSync();
    RADIOLIB_STATE(state, "disableBitSync");
  }
#endif

#ifdef MEMORY_DEBUG
  logprintfLn(LOG_INFO, "Post config receivers: %d", ESP.getFreeHeap());
#endif

  // Receviers configured, start reception

#if defined(RF_SX1276) || defined(RF_SX1278)
  state = radio.receiveDirect();
#else
  state = radio.receiveDirectAsync();
#endif
  RADIOLIB_STATE(state, "receiveDirect");

#ifdef RESOURCE_DEBUG
  logprintfLn(LOG_INFO, "rtl_433_ReceiverTask_Stack %d", rtl_433_ReceiverTask_Stack);
#endif

#ifdef RF_MODULE_INIT_STATUS
  getModuleStatus();
#endif

  if (!rtl_433_ReceiverHandle) {
    xTaskCreatePinnedToCore(
        rtl_433_ESP::rtl_433_ReceiverTask, /* Function to implement the task */
        "rtl_433_ReceiverTask", /* Name of the task */
        rtl_433_ReceiverTask_Stack, /* Stack size in bytes */
        NULL, /* Task input parameter */
        rtl_433_ReceiverTask_Priority, /* Priority of the task (set lower than core task) */
        &rtl_433_ReceiverHandle, /* Task handle. */
        rtl_433_ReceiverTask_Core); /* Core where the task should run */
  }
}

/**
 * @brief Is a signal available for decoding ?
 * 
 * @return int - which pulse train
 */
int rtl_433_ESP::receivePulseTrain() {
  if (_pulseTrains[_avaiablePulseTrain].num_pulses > 0) {
    uint8_t _currentTrain = _avaiablePulseTrain;
    _avaiablePulseTrain = (_avaiablePulseTrain + 1) % RECEIVER_BUFFER_SIZE;
    return _currentTrain;
  }
  return -1;
}

/**
 * @brief Main pulse receiver logic
 * 
 */
void ICACHE_RAM_ATTR rtl_433_ESP::interruptHandler() {
  if (!_enabledReceiver || !receiveMode) {
    _noiseCount++;
    return;
  }
  volatile pulse_data_t& pulseTrain = _pulseTrains[_actualPulseTrain];
  volatile int* pulse = pulseTrain.pulse;
  volatile int* gap = pulseTrain.gap;
#ifdef SIGNAL_RSSI
  volatile int* rssi = pulseTrain.rssi;
#endif

  const unsigned long now = micros();
  const unsigned int duration = now - _lastChange;

  /* We first do some filtering (same as pilight BPF) */

#ifdef RF_CC1101
  if (duration > MINIMUM_PULSE_LENGTH && currentRssi > rssiThreshold)
#else
  if (duration > MINIMUM_PULSE_LENGTH) // SX127X RSSI Value drops for a 0 value,
  // and the OOK floor compensates for this
#endif
  {
#ifdef SIGNAL_RSSI
    rssi[_nrpulses] = currentRssi;
#endif
    if (!digitalRead(receiverGpio)) {
      pulse[_nrpulses] = duration;

      //      _nrpulses = (uint16_t)((_nrpulses + 1) % PD_MAX_PULSES);
    } else {
      if (pulse[_nrpulses] > 0) // Did we collect a + pulse ?
      {
        gap[_nrpulses] = duration;

        _nrpulses = (uint16_t)((_nrpulses + 1) % PD_MAX_PULSES);
      } else if (_nrpulses > 1) { // Have we received any data ?
        // We received a random positive blib
        gap[_nrpulses - 1] += duration;
      } else {
        gap[_nrpulses] = duration;

        _nrpulses = (uint16_t)((_nrpulses + 1) % PD_MAX_PULSES);
      }
    }
    _lastChange = now;
  }
}

/**
 * @brief Reset received signal storage
 * 
 */
void rtl_433_ESP::resetReceiver() {
  for (unsigned int i = 0; i < RECEIVER_BUFFER_SIZE; i++) {
    _pulseTrains[i].num_pulses = 0;
  }
  _avaiablePulseTrain = 0;
  _actualPulseTrain = 0;
  _nrpulses = 0;

  receiveMode = false;
  signalStart = micros();
}

/**
 * @brief Enable signal receiver logic
 * 
 * @param inputPin 
 */
void rtl_433_ESP::enableReceiver() {
  if (receiverGpio >= 0) {
    pinMode(receiverGpio, INPUT);
    attachInterrupt((uint8_t)receiverGpio, interruptHandler, CHANGE);
    _enabledReceiver = true;
  }
}

/**
 * @brief Disable receiver logic, and pulse receiver
 * 
 */
void rtl_433_ESP::disableReceiver() {
  _enabledReceiver = false;
  detachInterrupt((uint8_t)receiverGpio);
}

/**
 * @brief watch for completed signals being received, and pass to decoder logic
 * 
 */
void rtl_433_ESP::loop() {
  if (_enabledReceiver) {
#if defined(RF_CC1101) && defined(DEAF_WORKAROUND)
    // workaround for a deaf CC1101, see issue #16
    if (millis() - _deafWorkaround > 3600000) // restart receiver every hour
    {
      _deafWorkaround = millis();
      // radio.SetRx(); // set Receive on
      radio.SPIsendCommand(RADIOLIB_CC1101_CMD_IDLE); // set Receive on
      radio.SPIsendCommand(RADIOLIB_CC1101_CMD_RX); // set Receive on

    } // workaround for a deaf CC1101
#endif

    int _receiveTrain = receivePulseTrain();
    if (_receiveTrain != -1) // Is there anything to receive ?
    {
#ifdef MEMORY_DEBUG
      logprintfLn(LOG_INFO, "Pre copy out of train: %d", ESP.getFreeHeap());
#endif
      pulse_data_t* rtl_pulses = (pulse_data_t*)heap_caps_calloc(1, sizeof(pulse_data_t), MALLOC_CAP_INTERNAL);
      memcpy(rtl_pulses, (char*)&_pulseTrains[_receiveTrain], sizeof(pulse_data_t));
      _pulseTrains[_receiveTrain].num_pulses = 0; // Make pulse train available for next train
      for (int x = 0; x < PD_MAX_PULSES; x++) {
        _pulseTrains[_receiveTrain].pulse[x] = 0;
        _pulseTrains[_receiveTrain].gap[x] = 0;
#ifdef SIGNAL_RSSI
        _pulseTrains[_receiveTrain].rssi[x] = 0;
#endif
      }
#ifdef MEMORY_DEBUG
      logprintfLn(LOG_INFO, "Post copy out of train: %d", ESP.getFreeHeap());
#endif

      if (rtl_pulses->num_pulses > PD_MIN_PULSES) {
        processSignal(rtl_pulses); // send received signal for decoding
      } else {
        ignoredSignals++;
#ifdef MEMORY_DEBUG
        logprintfLn(LOG_INFO, "Pre free copy out of train: %d",
                    ESP.getFreeHeap());
#endif
        free(rtl_pulses);
#ifdef MEMORY_DEBUG
        logprintfLn(LOG_INFO, "Post free copy out of train: %d",
                    ESP.getFreeHeap());
#endif
      }
    }

    // Adjust RegOokFix threshold

    if ((totalSignals % 100) == 0 && totalSignals != 0) {
#ifdef AUTOOOKFIX
#  if defined(RF_SX1276) || defined(RF_SX1278)
      OokFixedThreshold = _mod->SPIreadRegister(RADIOLIB_SX127X_REG_OOK_FIX);
#    ifdef REGOOKFIX_DEBUG
      logprintfLn(LOG_DEBUG,
                  "RegOokFix Threshold Adjust ignoredSignals %d, "
                  "unparsedSignals %d, totalSignals %d, RegOokFix 0x%.2x",
                  ignoredSignals, unparsedSignals, totalSignals,
                  OokFixedThreshold);
#    endif
      if (ignoredSignals >
          unparsedSignals) // too many ignored decrement threshold
      {
        int state = radio.setOokFixedOrFloorThreshold(--OokFixedThreshold);
        RADIOLIB_STATE(state, "OokFixedThreshold");
#    ifdef REGOOKFIX_DEBUG
        logprintfLn(LOG_DEBUG, "RegOokFix Threshold Decremented to 0x%.2x",
                    _mod->SPIreadRegister(RADIOLIB_SX127X_REG_OOK_FIX));
#    endif
      }
#  endif
#endif
      signalRatio = (totalSignals - (ignoredSignals + unparsedSignals)) / totalSignals * 100;

      totalSignals = 0;
      ignoredSignals = 0;
      unparsedSignals = 0;
    }
  }
  vTaskDelay(1);
}

/**
 * @brief Background task to monitor RSSI signal level and start / end signal receiving
 * 
 * @param pvParameters 
 */
void rtl_433_ESP::rtl_433_ReceiverTask(void* pvParameters) {
  for (;;) {
    if (_enabledReceiver) {
      // Calculate average RSSI signal level in environment

      currentRssi = _getRSSI();
      _rssiCount++;
      _totalRssi += currentRssi;

      if (_rssiCount > RSSI_SAMPLES) // Adjust RSSI Signal Threshold
      {
        averageRssi = _totalRssi / _rssiCount;

#ifdef AUTORSSITHRESHOLD
        rssiThreshold = averageRssi + rssiThresholdDelta;
        logprintfLn(LOG_DEBUG,
                    "Average RSSI Signal %d dbm, adjusted RSSI Threshold %d, "
                    "samples %d",
                    averageRssi, rssiThreshold, RSSI_SAMPLES);
#endif

        _totalRssi = 0;
        _rssiCount = 0;
      }

      if (currentRssi > rssiThreshold) // A signal is present
      {
        if (!receiveMode) {
          receiveMode = true;
          signalStart = micros();
#ifdef ONBOARD_LED
          digitalWrite(ONBOARD_LED, HIGH);
#endif
          signalRssi = currentRssi;
          _lastChange = micros();

          if (_noiseCount > 100) {
#ifdef AUTOOOKFIX
#  if defined(RF_SX1276) || defined(RF_SX1278)
            OokFixedThreshold =
                _mod->SPIreadRegister(RADIOLIB_SX127X_REG_OOK_FIX);
#    ifdef REGOOKFIX_DEBUG
            logprintfLn(
                LOG_DEBUG,
                "RegOokFix Threshold Adjust noise count %d, RegOokFix 0x%.2x",
                _noiseCount, OokFixedThreshold);
#    endif
            int state = radio.setOokFixedOrFloorThreshold(++OokFixedThreshold);
            RADIOLIB_STATE(state, "OokFixedThreshold");
#  endif
#endif
            _noiseCount = 0;
          }
        }
        signalEnd = micros();
      }
#if defined(RF_SX1276) || defined(RF_SX1278)
      // If we received a signal but had a minor drop in strength keep the
      // receiver running for an additional 150,000
      else if (micros() - signalEnd < MINIMUM_SIGNAL_LENGTH)
#else
      // If we received a signal but had a minor drop in strength keep the
      // receiver running for an additional 40,000
      else if (micros() - signalEnd < MINIMUM_SIGNAL_LENGTH && micros() - signalStart > 30000)
      // else if (micros() - signalEnd < PD_MAX_GAP_MS)
#endif
      {
        // skip over signal drop outs
      } else // A signal is not present
      {
        if (receiveMode) // Complete reception of a signal
        {
#ifdef ONBOARD_LED
          digitalWrite(ONBOARD_LED, LOW);
#endif
          receiveMode = false;
          totalSignals++;
          if ((_nrpulses > PD_MIN_PULSES) &&
              ((signalEnd - signalStart) >
               MINIMUM_SIGNAL_LENGTH)) // Minimum signal length of MINIMUM_SIGNAL_LENGTH MS
          {
            _pulseTrains[_actualPulseTrain].num_pulses = _nrpulses + 1;
            _pulseTrains[_actualPulseTrain].signalDuration =
                signalEnd - signalStart;
            _pulseTrains[_actualPulseTrain].signalRssi = signalRssi;
#ifdef DEMOD_DEBUG
            logprintf(LOG_INFO, "Signal length: %lu",
                      _pulseTrains[_actualPulseTrain].signalDuration);
            alogprintf(LOG_INFO, ", Gap length: %lu", signalStart - gapStart);
            alogprintf(LOG_INFO, ", Signal RSSI: %d",
                       _pulseTrains[_actualPulseTrain].signalRssi);
            alogprintf(LOG_INFO, ", train: %d", _actualPulseTrain);
            alogprintf(LOG_INFO, ", messageCount: %d", messageCount);
            alogprintfLn(LOG_INFO, ", pulses: %d", _nrpulses);
#endif
            messageCount++;
            gapStart = micros();
            _actualPulseTrain = (_actualPulseTrain + 1) % RECEIVER_BUFFER_SIZE;
            _nrpulses = 0;
          } else {
            ignoredSignals++;
#ifdef DEMOD_DEBUG
            if (micros() - signalStart > 1000) {
              logprintf(LOG_INFO, "Ignored Signal length: %lu",
                        signalEnd - signalStart);

              alogprintf(LOG_INFO, ", Time since last bit length: %lu",
                         micros() - signalEnd);
              alogprintf(LOG_INFO, ", Gap length: %lu", signalStart - gapStart);
              alogprintf(LOG_INFO, ", Signal RSSI: %d", signalRssi);
              alogprintf(LOG_INFO, ", Current RSSI: %d", currentRssi);
              alogprintf(LOG_INFO, ", pulses: %d", _nrpulses);
              alogprintfLn(LOG_INFO, ", noise count: %d", _noiseCount);
              gapStart = micros();
            }
#endif
            _nrpulses = 0;
          }
#ifdef MEMORY_DEBUG
          logprintfLn(LOG_INFO,
                      "rtl_433_ReceiverTask uxTaskGetStackHighWaterMark: %d", uxTaskGetStackHighWaterMark(NULL));
#endif
        }
      }
    }
    vTaskDelay(1);
  }
}

/**
 * @brief Client callback to receive decoded signals
 * 
 * @param callback 
 * @param messageBuffer 
 * @param bufferSize 
 */
rtl_433_ESPCallBack _callback; // TODO: Use global object
char* _messageBuffer;
int _bufferSize;

void rtl_433_ESP::setCallback(rtl_433_ESPCallBack callback, char* messageBuffer,
                              int bufferSize) {
  // logprintfLn(LOG_DEBUG, "rtl_433_ESP::setCallback location: %p", callback);
  _callback = callback;
  _messageBuffer = messageBuffer;
  _bufferSize = bufferSize;
  _setCallback(callback, messageBuffer, bufferSize);
}

/**
 * @brief Set delta applied to average RSSI level for determining start and end of signal
 * 
 * @param newRssi 
 */
void rtl_433_ESP::setRSSIThreshold(int newRssi) {
  rssiThresholdDelta = newRssi;
#ifndef AUTORSSITHRESHOLD
  logprintfLn(LOG_INFO, "RSSI Threshold not available: %d", rssiThresholdDelta);
#else
  logprintfLn(LOG_INFO, "Setting RSSI Threshold Delta to: %d",
              rssiThresholdDelta);
#endif
}

/**
 * @brief set OOK Threshold
 * 
 */
#if defined(RF_SX1276) || defined(RF_SX1278)
void rtl_433_ESP::setOOKThreshold(int newOokThreshold) {
  OokFixedThreshold = newOokThreshold;
#  ifdef REGOOKFIX_DEBUG
  logprintfLn(LOG_INFO, "Setting setOokFixedOrFloorThreshold to: %d",
              OokFixedThreshold);
#  endif

  int state = radio.setOokFixedOrFloorThreshold(OokFixedThreshold);
  RADIOLIB_STATE(state, "setOokFixedThreshold");
}
#endif

/**
 * @brief This does not work
 * 
 * @param debug 
 */
void rtl_433_ESP::setDebug(int debug) {
  rtlVerbose = debug;
  logprintfLn(LOG_INFO, "Setting rtl_433 debug to: %d", rtlVerbose);
}

/**
 * @brief Send RTL_433_ESP status to serial port and client. Also send to serial port transceiver status.
 * 
 * @param status 
 */
void rtl_433_ESP::getStatus() {
  alogprintfLn(LOG_INFO, " ");
  logprintf(LOG_INFO, "Status Message: Gap length: %lu",
            signalStart - gapStart);
  alogprintf(LOG_INFO, ", Modulation: %s", ookModulation ? "OOK" : "FSK");
  alogprintf(LOG_INFO, ", Signal RSSI: %d", signalRssi);
  alogprintf(LOG_INFO, ", train: %d", _actualPulseTrain);
  alogprintf(LOG_INFO, ", messageCount: %d", messageCount);
  alogprintf(LOG_INFO, ", totalSignals: %d", totalSignals);
  alogprintf(LOG_INFO, ", signalRatio: %d", signalRatio);
  alogprintf(LOG_INFO, ", ignoredSignals: %d", ignoredSignals);
  alogprintf(LOG_INFO, ", unparsedSignals: %d", unparsedSignals);
  alogprintf(LOG_INFO, ", _enabledReceiver: %d", _enabledReceiver);
  alogprintf(LOG_INFO, ", receiveMode: %d", receiveMode);
  alogprintf(LOG_INFO, ", currentRssi: %d", currentRssi);
  alogprintf(LOG_INFO, ", rssiThreshold: %d", rssiThreshold);
  alogprintf(LOG_INFO, ", StackHWM: %d", uxTaskGetStackHighWaterMark(NULL));
  alogprintf(LOG_INFO, ", RTL_HWM: %d", uxTaskGetStackHighWaterMark(rtl_433_ReceiverHandle));
  alogprintf(LOG_INFO, ", DCD_HWM: %d", uxTaskGetStackHighWaterMark(rtl_433_DecoderHandle));
  alogprintfLn(LOG_INFO, ", pulses: %d", _nrpulses);

  data_t* data;

  /* clang-format off */
  data = data_make(
                "model",          "", DATA_STRING,  "status",
                "protocol",       "", DATA_STRING,  "rtl_433_ESP status message",
                "modulation",     "", DATA_STRING,  ookModulation ? "OOK" : "FSK",
                "RTLRssi",        "", DATA_INT,     currentRssi,
                "RTLAVGRssi",     "", DATA_INT,     averageRssi,
                "RTLRssiThresh",  "", DATA_INT,     rssiThreshold,
                "signalRssi",     "", DATA_INT,     signalRssi,

#ifdef ZradioSX127x
               "RTLOOKThresh",    "", DATA_INT,     OokFixedThreshold,
#endif

                "train",          "", DATA_INT, _actualPulseTrain,
                "RTLCnt",         "", DATA_INT, messageCount,
                "totalSignals",   "", DATA_INT, totalSignals,
                "signalRatio",    "", DATA_INT, signalRatio,
                "ignoredSignals", "", DATA_INT, ignoredSignals,
                "unparsedSignals", "", DATA_INT, unparsedSignals,
                "StackHWM",       "", DATA_INT, uxTaskGetStackHighWaterMark(NULL),
                "RTL_HWM",        "", DATA_INT, uxTaskGetStackHighWaterMark(rtl_433_ReceiverHandle),
                "DCD_HWM",        "", DATA_INT, uxTaskGetStackHighWaterMark(rtl_433_DecoderHandle),
                "freeMem",        "", DATA_INT, ESP.getFreeHeap(),
                "_enabledReceiver", "", DATA_INT, _enabledReceiver,
                "receiveMode",    "", DATA_INT, receiveMode,
                NULL);
#ifdef RF_MODULE_INIT_STATUS
  getModuleStatus();
#endif

  data_print_jsons(data, _messageBuffer, _bufferSize);
  (_callback)(_messageBuffer);
  data_free(data);
}

/****************************************************************
 *FUNCTION NAME:RSSI Level - Replacement RSSI function for the one in RadioLib
 *that doesn't work in OOK async mode for a FUNCTION     :Calculating the RSSI
 *Level INPUT        :none OUTPUT       :none
 ****************************************************************/
int rtl_433_ESP::_getRSSI(void) {
  int rssi;
#ifdef RF_CC1101
  rssi = radio.getRSSI();
#elif RADIOLIB_VERSION_MAJOR >= 6
  rssi = radio.getRSSI(true, true);
#else
  rssi = radio.getRSSI(true);
#endif
  return rssi;
}

/**
 * Send to serial output current transceiver status
 *
 */
void rtl_433_ESP::getModuleStatus() {
#ifdef RF_CC1101
  alogprintfLn(LOG_INFO, "----- CC1101 Status -----");
  alogprintfLn(LOG_INFO, "CC1101_MDMCFG1: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_MDMCFG1));
  alogprintfLn(LOG_INFO, "CC1101_MDMCFG2: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_MDMCFG2));
  alogprintfLn(LOG_INFO, "CC1101_MDMCFG3: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_MDMCFG3));
  alogprintfLn(LOG_INFO, "CC1101_MDMCFG4: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_MDMCFG4));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_DEVIATN: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_DEVIATN));
  alogprintfLn(LOG_INFO, "CC1101_AGCCTRL0: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_AGCCTRL0));
  alogprintfLn(LOG_INFO, "CC1101_AGCCTRL1: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_AGCCTRL1));
  alogprintfLn(LOG_INFO, "CC1101_AGCCTRL2: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_AGCCTRL2));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_IOCFG0: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_IOCFG0));
  alogprintfLn(LOG_INFO, "CC1101_IOCFG1: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_IOCFG1));
  alogprintfLn(LOG_INFO, "CC1101_IOCFG2: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_IOCFG2));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_FIFOTHR: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_FIFOTHR));
  alogprintfLn(LOG_INFO, "CC1101_SYNC0: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_SYNC0));
  alogprintfLn(LOG_INFO, "CC1101_SYNC1: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_SYNC1));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_PKTLEN: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_PKTLEN));
  alogprintfLn(LOG_INFO, "CC1101_PKTCTRL0: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_PKTCTRL0));
  alogprintfLn(LOG_INFO, "CC1101_PKTCTRL1: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_PKTCTRL1));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_ADDR: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_ADDR));
  alogprintfLn(LOG_INFO, "CC1101_CHANNR: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_CHANNR));
  alogprintfLn(LOG_INFO, "CC1101_FSCTRL0: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_FSCTRL0));
  alogprintfLn(LOG_INFO, "CC1101_FSCTRL1: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_FSCTRL1));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_FREQ0: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_FREQ0));
  alogprintfLn(LOG_INFO, "CC1101_FREQ1: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_FREQ1));
  alogprintfLn(LOG_INFO, "CC1101_FREQ2: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_FREQ2));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_MCSM0: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_MCSM0));
  alogprintfLn(LOG_INFO, "CC1101_MCSM1: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_MCSM1));
  alogprintfLn(LOG_INFO, "CC1101_MCSM2: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_MCSM2));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_FOCCFG: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_FOCCFG));

  alogprintfLn(LOG_INFO, "CC1101_BSCFG: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_BSCFG));
  alogprintfLn(LOG_INFO, "CC1101_WOREVT0: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_WOREVT0));
  alogprintfLn(LOG_INFO, "CC1101_WOREVT1: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_WOREVT1));
  alogprintfLn(LOG_INFO, "CC1101_WORCTRL: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_WORCTRL));
  alogprintfLn(LOG_INFO, "CC1101_FREND0: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_FREND0));
  alogprintfLn(LOG_INFO, "CC1101_FREND1: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_FREND1));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_FSCAL0: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_FSCAL0));
  alogprintfLn(LOG_INFO, "CC1101_FSCAL1: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_FSCAL1));
  alogprintfLn(LOG_INFO, "CC1101_FSCAL2: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_FSCAL2));
  alogprintfLn(LOG_INFO, "CC1101_FSCAL3: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_FSCAL3));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_RCCTRL0: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_RCCTRL0));
  alogprintfLn(LOG_INFO, "CC1101_RCCTRL1: 0x%.2x",
               radio.SPIreadRegister(RADIOLIB_CC1101_REG_RCCTRL1));
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "CC1101_PARTNUM: 0x%.2x",
               radio.SPIgetRegValue(RADIOLIB_CC1101_REG_PARTNUM));
  alogprintfLn(LOG_INFO, "CC1101_VERSION: 0x%.2x",
               radio.SPIgetRegValue(RADIOLIB_CC1101_REG_VERSION));
  alogprintfLn(LOG_INFO, "CC1101_MARCSTATE: 0x%.2x",
               radio.SPIgetRegValue(RADIOLIB_CC1101_REG_MARCSTATE));
  alogprintfLn(LOG_INFO, "CC1101_PKTSTATUS: 0x%.2x",
               radio.SPIgetRegValue(RADIOLIB_CC1101_REG_PKTSTATUS));
  alogprintfLn(LOG_INFO, "CC1101_RXBYTES: 0x%.2x",
               radio.SPIgetRegValue(RADIOLIB_CC1101_REG_RXBYTES));

  alogprintfLn(LOG_INFO, "----- CC1101 Status -----");
#endif
#if defined(RF_SX1276) || defined(RF_SX1278)

  alogprintfLn(LOG_INFO, "----- SX127x Status -----");

  OokFixedThreshold = _mod->SPIreadRegister(RADIOLIB_SX127X_REG_OOK_FIX);

  alogprintfLn(LOG_INFO, "RegOpMode: 0x%.2x",
               _mod->SPIreadRegister(RADIOLIB_SX127X_REG_OP_MODE));
  alogprintfLn(LOG_INFO, "RegPacketConfig1: 0x%.2x",
               _mod->SPIreadRegister(RADIOLIB_SX127X_REG_PACKET_CONFIG_2));
  alogprintfLn(LOG_INFO, "RegPacketConfig2: 0x%.2x",
               _mod->SPIreadRegister(RADIOLIB_SX127X_REG_PACKET_CONFIG_2));
  alogprintfLn(LOG_INFO, "RegBitrateMsb: 0x%.2x",
               _mod->SPIreadRegister(RADIOLIB_SX127X_REG_BITRATE_MSB));
  alogprintfLn(LOG_INFO, "RegBitrateLsb: 0x%.2x",
               _mod->SPIreadRegister(RADIOLIB_SX127X_REG_BITRATE_LSB));
  alogprintfLn(LOG_INFO, "RegRxBw: 0x%.2x",
               _mod->SPIreadRegister(RADIOLIB_SX127X_REG_RX_BW));
  alogprintfLn(LOG_INFO, "RegAfcBw: 0x%.2x",
               _mod->SPIreadRegister(RADIOLIB_SX127X_REG_AFC_BW));
                if (ookModulation) {
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "RegOokPeak: 0x%.2x",
               _mod->SPIreadRegister(RADIOLIB_SX127X_REG_OOK_PEAK));
  alogprintfLn(LOG_INFO, "RegOokFix: 0x%.2x", OokFixedThreshold);
  alogprintfLn(LOG_INFO, "RegOokAvg: 0x%.2x",
               _mod->SPIreadRegister(RADIOLIB_SX127X_REG_OOK_AVG));
                }
  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "RegLna: 0x%.2x",
               _mod->SPIreadRegister(RADIOLIB_SX127X_REG_LNA));
  alogprintfLn(LOG_INFO, "RegRxConfig: 0x%.2x",
               _mod->SPIreadRegister(RADIOLIB_SX127X_REG_RX_CONFIG));
  alogprintfLn(LOG_INFO, "RegRssiConfig: 0x%.2x",
               _mod->SPIreadRegister(RADIOLIB_SX127X_REG_RSSI_CONFIG));

  alogprintfLn(LOG_INFO, "-------------------------");
  alogprintfLn(LOG_INFO, "RegDioMapping1: 0x%.2x",
               _mod->SPIreadRegister(RADIOLIB_SX127X_REG_DIO_MAPPING_1));

 if (!ookModulation) {
  alogprintfLn(LOG_INFO, "----------- FSK --------------");
  alogprintfLn(LOG_INFO, "FDEV_MSB: 0x%.2x",
               _mod->SPIreadRegister(RADIOLIB_SX127X_REG_FDEV_MSB));
  alogprintfLn(LOG_INFO, "FDEV_LSB: 0x%.2x",
               _mod->SPIreadRegister(RADIOLIB_SX127X_REG_FDEV_LSB));
 }
  alogprintfLn(LOG_INFO, "----- SX127x Status -----");

#endif
}

/**
 * Functions used only during testing
 *
 */
#if defined(setBitrate) || defined(setFreqDev) || defined(setRxBW)
int16_t rtl_433_ESP::setFrequencyDeviation(float value) {
  return radio.setFrequencyDeviation(value);
}

int16_t rtl_433_ESP::receiveDirect() {
#if defined(RF_SX1276) || defined(RF_SX1278)
  return radio.receiveDirect();
#else
  return radio.receiveDirectAsync();
#endif
}

int16_t rtl_433_ESP::setBitRate(float value) {
  return radio.setBitRate(value);
}

int16_t rtl_433_ESP::setRxBandwidth(float value) {
  return radio.setRxBandwidth(value);
}
#endif
