#!/usr/bin/env python3
"""Capture and compare CC1101 and local rtl_433 decoder events."""

from __future__ import annotations

import argparse
import csv
import json
import os
import queue
import re
import signal
import statistics
import subprocess
import sys
import tempfile
import threading
import time
from dataclasses import dataclass, field
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

import serial

IDENTITY_KEYS = ("model", "id", "channel", "subtype")
IGNORED_KEYS = {
    "time", "time_ms", "timestamp", "duration", "rssi", "snr", "noise",
    "freq", "frequency", "protocol", "mod", "mic",
}
TUNING_RE = re.compile(r"\b(phase|profile|setting|frequency_mhz|bandwidth_khz|agcctrl[012])=([^ ]+)")


@dataclass
class Event:
    source: str
    received: float
    data: dict[str, Any]
    setting: dict[str, str] = field(default_factory=dict)


def duration_seconds(value: str) -> float:
    match = re.fullmatch(r"([0-9]+(?:\.[0-9]+)?)([smh]?)", value.strip())
    if not match:
        raise argparse.ArgumentTypeError("use seconds or a suffix such as 10m or 2h")
    return float(match.group(1)) * {"": 1, "s": 1, "m": 60, "h": 3600}[match.group(2)]


def parse_targets(values: list[str]) -> dict[str, str]:
    result = {}
    for item in values:
        if "=" not in item:
            raise SystemExit(f"invalid --target {item!r}; expected key=value")
        key, value = item.split("=", 1)
        result[key] = value
    return result


def target_matches(data: dict[str, Any], target: dict[str, str]) -> bool:
    return all(str(data.get(key)) == value for key, value in target.items())


def identity(data: dict[str, Any]) -> tuple[tuple[str, str], ...]:
    return tuple((key, str(data[key])) for key in IDENTITY_KEYS if key in data)


def similarity(left: dict[str, Any], right: dict[str, Any]) -> int:
    keys = (set(left) & set(right)) - IGNORED_KEYS
    return sum(str(left[key]) == str(right[key]) for key in keys)


def packet_key(event: Event) -> str:
    payload = {key: value for key, value in event.data.items() if key not in IGNORED_KEYS}
    return json.dumps(payload, sort_keys=True, default=str)


def deduplicate(events: list[Event], window: float) -> list[Event]:
    """Collapse identical decoder outputs belonging to one RF repeat burst."""
    kept: list[Event] = []
    last_seen: dict[str, float] = {}
    for event in sorted(events, key=lambda item: item.received):
        key = packet_key(event)
        if event.received - last_seen.get(key, float("-inf")) > window:
            kept.append(event)
        last_seen[key] = event.received
    return kept


def correlate(reference: list[Event], cc1101: list[Event], window: float) -> tuple[list[tuple[Event, Event | None]], set[int]]:
    unused = set(range(len(cc1101)))
    matches = []
    for ref in reference:
        ref_identity = identity(ref.data)
        candidates = []
        for index in unused:
            candidate = cc1101[index]
            delta = abs(candidate.received - ref.received)
            if delta <= window and (not ref_identity or identity(candidate.data) == ref_identity):
                candidates.append((delta, -similarity(ref.data, candidate.data), index))
        if candidates:
            index = min(candidates)[2]
            unused.remove(index)
            matches.append((ref, cc1101[index]))
        else:
            matches.append((ref, None))
    return matches, unused


def parse_cc1101(line: str, received: float, setting: dict[str, str]) -> Event | None:
    if "TUNING_START " in line:
        setting.clear()
        setting.update(TUNING_RE.findall(line))
        return None
    if "CC1101_EVENT " in line and " json=" in line:
        encoded = line.split(" json=", 1)[1]
    elif "Received message : " in line:
        encoded = line.split("Received message : ", 1)[1]
    else:
        return None
    try:
        return Event("cc1101", received, json.loads(encoded), dict(setting))
    except json.JSONDecodeError:
        return None


def stream_reader(stream: Any, source: str, messages: queue.Queue[tuple[str, float, str]]) -> None:
    try:
        while True:
            line = stream.readline()
            if not line:
                if source == "serial":
                    continue
                return
            if isinstance(line, bytes):
                line = line.decode("utf-8", "replace")
            messages.put((source, time.time(), line.rstrip("\r\n")))
    except Exception as exc:  # Report failures from daemon reader threads.
        messages.put(("reader_error", time.time(), f"{source}: {exc!r}"))


def active_setting(periods: list[tuple[float, dict[str, str]]], timestamp: float) -> dict[str, str]:
    result: dict[str, str] = {}
    for started, setting in periods:
        if started > timestamp:
            break
        result = setting
    return result


def setting_name(setting: dict[str, str]) -> str:
    return " ".join(f"{key}={value}" for key, value in sorted(setting.items())) or "unlabelled"


def rssi_stats(values: list[float]) -> dict[str, float | None]:
    if not values:
        return {"rssi_min": None, "rssi_median": None, "rssi_max": None}
    return {"rssi_min": round(min(values), 3),
            "rssi_median": round(statistics.median(values), 3),
            "rssi_max": round(max(values), 3)}


def write_reports(output: Path, reference: list[Event], cc1101: list[Event], window: float,
                  periods: list[tuple[float, dict[str, str]]], raw_counts: dict[str, int]) -> dict[str, Any]:
    matches, unused = correlate(reference, cc1101, window)
    matched = sum(candidate is not None for _, candidate in matches)
    summary: dict[str, Any] = {
        "reference_events": len(reference), "cc1101_events": len(cc1101),
        "matched_events": matched, "reference_only": len(reference) - matched,
        "cc1101_only": len(unused),
        "reference_capture_rate_percent": round(100 * matched / len(reference), 2) if reference else None,
        "raw_events_before_deduplication": raw_counts,
    }
    by_setting: dict[str, dict[str, Any]] = {}
    by_device: dict[str, dict[str, Any]] = {}
    for ref, candidate in matches:
        setting = active_setting(periods, ref.received)
        name = setting_name(setting)
        row = by_setting.setdefault(name, {"reference_events": 0, "matched_events": 0,
                                           "reference_rssi": [], "missed_rssi": []})
        row["reference_events"] += 1
        row["matched_events"] += candidate is not None
        if isinstance(ref.data.get("rssi"), (int, float)):
            row["reference_rssi"].append(float(ref.data["rssi"]))
            if candidate is None:
                row["missed_rssi"].append(float(ref.data["rssi"]))

        device = json.dumps(dict(identity(ref.data)), sort_keys=True)
        device_row = by_device.setdefault(device, {"reference_events": 0, "matched_events": 0,
                                                    "reference_rssi": [], "missed_rssi": []})
        device_row["reference_events"] += 1
        device_row["matched_events"] += candidate is not None
        if isinstance(ref.data.get("rssi"), (int, float)):
            device_row["reference_rssi"].append(float(ref.data["rssi"]))
            if candidate is None:
                device_row["missed_rssi"].append(float(ref.data["rssi"]))
    for row in by_setting.values():
        row["capture_rate_percent"] = round(100 * row["matched_events"] / row["reference_events"], 2)
        row.update(rssi_stats(row.pop("reference_rssi")))
        missed = row.pop("missed_rssi")
        row["weakest_missed_rssi"] = round(min(missed), 3) if missed else None
    summary["by_setting"] = by_setting
    for row in by_device.values():
        row["capture_rate_percent"] = round(100 * row["matched_events"] / row["reference_events"], 2)
        row.update(rssi_stats(row.pop("reference_rssi")))
        missed = row.pop("missed_rssi")
        row["weakest_missed_rssi"] = round(min(missed), 3) if missed else None
    summary["by_device"] = dict(sorted(
        by_device.items(), key=lambda item: item[1]["rssi_median"] if item[1]["rssi_median"] is not None else float("inf")
    ))
    with (output / "events.jsonl").open("w", encoding="utf-8") as handle:
        for event in sorted(reference + cc1101, key=lambda item: item.received):
            handle.write(json.dumps({"source": event.source, "received": event.received,
                                     "setting": event.setting, "data": event.data}, sort_keys=True) + "\n")
    with (output / "matches.csv").open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(("reference_time", "cc1101_time", "delta_seconds", "identity", "setting"))
        for ref, candidate in matches:
            writer.writerow((datetime.fromtimestamp(ref.received, timezone.utc).isoformat(),
                             datetime.fromtimestamp(candidate.received, timezone.utc).isoformat() if candidate else "",
                             f"{candidate.received - ref.received:.3f}" if candidate else "",
                             json.dumps(dict(identity(ref.data)), sort_keys=True),
                             json.dumps(candidate.setting, sort_keys=True) if candidate else ""))
    (output / "summary.json").write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    return summary


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--serial", required=True, help="CC1101 ESP32 serial device")
    parser.add_argument("--baud", type=int, default=921600)
    parser.add_argument("--reset", action=argparse.BooleanOptionalAction, default=True,
                        help="reset the ESP32 at capture start (default: true)")
    parser.add_argument("--duration", type=duration_seconds, default=600.0)
    parser.add_argument("--frequency", default="433.92M")
    parser.add_argument("--sample-rate", default="250k")
    parser.add_argument("--gain", default="auto")
    parser.add_argument("--device", default="0", help="RTL-SDR index or serial")
    parser.add_argument("--rtl-433", default="rtl_433", dest="rtl_433")
    parser.add_argument("--match-window", type=float, default=1.5)
    parser.add_argument("--dedupe-window", type=float, default=2.0)
    parser.add_argument("--target", action="append", default=[], metavar="KEY=VALUE")
    parser.add_argument("--output", type=Path)
    args = parser.parse_args()
    target = parse_targets(args.target)
    output = args.output or Path("test-results") / f"cc1101-reference-{datetime.now():%Y%m%d-%H%M%S}"
    output.mkdir(parents=True, exist_ok=False)
    messages: queue.Queue[tuple[str, float, str]] = queue.Queue()
    reference: list[Event] = []
    cc1101_events: list[Event] = []
    setting: dict[str, str] = {}
    periods: list[tuple[float, dict[str, str]]] = []
    capture_log = (output / "capture.log").open("w", encoding="utf-8")

    with tempfile.TemporaryDirectory(prefix="rtl433-empty-config-") as config_dir:
        environment = os.environ.copy()
        environment["XDG_CONFIG_HOME"] = config_dir
        command = [args.rtl_433, "-d", args.device, "-f", args.frequency, "-s", args.sample_rate,
                   "-g", args.gain, "-F", "json", "-M", "level", "-T", str(int(args.duration) + 2)]
        try:
            port = serial.Serial(args.serial, args.baud, timeout=0.5)
            if args.reset:
                port.dtr = False
                port.rts = True
                time.sleep(0.1)
                port.rts = False
                startup_deadline = time.monotonic() + 8
                while time.monotonic() < startup_deadline:
                    line = port.readline().decode("utf-8", "replace").rstrip("\r\n")
                    if "TUNING_START " in line:
                        parsed_setting = dict(TUNING_RE.findall(line))
                        setting.update(parsed_setting)
                        periods.append((time.time(), dict(parsed_setting)))
                        print(f"cc1101 setting: {setting_name(parsed_setting)}")
                        break
            sdr = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                   text=True, bufsize=1, env=environment)
        except Exception as exc:
            print(f"startup failed: {exc}", file=sys.stderr)
            return 2
        assert sdr.stdout is not None and sdr.stderr is not None
        for stream, source in ((sdr.stdout, "rtl_433"), (sdr.stderr, "rtl_log"), (port, "serial")):
            threading.Thread(target=stream_reader, args=(stream, source, messages), daemon=True).start()
        deadline = time.monotonic() + args.duration
        print(f"Capturing for {args.duration:g}s; output={output}")
        try:
            while time.monotonic() < deadline and sdr.poll() is None:
                try:
                    source, received, line = messages.get(timeout=0.5)
                except queue.Empty:
                    continue
                capture_log.write(f"{received:.6f} {source} {line}\n")
                capture_log.flush()
                if source == "reader_error":
                    print(f"reader error: {line}", file=sys.stderr)
                    continue
                if source == "rtl_log":
                    print(f"rtl_433: {line}", file=sys.stderr)
                    continue
                if source == "rtl_433":
                    try:
                        event = Event("rtl_433", received, json.loads(line))
                    except json.JSONDecodeError:
                        event = None
                else:
                    if "TUNING_START " in line:
                        parsed_setting = dict(TUNING_RE.findall(line))
                        if parsed_setting:
                            periods.append((received, parsed_setting))
                            print(f"cc1101 setting: {setting_name(parsed_setting)}")
                    event = parse_cc1101(line, received, setting)
                if event and target_matches(event.data, target):
                    (reference if event.source == "rtl_433" else cc1101_events).append(event)
                    print(f"{event.source}: {dict(identity(event.data))}")
        except KeyboardInterrupt:
            print("Capture stopped; writing partial results.")
        finally:
            port.close()
            if sdr.poll() is None:
                sdr.send_signal(signal.SIGINT)
                try:
                    sdr.wait(timeout=3)
                except subprocess.TimeoutExpired:
                    sdr.terminate()
                    sdr.wait(timeout=3)

    capture_log.close()
    raw_counts = {"rtl_433": len(reference), "cc1101": len(cc1101_events)}
    reference = deduplicate(reference, args.dedupe_window)
    cc1101_events = deduplicate(cc1101_events, args.dedupe_window)
    summary = write_reports(output, reference, cc1101_events, args.match_window, periods, raw_counts)
    print(json.dumps(summary, indent=2, sort_keys=True))
    print(f"Reports written to {output}")
    return 0 if reference else 3


if __name__ == "__main__":
    raise SystemExit(main())
