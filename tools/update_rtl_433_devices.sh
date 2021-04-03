#! /bin/sh

export MODULATION="OOK_PULSE_PWM|OOK_PULSE_PPM|FSK_PULSE_PCM"

( cd .. ; rm -rf rtl_433 ; git clone https://github.com/merbanan/rtl_433 )
( cd ../rtl_433/src/devices/ ; egrep "\.name|\.modulation|\.decode_fn|^r_device " *.c ) |\
awk -f device.awk | egrep ${MODULATION} | awk -F : '{ print $1 }' | sort | uniq > copy_list

# Populate src/rtl_433/device

( cd ../src/rtl_433/devices ; rm * )

for i in `cat copy_list`
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

for i in newkaku.c nexa.c proove.c
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