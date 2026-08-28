# CC1101 Vivint 345 MHz tuning results

## Executive summary

An unattended series of CC1101 OOK/ASK tests was performed with a Vivint test
transmitter emitting approximately once every 30 seconds. The final long A/B
comparison favored the original tuned profile over the later extended-suite
candidate:

```ini
-DRF_MODULE_FREQUENCY=345.00
-DCC1101_RX_BANDWIDTH=162.5
-DCC1101_AGCCTRL2=0x84
-DCC1101_AGCCTRL1=0x40
-DCC1101_AGCCTRL0=0xA0
```

Across 56 five-minute windows per profile, this profile produced 6,080 decoded
messages versus 5,221 for the extended candidate. It won 53 paired windows,
lost two, and tied one. It remains the recommended starting point for the
tested installation.

## Test method

- Hardware: ESP32 and CC1101 receiver running rtl_433_ESP.
- Signal: Vivint 345 MHz OOK transmitter generated a burst about every 30
  seconds.
- Window length: five minutes, giving approximately ten transmission
  opportunities per setting.
- Primary score: number of messages successfully decoded during each window.
- Execution: settings changed automatically without reflashing between tests.
- Failure guard: a suite stopped after a complete phase in which every setting
  produced zero decodes, indicating a possible transmitter or receiver fault.
- Validation: finalists were alternated in A/B windows to reduce bias from
  changing propagation or interference conditions.

The firmware also recorded raw signals, decoder signals, zero-decode outcomes,
callback messages, RSSI summaries, and mean pulse counts. These were useful
diagnostics, but decoded-message count was the selection criterion.

## Profiles

The initial rtl_433_ESP-compatible CC1101 profile and the ultimately selected
profile differ as follows:

| Setting | Broad default | Recommended Vivint profile |
| --- | ---: | ---: |
| Center frequency | 433.92 MHz normally; 345 MHz for this test | 345.00 MHz |
| Receive bandwidth | approximately 812.5 kHz | 162.5 kHz |
| `AGCCTRL2` | `0xC7` | `0x84` |
| `AGCCTRL1` | `0x40` | `0x40` |
| `AGCCTRL0` | `0xB2` | `0xA0` |

The later extended-suite candidate tested in the final comparison was:

```ini
-DRF_MODULE_FREQUENCY=345.12
-DCC1101_RX_BANDWIDTH=325.0
-DCC1101_AGCCTRL2=0xC1
-DCC1101_AGCCTRL1=0x00
-DCC1101_AGCCTRL0=0x61
```

## Intermediate default-versus-refined result

After three five-minute windows per profile, an earlier comparison reported:

| Profile | Decoder signals | Decoded messages | Zero-decode outcomes |
| --- | ---: | ---: | ---: |
| Broad default | 201 | 487 | 25 |
| Refined | 233 | 416 | 13 |

The refined receiver captured more decoder signals and approximately half as
many zero-decode outcomes, while the broad profile produced more decoded
messages in that short run. This mixed result motivated longer, directly
alternating comparisons rather than selecting a profile from a single short
suite.

## Final comparison

The final comparison alternated these two profiles:

| Profile | Frequency | Bandwidth | `AGCCTRL2` | `AGCCTRL1` | `AGCCTRL0` |
| --- | ---: | ---: | ---: | ---: | ---: |
| Original recommendation | 345.00 MHz | 162.5 kHz | `0x84` | `0x40` | `0xA0` |
| Extended candidate | 345.12 MHz | 325.0 kHz | `0xC1` | `0x00` | `0x61` |

| Result | Original recommendation | Extended candidate |
| --- | ---: | ---: |
| Five-minute windows | 56 | 56 |
| Decoded messages | 6,080 | 5,221 |
| Paired-window wins | 53 | 2 |
| Paired-window ties | 1 | 1 |

The original recommendation produced 859 more decoded messages, an increase of
approximately 16.5% relative to the extended candidate. Its strong consistency
across paired windows is more persuasive than a winner selected from one short
search phase.

## Interpretation

The largest practical change from the broad default is the narrower channel
filter. It rejects more adjacent-channel energy but also leaves less tolerance
for transmitter and CC1101 crystal offset. The AGC changes alter the magnitude
target, gain behavior, hysteresis, and filtering behavior. These register
values are a tested combination and should be evaluated as a profile rather
than assuming that any individual byte is independently optimal.

This testing demonstrates improved decode yield for this receiver, antenna,
transmitter population, and RF environment. It does not establish a universal
Vivint preset or a calibrated sensitivity/range improvement. Module crystal
tolerance, antenna response, local interference, and transmitter carrier
offset can change the best settings.

## Reproducing the test

The dedicated example contains the broad, refinement, extended, and comparison
environments:

```bash
cd examples/cc1101_tuning
platformio run -e esp32_cc1101_vivint_original_profile_compare -t upload
platformio device monitor -e esp32_cc1101_vivint_original_profile_compare
```

See `examples/cc1101_tuning/README.md` for suite durations and configuration.
No reflashing is required while a suite advances through its settings.

