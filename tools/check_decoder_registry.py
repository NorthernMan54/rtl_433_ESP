#!/usr/bin/env python3
"""Validate generated decoder counts and indexed registry assignments."""

from __future__ import annotations

import argparse
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
HEADER = ROOT / "include" / "rtl_433_devices.h"
DECODER = ROOT / "src" / "signalDecoder.cpp"


def count(name: str, text: str) -> int:
    match = re.search(rf"#\s*define\s+{name}\s+(\d+)", text)
    if not match:
        raise ValueError(f"missing {name} in {HEADER}")
    return int(match.group(1))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true", help="validate committed output")
    parser.parse_args()

    header = HEADER.read_text(encoding="utf-8")
    decoder = DECODER.read_text(encoding="utf-8")
    marker = "// This is a generated fragment from tools/update_rtl_433_devices.sh"
    body = decoder.split(marker, 1)[1]
    body = body.split("if (rtl_433_ESP::ookModulation) {", 1)[1]
    ook_body, fsk_body = body.split("} else {", 1)
    fsk_body = fsk_body.split("// end of fragment", 1)[0]
    pattern = re.compile(r"cfg->devices\[(\d+)\]")
    groups = {
        "OOK": (pattern.findall(ook_body), count("NUMOF_OOK_DEVICES", header)),
        "FSK": (pattern.findall(fsk_body), count("NUMOF_FSK_DEVICES", header)),
    }
    errors: list[str] = []
    for label, (matches, expected) in groups.items():
        indexes = [int(value) for value in matches]
        if indexes != list(range(expected)):
            errors.append(
                f"{label}: expected contiguous indexes 0..{expected - 1}, "
                f"found {len(indexes)} assignments"
            )
    if errors:
        raise SystemExit("\n".join(errors))
    print("decoder registry is internally consistent")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
