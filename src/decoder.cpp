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

#include "decoder.h"

int rtlVerbose = 0;

r_cfg_t g_cfg; // Global config object

static TaskHandle_t rtl_433_DecoderHandle;
QueueHandle_t rtl_433_Queue;

void rtlSetup()
{
  unsigned i;

  r_cfg_t *cfg = &g_cfg;

#ifdef MEMORY_DEBUG
  logprintfLn(LOG_DEBUG, "sizeof(*cfg->demod) %d", sizeof(*cfg->demod));
#endif
  r_init_cfg(cfg);
#ifdef MEMORY_DEBUG
  logprintfLn(LOG_DEBUG, "sizeof(cfg) %d, heap %d", sizeof(cfg), ESP.getFreeHeap());
#endif
  cfg->conversion_mode = CONVERT_SI; // Default all output to Celcius
  cfg->num_r_devices = NUMOFDEVICES;
  cfg->devices = (r_device *)calloc(cfg->num_r_devices, sizeof(r_device));
  if (!cfg->devices)
    FATAL_CALLOC("cfg->devices");

#ifdef MEMORY_DEBUG
  logprintfLn(LOG_DEBUG, "sizeof(cfg) %d, heap %d", sizeof(cfg), ESP.getFreeHeap());
#endif

#ifndef MY_DEVICES
  // This is a generated fragment from tools/update_rtl_433_devices.sh

  memcpy(&cfg->devices[0], &acurite_rain_896, sizeof(r_device));
  memcpy(&cfg->devices[1], &acurite_th, sizeof(r_device));
  memcpy(&cfg->devices[2], &acurite_txr, sizeof(r_device));
  memcpy(&cfg->devices[3], &acurite_986, sizeof(r_device));
  memcpy(&cfg->devices[4], &acurite_606, sizeof(r_device));
  memcpy(&cfg->devices[5], &acurite_00275rm, sizeof(r_device));
  memcpy(&cfg->devices[6], &acurite_590tx, sizeof(r_device));
  memcpy(&cfg->devices[7], &acurite_01185m, sizeof(r_device));
  memcpy(&cfg->devices[8], &akhan_100F14, sizeof(r_device));
  memcpy(&cfg->devices[9], &alectov1, sizeof(r_device));
  memcpy(&cfg->devices[10], &ambientweather_tx8300, sizeof(r_device));
  memcpy(&cfg->devices[11], &auriol_4ld5661, sizeof(r_device));
  memcpy(&cfg->devices[12], &auriol_aft77b2, sizeof(r_device));
  memcpy(&cfg->devices[13], &auriol_afw2a1, sizeof(r_device));
  memcpy(&cfg->devices[14], &auriol_ahfl, sizeof(r_device));
  memcpy(&cfg->devices[15], &auriol_hg02832, sizeof(r_device));
  memcpy(&cfg->devices[16], &blueline, sizeof(r_device));
  memcpy(&cfg->devices[17], &blyss, sizeof(r_device));
  memcpy(&cfg->devices[18], &brennenstuhl_rcs_2044, sizeof(r_device));
  memcpy(&cfg->devices[19], &bresser_3ch, sizeof(r_device));
  memcpy(&cfg->devices[20], &bt_rain, sizeof(r_device));
  memcpy(&cfg->devices[21], &burnhardbbq, sizeof(r_device));
  memcpy(&cfg->devices[22], &calibeur_RF104, sizeof(r_device));
  memcpy(&cfg->devices[23], &cardin, sizeof(r_device));
  memcpy(&cfg->devices[24], &chuango, sizeof(r_device));
  memcpy(&cfg->devices[25], &companion_wtr001, sizeof(r_device));
  memcpy(&cfg->devices[26], &digitech_xc0324, sizeof(r_device));
  memcpy(&cfg->devices[27], &dish_remote_6_3, sizeof(r_device));
  memcpy(&cfg->devices[28], &ecowitt, sizeof(r_device));
  memcpy(&cfg->devices[29], &eurochron_efth800, sizeof(r_device));
  memcpy(&cfg->devices[30], &elro_db286a, sizeof(r_device));
  memcpy(&cfg->devices[31], &elv_em1000, sizeof(r_device));
  memcpy(&cfg->devices[32], &elv_ws2000, sizeof(r_device));
  memcpy(&cfg->devices[33], &esperanza_ews, sizeof(r_device));
  memcpy(&cfg->devices[34], &eurochron, sizeof(r_device));
  memcpy(&cfg->devices[35], &fineoffset_WH2, sizeof(r_device));
  memcpy(&cfg->devices[36], &fineoffset_WH0530, sizeof(r_device));
  memcpy(&cfg->devices[37], &fineoffset_wh1050, sizeof(r_device));
  memcpy(&cfg->devices[38], &fineoffset_wh1080, sizeof(r_device));
  memcpy(&cfg->devices[39], &fs20, sizeof(r_device));
  memcpy(&cfg->devices[40], &ft004b, sizeof(r_device));
  memcpy(&cfg->devices[41], &generic_motion, sizeof(r_device));
  memcpy(&cfg->devices[42], &generic_remote, sizeof(r_device));
  memcpy(&cfg->devices[43], &generic_temperature_sensor, sizeof(r_device));
  memcpy(&cfg->devices[44], &govee, sizeof(r_device));
  memcpy(&cfg->devices[45], &gt_tmbbq05, sizeof(r_device));
  memcpy(&cfg->devices[46], &gt_wt_02, sizeof(r_device));
  memcpy(&cfg->devices[47], &gt_wt_03, sizeof(r_device));
  memcpy(&cfg->devices[48], &hcs200, sizeof(r_device));
  memcpy(&cfg->devices[49], &honeywell_wdb, sizeof(r_device));
  memcpy(&cfg->devices[50], &ht680, sizeof(r_device));
  memcpy(&cfg->devices[51], &infactory, sizeof(r_device));
  memcpy(&cfg->devices[52], &kw9015b, sizeof(r_device));
  memcpy(&cfg->devices[53], &interlogix, sizeof(r_device));
  memcpy(&cfg->devices[54], &intertechno, sizeof(r_device));
  memcpy(&cfg->devices[55], &kedsum, sizeof(r_device));
  memcpy(&cfg->devices[56], &kerui, sizeof(r_device));
  memcpy(&cfg->devices[57], &lacrossetx, sizeof(r_device));
  memcpy(&cfg->devices[58], &lacrosse_tx141x, sizeof(r_device));
  memcpy(&cfg->devices[59], &lacrosse_ws7000, sizeof(r_device));
  memcpy(&cfg->devices[60], &lacrossews, sizeof(r_device));
  memcpy(&cfg->devices[61], &lightwave_rf, sizeof(r_device));
  memcpy(&cfg->devices[62], &markisol, sizeof(r_device));
  memcpy(&cfg->devices[63], &maverick_et73, sizeof(r_device));
  memcpy(&cfg->devices[64], &mebus433, sizeof(r_device));
  memcpy(&cfg->devices[65], &missil_ml0757, sizeof(r_device));
  memcpy(&cfg->devices[66], &new_template, sizeof(r_device));
  memcpy(&cfg->devices[67], &nexus, sizeof(r_device));
  memcpy(&cfg->devices[68], &nice_flor_s, sizeof(r_device));
  memcpy(&cfg->devices[69], &opus_xt300, sizeof(r_device));
  memcpy(&cfg->devices[70], &oregon_scientific_sl109h, sizeof(r_device));
  memcpy(&cfg->devices[71], &oregon_scientific_v1, sizeof(r_device));
  memcpy(&cfg->devices[72], &philips_aj3650, sizeof(r_device));
  memcpy(&cfg->devices[73], &philips_aj7010, sizeof(r_device));
  memcpy(&cfg->devices[74], &prologue, sizeof(r_device));
  memcpy(&cfg->devices[75], &quhwa, sizeof(r_device));
  memcpy(&cfg->devices[76], &rftech, sizeof(r_device));
  memcpy(&cfg->devices[77], &rubicson, sizeof(r_device));
  memcpy(&cfg->devices[78], &rubicson_48659, sizeof(r_device));
  memcpy(&cfg->devices[79], &s3318p, sizeof(r_device));
  memcpy(&cfg->devices[80], &silvercrest, sizeof(r_device));
  memcpy(&cfg->devices[81], &skylink_motion, sizeof(r_device));
  memcpy(&cfg->devices[82], &smoke_gs558, sizeof(r_device));
  memcpy(&cfg->devices[83], &solight_te44, sizeof(r_device));
  memcpy(&cfg->devices[84], &springfield, sizeof(r_device));
  memcpy(&cfg->devices[85], &tfa_30_3221, sizeof(r_device));
  memcpy(&cfg->devices[86], &tfa_drop_303233, sizeof(r_device));
  memcpy(&cfg->devices[87], &tfa_pool_thermometer, sizeof(r_device));
  memcpy(&cfg->devices[88], &tfa_twin_plus_303049, sizeof(r_device));
  memcpy(&cfg->devices[89], &thermopro_tp11, sizeof(r_device));
  memcpy(&cfg->devices[90], &thermopro_tp12, sizeof(r_device));
  memcpy(&cfg->devices[91], &thermopro_tx2, sizeof(r_device));
  memcpy(&cfg->devices[92], &ts_ft002, sizeof(r_device));
  memcpy(&cfg->devices[93], &visonic_powercode, sizeof(r_device));
  memcpy(&cfg->devices[94], &waveman, sizeof(r_device));
  memcpy(&cfg->devices[95], &wg_pb12v1, sizeof(r_device));
  memcpy(&cfg->devices[96], &ws2032, sizeof(r_device));
  memcpy(&cfg->devices[97], &wssensor, sizeof(r_device));
  memcpy(&cfg->devices[98], &wt1024, sizeof(r_device));
  memcpy(&cfg->devices[99], &X10_RF, sizeof(r_device));
  memcpy(&cfg->devices[100], &x10_sec, sizeof(r_device));

  // end of fragement

#else
  memcpy(&cfg->devices[0], &skylink_motion, sizeof(r_device));
  memcpy(&cfg->devices[1], &prologue, sizeof(r_device));
  memcpy(&cfg->devices[2], &acurite_986, sizeof(r_device));
  memcpy(&cfg->devices[3], &philips_aj3650, sizeof(r_device));
  memcpy(&cfg->devices[4], &lacrosse_tx141x, sizeof(r_device));
#endif

#ifdef RTL_FLEX
  // This option is non-functional. The flex decoder is too resource intensive for an ESP32, and needs the ESP32 stack set to 32768 in order for the flex_callback to execute
  // Tested with -DRTL_FLEX="n=Sonoff-PIR3-RF,m=OOK_PWM,s=300,l=860,r=7492,g=868,t=50,y=0,bits>=24,repeats>=5,invert,get=@0:{20}:id,get=@20:{4}:motion:[0:true ],unique"

  r_device *flex_device;
  flex_device = flex_create_device(RTL_FLEX);
  memcpy(&cfg->devices[101], &flex_device, sizeof(r_device));
  register_protocol(cfg, flex_device, NULL);
  alogprintfLn(LOG_INFO, "Flex Decoder enabled: %s", RTL_FLEX);
#endif

// logprintfLn(LOG_INFO, "Location of r_devices: %p", (void *)&r_devices);
// logprintfLn(LOG_INFO, "Location of cfg: %p", (void *)&cfg);
// logprintfLn(LOG_INFO, "cfg size %d", sizeof(r_cfg_t));
// logprintfLn(LOG_INFO, "Location of cfg->devices: %p", (void *)&cfg->devices);
#ifdef MEMORY_DEBUG
  logprintfLn(LOG_INFO, "size of bitbuffer: %d", sizeof(bitbuffer_t));
  logprintfLn(LOG_INFO, "size of pulse_data: %d", sizeof(pulse_data_t));
#endif

  int numberEnabled = 0;

  for (i = 0; i < cfg->num_r_devices; i++)
  {
    cfg->devices[i].protocol_num = i + 1;
    // These pulse demods have been tested (85), ymmv for the others
    if (cfg->devices[i].modulation == OOK_PULSE_PPM || cfg->devices[i].modulation == OOK_PULSE_PWM)
    {
      numberEnabled++;
    }
    else
    {
      cfg->devices[i].disabled = 1;
    }
  }
#ifdef DEMOD_DEBUG
  logprintfLn(LOG_INFO, "# of device(s) configured %d", cfg->num_r_devices);
  logprintfLn(LOG_INFO, "ssizeof(r_device): %d", sizeof(r_device));
  logprintfLn(LOG_INFO, "cfg->devices size: %d", sizeof(r_device) * cfg->num_r_devices);
  logprintfLn(LOG_INFO, "# of device(s) enabled %d", numberEnabled);
#endif
#ifdef RTL_DEBUG
  cfg->verbosity = RTL_DEBUG; // 0=normal, 1=verbose, 2=verbose decoders, 3=debug decoders, 4=trace decoding.
#else
  cfg->verbosity = rtlVerbose; // 0=normal, 1=verbose, 2=verbose decoders, 3=debug decoders, 4=trace decoding.
#endif
#ifdef RTL_VERBOSE
  cfg->devices[RTL_VERBOSE].verbose = 4;
  logprintfLn(LOG_INFO, "%s Log Level %d", cfg->devices[RTL_VERBOSE].name, cfg->devices[RTL_VERBOSE].verbose);
#endif
#ifdef MEMORY_DEBUG
  logprintfLn(LOG_DEBUG, "Pre register_all_protocols heap %d", ESP.getFreeHeap());
#endif

  // expand register_all_protocols to determine heap impact from each decoder
  // register_all_protocols(cfg, 0); 

  for (int i = 0; i < cfg->num_r_devices; i++)
  {
// register all device protocols that are not disabled
#ifdef MEMORY_DEBUG
    logprintfLn(LOG_DEBUG, "Pre register_protocol %d %s, heap %d", i, cfg->devices[i].name, ESP.getFreeHeap());
#endif
    if (cfg->devices[i].disabled <= 0)
    {
      register_protocol(cfg, &cfg->devices[i], NULL);
    }
  }

#ifdef MEMORY_DEBUG
  logprintfLn(LOG_DEBUG, "Pre xQueueCreate heap %d", ESP.getFreeHeap());
#endif
  rtl_433_Queue = xQueueCreate(5, sizeof(pulse_data_t *));

#ifdef MEMORY_DEBUG
  logprintfLn(LOG_DEBUG, "Pre xTaskCreatePinnedToCore heap %d", ESP.getFreeHeap());
#endif

  xTaskCreatePinnedToCore(
      rtl_433_DecoderTask,    /* Function to implement the task */
      "rtl_433_DecoderTask",  /* Name of the task */
      5120,                   /* Stack size in bytes */
      NULL,                   /* Task input parameter */
      1,                      /* Priority of the task (set lower than core task) */
      &rtl_433_DecoderHandle, /* Task handle. */
      0);                     /* Core where the task should run */
}

void _setCallback(rtl_433_ESPCallBack callback, char *messageBuffer, int bufferSize)
{
  // logprintfLn(LOG_DEBUG, "_setCallback location: %p", callback);

  r_cfg_t *cfg = &g_cfg;
  cfg->callback = callback;
  cfg->messageBuffer = messageBuffer;
  cfg->bufferSize = bufferSize;
}

void _setDebug(int debug)
{
  rtlVerbose = debug;
  logprintfLn(LOG_INFO, "Setting rtl_433 debug to: %d", rtlVerbose);
}

// ---------------------------------------------------------------------------------------------------------

void rtl_433_DecoderTask(void *pvParameters)
{
  pulse_data_t *rtl_pulses = nullptr;
  for (;;)
  {
    // logprintfLn(LOG_DEBUG, "rtl_433_DecoderTask awaiting signal");
    xQueueReceive(rtl_433_Queue, &rtl_pulses, portMAX_DELAY);
    // logprintfLn(LOG_DEBUG, "rtl_433_DecoderTask signal received");
#ifdef MEMORY_DEBUG
    unsigned long signalProcessingStart = micros();
#endif

#ifdef RAW_SIGNAL_DEBUG
    logprintf(LOG_INFO, "RAW (%lu): ", rtl_pulses->signalDuration);
    for (int i = 0; i < rtl_pulses->num_pulses; i++)
    {
      alogprintf(LOG_INFO, "+%d", rtl_pulses->pulse[i]);
      alogprintf(LOG_INFO, "-%d", rtl_pulses->gap[i]);
#ifdef SIGNAL_RSSI
      alogprintf(LOG_INFO, "(%d)", rtl_pulses->rssi[i]);
#endif
    }
    alogprintfLn(LOG_INFO, " ");
#endif
#ifdef MEMORY_DEBUG
    logprintfLn(LOG_INFO, "Pre run_ook_demods: %d", ESP.getFreeHeap());
#endif
    rtl_pulses->sample_rate = 1.0e6;
    r_cfg_t *cfg = &g_cfg;
    cfg->demod->pulse_data = rtl_pulses;
    int events = run_ook_demods(&cfg->demod->r_devs, rtl_pulses);
    if (events == 0)
    {
      rtl_433_ESP::unparsedSignals++;
#ifdef PUBLISH_UNPARSED
      logprintf(LOG_INFO, "Unparsed Signal length: %lu", rtl_pulses->signalDuration);
      alogprintf(LOG_INFO, ", Signal RSSI: %d", rtl_pulses->signalRssi);
      //      alogprintf(LOG_INFO, ", train: %d", _actualPulseTrain);
      //      alogprintf(LOG_INFO, ", messageCount: %d", messageCount);
      alogprintfLn(LOG_INFO, ", pulses: %d", rtl_pulses->num_pulses);

      logprintf(LOG_INFO, "RAW (%lu): ", rtl_pulses->signalDuration);
#ifndef RAW_SIGNAL_DEBUG
      for (int i = 0; i < rtl_pulses->num_pulses; i++)
      {
        alogprintf(LOG_INFO, "+%d", rtl_pulses->pulse[i]);
        alogprintf(LOG_INFO, "-%d", rtl_pulses->gap[i]);
#ifdef SIGNAL_RSSI
        alogprintf(LOG_INFO, "(%d)", rtl_pulses->rssi[i]);
#endif
      }
      alogprintfLn(LOG_INFO, " ");
#endif

      // Send a note saying unparsed signal signal received
      data_t *data;
      /* clang-format off */
  data = data_make(
                "model", "",      DATA_STRING,  "unknown",
                "protocol", "",   DATA_STRING,  "signal parsing failed",
                "duration", "",   DATA_INT,     rtl_pulses->signalDuration,
                "rssi", "", DATA_INT,     rtl_pulses->signalRssi,
                "pulses", "",     DATA_INT,     rtl_pulses->num_pulses,
//                "train", "",      DATA_INT,     _actualPulseTrain,
//                "messageCount", "", DATA_INT,   messageCount,
//                "_enabledReceiver", "", DATA_INT, _enabledReceiver,
//                "receiveMode", "", DATA_INT,    receiveMode,
//                "currentRssi", "", DATA_INT,    currentRssi,
//                "rssiThreshold", "", DATA_INT,    rssiThreshold,
                NULL);
      /* clang-format on */

      r_cfg_t *cfg = &g_cfg;
      data_print_jsons(data, cfg->messageBuffer, cfg->bufferSize);
      (cfg->callback)(cfg->messageBuffer);
      data_free(data);

#endif
    }

#ifdef MEMORY_DEBUG
    logprintfLn(LOG_INFO, "Signal processing time: %lu", micros() - signalProcessingStart);
    logprintfLn(LOG_INFO, "Post run_ook_demods memory %d", ESP.getFreeHeap());
#endif
#ifdef DEMOD_DEBUG
    logprintfLn(LOG_INFO, "# of messages decoded %d", events);
#endif
    if (events > 0)
    {
      alogprintfLn(LOG_INFO, " ");
    }
#if defined(MEMORY_DEBUG) 
    else
    {
      logprintfLn(LOG_DEBUG, "Process rtl_433_DecoderTask stack free: %u", uxTaskGetStackHighWaterMark(rtl_433_DecoderHandle));
      alogprintfLn(LOG_INFO, " ");
    }
#endif
#ifdef MEMORY_DEBUG
    logprintfLn(LOG_INFO, "Pre free rtl_433_DecoderTask: %d", ESP.getFreeHeap());
#endif
    free(rtl_pulses);
#ifdef MEMORY_DEBUG
    logprintfLn(LOG_INFO, "Post free rtl_433_DecoderTask: %d", ESP.getFreeHeap());
#endif
  }
}

void processSignal(pulse_data_t *rtl_pulses)
{
  // logprintfLn(LOG_DEBUG, "processSignal() about to place signal on rtl_433_Queue");
  if (xQueueSend(rtl_433_Queue, &rtl_pulses, 0) != pdTRUE)
  {
    logprintfLn(LOG_ERR, "ERROR: rtl_433_Queue full, discarding signal");
    free(rtl_pulses);
  }
  else
  {
    // logprintfLn(LOG_DEBUG, "processSignal() signal placed on rtl_433_Queue");
  }
}