# rtl_433_ESP architecture

## Scope

rtl_433_ESP adapts the upstream rtl_433 pulse demodulators and device decoders
to ESP32 radios supported by RadioLib. Radio selection and wiring are compile-
time settings. The runtime currently supports one receiver per firmware image;
the class is a facade over singleton radio, decoder, task, and capture state.

## Data flow

1. RadioLib configures a CC1101 or SX127x for direct OOK or FSK reception.
2. A pinned receiver task samples RSSI and decides when a signal starts/ends.
3. The GPIO edge ISR records pulse/gap widths into the active capture buffer.
4. `rtl_433_ESP::loop()` copies a completed buffer under the capture critical
   section and sends the copy to the decoder queue.
5. The pinned decoder task runs the selected upstream demodulators and invokes
   the decoded-message and optional raw-pulse callbacks.

Callbacks execute on the decoder FreeRTOS task, not the Arduino loop task. A
callback must return promptly, must not retain raw-pulse pointers, and must
synchronize access to application data shared with another task.

## Concurrency and ownership

- The ISR owns pulse/gap elements in the active capture buffer.
- The receiver task publishes a completed buffer only when the next ring slot
  is free. Otherwise it drops the newer capture and increments
  `droppedCaptureBuffers`.
- The Arduino loop copies and clears a ready capture inside `captureMux`.
- The decoder queue owns an allocated pulse copy after a successful send. The
  decoder task frees it after callbacks complete.
- Queue saturation drops the incoming copy and increments
  `droppedDecoderQueue`.
- `statusSnapshot()` is the supported way to read a coherent group of runtime
  counters.

Do not infer synchronization from `volatile`: it is retained where needed for
ISR visibility, while ownership transitions use a critical section or queue.

## Lifecycle

`begin()` initializes decoder resources, the radio, and the receiver task and
returns `false` on failure. `initReceiver()` remains as a compatibility wrapper.
`end()` disables the interrupt and releases tasks, queues, pending pulse copies,
and capture storage. The registered upstream decoder configuration is retained
between `end()` and the next `begin()` to avoid rebuilding hundreds of protocol
objects and fragmenting the embedded heap.

Radio configuration errors are logged and returned through `lastError()`; they
do not deliberately busy-loop the processor.

## Resource profile

The full `esp32_lilygo` OOK example compiled on 2026-08-25 with Espressif32
6.1.0 and RadioLib 7.7.1 at approximately 795 KB flash and 33 KB static RAM.
Runtime use is higher because decoder configuration, capture buffers, queues,
and task stacks are allocated at initialization. `MY_DEVICES` remains the main
mechanism for reducing decoder memory and flash requirements.

## Testing boundaries

Portable capture decisions live in `capture_utils.h` and have host tests.
Hardware builds cover representative radio, modulation, and ESP32 variants.
Protocol behavior should be regression-tested with recorded pulse fixtures;
fixtures should contain pulse/gap values and expected decoded JSON, without
depending on physical RF hardware.
