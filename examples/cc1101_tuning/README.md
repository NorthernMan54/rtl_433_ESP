# CC1101 OOK tuning

This example is a focused version of `OOK_Receiver` for unattended CC1101
OOK/ASK tuning. It changes radio settings at five-minute intervals, scores the
number of decoded messages, carries each phase winner forward, and prints
machine-readable `TUNING_START`, `TUNING_RESULT`, and `TUNING_WINNER` records.

## Vivint 345 MHz tuning

The Vivint tuning, including the extended tuning and subsequent validation,
was performed with Vivint devices at 345 MHz. The default configuration targets
a Vivint test transmitter that emits once every 30 seconds. Adjust the
frequency, pins, seeds, test interval, and window duration in `platformio.ini`
for another installation.

### Antenna length

A free-space quarter wavelength at 345 MHz is approximately 8.55 inches
(217 mm). The supplied spring was wound on approximately a 9/64-inch (3.57 mm)
diameter and contained approximately 12 inches of conductor when unwound. Its
original resonant frequency was not measured; it is believed to have been a
nominal 433 MHz antenna supplied with the CC1101 receiver.

The side-by-side test used two receivers named `north` and `south`; these are
device names without positional or directional meaning. The `north` device had
an approximately 8 1/2-inch straight whip and the `south` device had an
approximately 8 3/4-inch straight whip. Their observed 345 MHz receive
performance was about the same, but receiver differences remain confounded with
antenna length. The antenna has a slotted barrel approximately 1/4 inch long,
and its exact electrical contribution is unknown. See the formal discussion in
[CC1101 Vivint tuning results](../../docs/CC1101_VIVINT_TUNING_RESULTS.md)
before reproducing the antenna change or interpreting the comparison.

### Running a Vivint suite

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

### Vivint environments

| Environment | Purpose | Initial duration |
| --- | --- | ---: |
| `esp32_cc1101_vivint_tuning` | Focused post-antenna-trim search | 1 h 55 min |
| `esp32_cc1101_vivint_refinement` | Fine frequency, bandwidth, and AGC search | 2 h 25 min |
| `esp32_cc1101_vivint_extended_tuning` | Detailed AGC bit-field characterization | 2 h 35 min |
| `esp32_cc1101_vivint_final_exploration` | Narrow search around the released profile | 1 h 40 min |
| `esp32_cc1101_vivint_agc2_compare` | Alternating `AGCCTRL2=0x82` and `0x83` | Continuous |
| `esp32_cc1101_vivint_extended_profile_compare` | Intermediate versus extended profile | Continuous |
| `esp32_cc1101_vivint_original_profile_compare` | Original versus extended profile | Continuous |
| `esp32_cc1101_vivint_post_trim_compare` | Released Vivint profile versus post-trim candidate | Continuous |
| `esp32_cc1101_vivint_post_trim_frequency_compare` | Isolated 345.00 versus 345.10 MHz comparison | Continuous |
| `esp32_cc1101_vivint_post_trim_bandwidth_compare` | Isolated 162.5 versus 203.125 kHz comparison | Continuous |
| `esp32_cc1101_vivint_post_trim_agcctrl2_compare` | Isolated `AGCCTRL2=0x84` versus `0xC7` comparison | Continuous |
| `esp32_cc1101_vivint_post_trim_agcctrl0_compare` | Isolated `AGCCTRL0=0xA0` versus `0x90` comparison | Continuous |
| `esp32_cc1101_vivint_final_exploration_compare` | Released profile versus narrow-exploration candidate | Continuous |
| `esp32_cc1101_vivint_final_agcctrl2_compare` | Isolated `AGCCTRL2=0x84` versus `0x86` comparison | Continuous |
| `esp32_cc1101_vivint_final_frequency_compare` | Isolated 345.00 versus 345.02 MHz comparison | Continuous |
| `esp32_cc1101_vivint_counter_loss_compare` | Counter continuity for `AGCCTRL2=0x84/0x85/0x86` | Continuous |
| `esp32_cc1101_vivint_counter_bandwidth_compare` | Counter continuity for adjacent bandwidths | Continuous |
| `esp32_cc1101_vivint_counter_agc_wait_compare` | Counter continuity for AGC wait time | Continuous |
| `esp32_cc1101_vivint_counter_agc_filter_compare` | Counter continuity for AGC filter length | Continuous |
| `esp32_cc1101_vivint_counter_agc_hyst_compare` | Counter continuity for AGC hysteresis | Continuous |

Comparison environments emit cumulative `PROFILE_TOTAL` records. Most use
five-minute windows, so a two-profile three-hour comparison gives 18 windows
per profile. The counter-continuity environments use ten-minute windows.

### Final tested Vivint result

The earlier 56-window-per-profile comparison established the tuned profile.
A later repeat-yield test appeared to favor `AGCCTRL2=0x86`, but a
production-style counter-continuity test superseded that result: `0x84` and
`0x85` had no counter gaps, while `0x86` lost 16 counters in two windows.
Bandwidth and the remaining AGC fields were then isolated using the same
counter-based scoring. For the tested installation the recommendation is:

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

