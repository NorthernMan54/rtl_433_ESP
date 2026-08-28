# CC1101 433.92 MHz tuning results

## Executive summary

Testing did not demonstrate an improvement in reception sensitivity over the
rtl_433_ESP CC1101 defaults. Both the default and tuned profiles received 100%
of the RTL-SDR-confirmed transmissions from the weakest regularly observed
sensor during the valid A/B test.

The tuned profile did demonstrate cleaner decoder behavior. It produced fewer
zero-decode outcomes while preserving reception of every reference event. The
recommended profile for this installation is therefore:

```ini
-DRF_MODULE_FREQUENCY=433.920
-DCC1101_RX_BANDWIDTH=203.125
-DCC1101_AGCCTRL2=0xC7
-DCC1101_AGCCTRL1=0x40
-DCC1101_AGCCTRL0=0x93
```

This recommendation should be described as a noise-reduction improvement, not
as a demonstrated increase in range or sensitivity.

## Test equipment and target

- ESP32 with a CC1101 receiver running rtl_433_ESP
- Local RTL-SDR receiver running rtl_433 as the independent reference
- CC1101 and RTL-SDR antennas located within approximately 30 cm of each other
- Frequency band: 433.92 MHz ISM band
- Target: `Acurite-Tower`, id `2043`, channel `B`
- The target was selected because it was the weakest regularly observed sensor
  and therefore the best available proxy for a distant receiver test.

rtl_433 was run with `-M level` so its events included signal-strength
metadata. Automatic RTL-SDR gain was used for the valid A/B comparison because
fixed-gain trials did not reliably decode the target.

## Testing approach

### Reference correlation

The host test harness ran the CC1101 and RTL-SDR receivers simultaneously. It:

1. Captured machine-readable CC1101 decoder events over the ESP32 serial port.
2. Captured JSON events from the local rtl_433 process.
3. Restricted scoring to the selected Acurite model and id.
4. Deduplicated repeated copies belonging to the same RF transmission burst.
5. Correlated events by model, id, channel, payload similarity, and timestamp.
6. Calculated the CC1101 capture rate using RTL-SDR events as the denominator.

This makes the comparison insensitive to variations in the sensor's
transmission timing: a missed CC1101 packet is counted only when the independent
RTL-SDR receiver confirms that a transmission occurred.

### Initial and focused searches

Initial frequency, bandwidth, and AGC searches used short rotating windows to
eliminate settings that performed poorly. A focused 12-profile comparison then
tested nearby frequencies, valid CC1101 bandwidths, and two `AGCCTRL0` values
against the weakest sensor.

The focused profile selected for final comparison was:

- Frequency: 433.920 MHz
- Receive bandwidth: 203.125 kHz
- `AGCCTRL2`: `0xC7`
- `AGCCTRL1`: `0x40`
- `AGCCTRL0`: `0x93`

### Default-versus-tuned A/B test

The final A/B firmware alternated 60-second windows between these profiles:

| Setting | Library default | Tuned |
| --- | ---: | ---: |
| Center frequency | 433.920 MHz | 433.920 MHz |
| Receive bandwidth | approximately 812.5 kHz | 203.125 kHz |
| `AGCCTRL2` | `0xC7` | `0xC7` |
| `AGCCTRL1` | `0x40` | `0x40` |
| `AGCCTRL0` | `0xB2` | `0x93` |

The run lasted 20 minutes. Alternating settings within the same run reduced the
risk that changing propagation, interference, or sensor conditions would favor
one profile.

## A/B results

| Profile | RTL-SDR opportunities | CC1101 matches | Capture rate |
| --- | ---: | ---: | ---: |
| Library default | 37 | 37 | 100% |
| Tuned | 35 | 35 | 100% |

There were 72 reference events in total, 72 CC1101 matches, no reference-only
events, and no CC1101-only events after deduplication.

For the first nine complete windows per profile, the decoder-level totals were:

| Profile | Decoded messages | Zero-decode outcomes |
| --- | ---: | ---: |
| Library default | 313 | 18 |
| Tuned | 341 | 12 |

The tuned profile reduced zero-decode outcomes by 6, or approximately 33%, and
produced more decoded messages during equal exposure. These counters include
all received signals and messages, not only the selected Acurite sensor, so they
indicate overall decoder behavior in the local RF environment rather than a
target-specific sensitivity measurement.

## Frequency and bandwidth matrix

A subsequent matrix tested nine center frequencies from 433.820 through
434.020 MHz and four valid CC1101 receive bandwidths. AGC settings were held at
the tuned values. Each cell ran for 60 seconds.

The entries below are deduplicated CC1101 events for the target sensor:

| Center frequency | 203.125 kHz | 270.833 kHz | 325.0 kHz | 406.25 kHz |
| ---: | ---: | ---: | ---: | ---: |
| 433.820 MHz | 0 | 0 | 4 | 4 |
| 433.845 MHz | 0 | 4 | 3 | 4 |
| 433.870 MHz | 0 | 4 | 2 | 4 |
| 433.895 MHz | 4 | 4 | 4 | 3 |
| 433.920 MHz | 4 | 4 | 4 | 3 |
| 433.945 MHz | 3 | 4 | 3 | 4 |
| 433.970 MHz | 4 | 4 | 3 | 4 |
| 433.995 MHz | 4 | 3 | 4 | 4 |
| 434.020 MHz | 4 | 3 | 4 | 3 |

The narrow 203.125 kHz configuration failed below approximately 433.895 MHz,
then received every expected target transmission at 433.895 and 433.920 MHz.
This passband boundary suggests that this particular sensor's carrier is above
nominal, approximately 433.98 MHz. Moving the CC1101 center upward did not
produce a better capture rate than 433.920 MHz.

### Matrix limitation

The RTL-SDR reference became unreliable during the matrix run. It recorded only
6 target events while the CC1101 recorded 117, compared with 72 RTL-SDR events
in the preceding 20-minute A/B test. Restarting rtl_433 and moving the RTL-SDR
center frequency did not restore its normal event cadence.

The sensor continued transmitting at a stable interval of approximately 16.25
seconds, as observed by the CC1101 at passing settings. The matrix is therefore
useful for locating the CC1101 passband boundary, but it is not a valid
RTL-SDR-reference-scored sensitivity comparison. It must not be used to claim
that one of the passing matrix cells has greater range than another.

## Interpretation

The principal change is the receive bandwidth reduction from approximately
812.5 kHz to 203.125 kHz. The narrower filter admits less adjacent-band energy
in a very busy 433 MHz environment. Changing `AGCCTRL0` from `0xB2` to `0x93`
also changes AGC timing, hysteresis, and channel-filter averaging behavior.

The measured result is:

- No demonstrated improvement in target packet coverage: both profiles were
  100% during the valid A/B test.
- No demonstrated improvement in maximum range or minimum detectable signal.
- A demonstrated reduction in zero-decode outcomes without a reception loss.
- A cleaner configuration that should reduce unnecessary decoder workload in
  this installation.

## Final recommendation

Retain 433.920 MHz rather than shifting the receiver toward one individual
sensor. It received every tested target transmission and is safer for a mixed
population of nominal 433.92 MHz devices. Retain the 203.125 kHz bandwidth and
the tuned AGC settings because they reduced decoder noise without a measured
packet-coverage penalty.

To establish a sensitivity or range improvement, conduct a separate controlled
test using a repeatable transmitter and calibrated attenuation, or increase
distance under stable conditions. The transmitter should emit a known packet
count while both profiles alternate, and the independent reference receiver
must maintain a verified event cadence throughout the run.

## Result artifacts

- Valid default-versus-tuned A/B report:
  `test-results/cc1101-reference-20260826-190640/summary.json`
- Frequency/bandwidth matrix report:
  `test-results/cc1101-reference-20260827-165515/summary.json`
- Reference comparison harness: `tools/cc1101_reference_test.py`
- Tuning firmware: `examples/cc1101_tuning/cc1101_tuning.ino`
- Tuning environments: `examples/cc1101_tuning/platformio.ini`

