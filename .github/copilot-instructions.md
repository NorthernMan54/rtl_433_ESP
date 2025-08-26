# Copilot Instructions for rtl_433_ESP

## Repository Summary

ESP32 Arduino library porting rtl_433 433MHz RF protocol decoder for ESP32 microcontrollers with RF transceivers (CC1101, SX1276, SX1278). Decodes 100+ protocols from weather stations, sensors, IoT devices. Integrates with OpenMQTTGateway.

**Type**: Arduino library | **Languages**: C/C++, Shell | **Build**: PlatformIO | **Platform**: ESP32 | **Dependencies**: RadioLib ^5.3.0

## Build Instructions

**CRITICAL**: Install PlatformIO first: `pip install platformio`

**Building**: Always use example project for validation:
```bash
cd example/OOK_Receiver
platformio run                    # Build default (esp32_lilygo)
platformio run -e esp32_cc1101    # Build specific environment
```

**Environments**: esp32_lilygo (default), esp32_cc1101, esp32_heltec, esp32c3_cdc_cc1101, esp32doitv1_r01
**Build Time**: 5-10 minutes initial (dependencies + compilation)

**Linting**: `clang-format -i src/filename.cpp` (only `src/` root and `include/rtl_433_devices.h`, `include/log.h`)
**NEVER** format `src/rtl_433/` files (upstream ported code)

**Testing**: No unit tests. Validation through compilation + runtime testing
**Update Decoders**: `cd tools && ./update_rtl_433_devices.sh` (syncs with upstream rtl_433)

## Project Layout

**Core Files:**
```
src/rtl_433_ESP.cpp/.h      # Main library API
src/receiver.cpp/.h         # RadioLib RF module wrapper  
src/signalDecoder.cpp/.h    # rtl_433 decoder integration
src/rtl_433/               # Ported rtl_433 source (DO NOT EDIT)
├── devices/               # Protocol decoders (~150 files)  
├── bitbuffer.c           # Signal processing
└── data.c                # Data structures
```

**Config:** `library.json` (manifest), `example/OOK_Receiver/platformio.ini` (build config), `.clang-format`
**Generated:** `include/rtl_433_devices.h` (device registry, auto-generated via tools)
**Tools:** `tools/update_rtl_433_devices.sh` (sync upstream), `tools/device.awk` (parser)
**CI:** `.github/workflows/build.yml` (builds), `.github/workflows/lint.yml` (formatting)

## Critical Configuration

**RF Module Selection** (in `platformio.ini`):
- `-DRF_CC1101="CC1101"` (CC1101 transceiver)
- `-DRF_SX1276="SX1276"` (SX1276 LoRa) 
- `-DRF_SX1278="SX1278"` (SX1278 LoRa)

**Modulation**: `-DOOK_MODULATION=true` (OOK ~100 devices) / `=false` (FSK ~15 devices)

**Debug Flags**: `-DRTL_DEBUG=4` (verbose), `-DDEMOD_DEBUG=true` (signals), `-DMEMORY_DEBUG=true` (heap)

**Hardware Pins** (ESP32 defaults: SCK-18, MISO-19, MOSI-23, CS-5):
- CC1101: `-DRF_MODULE_GDO0=22`, `-DRF_MODULE_GDO2=4`
- SX127X: `-DRF_MODULE_DIO0=26`, `-DRF_MODULE_RST=14`

## Development Workflow

**Code Changes:**
1. **NEVER edit `src/rtl_433/`** (upstream managed)
2. Edit `src/` root, `include/`, `example/` only
3. Format with clang-format before commit
4. Test build with example project

**Add Device Decoders:** Add to `contrib/` → run `tools/update_rtl_433_devices.sh` → test

**Common Issues:**
- Memory constraints (some decoders excluded)
- Symbol conflicts (ESPiLight vs silvercrest)
- SPI config (custom pin definitions needed)
- Network timeouts (retry PlatformIO builds)

**Maintenance:** Update `library.json` version, sync upstream periodically, monitor ESP32 memory limits

**Trust these instructions** - comprehensive and validated. Only search if incomplete/incorrect.