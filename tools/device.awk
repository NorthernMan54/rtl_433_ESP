# rtl_433 
# this was created 
# egrep "\.name|\.modulation|\.decode_fn|^r_device " *.c > device.list
# List of files
# cat device.list | awk -f device.awk | egrep "OOK_PULSE_PWM|OOK_PULSE_PPM" | awk -F : '{ print $1 }' | sort | uniq > list
# Update to rtl_433_devices.h
# cat device.list | awk -f device.awk | egrep "OOK_PULSE_PWM|OOK_PULSE_PPM" | awk -F\" '{ print $3 }' | awk -F, '{ print $3 }' | awk '{ print "DECL("$1") \\" }'

#acurite.c:r_device const acurite_rain_896 = {
#acurite.c:        .name        = "Acurite 896 Rain Gauge",
#acurite.c:        .modulation  = OOK_PULSE_PPM,
#acurite.c:        .decode_fn   = &acurite_rain_896_decode,
#acurite.c:r_device const acurite_th = {
#acurite.c:        .name        = "Acurite 609TXC Temperature and Humidity Sensor",
#acurite.c:        .modulation  = OOK_PULSE_PPM,
#acurite.c:        .decode_fn   = &acurite_th_decode,
#acurite.c:r_device const acurite_txr = {
#acurite.c:        .name        = "Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 899 Rain, 3N1, Atlas",
#acurite.c:        .modulation  = OOK_PULSE_PWM,
#acurite.c:        .decode_fn   = &acurite_txr_callback,
#lacrosse.c:r_device const lacrossetx = {
#lacrosse.c:        .name        = "LaCrosse TX Temperature / Humidity Sensor",
#lacrosse.c:        .modulation  = OOK_PULSE_PWM,
#lacrosse.c:        .decode_fn   = &lacrossetx_decode,
#lacrosse_tx141x.c:r_device lacrosse_tx141x = {
#lacrosse_tx141x.c:        .name        = "LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth, (TFA, ORIA) sensor",
#lacrosse_tx141x.c:        .modulation  = OOK_PULSE_PWM,
#lacrosse_tx141x.c:        .decode_fn   = &lacrosse_tx141x_decode,
#lacrosse_ws7000.c:r_device const lacrosse_ws7000 = {
#lacrosse_ws7000.c:        .name        = "LaCrosse/ELV/Conrad WS7000/WS2500 weather sensors",
#lacrosse_ws7000.c:        .modulation  = OOK_PULSE_PWM,
#lacrosse_ws7000.c:        .decode_fn   = &lacrosse_ws7000_decode,
 
{
	if ( $5 == "{" ) {
		module = $3
		}
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
