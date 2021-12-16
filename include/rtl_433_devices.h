/** @file
    This is a generated file from tools/update_rtl_433_devices.sh
*/

#ifndef INCLUDE_RTL_433_DEVICES_H_
#define INCLUDE_RTL_433_DEVICES_H_

#include "r_device.h"

#ifndef MY_DEVICES
#define DEVICES                    \
  DECL(acurite_rain_896) \
  DECL(acurite_th) \
  DECL(acurite_txr) \
  DECL(acurite_986) \
  DECL(acurite_606) \
  DECL(acurite_00275rm) \
  DECL(acurite_590tx) \
  DECL(acurite_01185m) \
  DECL(akhan_100F14) \
  DECL(alectov1) \
  DECL(ambientweather_tx8300) \
  DECL(auriol_4ld5661) \
  DECL(auriol_aft77b2) \
  DECL(auriol_afw2a1) \
  DECL(auriol_ahfl) \
  DECL(auriol_hg02832) \
  DECL(blueline) \
  DECL(blyss) \
  DECL(brennenstuhl_rcs_2044) \
  DECL(bresser_3ch) \
  DECL(bt_rain) \
  DECL(burnhardbbq) \
  DECL(calibeur_RF104) \
  DECL(cardin) \
  DECL(chuango) \
  DECL(companion_wtr001) \
  DECL(digitech_xc0324) \
  DECL(dish_remote_6_3) \
  DECL(ecowitt) \
  DECL(eurochron_efth800) \
  DECL(elro_db286a) \
  DECL(elv_em1000) \
  DECL(elv_ws2000) \
  DECL(esperanza_ews) \
  DECL(eurochron) \
  DECL(fineoffset_WH2) \
  DECL(fineoffset_WH0530) \
  DECL(fineoffset_wh1050) \
  DECL(fineoffset_wh1080) \
  DECL(fs20) \
  DECL(ft004b) \
  DECL(generic_motion) \
  DECL(generic_remote) \
  DECL(generic_temperature_sensor) \
  DECL(govee) \
  DECL(gt_tmbbq05) \
  DECL(gt_wt_02) \
  DECL(gt_wt_03) \
  DECL(hcs200) \
  DECL(honeywell_wdb) \
  DECL(ht680) \
  DECL(infactory) \
  DECL(kw9015b) \
  DECL(interlogix) \
  DECL(intertechno) \
  DECL(kedsum) \
  DECL(kerui) \
  DECL(lacrossetx) \
  DECL(lacrosse_tx141x) \
  DECL(lacrosse_ws7000) \
  DECL(lacrossews) \
  DECL(lightwave_rf) \
  DECL(markisol) \
  DECL(maverick_et73) \
  DECL(mebus433) \
  DECL(missil_ml0757) \
  DECL(new_template) \
  DECL(nexus) \
  DECL(nice_flor_s) \
  DECL(opus_xt300) \
  DECL(oregon_scientific_sl109h) \
  DECL(oregon_scientific_v1) \
  DECL(philips_aj3650) \
  DECL(philips_aj7010) \
  DECL(prologue) \
  DECL(quhwa) \
  DECL(rftech) \
  DECL(rubicson) \
  DECL(rubicson_48659) \
  DECL(s3318p) \
  DECL(silvercrest) \
  DECL(skylink_motion) \
  DECL(smoke_gs558) \
  DECL(solight_te44) \
  DECL(springfield) \
  DECL(tfa_30_3221) \
  DECL(tfa_drop_303233) \
  DECL(tfa_pool_thermometer) \
  DECL(tfa_twin_plus_303049) \
  DECL(thermopro_tp11) \
  DECL(thermopro_tp12) \
  DECL(thermopro_tx2) \
  DECL(ts_ft002) \
  DECL(visonic_powercode) \
  DECL(waveman) \
  DECL(wg_pb12v1) \
  DECL(ws2032) \
  DECL(wssensor) \
  DECL(wt1024) \
  DECL(X10_RF) \
  DECL(x10_sec) \
  /* Add new decoders here. */
#define NUMOFDEVICES 101
  /* Add new decoders here. */
#else
/**
 * Subset of devices that I have access to and have tested with
 */
#define DEVICES        \
  DECL(acurite_986)    \
  DECL(skylink_motion) \
  DECL(prologue)       \
  DECL(philips_aj3650) \
  DECL(fineoffset_WH51) \
  /* Add new personal decoders here. */
  #define NUMOFDEVICES 5
#endif

#define DECL(name) extern r_device name;
DEVICES
#undef DECL

#endif /* INCLUDE_RTL_433_DEVICES_H_ */
