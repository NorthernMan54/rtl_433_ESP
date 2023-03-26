#! /bin/sh

export OOK_MODULATION="OOK_PULSE_PWM|OOK_PULSE_PPM|OOK_PULSE_MANCHESTER_ZEROBIT|OOK_MC_ZEROBIT|OOK_PULSE_DMC|OOK_PULSE_NRZS|OOK_PULSE_PCM|OOK_PULSE_PIWM_DC|OOK_PULSE_PWM_OSV1|OOK_PULSE_RZ"

export FSK_MODULATION="FSK_PULSE_MANCHESTER_ZEROBIT|FSK_PULSE_PCM|FSK_PULSE_PWM"

rm copy.list devices.list decoder.fragment

( cd .. ; rm -rf rtl_433 ; git clone https://github.com/merbanan/rtl_433 )
( cd ../rtl_433/src/devices/ ; egrep "\.name|\.modulation|\.decode_fn|^r_device " *.c ) |\
awk -f device.awk | awk -F : '{ print $1 }' | sort | uniq > copy.list

echo "Clone from rtl_433 complete"

# add flex decoder to the list

echo "flex.c" >> copy.list

# Populate src/rtl_433/device

( cd ../src/rtl_433/devices ; rm * )

for i in `cat copy.list`
do
cp ../rtl_433/src/devices/$i ../src/rtl_433/devices
done

echo "Device decoders updated"

for i in `ls ../contrib/`
do
cp ../contrib/$i ../src/rtl_433/devices
done

echo "Contrib decoders updated"

# remove non-functional device decoders

# device decoders that place a bitbuffer_t on the stack is causing an overflow or large memory usage
# ie bitbuffer_t databits = {0};
# ie blueline.c - approx 64000 bytes of memory used

for i in newkaku.c nexa.c proove.c cavius.c current_cost.c ge_coloreffects.c insteon.c m_bus.c oil_standard.c oil_watchman.c tpms_abarth124.c tpms_citroen.c tpms_elantra2012.c tpms_ford.c tpms_jansite.c tpms_jansite_solar.c tpms_pmv107j.c tpms_renault.c tpms_toyota.c blueline.c sharp_spc775.c secplus_v2.c nexus.c abmt.c cmr113.c jasco.c maverick_et73x.c neptune_r900.c proflame2.c oregon_scientific.c rainpoint.c somfy_rts.c schraeder.c celsia_czc1.c
do
rm ../src/rtl_433/devices/$i
done

echo "Problematic decoders removed"

# create include/rtl_433_devices.h

( cd ../src/rtl_433/devices ; egrep "\.name|\.modulation|\.decode_fn|^r_device " *.c ) > devices.list

OOK_COUNT=`cat devices.list | awk -f device.awk | egrep ${OOK_MODULATION} | awk -F\" '{ print $3 }' | awk -F, '{ print $3 }' | wc | awk '{ print $1 }'`
FSK_COUNT=`cat devices.list | awk -f device.awk | egrep ${FSK_MODULATION} |awk -F\" '{ print $3 }' | awk -F, '{ print $3 }' | wc | awk '{ print $1 }'`

echo $OOK_COUNT "OOK Decoders are copied"
echo $FSK_COUNT "OOK Decoders are copied"

cat devices.list | awk -f device.awk | awk -F\" '{ print $3 }' | \
awk -F, '{ print $3 }' | awk '{ print "  DECL("$1") \\" }' > rtl_433_devices.fragment

echo "  /* Add new decoders here. */" >> rtl_433_devices.fragment

echo "#define NUMOF_OOK_DEVICES ${OOK_COUNT}" >> rtl_433_devices.fragment
echo "#define NUMOF_FSK_DEVICES ${FSK_COUNT}" >> rtl_433_devices.fragment

cat rtl_433_devices.pre rtl_433_devices.fragment rtl_433_devices.post > ../include/rtl_433_devices.h

echo "rtl_433_devices.h created"

# create src/decoder.cpp fragment

echo "  // This is a generated fragment from tools/update_rtl_433_devices.sh" > decoder.fragment

echo "" >> decoder.fragment

echo "if (rtl_433_ESP::ookModulation) {" >> decoder.fragment

cat devices.list | awk -f device.awk | egrep ${OOK_MODULATION} | awk -F\" '{ print $3 }' | \
awk -F, '{ print $3 }' | awk '{ print "  memcpy(&cfg->devices["NR-1"], &"$1", sizeof(r_device));" }' >> decoder.fragment

echo "} else {" >> decoder.fragment

cat devices.list | awk -f device.awk | egrep ${FSK_MODULATION} | awk -F\" '{ print $3 }' | \
awk -F, '{ print $3 }' | awk '{ print "  memcpy(&cfg->devices["NR-1"], &"$1", sizeof(r_device));" }' >> decoder.fragment

echo "}" >> decoder.fragment
echo "" >> decoder.fragment
echo "  // end of fragement" >> decoder.fragment

echo
echo "Please update src/decoder.cpp with decoder.fragment"

# copy src files from rtl_433/src to src/rtl_433

for i in abuf.c bitbuffer.c decoder_util.c list.c r_util.c util.c optparse.c compat_time.c
do
  cp ../rtl_433/src/$i ../src/rtl_433
done

# copy include files from rtl_433/include to include

for i in abuf.h am_analyze.h baseband.h bitbuffer.h compat_time.h decoder.h decoder_util.h fatal.h fileformat.h list.h optparse.h pulse_slicer.h r_api.h r_util.h samp_grab.h term_ctl.h util.h compat_time.h
do
  echo "Copying rtl_433/include "$i" to include"
  cp ../rtl_433/include/$i ../include
done

echo "These source files need updating"

echo "data.c - Defined out unneeded functions ( #ifndef ESP32 )"
echo "pulse_demod.c - Move 'bitbuffer_t bits' to class level"
echo "r_api.c - Significant tuning and tweaking applied"

echo
echo "cp ../../rtl_433/src/data.c ."
echo "cp ../../rtl_433/src/pulse_demod.c ."
echo "cp ../../rtl_433/src/r_api.c ."
echo
echo "These include files need updating"

echo "data.h - Added '#define _POSIX_HOST_NAME_MAX 128'"
echo "log.h - Not from rtl_433.h"
echo "pulse_detect.h - Adjusted structures to reduce size"
echo "r_device.h - Adjusted structures to reduce size"
echo "r_private.h - Adjusted structures to reduce size"
echo "rtl_433.h - Adjusted structures to reduce size"

echo

echo "cp ../rtl_433/include/pulse_detect.h ."
echo "cp ../rtl_433/include/data.h ."
echo "cp ../rtl_433/include/r_device.h ."
echo "cp ../rtl_433/include/r_private.h ."
echo "cp ../rtl_433/include/rtl_433.h ."
