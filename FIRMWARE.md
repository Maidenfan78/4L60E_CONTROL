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

### Base Pressure
- Function of TPS (and optionally gear)
- Conservative mapping preferred

### Shift Bump
- Temporary EPC duty increase
- Duration: ~200–400 ms (tunable)

### Failsafe
- Invalid TPS / VSS → max EPC duty

---

## Torque Converter Clutch (TCC)

### Enable Conditions
- Gear: 3rd or 4th
- Vehicle speed above threshold
- TPS below threshold
- Brake not applied

### Disable Conditions
- Brake input
- Shift in progress
- Over-temperature
- Sensor fault

---

## Safety & Failsafes

- Watchdog timer enabled
- Default output states on reset:
  - Max EPC pressure
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

## Guiding Principles

- Deterministic behavior
- Conservative pressure bias
- No hidden automation
- Transmission safety above comfort
