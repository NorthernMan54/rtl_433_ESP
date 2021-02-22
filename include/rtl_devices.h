#ifndef INCLUDE_RTL_DEVICES_H_
#define INCLUDE_RTL_DEVICES_H_


int pulse_demod_ppm(const pulse_data_t *pulses, r_device *device);

int prologue_callback(r_device *decoder, bitbuffer_t *bitbuffer);
extern r_device prologue;
//void prologue_callback();


#endif /* INCLUDE_RTL_DEVICES_H_ */