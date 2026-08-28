# CC1101 OOK tuning

This example is a focused version of `OOK_Receiver` for unattended CC1101
OOK/ASK tuning. It changes radio settings at five-minute intervals, scores the
number of decoded messages, carries each phase winner forward, and prints
machine-readable `TUNING_START`, `TUNING_RESULT`, and `TUNING_WINNER` records.

The default configuration targets a Vivint test transmitter near 345 MHz that
emits once every 30 seconds. Adjust the frequency, pins, seeds, test interval,
and window duration in `platformio.ini` for another installation.

## Running a suite

Open this directory as the PlatformIO project, or run:

```bash
cd examples/cc1101_tuning
platformio run -e esp32_cc1101_vivint_extended_tuning -t upload
platformio device monitor -e esp32_cc1101_vivint_extended_tuning
```

No flashing is required between settings. If every setting in a complete phase
has zero decoded messages, the suite aborts and leaves reception active on the
last setting for hardware diagnosis. After all phases complete, the selected
combination runs in repeated validation windows.

## Environments

| Environment | Purpose | Initial duration |
| --- | --- | ---: |
| `esp32_cc1101_vivint_tuning` | Broad first-pass search | 2 h 30 min |
| `esp32_cc1101_vivint_refinement` | Fine frequency, bandwidth, and AGC search | 2 h 15 min |
| `esp32_cc1101_vivint_extended_tuning` | Detailed AGC bit-field characterization | 2 h 35 min |
| `esp32_cc1101_vivint_agc2_compare` | Alternating `AGCCTRL2=0x82` and `0x83` | Continuous |
| `esp32_cc1101_vivint_extended_profile_compare` | Intermediate versus extended profile | Continuous |
| `esp32_cc1101_vivint_original_profile_compare` | Original versus extended profile | Continuous |
| `esp32_cc1101_43392_tuning` | Generic 433.92 MHz sweep with reference events | 28 min |
| `esp32_cc1101_43392_focused_compare` | 12-profile 433.92 MHz focused comparison | Continuous |
| `esp32_cc1101_43392_default_ab` | Library defaults versus focused result | Continuous |
| `esp32_cc1101_43392_freq_bw_matrix` | 9-frequency by 4-bandwidth matrix | 36 min/cycle |

Comparison environments alternate five-minute windows and emit cumulative
`PROFILE_TOTAL` records. A three-hour comparison gives 18 windows per profile.

## Final tested Vivint result

The final 56-window-per-profile comparison favored the original profile by
6,080 decoded messages to 5,221. It won 53 paired windows, lost two, and tied
one. For the tested installation the recommendation remains:

```ini
-DRF_MODULE_FREQUENCY=345.00
-DCC1101_RX_BANDWIDTH=162.5
-DCC1101_AGCCTRL2=0x84
-DCC1101_AGCCTRL1=0x40
-DCC1101_AGCCTRL0=0xA0
```

These values are installation-specific. Antenna characteristics, CC1101
crystal tolerance, local interference, and transmitter offset can change the
best result.

## Comparing with an RTL-SDR reference

The 433.92 MHz environment emits machine-readable `CC1101_EVENT` records. The
host harness captures those alongside a local `rtl_433` process, correlates
events by device identity and time, and writes JSON and CSV reports. It starts
rtl_433 with an isolated configuration, so user MQTT settings are not inherited.

```bash
python3 tools/cc1101_reference_test.py \
  --serial /dev/cu.usbserial-0001 --duration 10m
```

Repeated `--target key=value` options restrict scoring to a stable transmitter,
for example `--target model=Acurite-Tower --target id=1234`.

### Initial 433.92 MHz result

An August 26, 2026 local sweep used 60-second windows and an RTL-SDR reference.
The suite selected the following profile, which then matched 22 of 22 reference
events over two final-validation windows:

```ini
-DRF_MODULE_FREQUENCY=433.88
-DCC1101_RX_BANDWIDTH=232.143
-DCC1101_AGCCTRL2=0xC7
-DCC1101_AGCCTRL1=0x40
-DCC1101_AGCCTRL0=0x93
```

This is an installation-specific first pass. Most neighboring frequency,
bandwidth, and AGC candidates also matched all reference events, so a longer
alternating comparison is needed before changing the library defaults.

A subsequent two-cycle focused comparison scored only the weakest regularly
observed device (`Acurite-Tower`, id `2043`). The earlier
433.88/232.143/0x93 profile matched 7 of 8 reference events. The selected
focused profile matched 8 of 8:

```ini
-DRF_MODULE_FREQUENCY=433.92
-DCC1101_RX_BANDWIDTH=203.125
-DCC1101_AGCCTRL2=0xC7
-DCC1101_AGCCTRL1=0x40
-DCC1101_AGCCTRL0=0x93
```

### Frequency/bandwidth matrix

An August 27, 2026 matrix tested center frequencies from 433.820 through
434.020 MHz and receive bandwidths of 203.125, 270.833, 325.0, and 406.25 kHz.
The selected Acurite Tower transmitted every approximately 16.25 seconds. The
203.125 kHz setting decoded no target events at 433.820--433.870 MHz, then
decoded every expected event at 433.895 and 433.920 MHz. This indicates that
the tested sensor's carrier is above nominal, approximately 433.98 MHz.

The RTL-SDR reference became unreliable during this run: it decoded only 6
target events while the CC1101 decoded 117, compared with 72 reference events
in the preceding 20-minute A/B test. Restarting and offsetting the SDR did not
restore its normal cadence. Consequently, this matrix is useful for locating
the CC1101 passband boundary but is not a valid reference-scored sensitivity
comparison. The fixed recommendation remains 433.920 MHz / 203.125 kHz because
it decoded every expected event and the matrix did not demonstrate an improved
capture rate at another passing center frequency.
