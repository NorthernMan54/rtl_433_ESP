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

  memcpy(&cfg->devices[0], &prologue, sizeof(r_device));
  memcpy(&cfg->devices[1], &acurite_986, sizeof(r_device));

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

  register_all_protocols(cfg, 0);
  // return &cfg;
}