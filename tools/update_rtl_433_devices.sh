#! /bin/sh

( cd .. ; rm -rf rtl_433 ; git clone https://github.com/merbanan/rtl_433 )
( cd ../rtl_433/src/devices/ ; egrep "\.name|\.modulation|\.decode_fn|^r_device " *.c ) |\
awk -f device.awk | egrep "OOK_PULSE_PWM|OOK_PULSE_PPM" | awk -F : '{ print $1 }' | sort | uniq > copy_list

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

# create include/rtl_433_devices.h

( cd ../src/rtl_433/devices ; egrep "\.name|\.modulation|\.decode_fn|^r_device " *.c ) > devices.list

cat devices.list | awk -f device.awk | egrep "OOK_PULSE_PWM|OOK_PULSE_PPM" | awk -F\" '{ print $3 }' | awk -F, '{ print $3 }' | awk '{ print "  DECL("$1") \\" }' > rtl_433_devices.fragment

cat rtl_433_devices.pre rtl_433_devices.fragment rtl_433_devices.post > ../include/rtl_433_devices.h

# create src/rtl_433_ESP.cpp fragment

cat ../include/rtl_433_devices.h | grep DECL | cut -c8- | awk -F\) '{ print "memcpy(&cfg->devices["NR-1"], &"$1", sizeof(r_device));" }' > rtl_433_ESP.fragment
