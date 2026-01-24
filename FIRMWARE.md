# Firmware Design – Teensy 4.1 TCM

This document describes the firmware logic running on the Teensy 4.1.

---

## Firmware Responsibilities

The Teensy is the authoritative transmission controller.

It is responsible for:
- Gear selection from range input
- Solenoid control (SSA / SSB)
- EPC (line pressure) PWM control
- TCC control
- Downshift protection
- Failsafe handling
- Watchdog monitoring

The firmware runs independently of the Raspberry Pi.

---

## Operating Mode

### Manual Mode Only
- No automatic upshifts or downshifts
- Gear is commanded directly by shifter position

| Shifter | Commanded Gear |
|--------|----------------|
| 1 | 1st |
| 2 | 2nd |
| 3 | 3rd |
| D | 4th |

---

## Core State Machine

### States
- INIT
- IDLE
- HOLD_GEAR
- SHIFTING
- FAILSAFE

### Transitions
- Shifter change → SHIFTING
- Shift complete → HOLD_GEAR
- Sensor fault → FAILSAFE

---

## Shift Execution Logic

1. Detect requested gear change
2. Validate downshift (speed check)
3. Apply EPC shift bump
4. Change SSA / SSB states
5. Hold bump for fixed duration
6. Ramp EPC back to base pressure

---

## EPC (Line Pressure) Control

### Pressure Command Abstraction

Firmware uses a **Pressure Command %** scale internally:

| Pressure Command | Meaning               |
|------------------|-----------------------|
| 0%               | Minimum line pressure |
| 100%             | Maximum line pressure |

This is converted to EPC duty via inversion (GM-style):

```text
Pressure 100% (max) → EPC duty ~5% (near OFF)
Pressure 0% (min)   → EPC duty ~60% (hard ceiling)
```

**Important:** 60% duty is a hard upper limit, not arbitrary. At ~4Ω coil resistance, this limits average current to ~1.1A for thermal and hydraulic protection.

**Why this abstraction?**

- Tuning tables are intuitive (higher load → higher pressure command)
- Inversion happens in one place, not scattered through code
- Matches MegaShift and other controller conventions

### Base Pressure Mapping

| TPS         | Pressure Command |
|-------------|------------------|
| 0% (closed) | ~30%             |
| 25%         | ~40%             |
| 50%         | ~55%             |
| WOT         | ~70%             |

(Values are starting points — tune with a line pressure gauge.)

### Shift Bump

- Temporarily raise pressure command (e.g., +20%)
- Duration: ~200–400 ms (tunable)

### Failsafe

- Invalid TPS / VSS / fault → **100% pressure command** (max pressure, ~0% EPC duty)

---

## Torque Converter Clutch (TCC)

### Enable Conditions
- Gear: 3rd or 4th
- Vehicle speed above threshold
- TPS below threshold
- Brake not applied
- **Delay timer satisfied** (prevents lock/unlock cycling)

### Disable Conditions

- Brake input (immediate unlock)
- Shift in progress
- Over-temperature
- Sensor fault
- Throttle transient (rapid TPS change)

### TCC Design Philosophy

Per FuelTech practice: hysteresis and delays matter more than fancy logic. TCC must never fight EPC or gear changes. Conservative, delayed lockup is industry standard.

---

## Safety & Failsafes

- Watchdog timer enabled
- Default output states on reset:
  - SSA OFF, SSB OFF → 3rd gear (intentional GM limp mode)
  - Max EPC pressure (0% duty)
  - TCC unlocked
- VSS loss:
  - Lock out downshifts
  - Disable TCC
- Over-temp:
  - Disable TCC
  - Increase EPC pressure

---

## Communication with Raspberry Pi

### Telemetry (Teensy → Pi)
- Current gear
- SSA / SSB state
- EPC duty
- TPS
- VSS
- TCC state
- Transmission temperature

### Commands (Pi → Teensy, optional)
- Mode selection
- Configuration updates

All incoming commands are validated before use.

---

## Development Strategy

1. Fixed EPC, fixed gear control
2. Add VSS + downshift protection
3. Add TPS-based EPC mapping
4. Add TCC logic
5. Add logging + configuration

---

## Repo Structure (Firmware)

The firmware is organized as a PlatformIO project under `firmware/`.

- `firmware/src/` – Implementation modules (state machine, EPC, VSS, TCC)
- `firmware/include/` – Public headers and pin map (`board_config.h`)
- `firmware/config/` – Build/runtime config headers

Build + upload:

```bash
pio run -d firmware
pio run -d firmware -t upload
```

---

## Guiding Principles

- Deterministic behavior
- Conservative pressure bias
- No hidden automation
- Transmission safety above comfort
