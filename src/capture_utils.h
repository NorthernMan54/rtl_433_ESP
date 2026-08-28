#ifndef RTL_433_ESP_CAPTURE_UTILS_H
#define RTL_433_ESP_CAPTURE_UTILS_H

#include <stdint.h>

namespace rtl_433_capture {

// Unsigned subtraction intentionally preserves Arduino micros() rollover.
constexpr uint32_t elapsedMicros(uint32_t end, uint32_t start) {
  return end - start;
}

// A completed capture may only advance into an unowned ring slot.
constexpr bool canPublishToNextBuffer(unsigned int nextPulseCount) {
  return nextPulseCount == 0;
}

constexpr bool isCompleteSignal(unsigned int pulseCount, uint32_t duration,
                                unsigned int minimumPulses,
                                uint32_t minimumDuration) {
  return pulseCount > minimumPulses && duration > minimumDuration;
}

} // namespace rtl_433_capture

#endif
