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

#include <atomic>

/*----------------------------- rtl_433_ESP Internals -----------------------------*/

#ifndef rtl_433_Decoder_Stack
#  if defined(RTL_ANALYZER) || defined(RTL_ANALYZE)
#    define rtl_433_Decoder_Stack 60000
#  elif defined(RTL_VERBOSE) || defined(RTL_DEBUG)
#    define rtl_433_Decoder_Stack 30000
#  else
#    if OOK_MODULATION
#      define rtl_433_Decoder_Stack 11500
#    else
#      define rtl_433_Decoder_Stack 20000
#    endif
#  endif
#endif

#if defined(CONFIG_FREERTOS_UNICORE)
#  define rtl_433_Decoder_Priority 3
#  define rtl_433_Decoder_Core     0
#else
#  define rtl_433_Decoder_Priority 2
#  define rtl_433_Decoder_Core     1
#endif

/*----------------------------- rtl_433_ESP Internals -----------------------------*/

int rtlVerbose = 0;

r_cfg_t g_cfg; // Global config object

TaskHandle_t rtl_433_DecoderHandle;
static QueueHandle_t rtl_433_Queue;
static std::atomic<rtl_433_raw_pulse_cb> rawPulsesCallback{nullptr};

bool rtlSetup() {
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
    cfg->conversion_mode = CONVERT_SI; // Default all output to Celsius
#ifdef MY_DEVICES
    cfg->num_r_devices = NUMOFDEVICES;
#else
    if (rtl_433_ESP::ookModulation) {
      cfg->num_r_devices = NUMOF_OOK_DEVICES;
    } else {
      cfg->num_r_devices = NUMOF_FSK_DEVICES;
    }
#endif
    cfg->devices = reinterpret_cast<r_device*>(calloc(cfg->num_r_devices, sizeof(r_device)));
    if (!cfg->devices)
      FATAL_CALLOC("cfg->devices");

#ifdef MEMORY_DEBUG
    logprintfLn(LOG_DEBUG, "sizeof(cfg) %d, heap %d", sizeof(cfg),
                ESP.getFreeHeap());
#endif

#ifndef MY_DEVICES
    // This is a generated fragment from tools/update_rtl_433_devices.sh

    if (rtl_433_ESP::ookModulation) {
      memcpy(&cfg->devices[0], &abmt, sizeof(r_device));
      memcpy(&cfg->devices[1], &acurite_rain_896, sizeof(r_device));
      memcpy(&cfg->devices[2], &acurite_th, sizeof(r_device));
      memcpy(&cfg->devices[3], &acurite_txr, sizeof(r_device));
      memcpy(&cfg->devices[4], &acurite_986, sizeof(r_device));
      memcpy(&cfg->devices[5], &acurite_606, sizeof(r_device));
      memcpy(&cfg->devices[6], &acurite_00275rm, sizeof(r_device));
      memcpy(&cfg->devices[7], &acurite_590tx, sizeof(r_device));
      memcpy(&cfg->devices[8], &acurite_01185m, sizeof(r_device));
      memcpy(&cfg->devices[9], &akhan_100F14, sizeof(r_device));
      memcpy(&cfg->devices[10], &alectov1, sizeof(r_device));
      memcpy(&cfg->devices[11], &ambient_weather, sizeof(r_device));
      memcpy(&cfg->devices[12], &ambientweather_tx8300, sizeof(r_device));
      memcpy(&cfg->devices[13], &astrostart_2000, sizeof(r_device));
      memcpy(&cfg->devices[14], &atech_ws308, sizeof(r_device));
      memcpy(&cfg->devices[15], &audiovox_pro_oe3b, sizeof(r_device));
      memcpy(&cfg->devices[16], &auriol_4ld5661, sizeof(r_device));
      memcpy(&cfg->devices[17], &auriol_aft77b2, sizeof(r_device));
      memcpy(&cfg->devices[18], &auriol_afw2a1, sizeof(r_device));
      memcpy(&cfg->devices[19], &auriol_ahfl, sizeof(r_device));
      memcpy(&cfg->devices[20], &auriol_hg02832, sizeof(r_device));
      memcpy(&cfg->devices[21], &auriol_hg04641a, sizeof(r_device));
      memcpy(&cfg->devices[22], &baldr_hcs528arf, sizeof(r_device));
      memcpy(&cfg->devices[23], &baldr_rain, sizeof(r_device));
      memcpy(&cfg->devices[24], &baldr_therm, sizeof(r_device));
      memcpy(&cfg->devices[25], &blyss, sizeof(r_device));
      memcpy(&cfg->devices[26], &bm5, sizeof(r_device));
      memcpy(&cfg->devices[27], &brennenstuhl_rcs_2044, sizeof(r_device));
      memcpy(&cfg->devices[28], &bresser_3ch, sizeof(r_device));
      memcpy(&cfg->devices[29], &bresser_st1005h, sizeof(r_device));
      memcpy(&cfg->devices[30], &bt_rain, sizeof(r_device));
      memcpy(&cfg->devices[31], &burnhardbbq, sizeof(r_device));
      memcpy(&cfg->devices[32], &calibeur_RF104, sizeof(r_device));
      memcpy(&cfg->devices[33], &cardin, sizeof(r_device));
      memcpy(&cfg->devices[34], &celsia_czc1, sizeof(r_device));
      memcpy(&cfg->devices[35], &chrysler_car_remote, sizeof(r_device));
      memcpy(&cfg->devices[36], &chuango, sizeof(r_device));
      memcpy(&cfg->devices[37], &cmr113, sizeof(r_device));
      memcpy(&cfg->devices[38], &code_alarm_frdpc2000_car_remote, sizeof(r_device));
      memcpy(&cfg->devices[39], &companion_wtr001, sizeof(r_device));
      memcpy(&cfg->devices[40], &compustar_1wg3r, sizeof(r_device));
      memcpy(&cfg->devices[41], &cotech_36_7900, sizeof(r_device));
      memcpy(&cfg->devices[42], &cotech_36_7959, sizeof(r_device));
      memcpy(&cfg->devices[43], &cotech_ft0203, sizeof(r_device));
      memcpy(&cfg->devices[44], &dickert_pwm, sizeof(r_device));
      memcpy(&cfg->devices[45], &digitech_xc0324, sizeof(r_device));
      memcpy(&cfg->devices[46], &dish_remote_6_3, sizeof(r_device));
      memcpy(&cfg->devices[47], &dsc_security, sizeof(r_device));
      memcpy(&cfg->devices[48], &dsc_security_ws4945, sizeof(r_device));
      memcpy(&cfg->devices[49], &ecowitt, sizeof(r_device));
      memcpy(&cfg->devices[50], &eurochron_efth800, sizeof(r_device));
      memcpy(&cfg->devices[51], &elro_db286a, sizeof(r_device));
      memcpy(&cfg->devices[52], &elv_em1000, sizeof(r_device));
      memcpy(&cfg->devices[53], &elv_ws2000, sizeof(r_device));
      memcpy(&cfg->devices[54], &emos_e6016, sizeof(r_device));
      memcpy(&cfg->devices[55], &emos_e6016_rain, sizeof(r_device));
      memcpy(&cfg->devices[56], &enocean_erp1, sizeof(r_device));
      memcpy(&cfg->devices[57], &ert_idm, sizeof(r_device));
      memcpy(&cfg->devices[58], &ert_netidm, sizeof(r_device));
      memcpy(&cfg->devices[59], &ert_scm, sizeof(r_device));
      memcpy(&cfg->devices[60], &esa_energy, sizeof(r_device));
      memcpy(&cfg->devices[61], &esperanza_ews, sizeof(r_device));
      memcpy(&cfg->devices[62], &esun_en2053, sizeof(r_device));
      memcpy(&cfg->devices[63], &eurochron, sizeof(r_device));
      memcpy(&cfg->devices[64], &fineoffset_WH2, sizeof(r_device));
      memcpy(&cfg->devices[65], &fineoffset_wh5rb, sizeof(r_device));
      memcpy(&cfg->devices[66], &fineoffset_WH0530, sizeof(r_device));
      memcpy(&cfg->devices[67], &fineoffset_wh1050, sizeof(r_device));
      memcpy(&cfg->devices[68], &fineoffset_wh1080, sizeof(r_device));
      memcpy(&cfg->devices[69], &florabest, sizeof(r_device));
      memcpy(&cfg->devices[70], &fordremote, sizeof(r_device));
      memcpy(&cfg->devices[71], &fs20, sizeof(r_device));
      memcpy(&cfg->devices[72], &ft004b, sizeof(r_device));
      memcpy(&cfg->devices[73], &funkbus_remote, sizeof(r_device));
      memcpy(&cfg->devices[74], &gasmate_ba1008, sizeof(r_device));
      memcpy(&cfg->devices[75], &geevon_tx16, sizeof(r_device));
      memcpy(&cfg->devices[76], &geevon_tx19, sizeof(r_device));
      memcpy(&cfg->devices[77], &generic_motion, sizeof(r_device));
      memcpy(&cfg->devices[78], &generic_remote, sizeof(r_device));
      memcpy(&cfg->devices[79], &generic_temperature_sensor, sizeof(r_device));
      memcpy(&cfg->devices[80], &gm_car_remote, sizeof(r_device));
      memcpy(&cfg->devices[81], &govee, sizeof(r_device));
      memcpy(&cfg->devices[82], &govee_h5054, sizeof(r_device));
      memcpy(&cfg->devices[83], &grill_thermometer, sizeof(r_device));
      memcpy(&cfg->devices[84], &gt_tmbbq05, sizeof(r_device));
      memcpy(&cfg->devices[85], &gt_wt_02, sizeof(r_device));
      memcpy(&cfg->devices[86], &gt_wt_03, sizeof(r_device));
      memcpy(&cfg->devices[87], &hcs200, sizeof(r_device));
      memcpy(&cfg->devices[88], &hcs361_txwak_0_bsel_0, sizeof(r_device));
      memcpy(&cfg->devices[89], &hcs361_txwak_0_bsel_1, sizeof(r_device));
      memcpy(&cfg->devices[90], &hcs361_txwak_1_bsel_0, sizeof(r_device));
      memcpy(&cfg->devices[91], &hcs361_txwak_1_bsel_1, sizeof(r_device));
      memcpy(&cfg->devices[92], &hcs361_vpwm_1_bsel_0, sizeof(r_device));
      memcpy(&cfg->devices[93], &hcs361_vpwm_1_bsel_1, sizeof(r_device));
      memcpy(&cfg->devices[94], &hcs362_pwm, sizeof(r_device));
      memcpy(&cfg->devices[95], &hcs362_mc, sizeof(r_device));
      memcpy(&cfg->devices[96], &hideki_ts04, sizeof(r_device));
      memcpy(&cfg->devices[97], &holman_ws5029pwm_ook, sizeof(r_device));
      memcpy(&cfg->devices[98], &homelead_hg9901, sizeof(r_device));
      memcpy(&cfg->devices[99], &honeywell, sizeof(r_device));
      memcpy(&cfg->devices[100], &honeywell_wdb, sizeof(r_device));
      memcpy(&cfg->devices[101], &ht680, sizeof(r_device));
      memcpy(&cfg->devices[102], &ibis_beacon, sizeof(r_device));
      memcpy(&cfg->devices[103], &infactory, sizeof(r_device));
      memcpy(&cfg->devices[104], &kw9015b, sizeof(r_device));
      memcpy(&cfg->devices[105], &interlogix, sizeof(r_device));
      memcpy(&cfg->devices[106], &intertechno, sizeof(r_device));
      memcpy(&cfg->devices[107], &jasco, sizeof(r_device));
      memcpy(&cfg->devices[108], &kedsum, sizeof(r_device));
      memcpy(&cfg->devices[109], &kerui, sizeof(r_device));
      memcpy(&cfg->devices[110], &kidde_smoke, sizeof(r_device));
      memcpy(&cfg->devices[111], &klimalogg, sizeof(r_device));
      memcpy(&cfg->devices[112], &lacrossetx, sizeof(r_device));
      memcpy(&cfg->devices[113], &lacrosse_tx141x, sizeof(r_device));
      memcpy(&cfg->devices[114], &lacrosse_tx22uit_ook, sizeof(r_device));
      memcpy(&cfg->devices[115], &lacrosse_ws7000, sizeof(r_device));
      memcpy(&cfg->devices[116], &lacrossews, sizeof(r_device));
      memcpy(&cfg->devices[117], &lightwave_rf, sizeof(r_device));
      memcpy(&cfg->devices[118], &markisol, sizeof(r_device));
      memcpy(&cfg->devices[119], &martec_mplcd, sizeof(r_device));
      memcpy(&cfg->devices[120], &maverick_et73, sizeof(r_device));
      memcpy(&cfg->devices[121], &maverick_et73x, sizeof(r_device));
      memcpy(&cfg->devices[122], &mebus433, sizeof(r_device));
      memcpy(&cfg->devices[123], &megacode, sizeof(r_device));
      memcpy(&cfg->devices[124], &six_sc_two_car_remote, sizeof(r_device));
      memcpy(&cfg->devices[125], &missil_ml0757, sizeof(r_device));
      memcpy(&cfg->devices[126], &neptune_r900, sizeof(r_device));
      memcpy(&cfg->devices[127], &new_template, sizeof(r_device));
      memcpy(&cfg->devices[128], &newkaku, sizeof(r_device));
      memcpy(&cfg->devices[129], &nexa, sizeof(r_device));
      memcpy(&cfg->devices[130], &nexus, sizeof(r_device));
      memcpy(&cfg->devices[131], &nexus_sauna, sizeof(r_device));
      memcpy(&cfg->devices[132], &nice_flor_s, sizeof(r_device));
      memcpy(&cfg->devices[133], &norgo, sizeof(r_device));
      memcpy(&cfg->devices[134], &oil_standard_ask, sizeof(r_device));
      memcpy(&cfg->devices[135], &omni, sizeof(r_device));
      memcpy(&cfg->devices[136], &opus_xt300, sizeof(r_device));
      memcpy(&cfg->devices[137], &oregon_scientific, sizeof(r_device));
      memcpy(&cfg->devices[138], &oregon_scientific_sl109h, sizeof(r_device));
      memcpy(&cfg->devices[139], &oregon_scientific_v1, sizeof(r_device));
      memcpy(&cfg->devices[140], &oria_wa150km, sizeof(r_device));
      memcpy(&cfg->devices[141], &philips_aj3650, sizeof(r_device));
      memcpy(&cfg->devices[142], &philips_aj7010, sizeof(r_device));
      memcpy(&cfg->devices[143], &proflame2, sizeof(r_device));
      memcpy(&cfg->devices[144], &prologue, sizeof(r_device));
      memcpy(&cfg->devices[145], &proove, sizeof(r_device));
      memcpy(&cfg->devices[146], &quhwa, sizeof(r_device));
      memcpy(&cfg->devices[147], &radiohead_ask, sizeof(r_device));
      memcpy(&cfg->devices[148], &sensible_living, sizeof(r_device));
      memcpy(&cfg->devices[149], &rainpoint, sizeof(r_device));
      memcpy(&cfg->devices[150], &rainpoint_hcs012arf, sizeof(r_device));
      memcpy(&cfg->devices[151], &regency_fan, sizeof(r_device));
      memcpy(&cfg->devices[152], &revolt_nc5462, sizeof(r_device));
      memcpy(&cfg->devices[153], &revolt_zx7717, sizeof(r_device));
      memcpy(&cfg->devices[154], &rftech, sizeof(r_device));
      memcpy(&cfg->devices[155], &rfxmeter, sizeof(r_device));
      memcpy(&cfg->devices[156], &risco_agility, sizeof(r_device));
      memcpy(&cfg->devices[157], &rosstech_dcu706, sizeof(r_device));
      memcpy(&cfg->devices[158], &rubicson, sizeof(r_device));
      memcpy(&cfg->devices[159], &rubicson_48659, sizeof(r_device));
      memcpy(&cfg->devices[160], &rubicson_pool_48942, sizeof(r_device));
      memcpy(&cfg->devices[161], &s3318p, sizeof(r_device));
      memcpy(&cfg->devices[162], &sainlogic_sa8, sizeof(r_device));
      memcpy(&cfg->devices[163], &schou_72543_rain, sizeof(r_device));
      memcpy(&cfg->devices[164], &schraeder, sizeof(r_device));
      memcpy(&cfg->devices[165], &schrader_EG53MA4, sizeof(r_device));
      memcpy(&cfg->devices[166], &schrader_SMD3MA4, sizeof(r_device));
      memcpy(&cfg->devices[167], &schrader_NIS315G3, sizeof(r_device));
      memcpy(&cfg->devices[168], &schrader_MRXBC5A4, sizeof(r_device));
      memcpy(&cfg->devices[169], &scmplus, sizeof(r_device));
      memcpy(&cfg->devices[170], &secplus_v1, sizeof(r_device));
      memcpy(&cfg->devices[171], &shenzhen_wale_wl_th6r, sizeof(r_device));
      memcpy(&cfg->devices[172], &siemens_5wy72xx_car_remote, sizeof(r_device));
      memcpy(&cfg->devices[173], &silvercrest, sizeof(r_device));
      memcpy(&cfg->devices[174], &ss_sensor, sizeof(r_device));
      memcpy(&cfg->devices[175], &skylink_motion, sizeof(r_device));
      memcpy(&cfg->devices[176], &smoke_gs558, sizeof(r_device));
      memcpy(&cfg->devices[177], &solight_te44, sizeof(r_device));
      memcpy(&cfg->devices[178], &somfy_rts, sizeof(r_device));
      memcpy(&cfg->devices[179], &springfield, sizeof(r_device));
      memcpy(&cfg->devices[180], &telldus_ft0385r, sizeof(r_device));
      memcpy(&cfg->devices[181], &tfa_30_3221, sizeof(r_device));
      memcpy(&cfg->devices[182], &tfa_drop_303233, sizeof(r_device));
      memcpy(&cfg->devices[183], &tfa_pool_thermometer, sizeof(r_device));
      memcpy(&cfg->devices[184], &tfa_twin_plus_303049, sizeof(r_device));
      memcpy(&cfg->devices[185], &thermopro_tp11, sizeof(r_device));
      memcpy(&cfg->devices[186], &thermopro_tp12, sizeof(r_device));
      memcpy(&cfg->devices[187], &thermopro_tx2, sizeof(r_device));
      memcpy(&cfg->devices[188], &thermopro_tx2c, sizeof(r_device));
      memcpy(&cfg->devices[189], &thermor, sizeof(r_device));
      memcpy(&cfg->devices[190], &thermor_a6n_132tx, sizeof(r_device));
      memcpy(&cfg->devices[191], &tpms_eezrv, sizeof(r_device));
      memcpy(&cfg->devices[192], &tmps_gear_hive, sizeof(r_device));
      memcpy(&cfg->devices[193], &tpms_gm, sizeof(r_device));
      memcpy(&cfg->devices[194], &tpms_imars_t240, sizeof(r_device));
      memcpy(&cfg->devices[195], &tpms_jansite_ty468, sizeof(r_device));
      memcpy(&cfg->devices[196], &tpms_schrader_motorcycle, sizeof(r_device));
      memcpy(&cfg->devices[197], &tpms_smartire, sizeof(r_device));
      memcpy(&cfg->devices[198], &tpms_trw_ook, sizeof(r_device));
      memcpy(&cfg->devices[199], &tpms_tyreguard400, sizeof(r_device));
      memcpy(&cfg->devices[200], &tr_502msv, sizeof(r_device));
      memcpy(&cfg->devices[201], &ts_ft002, sizeof(r_device));
      memcpy(&cfg->devices[202], &ttx201, sizeof(r_device));
      memcpy(&cfg->devices[203], &twogig_key2e, sizeof(r_device));
      memcpy(&cfg->devices[204], &universalfanctrl, sizeof(r_device));
      memcpy(&cfg->devices[205], &vaillant_vrt340f, sizeof(r_device));
      memcpy(&cfg->devices[206], &vauno_en8822c, sizeof(r_device));
      memcpy(&cfg->devices[207], &visonic_powercode, sizeof(r_device));
      memcpy(&cfg->devices[208], &vivint, sizeof(r_device));
      memcpy(&cfg->devices[209], &wallarge_cltx001, sizeof(r_device));
      memcpy(&cfg->devices[210], &watchman_plus, sizeof(r_device));
      memcpy(&cfg->devices[211], &watts_thermostat, sizeof(r_device));
      memcpy(&cfg->devices[212], &watts_wfht_rf, sizeof(r_device));
      memcpy(&cfg->devices[213], &waveman, sizeof(r_device));
      memcpy(&cfg->devices[214], &wec2103, sizeof(r_device));
      memcpy(&cfg->devices[215], &wg_pb12v1, sizeof(r_device));
      memcpy(&cfg->devices[216], &ws2032, sizeof(r_device));
      memcpy(&cfg->devices[217], &wssensor, sizeof(r_device));
      memcpy(&cfg->devices[218], &wt1024, sizeof(r_device));
      memcpy(&cfg->devices[219], &wt450, sizeof(r_device));
      memcpy(&cfg->devices[220], &X10_RF, sizeof(r_device));
      memcpy(&cfg->devices[221], &x10_sec, sizeof(r_device));
      memcpy(&cfg->devices[222], &yale_hsa, sizeof(r_device));
    } else {
      memcpy(&cfg->devices[0], &alps_fwb1u545_car_remote, sizeof(r_device));
      memcpy(&cfg->devices[1], &ambientweather_wh31e, sizeof(r_device));
      memcpy(&cfg->devices[2], &ant_antplus, sizeof(r_device));
      memcpy(&cfg->devices[3], &apator_metra_eitn30, sizeof(r_device));
      memcpy(&cfg->devices[4], &apator_metra_erm30, sizeof(r_device));
      memcpy(&cfg->devices[5], &arad_ms_meter, sizeof(r_device));
      memcpy(&cfg->devices[6], &archos_tbh, sizeof(r_device));
      memcpy(&cfg->devices[7], &arexx_ml, sizeof(r_device));
      memcpy(&cfg->devices[8], &orion_endpoint, sizeof(r_device));
      memcpy(&cfg->devices[9], &orion_endpoint_2020, sizeof(r_device));
      memcpy(&cfg->devices[10], &badger_orion, sizeof(r_device));
      memcpy(&cfg->devices[11], &bresser_5in1, sizeof(r_device));
      memcpy(&cfg->devices[12], &bresser_6in1, sizeof(r_device));
      memcpy(&cfg->devices[13], &bresser_7in1, sizeof(r_device));
      memcpy(&cfg->devices[14], &bresser_garden, sizeof(r_device));
      memcpy(&cfg->devices[15], &bresser_leakage, sizeof(r_device));
      memcpy(&cfg->devices[16], &bresser_lightning, sizeof(r_device));
      memcpy(&cfg->devices[17], &cavius, sizeof(r_device));
      memcpy(&cfg->devices[18], &cavius_door, sizeof(r_device));
      memcpy(&cfg->devices[19], &ced7000, sizeof(r_device));
      memcpy(&cfg->devices[20], &chamberlain_cwpirc, sizeof(r_device));
      memcpy(&cfg->devices[21], &continental_car_remote, sizeof(r_device));
      memcpy(&cfg->devices[22], &honda_keyfob, sizeof(r_device));
      memcpy(&cfg->devices[23], &ctt_life_power_hybrid, sizeof(r_device));
      memcpy(&cfg->devices[24], &current_cost, sizeof(r_device));
      memcpy(&cfg->devices[25], &danfoss_CFR, sizeof(r_device));
      memcpy(&cfg->devices[26], &deltadore_x3d, sizeof(r_device));
      memcpy(&cfg->devices[27], &directv, sizeof(r_device));
      memcpy(&cfg->devices[28], &eberle_instat868r1, sizeof(r_device));
      memcpy(&cfg->devices[29], &ec3k, sizeof(r_device));
      memcpy(&cfg->devices[30], &ecodhome, sizeof(r_device));
      memcpy(&cfg->devices[31], &ecoeye, sizeof(r_device));
      memcpy(&cfg->devices[32], &efergy_e2_classic, sizeof(r_device));
      memcpy(&cfg->devices[33], &efergy_optical, sizeof(r_device));
      memcpy(&cfg->devices[34], &elero, sizeof(r_device));
      memcpy(&cfg->devices[35], &elsner_solexa, sizeof(r_device));
      memcpy(&cfg->devices[36], &elster_power_meter, sizeof(r_device));
      memcpy(&cfg->devices[37], &elster_power_meter2, sizeof(r_device));
      memcpy(&cfg->devices[38], &emax, sizeof(r_device));
      memcpy(&cfg->devices[39], &emontx, sizeof(r_device));
      memcpy(&cfg->devices[40], &en2058, sizeof(r_device));
      memcpy(&cfg->devices[41], &esic_emt7110, sizeof(r_device));
      memcpy(&cfg->devices[42], &fineoffset_WH25, sizeof(r_device));
      memcpy(&cfg->devices[43], &fineoffset_WH51, sizeof(r_device));
      memcpy(&cfg->devices[44], &tfa_303151, sizeof(r_device));
      memcpy(&cfg->devices[45], &fineoffset_wh1080_fsk, sizeof(r_device));
      memcpy(&cfg->devices[46], &fineoffset_wh31l, sizeof(r_device));
      memcpy(&cfg->devices[47], &fineoffset_wh43, sizeof(r_device));
      memcpy(&cfg->devices[48], &fineoffset_wh45, sizeof(r_device));
      memcpy(&cfg->devices[49], &fineoffset_wh46, sizeof(r_device));
      memcpy(&cfg->devices[50], &fineoffset_wh52, sizeof(r_device));
      memcpy(&cfg->devices[51], &fineoffset_wh55, sizeof(r_device));
      memcpy(&cfg->devices[52], &fineoffset_wn34, sizeof(r_device));
      memcpy(&cfg->devices[53], &fineoffset_ws80, sizeof(r_device));
      memcpy(&cfg->devices[54], &fineoffset_ws85, sizeof(r_device));
      memcpy(&cfg->devices[55], &fineoffset_ws90, sizeof(r_device));
      memcpy(&cfg->devices[56], &flowis, sizeof(r_device));
      memcpy(&cfg->devices[57], &fsl_scoreboard, sizeof(r_device));
      memcpy(&cfg->devices[58], &ge_coloreffects, sizeof(r_device));
      memcpy(&cfg->devices[59], &geo_minim, sizeof(r_device));
      memcpy(&cfg->devices[60], &govee_h5059, sizeof(r_device));
      memcpy(&cfg->devices[61], &govee_h5112, sizeof(r_device));
      memcpy(&cfg->devices[62], &govee_h5310, sizeof(r_device));
      memcpy(&cfg->devices[63], &gridstream96, sizeof(r_device));
      memcpy(&cfg->devices[64], &gridstream192, sizeof(r_device));
      memcpy(&cfg->devices[65], &gridstream384, sizeof(r_device));
      memcpy(&cfg->devices[66], &hanwell_ml4000, sizeof(r_device));
      memcpy(&cfg->devices[67], &hcs200_fsk, sizeof(r_device));
      memcpy(&cfg->devices[68], &holman_ws5029pcm, sizeof(r_device));
      memcpy(&cfg->devices[69], &holman_ws5029pwm, sizeof(r_device));
      memcpy(&cfg->devices[70], &hondaremote, sizeof(r_device));
      memcpy(&cfg->devices[71], &honeywell_cm921, sizeof(r_device));
      memcpy(&cfg->devices[72], &honeywell_wdb_fsk, sizeof(r_device));
      memcpy(&cfg->devices[73], &ikea_sparsnas, sizeof(r_device));
      memcpy(&cfg->devices[74], &inkbird_ith20r, sizeof(r_device));
      memcpy(&cfg->devices[75], &insteon, sizeof(r_device));
      memcpy(&cfg->devices[76], &lacrosse_breezepro, sizeof(r_device));
      memcpy(&cfg->devices[77], &lacrosse_r1, sizeof(r_device));
      memcpy(&cfg->devices[78], &lacrosse_th3, sizeof(r_device));
      memcpy(&cfg->devices[79], &lacrosse_tx22uit, sizeof(r_device));
      memcpy(&cfg->devices[80], &lacrosse_tx31u, sizeof(r_device));
      memcpy(&cfg->devices[81], &lacrosse_tx34, sizeof(r_device));
      memcpy(&cfg->devices[82], &lacrosse_tx29, sizeof(r_device));
      memcpy(&cfg->devices[83], &lacrosse_tx35, sizeof(r_device));
      memcpy(&cfg->devices[84], &lacrosse_wr1, sizeof(r_device));
      memcpy(&cfg->devices[85], &lacrosse_ws6868_tx232th, sizeof(r_device));
      memcpy(&cfg->devices[86], &lacrosse_ws6868_tx231rw, sizeof(r_device));
      memcpy(&cfg->devices[87], &m_bus_mode_c_t, sizeof(r_device));
      memcpy(&cfg->devices[88], &m_bus_mode_c_t_downlink, sizeof(r_device));
      memcpy(&cfg->devices[89], &m_bus_mode_s, sizeof(r_device));
      memcpy(&cfg->devices[90], &m_bus_mode_r, sizeof(r_device));
      memcpy(&cfg->devices[91], &m_bus_mode_f, sizeof(r_device));
      memcpy(&cfg->devices[92], &radian, sizeof(r_device));
      memcpy(&cfg->devices[93], &marlec_solar, sizeof(r_device));
      memcpy(&cfg->devices[94], &maverick_xr30, sizeof(r_device));
      memcpy(&cfg->devices[95], &maverick_xr50, sizeof(r_device));
      memcpy(&cfg->devices[96], &mcpower_kinetic, sizeof(r_device));
      memcpy(&cfg->devices[97], &mueller_hotrod, sizeof(r_device));
      memcpy(&cfg->devices[98], &netatmo_thw, sizeof(r_device));
      memcpy(&cfg->devices[99], &nidec_car_remote, sizeof(r_device));
      memcpy(&cfg->devices[100], &oil_smart, sizeof(r_device));
      memcpy(&cfg->devices[101], &oil_standard, sizeof(r_device));
      memcpy(&cfg->devices[102], &oil_watchman, sizeof(r_device));
      memcpy(&cfg->devices[103], &oil_watchman_advanced, sizeof(r_device));
      memcpy(&cfg->devices[104], &opel_mokka, sizeof(r_device));
      memcpy(&cfg->devices[105], &oregon_scientific_wmr500, sizeof(r_device));
      memcpy(&cfg->devices[106], &quinetic, sizeof(r_device));
      memcpy(&cfg->devices[107], &rfm69_lowpowerlab_moteino, sizeof(r_device));
      memcpy(&cfg->devices[108], &rojaflex, sizeof(r_device));
      memcpy(&cfg->devices[109], &sharp_spc775, sizeof(r_device));
      memcpy(&cfg->devices[110], &silver_spring_mesh, sizeof(r_device));
      memcpy(&cfg->devices[111], &simplisafe_gen3, sizeof(r_device));
      memcpy(&cfg->devices[112], &somfy_iohc, sizeof(r_device));
      memcpy(&cfg->devices[113], &srsmith_pool_srs_2c_tx, sizeof(r_device));
      memcpy(&cfg->devices[114], &steelmate, sizeof(r_device));
      memcpy(&cfg->devices[115], &tfa_14_1504_v2, sizeof(r_device));
      memcpy(&cfg->devices[116], &tfa_303196, sizeof(r_device));
      memcpy(&cfg->devices[117], &tfa_30_390x, sizeof(r_device));
      memcpy(&cfg->devices[118], &tfa_marbella, sizeof(r_device));
      memcpy(&cfg->devices[119], &thermopro_tp211b, sizeof(r_device));
      memcpy(&cfg->devices[120], &thermopro_tp28b, sizeof(r_device));
      memcpy(&cfg->devices[121], &thermopro_tp828b, sizeof(r_device));
      memcpy(&cfg->devices[122], &thermopro_tp829b, sizeof(r_device));
      memcpy(&cfg->devices[123], &thermopro_tp86xb, sizeof(r_device));
      memcpy(&cfg->devices[124], &thermopro_tx7b, sizeof(r_device));
      memcpy(&cfg->devices[125], &tpms_abarth124, sizeof(r_device));
      memcpy(&cfg->devices[126], &tpms_airpuxem, sizeof(r_device));
      memcpy(&cfg->devices[127], &tpms_ave, sizeof(r_device));
      memcpy(&cfg->devices[128], &tpms_bmw, sizeof(r_device));
      memcpy(&cfg->devices[129], &tpms_bmwg3, sizeof(r_device));
      memcpy(&cfg->devices[130], &tpms_citroen, sizeof(r_device));
      memcpy(&cfg->devices[131], &tpms_elantra2012, sizeof(r_device));
      memcpy(&cfg->devices[132], &tpms_ford, sizeof(r_device));
      memcpy(&cfg->devices[133], &tpms_honda, sizeof(r_device));
      memcpy(&cfg->devices[134], &tpms_hyundai_vdo, sizeof(r_device));
      memcpy(&cfg->devices[135], &tpms_jansite, sizeof(r_device));
      memcpy(&cfg->devices[136], &tpms_jansite_solar, sizeof(r_device));
      memcpy(&cfg->devices[137], &tpms_jansite_ty588, sizeof(r_device));
      memcpy(&cfg->devices[138], &tpms_jeep, sizeof(r_device));
      memcpy(&cfg->devices[139], &tpms_kia, sizeof(r_device));
      memcpy(&cfg->devices[140], &tpms_mercedes_benz, sizeof(r_device));
      memcpy(&cfg->devices[141], &tpms_nissan, sizeof(r_device));
      memcpy(&cfg->devices[142], &tpms_pmv107j, sizeof(r_device));
      memcpy(&cfg->devices[143], &tpms_porsche, sizeof(r_device));
      memcpy(&cfg->devices[144], &tpms_renault, sizeof(r_device));
      memcpy(&cfg->devices[145], &tpms_renault_0435r, sizeof(r_device));
      memcpy(&cfg->devices[146], &tpms_sefis_m3, sizeof(r_device));
      memcpy(&cfg->devices[147], &tpms_toyota, sizeof(r_device));
      memcpy(&cfg->devices[148], &tpms_truck, sizeof(r_device));
      memcpy(&cfg->devices[149], &tpms_trw_fsk, sizeof(r_device));
      memcpy(&cfg->devices[150], &typhur_sync_gold, sizeof(r_device));
      memcpy(&cfg->devices[151], &vevor_7in1, sizeof(r_device));
      memcpy(&cfg->devices[152], &watts_vision, sizeof(r_device));
    }

    // end of fragment

#else
#  define DECL(name) &name,
    static r_device const* const myDevices[] = {DEVICES};
#  undef DECL
    for (size_t i = 0; i < NUMOFDEVICES; ++i) {
      memcpy(&cfg->devices[i], myDevices[i], sizeof(r_device));
    }
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
      cfg->devices[i].protocol_num = i;
#ifdef MEMORY_DEBUG
      logprintfLn(LOG_DEBUG, "Pre register_protocol %d %s, heap %d", i,
                  cfg->devices[i].name, ESP.getFreeHeap());
#endif
#ifdef RESOURCE_DEBUG
      int preStack = uxTaskGetStackHighWaterMark(NULL);
      int preHeap = ESP.getFreeHeap();
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
#ifdef RESOURCE_DEBUG
      int deltaStack = preStack - uxTaskGetStackHighWaterMark(NULL);
      int deltaHeap = preHeap - ESP.getFreeHeap();
      if (deltaStack || (deltaHeap > 200)) {
        logprintfLn(LOG_DEBUG, "Process rtl_433_DecoderTask resource hit %s, deltaStack: %d, stack: %u, deltaHeap: %d, heap: %d", cfg->devices[i].name,
                    deltaStack, uxTaskGetStackHighWaterMark(NULL), deltaHeap, ESP.getFreeHeap());
      }
#endif
    }
  }

#ifdef MEMORY_DEBUG
  logprintfLn(LOG_DEBUG, "Pre xQueueCreate heap %d", ESP.getFreeHeap());
#endif
  if (!rtl_433_Queue) {
    rtl_433_Queue = xQueueCreate(5, sizeof(pulse_data_t*));
    if (!rtl_433_Queue) {
      return false;
    }
  }

#ifdef MEMORY_DEBUG
  logprintfLn(LOG_DEBUG, "Pre xTaskCreatePinnedToCore heap %d",
              ESP.getFreeHeap());
#endif
#ifdef RESOURCE_DEBUG
  logprintfLn(LOG_INFO, "rtl_433_Decoder_Stack %d", rtl_433_Decoder_Stack);
#endif

  if (!rtl_433_DecoderHandle) {
    BaseType_t taskCreated = xTaskCreatePinnedToCore(
        rtl_433_DecoderTask, /* Function to implement the task */
        "rtl_433_DecoderTask", /* Name of the task */
        rtl_433_Decoder_Stack, /* Stack size in bytes */
        NULL, /* Task input parameter */
        rtl_433_Decoder_Priority, /* Priority of the task (set lower than core task) */
        &rtl_433_DecoderHandle, /* Task handle. */
        rtl_433_Decoder_Core); /* Core where the task should run */
    if (taskCreated != pdPASS) {
      vQueueDelete(rtl_433_Queue);
      rtl_433_Queue = nullptr;
      return false;
    }
  }
  return true;
}

void rtlShutdown() {
  if (rtl_433_DecoderHandle) {
    vTaskDelete(rtl_433_DecoderHandle);
    rtl_433_DecoderHandle = nullptr;
  }
  if (rtl_433_Queue) {
    pulse_data_t* pending = nullptr;
    while (xQueueReceive(rtl_433_Queue, &pending, 0) == pdTRUE) {
      free(pending);
    }
    vQueueDelete(rtl_433_Queue);
    rtl_433_Queue = nullptr;
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

void _setRawPulsesCallback(rtl_433_raw_pulse_cb callback) {
  rawPulsesCallback.store(callback, std::memory_order_release);
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
    if (xQueueReceive(rtl_433_Queue, &rtl_pulses, portMAX_DELAY) != pdTRUE ||
        !rtl_pulses) {
      continue;
    }
    // logprintfLn(LOG_DEBUG, "rtl_433_DecoderTask signal received");
#ifdef MEMORY_DEBUG
    uint32_t signalProcessingStart = micros();
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
    rtl_433_ESP::decoderSignals++;
    if (events > 0) {
      rtl_433_ESP::decodedMessages += events;
    } else {
      rtl_433_ESP::zeroDecodedSignals++;
    }
    rtl_433_raw_pulse_cb rawCallback =
        rawPulsesCallback.load(std::memory_order_acquire);
    if (rawCallback != nullptr) {
      rawCallback(rtl_pulses->pulse, rtl_pulses->gap, rtl_pulses->num_pulses,
                  rtl_pulses->signalDuration, rtl_pulses->signalRssi);
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
                "model", "",      DATA_STRING,  "undecoded signal",
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
      if (cfg->callback && cfg->messageBuffer && cfg->bufferSize > 0) {
        data_print_jsons(data, cfg->messageBuffer, cfg->bufferSize);
        (cfg->callback)(cfg->messageBuffer);
      }
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
  if (!rtl_433_Queue ||
      xQueueSend(rtl_433_Queue, &rtl_pulses, 0) != pdTRUE) {
    logprintfLn(LOG_ERR, "ERROR: rtl_433_Queue full, discarding signal");
    rtl_433_ESP::droppedDecoderQueue++;
    free(rtl_pulses);
  } else {
    // logprintfLn(LOG_DEBUG, "processSignal() signal placed on rtl_433_Queue");
  }
}
