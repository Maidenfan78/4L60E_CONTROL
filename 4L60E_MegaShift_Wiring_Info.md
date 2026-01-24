# 4L60-E MegaShift Wiring & Pinout Reference

This document is derived from the MegaShift 4L60-E wiring documentation:
https://www.msgpio.com/manuals/mshift/wiring.html

It consolidates all wiring, pinout, and signal information relevant to building
a standalone transmission controller (e.g. Teensy-based TCM).

---

## 1) Solenoid Outputs

All solenoids are **low-side (ground-switched)** unless noted.

| Solenoid | Function | Control | Resistance | Notes |
|--------|---------|---------|------------|------|
| Shift Solenoid A (SSA) | 1–2 / 3–4 shifts | On/Off | 20–40 Ω | Low-side switched |
| Shift Solenoid B (SSB) | 2–3 shifts | On/Off | 20–40 Ω | Low-side switched |
| EPC / PCS | Line pressure | PWM | 3.5–4.6 Ω | Separate +12V feed |
| TCC | Converter lockup | On/Off or PWM | 20–40 Ω | Low-side switched |
| 3–2 Solenoid | Coast downshift | PWM | 9–14 Ω | Optional use |
| LUF (if fitted) | Lockup feel | PWM | ~10–15 Ω | Some variants only |

---

## 2) Solenoid Power Feeds

- **Common +12V feed** for SSA, SSB, TCC, 3–2, LUF
- **Dedicated +12V feed** for EPC solenoid
- All solenoids are activated by **grounding the control pin**

---

## 3) AMPseal Connector Pinout (MegaShift Reference)

| Pin | Function |
|----|---------|
| 1 | +12V Power |
| 2 | VSS (VR+) |
| 3 | Brake Switch |
| 4 | Paddle Down |
| 5 | 2WD/4WD Sense |
| 6 | Pressure Switch B |
| 7 | LED2 / GPO2 |
| 8 | LED3 / GPO3 |
| 9 | LED4 / GPO4 |
| 10 | LED1 / GPO1 |
| 12 | Spare Output 2 |
| 13 | CAN H |
| 15 | Paddle Up |
| 16 | CAN L |
| 17 | Ground |
| 18 | Ground |
| 19 | Ground |
| 20 | Ground |
| 23 | Shift Solenoid A |
| 24 | MAP / TPS / MAF (non-CAN) |
| 25 | Pressure Switch C |
| 26 | Pressure Switch A |
| 27 | Line Pressure Sensor |
| 28 | +5V Reference |
| 30 | Transmission Temp Sensor |
| 31 | Speedometer Output |
| 32 | TCC Solenoid |
| 33 | EPC PWM Ground |
| 34 | 3–2 Solenoid |
| 35 | Shift Solenoid B |

---

## 4) Sensor Inputs

### Vehicle Speed Sensor (VSS)
- Typically **2-wire VR sensor**
- Requires conditioning (zero-cross or comparator)
- Frequency proportional to output shaft speed

### Brake Switch
- Active when braking
- Used to unlock TCC immediately

### Transmission Temperature
- GM NTC thermistor
- Used for TCC inhibit and over-temp protection

### Line Pressure Sensor (optional)
- 0.5–4.5V transducer
- Requires +5V reference

---

## 5) Pressure Switch Manifold (Manual Valve)

| Gear | Switch A | Switch B | Switch C |
|----|----|----|----|
| 1st | 0 | 1 | 1 |
| 2nd | 1 | 1 | 1 |
| 3rd | 1 | 1 | 0 |
| 4th | 1 | 0 | 0 |
| Reverse | 0 | 0 | 1 |
| Park / Neutral | 0 | 0 | 0 |

Used for diagnostics and sanity checking.

---

## 6) EPC Control Notes

- PWM frequency ≈ **293 Hz** (OEM)
- Higher duty = **lower** line pressure (exhaust-bleed solenoid)
- 0% duty = max pressure (failsafe)
- Must default to **high pressure on fault** (0% duty)

---

## 7) Standalone Controller Notes

- All solenoids are ground-switched
- Use external drivers (MOSFETs)
- Separate solenoid ground and sensor ground
- Star-ground at controller enclosure

---

## 8) Intended Use

This document is intended as a **technical wiring reference**
for standalone controllers such as:
- Teensy-based TCMs
- MegaShift / GPIO controllers
- Manual-mode or paddle-shift 4L60-E builds

Always test incrementally.
