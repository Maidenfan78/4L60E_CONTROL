EPC + Solenoid Driver Stage

4L60-E | Teensy 4.1 | Automotive-safe

1️⃣ Design philosophy (important)

Low-side switching (OEM-correct for GM)

MCU never drives solenoids directly

EPC must survive continuous PWM

Fail-safe bias = MAX pressure

Flyback suppression that won’t kill MOSFETs

Survives load dump, inductive kick, noise

2️⃣ Solenoids to drive
Solenoid	Type	Control
SSA	On/Off	Digital
SSB	On/Off	Digital
TCC	On/Off (or PWM optional)	Digital
EPC / PCS	PWM, current-controlled-ish	Critical

EPC current range: **0–1.1 A** (0A = max pressure, ~1.1A = min pressure), continuous PWM.

3️⃣ Recommended topology (one channel)

All solenoids use the same base topology, EPC just needs better thermal handling.

+12V ---- Solenoid ---- Drain MOSFET Source ---- GND
                      |
                      +-- Flyback clamp (TVS)
Gate ← Gate resistor ← Teensy pin
         |
        Gate pulldown

4️⃣ MOSFET selection (do NOT cheap out)
Recommended MOSFET (proven automotive choice)

**STP36NF06L** (ST Microelectronics)

- 60V VDS
- 30A ID (continuous)
- 40mΩ RDS(on) max @ VGS=10V
- Logic-level gate (VGS(th) 1.0–2.5V, 3.3V compatible)
- Low gate charge (13–17nC)
- **AEC-Q101 automotive qualified**
- TO-220 package (hand-solderable)

Good alternatives:

- Infineon BSC010NE2LS5 (SMD)
- IPB044N06L3

⚠️ Avoid random eBay MOSFETs.

5️⃣ Gate drive network (critical for noise immunity)
Gate components (per solenoid)
Component	Value	Purpose
Gate resistor	100 Ω	Limits inrush, reduces ringing
Gate pulldown	100 kΩ	Ensures OFF at reset
MCU pin	3.3V	Teensy compatible

Why this matters

Prevents accidental turn-on during reset

Controls EMI

Improves MOSFET longevity

6️⃣ Flyback protection (THIS IS NOT OPTIONAL)

### For SSA / SSB / TCC (on/off solenoids)

Use **bidirectional TVS** across solenoid:

- Recommended: **5KP36CA** (36V clamp, 5000W peak pulse, axial) or SMBJ36CA (SMD)
- Connection: Drain ─── TVS ─── +12V

This allows:

- fast current decay
- controlled voltage spike
- crisp solenoid release

### For EPC (PWM pressure control)

Use a **diode** (e.g., **1N4001** or 1N4007) across the EPC solenoid:

- Connection: Cathode to +12V, Anode to Drain
- The slower current decay acts as a **low-pass filter** on pressure
- Results in **smoother pressure modulation**

This matches MegaShift recommendations for "greater line pressure control."

Optionally add a small TVS (5KP36CA) in parallel for spike protection.

| Solenoid | Flyback Type | Reason                        |
|----------|--------------|-------------------------------|
| SSA      | TVS          | Fast response for shifts      |
| SSB      | TVS          | Fast response for shifts      |
| TCC      | TVS          | Crisp lockup engagement       |
| EPC      | Diode (+TVS) | Smooth pressure modulation    |

7️⃣ EPC PWM specifics (very important)
PWM frequency

Recommended: 200–300 Hz

Matches GM PCS behavior

Avoids audible noise

Avoids heating the solenoid

Teensy hardware PWM handles this perfectly.

Default EPC behavior (hardware + firmware)

Gate pulldown ensures OFF if MCU dies

Firmware sets **0% duty on fault** (max pressure, project assumption)

Solenoid supply is always present

8️⃣ EPC direction and pressure abstraction

**Hardware behavior (GM 4L60E):**

- Higher EPC duty → more current → exhaust open longer → **lower** line pressure
- Lower EPC duty → less current → exhaust closed more → **higher** line pressure
- 0% duty = max pressure (failsafe)

**Firmware abstraction (recommended):**

Use a **Pressure Command %** scale (0–100%, where 100% = max pressure).
Convert to EPC duty via inversion in one place:

```text
pressure_cmd 100% → EPC duty ~5%  (max pressure)
pressure_cmd 0%   → EPC duty ~60% (min pressure, hard ceiling)
```

**Important:** 60% duty is a hard upper limit — not comfort tuning, but thermal + hydraulic protection. At ~4Ω coil, this limits average current to ~1.1A.

This keeps tuning tables intuitive (higher TPS → higher pressure command).

9️⃣ Current handling & thermal design
EPC MOSFET

Continuous PWM = heat

Mount MOSFET to:

copper pour on PCB or

small aluminum heatsink

On/off solenoids

Lower duty

No heatsink usually needed

10️⃣ Protection on the 12V supply (mandatory)
At the driver board input:

TVS diode: SMBJ58A (load dump protection)

Reverse polarity protection

P-channel MOSFET OR

Schottky diode (less ideal)

Decoupling

100 µF electrolytic near solenoids

0.1 µF ceramic per channel

11️⃣ Full driver channel BOM (per solenoid)
Part	Value
MOSFET	STP36NF06L (TO-220)
Gate resistor	100 Ω
Gate pulldown	100 kΩ
Flyback TVS	5KP36CA (axial)
Decoupling cap	0.1 µF
Bulk cap	100 µF (shared rail)
12️⃣ Wiring summary (Teensy → Driver)
Signal	Teensy	Driver
SSA	Pin 2	Gate via 100Ω
SSB	Pin 3	Gate via 100Ω
TCC	Pin 4	Gate via 100Ω
EPC	Pin 5 (PWM)	Gate via 100Ω
GND	GND	Power ground
+12V	Vehicle	Solenoid feed
13️⃣ Fail-safe behavior (designed in)
Failure	Result
MCU reset	SSA/SSB default to 3rd (both OFF — intentional GM limp mode), TCC OFF, EPC 0% duty
EPC fault	Firmware forces 0% duty (max pressure)
Pi crash	No effect
Sensor loss	MAX pressure, no TCC
Brownout	Gate pulldowns force OFF
14️⃣ What this design deliberately avoids

❌ Relay switching (too slow)

❌ ULN2003 / Darlington arrays (voltage drop, heat)

❌ Flyback diodes only (bad shift quality)

❌ Direct MCU drive (dangerous)

Next steps (pick one)
