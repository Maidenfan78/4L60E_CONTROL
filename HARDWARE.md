# Hardware Design – 4L60-E Manual Controller

This document describes the electrical and hardware architecture of the transmission controller.

---

## Core Components

### Real-Time Controller
- **Teensy 4.1**
- 600 MHz ARM Cortex-M7
- 3.3V logic
- Hardware PWM timers
- Interrupt-based VSS capture
- Watchdog support

### Logger / Interface
- **Raspberry Pi Zero 2 W**
- Wi-Fi + Bluetooth
- SSH access
- SD-card based logging
- Optional web UI

---

## Power Architecture

- Vehicle 12V supply
- Automotive-rated buck converter(s)
- Separate regulated outputs:
  - Teensy 4.1 (5V / VIN)
  - Raspberry Pi Zero 2 W (5V)
- Required protections:
  - Reverse polarity protection
  - TVS diode on 12V input
  - Crank brownout tolerance

The Teensy must continue running if the Pi browns out or resets.

---

## Communication

- **Protocol:** UART (3.3V TTL)
- **Baud rate:** TBD (e.g. 115200)
- **Direction:**
  - Teensy → Pi: telemetry
  - Pi → Teensy: validated config updates (optional)

CAN may be added later but is not required.

---

## Transmission Outputs (from Teensy)

All solenoids are driven via **automotive-rated low-side drivers**.

| Output | Function |
|------|---------|
| SSA | Shift Solenoid A |
| SSB | Shift Solenoid B |
| EPC | Line pressure control (PWM) |
| TCC | Torque converter clutch |

> Solenoids must **never** be driven directly from MCU pins.

---

## Required Inputs to the Controller

### Mandatory
- **Range Selector** (1 / 2 / 3 / D)
- **Vehicle Speed Sensor (VSS)**
- **Throttle Position (TPS)**  
  (from MS3, analog 0–5V or CAN)
- **Brake Switch**

### Strongly Recommended
- **Transmission Temperature Sensor**
- **Engine RPM** (from MS3)

### Optional / Future
- Paddle shifters
- Mode selector (street / firm / valet)

---

## Shift Solenoid Truth Table (4L60-E)

| Gear | SSA | SSB |
|-----|-----|-----|
| 1st | ON  | ON  |
| 2nd | OFF | ON  |
| 3rd | OFF | OFF |
| 4th | ON  | OFF |

---

## Hardware Design Notes

- EPC solenoid requires **stable PWM** and sufficient current capacity
- Grounding must be low-impedance and star-routed
- VSS input should be protected and conditioned
- All outputs default to a **safe state** on MCU reset

---

## Safety Philosophy

- Loss of logger/UI must not affect transmission control
- Any invalid sensor state → increased line pressure
- Harsh shifts are preferred over clutch slip

Teensy 4.1 – Pin Assignment Table

4L60-E Manual Mode Transmission Controller

Digital Outputs (Solenoids)
Function	Teensy Pin	Type	Notes
Shift Solenoid A (SSA)	Pin 2	Digital OUT	Low-side driver input
Shift Solenoid B (SSB)	Pin 3	Digital OUT	Low-side driver input
Torque Converter Clutch (TCC)	Pin 4	Digital OUT / PWM (optional)	Usually on/off
EPC / PCS (Line Pressure)	Pin 5	PWM OUT	Hardware PWM, critical

Why these pins

All are on the same GPIO bank

Clean hardware PWM available on Pin 5

Easy to scope during testing

Digital Inputs
Function	Teensy Pin	Type	Notes
Range Selector – 1	Pin 6	Digital IN	Pulled-up, grounded when active
Range Selector – 2	Pin 7	Digital IN	〃
Range Selector – 3	Pin 8	Digital IN	〃
Range Selector – D	Pin 9	Digital IN	〃
Brake Switch	Pin 10	Digital IN	Active high or low (configurable)
Watchdog / Enable (optional)	Pin 11	Digital IN	Optional safety input

Range selector assumed to be decoded into discrete signals
(preferred over analog decoding for reliability).

Analog Inputs
Function	Teensy Pin	ADC	Notes
TPS (from MS3)	A0 (Pin 14)	ADC1	0–5V scaled to 3.3V
Transmission Temp	A1 (Pin 15)	ADC1	NTC with pull-up
Optional Line Pressure Sensor	A2 (Pin 16)	ADC1	Future expansion

⚠️ Important:
Teensy 4.1 is 3.3V max on ADC pins.
Any 0–5V signal must be divided or buffered.

Speed / Timing Inputs
Function	Teensy Pin	Type	Notes
Vehicle Speed Sensor (VSS)	Pin 18	Interrupt IN	VR → conditioner, Hall → direct
Engine RPM (optional)	Pin 19	Interrupt IN	From MS3 tach out

Pins 18/19 are ideal because:

Dedicated interrupt capability

High-speed edge capture

Minimal latency

Communication
Function	Teensy Pins	Interface	Notes
UART to Pi	TX1 (Pin 1)	Serial1 TX	Teensy → Pi
UART to Pi	RX1 (Pin 0)	Serial1 RX	Pi → Teensy
USB	USB Port	USB CDC	Firmware + bench testing

UART is preferred in-vehicle.
USB is bench/debug only.

Power & Ground
Signal	Connection	Notes
VIN	5V Regulated	From automotive buck
GND	Star Ground	Shared with drivers
AGND	Sensor Ground	TPS / temp reference
Default Output States (Fail-Safe)
Output	Reset State	Reason
SSA	OFF	Prevent unintended shift
SSB	OFF	Prevent unintended shift
EPC	MAX DUTY	Protect clutches
TCC	OFF	Prevent stall

These defaults must be enforced in firmware AND hardware pull states.

Pin Usage Summary
Category	Pins Used
Digital Outputs	2, 3, 4, 5
Digital Inputs	6–11
Analog Inputs	14–16
Interrupt Inputs	18, 19
UART	0, 1

Plenty of pins remain free for:

CAN

paddles

mode switch

pressure sensor

diagnostics LEDs