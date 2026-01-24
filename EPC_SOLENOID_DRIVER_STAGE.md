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

EPC current is typically 0.6–1.1 A, continuous PWM.

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

Infineon BSC010NE2LS5

40V

Logic-level gate

Very low Rds(on)

Automotive qualified

Good alternatives:

IRLZ44N (old but works)

AOZ1284

IPB044N06L3

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
DO NOT use simple diodes for EPC

A diode slows current decay → mushy shifts.

Correct approach

Bidirectional TVS diode across solenoid:

Recommended:

SMBJ36CA (36V clamp)

SMCJ36CA (higher power)

Connection:

Drain ─── TVS ─── +12V


This allows:

fast current decay

controlled voltage spike

crisp solenoid release

This is OEM-style suppression.

7️⃣ EPC PWM specifics (very important)
PWM frequency

Recommended: 200–300 Hz

Matches GM PCS behavior

Avoids audible noise

Avoids heating the solenoid

Teensy hardware PWM handles this perfectly.

Default EPC behavior (hardware + firmware)

Gate pulldown ensures OFF if MCU dies

Firmware sets MAX duty on fault

Solenoid supply is always present

8️⃣ Current handling & thermal design
EPC MOSFET

Continuous PWM = heat

Mount MOSFET to:

copper pour on PCB or

small aluminum heatsink

On/off solenoids

Lower duty

No heatsink usually needed

9️⃣ Protection on the 12V supply (mandatory)
At the driver board input:

TVS diode: SMBJ58A (load dump protection)

Reverse polarity protection

P-channel MOSFET OR

Schottky diode (less ideal)

Decoupling

100 µF electrolytic near solenoids

0.1 µF ceramic per channel

🔟 Full driver channel BOM (per solenoid)
Part	Value
MOSFET	Infineon BSC010NE2LS5
Gate resistor	100 Ω
Gate pulldown	100 kΩ
Flyback TVS	SMBJ36CA
Decoupling cap	0.1 µF
Bulk cap	100 µF (shared rail)
1️⃣1️⃣ Wiring summary (Teensy → Driver)
Signal	Teensy	Driver
SSA	Pin 2	Gate via 100Ω
SSB	Pin 3	Gate via 100Ω
TCC	Pin 4	Gate via 100Ω
EPC	Pin 5 (PWM)	Gate via 100Ω
GND	GND	Power ground
+12V	Vehicle	Solenoid feed
1️⃣2️⃣ Fail-safe behavior (designed in)
Failure	Result
MCU reset	All solenoids OFF
EPC fault	Firmware forces MAX duty
Pi crash	No effect
Sensor loss	MAX pressure, no TCC
Brownout	Gate pulldowns force OFF
1️⃣3️⃣ What this design deliberately avoids

❌ Relay switching (too slow)

❌ ULN2003 / Darlington arrays (voltage drop, heat)

❌ Flyback diodes only (bad shift quality)

❌ Direct MCU drive (dangerous)

Next steps (pick one)