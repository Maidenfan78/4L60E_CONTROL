# Pressure Switch Verification Guide

**Purpose:** Verify the pressure switch truth table on YOUR transmission before relying on it for gear confirmation logic.

**Time Required:** ~15 minutes  
**Tools Needed:** Multimeter, notepad/phone for recording values

---

## ⚠️ Important Safety Notes

- **Engine OFF** for all tests
- **Key ON** (ignition on, engine not running) to power the transmission
- **Transmission in PARK** when starting
- **Parking brake engaged**
- **Vehicle on level ground**

---

## Understanding Pressure Switches

The 4L60-E has a **pressure switch manifold** inside the transmission valve body. It contains 3 switches (A, B, C) that close when hydraulic pressure is present in specific circuits.

**How it works:**
- The **shifter position** (P, R, N, D, 3, 2, 1) moves the **manual valve** inside the transmission
- The manual valve directs hydraulic pressure to different circuits
- Pressure closes switches → controller reads which position the shifter is in

**What it tells you:**
- ✅ Where the **shifter** is (manual valve position)
- ❌ NOT the actual gear the transmission is in (that's controlled by solenoids)

---

## Method 1: Using the TCM Controller (Preferred)

If you've already built the controller and uploaded the firmware:

### Step 1: Connect to Serial Monitor
1. Connect Teensy to computer via USB
2. Open Serial monitor (Arduino IDE, PlatformIO Serial Monitor, or `screen /dev/ttyACM0 115200`)
3. Set baud rate to **115200**

### Step 2: Power Up
1. Turn ignition to **ON** (do not start engine)
2. TCM will boot and display: `4L60E TCM boot`
3. Every 1 second, you'll see telemetry including:
   ```
   Manual Position: P/N (A:H B:L C:H)
   ```

### Step 3: Test Each Position
1. **With key ON, engine OFF:**
2. Move shifter slowly through each position: **P → R → N → D → 3 → 2 → 1**
3. Wait 2 seconds at each position for the reading to stabilize
4. Record the **Manual Position** and **switch states (A/B/C)** for each position

**Expected Pattern (OEM 4L60-E):**

| Shifter | Position Code | Switch A | Switch B | Switch C |
|---------|---------------|----------|----------|----------|
| **P**   | P/N           | H        | L        | H        |
| **R**   | R             | L        | L        | H        |
| **N**   | P/N           | H        | L        | H        |
| **D**   | D             | H        | L        | L        |
| **3**   | 3             | H        | H        | L        |
| **2**   | 2             | H        | H        | H        |
| **1**   | 1             | L        | H        | H        |

**Notes:**
- H = HIGH (switch open, no pressure)
- L = LOW (switch closed, pressure present)
- Park and Neutral read the same (H/L/H) — this is normal

### Step 4: Compare Results
- If all positions match the expected pattern → ✅ **OEM table confirmed, you're good to go**
- If some positions differ → ⚠️ **Update firmware with YOUR table** (see "Non-Standard Pattern" below)
- If all positions read the same → ❌ **Wiring fault or bad pressure switch manifold**

---

## Method 2: Using a Multimeter (No Controller Needed)

If you haven't built the controller yet, you can test with a multimeter:

### Step 1: Locate the Transmission Connector
1. Find the **20-pin connector** on the driver's side of the transmission case
2. It's usually near the shift solenoids (accessible from underneath)

### Step 2: Identify Pressure Switch Pins
Looking at the connector from the **wire side** (not the transmission side):

| Pin Label | Wire Color (typical) | Function |
|-----------|----------------------|----------|
| **N**     | Often TAN or LT GRN  | Pressure Switch A |
| **R**     | Often DK GRN         | Pressure Switch B |
| **P**     | Often LT BLU         | Pressure Switch C |
| **GND**   | BLACK (multiple)     | Common Ground |

**⚠️ Wire colors vary by year/harness. Use a wiring diagram for YOUR vehicle if unsure.**

### Step 3: Measure Resistance
1. **Ignition OFF, key out**
2. **Disconnect the transmission connector** (unplug it from the trans)
3. Set multimeter to **Resistance (Ω)** mode
4. Measure between each switch pin and **ground** on the **transmission side** (not harness side)

**For each shifter position:**
1. Move shifter to position (P, R, N, D, 3, 2, 1)
2. Measure resistance from **Pin N** (Switch A) to ground
3. Measure resistance from **Pin R** (Switch B) to ground
4. Measure resistance from **Pin P** (Switch C) to ground

**Interpreting Results:**
- **0-5 Ω (near zero):** Switch CLOSED (pressure present) → reads as **LOW** in controller
- **Open/OL (infinite):** Switch OPEN (no pressure) → reads as **HIGH** in controller

**Expected Readings (OEM 4L60-E):**

| Shifter | Switch A (Pin N) | Switch B (Pin R) | Switch C (Pin P) |
|---------|------------------|------------------|------------------|
| **P**   | Open (HIGH)      | 0Ω (LOW)         | Open (HIGH)      |
| **R**   | 0Ω (LOW)         | 0Ω (LOW)         | Open (HIGH)      |
| **N**   | Open (HIGH)      | 0Ω (LOW)         | Open (HIGH)      |
| **D**   | Open (HIGH)      | 0Ω (LOW)         | 0Ω (LOW)         |
| **3**   | Open (HIGH)      | Open (HIGH)      | 0Ω (LOW)         |
| **2**   | Open (HIGH)      | Open (HIGH)      | Open (HIGH)      |
| **1**   | 0Ω (LOW)         | Open (HIGH)      | Open (HIGH)      |

### Step 4: Record Results
Write down the actual resistance values you measured. If they match the expected pattern → you're good. If not → see "Non-Standard Pattern" below.

---

## Troubleshooting

### All Switches Read the Same (No Change with Shifter Movement)
**Possible causes:**
- Bad pressure switch manifold (common failure on high-mileage 4L60-E)
- Wiring harness fault (broken wire, corrosion)
- Transmission not receiving hydraulic pressure (pump not running, low fluid)

**Fix:**
- Replace pressure switch manifold (~$20-50 part)
- Check wiring continuity
- Check transmission fluid level

### Intermittent / Flickering Readings
**Possible causes:**
- Loose connector
- Corroded pins
- Worn manual valve or valve body

**Fix:**
- Clean connector with contact cleaner
- Check pin tension
- Inspect valve body (requires trans disassembly)

### One Switch Always Reads Wrong
**Possible causes:**
- Single switch failure inside manifold
- Broken wire for that specific switch
- Shorted wire (if always reads LOW)

**Fix:**
- Check wiring continuity for that switch
- Replace pressure switch manifold

---

## Non-Standard Pattern Detected

If your transmission's pressure switches don't match the OEM table, you have two options:

### Option 1: Update Firmware Truth Table (Recommended)
Edit `firmware/src/pressure_switches.cpp`, function `DecodePosition()`:

```cpp
ManualPosition PressureSwitches::DecodePosition(bool a, bool b, bool c) const {
  // YOUR VERIFIED TRUTH TABLE
  // Replace the patterns below with YOUR measured values
  
  if (a == HIGH && b == LOW && c == HIGH) {
    return ManualPosition::kParkNeutral;
  }
  // ... add your other positions here
}
```

Recompile and upload firmware.

### Option 2: Use Pressure Switches for Diagnostics Only
If the pattern is too inconsistent or non-standard, you can:
1. Leave the code as-is (will read `kUnknown` for most positions)
2. Use pressure switches only for **fault detection** (stuck switches, wiring faults)
3. Rely on **range selector inputs** (Pins 6/7/8/9) for gear selection instead

**Trade-off:**
- Range selector inputs tell you **shifter position** (reliable, direct wiring)
- Pressure switches tell you **hydraulic state** (diagnostic confirmation)
- For a manual-mode controller, range selector is primary; pressure switches are backup validation

---

## Recording Your Results

Use this template to document your transmission's truth table:

```
Transmission: GM 4L60-E
Year/Model: _______________
Date Verified: _______________
Tested By: _______________

┌────────┬──────────┬──────────┬──────────┐
│Shifter │ Switch A │ Switch B │ Switch C │
├────────┼──────────┼──────────┼──────────┤
│   P    │          │          │          │
│   R    │          │          │          │
│   N    │          │          │          │
│   D    │          │          │          │
│   3    │          │          │          │
│   2    │          │          │          │
│   1    │          │          │          │
└────────┴──────────┴──────────┴──────────┘

Notes:
_______________________________________________
_______________________________________________
```

**Save this data in:** `CALIBRATION_NOTES.md` (Section 2)

---

## After Verification

Once you've confirmed the truth table:

1. ✅ Update `CALIBRATION_NOTES.md` with your verified table
2. ✅ If non-standard, update `firmware/src/pressure_switches.cpp`
3. ✅ Remove the warning from `firmware/src/main.cpp` setup():
   ```cpp
   Serial.println("✅ Pressure switch truth table VERIFIED");
   ```
4. ✅ Mark as complete in `TODO.md`:
   ```
   - [x] **Pressure switches:** Truth table verified on trans
   ```

---

## Questions?

- **Can I skip this test?** Not recommended. Wrong truth table = wrong gear confirmation = potential safety issue.
- **What if I don't have a multimeter?** Use Method 1 (controller + Serial monitor) — easier anyway.
- **Do I need to start the engine?** No. Key ON is enough to power the solenoids and allow shifter movement.
- **Can I test in the car or on a lift?** Either works. Just make sure parking brake is engaged and wheels can't roll.

---

**Last Updated:** 2026-02-06
