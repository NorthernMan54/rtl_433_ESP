#include "decoder.h"

/*
* Debug decoder
*/

static int debug_device_decode(r_device *decoder, bitbuffer_t *bitbuffer)
{   
    data_t *data;
    int pulses = 0;
    int duration = 0;
    int rssi = 0;
    data = data_make(
        "model",     "",          DATA_STRING, "Debug Device",
        "pulses",    "pulses",    DATA_INT,    pulses,
        "duration",  "duration",  DATA_INT, duration,
        "rssi",      "rssi",      DATA_INT, rssi,
        NULL);
    decoder_output_data(decoder, data);
    return 1;
}

static char *output_fields[] = {
    "model",
    "length",
    "duration",
    "rssi",
    NULL,
};

r_device debug_device = {
        .name        = "Debug Device",
        .modulation  = 0,
        .short_width = 0,
        .long_width  = 0,
        .gap_limit   = 0,
        .reset_limit = 0,
        .decode_fn   = &debug_device_decode,
        .disabled    = 0,
        .fields      = output_fields,
};
