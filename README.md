This is an attempt at creating an Arduino library for use on ESP32 boards with a CC1101 transceiver with the device decoders from the rtl_433 package.  And be available for use with openMQTTGateway as an available module.

Inspiration for this came from the ESPiLight effort.  Kudos to puuu for this, and the awesome package.

The initial port implements only enables a subset of the available modulations and device decoders, and is limited to devices I have access to and can test with.

## Available Pulse Demodulation modules

```
	OOK_PPM :         Pulse Position Modulation
	OOK_PWM :         Pulse Width Modulation
```

## Roadmap / Next Steps

* [ ] Firmware size tuning
* [ ] Further heap usage tuning
* [ ] Find stack leak when running all devices
* [ ] Enable addition pulse demod functions

## Compile definition options

```
MINRSSI						; Default rssi to enable receiver, defaults to -82
MY_DEVICES					; Only include my personal subset of devices

DEMOD_DEBUG					; enable verbose debugging of signal processing
DEVICE_DEBUG				; Validate fields are mapped to response object ( rtl_433 )
MEMORY_DEBUG				; display heap usage information
RAW_SIGNAL_DEBUG			; display raw received messages
RSSI						; Enable collection of per pulse RSSI Values during signal reception
RTL_DEBUG					; Enable RTL_433 Verbose option ( 0=normal, 1=verbose, 2=verbose decoders, 3=debug decoders, 4=trace decoding. )
```

## Porting approach

* Copy of all files in src/rtl_433/devices without any modification
* Copy of only used files in src/rtl_433, with minimal modifications
- abuf.c 			- No changes made
- bitbuffer.c		- No changes made 
- data.c			- Defined out unneeded functions ( #ifndef ESP32 )
- decoder_util.c 	- No changes made
- list.c			- No changes made
- pulse_demod.c		- Move 'bitbuffer_t bits' to class level
- r_api.c			- Significant tuning and tweaking applied
- r_util.c			- No changes made
- util.c			- No changes made

* include directory is a copy 
- abuf.h			- No changes made
- am_analyze.h		- No changes made
- baseband.h		- No changes made
- bitbuffer.h		- No changes made
- compat_time.h		- No changes made
- data.h			- Added '#define _POSIX_HOST_NAME_MAX  128'
- decoder.h			- No changes made
- decoder_util.h	- No changes made
- fatal.h			- No changes made
- fileformat.h		- No changes made
- list.h			- No changes made
- log.h				- Not rtl_433.h
- optparse.h		- No changes made
- pulse_demod.h		- No changes made
- pulse_detect.h	- Adjusted structures to reduce size
- r_api.h			- No changes made
- r_device.h		- Adjusted structures to reduce size
- r_private.h		- Adjusted structures to reduce size
- r_util.h			- No changes made
- rtl_433.h			- Adjusted structures to reduce size
- rtl_433_devices.h	- Only added my skylink device
- rtl_devices.h		- Not needed ??
- samp_grab.h		- No changes made
- term_ctl.h		- No changes made
- util.h			- No changes made

## Codebase conflicts

* ESPiLight and rtl_433 conflict on silvercrest

```
.platformio/packages/toolchain-xtensa32/bin/../lib/gcc/xtensa-esp32-elf/5.2.0/../../../../xtensa-esp32-elf/bin/ld: Warning: size of symbol `silvercrest' changed from 4 in .pio/build/rtl_433-9e0770/libb28/libESPiLight.a(protocol.c.o) to 76 in .pio/build/rtl_433-9e0770/lib395/librtl_433_ESP.a(silvercrest.c.o)
```