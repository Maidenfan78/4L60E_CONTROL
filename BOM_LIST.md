# 4L60-E Manual Controller -- DIY-Friendly BOM (No Tiny SMD)

This BOM is aimed at **hand-solderable parts** (through-hole, TO-220/TO-247, radial/axial leaded, and plug-in modules).
It covers the core controller plus a **4-channel low-side solenoid driver stage** (SSA, SSB, TCC, EPC).

> Notes:
> - Use **automotive wire**, proper crimp tooling, and fuse protection.
> - Values shown are "known-good starting points" for a 4L60-E manual-mode controller.
> - Where possible, I list **common, easily sourced** example parts; equivalents are fine.

---

## 1) Core compute + comms

| Item | Qty | Example Part | Package | Notes |
|---|---:|---|---|---|
| Real-time controller | 1 | **Teensy 4.1** | Module | Main TCM controller |
| Logging / UI (optional) | 1 | **Raspberry Pi Zero 2 W** | Module | Optional; Teensy must run standalone |
| UART level | 1 | (none) | -- | Teensy<->Pi is **3.3V TTL**; keep wiring short, twisted if possible. 2x CP2102 USB-TTL adapters for bench |
| USB cable (bench) | 1 | USB-A to Micro-B | -- | Firmware upload + bench debug |

---

## 2) Power input + protection (vehicle 12V to clean 5V)

| Item | Qty | Example Part | Package | Notes |
|---|---:|---|---|---|
| Main fuse (solenoids + controller feed) | 1 | **ATO/ATC inline fuse holder** + 15-25A fuse | Inline | Size depends on solenoid wiring + accessories |
| Controller fuse (logic only) | 1 | Inline fuse holder + **2A fast-blow** fuse | Inline | Per OKI-78SR datasheet recommendation |
| Reverse polarity protection | 1 | **VFT4045BP** Schottky rectifier | TO-220 | 45V/40A, 0.41V drop at 5A; blocks reverse polarity |
| Load-dump / surge TVS (12V rail) | 1 | **P6KE18A** | Axial leaded | 15.3V standoff, 18V breakdown; clamps automotive transients to safe levels |
| Bulk capacitor (12V) | 1-2 | 470-1000 uF, 35V, low-ESR | Radial | Place near driver board input |
| Buck input cap (Teensy) | 1 | **22uF ceramic, 50V** (X5R or X7R) | Through-hole | Close to OKI-78SR input pins; low ESR required per datasheet |
| Buck output cap (Teensy) | 1 | **22uF ceramic, 10V** | Through-hole | OKI-78SR output filter; do not exceed 300uF low-ESR |
| Buck input cap (Pi) | 1 | **22uF ceramic, 50V** (X5R or X7R) | Through-hole | Close to DFR0831 input |
| Decoupling caps | 4-10 | 0.1 uF | Through-hole disc | One near each driver channel + logic rail |
| 12V to 5V buck for Teensy | 1 | **OKI-78SR-5/1.5-W36E-C** | SIP module | 7-36V input, 5V/1.5A; drop-in 7805 replacement |
| 12V to 5V buck for Pi | 1 | **DFR0831** | Module | 7-24V input, 5V/4A; separate from Teensy supply |
| EMI input filter (optional) | 1 | 100 uH choke + 100 uF cap | Through-hole | Optional but helps noise |

---

## 3) Solenoid driver stage (4 channels, low-side MOSFET)

### 3.1 MOSFETs (hand-solderable)
| Item | Qty | Example Part | Package | Notes |
|---|---:|---|---|---|
| Low-side MOSFET (SSA) | 1 | **STP36NF06L** | TO-220 | Logic-level, AEC-Q101 automotive qualified |
| Low-side MOSFET (SSB) | 1 | STP36NF06L | TO-220 | 60V, 30A, 40m ohm RDS(on) |
| Low-side MOSFET (TCC) | 1 | STP36NF06L | TO-220 | Low gate charge (13-17nC) for fast switching |
| Low-side MOSFET (EPC/PCS PWM) | 1 | STP36NF06L | TO-220 | EPC runs PWM continuously -- **heatsink recommended** |
| Heatsinks (EPC at minimum) | 1-2 | TO-220 clip-on or bolt-on heatsink | -- | Use thermal pad/grease as needed |
| Insulators (if shared heatsink) | 1 set | TO-220 mica/silicone pads + shoulder washers | -- | Only if multiple MOSFETs share one heatsink |

### 3.2 Gate network + local caps (per channel)
| Item | Qty | Example Part | Package | Notes |
|---|---:|---|---|---|
| Gate resistor | 4 | 100 ohm, 1/4W | Axial leaded | Limits ringing/inrush |
| Gate pulldown | 4 | 100k, 1/4W | Axial leaded | Ensures OFF at reset |
| Local decoupling | 4 | 0.1 uF | Through-hole disc | Near MOSFET + supply rail |

### 3.3 Flyback / clamp (DIY-friendly)

| Item | Qty | Example Part | Package | Notes |
|---|---:|---|---|---|
| Solenoid clamp TVS (SSA) | 1 | **5KP36CA** | Axial leaded | Bidirectional TVS -- 5000W peak pulse power |
| Solenoid clamp TVS (SSB) | 1 | 5KP36CA | Axial leaded | 36V standoff, fast response |
| Solenoid clamp TVS (TCC) | 1 | 5KP36CA | Axial leaded | |
| EPC flyback diode | 1 | **1N4001** (or 1N4007) | Axial leaded | Smoother pressure modulation |
| EPC spike TVS (optional) | 1 | 5KP36CA | Axial leaded | Extra spike protection if needed |

> **Why different flyback for EPC?**
>
> - **SSA/SSB/TCC** use TVS for fast current decay --> crisp solenoid response
> - **EPC** uses diode for slow current decay --> smoother pressure modulation (matches MegaShift recommendation)

---

## 4) Inputs (DIY-friendly conditioning)

| Input | Qty | Example Part | Package | Notes |
|---|---:|---|---|---|
| TPS scaling (0-5V to 0-3.3V) | 1 | **15k/24k** resistor divider + 0.1uF cap | Axial | 5V in = 3.08V out; safe margin for a 5.25V rail |
| Brake input (12V switched) | 1 | **47k/12k** resistor divider + clamp | Axial | 15V worst case = 3.05V out; add series R + Schottky clamps |
| Trans temp pull-up | 1 | **3.3k** resistor | Axial | NTC pullup to 3.3V on A1 (Pin 15) |
| Line pressure scaling (0-4.5V) | 1 | **15k/24k** resistor divider + 0.1uF cap | Axial | 4.5V in = 2.77V out; sensor powered from 5V_TEENSY rail |
| RPM input (0-12V from MS3) | 1 | **47k/12k** resistor divider + Schottky clamps | Axial | Divider: 12V = 2.44V; clamps are backup only |
| Range selector inputs | 4 | **10k** pullups to 3.3V | Axial | Ground-switched; Teensy has built-in Schmitt triggers |
| Pressure switch inputs | 3 | **10k** pullups to 3.3V | Axial | Ground-switched, gear confirmation (A/B/C on Pins 20-22) |
| VSS VR conditioner | 1 | **LM2903N** | DIP-8 | Dual comparator; 1 of 2 units used (see circuit below) |
| VSS bias resistors | 2 | **10k** (to +3.3V and to GND) | Axial | Sets 1.65V bias/threshold at comparator inputs |
| VSS hysteresis resistor | 1 | **100k** | Axial | From output back to (+) input, prevents chatter |
| VSS output pullup | 1 | **10k** (to +3.3V) | Axial | LM2903 is open-collector, needs external pullup |
| VSS coupling cap | 1 | **0.1uF** film/ceramic | Through-hole | AC couples VR signal to comparator |
| VSS output filter cap | 1 | **0.1uF** ceramic | Through-hole | Cleans output edges |
| VSS protection | 2 | **1N4148** (clamp to 3.3V and GND) | Axial | Clamps VR spikes to safe levels |
| Input clamp diodes | 8 | **1N5817/1N5818/1N5819** | Axial | Schottky clamps for TPS, RPM, line pressure, brake (2 per input) |
| Input series resistors | 4 | **1k** | Axial | Series protection for TPS, RPM, line pressure, brake |
| Schmitt trigger buffer (optional) | 1 | **74HC14** | DIP-14 | Not needed if inputs are clean; Teensy has built-in Schmitt |

---

## 5) Connectors, wiring, and enclosure

| Item | Qty | Example Part | Package | Notes |
|---|---:|---|---|---|
| Transmission connector approach | 1 | **4L60E pigtail** (salvage/aftermarket) | -- | Simplest; avoids sourcing the case connector |
| Controller I/O connectors | 2-6 | **Deutsch DT** 2/3/4/6/8/12 pin | -- | Rugged, common, proper seals |
| Fuse/relay box (optional) | 1 | Sealed mini fuse box | -- | Clean power distribution |
| Wire (solenoids) | as needed | 14-16 AWG TXL/GXL | -- | Higher current runs |
| Wire (signals) | as needed | 18-22 AWG TXL/GXL | -- | Sensors, switches, UART |
| Enclosure | 1 | Sealed polycarbonate box (IP65+) | -- | Cable glands or bulkhead connectors |
| Grounding hardware | 1 set | Ring terminals + star washers | -- | Star-ground at enclosure/chassis point |

---

## 6) Optional but highly recommended sensors

| Item | Qty | Example Part | Package | Notes |
|---|---:|---|---|---|
| Transmission temp sensor | 1 | GM-style TFT sensor (OEM compatible) | -- | Great for TCC inhibit + safety |
| Line pressure sensor | 1 | 0.5-4.5V, 0-300 psi transducer | 1/8" NPT | Makes EPC tuning vastly safer/easier |

---

## 7) Assembly notes (practical)

- Put **EPC MOSFET** on the best heatsinking area (or its own heatsink).
- Keep **power ground** (solenoids) and **sensor ground** separated and joined at a single star point.
- Use **fuses**: one for logic (2A), one for solenoids (15-25A).
- Put the **12V load-dump TVS (P6KE18A)** as physically close to the 12V input as possible.
- Place the **Schottky diode (VFT4045BP)** between fuse and TVS for reverse polarity protection.
- If your VSS is Hall (not VR), you can skip the LM2903N and use a simple pullup + Schmitt input approach.
- All 3.3V pullups are sourced from the **Teensy onboard 3.3V regulator** (250mA capacity, total load ~5mA).

### LM2903N VR Conditioner Circuit

```text
VR sensor (+) --+-- 0.1uF --+----------+--- LM2903 IN+ (pin 3)
                |            |          |
              clamp        10k        100k (hysteresis)
              diodes         |          |
                |            +-- 1.65V -+--- LM2903 OUT (pin 1) --+-- to Teensy Pin 18
VR sensor (-) --+------------+-- GND                              |
                                                                [10k] pullup to +3.3V
                            10k                                   |
              +3.3V -------+-------- LM2903 IN- (pin 2) -- 1.65V ref
                           |
                          10k
                           |
                          GND
```

- The 10k/10k divider sets a **1.65V** threshold at IN- (from +3.3V, not 5V)
- VR signal AC-couples through 0.1uF to IN+
- 100k hysteresis resistor prevents output chatter near zero-cross
- 1N4148 diodes clamp VR spikes: one to GND, one to +3.3V
- Output is **0V/3.3V** square wave at VSS frequency (open-collector with 10k pullup to 3.3V)

---

## 8) Quick quantity summary (core build)

**Compute:**
- Teensy 4.1: **1**
- Raspberry Pi Zero 2 W: **1** (optional)

**Power supply:**
- VFT4045BP Schottky (reverse polarity): **1**
- P6KE18A TVS (12V rail transient clamp): **1**
- OKI-78SR-5/1.5-W36E-C (Teensy 5V buck): **1**
- DFR0831 (Pi 5V buck): **1**
- 22uF ceramic 50V (buck input caps): **2**
- 22uF ceramic 10V (buck output cap): **1**
- Inline fuse holders: **2** (2A logic + 15-25A solenoid)

**Solenoid drivers:**
- STP36NF06L (TO-220): **4**
- Gate resistors 100 ohm: **4**
- Gate pulldowns 100k: **4**
- 5KP36CA TVS for SSA/SSB/TCC: **3**
- 1N4001 EPC flyback diode: **1**
- Bulk caps (470-1000uF, 35V): **2-3**

**Input conditioning resistors:**
- 1k resistors: **4** (series protection for TPS, RPM, line pressure, brake)
- 10k resistors: **10** (range x4 + pressure sw x3 + VSS pullup + VSS bias x2)
- 12k resistor: **2** (brake divider, RPM divider - bottom resistor)
- 15k resistors: **2** (TPS, line pressure dividers - top resistor)
- 24k resistors: **2** (TPS, line pressure dividers - bottom resistor)
- 47k resistors: **2** (brake divider, RPM divider - top resistor)
- 3.3k resistor: **1** (temp pullup)
- 100k resistor: **1** (VSS hysteresis)

**Input conditioning other:**
- 0.1uF caps: **5** (TPS, temp, line pressure, VSS coupling, VSS output)
- 1N4148 diodes: **2** (VSS clamp x2)
- 1N5817/1N5818/1N5819 diodes: **8** (TPS, RPM, line pressure, brake clamps)
- LM2903N (VR comparator): **1**
- 0.1uF caps (driver decoupling): **4-10**

**Connectors:**
- Deutsch DT connectors + pins/seals: **as needed**

**Optional:**
- 74HC14 Schmitt trigger (DIP-14): **1** (only if noisy inputs)
