int pulse_demod_ppm(const pulse_data_t *pulses, r_device *device);

void prologue_callback(r_device *decoder, bitbuffer_t *bitbuffer);
extern r_device prologue;
//void prologue_callback();