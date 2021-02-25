/*
 Basic rtl_433_ESP rtl_433_ESP_debug example

 This example mimic the output of the piligh_debug tool.

 https://github.com/puuu/rtl_433_ESP
*/

#include <rtl_433_ESP.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>

#define CC1101_FREQUENCY 433.92
#define JSON_MSG_BUFFER 512
#define ONBOARD_LED 2

char messageBuffer[JSON_MSG_BUFFER];

rtl_433_ESP rf(-1); // use -1 to disable transmitter

void rtl_433_Callback(char *protocol, char *message, unsigned int modulation)
{
  StaticJsonBuffer<JSON_MSG_BUFFER> jsonBuffer2;
  JsonObject &RFrtl_433_ESPdata = jsonBuffer2.parseObject(message);
  RFrtl_433_ESPdata.set("protocol", protocol);
  RFrtl_433_ESPdata.set("modulation", modulation);

  logJson(RFrtl_433_ESPdata);
}

void logJson(JsonObject &jsondata)
{
#if defined(ESP8266) || defined(ESP32) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
  char JSONmessageBuffer[jsondata.measureLength() + 1];
#else
  char JSONmessageBuffer[JSON_MSG_BUFFER];
#endif
  jsondata.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Log.notice(F("Received message : %s" CR), JSONmessageBuffer);
}

void setup()
{
  Serial.begin(921600);
delay(1000);
#ifndef LOG_LEVEL
  LOG_LEVEL_SILENT
#endif
  Log.begin(LOG_LEVEL, &Serial);
  Log.notice(F(" " CR));
  Log.notice(F("****** setup ******" CR));
  rf.initReceiver(RF_RECEIVER_GPIO, CC1101_FREQUENCY);
  rf.setCallback(rtl_433_Callback, messageBuffer, JSON_MSG_BUFFER);
  Log.notice(F("****** setup complete ******" CR));
}

void loop()
{
  rf.loop();
}