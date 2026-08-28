/*
 Basic rtl_433_ESP example for OOK/ASK Devices

 CC1101 tuning build flags:
   -DCC1101_OOK_TUNING
   -DRF_MODULE_FREQUENCY=345.0

 The suite runs frequency, bandwidth, AGCCTRL2, and AGCCTRL0 phases in order,
 automatically carries each winner forward, then validates the final setting.
 Define CC1101_OOK_TUNING_REFINEMENT to use a focused search around the winning
 Vivint settings instead of the broad first-pass search.
 Define CC1101_OOK_TUNING_EXTENDED to characterize the remaining bandwidth,
 AGCCTRL2, AGCCTRL1, and AGCCTRL0 bit fields around the latest candidate.
 Optional starting values:
   -DCC1101_TUNING_BASE_BANDWIDTH=406.0
   -DCC1101_TUNING_BASE_AGCCTRL2=0x03
   -DCC1101_TUNING_BASE_AGCCTRL0=0x91

 Each setting runs for 300 seconds by default. Override with
 CC1101_TUNING_WINDOW_SECONDS and CC1101_TUNING_SIGNAL_INTERVAL_SECONDS.
*/

#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <rtl_433_ESP.h>

#ifndef RF_MODULE_FREQUENCY
#  define RF_MODULE_FREQUENCY 433.92
#endif

#define JSON_MSG_BUFFER            512
#define RAW_CALLBACK_SAMPLE_PULSES 8

char messageBuffer[JSON_MSG_BUFFER];

rtl_433_ESP rf; // use -1 to disable transmitter

volatile int count = 0;

#if defined(CC1101_OOK_TUNING)
#  if !defined(RF_CC1101)
#    error "CC1101_OOK_TUNING requires RF_CC1101"
#  endif
#  ifndef CC1101_TUNING_WINDOW_SECONDS
#    define CC1101_TUNING_WINDOW_SECONDS 300UL
#  endif
#  ifndef CC1101_TUNING_SIGNAL_INTERVAL_SECONDS
#    define CC1101_TUNING_SIGNAL_INTERVAL_SECONDS 30UL
#  endif
#  ifndef CC1101_TUNING_BASE_BANDWIDTH
#    define CC1101_TUNING_BASE_BANDWIDTH 406.0f
#  endif
#  ifndef CC1101_TUNING_BASE_AGCCTRL2
#    define CC1101_TUNING_BASE_AGCCTRL2 0x03
#  endif
#  ifndef CC1101_TUNING_BASE_AGCCTRL0
#    define CC1101_TUNING_BASE_AGCCTRL0 0x91
#  endif

struct CC1101TuningSetting {
  float frequencyMHz;
  float bandwidthKHz;
  uint8_t agcctrl2;
  uint8_t agcctrl1;
  uint8_t agcctrl0;
};

volatile unsigned int rawCount = 0;
volatile unsigned long rawPulseTotal = 0;
volatile long rawRssiTotal = 0;
volatile int rawRssiMin = 127;
volatile int rawRssiMax = -256;

enum CC1101TuningPhase : uint8_t {
  TUNING_FREQUENCY = 1,
  TUNING_BANDWIDTH,
  TUNING_AGCCTRL2,
  TUNING_AGCCTRL0,
  TUNING_AGCCTRL2_DVGA,
  TUNING_AGCCTRL2_LNA,
  TUNING_AGCCTRL0_FILTER,
  TUNING_AGCCTRL0_WAIT,
  TUNING_AGCCTRL0_HYST,
  TUNING_AGCCTRL1_PRIORITY,
  TUNING_FINAL_VALIDATION
};

#  if defined(CC1101_OOK_PROFILE_COMPARE)
// Fixed-frequency A/B profiles. The default values match rtl_433_ESP's
// supplied CC1101 OOK initialization; the refined profile changes only the
// receive bandwidth and AGC registers.
const CC1101TuningSetting profileValues[] = {
#    if defined(CC1101_OOK_ORIGINAL_PROFILE_COMPARE)
    // Original documented Vivint recommendation versus the extended-suite
    // winner selected by the long alternating comparison.
    {345.00f, 162.5f, 0x84, 0x40, 0xA0},
    {345.12f, 325.0f, 0xC1, 0x00, 0x61},
#    elif defined(CC1101_OOK_EXTENDED_PROFILE_COMPARE)
    // Current recommendation versus the complete winner assembled by the
    // extended characterization suite.
    {345.10f, 270.833f, 0x83, 0x40, 0x90},
    {345.12f, 325.0f, 0xC1, 0x00, 0x61},
#    elif defined(CC1101_OOK_AGC2_COMPARE)
    // Direct Vivint comparison of the clean 30 dB magnitude-target result
    // against the current 33 dB candidate. All other settings remain fixed.
    {345.10f, 270.833f, 0x82, 0x40, 0x90},
    {345.10f, 270.833f, 0x83, 0x40, 0x90},
#    else
    // RadioLib accepts 812.0 kHz as the request for the CC1101's actual
    // 812.5 kHz hardware setting (MDMCFG4 channel-bandwidth bits 0x00).
    {433.92f, 812.0f, 0xC7, 0x40, 0xB2},
    {433.92f, 162.5f, 0x84, 0x40, 0xA0},
#    endif
};
#    if defined(CC1101_OOK_ORIGINAL_PROFILE_COMPARE)
const char* const profileNames[] = {"original", "new_recommendation"};
#    elif defined(CC1101_OOK_EXTENDED_PROFILE_COMPARE)
const char* const profileNames[] = {"current", "extended_winner"};
#    elif defined(CC1101_OOK_AGC2_COMPARE)
const char* const profileNames[] = {"agcctrl2_0x82", "agcctrl2_0x83"};
#    else
const char* const profileNames[] = {"default", "refined"};
#    endif
unsigned long profileWindows[] = {0, 0};
unsigned long profileSignals[] = {0, 0};
unsigned long profileDecoded[] = {0, 0};
unsigned long profileZero[] = {0, 0};
#  elif defined(CC1101_OOK_TUNING_EXTENDED)
// Focused characterization around the long-run candidate. Values for the
// split AGC phases are field values and are merged with the winning register
// value from the preceding phase.
const float frequencyValues[] = {345.06f, 345.08f, 345.10f, 345.12f, 345.14f};
const float bandwidthValues[] = {232.143f, 270.833f, 325.0f};
const uint8_t agcctrl2Values[] = {0x01, 0x02, 0x03, 0x04, 0x05};
const uint8_t agcctrl0Values[] = {0x90}; // Not a standalone phase in this suite.
const uint8_t agcctrl2DvgaValues[] = {0x00, 0x40, 0x80, 0xC0};
const uint8_t agcctrl2LnaValues[] = {0x00, 0x08, 0x10};
const uint8_t agcctrl0FilterValues[] = {0x00, 0x01, 0x02};
const uint8_t agcctrl0WaitValues[] = {0x00, 0x10, 0x20};
const uint8_t agcctrl0HystValues[] = {0x40, 0x80, 0xC0};
const uint8_t agcctrl1PriorityValues[] = {0x00, 0x40};
#  elif defined(CC1101_OOK_TUNING_REFINEMENT)
// Fine frequency grid around 345.10 MHz. CC1101 receive bandwidths are
// discrete; these are the valid hardware points surrounding 203.125 kHz.
const float frequencyValues[] = {345.00f, 345.02f, 345.04f, 345.06f,
                                 345.08f, 345.10f, 345.12f, 345.14f,
                                 345.16f, 345.18f, 345.20f};
const float bandwidthValues[] = {162.5f, 203.125f, 232.143f, 270.833f};
// Hold MAX_DVGA_GAIN at the winning value and refine MAGN_TARGET.
const uint8_t agcctrl2Values[] = {0x80, 0x81, 0x82, 0x83,
                                  0x84, 0x85, 0x86, 0x87};
// Hold hysteresis, freeze, and filter length at the winner while testing each
// AGC_WAIT_TIME value.
const uint8_t agcctrl0Values[] = {0x80, 0x90, 0xA0, 0xB0};
#  else
const float frequencyValues[] = {344.50f, 344.60f, 344.70f, 344.80f,
                                 344.90f, 345.00f, 345.10f, 345.20f,
                                 345.30f, 345.40f, 345.50f};
const float bandwidthValues[] = {812.0f, 650.0f, 406.0f, 325.0f, 270.0f,
                                 203.0f, 162.0f, 116.0f, 81.0f, 58.0f};
const uint8_t agcctrl2Values[] = {0x03, 0x07, 0x43, 0x83, 0xC7};
const uint8_t agcctrl0Values[] = {0x90, 0x91, 0x92, 0x93};
#  endif

CC1101TuningPhase tuningPhase =
#  if defined(CC1101_OOK_TUNING_EXTENDED)
    TUNING_BANDWIDTH;
#  else
    TUNING_FREQUENCY;
#  endif
#  if defined(CC1101_OOK_PROFILE_COMPARE)
CC1101TuningSetting selectedSetting = profileValues[0];
#  elif defined(CC1101_OOK_TUNING_EXTENDED)
CC1101TuningSetting selectedSetting = {345.10f, 270.833f, 0x83, 0x40, 0x90};
#  elif defined(CC1101_OOK_TUNING_REFINEMENT)
CC1101TuningSetting selectedSetting = {345.10f, 203.125f, 0x83, 0x40, 0x90};
#  else
CC1101TuningSetting selectedSetting = {
    RF_MODULE_FREQUENCY, CC1101_TUNING_BASE_BANDWIDTH,
    CC1101_TUNING_BASE_AGCCTRL2, 0x40, CC1101_TUNING_BASE_AGCCTRL0};
#  endif
size_t tuningSettingIndex = 0;
unsigned long tuningWindowStartedMs = 0;
bool tuningAborted = false;
int bestDecoded = -1;
unsigned int bestDecoderSignals = 0;
unsigned int bestZeroDecoded = 0xFFFF;
int bestRssi = -1000;
CC1101TuningSetting bestSetting;
CC1101TuningSetting tuningSetting(size_t index);
#endif

void rtl_433_Callback(char* message) {
  JsonDocument jsonDocument;
  deserializeJson(jsonDocument, message);
  logJson(jsonDocument);
  count++;
}

void rtl_433_RawCallback(const int* pulse_us, const int* gap_us,
                         unsigned int num_pulses, unsigned long duration_us,
                         int rssi) {
#if defined(CC1101_OOK_TUNING)
  (void)pulse_us;
  (void)gap_us;
  (void)duration_us;
  rawCount++;
  rawPulseTotal += num_pulses;
  rawRssiTotal += rssi;
  if (rssi < rawRssiMin) rawRssiMin = rssi;
  if (rssi > rawRssiMax) rawRssiMax = rssi;
#else
  Log.notice(F("Raw signal: time_ms=%u duration=%uus pulses=%u rssi=%d" CR),
             millis(), duration_us, num_pulses, rssi);
  unsigned int sampleCount = num_pulses;
  if (sampleCount > RAW_CALLBACK_SAMPLE_PULSES) {
    sampleCount = RAW_CALLBACK_SAMPLE_PULSES;
  }
  for (unsigned int i = 0; i < sampleCount; i++) {
    Log.notice(F("  pulse[%u]=+%dus -%dus" CR), i, pulse_us[i], gap_us[i]);
  }
#endif
}

void logJson(JsonDocument jsondata) {
#if defined(ESP8266) || defined(ESP32) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
  char JSONmessageBuffer[measureJson(jsondata) + 1];
  serializeJson(jsondata, JSONmessageBuffer, measureJson(jsondata) + 1);
#else
  char JSONmessageBuffer[JSON_MSG_BUFFER];
  serializeJson(jsondata, JSONmessageBuffer, JSON_MSG_BUFFER);
#endif
#if defined(CC1101_OOK_TUNING)
  Log.setShowLevel(false);
  Log.notice(F("."));
  Log.setShowLevel(true);
#else
  Log.notice(F("Received message : %s" CR), JSONmessageBuffer);
#endif
}

#if defined(CC1101_OOK_TUNING)
const char* tuningPhaseName() {
#  if defined(CC1101_OOK_PROFILE_COMPARE)
  return "profile_compare";
#  else
  switch (tuningPhase) {
    case TUNING_FREQUENCY: return "frequency";
    case TUNING_BANDWIDTH: return "bandwidth";
    case TUNING_AGCCTRL2: return "agcctrl2";
    case TUNING_AGCCTRL0: return "agcctrl0";
    case TUNING_AGCCTRL2_DVGA: return "agcctrl2_dvga";
    case TUNING_AGCCTRL2_LNA: return "agcctrl2_lna";
    case TUNING_AGCCTRL0_FILTER: return "agcctrl0_filter";
    case TUNING_AGCCTRL0_WAIT: return "agcctrl0_wait";
    case TUNING_AGCCTRL0_HYST: return "agcctrl0_hyst";
    case TUNING_AGCCTRL1_PRIORITY: return "agcctrl1_priority";
    default: return "final_validation";
  }
#  endif
}

size_t tuningSettingCount() {
#  if defined(CC1101_OOK_PROFILE_COMPARE)
  return sizeof(profileValues) / sizeof(profileValues[0]);
#  else
  switch (tuningPhase) {
    case TUNING_FREQUENCY: return sizeof(frequencyValues) / sizeof(frequencyValues[0]);
    case TUNING_BANDWIDTH: return sizeof(bandwidthValues) / sizeof(bandwidthValues[0]);
    case TUNING_AGCCTRL2: return sizeof(agcctrl2Values) / sizeof(agcctrl2Values[0]);
    case TUNING_AGCCTRL0: return sizeof(agcctrl0Values) / sizeof(agcctrl0Values[0]);
#  if defined(CC1101_OOK_TUNING_EXTENDED)
    case TUNING_AGCCTRL2_DVGA: return sizeof(agcctrl2DvgaValues) / sizeof(agcctrl2DvgaValues[0]);
    case TUNING_AGCCTRL2_LNA: return sizeof(agcctrl2LnaValues) / sizeof(agcctrl2LnaValues[0]);
    case TUNING_AGCCTRL0_FILTER: return sizeof(agcctrl0FilterValues) / sizeof(agcctrl0FilterValues[0]);
    case TUNING_AGCCTRL0_WAIT: return sizeof(agcctrl0WaitValues) / sizeof(agcctrl0WaitValues[0]);
    case TUNING_AGCCTRL0_HYST: return sizeof(agcctrl0HystValues) / sizeof(agcctrl0HystValues[0]);
    case TUNING_AGCCTRL1_PRIORITY: return sizeof(agcctrl1PriorityValues) / sizeof(agcctrl1PriorityValues[0]);
#  endif
    default: return 1;
  }
#  endif
}

CC1101TuningSetting tuningSetting(size_t index) {
#  if defined(CC1101_OOK_PROFILE_COMPARE)
  return profileValues[index];
#  else
  CC1101TuningSetting setting = selectedSetting;
  switch (tuningPhase) {
    case TUNING_FREQUENCY: setting.frequencyMHz = frequencyValues[index]; break;
    case TUNING_BANDWIDTH: setting.bandwidthKHz = bandwidthValues[index]; break;
    case TUNING_AGCCTRL2:
#  if defined(CC1101_OOK_TUNING_EXTENDED)
      setting.agcctrl2 = (setting.agcctrl2 & 0xF8) | agcctrl2Values[index];
#  else
      setting.agcctrl2 = agcctrl2Values[index];
#  endif
      break;
    case TUNING_AGCCTRL0: setting.agcctrl0 = agcctrl0Values[index]; break;
#  if defined(CC1101_OOK_TUNING_EXTENDED)
    case TUNING_AGCCTRL2_DVGA:
      setting.agcctrl2 = (setting.agcctrl2 & 0x3F) | agcctrl2DvgaValues[index]; break;
    case TUNING_AGCCTRL2_LNA:
      setting.agcctrl2 = (setting.agcctrl2 & 0xC7) | agcctrl2LnaValues[index]; break;
    case TUNING_AGCCTRL0_FILTER:
      setting.agcctrl0 = (setting.agcctrl0 & 0xFC) | agcctrl0FilterValues[index]; break;
    case TUNING_AGCCTRL0_WAIT:
      setting.agcctrl0 = (setting.agcctrl0 & 0xCF) | agcctrl0WaitValues[index]; break;
    case TUNING_AGCCTRL0_HYST:
      setting.agcctrl0 = (setting.agcctrl0 & 0x3F) | agcctrl0HystValues[index]; break;
    case TUNING_AGCCTRL1_PRIORITY:
      setting.agcctrl1 = (setting.agcctrl1 & 0xBF) | agcctrl1PriorityValues[index]; break;
#  endif
    default: break;
  }
  return setting;
#  endif
}

const char* tuningSettingName(size_t index) {
#  if defined(CC1101_OOK_PROFILE_COMPARE)
  return profileNames[index];
#  else
  (void)index;
  return "candidate";
#  endif
}

void resetTuningStatistics() {
  count = 0;
  rawCount = 0;
  rawPulseTotal = 0;
  rawRssiTotal = 0;
  rawRssiMin = 127;
  rawRssiMax = -256;
  rtl_433_ESP::decoderSignals = 0;
  rtl_433_ESP::decodedMessages = 0;
  rtl_433_ESP::zeroDecodedSignals = 0;
}

void applyTuningSetting(size_t index) {
  CC1101TuningSetting setting = tuningSetting(index);
  rf.disableReceiver();

  int16_t frequencyState = rf.setFrequency(setting.frequencyMHz);
  int16_t bandwidthState = rf.setRxBandwidth(setting.bandwidthKHz);
  int16_t agc2State = rf.setCC1101Register(
      RADIOLIB_CC1101_REG_AGCCTRL2, setting.agcctrl2);
  int16_t agc1State = rf.setCC1101Register(
      RADIOLIB_CC1101_REG_AGCCTRL1, setting.agcctrl1);
  int16_t agc0State = rf.setCC1101Register(
      RADIOLIB_CC1101_REG_AGCCTRL0, setting.agcctrl0);
  int16_t receiveState = rf.receiveDirect();
  rf.enableReceiver();

  resetTuningStatistics();
  tuningWindowStartedMs = millis();

  uint8_t mdmcfg4 = rf.getCC1101Register(RADIOLIB_CC1101_REG_MDMCFG4);
  uint8_t agcctrl2 = rf.getCC1101Register(RADIOLIB_CC1101_REG_AGCCTRL2);
  uint8_t agcctrl1 = rf.getCC1101Register(RADIOLIB_CC1101_REG_AGCCTRL1);
  uint8_t agcctrl0 = rf.getCC1101Register(RADIOLIB_CC1101_REG_AGCCTRL0);
  Log.notice(
      F(CR "TUNING_START phase=%s profile=%s setting=%u/%u frequency_mhz=%F bandwidth_khz=%F "
           "agcctrl2=0x%x agcctrl1=0x%x agcctrl0=0x%x mdmcfg4=0x%x "
           "window_s=%u expected=%u states=%d,%d,%d,%d,%d,%d" CR),
      tuningPhaseName(), tuningSettingName(index), (unsigned int)(index + 1),
      (unsigned int)tuningSettingCount(), (double)setting.frequencyMHz,
      (double)setting.bandwidthKHz, agcctrl2, agcctrl1, agcctrl0, mdmcfg4,
      (unsigned int)CC1101_TUNING_WINDOW_SECONDS,
      (unsigned int)(CC1101_TUNING_WINDOW_SECONDS /
                     CC1101_TUNING_SIGNAL_INTERVAL_SECONDS),
      frequencyState, bandwidthState, agc2State, agc1State, agc0State,
      receiveState);
}

unsigned int finishTuningSetting() {
  CC1101TuningSetting setting = tuningSetting(tuningSettingIndex);
  unsigned long elapsedMs = millis() - tuningWindowStartedMs;
  unsigned int capturedRawCount = rawCount;
  int capturedDecodedCount = count;
  unsigned long capturedPulseTotal = rawPulseTotal;
  long capturedRssiTotal = rawRssiTotal;
  int capturedRssiMin = rawRssiMin;
  int capturedRssiMax = rawRssiMax;
  unsigned int capturedDecoderSignals = rtl_433_ESP::decoderSignals;
  unsigned int capturedDecodedMessages = rtl_433_ESP::decodedMessages;
  unsigned int capturedZeroDecoded = rtl_433_ESP::zeroDecodedSignals;

  long meanRssi = capturedRawCount ? capturedRssiTotal / (long)capturedRawCount : 0;
  unsigned long meanPulses =
      capturedRawCount ? capturedPulseTotal / capturedRawCount : 0;
  Log.notice(
      F("TUNING_RESULT phase=%s profile=%s setting=%u/%u frequency_mhz=%F bandwidth_khz=%F "
        "agcctrl2=0x%x agcctrl1=0x%x agcctrl0=0x%x elapsed_s=%u expected=%u raw=%u "
        "decoder_signals=%u decoded_messages=%u decoded_zero=%u callback_messages=%d "
        "rssi_mean=%d rssi_min=%d rssi_max=%d pulses_mean=%u" CR),
      tuningPhaseName(), tuningSettingName(tuningSettingIndex),
      (unsigned int)(tuningSettingIndex + 1),
      (unsigned int)tuningSettingCount(), (double)setting.frequencyMHz,
      (double)setting.bandwidthKHz, setting.agcctrl2, setting.agcctrl1,
      setting.agcctrl0,
      (unsigned int)(elapsedMs / 1000UL),
      (unsigned int)(CC1101_TUNING_WINDOW_SECONDS /
                     CC1101_TUNING_SIGNAL_INTERVAL_SECONDS),
      capturedRawCount, capturedDecoderSignals, capturedDecodedMessages,
      capturedZeroDecoded, capturedDecodedCount, (int)meanRssi,
      capturedRawCount ? capturedRssiMin : 0,
      capturedRawCount ? capturedRssiMax : 0, (unsigned int)meanPulses);

#  if defined(CC1101_OOK_PROFILE_COMPARE)
  profileWindows[tuningSettingIndex]++;
  profileSignals[tuningSettingIndex] += capturedDecoderSignals;
  profileDecoded[tuningSettingIndex] += capturedDecodedMessages;
  profileZero[tuningSettingIndex] += capturedZeroDecoded;
  Log.notice(
      F("PROFILE_TOTAL profile=%s windows=%u decoder_signals=%u "
        "decoded_messages=%u decoded_zero=%u" CR),
      tuningSettingName(tuningSettingIndex),
      (unsigned int)profileWindows[tuningSettingIndex],
      (unsigned int)profileSignals[tuningSettingIndex],
      (unsigned int)profileDecoded[tuningSettingIndex],
      (unsigned int)profileZero[tuningSettingIndex]);
#  endif

  bool better = (int)capturedDecodedMessages > bestDecoded;
  if ((int)capturedDecodedMessages == bestDecoded) {
    if (capturedDecodedMessages == 0) {
      // With no successful decodes, prefer evidence of an RF signal over a
      // silent setting whose zero failure count is otherwise misleading.
      better = capturedDecoderSignals > bestDecoderSignals ||
               (capturedDecoderSignals == bestDecoderSignals &&
                meanRssi > bestRssi);
    } else {
      better = capturedZeroDecoded < bestZeroDecoded ||
               (capturedZeroDecoded == bestZeroDecoded &&
                meanRssi > bestRssi);
    }
  }
  if (better) {
    bestDecoded = capturedDecodedMessages;
    bestDecoderSignals = capturedDecoderSignals;
    bestZeroDecoded = capturedZeroDecoded;
    bestRssi = meanRssi;
    bestSetting = setting;
  }
  return capturedDecodedMessages;
}

void abortTuningSuite() {
  CC1101TuningSetting setting = tuningSetting(tuningSettingIndex);
  tuningAborted = true;
  Log.notice(
      F("TUNING_ABORTED reason=phase_zero_decodes phase=%s settings_tested=%u "
        "frequency_mhz=%F bandwidth_khz=%F agcctrl2=0x%x agcctrl1=0x%x agcctrl0=0x%x; "
        "receiver remains active on final phase setting for diagnosis" CR),
      tuningPhaseName(), (unsigned int)tuningSettingCount(),
      (double)setting.frequencyMHz,
      (double)setting.bandwidthKHz, setting.agcctrl2, setting.agcctrl1,
      setting.agcctrl0);
}

void completeTuningPhase() {
  selectedSetting = bestSetting;
  Log.notice(
      F("TUNING_WINNER phase=%s frequency_mhz=%F bandwidth_khz=%F "
        "agcctrl2=0x%x agcctrl1=0x%x agcctrl0=0x%x decoder_signals=%u "
        "decoded_messages=%d decoded_zero=%u rssi=%d" CR),
      tuningPhaseName(), (double)selectedSetting.frequencyMHz,
      (double)selectedSetting.bandwidthKHz, selectedSetting.agcctrl2,
      selectedSetting.agcctrl1, selectedSetting.agcctrl0, bestDecoderSignals, bestDecoded,
      bestZeroDecoded, bestRssi);

  if (false) {
    // The following branches select the next phase explicitly.
#  if defined(CC1101_OOK_TUNING_EXTENDED)
  } else if (tuningPhase == TUNING_BANDWIDTH) {
    tuningPhase = TUNING_AGCCTRL2;
  } else if (tuningPhase == TUNING_AGCCTRL2) {
    tuningPhase = TUNING_AGCCTRL2_DVGA;
  } else if (tuningPhase == TUNING_AGCCTRL2_DVGA) {
    tuningPhase = TUNING_AGCCTRL2_LNA;
  } else if (tuningPhase == TUNING_AGCCTRL2_LNA) {
    tuningPhase = TUNING_AGCCTRL0_FILTER;
  } else if (tuningPhase == TUNING_AGCCTRL0_FILTER) {
    tuningPhase = TUNING_AGCCTRL0_WAIT;
  } else if (tuningPhase == TUNING_AGCCTRL0_WAIT) {
    tuningPhase = TUNING_AGCCTRL0_HYST;
  } else if (tuningPhase == TUNING_AGCCTRL0_HYST) {
    tuningPhase = TUNING_AGCCTRL1_PRIORITY;
  } else if (tuningPhase == TUNING_AGCCTRL1_PRIORITY) {
    tuningPhase = TUNING_FREQUENCY;
  } else if (tuningPhase == TUNING_FREQUENCY) {
    tuningPhase = TUNING_FINAL_VALIDATION;
#  else
  } else if (tuningPhase == TUNING_FREQUENCY) {
    tuningPhase = TUNING_BANDWIDTH;
  } else if (tuningPhase == TUNING_BANDWIDTH) {
    tuningPhase = TUNING_AGCCTRL2;
  } else if (tuningPhase == TUNING_AGCCTRL2) {
    tuningPhase = TUNING_AGCCTRL0;
  } else if (tuningPhase == TUNING_AGCCTRL0) {
    tuningPhase = TUNING_FINAL_VALIDATION;
#  endif
  }
  tuningSettingIndex = 0;
  bestDecoded = -1;
  bestDecoderSignals = 0;
  bestZeroDecoded = 0xFFFF;
  bestRssi = -1000;

  if (tuningPhase == TUNING_FINAL_VALIDATION) {
    Log.notice(
        F("TUNING_SUITE_COMPLETE frequency_mhz=%F bandwidth_khz=%F "
          "agcctrl2=0x%x agcctrl1=0x%x agcctrl0=0x%x; starting repeated validation" CR),
        (double)selectedSetting.frequencyMHz,
        (double)selectedSetting.bandwidthKHz, selectedSetting.agcctrl2,
        selectedSetting.agcctrl1, selectedSetting.agcctrl0);
  }
}
#endif

void setup() {
  Serial.begin(921600);
  delay(1000);
#ifndef LOG_LEVEL
  LOG_LEVEL_SILENT
#endif
  Log.begin(LOG_LEVEL, &Serial);
  Log.notice(F(" " CR));
  Log.notice(F("****** setup ******" CR));
  rf.initReceiver(RF_MODULE_RECEIVER_GPIO, RF_MODULE_FREQUENCY);
  Log.notice(F("RF_MODULE_FREQUENCY %F" CR), (double)RF_MODULE_FREQUENCY);
  rf.setCallback(rtl_433_Callback, messageBuffer, JSON_MSG_BUFFER);
#if defined(CC1101_OOK_TUNING)
  rf.setRawPulsesCallback(rtl_433_RawCallback);
#endif
  rf.enableReceiver();
  Log.notice(F("****** setup complete ******" CR));
  rf.getModuleStatus();
#if defined(CC1101_OOK_TUNING)
  Log.notice(F("CC1101 OOK hands-off %s tuning enabled; %u total test settings" CR),
#  if defined(CC1101_OOK_PROFILE_COMPARE)
             "profile comparison",
#  elif defined(CC1101_OOK_TUNING_REFINEMENT)
             "refinement",
#  elif defined(CC1101_OOK_TUNING_EXTENDED)
             "extended characterization",
#  else
             "broad",
#  endif
#  if defined(CC1101_OOK_PROFILE_COMPARE)
             (unsigned int)(sizeof(profileValues) / sizeof(profileValues[0])));
#  elif defined(CC1101_OOK_TUNING_EXTENDED)
             (unsigned int)(sizeof(frequencyValues) / sizeof(frequencyValues[0]) +
                            sizeof(bandwidthValues) / sizeof(bandwidthValues[0]) +
                            sizeof(agcctrl2Values) / sizeof(agcctrl2Values[0]) +
                            sizeof(agcctrl2DvgaValues) / sizeof(agcctrl2DvgaValues[0]) +
                            sizeof(agcctrl2LnaValues) / sizeof(agcctrl2LnaValues[0]) +
                            sizeof(agcctrl0FilterValues) / sizeof(agcctrl0FilterValues[0]) +
                            sizeof(agcctrl0WaitValues) / sizeof(agcctrl0WaitValues[0]) +
                            sizeof(agcctrl0HystValues) / sizeof(agcctrl0HystValues[0]) +
                            sizeof(agcctrl1PriorityValues) / sizeof(agcctrl1PriorityValues[0])));
#  else
             (unsigned int)(sizeof(frequencyValues) / sizeof(frequencyValues[0]) +
                            sizeof(bandwidthValues) / sizeof(bandwidthValues[0]) +
                            sizeof(agcctrl2Values) / sizeof(agcctrl2Values[0]) +
                            sizeof(agcctrl0Values) / sizeof(agcctrl0Values[0])));
#  endif
  applyTuningSetting(tuningSettingIndex);
#endif
}

void loop() {
  rf.loop();
#if defined(CC1101_OOK_TUNING)
  if (!tuningAborted && millis() - tuningWindowStartedMs >=
      CC1101_TUNING_WINDOW_SECONDS * 1000UL) {
    finishTuningSetting();
#  if defined(CC1101_OOK_PROFILE_COMPARE)
    tuningSettingIndex = (tuningSettingIndex + 1) % tuningSettingCount();
    if (tuningSettingIndex == 0) {
      Log.notice(F("PROFILE_CYCLE_COMPLETE; continuing alternating comparison" CR));
    }
    applyTuningSetting(tuningSettingIndex);
#  else
    tuningSettingIndex++;
    if (tuningSettingIndex >= tuningSettingCount()) {
      if (bestDecoded == 0) {
        // Every setting in this phase completed without a decoded message.
        // Leave the receiver on the final setting to aid hardware diagnosis.
        tuningSettingIndex--;
        abortTuningSuite();
        return;
      }
      completeTuningPhase();
    }
    applyTuningSetting(tuningSettingIndex);
#  endif
  }
#endif
}
