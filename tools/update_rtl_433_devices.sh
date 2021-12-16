#! /bin/sh

export MODULATION="OOK_PULSE_PWM|OOK_PULSE_PPM"

rm copy.list devices.list rtl_433_ESP.fragment

( cd .. ; rm -rf rtl_433 ; git clone https://github.com/merbanan/rtl_433 )
( cd ../rtl_433/src/devices/ ; egrep "\.name|\.modulation|\.decode_fn|^r_device " *.c ) |\
awk -f device.awk | egrep ${MODULATION} | awk -F : '{ print $1 }' | sort | uniq > copy.list

# Populate src/rtl_433/device

( cd ../src/rtl_433/devices ; rm * )

for i in `cat copy.list`
do
cp ../rtl_433/src/devices/$i ../src/rtl_433/devices
done

for i in `ls ../contrib/`
do
cp ../contrib/$i ../src/rtl_433/devices
done

# remove non-functional device decoders

# device decoders that place a bitbuffer_t on the stack is causing an overflow
# ie bitbuffer_t databits = {0};

for i in newkaku.c nexa.c proove.c cavius.c current_cost.c ge_coloreffects.c insteon.c m_bus.c oil_standard.c oil_watchman.c tpms_abarth124.c tpms_citroen.c tpms_elantra2012.c tpms_ford.c tpms_jansite.c tpms_jansite_solar.c tpms_pmv107j.c tpms_renault.c tpms_toyota.c
do
rm ../src/rtl_433/devices/$i
done

# create include/rtl_433_devices.h

( cd ../src/rtl_433/devices ; egrep "\.name|\.modulation|\.decode_fn|^r_device " *.c ) > devices.list

COUNT=`cat devices.list | awk -f device.awk | egrep ${MODULATION} | awk -F\" '{ print $3 }' | awk -F, '{ print $3 }' | wc | awk '{ print $1 }'`

cat devices.list | awk -f device.awk | egrep ${MODULATION} | awk -F\" '{ print $3 }' | \
awk -F, '{ print $3 }' | awk '{ print "  DECL("$1") \\" }' > rtl_433_devices.fragment

echo "  /* Add new decoders here. */" >> rtl_433_devices.fragment

echo "#define NUMOFDEVICES ${COUNT}" >> rtl_433_devices.fragment

cat rtl_433_devices.pre rtl_433_devices.fragment rtl_433_devices.post > ../include/rtl_433_devices.h

# create src/rtl_433_ESP.cpp fragment

echo "  // This is a generated fragment from tools/update_rtl_433_devices.sh" > rtl_433_ESP.fragment

echo "" >> rtl_433_ESP.fragment

cat devices.list | awk -f device.awk | egrep ${MODULATION} | awk -F\" '{ print $3 }' | \
awk -F, '{ print $3 }' | awk '{ print "  memcpy(&cfg->devices["NR-1"], &"$1", sizeof(r_device));" }' >> rtl_433_ESP.fragment

echo "" >> rtl_433_ESP.fragment

echo "  // end of fragement" >> rtl_433_ESP.fragment

echo
echo "Please update src/rtl_433_ESP.cpp with rtl_433_ESP.fragment"

# copy src files from rtl_433/src to src/rtl_433

for i in abuf.c bitbuffer.c decoder_util.c list.c r_util.c util.c
do
  cp ../rtl_433/src/$i ../src/rtl_433
done

# copy include files from rtl_433/include to include

for i in abuf.h am_analyze.h baseband.h bitbuffer.h compat_time.h decoder.h decoder_util.h fatal.h fileformat.h list.h optparse.h pulse_demod.h r_api.h r_util.h samp_grab.h term_ctl.h util.h 
do
  cp ../rtl_433/include/$i ../include
done


