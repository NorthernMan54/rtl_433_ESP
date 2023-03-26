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

#include "signalDecoder.h"

/*----------------------------- rtl_433_ESP Internals -----------------------------*/

#if defined(RTL_ANALYZER)
#  define rtl_433_Decoder_Stack 50000
#elif defined(RTL_VERBOSE) || defined(RTL_DEBUG)
#  define rtl_433_Decoder_Stack 30000
#else
#  define rtl_433_Decoder_Stack 5000
#endif

#define rtl_433_Decoder_Priority 2
#define rtl_433_Decoder_Core     1

/*----------------------------- rtl_433_ESP Internals -----------------------------*/

int rtlVerbose = 0;

r_cfg_t g_cfg; // Global config object

QueueHandle_t rtl_433_Queue;

void rtlSetup() {
  r_cfg_t* cfg = &g_cfg;

#ifdef MEMORY_DEBUG
  logprintfLn(LOG_DEBUG, "sizeof(*cfg->demod) %d", sizeof(*cfg->demod));
#endif

  if (!cfg->demod) {
    r_init_cfg(cfg);
    add_log_output(cfg, NULL);
#ifdef MEMORY_DEBUG
    logprintfLn(LOG_DEBUG, "sizeof(cfg) %d, heap %d", sizeof(cfg),
                ESP.getFreeHeap());
#endif
    cfg->conversion_mode = CONVERT_SI; // Default all output to Celcius
    if (rtl_433_ESP::ookModulation) {
      cfg->num_r_devices = NUMOF_OOK_DEVICES;
    } else {
      cfg->num_r_devices = NUMOF_FSK_DEVICES;
    }
    cfg->devices = (r_device*)calloc(cfg->num_r_devices, sizeof(r_device));
    if (!cfg->devices)
      FATAL_CALLOC("cfg->devices");

#ifdef MEMORY_DEBUG
    logprintfLn(LOG_DEBUG, "sizeof(cfg) %d, heap %d", sizeof(cfg),
                ESP.getFreeHeap());
#endif

#ifndef MY_DEVICES
    // This is a generated fragment from tools/update_rtl_433_devices.sh

    if (rtl_433_ESP::ookModulation) {
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
      memcpy(&cfg->devices[10], &ambient_weather, sizeof(r_device));
      memcpy(&cfg->devices[11], &ambientweather_tx8300, sizeof(r_device));
      memcpy(&cfg->devices[12], &atech_ws308, sizeof(r_device));
      memcpy(&cfg->devices[13], &auriol_4ld5661, sizeof(r_device));
      memcpy(&cfg->devices[14], &auriol_aft77b2, sizeof(r_device));
      memcpy(&cfg->devices[15], &auriol_afw2a1, sizeof(r_device));
      memcpy(&cfg->devices[16], &auriol_ahfl, sizeof(r_device));
      memcpy(&cfg->devices[17], &auriol_hg02832, sizeof(r_device));
      memcpy(&cfg->devices[18], &baldr_rain, sizeof(r_device));
      memcpy(&cfg->devices[19], &blyss, sizeof(r_device));
      memcpy(&cfg->devices[20], &brennenstuhl_rcs_2044, sizeof(r_device));
      memcpy(&cfg->devices[21], &bresser_3ch, sizeof(r_device));
      memcpy(&cfg->devices[22], &bt_rain, sizeof(r_device));
      memcpy(&cfg->devices[23], &burnhardbbq, sizeof(r_device));
      memcpy(&cfg->devices[24], &calibeur_RF104, sizeof(r_device));
      memcpy(&cfg->devices[25], &cardin, sizeof(r_device));
      memcpy(&cfg->devices[26], &chuango, sizeof(r_device));
      memcpy(&cfg->devices[27], &companion_wtr001, sizeof(r_device));
      memcpy(&cfg->devices[28], &cotech_36_7959, sizeof(r_device));
      memcpy(&cfg->devices[29], &digitech_xc0324, sizeof(r_device));
      memcpy(&cfg->devices[30], &dish_remote_6_3, sizeof(r_device));
      memcpy(&cfg->devices[31], &dsc_security, sizeof(r_device));
      memcpy(&cfg->devices[32], &dsc_security_ws4945, sizeof(r_device));
      memcpy(&cfg->devices[33], &ecowitt, sizeof(r_device));
      memcpy(&cfg->devices[34], &eurochron_efth800, sizeof(r_device));
      memcpy(&cfg->devices[35], &elro_db286a, sizeof(r_device));
      memcpy(&cfg->devices[36], &elv_em1000, sizeof(r_device));
      memcpy(&cfg->devices[37], &elv_ws2000, sizeof(r_device));
      memcpy(&cfg->devices[38], &emos_e6016, sizeof(r_device));
      memcpy(&cfg->devices[39], &emos_e6016_rain, sizeof(r_device));
      memcpy(&cfg->devices[40], &enocean_erp1, sizeof(r_device));
      memcpy(&cfg->devices[41], &ert_idm, sizeof(r_device));
      memcpy(&cfg->devices[42], &ert_netidm, sizeof(r_device));
      memcpy(&cfg->devices[43], &ert_scm, sizeof(r_device));
      memcpy(&cfg->devices[44], &esa_energy, sizeof(r_device));
      memcpy(&cfg->devices[45], &esperanza_ews, sizeof(r_device));
      memcpy(&cfg->devices[46], &eurochron, sizeof(r_device));
      memcpy(&cfg->devices[47], &fineoffset_WH2, sizeof(r_device));
      memcpy(&cfg->devices[48], &fineoffset_WH0530, sizeof(r_device));
      memcpy(&cfg->devices[49], &fineoffset_wh1050, sizeof(r_device));
      memcpy(&cfg->devices[50], &fineoffset_wh1080, sizeof(r_device));
      memcpy(&cfg->devices[51], &fordremote, sizeof(r_device));
      memcpy(&cfg->devices[52], &fs20, sizeof(r_device));
      memcpy(&cfg->devices[53], &ft004b, sizeof(r_device));
      memcpy(&cfg->devices[54], &funkbus_remote, sizeof(r_device));
      memcpy(&cfg->devices[55], &gasmate_ba1008, sizeof(r_device));
      memcpy(&cfg->devices[56], &generic_motion, sizeof(r_device));
      memcpy(&cfg->devices[57], &generic_remote, sizeof(r_device));
      memcpy(&cfg->devices[58], &generic_temperature_sensor, sizeof(r_device));
      memcpy(&cfg->devices[59], &govee, sizeof(r_device));
      memcpy(&cfg->devices[60], &govee_h5054, sizeof(r_device));
      memcpy(&cfg->devices[61], &gt_tmbbq05, sizeof(r_device));
      memcpy(&cfg->devices[62], &gt_wt_02, sizeof(r_device));
      memcpy(&cfg->devices[63], &gt_wt_03, sizeof(r_device));
      memcpy(&cfg->devices[64], &hcs200, sizeof(r_device));
      memcpy(&cfg->devices[65], &hideki_ts04, sizeof(r_device));
      memcpy(&cfg->devices[66], &honeywell, sizeof(r_device));
      memcpy(&cfg->devices[67], &honeywell_wdb, sizeof(r_device));
      memcpy(&cfg->devices[68], &ht680, sizeof(r_device));
      memcpy(&cfg->devices[69], &ibis_beacon, sizeof(r_device));
      memcpy(&cfg->devices[70], &infactory, sizeof(r_device));
      memcpy(&cfg->devices[71], &kw9015b, sizeof(r_device));
      memcpy(&cfg->devices[72], &interlogix, sizeof(r_device));
      memcpy(&cfg->devices[73], &intertechno, sizeof(r_device));
      memcpy(&cfg->devices[74], &kedsum, sizeof(r_device));
      memcpy(&cfg->devices[75], &kerui, sizeof(r_device));
      memcpy(&cfg->devices[76], &klimalogg, sizeof(r_device));
      memcpy(&cfg->devices[77], &lacrossetx, sizeof(r_device));
      memcpy(&cfg->devices[78], &lacrosse_tx141x, sizeof(r_device));
      memcpy(&cfg->devices[79], &lacrosse_ws7000, sizeof(r_device));
      memcpy(&cfg->devices[80], &lacrossews, sizeof(r_device));
      memcpy(&cfg->devices[81], &lightwave_rf, sizeof(r_device));
      memcpy(&cfg->devices[82], &markisol, sizeof(r_device));
      memcpy(&cfg->devices[83], &maverick_et73, sizeof(r_device));
      memcpy(&cfg->devices[84], &mebus433, sizeof(r_device));
      memcpy(&cfg->devices[85], &megacode, sizeof(r_device));
      memcpy(&cfg->devices[86], &missil_ml0757, sizeof(r_device));
      memcpy(&cfg->devices[87], &new_template, sizeof(r_device));
      memcpy(&cfg->devices[88], &nice_flor_s, sizeof(r_device));
      memcpy(&cfg->devices[89], &norgo, sizeof(r_device));
      memcpy(&cfg->devices[90], &opus_xt300, sizeof(r_device));
      memcpy(&cfg->devices[91], &oregon_scientific_sl109h, sizeof(r_device));
      memcpy(&cfg->devices[92], &oregon_scientific_v1, sizeof(r_device));
      memcpy(&cfg->devices[93], &philips_aj3650, sizeof(r_device));
      memcpy(&cfg->devices[94], &philips_aj7010, sizeof(r_device));
      memcpy(&cfg->devices[95], &prologue, sizeof(r_device));
      memcpy(&cfg->devices[96], &quhwa, sizeof(r_device));
      memcpy(&cfg->devices[97], &radiohead_ask, sizeof(r_device));
      memcpy(&cfg->devices[98], &sensible_living, sizeof(r_device));
      memcpy(&cfg->devices[99], &regency_fan, sizeof(r_device));
      memcpy(&cfg->devices[100], &revolt_nc5462, sizeof(r_device));
      memcpy(&cfg->devices[101], &rftech, sizeof(r_device));
      memcpy(&cfg->devices[102], &rubicson, sizeof(r_device));
      memcpy(&cfg->devices[103], &rubicson_48659, sizeof(r_device));
      memcpy(&cfg->devices[104], &rubicson_pool_48942, sizeof(r_device));
      memcpy(&cfg->devices[105], &s3318p, sizeof(r_device));
      memcpy(&cfg->devices[106], &scmplus, sizeof(r_device));
      memcpy(&cfg->devices[107], &secplus_v1, sizeof(r_device));
      memcpy(&cfg->devices[108], &silvercrest, sizeof(r_device));
      memcpy(&cfg->devices[109], &ss_sensor, sizeof(r_device));
      memcpy(&cfg->devices[110], &skylink_motion, sizeof(r_device));
      memcpy(&cfg->devices[111], &smoke_gs558, sizeof(r_device));
      memcpy(&cfg->devices[112], &solight_te44, sizeof(r_device));
      memcpy(&cfg->devices[113], &springfield, sizeof(r_device));
      memcpy(&cfg->devices[114], &telldus_ft0385r, sizeof(r_device));
      memcpy(&cfg->devices[115], &tfa_30_3221, sizeof(r_device));
      memcpy(&cfg->devices[116], &tfa_drop_303233, sizeof(r_device));
      memcpy(&cfg->devices[117], &tfa_pool_thermometer, sizeof(r_device));
      memcpy(&cfg->devices[118], &tfa_twin_plus_303049, sizeof(r_device));
      memcpy(&cfg->devices[119], &thermopro_tp11, sizeof(r_device));
      memcpy(&cfg->devices[120], &thermopro_tp12, sizeof(r_device));
      memcpy(&cfg->devices[121], &thermopro_tx2, sizeof(r_device));
      memcpy(&cfg->devices[122], &tpms_eezrv, sizeof(r_device));
      memcpy(&cfg->devices[123], &tpms_tyreguard400, sizeof(r_device));
      memcpy(&cfg->devices[124], &ts_ft002, sizeof(r_device));
      memcpy(&cfg->devices[125], &ttx201, sizeof(r_device));
      memcpy(&cfg->devices[126], &vaillant_vrt340f, sizeof(r_device));
      memcpy(&cfg->devices[127], &vauno_en8822c, sizeof(r_device));
      memcpy(&cfg->devices[128], &visonic_powercode, sizeof(r_device));
      memcpy(&cfg->devices[129], &waveman, sizeof(r_device));
      memcpy(&cfg->devices[130], &wec2103, sizeof(r_device));
      memcpy(&cfg->devices[131], &wg_pb12v1, sizeof(r_device));
      memcpy(&cfg->devices[132], &ws2032, sizeof(r_device));
      memcpy(&cfg->devices[133], &wssensor, sizeof(r_device));
      memcpy(&cfg->devices[134], &wt1024, sizeof(r_device));
      memcpy(&cfg->devices[135], &wt450, sizeof(r_device));
      memcpy(&cfg->devices[136], &X10_RF, sizeof(r_device));
      memcpy(&cfg->devices[137], &x10_sec, sizeof(r_device));
      memcpy(&cfg->devices[138], &yale_hsa, sizeof(r_device));
    } else {
      memcpy(&cfg->devices[0], &ambientweather_wh31e, sizeof(r_device));
      memcpy(&cfg->devices[1], &ant_antplus, sizeof(r_device));
      memcpy(&cfg->devices[2], &archos_tbh, sizeof(r_device));
      memcpy(&cfg->devices[3], &badger_orion, sizeof(r_device));
      memcpy(&cfg->devices[4], &bresser_5in1, sizeof(r_device));
      memcpy(&cfg->devices[5], &bresser_6in1, sizeof(r_device));
      memcpy(&cfg->devices[6], &bresser_7in1, sizeof(r_device));
      memcpy(&cfg->devices[7], &ced7000, sizeof(r_device));
      memcpy(&cfg->devices[8], &danfoss_CFR, sizeof(r_device));
      memcpy(&cfg->devices[9], &directv, sizeof(r_device));
      memcpy(&cfg->devices[10], &ecodhome, sizeof(r_device));
      memcpy(&cfg->devices[11], &efergy_e2_classic, sizeof(r_device));
      memcpy(&cfg->devices[12], &efergy_optical, sizeof(r_device));
      memcpy(&cfg->devices[13], &emax, sizeof(r_device));
      memcpy(&cfg->devices[14], &emontx, sizeof(r_device));
      memcpy(&cfg->devices[15], &esic_emt7110, sizeof(r_device));
      memcpy(&cfg->devices[16], &fineoffset_WH25, sizeof(r_device));
      memcpy(&cfg->devices[17], &fineoffset_WH51, sizeof(r_device));
      memcpy(&cfg->devices[18], &fineoffset_wh1080_fsk, sizeof(r_device));
      memcpy(&cfg->devices[19], &fineoffset_wh31l, sizeof(r_device));
      memcpy(&cfg->devices[20], &fineoffset_wh45, sizeof(r_device));
      memcpy(&cfg->devices[21], &fineoffset_wn34, sizeof(r_device));
      memcpy(&cfg->devices[22], &fineoffset_ws80, sizeof(r_device));
      memcpy(&cfg->devices[23], &flowis, sizeof(r_device));
      memcpy(&cfg->devices[24], &geo_minim, sizeof(r_device));
      memcpy(&cfg->devices[25], &hcs200_fsk, sizeof(r_device));
      memcpy(&cfg->devices[26], &holman_ws5029pcm, sizeof(r_device));
      memcpy(&cfg->devices[27], &holman_ws5029pwm, sizeof(r_device));
      memcpy(&cfg->devices[28], &hondaremote, sizeof(r_device));
      memcpy(&cfg->devices[29], &honeywell_cm921, sizeof(r_device));
      memcpy(&cfg->devices[30], &honeywell_wdb_fsk, sizeof(r_device));
      memcpy(&cfg->devices[31], &ikea_sparsnas, sizeof(r_device));
      memcpy(&cfg->devices[32], &inkbird_ith20r, sizeof(r_device));
      memcpy(&cfg->devices[33], &lacrosse_breezepro, sizeof(r_device));
      memcpy(&cfg->devices[34], &lacrosse_r1, sizeof(r_device));
      memcpy(&cfg->devices[35], &lacrosse_th3, sizeof(r_device));
      memcpy(&cfg->devices[36], &lacrosse_tx31u, sizeof(r_device));
      memcpy(&cfg->devices[37], &lacrosse_tx34, sizeof(r_device));
      memcpy(&cfg->devices[38], &lacrosse_tx29, sizeof(r_device));
      memcpy(&cfg->devices[39], &lacrosse_tx35, sizeof(r_device));
      memcpy(&cfg->devices[40], &lacrosse_wr1, sizeof(r_device));
      memcpy(&cfg->devices[41], &marlec_solar, sizeof(r_device));
      memcpy(&cfg->devices[42], &maverick_xr30, sizeof(r_device));
      memcpy(&cfg->devices[43], &oil_smart, sizeof(r_device));
      memcpy(&cfg->devices[44], &oil_watchman_advanced, sizeof(r_device));
      memcpy(&cfg->devices[45], &rojaflex, sizeof(r_device));
      memcpy(&cfg->devices[46], &simplisafe_gen3, sizeof(r_device));
      memcpy(&cfg->devices[47], &somfy_iohc, sizeof(r_device));
      memcpy(&cfg->devices[48], &srsmith_pool_srs_2c_tx, sizeof(r_device));
      memcpy(&cfg->devices[49], &steelmate, sizeof(r_device));
      memcpy(&cfg->devices[50], &tfa_14_1504_v2, sizeof(r_device));
      memcpy(&cfg->devices[51], &tfa_303196, sizeof(r_device));
      memcpy(&cfg->devices[52], &tfa_marbella, sizeof(r_device));
      memcpy(&cfg->devices[53], &tpms_ave, sizeof(r_device));
      memcpy(&cfg->devices[54], &tpms_hyundai_vdo, sizeof(r_device));
      memcpy(&cfg->devices[55], &tpms_kia, sizeof(r_device));
      memcpy(&cfg->devices[56], &tpms_porsche, sizeof(r_device));
      memcpy(&cfg->devices[57], &tpms_renault_0435r, sizeof(r_device));
      memcpy(&cfg->devices[58], &tpms_truck, sizeof(r_device));
    }

    // end of fragement

#else
    memcpy(&cfg->devices[0], &oregon_scientific, sizeof(r_device));
#endif

#ifdef RTL_FLEX
    // This option is non-functional. The flex decoder is too resource intensive
    // for an ESP32, and needs the ESP32 stack set to 32768 in order for the
    // flex_callback to execute Tested with
    // -DRTL_FLEX="n=Sonoff-PIR3-RF,m=OOK_PWM,s=300,l=860,r=7492,g=868,t=50,y=0,bits>=24,repeats>=5,invert,get=@0:{20}:id,get=@20:{4}:motion:[0:true
    // ],unique"

    r_device* flex_device;
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

#ifdef DEMOD_DEBUG
    logprintfLn(LOG_INFO, "# of device(s) configured %d", cfg->num_r_devices);
    logprintfLn(LOG_INFO, "ssizeof(r_device): %d", sizeof(r_device));
    logprintfLn(LOG_INFO, "cfg->devices size: %d",
                sizeof(r_device) * cfg->num_r_devices);
#endif
#ifdef RTL_DEBUG
    cfg->verbosity = RTL_DEBUG + 5; // 0=normal, 1=verbose, 2=verbose decoders,
        // 3=debug decoders, 4=trace decoding.
#else
    cfg->verbosity = rtlVerbose; // 0=normal, 1=verbose, 2=verbose decoders,
        // 3=debug decoders, 4=trace decoding.
#endif

#ifdef MEMORY_DEBUG
    logprintfLn(LOG_DEBUG, "Pre register_all_protocols heap %d",
                ESP.getFreeHeap());
#endif

    // expand register_all_protocols to determine heap impact from each decoder
    // register_all_protocols(cfg, 0);

    for (int i = 0; i < cfg->num_r_devices; i++) {
// register all device protocols that are not disabled
#ifdef MEMORY_DEBUG
      logprintfLn(LOG_DEBUG, "Pre register_protocol %d %s, heap %d", i,
                  cfg->devices[i].name, ESP.getFreeHeap());
#endif

      char* arg = NULL;
      char verbose[4] = "vvv";
#ifndef RTL_VERBOSE
#  define RTL_VERBOSE -1
#endif
      if (RTL_VERBOSE && i == RTL_VERBOSE) {
        arg = verbose;
      }
      if (cfg->devices[i].disabled <= 0) {
        register_protocol(cfg, &cfg->devices[i], arg);
      }
    }

#ifdef MEMORY_DEBUG
    logprintfLn(LOG_DEBUG, "Pre xQueueCreate heap %d", ESP.getFreeHeap());
#endif
    rtl_433_Queue = xQueueCreate(5, sizeof(pulse_data_t*));

#ifdef MEMORY_DEBUG
    logprintfLn(LOG_DEBUG, "Pre xTaskCreatePinnedToCore heap %d",
                ESP.getFreeHeap());
#endif

    xTaskCreatePinnedToCore(
        rtl_433_DecoderTask, /* Function to implement the task */
        "rtl_433_DecoderTask", /* Name of the task */
        rtl_433_Decoder_Stack, /* Stack size in bytes */
        NULL, /* Task input parameter */
        rtl_433_Decoder_Priority, /* Priority of the task (set lower than core task) */
        &rtl_433_DecoderHandle, /* Task handle. */
        rtl_433_Decoder_Core); /* Core where the task should run */
  }
}

void _setCallback(rtl_433_ESPCallBack callback, char* messageBuffer,
                  int bufferSize) {
  // logprintfLn(LOG_DEBUG, "_setCallback location: %p", callback);

  r_cfg_t* cfg = &g_cfg;
  cfg->callback = callback;
  cfg->messageBuffer = messageBuffer;
  cfg->bufferSize = bufferSize;
}

void _setDebug(int debug) {
  rtlVerbose = debug;
  logprintfLn(LOG_INFO, "Setting rtl_433 debug to: %d", rtlVerbose);
}

// ---------------------------------------------------------------------------------------------------------

void rtl_433_DecoderTask(void* pvParameters) {
  pulse_data_t* rtl_pulses = nullptr;
  for (;;) {
    // logprintfLn(LOG_DEBUG, "rtl_433_DecoderTask awaiting signal");
    xQueueReceive(rtl_433_Queue, &rtl_pulses, portMAX_DELAY);
    // logprintfLn(LOG_DEBUG, "rtl_433_DecoderTask signal received");
#ifdef MEMORY_DEBUG
    unsigned long signalProcessingStart = micros();
#endif

#ifdef RAW_SIGNAL_DEBUG
    logprintf(LOG_INFO, "RAW (%lu): ", rtl_pulses->signalDuration);
    for (int i = 0; i < rtl_pulses->num_pulses; i++) {
      alogprintf(LOG_INFO, "+%d", rtl_pulses->pulse[i]);
      alogprintf(LOG_INFO, "-%d", rtl_pulses->gap[i]);
#  ifdef SIGNAL_RSSI
      alogprintf(LOG_INFO, "(%d)", rtl_pulses->rssi[i]);
#  endif
    }
    alogprintfLn(LOG_INFO, " ");
#endif
#ifdef MEMORY_DEBUG
    logprintfLn(LOG_INFO, "Pre run_%s_demods: %d", rtl_433_ESP::ookModulation ? "OOK" : "FSK", ESP.getFreeHeap());
#endif
    rtl_pulses->sample_rate = 1.0e6;
    r_cfg_t* cfg = &g_cfg;
    cfg->demod->pulse_data = *rtl_pulses;
    int events = 0;

    if (rtl_433_ESP::ookModulation) {
      events = run_ook_demods(&cfg->demod->r_devs, rtl_pulses);
    } else {
      events = run_fsk_demods(&cfg->demod->r_devs, rtl_pulses);
    }
    if (events == 0) {
#ifdef RTL_ANALYZER
      pulse_analyzer(rtl_pulses, rtl_433_ESP::ookModulation ? 1 : 2);
#endif
      rtl_433_ESP::unparsedSignals++;
#ifdef PUBLISH_UNPARSED
      logprintf(LOG_INFO, "Unparsed Signal length: %lu",
                rtl_pulses->signalDuration);
      alogprintf(LOG_INFO, ", Signal RSSI: %d", rtl_pulses->signalRssi);
      //      alogprintf(LOG_INFO, ", train: %d", _actualPulseTrain);
      //      alogprintf(LOG_INFO, ", messageCount: %d", messageCount);
      alogprintfLn(LOG_INFO, ", pulses: %d", rtl_pulses->num_pulses);

      logprintf(LOG_INFO, "RAW (%lu): ", rtl_pulses->signalDuration);
#  ifndef RAW_SIGNAL_DEBUG
      for (int i = 0; i < rtl_pulses->num_pulses; i++) {
        alogprintf(LOG_INFO, "+%d", rtl_pulses->pulse[i]);
        alogprintf(LOG_INFO, "-%d", rtl_pulses->gap[i]);
#    ifdef SIGNAL_RSSI
        alogprintf(LOG_INFO, "(%d)", rtl_pulses->rssi[i]);
#    endif
      }
      alogprintfLn(LOG_INFO, " ");
#  endif

      // Send a note saying unparsed signal signal received
      data_t* data;
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

      r_cfg_t* cfg = &g_cfg;
      data_print_jsons(data, cfg->messageBuffer, cfg->bufferSize);
      (cfg->callback)(cfg->messageBuffer);
      data_free(data);

#endif
    }

#ifdef MEMORY_DEBUG
    logprintfLn(LOG_INFO, "Signal processing time: %lu",
                micros() - signalProcessingStart);
    logprintfLn(LOG_INFO, "Post run_ook_demods memory %d", ESP.getFreeHeap());
#endif
#ifdef DEMOD_DEBUG
    logprintfLn(LOG_INFO, "# of messages decoded %d", events);
#endif
    if (events > 0) {
      // alogprintfLn(LOG_INFO, " ");
    }
#if defined(MEMORY_DEBUG)
    else {
      logprintfLn(LOG_DEBUG, "Process rtl_433_DecoderTask stack free: %u",
                  uxTaskGetStackHighWaterMark(rtl_433_DecoderHandle));
      alogprintfLn(LOG_INFO, " ");
    }
#endif
#ifdef MEMORY_DEBUG
    logprintfLn(LOG_INFO, "Pre free rtl_433_DecoderTask: %d",
                ESP.getFreeHeap());
#endif
    free(rtl_pulses);
#ifdef MEMORY_DEBUG
    logprintfLn(LOG_INFO, "Post free rtl_433_DecoderTask: %d",
                ESP.getFreeHeap());
    logprintfLn(LOG_INFO, "rtl_433_DecoderTask uxTaskGetStackHighWaterMark: %d",
                uxTaskGetStackHighWaterMark(NULL));
#endif
  }
}

void processSignal(pulse_data_t* rtl_pulses) {
  // logprintfLn(LOG_DEBUG, "processSignal() about to place signal on
  // rtl_433_Queue");
  if (xQueueSend(rtl_433_Queue, &rtl_pulses, 0) != pdTRUE) {
    logprintfLn(LOG_ERR, "ERROR: rtl_433_Queue full, discarding signal");
    free(rtl_pulses);
  } else {
    // logprintfLn(LOG_DEBUG, "processSignal() signal placed on rtl_433_Queue");
  }
}