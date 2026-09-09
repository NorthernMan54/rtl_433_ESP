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

The earlier `0x84` profile produced 6,080 decoded messages versus 5,221 for the
extended candidate across 56 five-minute windows per profile. A later test that
counted duplicate burst repeats favored `0x86`, but production counter skips
showed that repeat yield was the wrong primary metric. A counter-continuity
retest found zero gaps with `0x84` and `0x85`, while `0x86` lost 16 counters in
two windows. The recommendation therefore remains `0x84`.

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
callback messages, RSSI summaries, and mean pulse counts. Later production-style
tests used unique Vivint counter continuity as the primary score: duplicate
repeats counted once and skipped counter values indicated a completely missed
transmission.

## Antenna tuning

### Reference dimensions and feed point

The free-space wavelength at 345 MHz is approximately 869 mm (34.2 inches), so
a theoretical quarter-wave monopole is approximately 217 mm (8.55 inches).
That value is a starting point, not a guaranteed installed resonant length.
The connector, receiver ground plane, wire diameter, enclosure, nearby wiring,
and surrounding objects all contribute to the antenna system and can shift its
resonance.

A straight whip should be measured along the conductive path from the point
where the connector center conductor joins the radiating element to the tip.
The connector's grounded outer shell is not part of that measurement. The
antenna used here has a slotted metal barrel approximately 1/4 inch long on the
coil side. It was not established how much of that barrel is electrically part
of the center-fed radiator, so quoted physical lengths should state whether the
barrel is included. This uncertainty is significant compared with the 1/4-inch
difference between the two whips tested below.

### Supplied spring antenna

The CC1101 receiver was supplied with a spring antenna believed to be intended
for the commonly used 433 MHz band. The spring was wound on approximately a
9/64-inch (3.57 mm) diameter, determined by inserting a snug drill bit. When
unwound, its conductor measured approximately 12 inches.

The 12-inch conductor length must not be compared directly with the 8.55-inch
quarter-wave dimension. In the spring form, distributed inductance,
turn-to-turn capacitance, pitch, wire diameter, straight lead length, connector,
and PCB ground plane determine resonance. The spring is therefore an
electrically shortened antenna, rather than a 12-inch straight whip folded into
a mechanically convenient shape.

Moving a nominal 433 MHz spring design toward 345 MHz requires making it
electrically longer. The frequency ratio, 433/345, is approximately 1.255, so a
fully geometrically scaled design would be about 25.5% larger. That ratio is
useful only as an initial estimate: adding turns, compressing turns, or adding a
straight section does not scale every distributed property equally. Trimming
the spring would normally move its resonance upward, away from 345 MHz.
Accurate adjustment of the coiled form requires a VNA measurement at the feed
point with the antenna installed over a representative receiver ground plane.

For this receive-only experiment, converting the available conductor into a
straight quarter-wave whip provided a simpler, reproducible starting point.
The wire should initially be left slightly long and shortened in small steps
only after comparative testing or a VNA measurement. Folding back the excess
is reversible but still changes the antenna electrically, so it is not an
exact substitute for testing the final cut length.

### Installed comparison and interpretation

Two receivers named `north` and `south` were operated concurrently, side by
side. These names identify the devices and do not describe their test positions
or signal directions. The original tuned `north` device used an antenna
approximately 8 1/2 inches long, while the new `south` device used an antenna
approximately 8 3/4 inches long. Both receive-only installations produced about
the same observed performance at 345 MHz.

In isolation, an ideal 8.5-inch quarter wave corresponds to approximately
347 MHz and an ideal 8.75-inch quarter wave to approximately 337 MHz. Real
installed resonance cannot be inferred from those calculations because of the
feed-point and ground-plane effects described above. The 1/4-inch length
difference is only about 3%, and reception is normally broad enough that a
clear difference may not appear when the received signal has ample margin.

The present observation supports the limited conclusion that neither antenna
has demonstrated a practical advantage in its current position. It does not
show that both antennas are equally resonant or that 8.5 and 8.75 inches are
universally equivalent. Receiver sensitivity and construction remain
confounded with antenna length, while orientation, multipath, interference,
and transmitter distance can also obscure small differences.

A stronger follow-up test would use unique Vivint counters and median RSSI over
many transmissions, then swap the antennas between the two side-by-side
receivers. Testing at the weakest reliably received transmitter location will
expose differences that a strong signal can conceal. Until such a crossover
test or VNA sweep shows otherwise, 8.5 inches is a defensible nominal 345 MHz
length and there is no measured reason to shorten the 8.75-inch antenna.

Any cutting, coiling, feed-line rerouting, enclosure change, or antenna
repositioning can change reception and should be followed by a new controlled
comparison. The RF profile results in this document apply to the antenna and
installation used during each test; antenna changes can justify repeating the
receiver-profile validation.

## Receiver profile retesting after the antenna change

### Post-trim retest result

After trimming the antenna, a fresh sweep initially selected a combined
345.10 MHz / 203.125 kHz / `0xC7` / `0x40` / `0x90` candidate. A direct
alternating comparison rejected that combination: the released profile
produced 333 decoded messages across four windows versus 159 for the candidate.

Follow-up A/B tests changed one setting at a time while holding the released
profile constant:

| Isolated setting | Released value | Candidate value | Result |
| --- | ---: | ---: | --- |
| Frequency | 345.00 MHz | 345.10 MHz | 79 versus 0 decoded messages; retain 345.00 MHz |
| Bandwidth | 162.5 kHz | 203.125 kHz | 166 versus 96 after two cycles; retain 162.5 kHz |
| `AGCCTRL2` | `0x84` | `0xC7` | 167 versus 46 after two cycles; retain `0x84` |
| `AGCCTRL0` | `0xA0` | `0x90` | 346 versus 350 after four cycles; no meaningful decode-yield difference |

Because decoded-message count is the primary score, the four-message
`AGCCTRL0` difference is treated as a tie rather than evidence for changing
the shipping profile. The post-trim retest therefore leaves the existing
recommendation unchanged.

### Final narrow exploration

A final sweep tested frequency from 344.97 through 345.03 MHz, adjacent valid
CC1101 bandwidths, `AGCCTRL2` values `0x82` through `0x86`, both AGC carrier
sense modes, and three AGC wait times. Sequential winners were treated only as
candidates and then checked with directly alternating A/B windows.

The combined `345.02 / 162.5 / 0x86 / 0x40 / 0x90` candidate produced 214
decoded messages versus 120 for the released profile across two complete A/B
cycles. Follow-up tests attributed the useful change to `AGCCTRL2`:

| Isolated test | Result | Decision |
| --- | ---: | --- |
| `AGCCTRL2` `0x84` vs `0x86` | 130 vs 228 decoded; 106 vs 18 failures | Use `0x86` |
| 345.00 vs 345.02 MHz with `0x86` | 444 vs 442 decoded; 20 vs 30 failures | Keep 345.00 MHz |
| `AGCCTRL0` `0x90`, `0xA0`, `0xB0` sequential check | 112, 110, 111 decoded | Keep `0xA0` |
| 135.417, 162.5, 203.125 kHz | 39, 73, 45 decoded | Keep 162.5 kHz |

This result demonstrates why decoded-repeat totals alone are not sufficient.
The counter-continuity validation below supersedes its `0x86` conclusion.

### Production-style counter-continuity validation

Ten-minute alternating windows counted each Vivint counter once, regardless of
how many copies of its burst decoded. The key results were:

| Isolated test | Counter-continuity result | Decision |
| --- | --- | --- |
| `AGCCTRL2` `0x84` / `0x85` / `0x86` | `0x84`: 57 unique, 0 gaps; `0x85`: 58 unique, 0 gaps; `0x86`: 21 unique, 16 gaps | Keep conservative `0x84` |
| Bandwidth 135.417 / 162.5 / 203.125 / 232.143 kHz | 34/0, 39/0, 38/3, 39/0 unique/gaps | Keep narrower 162.5 kHz |
| AGC wait `0x80` / `0x90` / `0xA0` / `0xB0` | Longer three values were effectively tied and gap-free | Keep `0xA0` |
| AGC filter `0xA0` / `0xA1` / `0xA2` / `0xA3` | `0xA1` halved repeat margin; `0xA2`/`0xA3` nearly eliminated Vivint decodes | Keep `0xA0` |
| AGC hysteresis `0x20` / `0x60` / `0xA0` / `0xE0` | `0xA0` led with 41 unique counters; `0xE0` reduced repeat margin | Keep `0xA0` |

For the tested receiver, antenna, transmitter, and RF environment, the final
recommendation is `345.00 MHz / 162.5 kHz / 0x84 / 0x40 / 0xA0`.

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
