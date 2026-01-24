# 4L60‑E Electrical & Control Reference  
**Solenoids, Sensors, PWM, and Calibration Notes**

This document is a **supplemental technical reference** for a GM **4L60‑E** automatic transmission, intended to be used alongside wiring/pinout documentation.

It focuses on:
- solenoid electrical characteristics
- PWM requirements
- sensor ranges
- practical control notes for standalone controllers (Teensy, MegaShift, etc.)

---

## 1) Solenoid Electrical Characteristics

| Solenoid | Function | Resistance (Ω) | Typical Current | Control Type |
|--------|---------|----------------|-----------------|-------------|
| Shift Solenoid A (SSA) | 1–2 / 3–4 shifts | 20–40 Ω | ~0.5–0.8 A | On/Off |
| Shift Solenoid B (SSB) | 2–3 shifts | 20–40 Ω | ~0.5–0.8 A | On/Off |
| 3–2 Control Solenoid | Coast downshift assist | 9–14 Ω | ~1.0–1.3 A | PWM |
| EPC / PCS Solenoid | Line pressure control | **3.5–4.6 Ω** | **0.6–1.1 A** | **PWM (critical)** |
| TCC Solenoid | Torque converter lockup | 20–40 Ω | ~1.0–1.5 A | On/Off or PWM |
| LUF (if fitted) | Lockup feel modulation | ~10–15 Ω | ~1.0 A | PWM |

**Notes**
- All solenoids are **ground‑switched** (low‑side control).
- EPC solenoid has the **highest thermal load** and must be heatsinked.
- Resistance varies by year/manufacturer; measure if in doubt.

---

## 2) Recommended PWM Frequencies

| Solenoid | PWM Frequency | Notes |
|-------|---------------|------|
| EPC / PCS | **200–300 Hz** (OEM ≈ 293 Hz) | Higher = heat, lower = noise |
| 3–2 Solenoid | ~50 Hz | Only active during coast downshifts |
| LUF | ~30–40 Hz | Optional / transmission dependent |
| TCC (if PWM) | 20–100 Hz | Often used as on/off |

For EPC (exhaust-bleed type):

- Duty cycle ↑ → line pressure ↓ (exhaust open longer)
- Duty cycle ↓ → line pressure ↑ (exhaust open shorter)
- 0% duty = max pressure (failsafe)

---

## 3) EPC (Line Pressure) Control Guidelines

- EPC duty must **never rise too high under load** (causes low pressure / slip)
- Conservative tuning is preferred:
  - harsh shifts > clutch slip
- Use a **shift pressure bump**:
  - temporarily lower EPC duty during gear changes (raises pressure)
- On fault:
  - force **0% EPC duty** (max pressure)
  - disable TCC

Typical OEM duty range:
- Light load cruise: ~20–35%
- Moderate load: ~35–50%
- Heavy load / WOT: ~50–65%

(Exact values depend on valve body and calibration.)

---

## 4) Transmission Sensors

### 4.1 Vehicle Speed Sensor (VSS)
- Type: usually **2‑wire VR**
- Signal: AC sine wave
- Requires zero‑crossing or comparator conditioning
- Frequency proportional to output shaft speed

Standalone controllers must:
- condition VR properly
- debounce / filter noise
- detect signal loss

---

### 4.2 Transmission Temperature Sensor
- Type: NTC thermistor (GM style)
- Typical resistance:
  - ~100kΩ cold
  - ~2–5kΩ hot
- Used for:
  - TCC inhibit
  - pressure biasing
  - over‑temp protection

---

### 4.3 Line Pressure Sensor (optional)
- Type: 0.5–4.5 V transducer
- Supply: 5 V reference
- Range: commonly 0–300 psi
- Very useful for EPC tuning

---

### 4.4 Pressure Switch Manifold (Manual Valve Position)

The pressure switches report hydraulic state of the manual valve.

| Gear | Switch A | Switch B | Switch C |
|----|----|----|----|
| 1st | 0 | 1 | 1 |
| 2nd | 1 | 1 | 1 |
| 3rd | 1 | 1 | 0 |
| 4th | 1 | 0 | 0 |
| Reverse | 0 | 0 | 1 |
| Neutral / Park | 0 | 0 | 0 |

These can be used as:
- sanity checks
- limp‑mode detection
- diagnostics

---

## 5) TCC (Torque Converter Clutch) Notes

- Lockup only recommended in **3rd and 4th**
- Always unlock:
  - during shifts
  - on brake input
  - at high TPS
  - during faults
- PWM lockup (if used) should ramp gently to avoid shudder

---

## 6) Electrical & Grounding Notes

- Solenoids must have:
  - dedicated high‑current ground returns
  - separate sensor ground
- Avoid shared grounds between:
  - EPC return
  - TPS / temp sensors
- Use star grounding at controller enclosure

---

## 7) Standalone Controller Safety Rules (Recommended)

- Default state on reset:
  - max EPC duty
  - TCC off
- Loss of VSS:
  - inhibit downshifts
  - inhibit TCC
- Over‑temperature:
  - disable TCC
  - raise line pressure
- MCU watchdog enabled at all times

---

## 8) Practical Tuning Advice

- Start with **fixed high pressure**
- Verify correct gear engagement
- Add TPS‑based pressure mapping
- Add shift pressure bump
- Enable TCC last

---

## 9) Intended Use

This document is intended to support:
- Teensy‑based TCM projects
- MegaShift / GPIO‑style controllers
- Manual‑mode or paddle‑shift 4L60‑E builds

Improper control of automatic transmissions can cause damage or injury.  
Proceed carefully and test incrementally.
