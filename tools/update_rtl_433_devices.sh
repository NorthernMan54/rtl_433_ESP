#!/bin/sh

set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT_DIR=$(dirname -- "$SCRIPT_DIR")
UPSTREAM_DIR=${1:-"$ROOT_DIR/rtl_433"}
DEVICE_SOURCE="$UPSTREAM_DIR/src/devices"
TARGET_DEVICES="$ROOT_DIR/src/rtl_433/devices"

if [ ! -d "$DEVICE_SOURCE" ]; then
    echo "usage: $0 [local-rtl_433-checkout]" >&2
    echo "missing upstream device directory: $DEVICE_SOURCE" >&2
    exit 2
fi

STAGING_DIR=$(mktemp -d "${TMPDIR:-/tmp}/rtl433-esp-devices.XXXXXX")
trap 'rm -rf "$STAGING_DIR"' EXIT HUP INT TERM

# Include the trailing comma on OOK_PULSE_RZ so it does not also match the
# unsupported OOK_PULSE_RZI modulation.
OOK_MODULATION="OOK_PULSE_PWM|OOK_PULSE_PPM|OOK_PULSE_MANCHESTER_ZEROBIT|OOK_MC_ZEROBIT|OOK_PULSE_DMC|OOK_PULSE_NRZS|OOK_PULSE_PCM|OOK_PULSE_PIWM_DC|OOK_PULSE_PWM_OSV1|OOK_PULSE_RZ,"

FSK_MODULATION="FSK_PULSE_MANCHESTER_ZEROBIT|FSK_PULSE_PCM|FSK_PULSE_PWM"

cd "$SCRIPT_DIR"
rm -f copy.list devices.list decoder.fragment

(cd "$DEVICE_SOURCE" && grep -E "\.name|\.modulation|\.decode_fn|^r_device " ./*.c) |
    awk -f device.awk | awk -F : '{ print $1 }' | sed 's#^\./##' | sort -u > copy.list

# Populate src/rtl_433/device

while IFS= read -r device_file; do
    cp "$DEVICE_SOURCE/$device_file" "$STAGING_DIR/"
done < copy.list

echo "Device decoders updated"

for contribution in "$ROOT_DIR"/contrib/*.c; do
    cp "$contribution" "$STAGING_DIR/"
done

echo "Contrib decoders updated"

# remove non-functional device decoders

# device decoders that place a bitbuffer_t on the stack is causing an overflow or large memory usage
# ie bitbuffer_t databits = {0};
# ie blueline.c - approx 64000 bytes of memory used

# newkaku.c nexa.c proove.c cavius.c current_cost.c ge_coloreffects.c insteon.c m_bus.c oil_standard.c oil_watchman.c tpms_abarth124.c tpms_citroen.c tpms_elantra2012.c tpms_ford.c tpms_jansite.c tpms_jansite_solar.c tpms_pmv107j.c tpms_renault.c tpms_toyota.c

for problematic in blueline.c secplus_v2.c; do
    rm -f "$STAGING_DIR/$problematic"
done

# Replace the generated directory only after staging completed successfully.
find "$TARGET_DEVICES" -maxdepth 1 -type f -name '*.c' -delete
cp "$STAGING_DIR"/*.c "$TARGET_DEVICES/"

echo "Problematic decoders removed"

# create include/rtl_433_devices.h

(cd "$TARGET_DEVICES" && grep -E "\.name|\.modulation|\.decode_fn|^r_device " ./*.c) > devices.list

OOK_COUNT=`cat devices.list | awk -f device.awk | egrep ${OOK_MODULATION} | awk -F\" '{ print $3 }' | awk -F, '{ print $3 }' | wc | awk '{ print $1 }'`
FSK_COUNT=`cat devices.list | awk -f device.awk | egrep ${FSK_MODULATION} |awk -F\" '{ print $3 }' | awk -F, '{ print $3 }' | wc | awk '{ print $1 }'`

echo $OOK_COUNT "OOK Decoders are copied"
echo $FSK_COUNT "OOK Decoders are copied"

cat devices.list | awk -f device.awk | awk -F\" '{ print $3 }' | \
    awk -F, '{ print $3 }' | awk '{ print "  DECL("$1") \\" }' > rtl_433_devices.fragment

echo "  /* Add new decoders here. */" >> rtl_433_devices.fragment

echo "#define NUMOF_OOK_DEVICES ${OOK_COUNT}" >> rtl_433_devices.fragment
echo "#define NUMOF_FSK_DEVICES ${FSK_COUNT}" >> rtl_433_devices.fragment

cat rtl_433_devices.pre rtl_433_devices.fragment rtl_433_devices.post > ../include/rtl_433_devices.h

echo "rtl_433_devices.h created"

# create src/decoder.cpp fragment

echo "  // This is a generated fragment from tools/update_rtl_433_devices.sh" > decoder.fragment

echo "" >> decoder.fragment

echo "if (rtl_433_ESP::ookModulation) {" >> decoder.fragment

cat devices.list | awk -f device.awk | egrep ${OOK_MODULATION} | awk -F\" '{ print $3 }' | \
    awk -F, '{ print $3 }' | awk '{ print "  memcpy(&cfg->devices["NR-1"], &"$1", sizeof(r_device));" }' >> decoder.fragment

echo "} else {" >> decoder.fragment

cat devices.list | awk -f device.awk | egrep ${FSK_MODULATION} | awk -F\" '{ print $3 }' | \
    awk -F, '{ print $3 }' | awk '{ print "  memcpy(&cfg->devices["NR-1"], &"$1", sizeof(r_device));" }' >> decoder.fragment

echo "}" >> decoder.fragment
echo "" >> decoder.fragment
echo "  // end of fragment" >> decoder.fragment

echo
