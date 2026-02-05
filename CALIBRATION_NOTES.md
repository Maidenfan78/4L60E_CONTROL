# Calibration & Configuration Data

This file tracks **vehicle-specific** and **transmission-specific** calibration data that must be measured/verified before firmware tuning.

---

## 1) Transmission Temperature Sensor

**Status:** ✅ OEM SENSOR DATA CONFIRMED (2026-02-06)

### Original GM 4L60-E Sensor (NTC Thermistor)

**Confirmed Resistance Values (from GM eSI):**

| Temperature | Resistance (Ω) | Notes |
|-------------|----------------|-------|
| 68°F (20°C) | 3088 - 3942    | Room temp / cold start |
| 212°F (100°C) | 159 - 198    | Boiling point / hot operation |

**Typical operating values:**
- Cold trans (~70°F / 21°C): ~3500 - 4000 Ω
- Normal operating (~190°F / 88°C): ~200 - 250 Ω
- Hot (~230°F / 110°C): ~120 - 150 Ω
- Over-temp (>250°F / 121°C): <100 Ω

**Sensor type:** NTC (Negative Temperature Coefficient) — resistance **decreases** as temperature **increases**

**Connector:** Pins L & M on transmission case 20-pin connector

---

### Your Circuit:
- 3.3kΩ pullup to 3.3V (on Teensy Pin A1)
- NTC thermistor to ground
- Voltage divider: V_out = 3.3V × (R_NTC / (R_NTC + 3300Ω))

**Calculated ADC Voltages:**

| Temp (°F) | Temp (°C) | R_NTC (Ω) | V_ADC (calculated) | ADC 10-bit (0-1023) |
|-----------|-----------|-----------|--------------------|--------------------|
| 68        | 20        | 3500      | 1.70V              | ~523               |
| 140       | 60        | 850       | 0.67V              | ~206               |
| 190       | 88        | 225       | 0.21V              | ~65                |
| 212       | 100       | 178       | 0.17V              | ~52                |
| 230       | 110       | 135       | 0.13V              | ~40                |

**Note:** With a 3.3kΩ pullup, the voltage drops quickly at higher temps (lower resistance).
This gives you good resolution in the critical 180-230°F range.

---

### Firmware Implementation (Lookup Table Recommended):

```cpp
// Lookup table for GM 4L60-E temp sensor with 3.3kΩ pullup
// ADC value (10-bit) → Temperature (°C)
const uint16_t temp_adc_table[] = {
    523,  // 20°C (68°F) - room temp
    400,  // 30°C (86°F)
    280,  // 40°C (104°F)
    206,  // 60°C (140°F)
    150,  // 70°C (158°F)
    100,  // 80°C (176°F)
    65,   // 88°C (190°F) - normal operating
    52,   // 100°C (212°F)
    40    // 110°C (230°F) - hot
};

const int8_t temp_celsius_table[] = {
    20, 30, 40, 60, 70, 80, 88, 100, 110
};

// Linear interpolation between points
int8_t GetTransTemp(uint16_t adc_value) {
    // (implementation left as exercise)
}
```

**Over-temp thresholds:**
- **130°C (266°F):** Disable TCC, increase line pressure
- **135°C (275°F):** Fault condition, force max pressure, hold current gear

---

### TODO:
- [ ] Bench test: measure actual ADC values at room temp (should be ~520)
- [ ] Verify ADC values match lookup table during first warm-up
- [ ] Fine-tune lookup table if needed based on real measurements
- [ ] Implement over-temp protection in firmware

---

## 2) Pressure Switch Truth Table

**Status:** ✅ OEM TABLE CONFIRMED (2026-02-06)

### Confirmed (GM 4L60-E Standard - MegaShift Documentation):

**Shifter Position → Switch States:**

| Gear Lever Position | Switch A | Switch B | Switch C | Notes |
|---------------------|----------|----------|----------|-------|
| Park / Neutral      | HIGH     | LOW      | HIGH     | Cannot distinguish P from N |
| Reverse             | LOW      | LOW      | HIGH     | Unique pattern |
| 1st (D1)            | LOW      | HIGH     | HIGH     | Unique pattern |
| 2nd (D2)            | HIGH     | HIGH     | HIGH     | All switches closed |
| 3rd (D3)            | HIGH     | HIGH     | LOW      | Unique pattern |
| 4th (D4 / Drive)    | HIGH     | LOW      | LOW      | Unique pattern |

**Source:** MegaShift 4L60-E documentation (msgpio.com/manuals/mshift/operation.html)

---

### Hardware Implementation (YOUR CIRCUIT):

Pressure switches are **ground-switched** (normally open, close when hydraulic pressure present):
- Each switch has a **10kΩ pullup to 3.3V** on the controller
- Switch OPEN (no pressure) → MCU reads **HIGH** (3.3V)
- Switch CLOSED (pressure present) → MCU reads **LOW** (0V, pulled to GND)

**Connector Pins:**
- Teensy Pin 20 → Pressure Switch A
- Teensy Pin 21 → Pressure Switch B
- Teensy Pin 22 → Pressure Switch C
- Common GND at transmission case connector

---

### Important Notes:

1. **Manual Valve Position, NOT Actual Gear:**
   - These switches tell you where the **shifter** is, not what gear the transmission is actually in
   - Example: Shifter in "D" (4th position) → switches read HIGH / LOW / LOW
   - But transmission could still be in 1st, 2nd, or 3rd due to solenoid control

2. **Cannot Distinguish Park from Neutral:**
   - Both read: HIGH / LOW / HIGH
   - This is normal for 4L60-E (no hydraulic pressure in either position)
   - Use separate Park/Neutral switch if needed (typically on column shifter)

3. **Use for Gear Confirmation:**
   - Compare commanded gear (solenoid states) with pressure switch readings
   - If mismatch persists for >2 seconds → shift fault, trigger failsafe
   - Example: Commanded 2nd gear (SSA OFF, SSB ON) but switches read 3rd pattern → stuck shift

4. **Diagnostic Codes:**
   - If switches never change → broken wiring or bad manifold
   - If impossible pattern appears (e.g., all LOW) → wiring short

---

### Firmware Implementation:

```cpp
// Read pressure switches (active LOW, inverted logic)
bool sw_a = digitalRead(hw::kPinPressureSwitchA);
bool sw_b = digitalRead(hw::kPinPressureSwitchB);
bool sw_c = digitalRead(hw::kPinPressureSwitchC);

// Decode manual valve position
enum class ManualPosition {
    PARK_NEUTRAL,  // HIGH, LOW, HIGH (cannot distinguish)
    REVERSE,       // LOW, LOW, HIGH
    FIRST,         // LOW, HIGH, HIGH
    SECOND,        // HIGH, HIGH, HIGH
    THIRD,         // HIGH, HIGH, LOW
    FOURTH,        // HIGH, LOW, LOW
    UNKNOWN        // Invalid pattern
};

ManualPosition GetManualPosition(bool a, bool b, bool c) {
    if (a == HIGH && b == LOW && c == HIGH) return ManualPosition::PARK_NEUTRAL;
    if (a == LOW && b == LOW && c == HIGH) return ManualPosition::REVERSE;
    if (a == LOW && b == HIGH && c == HIGH) return ManualPosition::FIRST;
    if (a == HIGH && b == HIGH && c == HIGH) return ManualPosition::SECOND;
    if (a == HIGH && b == HIGH && c == LOW) return ManualPosition::THIRD;
    if (a == HIGH && b == LOW && c == LOW) return ManualPosition::FOURTH;
    return ManualPosition::UNKNOWN;
}
```

---

### Verification Procedure (Still Recommended):

Even though this is the OEM standard, **test on YOUR transmission** to confirm:

1. Connect controller to transmission (engine OFF, key ON)
2. Manually move shifter through all positions: P → R → N → D → 3 → 2 → 1
3. Read pressure switch states via Serial monitor
4. Compare to table above
5. If all match → you're good to go
6. If mismatch → document your actual table (could be non-standard valve body)

### TODO:
- [ ] Bench test pressure switches with multimeter (shifter in each position)
- [ ] Verify truth table matches OEM standard
- [ ] Implement gear confirmation logic in firmware (Phase 2)
- [ ] Test diagnostic detection (stuck switches, impossible patterns)

---

## 3) Brake Switch Configuration

**Status:** ✅ VERIFIED (2026-02-06)

### Confirmed Configuration:
- Brake switch is **+12V-switched** (voltage-switched)
- One terminal: switched +12V from fuse box
- Other terminal: to controller (originally went to OEM TCM)
- When brake pressed: switch closes, +12V applied to controller input

### Hardware Design (CORRECT AS-IS):
- 47kΩ / 12kΩ voltage divider scales +12V down to ~2.4V at MCU pin
- 1kΩ series resistor + dual Schottky clamps protect MCU pin
- Brake NOT pressed: MCU pin reads LOW (~0V)
- Brake pressed: MCU pin reads HIGH (~2.4V)

### Firmware Logic:
```cpp
bool brake_pressed = digitalRead(hw::kPinBrake);
// HIGH = brake pressed, LOW = brake not pressed
```

### No Changes Required
Current schematic and firmware logic are correct for this vehicle.

---

## 4) Line Pressure Sensor Calibration (Optional)

**Status:** NOT INSTALLED YET (Optional sensor)

If you install a line pressure sensor (0.5-4.5V, 0-300 psi):

### Circuit:
- Sensor powered from 5V_TEENSY rail (Pin 1 = +5V, Pin 3 = GND)
- Signal (Pin 2) through 15kΩ / 24kΩ divider → 4.5V becomes 2.77V at ADC
- ADC read on Teensy Pin A2

### Calibration Procedure:
1. Install mechanical pressure gauge on transmission (test port)
2. Start engine, transmission in Park
3. Record:
   - Idle pressure (psi) vs ADC value
   - Increased throttle pressure vs ADC values
4. Generate lookup table or linear equation:
   ```cpp
   float psi = (adc_value - zero_offset) * scale_factor;
   ```

### Uses:
- EPC tuning (verify actual pressure vs commanded)
- Failsafe verification (confirm max pressure on fault)
- Shift quality tuning (shift bump magnitude)

### TODO:
- [ ] Install line pressure sensor (1/8" NPT tap on case)
- [ ] Install mechanical gauge for calibration
- [ ] Record ADC vs pressure data
- [ ] Implement conversion in firmware

---

## 5) EPC Pressure Command Table (Tuning)

**Status:** NOT TUNED YET

### Starting Point (Conservative):

| TPS (%)  | Pressure Command (%) | Notes                      |
|----------|----------------------|----------------------------|
| 0        | 40                   | Idle / coast               |
| 10       | 42                   | Light cruise               |
| 25       | 45                   | Moderate throttle          |
| 50       | 55                   | Medium load                |
| 75       | 65                   | Heavy load                 |
| 100      | 75                   | WOT (do NOT exceed ~75%)   |

**Shift Bump:** +20% for 300ms during gear change

**Conversion to EPC Duty:**
```cpp
// Pressure Command % → EPC Duty %
// Inversion formula (exhaust-bleed EPC):
epc_duty = map(pressure_cmd, 0, 100, 60, 5);
// Pressure 0% (min) → EPC 60% duty (hard limit for thermal protection)
// Pressure 100% (max) → EPC 5% duty (near OFF = max pressure)
```

### Tuning Procedure (REQUIRES LINE PRESSURE GAUGE):
1. Start with fixed pressure command (e.g., 50%)
2. Measure actual line pressure with gauge
3. Adjust pressure command table until pressure is appropriate:
   - Idle: ~90-110 psi (minimum)
   - Light throttle: ~110-140 psi
   - Heavy throttle: ~140-180 psi
   - Shift bump: +20-40 psi during shift
4. Test shift quality on dyno or test road
5. Refine shift bump duration and magnitude

### Safety Rules:
- Never allow pressure to drop below ~80 psi under load (clutch slip)
- Harsh shifts are better than clutch slip
- Start conservative (high pressure), reduce gradually
- If in doubt, add more pressure

### TODO:
- [ ] Bench test EPC PWM output (scope verification)
- [ ] Install line pressure gauge
- [ ] Tune pressure table with data logging
- [ ] Refine shift bump timing

---

## 6) Downshift Speed Limits

**Status:** NOT TUNED YET

### Starting Point (Conservative):

| Gear    | Maximum Vehicle Speed (mph) | Notes                          |
|---------|-----------------------------|---------------------------------|
| 1st     | 15                          | Prevents over-rev on downshift |
| 2nd     | 35                          | Safe for most engines          |
| 3rd     | 60                          | Rarely limited in practice     |

**Logic:**
- If shifter commands downshift AND speed exceeds limit → BLOCK shift, hold current gear
- Display warning (if using dashboard UI)

### Tuning Based on Engine:
- Holden Alloytec 3.6L V6 redline: ~6500 RPM (verify with MS3)
- 4L60-E gear ratios:
  - 1st: 3.06:1
  - 2nd: 1.63:1
  - 3rd: 1.00:1
  - 4th: 0.70:1
- Final drive: ??? (measure or look up for your vehicle)

### Calculation:
```
Max safe speed (mph) = (Redline RPM × Tire diameter (in) × 0.002975) / (Gear ratio × Final drive)
```

### TODO:
- [ ] Verify final drive ratio
- [ ] Calculate actual downshift limits for your setup
- [ ] Update firmware constants
- [ ] Test downshift protection on dyno (safe environment)

---

## 7) TCC Lockup Thresholds

**Status:** NOT TUNED YET

### Starting Point (Conservative):

| Parameter              | Value       | Notes                                   |
|------------------------|-------------|-----------------------------------------|
| Minimum gear           | 3rd         | Never lock in 1st/2nd                   |
| Minimum speed          | 45 mph      | Prevents stall at low speed             |
| Maximum TPS            | 20%         | Unlock under heavy throttle             |
| Lockup delay           | 3 seconds   | Prevents cycling after gear change      |
| Unlock on TPS spike    | >15% in 0.5s| Detects throttle transient (kickdown)   |
| Over-temp inhibit      | 130°C       | Disable TCC if trans is overheating     |

### TCC PWM Progressive Apply (Phase 4):
- Ramp TCC PWM duty from 0% → 80% over 800ms
- Smooth engagement prevents shudder
- Unlock: drop TCC PWM to 0% immediately, then turn off TCC enable

### Tuning Procedure:
1. Enable TCC in 4th gear only (safest)
2. Test lockup on flat road at 50 mph, light throttle
3. Monitor for shudder or harsh engagement
4. Adjust TCC PWM ramp rate and duration
5. Gradually expand enable conditions (3rd gear, lower speeds)

### TODO:
- [ ] Bench test TCC enable/disable logic
- [ ] Test TCC lockup on dyno first (controlled environment)
- [ ] Tune TCC PWM ramp rate on test drive
- [ ] Verify immediate unlock on brake input

---

## 8) Transmission Specifications (Vehicle-Specific)

**Record for reference:**

### Vehicle:
- Year: _____
- Make/Model: Holden (year/model?)
- Engine: Alloytec 3.6L V6
- ECU: MegaSquirt 3 + MS3X

### Transmission:
- Type: GM 4L60-E
- Year/Model: _____ (affects pressure switch logic, valve body calibration)
- Stock or modified: Stock valve body
- Torque converter: Stock or upgraded? _____

### Gear Ratios:
- 1st: 3.06:1
- 2nd: 1.63:1
- 3rd: 1.00:1
- 4th: 0.70:1

### Final Drive:
- Ratio: _____ (measure or look up)

### Tire Size:
- Diameter (inches): _____ (calculate VSS calibration)

### VSS:
- Pulses per mile: _____ (needed for speed calculation)
- Sensor type: VR (magnetic) or Hall-effect? _____

---

## Notes

**Safety First:**
- All initial testing should be done on a dyno or private test road
- Install a mechanical line pressure gauge BEFORE first test drive
- Start with conservative (high) pressure settings
- Test each phase incrementally (don't enable TCC until basic shifts work)

**Data Logging:**
- Log all sensor inputs, solenoid states, calculated values
- CSV format with timestamps
- Review logs after each test session
- Identify patterns before tuning

**Firmware Constants:**
Once calibrated, update these files:
- `firmware/config/config.h` (pressure tables, speed limits, TCC thresholds)
- `firmware/src/epc_control.cpp` (pressure command mapping)
- `firmware/src/tcc.cpp` (TCC enable logic)
- `firmware/src/state_machine.cpp` (downshift protection)

---

**Last Updated:** 2026-02-06 (initial template, no calibration data yet)
