# TODO

## Phase 1 — Core Gear Control (bench testable)

- [ ] Read range selector inputs (1/2/3/D pins)
- [ ] Command SSA/SSB based on selected gear
- [ ] Fixed EPC duty (~10-15% = firm but not max pressure)
- [x] Serial logging of gear state and inputs (implemented: temp + pressure switches)
- [ ] Verify failsafe: SSA/SSB OFF on boot = 3rd gear
- [x] **Trans temp sensor:** Firmware implemented with lookup table
- [x] **Pressure switches:** Firmware implemented (OEM truth table, needs verification)

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
- [ ] TCC PWM progressive apply (ramp duty on lockup request)
- [ ] TCC PWM unlock sequence (PWM to 0% before enable OFF)

## Phase 5 — Hardening

- [ ] Sensor validation (TPS range, VSS sanity)
- [ ] Failsafe escalation paths
- [ ] Watchdog timer integration
- [ ] Transmission temp sensor input (optional)

---

## Hardware

- [x] Finalize solenoid driver schematic and BOM
- [x] **Brake switch verified** (+12V-switched, schematic is correct)
- [ ] **Select connectors** (Deutsch DTM or Amphenol Superseal — NOT GX20)
- [ ] Build driver board prototype
- [ ] Confirm pin mapping against Teensy 4.1 layout
- [ ] Decide VSS conditioning (VR → LM2903N comparator or Hall → direct)
- [ ] Power protection (TVS, reverse polarity, fusing)

## Tooling

- [ ] PlatformIO build/upload script
- [ ] Serial monitor with gear state display
- [ ] Basic logging to SD card (optional)

---

## Calibration (See CALIBRATION_NOTES.md & PRESSURE_SWITCH_VERIFICATION.md)

- [x] **Trans temp sensor:** OEM resistance values confirmed (3500Ω @ 20°C, 178Ω @ 100°C)
- [x] **Pressure switches:** OEM truth table confirmed (MegaShift documentation)
- [x] **Brake switch:** Confirmed +12V-switched (schematic correct)
- [ ] **Trans temp ADC:** Bench test ADC values at room temp, verify lookup table
- [ ] **Pressure switches:** ⚠️ CRITICAL - Physical verification on YOUR trans (see PRESSURE_SWITCH_VERIFICATION.md)
- [ ] **Line pressure sensor:** Install mechanical gauge, calibrate ADC vs psi (optional but recommended)
- [ ] **EPC pressure table:** Tune with line pressure gauge on dyno/test road
- [ ] **Downshift limits:** Calculate based on engine redline + gear ratios
- [ ] **TCC thresholds:** Tune lockup speed, delay, unlock conditions
