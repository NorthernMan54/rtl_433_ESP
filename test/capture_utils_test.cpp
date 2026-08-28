#include <assert.h>
#include <stdint.h>

#include "capture_utils.h"

int main() {
  using namespace rtl_433_capture;

  assert(elapsedMicros(150, 100) == 50);
  assert(elapsedMicros(0x00000010u, 0xfffffff0u) == 32);
  assert(canPublishToNextBuffer(0));
  assert(!canPublishToNextBuffer(1));
  assert(isCompleteSignal(17, 40001, 16, 40000));
  assert(!isCompleteSignal(16, 40001, 16, 40000));
  assert(!isCompleteSignal(17, 40000, 16, 40000));
  return 0;
}
