This is an attempt at creating an Arduino library for use on ESP32 boards with a CC1101 transceiver from the rtl_433 package.  And be available for use with the openMQTTGateway package as an available module.

Inspiration for this came from the ESPiLight effort.

The initial port only enables a subset of the available modulations and device decoders.

```
	OOK_PPM :         Pulse Position Modulation
	OOK_PWM :         Pulse Width Modulation
```

```
 	'-DRAW_SIGNAL_DEBUG=true'   ; display raw received messages
	'-DDEMOD_DEBUG=true'   ; RTL_433 Verbose
	'-DMEMORY_DEBUG=true'   ; display memory usage information
	'-DDEVICE_DEBUG=true'	; debug devices 
	 -DMY_DEVICES=true		; subset of devices
```