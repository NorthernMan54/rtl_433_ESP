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
#include "log.h"

// ESP32 doesn't define ICACHE_RAM_ATTR
#ifndef ICACHE_RAM_ATTR
#define ICACHE_RAM_ATTR IRAM_ATTR
#endif

#define ONBOARD_LED 2

extern "C"
{
#include "bitbuffer.h"
#include "pulse_detect.h"
#include "pulse_demod.h"
#include "list.h"
#include "rtl_devices.h"
#include "r_api.h"
#include "r_private.h"
#include "rtl_433.h"
#include "rtl_433_devices.h"
#include "fatal.h"
  // #include "decoder.h"
}

static bool receiveMode = false;
static unsigned long signalStart = micros();
static unsigned long gapStart = micros();
static int messageCount = 0;
static int currentRssi = 0;
static int signalRssi = 0;
static int minimumRssi = 5;

volatile RTL433PulseTrain_t rtl_433_ESP::_pulseTrains[RECEIVER_BUFFER_SIZE];
uint16_t rtl_433_ESP::pulses[MAXPULSESTREAMLENGTH];
boolean rtl_433_ESP::pins[MAXPULSESTREAMLENGTH];
#ifdef RSSI
int rtl_433_ESP::rssi[MAXPULSESTREAMLENGTH];
#endif

bool rtl_433_ESP::_enabledReceiver;
volatile uint8_t rtl_433_ESP::_actualPulseTrain = 0;
uint8_t rtl_433_ESP::_avaiablePulseTrain = 0;
volatile unsigned long rtl_433_ESP::_lastChange =
    0; // Timestamp of previous edge
volatile uint16_t rtl_433_ESP::_nrpulses = 0;

int16_t rtl_433_ESP::_interrupt = NOT_AN_INTERRUPT;
static byte receiverGpio = -1;

static unsigned long signalEnd = micros();

r_cfg_t rtl_433_ESP::g_cfg;

void rtlSetup(r_cfg_t *cfg)
{
  unsigned i;
#ifdef MEMORY_DEBUG
  logprintfLn(LOG_INFO, "sizeof(*cfg->demod) %d", sizeof(*cfg->demod));
#endif
  r_init_cfg(cfg);

  cfg->num_r_devices = NUMOFDEVICES;
  cfg->devices = (r_device *)calloc(cfg->num_r_devices, sizeof(r_device));
  if (!cfg->devices)
    FATAL_CALLOC("cfg->devices");

    // cat rtl_433_devices.h | grep DECL | cut -c10- | awk -F\) '{ print "memcpy(&cfg->devices["NR-1"], &"$1", sizeof(r_device));" }'
#ifndef MY_DEVICES
  memcpy(&cfg->devices[0], &silvercrest, sizeof(r_device));
  memcpy(&cfg->devices[1], &rubicson, sizeof(r_device));
  memcpy(&cfg->devices[2], &prologue, sizeof(r_device));
  memcpy(&cfg->devices[3], &waveman, sizeof(r_device));
  memcpy(&cfg->devices[4], &new_template, sizeof(r_device));
  memcpy(&cfg->devices[5], &elv_em1000, sizeof(r_device));
  memcpy(&cfg->devices[6], &elv_ws2000, sizeof(r_device));
  memcpy(&cfg->devices[7], &lacrossetx, sizeof(r_device));
  memcpy(&cfg->devices[8], &new_template, sizeof(r_device));
  memcpy(&cfg->devices[9], &acurite_rain_896, sizeof(r_device));
  memcpy(&cfg->devices[10], &acurite_th, sizeof(r_device));
  memcpy(&cfg->devices[11], &oregon_scientific, sizeof(r_device));
  memcpy(&cfg->devices[12], &mebus433, sizeof(r_device));
  memcpy(&cfg->devices[13], &intertechno, sizeof(r_device));
  memcpy(&cfg->devices[14], &newkaku, sizeof(r_device));
  memcpy(&cfg->devices[15], &alectov1, sizeof(r_device));
  memcpy(&cfg->devices[16], &cardin, sizeof(r_device));
  memcpy(&cfg->devices[17], &fineoffset_WH2, sizeof(r_device));
  memcpy(&cfg->devices[18], &nexus, sizeof(r_device));
  memcpy(&cfg->devices[19], &ambient_weather, sizeof(r_device));
  memcpy(&cfg->devices[20], &calibeur_RF104, sizeof(r_device));
  memcpy(&cfg->devices[21], &X10_RF, sizeof(r_device));
  memcpy(&cfg->devices[22], &dsc_security, sizeof(r_device));
  memcpy(&cfg->devices[23], &brennenstuhl_rcs_2044, sizeof(r_device));
  memcpy(&cfg->devices[24], &gt_wt_02, sizeof(r_device));
  memcpy(&cfg->devices[25], &danfoss_CFR, sizeof(r_device));
  memcpy(&cfg->devices[26], &new_template, sizeof(r_device));
  memcpy(&cfg->devices[27], &new_template, sizeof(r_device));
  memcpy(&cfg->devices[28], &chuango, sizeof(r_device));
  memcpy(&cfg->devices[29], &generic_remote, sizeof(r_device));
  memcpy(&cfg->devices[30], &tfa_twin_plus_303049, sizeof(r_device));
  memcpy(&cfg->devices[31], &fineoffset_wh1080, sizeof(r_device));
  memcpy(&cfg->devices[32], &wt450, sizeof(r_device));
  memcpy(&cfg->devices[33], &lacrossews, sizeof(r_device));
  memcpy(&cfg->devices[34], &esperanza_ews, sizeof(r_device));
  memcpy(&cfg->devices[35], &efergy_e2_classic, sizeof(r_device));
  memcpy(&cfg->devices[36], &kw9015b, sizeof(r_device));
  memcpy(&cfg->devices[37], &generic_temperature_sensor, sizeof(r_device));
  memcpy(&cfg->devices[38], &wg_pb12v1, sizeof(r_device));
  memcpy(&cfg->devices[39], &acurite_txr, sizeof(r_device));
  memcpy(&cfg->devices[40], &acurite_986, sizeof(r_device));
  memcpy(&cfg->devices[41], &hideki_ts04, sizeof(r_device));
  memcpy(&cfg->devices[42], &oil_watchman, sizeof(r_device));
  memcpy(&cfg->devices[43], &current_cost, sizeof(r_device));
  memcpy(&cfg->devices[44], &emontx, sizeof(r_device));
  memcpy(&cfg->devices[45], &ht680, sizeof(r_device));
  memcpy(&cfg->devices[46], &s3318p, sizeof(r_device));
  memcpy(&cfg->devices[47], &akhan_100F14, sizeof(r_device));
  memcpy(&cfg->devices[48], &quhwa, sizeof(r_device));
  memcpy(&cfg->devices[49], &oregon_scientific_v1, sizeof(r_device));
  memcpy(&cfg->devices[50], &skylink_motion, sizeof(r_device));
  memcpy(&cfg->devices[51], &ecodhome, sizeof(r_device));
  memcpy(&cfg->devices[52], &springfield, sizeof(r_device));
  memcpy(&cfg->devices[53], &oregon_scientific_sl109h, sizeof(r_device));
  memcpy(&cfg->devices[54], &acurite_606, sizeof(r_device));
  memcpy(&cfg->devices[55], &tfa_pool_thermometer, sizeof(r_device));
  memcpy(&cfg->devices[56], &kedsum, sizeof(r_device));
  memcpy(&cfg->devices[57], &blyss, sizeof(r_device));
  memcpy(&cfg->devices[58], &steelmate, sizeof(r_device));
  memcpy(&cfg->devices[59], &schraeder, sizeof(r_device));
  memcpy(&cfg->devices[60], &lightwave_rf, sizeof(r_device));
  memcpy(&cfg->devices[61], &elro_db286a, sizeof(r_device));
  memcpy(&cfg->devices[62], &efergy_optical, sizeof(r_device));
  memcpy(&cfg->devices[63], &hondaremote, sizeof(r_device));
  memcpy(&cfg->devices[64], &new_template, sizeof(r_device));
  memcpy(&cfg->devices[65], &new_template, sizeof(r_device));
  memcpy(&cfg->devices[66], &radiohead_ask, sizeof(r_device));
  memcpy(&cfg->devices[67], &kerui, sizeof(r_device));
  memcpy(&cfg->devices[68], &fineoffset_wh1050, sizeof(r_device));
  memcpy(&cfg->devices[69], &honeywell, sizeof(r_device));
  memcpy(&cfg->devices[70], &maverick_et73x, sizeof(r_device));
  memcpy(&cfg->devices[71], &rftech, sizeof(r_device));
  memcpy(&cfg->devices[72], &lacrosse_tx141x, sizeof(r_device));
  memcpy(&cfg->devices[73], &acurite_00275rm, sizeof(r_device));
  memcpy(&cfg->devices[74], &lacrosse_tx35, sizeof(r_device));
  memcpy(&cfg->devices[75], &lacrosse_tx29, sizeof(r_device));
  memcpy(&cfg->devices[76], &vaillant_vrt340f, sizeof(r_device));
  memcpy(&cfg->devices[77], &fineoffset_WH25, sizeof(r_device));
  memcpy(&cfg->devices[78], &fineoffset_WH0530, sizeof(r_device));
  memcpy(&cfg->devices[79], &ibis_beacon, sizeof(r_device));
  memcpy(&cfg->devices[80], &oil_standard, sizeof(r_device));
  memcpy(&cfg->devices[81], &tpms_citroen, sizeof(r_device));
  memcpy(&cfg->devices[82], &oil_standard_ask, sizeof(r_device));
  memcpy(&cfg->devices[83], &thermopro_tp11, sizeof(r_device));
  memcpy(&cfg->devices[84], &solight_te44, sizeof(r_device));
  memcpy(&cfg->devices[85], &smoke_gs558, sizeof(r_device));
  memcpy(&cfg->devices[86], &generic_motion, sizeof(r_device));
  memcpy(&cfg->devices[87], &tpms_toyota, sizeof(r_device));
  memcpy(&cfg->devices[88], &tpms_ford, sizeof(r_device));
  memcpy(&cfg->devices[89], &tpms_renault, sizeof(r_device));
  memcpy(&cfg->devices[90], &infactory, sizeof(r_device));
  memcpy(&cfg->devices[91], &ft004b, sizeof(r_device));
  memcpy(&cfg->devices[92], &fordremote, sizeof(r_device));
  memcpy(&cfg->devices[93], &philips_aj3650, sizeof(r_device));
  memcpy(&cfg->devices[94], &schrader_EG53MA4, sizeof(r_device));
  memcpy(&cfg->devices[95], &nexa, sizeof(r_device));
  memcpy(&cfg->devices[96], &thermopro_tp12, sizeof(r_device));
  memcpy(&cfg->devices[97], &ge_coloreffects, sizeof(r_device));
  memcpy(&cfg->devices[98], &x10_sec, sizeof(r_device));
  memcpy(&cfg->devices[99], &interlogix, sizeof(r_device));
  memcpy(&cfg->devices[100], &dish_remote_6_3, sizeof(r_device));
  memcpy(&cfg->devices[101], &ss_sensor, sizeof(r_device));
  memcpy(&cfg->devices[102], &sensible_living, sizeof(r_device));
  memcpy(&cfg->devices[103], &m_bus_mode_c_t, sizeof(r_device));
  memcpy(&cfg->devices[104], &m_bus_mode_s, sizeof(r_device));
  memcpy(&cfg->devices[105], &m_bus_mode_r, sizeof(r_device));
  memcpy(&cfg->devices[106], &m_bus_mode_f, sizeof(r_device));
  memcpy(&cfg->devices[107], &wssensor, sizeof(r_device));
  memcpy(&cfg->devices[108], &wt1024, sizeof(r_device));
  memcpy(&cfg->devices[109], &tpms_pmv107j, sizeof(r_device));
  memcpy(&cfg->devices[110], &ttx201, sizeof(r_device));
  memcpy(&cfg->devices[111], &ambientweather_tx8300, sizeof(r_device));
  memcpy(&cfg->devices[112], &ambientweather_wh31e, sizeof(r_device));
  memcpy(&cfg->devices[113], &maverick_et73, sizeof(r_device));
  memcpy(&cfg->devices[114], &honeywell_wdb, sizeof(r_device));
  memcpy(&cfg->devices[115], &honeywell_wdb_fsk, sizeof(r_device));
  memcpy(&cfg->devices[116], &esa_energy, sizeof(r_device));
  memcpy(&cfg->devices[117], &bt_rain, sizeof(r_device));
  memcpy(&cfg->devices[118], &lacrosse_th3, sizeof(r_device));
  memcpy(&cfg->devices[119], &digitech_xc0324, sizeof(r_device));
  memcpy(&cfg->devices[120], &opus_xt300, sizeof(r_device));
  memcpy(&cfg->devices[121], &fs20, sizeof(r_device));
  memcpy(&cfg->devices[122], &tpms_jansite, sizeof(r_device));
  memcpy(&cfg->devices[123], &lacrosse_ws7000, sizeof(r_device));
  memcpy(&cfg->devices[124], &ts_ft002, sizeof(r_device));
  memcpy(&cfg->devices[125], &companion_wtr001, sizeof(r_device));
  memcpy(&cfg->devices[126], &ecowitt, sizeof(r_device));
  memcpy(&cfg->devices[127], &directv, sizeof(r_device));
  memcpy(&cfg->devices[128], &eurochron, sizeof(r_device));
  memcpy(&cfg->devices[129], &ikea_sparsnas, sizeof(r_device));
  memcpy(&cfg->devices[130], &hcs200, sizeof(r_device));
  memcpy(&cfg->devices[131], &tfa_303196, sizeof(r_device));
  memcpy(&cfg->devices[132], &rubicson_48659, sizeof(r_device));
  memcpy(&cfg->devices[133], &holman_ws5029pcm, sizeof(r_device));
  memcpy(&cfg->devices[134], &philips_aj7010, sizeof(r_device));
  memcpy(&cfg->devices[135], &esic_emt7110, sizeof(r_device));
  memcpy(&cfg->devices[136], &gt_tmbbq05, sizeof(r_device));
  memcpy(&cfg->devices[137], &gt_wt_03, sizeof(r_device));
  memcpy(&cfg->devices[138], &norgo, sizeof(r_device));
  memcpy(&cfg->devices[139], &tpms_elantra2012, sizeof(r_device));
  memcpy(&cfg->devices[140], &auriol_hg02832, sizeof(r_device));
  memcpy(&cfg->devices[141], &fineoffset_WH51, sizeof(r_device));
  memcpy(&cfg->devices[142], &holman_ws5029pwm, sizeof(r_device));
  memcpy(&cfg->devices[143], &archos_tbh, sizeof(r_device));
  memcpy(&cfg->devices[144], &ws2032, sizeof(r_device));
  memcpy(&cfg->devices[145], &auriol_afw2a1, sizeof(r_device));
  memcpy(&cfg->devices[146], &tfa_drop_303233, sizeof(r_device));
  memcpy(&cfg->devices[147], &dsc_security_ws4945, sizeof(r_device));
  memcpy(&cfg->devices[148], &ert_scm, sizeof(r_device));
  memcpy(&cfg->devices[149], &klimalogg, sizeof(r_device));
  memcpy(&cfg->devices[150], &visonic_powercode, sizeof(r_device));
  memcpy(&cfg->devices[151], &eurochron_efth800, sizeof(r_device));
  memcpy(&cfg->devices[152], &cotech_36_7959, sizeof(r_device));
  memcpy(&cfg->devices[153], &scmplus, sizeof(r_device));
  memcpy(&cfg->devices[154], &fineoffset_wh1080_fsk, sizeof(r_device));
  memcpy(&cfg->devices[155], &tpms_abarth124, sizeof(r_device));
  memcpy(&cfg->devices[156], &missil_ml0757, sizeof(r_device));
  memcpy(&cfg->devices[157], &sharp_spc775, sizeof(r_device));
  memcpy(&cfg->devices[158], &insteon, sizeof(r_device));
  memcpy(&cfg->devices[159], &ert_idm, sizeof(r_device));
  memcpy(&cfg->devices[160], &ert_netidm, sizeof(r_device));
  memcpy(&cfg->devices[161], &thermopro_tx2, sizeof(r_device));
  memcpy(&cfg->devices[162], &acurite_590tx, sizeof(r_device));
  memcpy(&cfg->devices[163], &burnhardbbq, sizeof(r_device));
  memcpy(&cfg->devices[164], &tfa_30_3221, sizeof(r_device));
  memcpy(&cfg->devices[165], &lacrosse_breezepro, sizeof(r_device));
  memcpy(&cfg->devices[166], &blueline, sizeof(r_device));
  memcpy(&cfg->devices[167], &lacrosse_r1, sizeof(r_device));
  memcpy(&cfg->devices[168], &nice_flor_s, sizeof(r_device));
  memcpy(&cfg->devices[169], &lacrosse_wr1, sizeof(r_device));
  memcpy(&cfg->devices[170], &bresser_5in1, sizeof(r_device));
  memcpy(&cfg->devices[171], &bresser_6in1, sizeof(r_device));
  memcpy(&cfg->devices[172], &bresser_7in1, sizeof(r_device));
  memcpy(&cfg->devices[173], &bresser_3ch, sizeof(r_device));
  memcpy(&cfg->devices[174], &schrader_SMD3MA4, sizeof(r_device));
  memcpy(&cfg->devices[175], &somfy_rts, sizeof(r_device));
  memcpy(&cfg->devices[176], &secplus_v2, sizeof(r_device));
  memcpy(&cfg->devices[177], &proove, sizeof(r_device));
#else
  memcpy(&cfg->devices[0], &skylink_motion, sizeof(r_device));
  memcpy(&cfg->devices[1], &prologue, sizeof(r_device));
  memcpy(&cfg->devices[2], &acurite_986, sizeof(r_device));
  memcpy(&cfg->devices[3], &philips_aj3650, sizeof(r_device));
#endif

// r_device [177]{(struct r_device)silvercrest, (struct r_device)rubicson, (struct r_device)prologue, (struct r_device)waveman, (struct r_device)new_template, (struct r_device)elv_em1000, (struct r_device)elv_ws2000, (struct r_device)lacrossetx, (struct r_device)new_template, (struct r_device)acurite_rain_896, (struct r_device)acurite_th, (struct r_device)oregon_scientific, (struct r_device)mebus433, (struct r_device)intertechno, (struct r_device)newkaku, (struct r_device)alectov1, (struct r_device)cardin, (struct r_device)fineoffset_WH2, (struct r_device)nexus, (struct r_device)ambient_weather, (struct r_device)calibeur_RF104, (struct r_device)X10_RF, (struct r_device)dsc_security, (struct r_device)brennenstuhl_rcs_2044, (struct r_device)gt_wt_02, (struct r_device)danfoss_CFR, (struct r_device)new_template, (struct r_device)new_template, (struct r_device)chuango, (struct r_device)generic_remote, (struct r_device)tfa_twin_plus_303049, (struct r_device)fineoffset_wh1080, (struct r_device)wt450, (struct r_device)lacrossews, (struct r_device)esperanza_ews, (struct r_device)efergy_e2_classic, (struct r_device)kw9015b, (struct r_device)generic_temperature_sensor, (struct r_device)wg_pb12v1, (struct r_device)acurite_txr, (struct r_device)acurite_986, (struct r_device)hideki_ts04, (struct r_device)oil_watchman, (struct r_device)current_cost, (struct r_device)emontx, (struct r_device)ht680, (struct r_device)s3318p, (struct r_device)akhan_100F14, (struct r_device)quhwa, (struct r_device)oregon_scientific_v1, (struct r_device)proove, (struct r_device)bresser_3ch, (struct r_device)springfield, (struct r_device)oregon_scientific_sl109h, (struct r_device)acurite_606, (struct r_device)tfa_pool_thermometer, (struct r_device)kedsum, (struct r_device)blyss, (struct r_device)steelmate, (struct r_device)schraeder, (struct r_device)lightwave_rf, (struct r_device)elro_db286a, (struct r_device)efergy_optical, (struct r_device)hondaremote, (struct r_device)new_template, (struct r_device)new_template, (struct r_device)radiohead_ask, (struct r_device)kerui, (struct r_device)fineoffset_wh1050, (struct r_device)honeywell, (struct r_device)maverick_et73x, (struct r_device)rftech, (struct r_device)lacrosse_tx141x, (struct r_device)acurite_00275rm, (struct r_device)lacrosse_tx35, (struct r_device)lacrosse_tx29, (struct r_device)vaillant_vrt340f, (struct r_device)fineoffset_WH25, (struct r_device)fineoffset_WH0530, (struct r_device)ibis_beacon, (struct r_device)oil_standard, (struct r_device)tpms_citroen, (struct r_device)oil_standard_ask, (struct r_device)thermopro_tp11, (struct r_device)solight_te44, (struct r_device)smoke_gs558, (struct r_device)generic_motion, (struct r_device)tpms_toyota, (struct r_device)tpms_ford, (struct r_device)tpms_renault, (struct r_device)infactory, (struct r_device)ft004b, (struct r_device)fordremote, (struct r_device)philips_aj3650, (struct r_device)schrader_EG53MA4, (struct r_device)nexa, (struct r_device)thermopro_tp12, (struct r_device)ge_coloreffects, (struct r_device)x10_sec, (struct r_device)interlogix, (struct r_device)dish_remote_6_3, (struct r_device)ss_sensor, (struct r_device)sensible_living, (struct r_device)m_bus_mode_c_t, (struct r_device)m_bus_mode_s, (struct r_device)m_bus_mode_r, (struct r_device)m_bus_mode_f, (struct r_device)wssensor, (struct r_device)wt1024, (struct r_device)tpms_pmv107j, (struct r_device)ttx201, (struct r_device)ambientweather_tx8300, (struct r_device)ambientweather_wh31e, (struct r_device)maverick_et73, (struct r_device)honeywell_wdb, (struct r_device)honeywell_wdb_fsk, (struct r_device)esa_energy, (struct r_device)bt_rain, (struct r_device)bresser_5in1, (struct r_device)digitech_xc0324, (struct r_device)opus_xt300, (struct r_device)fs20, (struct r_device)tpms_jansite, (struct r_device)lacrosse_ws7000, (struct r_device)ts_ft002, (struct r_device)companion_wtr001, (struct r_device)ecowitt, (struct r_device)directv, (struct r_device)eurochron, (struct r_device)ikea_sparsnas, (struct r_device)hcs200, (struct r_device)tfa_303196, (struct r_device)rubicson_48659, (struct r_device)holman_ws5029pcm, (struct r_device)philips_aj7010, (struct r_device)esic_emt7110, (struct r_device)gt_tmbbq05, (struct r_device)gt_wt_03, (struct r_device)norgo, (struct r_device)tpms_elantra2012, (struct r_device)auriol_hg02832, (struct r_device)fineoffset_WH51, (struct r_device)holman_ws5029pwm, (struct r_device)archos_tbh, (struct r_device)ws2032, (struct r_device)auriol_afw2a1, (struct r_device)tfa_drop_303233, (struct r_device)dsc_security_ws4945, (struct r_device)ert_scm, (struct r_device)klimalogg, (struct r_device)visonic_powercode, (struct r_device)eurochron_efth800, (struct r_device)cotech_36_7959, (struct r_device)scmplus, (struct r_device)fineoffset_wh1080_fsk, (struct r_device)tpms_abarth124, (struct r_device)missil_ml0757, (struct r_device)sharp_spc775, (struct r_device)insteon, (struct r_device)ert_idm, (struct r_device)ert_netidm, (struct r_device)thermopro_tx2, (struct r_device)acurite_590tx, (struct r_device)secplus_v2, (struct r_device)tfa_30_3221, (struct r_device)lacrosse_breezepro, (struct r_device)somfy_rts, (struct r_device)schrader_SMD3MA4, (struct r_device)nice_flor_s, (struct r_device)lacrosse_wr1, (struct r_device)lacrosse_th3, (struct r_device)bresser_6in1, (struct r_device)bresser_7in1, (struct r_device)ecodhome, (struct r_device)lacrosse_r1, (struct r_device)blueline, (struct r_device)burnhardbbq}

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
  cfg->verbosity = 1;
#endif
  register_all_protocols(cfg, 0);
}

void rtl_433_ESP::initReceiver(byte inputPin, float receiveFrequency)
{
  receiverGpio = inputPin;
#ifdef MEMORY_DEBUG
  logprintfLn(LOG_INFO, "Pre initReceiver: %d", ESP.getFreeHeap());
#endif
#ifdef DEMOD_DEBUG
  logprintfLn(LOG_INFO, "CC1101 gpio receive pin: %d", inputPin);
  logprintfLn(LOG_INFO, "CC1101 receive frequency: %f", receiveFrequency);
#endif
  r_cfg_t *cfg = &g_cfg;

  rtlSetup(cfg);

  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.SetRx(receiveFrequency);
  minimumRssi = MINRSSI;
#ifdef DEMOD_DEBUG
  logprintfLn(LOG_INFO, "CC1101 minumum rssi: %d", minimumRssi);
#endif
  resetReceiver();
  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, LOW);
#ifdef MEMORY_DEBUG
  logprintfLn(LOG_INFO, "Post initReceiver: %d", ESP.getFreeHeap());
#endif
}

#ifdef RSSI
uint16_t rtl_433_ESP::receivePulseTrain(uint16_t *pulses, boolean *pins, int *rssi)
#else
uint16_t rtl_433_ESP::receivePulseTrain(uint16_t *pulses, boolean *pins)
#endif
{
  uint16_t length = nextPulseTrainLength();

  if (length > 0)
  {
    volatile RTL433PulseTrain_t &pulseTrain = _pulseTrains[_avaiablePulseTrain];
    _avaiablePulseTrain = (_avaiablePulseTrain + 1) % RECEIVER_BUFFER_SIZE;
    for (uint16_t i = 0; i < length; i++)
    {
      pulses[i] = pulseTrain.pulses[i];
      pins[i] = pulseTrain.pins[i];
#ifdef RSSI
      rssi[i] = pulseTrain.rssi[i];
#endif
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
  volatile RTL433PulseTrain_t &pulseTrain = _pulseTrains[_actualPulseTrain];
  volatile uint16_t *codes = pulseTrain.pulses;
  volatile boolean *pins = pulseTrain.pins;
#ifdef RSSI
  volatile int *rssi = pulseTrain.rssi;
#endif

  const unsigned long now = micros();
  const unsigned int duration = now - _lastChange;

  /* We first do some filtering (same as pilight BPF) */
  if (duration > MINIMUM_PULSE_LENGTH && currentRssi > minimumRssi)
  {
    //        if (duration < maxpulselen)
    //        {
    /* All codes are buffered */
    codes[_nrpulses] = (uint16_t)duration;
#ifdef RSSI
    rssi[_nrpulses] = currentRssi;
#endif
    if (digitalRead(receiverGpio))
    {
      pins[_nrpulses] = true;
    }
    else
    {
      pins[_nrpulses] = false;
    }
    _nrpulses = (uint16_t)((_nrpulses + 1) % MAXPULSESTREAMLENGTH);
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
  currentRssi = ELECHOUSE_cc1101.getRssi();
  if (currentRssi > minimumRssi)
  {
    if (!receiveMode)
    {
      receiveMode = true;
      signalStart = micros();
      enableReceiver(receiverGpio);
      digitalWrite(ONBOARD_LED, HIGH);
      signalRssi = currentRssi;
    }
    signalEnd = micros();
  }
  // If we received a signal but had a minor drop in strength keep the receiver running for an additional 20,0000
  else if (micros() - signalEnd < 40000 && micros() - signalStart > 30000)
  {
    // skip over signal drop outs
  }
  else
  {
    if (receiveMode)
    {
      digitalWrite(ONBOARD_LED, LOW);
      receiveMode = false;
      enableReceiver(-1);
      if (_nrpulses > 30 && (micros() - signalStart > 40000))
      {
        alogprintfLn(LOG_INFO, " ");
#ifdef DEMOD_DEBUG
        logprintf(LOG_INFO, "Signal length: %lu", micros() - signalStart);
        alogprintf(LOG_INFO, ", Gap length: %lu", signalStart - gapStart);
        alogprintf(LOG_INFO, ", Signal RSSI: %d", signalRssi);
        alogprintf(LOG_INFO, ", train: %d", _actualPulseTrain);
        alogprintf(LOG_INFO, ", messageCount: %d", messageCount);
        alogprintfLn(LOG_INFO, ", pulses: %d", _nrpulses);
#endif
        messageCount++;

        gapStart = micros();
        _pulseTrains[_actualPulseTrain].length = _nrpulses;
        _pulseTrains[_actualPulseTrain].duration = micros() - signalStart;
        _pulseTrains[_actualPulseTrain].signalRssi = signalRssi;
        _actualPulseTrain = (_actualPulseTrain + 1) % RECEIVER_BUFFER_SIZE;

        _nrpulses = 0;
      }
      else
      {

        _nrpulses = 0;
      }
    }
  }
#ifdef RSSI
  uint16_t length = receivePulseTrain(pulses, pins, rssi);
#else
  uint16_t length = receivePulseTrain(pulses, pins);
#endif

  if (length > 6)
  {
    unsigned long signalProcessingStart = micros();
#ifdef RAW_SIGNAL_DEBUG
    logprintf(LOG_INFO, "RAW (%d): ", length);
#endif
    pulse_data_t *rtl_pulses = (pulse_data_t *)calloc(1, sizeof(pulse_data_t));
    rtl_pulses->sample_rate = 1.0e6;
    for (int i = 0; i < length; i++)
    {
      if (pins[i])
      {
#ifdef RAW_SIGNAL_DEBUG
        alogprintf(LOG_INFO, "+");
#endif
        rtl_pulses->gap[rtl_pulses->num_pulses] = pulses[i];
        rtl_pulses->num_pulses++;
      }
      else
      {
#ifdef RAW_SIGNAL_DEBUG
        alogprintf(LOG_INFO, "-");
#endif
        rtl_pulses->pulse[rtl_pulses->num_pulses] = pulses[i];
      }
#ifdef RAW_SIGNAL_DEBUG
      alogprintf(LOG_INFO, "%d", pulses[i]);
#ifdef RSSI
      alogprintf(LOG_INFO, "(%d)", rssi[i]);
#endif
#endif
    }
#ifdef RAW_SIGNAL_DEBUG
    alogprintfLn(LOG_INFO, " ");
#endif
#ifdef MEMORY_DEBUG
    logprintfLn(LOG_INFO, "Pre run_ook_demods: %d", ESP.getFreeHeap());
#endif

    r_cfg_t *cfg = &g_cfg;

    int events = run_ook_demods(&cfg->demod->r_devs, rtl_pulses);
#ifdef DEMOD_DEBUG
    logprintfLn(LOG_INFO, "# of messages decoded %d", events);
#endif
    if (events == 0)
    {
#ifdef DEMOD_DEBUG
      logprintfLn(LOG_INFO, "Sending debug message", events);
#endif
      logprintf(LOG_INFO, "Signal length: %lu", signalProcessingStart - signalStart);
      alogprintf(LOG_INFO, ", Gap length: %lu", signalStart - gapStart);
      alogprintf(LOG_INFO, ", Signal RSSI: %d", signalRssi);
      alogprintf(LOG_INFO, ", train: %d", _actualPulseTrain);
      alogprintf(LOG_INFO, ", messageCount: %d", messageCount);
      alogprintfLn(LOG_INFO, ", pulses: %d", _nrpulses);
/*      data_t *data;
      data = data_make(
          "model", "", DATA_STRING, "Debug Device",
          "pulses", "pulses", DATA_INT, _nrpulses,
          "duration", "duration", DATA_INT, signalProcessingStart - signalStart,
          "rssi", "rssi", DATA_INT, signalRssi,
          NULL);
      data_acquired_handler(&cfg->devices[0], data);
          */
    }

    free(rtl_pulses);
#ifdef MEMORY_DEBUG
    logprintfLn(LOG_INFO, "Signal processing time: %lu", micros() - signalProcessingStart);
    logprintfLn(LOG_INFO, "Post run_ook_demods memory %d", ESP.getFreeHeap());
#endif
  }
  // alogprintf(LOG_INFO, "%d,", currentRssi);
}

rtl_433_ESP::rtl_433_ESP(int8_t outputPin)
{
  _outputPin = outputPin;

  if (_outputPin >= 0)
  {
    pinMode((uint8_t)_outputPin, OUTPUT);
    digitalWrite((uint8_t)_outputPin, LOW);
  }
}

void rtl_433_ESP::setCallback(rtl_433_ESPCallBack callback, char *messageBuffer, int bufferSize)
{
  r_cfg_t *cfg = &g_cfg;
  cfg->callback = callback;
  cfg->messageBuffer = messageBuffer;
  cfg->bufferSize = bufferSize;
  // logprintfLn(LOG_INFO, "setCallback location: %p", cfg->callback);
}