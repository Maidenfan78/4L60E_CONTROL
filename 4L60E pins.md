
Inputs to GPIO
Pin
Function
L	Temperature Sensor Signal
M	Temperature Sensor GND
N	Pressure Switch A (aka. Input1)
R	Pressure Switch B (aka. Input2)
P	Pressure Switch C (aka. Input3)
n/a	2WD/4WD Input (ground for 4WD)
n/a	Vehicle Speed Sensor (VSS) - has its own 2-pin connector

Outputs from GPIO
Pin
Function
Control	Limits
A	Sol A (1-2/3-4) Switched GND (aka. Output1)	On/Off	20-40 Ohms (0.75 Amps Max.)
B	Sol B (2-3) Switched GND (aka. Output2)	On/Off	20-40 Ohms (0.75 Amps Max.)
C	Pressure Control Solenoid High	Power (nominal +12V)	Switched 12V from harness
D	Pressure Control Switched GND	PWM (293Hz)	0 to 60% (3.5 - 4.6 Ohms at 20°C, 1.1 Amps Max.)
E	+12 Volt Power	High (nominal +12V)	Switched 12V from harness
U	Lock-Up Feel Solenoid Switched GND not always used	PWM (32 Hertz)	10-11.5 Ohms at 20°C
S	3-2 Sol Switched GND (aka. Output3)	PWM (50Hz)	9-14 Ohms at 20°C
T	Torque Converter Clutch Solenoid Switched GND	On/Off	20-40 Ohms (1.5 Amps Max.)


# 4L60-E MegaShift Wiring Reference

This reference is based on the MegaShift 4L60E wiring documentation. :contentReference[oaicite:1]{index=1}

## 1) Solenoid Outputs & Control

The 4L60-E uses multiple solenoids, driven by switched grounds or +12V:

| Function | Wiring | Control Type | Notes |
|---|---|---|---|
| Solenoid A (1-2 / 3-4) | Switched GND | On/Off | 20-40Ω, ~0.75A max :contentReference[oaicite:2]{index=2} |
| Solenoid B (2-3) | Switched GND | On/Off | 20-40Ω max :contentReference[oaicite:3]{index=3} |
| 3-2 Solenoid | Switched GND | PWM (~50Hz) | 9-14Ω :contentReference[oaicite:4]{index=4} |
| Pressure Control (PC) | +12V & Switched GND | PWM (~293Hz) | 3.5-4.6Ω, ~1.1A :contentReference[oaicite:5]{index=5} |
| Torque Converter Clutch (TCC) | Switched GND | On/Off or PWM | 20-40Ω, ~1.5A :contentReference[oaicite:6]{index=6} |
| Lock-Up Feel (LUF) | Switched GND | PWM (~32Hz) | Optional in some versions :contentReference[oaicite:7]{index=7} |

Solenoids share a common +12 V feed on pin E (except PC solenoid which has its own +12 V on pin C). :contentReference[oaicite:8]{index=8}

---

## 2) AMPseal Connector Pinout

| AMPseal Pin | Function |
|---|---|
| 1 | +12V Power |
| 2 | VSS (VR1) |
| 3 | Brake Sense |
| 4 | Paddle DOWN |
| 5 | 2WD/4WD Sense |
| 6 | Pressure Switch B |
| 7 | LED2 (GPO2) |
| 8 | LED3 (GPO3) |
| 9 | LED4 (GPO4) |
| 10 | LED1 (GPO1) |
| 12 | Spare Output2 |
| 15 | Paddle UP |
| 23 | Solenoid A |
| 24 | non-CAN MAP/TPS/MAF |
| 25 | Pressure Switch C |
| 26 | Pressure Switch A |
| 27 | Line Pressure Sensor |
| 28 | Vref (+5V) |
| 30 | Temp Sensor |
| 31 | Speedo Output |
| 32 | TCC Solenoid |
| 33 | PC GND PWM |
| 34 | 3-2 Sol PWM |
| 35 | Solenoid B |
| 17,18,19,20 | Grounds |
| 13,16 | CAN H/L |
:contentReference[oaicite:9]{index=9}

---

## 3) Sensor Inputs

- **VSS (Vehicle Speed Sensor)** — typically VR sensor. :contentReference[oaicite:10]{index=10}  
- **Brake Sense** — ground signal when braking. :contentReference[oaicite:11]{index=11}  
- **Pressure Switches A/B/C** — indicate manual valve position via hydraulic switch block. :contentReference[oaicite:12]{index=12}  
- **Temp Sensor** — transmission temperature. :contentReference[oaicite:13]{index=13}  
- **Line Pressure Sensor (optional)** — 0.5-4.5 V transducer. :contentReference[oaicite:14]{index=14}

---

## 4) Pressure Switch “Manual Valve” Table

| Gear | Switch A | Switch B | Switch C |
|---|---|---|---|
| 1st | 0 | 1 | 1 |
| 2nd | 1 | 1 | 1 |
| 3rd | 1 | 1 | 0 |
| 4th | 1 | 0 | 0 |
Park/Neutral/Reverse also have unique states. :contentReference[oaicite:15]{index=15}

---

## 5) Notes on PC Solenoid Behavior

- PWM duty inversely related to throttle load (higher duty = higher pressure). :contentReference[oaicite:16]{index=16}  
- OEM runs ~293 Hz, 0-60% duty range. :contentReference[oaicite:17]{index=17}

---

## 6) Additional Notes

- 4L60E uses a common ground switch-to-engage style for most solenoids. :contentReference[oaicite:18]{index=18}  
- The PC solenoid has its own +12 V feed pin and is switched on the ground side via PWM. :contentReference[oaicite:19]{index=19}  
- You can use this wiring reference with any standalone controller (like a Teensy-based TCM) to map inputs/outputs correctly.

