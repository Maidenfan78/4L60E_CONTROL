# Input Conditioning Circuits

All outputs go to Teensy 4.1 (3.3V absolute max on GPIO).
For external signals, use divider + series resistor + Schottky clamp diodes to keep pins safe.
Automotive voltage range: 10V cranking, 12.6V resting, 14.7V charging (design for 15V worst case).

## Diagrams (SVG)

- `kicad_4l60e/input_diagrams/tps_input.svg`
- `kicad_4l60e/input_diagrams/brake_input.svg`
- `kicad_4l60e/input_diagrams/trans_temp_input.svg`
- `kicad_4l60e/input_diagrams/line_pressure_input.svg`
- `kicad_4l60e/input_diagrams/rpm_input.svg`
- `kicad_4l60e/input_diagrams/range_selector_input.svg`
- `kicad_4l60e/input_diagrams/pressure_switches_input.svg`
- `kicad_4l60e/vss_input_schematic.svg` (VR VSS)

---

## TPS (Pin A0) - Throttle Position Sensor, 0-5V

```
TPS_IN (0-5V) ---[15k]---+---[24k]--- GND
                          |
                          +---[0.1uF]--- GND
                          |
                          +---[1k]---+--- MCU_TPS_A0
                                   |
                                 [D1] 1N5817 (clamp to GND)
                                   |
                                  GND
                                   |
                                 [D2] 1N5817 (clamp to +3.3V)
                                   |
                                 +3.3V
```
Divider: 24k/(24k+15k) = 0.615 --> 5V in = 3.08V out, 5.25V = 3.23V
Filter: ~100Hz cutoff with 10k and 0.1uF

---

## Brake Switch (Pin 10) - 12V Switched

One side of switch wired to +12V (battery/ignition), other side to TCM input.

```
+12V --- [brake switch] ---[47k]---+---[12k]--- GND
                                   |
                                   +---[1k]---+--- MCU_BRAKE_D10
                                             |
                                           [D3] 1N5817 (clamp to GND)
                                             |
                                            GND
                                             |
                                           [D4] 1N5817 (clamp to +3.3V)
                                             |
                                           +3.3V
```
Divider: 12k/(12k+47k) = 0.203

| Condition              | Vin   | MCU sees | Logic |
|------------------------|-------|----------|-------|
| Brake OFF              | 0V    | 0V       | LOW   |
| Brake ON (cranking)    | 10V   | 2.03V    | HIGH  |
| Brake ON (running)     | 14.7V | 2.98V    | HIGH  |
| Brake ON (worst case)  | 15V   | 3.05V    | HIGH  |

All values under 3.3V max. Cranking still reads as solid HIGH.

---

## Trans Temp (Pin A1) - NTC Thermistor

```
+3.3V
  |
[3.3k]  pullup
  |
  +---[0.1uF]--- GND
  |
  +--- MCU_TTEMP_A1
  |
 NTC (from trans)
  |
 GND
```
3.3k pullup + NTC forms voltage divider.
Cold = high resistance = high voltage. Hot = low resistance = low voltage.

Connector (2-pin): Pin 1 = NTC signal, Pin 2 = GND

---

## Line Pressure (Pin A2) - Optional Pressure Transducer

Sensor is 5V powered, outputs 0.5-4.5V. 4.5V exceeds 3.3V max so a divider is needed.

```
+5V ------------ J_LPRESS Pin 1 (sensor power)

Sensor signal --- J_LPRESS Pin 2 ---[15k]---+---[24k]--- GND
(0.5-4.5V)                                  |
                                             +---[0.1uF]--- GND
                                             |
                                             +---[1k]---+--- MCU_LPRESS_A2
                                                       |
                                                     [D5] 1N5817 (clamp to GND)
                                                       |
                                                      GND
                                                       |
                                                     [D6] 1N5817 (clamp to +3.3V)
                                                       |
                                                     +3.3V

GND ------------ J_LPRESS Pin 3 (sensor ground)
```
Divider: 24k/(24k+15k) = 0.615

| Sensor output | MCU sees |
|---------------|----------|
| 0.5V (0 psi)  | 0.31V   |
| 2.5V (mid)    | 1.54V   |
| 4.5V (max)    | 2.77V   |
| 5.0V (fault)  | 3.08V   |

All values under 3.3V max. Filter: ~100Hz cutoff with 10k and 0.1uF.

Connector (3-pin): Pin 1 = +5V, Pin 2 = Signal, Pin 3 = GND

---

## RPM (Pin 19) - Tach from MegaSquirt 3

MS3 tach output is a 0-12V square wave (battery-level). Use a voltage divider to bring it under 3.3V,
then clamp diodes as backup protection.

```
RPM_IN (0-12V sq) ---[47k]---+---[12k]--- GND
                              |
                              +---[1k]---+--- MCU_RPM_D19
                                        |
                                      [D7] 1N5817 (clamp to GND)
                                        |
                                       GND
                                        |
                                      [D8] 1N5817 (clamp to +3.3V)
                                        |
                                      +3.3V
```
Divider: 12k/(12k+47k) = 0.203 --> 12V in = 2.44V out
Schottky clamp diodes are backup only - divider does the real work.
Square wave at ~2.4V is a solid logic HIGH.

Connector (2-pin): Pin 1 = RPM signal, Pin 2 = GND

---

## Range Selector (Pins 6/7/8/9) - T-Bar 1/2/3/D

Each range position has its own wire from the TCM to the switch.
All switches share a common ground on the other side.
Selecting a gear grounds that wire.

```
+3.3V  +3.3V  +3.3V  +3.3V
  |      |      |      |
[10k]  [10k]  [10k]  [10k]   external pullups
  |      |      |      |
  +------+------+------+----- J_RANGE Pin 5 (common GND on switch side)
  |      |      |      |             |
  |      |      |      |           (GND)
  |      |      |      |
  |      |      |      +--- MCU_RNGD_D9  (Drive/4th)
  |      |      +--- MCU_RNG3_D8         (3rd)
  |      +--- MCU_RNG2_D7                (2nd)
  +--- MCU_RNG1_D6                       (1st)
```

Better view - each channel is identical:

```
+3.3V ---[10k]---+--- MCU_RNGx
                  |
           Range switch (normally open)
                  |
                 GND (common ground at switch)
```

- Switch OPEN  = 10k pulls pin HIGH (3.3V) = not selected
- Switch CLOSED = pin pulled to GND          = gear selected
- Only one position active at a time

Connector (5-pin): Pin 1 = Gear 1, Pin 2 = Gear 2, Pin 3 = Gear 3, Pin 4 = Drive, Pin 5 = Common GND

---

## Pressure Switches A/B/C (Pins 20/21/22) - Gear Confirmation

Same topology as range selector. Trans internal switches are ground-switched.

```
+3.3V ---[10k]---+--- MCU_PSW_x
                  |
           Pressure switch (inside trans)
                  |
                 GND (common ground)
```

- Switch OPEN  = HIGH = pressure not present
- Switch CLOSED = LOW  = pressure present

Connector (4-pin): Pin 1 = PSW_A, Pin 2 = PSW_B, Pin 3 = PSW_C, Pin 4 = Common GND

---

## VSS (Pin 18) - Variable Reluctance Speed Sensor with LM2903P

```
                    +3.3V             +3.3V
                     |                   |
                    [10k] bias        [10k] pullup
                     |                   |
VR sensor ---[0.1uF]-+---[D2]---+3.3V    |
  (AC)               |              LM2903P OUT (Pin 1) ---+--- MCU_VSS_D18
                     +---[D1]---GND     |                  |
                     |                  |                [0.1uF] C4
                   [10k] bias       [100k]                 |
                     |              hysteresis             GND
                    GND                |
                     |                 |
                     +--- LM2903P IN(+) Pin 3 -+
                     |
                     +--- LM2903P IN(-) Pin 2 --- same bias point (1.65V)

                    +3.3V (Pin 8)
                       |
                     [0.1uF] C5 (decoupling cap, close to IC)
                       |
                      GND (Pin 4)
```

**LM2903P DIP-8 Pinout:**

- Pin 1: Output (comparator 1) → to MCU via pullup
- Pin 2: IN(-) → 1.65V reference
- Pin 3: IN(+) → biased VR signal
- Pin 4: GND
- Pin 5: IN(+) comparator 2 (NC)
- Pin 6: IN(-) comparator 2 (NC)
- Pin 7: Output comparator 2 (NC)
- Pin 8: VCC (+3.3V)

**Step by step:**

1. **AC coupling**: C3 = 0.1uF blocks DC from VR sensor, passes AC signal
2. **Clamp diodes**: D1 (1N4148) to GND, D2 (1N4148) to +3.3V - limits swing
3. **Bias network**: Two 10k resistors, one to +3.3V and one to GND = 1.65V DC bias
4. **Comparator (-) input (Pin 2)**: Tied to same 1.65V bias point (reference)
5. **Comparator (+) input (Pin 3)**: Biased VR signal - swings above/below 1.65V
6. **Hysteresis**: R9 = 100k from output back to (+) input prevents chatter
7. **Output pullup**: R10 = 10k to +3.3V (LM2903 is open-collector, needs pullup)
8. **Output filter**: C4 = 0.1uF on output cleans up edges
9. **Power**: Pin 8 to +3.3V, Pin 4 to GND, C5 = 0.1uF decoupling cap between them

Connector (2-pin): Pin 1 = VR signal, Pin 2 = VR return / GND

For Hall-effect VSS: skip this circuit, just use 10k pullup to 3.3V directly.

---

## Component Summary

| Part   | Value   | Qty | Used In                                                                           |
|--------|---------|-----|-----------------------------------------------------------------------------------|
| R      | 1k      | 4   | Series resistors for TPS, RPM, line pressure, brake                               |
| R      | 10k     | 10  | Range (x4), pressure sw (x3), VSS pullup, VSS bias (x2)                            |
| R      | 12k     | 2   | Brake divider, RPM divider (bottom resistor)                                      |
| R      | 15k     | 2   | TPS divider, line pressure divider (top resistor)                                 |
| R      | 24k     | 2   | TPS divider, line pressure divider (bottom resistor)                              |
| R      | 47k     | 2   | Brake divider, RPM divider (top resistor)                                         |
| R      | 3.3k    | 1   | Temp pullup                                                                       |
| R      | 100k    | 1   | VSS hysteresis                                                                    |
| C      | 0.1uF   | 6   | TPS, temp, line pressure, VSS coupling (C3), VSS output (C4), VSS decoupling (C5) |
| D      | 1N5817  | 8   | TPS clamp (x2), RPM clamp (x2), line pressure clamp (x2), brake clamp (x2)        |
| D      | 1N4148  | 2   | VSS clamp (x2)                                                                    |
| IC     | LM2903P | 1   | VSS comparator (DIP-8, only comparator 1 used)                                    |

## Pin Summary

| Teensy Pin | Function | Type |
|------------|----------|------|
| A0 (14) | TPS | Analog 0-3.3V |
| A1 (15) | Trans Temp | Analog (NTC) |
| A2 (16) | Line Pressure | Analog 0.5-4.5V (optional) |
| 6 | Range 1st | Digital, active LOW |
| 7 | Range 2nd | Digital, active LOW |
| 8 | Range 3rd | Digital, active LOW |
| 9 | Range Drive | Digital, active LOW |
| 10 | Brake Switch | Digital, active HIGH |
| 18 | VSS | Digital, interrupt |
| 19 | RPM Tach | Digital, frequency |
| 20 | Pressure Sw A | Digital, active LOW |
| 21 | Pressure Sw B | Digital, active LOW |
| 22 | Pressure Sw C | Digital, active LOW |
