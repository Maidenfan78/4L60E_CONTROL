# Power Supply Circuits

Automotive 12V to regulated 5V for Teensy 4.1 and Raspberry Pi Zero 2 W.
Automotive voltage range: 10V cranking, 12.6V resting, 14.7V charging, transients to ~40V.

---

## Power Architecture Overview

```
                                        +--- OKI-78SR-5 ---+--- 5V_TEENSY ---> Teensy 4.1 VIN
                                        |   (5V / 1.5A)    |
VEHICLE +12V ---[F1]---[D1]---[TVS1]---+                   +--- 5V_SENSOR ---> Line pressure sensor
                                        |
                                        +--- DFR0831 ------+--- 5V_PI -------> Pi Zero 2 W
                                            (5V / 4A)

VEHICLE GND -------------------------------- GND (star ground point)
```

Key design rules:
- Teensy and Pi have **separate** buck converters (Teensy must survive Pi brownout)
- Reverse polarity protection **before** both converters
- TVS clamping **before** both converters
- Fuse protects the entire board

---

## Stage 1: Input Protection

```
VEHICLE +12V                                         To buck converters
    |                                                       |
    +---[F1 2A fast-blow]---+---[D1 VFT4045BP]-----+--------+
                            |    (Schottky)        |
                            |    Anode --> Cathode |
                            |                      |
                            |                   [TVS1]  P6KE18A (or SMBJ16A)
                            |                      |
                           GND                    GND
```

### F1 - Input Fuse
- **2A fast-blow** fuse
- Protects against short circuits and reverse polarity (blows if D1 is reverse-biased)
- OKI-78SR datasheet recommends 2A fuse

### D1 - Reverse Polarity Protection (VFT4045BP Schottky)
- 45V, 40A Schottky rectifier
- Forward voltage drop: **0.41V** at 5A (very low loss)
- At 1A typical system draw: ~0.3V drop, so converters see ~12.3V (well above 7V min)
- At cranking (10V): converters see ~9.6V (still above 7V min)
- Blocks reverse polarity completely (fuse blows as backup)

### TVS1 - Transient Voltage Suppression
- **P6KE18A** (or SMBJ16A bidirectional)
- Standoff voltage: 15.3V (won't conduct during normal operation at 14.7V)
- Breakdown: 18V
- Clamps load dump transients (up to 40V+ spikes) to safe levels
- Placed **after** D1 so it only sees forward-polarity transients

### Voltage at converter inputs after protection:

| Condition       | Battery | After D1 (Schottky) | TVS clamp |
|-----------------|---------|---------------------|-----------|
| Cranking        | 10V     | ~9.6V               | No clamp  |
| Resting         | 12.6V   | ~12.2V              | No clamp  |
| Charging        | 14.7V   | ~14.3V              | No clamp  |
| Load dump spike | 40V+    | ~39.6V              | Clamped to ~18V |

All values within OKI-78SR input range (7-36V).

---

## Stage 2A: Teensy 4.1 Power Supply

```
Protected 12V rail                              To Teensy 4.1
      |                                              |
      +---[Cin 22uF ceramic]---GND          +--------+---[Cout 22uF ceramic]---GND
      |                                     |
      +--- OKI-78SR-5/1.5-W36E-C ---+------+
           Pin 1: +Vin               |
           Pin 2: GND (Common)       |
           Pin 3: +Vout (5V)         |
                                     |
                              5V_TEENSY rail
                                     |
                                     +---> Teensy 4.1 VIN pin
                                     +---> Line pressure sensor +5V (if used)
```

### OKI-78SR-5/1.5-W36E-C
- Input: 7-36V DC (covers full automotive range after Schottky drop)
- Output: **5.0V regulated**, 1.5A max
- Efficiency: ~90% at 12V in
- Short circuit protection (hiccup mode, auto-recovery)
- No heatsink required
- Drop-in replacement for 7805 linear regulator (same pinout)
- Operating temp: -40 to +85C

### Input capacitor (Cin)
- 22uF ceramic, 50V rated
- Placed close to OKI-78SR input pins
- Low ESR type (X5R or X7R)
- Per datasheet: "install a low-ESR capacitor immediately adjacent to input terminals"

### Output capacitor (Cout)
- 22uF ceramic, 10V rated
- Optional per datasheet but recommended for improved transient response
- Do NOT exceed 300uF (low ESR) or 3300uF (higher ESR)

### Load budget (Teensy rail):

| Load                      | Current   |
|---------------------------|-----------|
| Teensy 4.1                | ~100mA    |
| Input conditioning (3.3V) | ~5mA      |
| LM2903 comparator         | ~2mA      |
| Line pressure sensor      | ~20mA     |
| **Total**                 | **~130mA** |

Well within 1.5A capacity. Teensy's onboard regulator converts 5V to 3.3V for the MCU and I/O.

---

## Stage 2B: Raspberry Pi Power Supply

```
Protected 12V rail                              To Pi Zero 2 W
      |                                              |
      +---[Cin 22uF ceramic]---GND                   |
      |                                     +--------+
      +--- DFR0831 ---- OUT+ ---+-----------+
           IN+                  |            |
           IN-                  |        Pi GPIO Header
           OUT-                 |        Pin 2/4 = +5V
             |                  |        Pin 6 = GND
            GND                 |
                           [Cout2 220uF electrolytic]
                                |
                               GND
```

### DFR0831 DC-DC Buck Converter
- Input: 7-24V DC
- Output: **5.0V regulated**, 4A max (20W)
- Efficiency: ~90%
- Output ripple: <100mV
- Output accuracy: +/-0.1V
- Operating temp: -40 to +85C
- 6 output ports on board (convenient for Pi + accessories)

### Output bulk capacitor (Cout2)

- 220uF electrolytic, 10V rated
- Smooths transient current demands from the Pi (draws current in bursts during boot and CPU/USB activity)
- The DFR0831's onboard caps are small ceramics — the bulk cap supplements them
- Not strictly required but cheap insurance in an automotive environment

### Pi Zero 2 W connection

- Power via **GPIO header** (not micro USB port)
- Pin 2 or Pin 4 = +5V, Pin 6 = GND
- This bypasses the Pi's onboard polyfuse, so the DFR0831's current limit and upstream fuse F1 provide protection
- UART connection to Teensy: Pi GPIO 14 (TX) → Teensy RX1 (Pin 0), Pi GPIO 15 (RX) → Teensy TX1 (Pin 1)
- Both are 3.3V logic — no level shifting needed

### Important note on input range

- DFR0831 max input is **24V** (30V absolute max on PCB marking)
- The TVS clamps transients to ~18V so this is safe
- At charging voltage (14.7V - 0.4V Schottky = 14.3V): well within range
- Without the TVS, automotive load dump could exceed 24V and damage this converter

### Load budget (Pi rail):

| Load                | Current    |
|---------------------|------------|
| Pi Zero 2 W (idle)  | ~100mA     |
| Pi Zero 2 W (busy)  | ~350mA     |
| Pi Zero 2 W (peak)  | ~500mA     |
| Wi-Fi active        | +50-100mA  |
| **Total peak**      | **~600mA** |

Well within 4A capacity.

---

## Stage 3: Teensy Power Input Detail

```
5V_TEENSY rail
      |
      +---> Teensy 4.1 VIN pin (regulated 5V input)

      Teensy onboard:
      VIN (5V) ---> [onboard 3.3V regulator] ---> 3.3V rail (250mA max)
                                                     |
                                                     +---> MCU (i.MX RT1062)
                                                     +---> GPIO pins (3.3V logic)
                                                     +---> 3.3V output pin
                                                              |
                                                              +---> Input pullups
                                                              +---> VSS bias network
                                                              +---> LM2903 Vcc
```

The Teensy's **onboard 3.3V regulator** supplies:
- The MCU itself
- All GPIO pins (3.3V logic)
- The 3.3V output pin that powers our input conditioning pullups and bias networks
- Total 3.3V load from our circuits: ~5mA (well under 250mA capacity)

---

## Grounding

```
                    STAR GROUND POINT (single PCB location)
                            |
              +-------------+-------------+-------------+
              |             |             |             |
         Power GND     Signal GND    Analog GND    Solenoid GND
              |             |             |             |
         Buck converter  Digital I/O   TPS/Temp ADC   MOSFET drivers
         returns         range sel.    NTC sensor     SSA/SSB/EPC/TCC
                         press. sw.    line pressure
                         brake sw.     VSS bias
```

### Grounding rules:
1. **Single star ground point** on PCB - all ground paths meet here
2. **Analog ground (AGND)** traces kept separate from digital until star point
3. **Solenoid return currents** routed separately - high current PWM noise must not
   couple into signal grounds
4. Teensy AGND pin tied to analog ground trace
5. Teensy GND pins tied to digital/power ground
6. Both connect at the star point

---

## Component Summary

| Part | Value / Part Number | Qty | Function |
|------|---------------------|-----|----------|
| F1 | 2A fast-blow fuse | 1 | Input overcurrent protection |
| D1 | VFT4045BP (Schottky) | 1 | Reverse polarity protection |
| TVS1 | P6KE18A | 1 | Transient voltage clamping |
| U_REG1 | OKI-78SR-5/1.5-W36E-C | 1 | 12V to 5V buck (Teensy) |
| U_REG2 | DFR0831 | 1 | 12V to 5V buck (Pi) |
| Cin1 | 22uF ceramic 50V | 1 | OKI-78SR input filter |
| Cout1 | 22uF ceramic 10V | 1 | OKI-78SR output filter |
| Cin2 | 22uF ceramic 50V | 1 | DFR0831 input filter |
| Cout2 | 220uF electrolytic 10V | 1 | DFR0831 output bulk cap |

---

## Cranking / Brownout Behavior

| Event | Battery V | After D1 | OKI-78SR (Teensy) | DFR0831 (Pi) |
|-------|-----------|----------|-------------------|--------------|
| Normal running | 14.0V | 13.6V | 5.0V output OK | 5.0V output OK |
| Key on, engine off | 12.6V | 12.2V | 5.0V output OK | 5.0V output OK |
| Cranking dip | 10.0V | 9.6V | 5.0V output OK | 5.0V output OK |
| Deep crank | 7.5V | 7.1V | 5.0V output OK (min 7V) | 5.0V output OK (min 7V) |
| Severe brownout | 6.5V | 6.1V | **Drops out** | **Drops out** |

Both converters have 7V minimum input. After the ~0.4V Schottky drop, the system needs at
least **7.4V at the battery** to maintain regulation. Below that, both converters will drop out.

The OKI-78SR has hiccup-mode short circuit protection and will auto-recover when voltage
returns. The Teensy will reboot cleanly on power restore.

If the Pi browns out first (due to higher current draw), the Teensy continues running
independently - this is by design. The transmission controller does not depend on the Pi.
