# rtl_433 
# this was created 
# egrep "\.name|\.modulation|\.decode_fn|^r_device " *.c > device.list
# List of files
# cat device.list | awk -f device.awk | egrep "OOK_PULSE_PWM|OOK_PULSE_PPM" | awk -F : '{ print $1 }' | sort | uniq > list
# Update to rtl_433_devices.h
# cat device.list | awk -f device.awk | egrep "OOK_PULSE_PWM|OOK_PULSE_PPM" | awk -F\" '{ print $3 }' | awk -F, '{ print $3 }' | awk '{ print "DECL("$1") \\" }'

# acurite.c:r_device acurite_rain_896 = {
# acurite.c:        .name        = "Acurite 896 Rain Gauge",
# acurite.c:        .modulation  = OOK_PULSE_PPM,
# acurite.c:        .decode_fn   = &acurite_rain_896_decode,
# acurite.c:        .name        = "Acurite 609TXC Temperature and Humidity Sensor",
# acurite.c:        .modulation  = OOK_PULSE_PPM,
# acurite.c:        .decode_fn   = &acurite_th_decode,
# acurite.c:        .name        = "Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas",
# acurite.c:        .modulation  = OOK_PULSE_PWM,
# acurite.c:        .decode_fn   = &acurite_txr_decode,

{
	if ( $4 == "{" ) {
		module = $2
		}
	if ( $2 == ".name" ) { 
		for(i=4;i<=NF;i++){name=name" "$i}
		file = $1
		}
        if ( $2 == ".modulation" ) {
		modulation = $4
		}
        if ( $2 == ".decode_fn" ) {
	        decode_fn = $4
        	print $1",", name, modulation, module
		name = ""
		}
}
