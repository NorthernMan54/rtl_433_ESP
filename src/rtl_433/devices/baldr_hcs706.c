/** @file
    Baldr HCS706 Pool Thermometer sensor.

    Copyright (C) 2026 Ke Wu

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
*/

#include "decoder.h"

/**
Baldr HCS706 Pool Thermometer sensor.

Brand:

- Fujian Baldr Technology Co., Ltd.

Reference:

- BaldrTherm HCS706 Pool Sensor, paired with a HomGar hub, 433.92 MHz

Protocol reverse engineered 2026-11 from ESP32/CC1101 pulse captures
(see rtl_433 issue 3333 for the sibling HCS528ARF protocol).

OOK PCM, MC, Invert and Reflect, the message is repeated 10 times.
Same family as Baldr HCS528ARF, but a 14 byte message instead of 11.

Flex:

    rtl_433 -X 'n=name,m=OOK_PCM,s=300,l=300,g=700,r=1100'

Data Layout (after reflect):

    Byte Position  0  1  2  3  4  5  6  7  8  9 10 11 12 13
    Data          SS II II II II FB FF FF FF FF FF TT TF CC

- SS:{8}  0x55 , header sync word (raw MC byte 0xaa before reflect)
- II:{32} Sensor ID
- FF:{x}  Fixed / flags, meaning not fully decoded
- B1:{1}  Low Battery flag = 1, Good Battery = 0  (assumption from baldr_hcs528arf, bit 1 of byte 5)
- TT:{12} Temperature, Fahrenheit, scale 10, little endian (byte 11 + low nibble of byte 12)
- CC:{8}  Checksum, addition of previous reflected bytes except sync word.

The MC bit stream carries two leading bits before the first data byte,
so decoding is attempted at a small range of start offsets and the first
offset passing sync + checksum wins.
*/

static int baldr_hcs706_decode(r_device *decoder, bitbuffer_t *bitbuffer)
{
    // Find repeats
    int row = bitbuffer_find_repeated_row(bitbuffer, 4, 200);
    if (row < 0) {
        return DECODE_ABORT_EARLY;
    }

    if (bitbuffer->bits_per_row[row] > 260) {
        return DECODE_ABORT_LENGTH;
    }

    for (unsigned start = 0; start <= 4; start++) {
        bitbuffer_t decoded = {0};
        bitbuffer_manchester_decode(bitbuffer, row, start, &decoded, 14 * 2 * 8);
        bitbuffer_invert(&decoded);

        uint8_t *b = decoded.bb[0];
        reflect_bytes(b, 14);

        if (b[0] != 0x55) { // Header sync word
            continue;
        }

        // Checksum
        int sum = add_bytes(&b[1], 12); // header not part of the sum
        if ((sum & 0xff) != b[13]) {
            continue;
        }

        decoder_log_bitrow(decoder, 2, __func__, b, 112, "MSG");

        int id       = b[4] << 24 | b[3] << 16 | b[2] << 8 | b[1]; // little endian
        int bat_low  = (b[5] & 0x02) >> 1;
        int temp_raw = (b[12] & 0x0F) << 8 | b[11]; // little endian
        float temp_f = temp_raw * 0.1f;

        /* clang-format off */
        data_t *data = data_make(
                "model",            "",             DATA_STRING,    "Baldr-HCS706",
                "id",               "",             DATA_FORMAT,    "%08x",   DATA_INT,       id,
                "battery_ok",       "Battery",      DATA_INT,       !bat_low,
                "temperature_F",    "Temperature",  DATA_FORMAT,    "%.1f F", DATA_DOUBLE, temp_f,
                "mic",              "Integrity",    DATA_STRING, "CHECKSUM",
                NULL);
        /* clang-format on */

        decoder_output_data(decoder, data);
        return 1;
    }

    return DECODE_ABORT_EARLY;
}

static char const *const output_fields[] = {
        "model",
        "id",
        "battery_ok",
        "temperature_F",
        NULL,
};

r_device const baldr_hcs706 = {
        .name        = "Baldr HCS706 Pool Thermometer sensor",
        .modulation  = OOK_PULSE_PCM,
        .short_width = 305,
        .long_width  = 305,
        .gap_limit   = 700,
        .reset_limit = 1100,
        .decode_fn   = &baldr_hcs706_decode,
        .fields      = output_fields,
};
