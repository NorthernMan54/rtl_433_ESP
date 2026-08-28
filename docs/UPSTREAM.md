# Upstream synchronization

The `rtl_433/` submodule identifies the upstream rtl_433 revision used during a
refresh. Files compiled by the Arduino library are copied into `src/rtl_433`
and `include`; those copies are release artifacts and may contain ESP-specific
adaptations. `contrib` contains local decoder overrides applied after upstream
device files are selected.

For a refresh:

1. Update the `rtl_433` submodule to the intended reviewed commit.
2. Run the device update tooling from `tools` in a clean branch.
3. Reapply or review every local adaptation rather than treating copied files
   as an opaque vendor dump.
4. Run `python3 tools/check_decoder_registry.py --check`.
5. Run the host tests and the full PlatformIO build matrix.
6. Record the upstream commit and refresh date in the README.

The registry checker verifies that generated OOK and FSK assignments are
contiguous and match the generated counts. A mismatch must be fixed by
regeneration; do not hand-edit indexes or count macros independently.

The legacy `update_rtl_433_devices.sh` performs network access and replaces the
copied device directory. Run it only in a clean, reviewable worktree. The
recommended follow-up is to split its fetch and generation operations so the
generator accepts an explicit local upstream checkout and writes to a staging
directory before applying changes.
