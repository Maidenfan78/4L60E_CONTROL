# TODO

## Phase 1 — Core Gear Control (bench testable)

- [ ] Read range selector inputs (1/2/3/D pins)
- [ ] Command SSA/SSB based on selected gear
- [ ] Fixed EPC duty (~10-15% = firm but not max pressure)
- [ ] Serial logging of gear state and inputs
- [ ] Verify failsafe: SSA/SSB OFF on boot = 3rd gear

## Phase 2 — Speed Safety

- [ ] VSS interrupt capture and speed calculation
- [ ] Downshift protection (block 1st/2nd above safe speeds)
- [ ] VSS loss detection → hold current gear, raise pressure

## Phase 3 — Pressure Refinement

- [ ] TPS input scaling (5V → 3.3V divider)
- [ ] TPS-based EPC duty mapping (lower duty at higher load)
- [ ] Shift pressure bump (lower EPC duty during gear change)
- [ ] Configurable pressure tables

## Phase 4 — TCC Control

- [ ] TCC enable only in 3rd/4th gear
- [ ] Minimum speed threshold for TCC
- [ ] Brake switch → immediate TCC unlock
- [ ] TCC inhibit on high TPS / during shifts
- [ ] Delay timer before lockup (prevents cycling)
- [ ] Throttle transient detection → unlock
- [ ] Hysteresis on enable/disable thresholds

## Phase 5 — Hardening

- [ ] Sensor validation (TPS range, VSS sanity)
- [ ] Failsafe escalation paths
- [ ] Watchdog timer integration
- [ ] Transmission temp sensor input (optional)

---

## Hardware

- [x] Finalize solenoid driver schematic and BOM
- [ ] Build driver board prototype
- [ ] Confirm pin mapping against Teensy 4.1 layout
- [ ] Decide VSS conditioning (VR → LM1815 or Hall → direct)
- [ ] Power protection (TVS, reverse polarity, fusing)

## Tooling

- [ ] PlatformIO build/upload script
- [ ] Serial monitor with gear state display
- [ ] Basic logging to SD card (optional)
