/*
 Basic rtl_433_ESP example for OOK/ASK Devices

*/

#include <rtl_433_ESP.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>

#ifndef RF_MODULE_FREQUENCY
#define RF_MODULE_FREQUENCY 914.938
#endif

#define JSON_MSG_BUFFER 512

char messageBuffer[JSON_MSG_BUFFER];

rtl_433_ESP rf(-1); // use -1 to disable transmitter

void rtl_433_Callback(char* message) {
  DynamicJsonBuffer jsonBuffer2(JSON_MSG_BUFFER);
  JsonObject& RFrtl_433_ESPdata = jsonBuffer2.parseObject(message);
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
  rf.initReceiver(RF_MODULE_RECEIVER_GPIO, RF_MODULE_FREQUENCY);
  rf.setCallback(rtl_433_Callback, messageBuffer, JSON_MSG_BUFFER);
//  ELECHOUSE_cc1101.SetRx(CC1101_FREQUENCY); // set Receive on
  rf.enableReceiver(RF_MODULE_RECEIVER_GPIO);
  Log.notice(F("****** setup complete ******" CR));
}

void loop()
{
  rf.loop();
}