# 4L60-E Manual Mode Transmission Controller

Standalone **manual-mode** transmission controller for a GM **4L60-E**, designed to work alongside a **MegaSquirt 3 ECU (MS3 + MS3X)**.

There is **no automatic shifting**.  
All gear changes are commanded directly by the **T-bar range selector (1 / 2 / 3 / D)**.

The system uses:
- **Teensy 4.1** for real-time transmission control
- **Raspberry Pi Zero 2 W** for logging, tuning, and remote access
- **UART** communication between controller and logger. CP2102 USB–TTL adapter

This architecture prioritizes:
- deterministic real-time behavior
- transmission safety
- simplicity and robustness
- extensibility for future features

Pressure switch manifold feedback is strongly recommended as a safety cross-check.

---

## Vehicle / Powertrain

- **Engine:** Holden Alloytec 3.6L V6  
- **ECU:** MegaSquirt 3 + MS3X Expansion  
- **Throttle Body:** Converted from DBW (Drive-by-Wire) to DVC (Drive-by-Cable)  
- **Transmission:** GM 4L60-E (stock valve body)  
- **Shifter:** T-bar (1 / 2 / 3 / D)

---

## Project Goals

- Manual-only gear control
- Stock valve body retained
- Safe EPC (line pressure) control
- Optional torque converter lockup
- Independent transmission operation (no reliance on Pi)
- Logging and configuration via SSH or USB

EPC control assumption for this project: **PWM duty increases EPC current and reduces line pressure**.
Failsafe behavior is **0% duty = max pressure**.

---

## High-Level Architecture

```text
MegaSquirt 3 ECU (TPS / RPM / CAN / Analog)
            |
            v
Teensy 4.1 (TCM / Controller)
- SSA / SSB
- EPC PWM
- TCC enable + TCC PWM
- VSS capture
- Safety + watchdog
            |
            v
          4L60-E

      UART telemetry/config
            |
            v
Raspberry Pi Zero 2 W (Logger/UI)
- SSH
- Logs
- Config
- (optional) UI
```

---

## Documentation Structure

- **README.md** – Project overview (this file)
- **HARDWARE.md** – Electrical architecture, I/O, drivers
- **FIRMWARE.md** – Control logic, state machines, safety rules
- **TODO.md** – Current build plan and next steps
- **CALIBRATION_NOTES.md** – Vehicle-specific sensor calibration data
- **PRESSURE_SWITCH_VERIFICATION.md** – Step-by-step guide to verify pressure switch truth table

---

## Firmware Workspace

Firmware sources and build config live in `firmware/`.

- **PlatformIO** project targeting Teensy 4.1
- Module layout under `firmware/src/` with headers in `firmware/include/`

Build + upload (from repo root):

```bash
pio run -d firmware
pio run -d firmware -t upload
```

---

## Status

Architecture defined.  
Next steps:
- Hardware pin mapping
- Solenoid driver design
- Firmware skeleton

