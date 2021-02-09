#include "rtl_bridge.h"

//#include <stdio.h>
#include <stdlib.h>
//#include <stdint.h>
//#include <stdbool.h>
#include <string.h>
//#include <math.h>
#include "fatal.h"

// static r_cfg_t g_cfg;

void rtlSetup(r_cfg_t *cfg)
{
  // #ifndef _WIN32
  //    struct sigaction sigact;
  // #endif
  //    FILE *in_file;
  //    int r = 0;
  unsigned i;
  // struct dm_state *demod;
  // r_cfg_t *cfg = &g_cfg;

  //    print_version(); // always print the version info
  logprintf(LOG_INFO, "sizeof(*cfg->demod) %d", sizeof(*cfg->demod));
  r_init_cfg(cfg);

  //   setbuf(stdout, NULL);
  //   setbuf(stderr, NULL);

  // demod = cfg->demod;

  // demod->pulse_detect = pulse_detect_create();

  /* initialize tables */
  // baseband_init();

  // r_devices[1] = &(struct r_device)prologue;

  /*
  r_device r_devices[] = {
#define DECL(name) name,
      DEVICES
#undef DECL
  };
*/

  cfg->num_r_devices = NUMOFDEVICES;
  cfg->devices = calloc(cfg->num_r_devices, sizeof(r_device));
  if (!cfg->devices)
    FATAL_CALLOC("cfg->devices");

// cat rtl_433_devices.h | grep DECL | cut -c10- | awk -F\) '{ print "memcpy(&cfg->devices["NR-1"], &"$1", sizeof(r_device));" }'

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
memcpy(&cfg->devices[50], &proove, sizeof(r_device));
memcpy(&cfg->devices[51], &bresser_3ch, sizeof(r_device));
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
memcpy(&cfg->devices[118], &bresser_5in1, sizeof(r_device));
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
memcpy(&cfg->devices[163], &secplus_v2, sizeof(r_device));
memcpy(&cfg->devices[164], &tfa_30_3221, sizeof(r_device));
memcpy(&cfg->devices[165], &lacrosse_breezepro, sizeof(r_device));
memcpy(&cfg->devices[166], &somfy_rts, sizeof(r_device));
memcpy(&cfg->devices[167], &schrader_SMD3MA4, sizeof(r_device));
memcpy(&cfg->devices[168], &nice_flor_s, sizeof(r_device));
memcpy(&cfg->devices[169], &lacrosse_wr1, sizeof(r_device));
memcpy(&cfg->devices[170], &lacrosse_th3, sizeof(r_device));
memcpy(&cfg->devices[171], &bresser_6in1, sizeof(r_device));
memcpy(&cfg->devices[172], &bresser_7in1, sizeof(r_device));
memcpy(&cfg->devices[173], &ecodhome, sizeof(r_device));
memcpy(&cfg->devices[174], &lacrosse_r1, sizeof(r_device));
memcpy(&cfg->devices[175], &blueline, sizeof(r_device));
memcpy(&cfg->devices[176], &burnhardbbq, sizeof(r_device));

  // r_device [177]{(struct r_device)silvercrest, (struct r_device)rubicson, (struct r_device)prologue, (struct r_device)waveman, (struct r_device)new_template, (struct r_device)elv_em1000, (struct r_device)elv_ws2000, (struct r_device)lacrossetx, (struct r_device)new_template, (struct r_device)acurite_rain_896, (struct r_device)acurite_th, (struct r_device)oregon_scientific, (struct r_device)mebus433, (struct r_device)intertechno, (struct r_device)newkaku, (struct r_device)alectov1, (struct r_device)cardin, (struct r_device)fineoffset_WH2, (struct r_device)nexus, (struct r_device)ambient_weather, (struct r_device)calibeur_RF104, (struct r_device)X10_RF, (struct r_device)dsc_security, (struct r_device)brennenstuhl_rcs_2044, (struct r_device)gt_wt_02, (struct r_device)danfoss_CFR, (struct r_device)new_template, (struct r_device)new_template, (struct r_device)chuango, (struct r_device)generic_remote, (struct r_device)tfa_twin_plus_303049, (struct r_device)fineoffset_wh1080, (struct r_device)wt450, (struct r_device)lacrossews, (struct r_device)esperanza_ews, (struct r_device)efergy_e2_classic, (struct r_device)kw9015b, (struct r_device)generic_temperature_sensor, (struct r_device)wg_pb12v1, (struct r_device)acurite_txr, (struct r_device)acurite_986, (struct r_device)hideki_ts04, (struct r_device)oil_watchman, (struct r_device)current_cost, (struct r_device)emontx, (struct r_device)ht680, (struct r_device)s3318p, (struct r_device)akhan_100F14, (struct r_device)quhwa, (struct r_device)oregon_scientific_v1, (struct r_device)proove, (struct r_device)bresser_3ch, (struct r_device)springfield, (struct r_device)oregon_scientific_sl109h, (struct r_device)acurite_606, (struct r_device)tfa_pool_thermometer, (struct r_device)kedsum, (struct r_device)blyss, (struct r_device)steelmate, (struct r_device)schraeder, (struct r_device)lightwave_rf, (struct r_device)elro_db286a, (struct r_device)efergy_optical, (struct r_device)hondaremote, (struct r_device)new_template, (struct r_device)new_template, (struct r_device)radiohead_ask, (struct r_device)kerui, (struct r_device)fineoffset_wh1050, (struct r_device)honeywell, (struct r_device)maverick_et73x, (struct r_device)rftech, (struct r_device)lacrosse_tx141x, (struct r_device)acurite_00275rm, (struct r_device)lacrosse_tx35, (struct r_device)lacrosse_tx29, (struct r_device)vaillant_vrt340f, (struct r_device)fineoffset_WH25, (struct r_device)fineoffset_WH0530, (struct r_device)ibis_beacon, (struct r_device)oil_standard, (struct r_device)tpms_citroen, (struct r_device)oil_standard_ask, (struct r_device)thermopro_tp11, (struct r_device)solight_te44, (struct r_device)smoke_gs558, (struct r_device)generic_motion, (struct r_device)tpms_toyota, (struct r_device)tpms_ford, (struct r_device)tpms_renault, (struct r_device)infactory, (struct r_device)ft004b, (struct r_device)fordremote, (struct r_device)philips_aj3650, (struct r_device)schrader_EG53MA4, (struct r_device)nexa, (struct r_device)thermopro_tp12, (struct r_device)ge_coloreffects, (struct r_device)x10_sec, (struct r_device)interlogix, (struct r_device)dish_remote_6_3, (struct r_device)ss_sensor, (struct r_device)sensible_living, (struct r_device)m_bus_mode_c_t, (struct r_device)m_bus_mode_s, (struct r_device)m_bus_mode_r, (struct r_device)m_bus_mode_f, (struct r_device)wssensor, (struct r_device)wt1024, (struct r_device)tpms_pmv107j, (struct r_device)ttx201, (struct r_device)ambientweather_tx8300, (struct r_device)ambientweather_wh31e, (struct r_device)maverick_et73, (struct r_device)honeywell_wdb, (struct r_device)honeywell_wdb_fsk, (struct r_device)esa_energy, (struct r_device)bt_rain, (struct r_device)bresser_5in1, (struct r_device)digitech_xc0324, (struct r_device)opus_xt300, (struct r_device)fs20, (struct r_device)tpms_jansite, (struct r_device)lacrosse_ws7000, (struct r_device)ts_ft002, (struct r_device)companion_wtr001, (struct r_device)ecowitt, (struct r_device)directv, (struct r_device)eurochron, (struct r_device)ikea_sparsnas, (struct r_device)hcs200, (struct r_device)tfa_303196, (struct r_device)rubicson_48659, (struct r_device)holman_ws5029pcm, (struct r_device)philips_aj7010, (struct r_device)esic_emt7110, (struct r_device)gt_tmbbq05, (struct r_device)gt_wt_03, (struct r_device)norgo, (struct r_device)tpms_elantra2012, (struct r_device)auriol_hg02832, (struct r_device)fineoffset_WH51, (struct r_device)holman_ws5029pwm, (struct r_device)archos_tbh, (struct r_device)ws2032, (struct r_device)auriol_afw2a1, (struct r_device)tfa_drop_303233, (struct r_device)dsc_security_ws4945, (struct r_device)ert_scm, (struct r_device)klimalogg, (struct r_device)visonic_powercode, (struct r_device)eurochron_efth800, (struct r_device)cotech_36_7959, (struct r_device)scmplus, (struct r_device)fineoffset_wh1080_fsk, (struct r_device)tpms_abarth124, (struct r_device)missil_ml0757, (struct r_device)sharp_spc775, (struct r_device)insteon, (struct r_device)ert_idm, (struct r_device)ert_netidm, (struct r_device)thermopro_tx2, (struct r_device)acurite_590tx, (struct r_device)secplus_v2, (struct r_device)tfa_30_3221, (struct r_device)lacrosse_breezepro, (struct r_device)somfy_rts, (struct r_device)schrader_SMD3MA4, (struct r_device)nice_flor_s, (struct r_device)lacrosse_wr1, (struct r_device)lacrosse_th3, (struct r_device)bresser_6in1, (struct r_device)bresser_7in1, (struct r_device)ecodhome, (struct r_device)lacrosse_r1, (struct r_device)blueline, (struct r_device)burnhardbbq}

  if (!cfg->devices)
    FATAL_CALLOC("cfg->devices");

  // logprintf(LOG_INFO, "Location of r_devices: %p", (void *)&r_devices);
  // logprintf(LOG_INFO, "ssizeof(r_devices) %d", sizeof(r_devices));
  logprintf(LOG_INFO, "Location of cfg: %p", (void *)&cfg);
  logprintf(LOG_INFO, "ssizeof(cfg->devices): %d", sizeof(cfg->devices));
  logprintf(LOG_INFO, "Location of cfg->devices: %p", (void *)&cfg->devices);

  for (i = 0; i < cfg->num_r_devices; i++)
  {
    // r_devices[i].protocol_num = i + 1;
    cfg->devices[i].protocol_num = i + 1;
    // cfg->devices[i].name = r_devices[i].name;
    // memcpy(cfg->devices[i], &r_devices[i], sizeof(r_devices));
    // cfg->devices[i] = r_devices[i];

    // cfg->devices[i].protocol_num = i + 1;
  }

  // bitbuffer_t *bits = (bitbuffer_t *)calloc(1, sizeof(bitbuffer_t));

  // r_device *dev = (r_device *)malloc(sizeof(r_devices));

  // logprintf(LOG_INFO, "ssizeof(r_devices) %d", &r_devices);
  // logprintf(LOG_INFO, r_devices);
  // memcpy(&dev, &r_devices, sizeof(r_devices));
  // cfg->devices = dev;
#ifdef DEBUG
  cfg->verbosity = 1;
#endif

  register_all_protocols(cfg, 0);
  // return &cfg;
}