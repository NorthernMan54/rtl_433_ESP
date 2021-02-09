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

volatile bool rtl_433_ESP::receiveMode = false;
volatile unsigned long rtl_433_ESP::signalStart = micros();

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

// rtl_433


  // static volatile r_cfg cfg;

// r_cfg rtl_433_ESP::cfg;
r_cfg_t rtl_433_ESP::g_cfg;

/*
static void fire_callback(protocol_t *protocol, rtl_433_ESPCallBack callback);
static void calc_lengths();

static protocols_t *get_protocols() {
  if (pilight_protocols == nullptr) {
    rtl_433_ESP::setErrorOutput(Serial);
    protocol_init();
    calc_lengths();
  }
  return pilight_protocols;
}


static protocols_t *get_used_protocols() {
  if (used_protocols == nullptr) {
    used_protocols = get_protocols();
  }
  return used_protocols;
}

static protocols_t *find_protocol_node(const char *name) {
  protocols_t *pnode = get_protocols();
  while (pnode != nullptr) {
    if (strcmp(name, pnode->listener->id) == 0) {
      return pnode;
    }
    pnode = pnode->next;
  }
  return nullptr;
}

static protocol_t *find_protocol(const char *name) {
  protocols_t *pnode = find_protocol_node(name);
  if (pnode != nullptr) {
    return pnode->listener;
  }
  return nullptr;
}

static int create_pulse_train(uint16_t *pulses, protocol_t *protocol,
                              const String &content) {
  Debug("piLightCreatePulseTrain: ");

  if (!json_validate(content.c_str())) {
    Debug("invalid json: ");
    DebugLn(content);
    return rtl_433_ESP::ERROR_INVALID_JSON;
  }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
  if ((protocol != nullptr) && (protocol->createCode != nullptr) &&
      (protocol->maxrawlen <= MAXPULSESTREAMLENGTH)) {
#pragma GCC diagnostic pop
    Debug("protocol: ");
    Debug(protocol->id);

    protocol->rawlen = 0;
    protocol->raw = pulses;
    JsonNode *message = json_decode(content.c_str());
    int return_value = protocol->createCode(message);
    json_delete(message);
    // delete message created by createCode()
    json_delete(protocol->message);
    protocol->message = nullptr;

    if (return_value == EXIT_SUCCESS) {
      DebugLn(" create Code succeded.");
      return protocol->rawlen;
    } else {
      DebugLn(" create Code failed.");
      return rtl_433_ESP::ERROR_INVALID_PILIGHT_MSG;
    }
  }
  return rtl_433_ESP::ERROR_UNAVAILABLE_PROTOCOL;
}

static void calc_lengths() {
  protocols_t *pnode = get_used_protocols();
  rtl_433_ESP::minrawlen = std::numeric_limits<uint8_t>::max();
  rtl_433_ESP::maxrawlen = std::numeric_limits<uint8_t>::min();
  rtl_433_ESP::mingaplen = std::numeric_limits<uint16_t>::max();
  rtl_433_ESP::maxgaplen = std::numeric_limits<uint16_t>::min();
  rtl_433_ESP::minpulselen = 80;
  rtl_433_ESP::maxpulselen = 16000;
  while (pnode != nullptr) {
    if (pnode->listener->parseCode != nullptr) {
      const protocol_t *protocol = pnode->listener;
      const uint8_t minLen = protocol->minrawlen;
      const uint8_t maxLen = protocol->maxrawlen;
      const uint16_t minGap = protocol->mingaplen;
      const uint16_t maxGap = protocol->maxgaplen;

      if (minLen < rtl_433_ESP::minrawlen) {
        rtl_433_ESP::minrawlen = minLen;
      }

      if (maxLen > rtl_433_ESP::maxrawlen && maxLen <= MAXPULSESTREAMLENGTH) {
        rtl_433_ESP::maxrawlen = maxLen;
      }

      if (minGap < rtl_433_ESP::mingaplen) {
        rtl_433_ESP::mingaplen = minGap;
      }

      if (maxGap > rtl_433_ESP::maxgaplen) {
        rtl_433_ESP::maxgaplen = maxGap;
      }

      if (minGap < rtl_433_ESP::minpulselen) {
        rtl_433_ESP::minpulselen = minGap;
      }

      if (maxGap > rtl_433_ESP::maxpulselen) {
        rtl_433_ESP::maxpulselen = maxGap;
      }
    }
    pnode = pnode->next;
  }

}

*/

void rtl_433_ESP::initReceiver(byte inputPin)
{
  Debug("Pre initReceiver: ");DebugLn(ESP.getFreeHeap());
  r_cfg_t *cfg = &g_cfg;

  rtlSetup(cfg);

  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.SetRx(CC1101_FREQUENCY);
  resetReceiver();

  Debug("Post initReceiver: ");DebugLn(ESP.getFreeHeap());
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
  }
  else
  {
    if (receiveMode)
    {
      DebugLn();

      Debug("Signal length: ");
      Debug(micros() - signalStart);
      Debug(", train: ");
      Debug(_actualPulseTrain);
      Debug(", pulses: ");
      DebugLn(_nrpulses);

      // Log.trace(F("Signal length: %d, train: %d, pulses: %d" CR), micros() - signalStart, _actualPulseTrain, _nrpulses);
      receiveMode = false;
      enableReceiver(-1);
      if (_nrpulses > 30 && (micros() - signalStart > 40000))
      {

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
    Debug("Pre pulse_demod_ppm ");
    DebugLn(ESP.getFreeHeap());
    // Log.notice(F("Pre pulse_demod_ppm %d" CR), ESP.getFreeHeap());

    // r_cfg_t *cfg = &g_cfg;
    r_cfg_t *cfg = &g_cfg;

    int events = run_ook_demods(&cfg->demod->r_devs, rtl_pulses);
    Debug("Post run_ook_demods "); DebugLn(events);
    // prologue.output_fn = &data_acquired_handler;
    // int events = pulse_demod_ppm(rtl_pulses, &prologue);
    free(rtl_pulses);
    Debug("Post pulse_demod_ppm ");
    DebugLn(ESP.getFreeHeap());
    // Log.notice(F("Post pulse_demod_ppm %d %d" CR), events, ESP.getFreeHeap());

    // bitbuffer is in the event buffer

    // Log.notice(F("Pre prologue_callback %d" CR), ESP.getFreeHeap());
    // prologue_callback(&prologue, &bitbuffer);
    // Log.notice(F("Post prologue_callback %d" CR), ESP.getFreeHeap());
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

/*

void rtl_433_ESP::setCallback(rtl_433_ESPCallBack callback) {
  _callback = callback;
}
*/
void rtl_433_ESP::setPulseTrainCallBack(PulseTrainCallBack rawCallback)
{
  _rawCallback = rawCallback;
}
/*

void rtl_433_ESP::sendPulseTrain(const uint16_t *pulses, size_t length,
                               size_t repeats) {
  if (_outputPin >= 0) {
    bool receiverState = _enabledReceiver;
    _enabledReceiver = (_echoEnabled && receiverState);
    for (unsigned int r = 0; r < repeats; r++) {
      for (unsigned int i = 0; i < length; i += 2) {
        digitalWrite((uint8_t)_outputPin, HIGH);
        delayMicroseconds(pulses[i]);
        digitalWrite((uint8_t)_outputPin, LOW);
        if (i + 1 < length) {
          delayMicroseconds(pulses[i + 1]);
        }
      }
    }
    digitalWrite((uint8_t)_outputPin, LOW);
    _enabledReceiver = receiverState;
  }
}

int rtl_433_ESP::send(const String &protocol, const String &json,
                    size_t repeats) {
  if (_outputPin < 0) {
    DebugLn("No output pin set, cannot send");
    return ERROR_NO_OUTPUT_PIN;
  }
  int length = 0;
  uint16_t pulses[MAXPULSESTREAMLENGTH];

  protocol_t *protocol_listener = find_protocol(protocol.c_str());
  length = create_pulse_train(pulses, protocol_listener, json);
  if (length > 0) {
    if (repeats == 0) {
      repeats = protocol_listener->txrpt;
    }
    sendPulseTrain(pulses, (unsigned)length, repeats);
  }
  return length;
}

int rtl_433_ESP::createPulseTrain(uint16_t *pulses, const String &protocol_id,
                                const String &content) {
  protocol_t *protocol = find_protocol(protocol_id.c_str());
  return create_pulse_train(pulses, protocol, content);
}

*/

size_t rtl_433_ESP::parsePulseTrain(uint16_t *pulses, uint8_t length)
{
  size_t matches = 0;
  DebugLn("piLightParsePulseTrain start");
  /*
  protocol_t *protocol = nullptr;
  protocols_t *pnode = get_used_protocols();

  // DebugLn("piLightParsePulseTrain start");
  while ((pnode != nullptr) && (_callback != nullptr)) {
    protocol = pnode->listener;

    if (protocol->parseCode != nullptr && protocol->validate != nullptr) {
      protocol->raw = pulses;
      protocol->rawlen = length;

      if (protocol->validate() == 0) {
        Debug("pulses: ");
        Debug(length);
        Debug(" possible protocol: ");
        DebugLn(protocol->id);

        if (protocol->first > 0) {
          protocol->first = protocol->second;
        }
        protocol->second = micros();
        if (protocol->first == 0) {
          protocol->first = protocol->second;
        }

        // Reset # of repeats after a certain delay
        if ((protocol->second - protocol->first) > 500000) {
          protocol->repeats = 0;
        }

        protocol->message = nullptr;
        protocol->parseCode();
        if (protocol->message != nullptr) {
          matches++;
          protocol->repeats++;

          fire_callback(protocol, _callback);

          json_delete(protocol->message);
          protocol->message = nullptr;
        }
      }
    }
    pnode = pnode->next;
  }
  if (_rawCallback != nullptr) {
    (_rawCallback)(pulses, length);
  }

  // Debug("piLightParsePulseTrain end. matches: ");
  // DebugLn(matches);
  */
  return matches;
}

/*

static void fire_callback(protocol_t *protocol, rtl_433_ESPCallBack callback) {
  PilightRepeatStatus_t status = FIRST;
  char *content = json_encode(protocol->message);
  String deviceId = "";
  double itmp;
  char *stmp;

  if ((protocol->repeats <= 1) || (protocol->old_content == nullptr)) {
    status = FIRST;
    json_free(protocol->old_content);
    protocol->old_content = content;
  } else if (!(protocol->repeats & 0x80)) {
    if (strcmp(content, protocol->old_content) == 0) {
      protocol->repeats |= 0x80;
      status = VALID;
    } else {
      status = INVALID;
    }
    json_free(protocol->old_content);
    protocol->old_content = content;
  } else {
    status = KNOWN;
    json_free(content);
  }
  if (json_find_number(protocol->message, "id", &itmp) == 0) {
    deviceId = String((int)round(itmp));
  } else if (json_find_string(protocol->message, "id", &stmp) == 0) {
    deviceId = String(stmp);
  };
  (callback)(String(protocol->id), String(protocol->old_content), status,
             protocol->repeats & 0x7F, deviceId);
}

String rtl_433_ESP::pulseTrainToString(const uint16_t *codes, size_t length) {
  bool match = false;
  int diff = 0;

  uint8_t nrpulses = 0;  // number of pulse types
  uint16_t plstypes[MAX_PULSE_TYPES] = {};

  String data("");
  data.reserve(6 + length);
  data += "c:";
  for (unsigned int i = 0; i < length; i++) {
    match = false;
    for (uint8_t j = 0; j < MAX_PULSE_TYPES; j++) {
      // We device these numbers by 10 to normalize them a bit
      diff = (plstypes[j] / 50) - (codes[i] / 50);
      if ((diff >= -2) && (diff <= 2)) {
        // Write numbers
        data += (char)('0' + ((char)j));
        match = true;
        break;
      }
    }
    if (!match) {
      plstypes[nrpulses++] = codes[i];
      data += (char)('0' + ((char)(nrpulses - 1)));
      if (nrpulses >= MAX_PULSE_TYPES) {
        DebugLn("too many pulse types");
        return String("");
      }
    }
  }
  data += ";p:";
  for (uint8_t i = 0; i < nrpulses; i++) {
    data += plstypes[i];
    if (i + 1 < nrpulses) {
      data += ',';
    }
  }
  data += '@';
  return data;
}

int rtl_433_ESP::stringToPulseTrain(const String &data, uint16_t *codes,
                                  size_t maxlength) {
  unsigned int length = 0;    // length of pulse train
  unsigned int nrpulses = 0;  // number of pulse types
  uint16_t plstypes[MAX_PULSE_TYPES] = {};

  // validate data string
  int scode = data.indexOf('c') + 2;
  if (scode < 2 || (unsigned)scode > data.length()) {
    DebugLn("'c' not found in data string, or has no data");
    return ERROR_INVALID_PULSETRAIN_MSG_C;
  }
  int spulse = data.indexOf('p') + 2;
  if (spulse < 2 || (unsigned)spulse > data.length()) {
    DebugLn("'p' not found in data string, or has no data");
    return ERROR_INVALID_PULSETRAIN_MSG_P;
  }
  // parsing pulse types
  unsigned int start = (unsigned)spulse;
  int end = data.indexOf(',', start);
  while (end > 0) {
    plstypes[nrpulses++] =
        (uint16_t)data.substring(start, (unsigned)end).toInt();
    start = (unsigned)end + 1;
    end = data.indexOf(',', start);
  }
  end = data.indexOf(';', start);
  if (end < 0) {
    end = data.indexOf('@', start);
  }
  if (end < 0) {
    DebugLn("';' or '@' not found in data string");
    return ERROR_INVALID_PULSETRAIN_MSG_END;
  }
  plstypes[nrpulses++] = (uint16_t)data.substring(start, (unsigned)end).toInt();
  // parsing pulses
  int pulse_index = 0;
  for (unsigned int i = (unsigned)scode; i < data.length(); i++) {
    if ((data[i] == ';') || (data[i] == '@')) break;
    if (i >= maxlength) break;
    pulse_index = data[i] - '0';
    if ((pulse_index < 0) || ((unsigned)pulse_index >= nrpulses)) {
      DebugLn("Pulse type not defined");
      return ERROR_INVALID_PULSETRAIN_MSG_TYPE;
    }
    codes[length++] = plstypes[pulse_index];
  }
  return length;
}

int rtl_433_ESP::stringToRepeats(const String &data) {
  // parsing (optional) repeats
  int srepeat = data.indexOf('r') + 2;
  if (srepeat < 2 || (unsigned)srepeat > data.length()) {
    DebugLn("'r' not found in data string, or has no data");
    return ERROR_INVALID_PULSETRAIN_MSG_R;
  }
  unsigned int start = (unsigned)srepeat;
  int end = data.indexOf(';', start);
  if (end < 0) {
    end = data.indexOf('@', start);
  }
  if (end < 0) {
    DebugLn("';' or '@' not found in data string");
    return ERROR_INVALID_PULSETRAIN_MSG_END;
  }
  return data.substring(start, (unsigned)end).toInt();
}

void rtl_433_ESP::limitProtocols(const String &protos) {
  if (!json_validate(protos.c_str())) {
    DebugLn("Protocol limit argument is not a valid json message!");
    return;
  }
  JsonNode *message = json_decode(protos.c_str());

  if (message->tag != JSON_ARRAY) {
    DebugLn("Protocol limit argument is not a json array!");
    json_delete(message);
    return;
  }

  if (get_used_protocols() != get_protocols()) {
    protocols_t *pnode = get_used_protocols();
    while (pnode != nullptr) {
      protocols_t *tmp = pnode;
      pnode = pnode->next;
      delete tmp;
    }
  }

  used_protocols = nullptr;
  JsonNode *curr = message->children.head;
  unsigned int proto_count = 0;

  while (curr != nullptr) {
    if (curr->tag != JSON_STRING) {
      DebugLn("Element is not a String");
      curr = curr->next;
      continue;
    }

    protocols_t *templ = find_protocol_node(curr->string_);
    if (templ == nullptr) {
      Debug("Protocol not found: ");
      DebugLn(curr->string_);
      curr = curr->next;
      continue;
    }

    protocols_t *new_node = new protocols_t;
    new_node->listener = templ->listener;
    new_node->next = used_protocols;
    used_protocols = new_node;

    Debug("activated protocol ");
    DebugLn(templ->listener->id);
    proto_count++;

    if (curr == message->children.tail) {
      break;
    }
    curr = curr->next;
  }

  json_delete(message);
  calc_lengths();
}

static String protocols_to_array(protocols_t *pnode) {
  protocols_t *tmp = pnode;
  size_t needed_len = 2;  // []
  while (tmp != nullptr) {
    needed_len += strlen(tmp->listener->id) + 3;  // "xx",
    tmp = tmp->next;
  }

  String ret;
  ret.reserve(needed_len);

  ret += '[';

  bool first = true;
  while (pnode != nullptr) {
    if (first) {
      first = false;
    } else {
      ret += ",";
    }
    ret += '"';
    ret += pnode->listener->id;
    ret += '"';
    pnode = pnode->next;
  }
  ret += "]";

  return ret;
}


String rtl_433_ESP::availableProtocols() {
  return protocols_to_array(get_protocols());
}

String rtl_433_ESP::enabledProtocols() {
  return protocols_to_array(get_used_protocols());
}

void rtl_433_ESP::setEchoEnabled(bool enabled) { _echoEnabled = enabled; }

void rtl_433_ESP::setErrorOutput(Print &output) { set_aprintf_output(&output); }

*/
