# 4L60‑E Manual Controller — DIY‑Friendly BOM (No Tiny SMD)

This BOM is aimed at **hand‑solderable parts** (through‑hole, TO‑220/TO‑247, radial/axial leaded, and plug‑in modules).  
It covers the core controller plus a **4‑channel low‑side solenoid driver stage** (SSA, SSB, TCC, EPC).

> Notes:
> - Use **automotive wire**, proper crimp tooling, and fuse protection.
> - Values shown are “known‑good starting points” for a 4L60‑E manual‑mode controller.
> - Where possible, I list **common, easily sourced** example parts; equivalents are fine.

---

## 1) Core compute + comms

| Item | Qty | Example Part | Package | Notes |
|---|---:|---|---|---|
| Real‑time controller | 1 | **Teensy 4.1** | Module | Main TCM controller |
| Logging / UI (optional) | 1 | **Raspberry Pi Zero 2 W** | Module | Optional; Teensy must run standalone |
| UART level | 1 | (none) | — | Teensy↔Pi is **3.3V TTL**; keep wiring short, twisted if possible |
| USB cable (bench) | 1 | USB‑A → Micro‑B | — | Firmware upload + bench debug |

---

## 2) Power input + protection (vehicle 12V → clean 5V)

| Item | Qty | Example Part | Package | Notes |
|---|---:|---|---|---|
| Main fuse (solenoids + controller feed) | 1 | **ATO/ATC inline fuse holder** + 15–25A fuse | Inline | Size depends on solenoid wiring + accessories |
| Controller fuse (logic only) | 1 | Inline fuse holder + 3–5A fuse | Inline | Separate logic fuse strongly recommended |
| Reverse polarity protection (simple) | 1 | **High‑current Schottky diode** (e.g., MBR3045 / MBR4045) | TO‑220 | Easy DIY option; small voltage drop |
| Load‑dump / surge TVS (12V rail) | 1 | **5KP58A** (or similar) | Axial leaded | Through‑hole high‑energy TVS on 12V input |
| Bulk capacitor (12V) | 1–2 | 470–1000 µF, 35V, low‑ESR | Radial | Place near driver board input |
| Decoupling caps | 4–10 | 0.1 µF | Through‑hole disc | One near each driver channel + logic rail |
| 12V→5V buck for Teensy | 1 | **OKI‑78SR‑5/1.5‑W36** (or R‑78E5.0) | SIP module | Wide‑input switching regulator, hand‑solderable |
| 12V→5V buck for Pi (optional) | 1 | OKI‑78SR‑5/1.5 or **5V/3A buck module** | Module | Pi can pull bursts; keep it separate if possible |
| EMI input filter (optional) | 1 | 100 µH choke + 100 µF cap | Through‑hole | Optional but helps noise |

---

## 3) Solenoid driver stage (4 channels, low‑side MOSFET)

### 3.1 MOSFETs (hand‑solderable)
| Item | Qty | Example Part | Package | Notes |
|---|---:|---|---|---|
| Low‑side MOSFET (SSA) | 1 | **IRLZ44N** | TO‑220 | Logic‑level, common, easy to mount |
| Low‑side MOSFET (SSB) | 1 | IRLZ44N | TO‑220 |  |
| Low‑side MOSFET (TCC) | 1 | IRLZ44N | TO‑220 | TCC can draw more; TO‑220 is fine with copper/heatsink |
| Low‑side MOSFET (EPC/PCS PWM) | 1 | IRLZ44N (or IRLZ34N / IRLZ48N) | TO‑220 | EPC runs PWM continuously—**heatsink recommended** |
| Heatsinks (EPC at minimum) | 1–2 | TO‑220 clip‑on or bolt‑on heatsink | — | Use thermal pad/grease as needed |
| Insulators (if shared heatsink) | 1 set | TO‑220 mica/silicone pads + shoulder washers | — | Only if multiple MOSFETs share one heatsink |

### 3.2 Gate network + local caps (per channel)
| Item | Qty | Example Part | Package | Notes |
|---|---:|---|---|---|
| Gate resistor | 4 | 100 Ω, 1/4W | Axial leaded | Limits ringing/inrush |
| Gate pulldown | 4 | 100 kΩ, 1/4W | Axial leaded | Ensures OFF at reset |
| Local decoupling | 4 | 0.1 µF | Through‑hole disc | Near MOSFET + supply rail |

### 3.3 Flyback / clamp (fast release, DIY‑friendly)
| Item | Qty | Example Part | Package | Notes |
|---|---:|---|---|---|
| Solenoid clamp TVS (SSA) | 1 | **1.5KE36CA** | Axial leaded | Bidirectional TVS across solenoid |
| Solenoid clamp TVS (SSB) | 1 | 1.5KE36CA | Axial leaded |  |
| Solenoid clamp TVS (TCC) | 1 | 1.5KE36CA | Axial leaded |  |
| Solenoid clamp TVS (EPC) | 1 | **1.5KE36CA** (or higher power 5KP36CA) | Axial leaded | EPC is hardest‑working; consider 5KP series |

> Why TVS instead of a plain diode: a diode slows current decay and can cause “mushy” solenoid release and shift feel issues. TVS clamps faster.

---

## 4) Inputs (DIY‑friendly conditioning)

| Input | Qty | Example Part | Package | Notes |
|---|---:|---|---|---|
| Brake input protection | 1 | 1N4148 + 10k pullup/down resistors | Axial leaded | Simple protection + configurable polarity |
| Brake opto isolator (optional) | 1 | **PC817** | DIP‑4 | Extra noise isolation if needed |
| Range selector inputs | 4 | 10k pullups + 0.1 µF RC (optional) | Axial/TH | Debounce/noise filter |
| Schmitt trigger buffer (optional) | 1 | **74HC14** | DIP‑14 | Cleans noisy switch/tach signals; 6 inverting buffers |
| TPS scaling (0–5V → 0–3.3V) | 1 | Resistor divider (e.g., 10k/20k) | Axial | Keep source impedance reasonable; add 0.1 µF to ground |
| TPS buffer op‑amp (optional) | 1 | **LM358** | DIP‑8 | Only if you need input buffering |
| VSS VR conditioner (recommended) | 1 | **LM1815N** | DIP‑8 | Classic VR conditioner in a DIY package |
| VSS protection | 2–4 | 1N4148 / 1N4007 + resistors | Axial | Input clamp + series resistor before LM1815 |
| Optional pressure switch inputs | up to 3 | 10k pullups + input protection | Axial | Great for sanity checks |

---

## 5) Connectors, wiring, and enclosure

| Item | Qty | Example Part | Package | Notes |
|---|---:|---|---|---|
| Transmission connector approach | 1 | **4L60E pigtail** (salvage/aftermarket) | — | Simplest; avoids sourcing the case connector |
| Controller I/O connectors | 2–6 | **Deutsch DT** 2/3/4/6/8/12 pin | — | Rugged, common, proper seals |
| Fuse/relay box (optional) | 1 | Sealed mini fuse box | — | Clean power distribution |
| Wire (solenoids) | as needed | 14–16 AWG TXL/GXL | — | Higher current runs |
| Wire (signals) | as needed | 18–22 AWG TXL/GXL | — | Sensors, switches, UART |
| Enclosure | 1 | Sealed polycarbonate box (IP65+) | — | Cable glands or bulkhead connectors |
| Grounding hardware | 1 set | Ring terminals + star washers | — | Star‑ground at enclosure/chassis point |

---

## 6) Optional but highly recommended sensors

| Item | Qty | Example Part | Package | Notes |
|---|---:|---|---|---|
| Transmission temp sensor | 1 | GM‑style TFT sensor (OEM compatible) | — | Great for TCC inhibit + safety |
| Line pressure sensor | 1 | 0.5–4.5V, 0–300 psi transducer | 1/8" NPT | Makes EPC tuning vastly safer/easier |
| 5V reference (for sensors) | 1 | R‑78E5.0 or a dedicated 5V LDO module | Module | Keep sensor 5V clean and separate if possible |

---

## 7) Assembly notes (practical)

- Put **EPC MOSFET** on the best heatsinking area (or its own heatsink).
- Keep **power ground** (solenoids) and **sensor ground** separated and joined at a single star point.
- Use **fuses**: one for logic, one for solenoids.
- Put the **12V load‑dump TVS** as physically close to the 12V input as possible.
- If your VSS is Hall (not VR), you can skip LM1815 and use a simple pullup + Schmitt input approach.

---

## 8) Quick quantity summary (core build)

- Teensy 4.1: **1**
- IRLZ44N (TO‑220): **4**
- Gate resistors 100Ω: **4**
- Gate pulldowns 100k: **4**
- TVS for solenoids (1.5KE36CA): **4**
- 12V rail TVS (5KP58A): **1**
- Bulk caps: **2–3**
- 0.1µF caps: **6–12**
- Buck modules (5V): **1–2**
- LM1815N (VR conditioner): **1**
- Deutsch DT connectors + pins/seals: **as needed**
- Inline fuse holders: **2**

**Optional signal conditioning:**

- 74HC14 Schmitt trigger (DIP‑14): **1** (if noisy inputs)
- PC817 opto isolator (DIP‑4): **1** (brake isolation)
- LM358 op‑amp (DIP‑8): **1** (TPS buffering)

---

If you want, I can also produce a **shopping‑list version** (same BOM, but grouped by supplier category: electronics / wiring / connectors / enclosure) and a **PCB‑less “perfboard wiring layout” suggestion** that stays DIY‑friendly.
