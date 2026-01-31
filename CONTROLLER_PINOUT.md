# Controller Connector Pinout

External connector pinout for the 4L60E TCM controller enclosure.
Two panel-mount connectors separate high-current solenoid circuits from low-current signals.

---

## Connector 1 — Transmission (16 pins)

All wires run to/from the transmission case connector, VSS sensor, and pressure switches.

| Pin | Wire | Direction | Notes |
| ----- | ------ | --------- | ------- |
| 1 | SSA return | In | Shift solenoid A, low-side switched |
| 2 | SSB return | In | Shift solenoid B, low-side switched |
| 3 | EPC return | In | EPC solenoid, PWM low-side switched |
| 4 | TCC return | In | TCC solenoid, low-side switched |
| 5 | +12V solenoid feed | Out | Ignition-switched, fused, to solenoid common |
| 6 | Pressure switch A | In | Ground-switched, 10k pullup to 3.3V on PCB |
| 7 | Pressure switch B | In | Ground-switched, 10k pullup to 3.3V on PCB |
| 8 | Pressure switch C | In | Ground-switched, 10k pullup to 3.3V on PCB |
| 9 | TFT sensor signal | In | NTC thermistor, 4.7k pullup to 3.3V on PCB |
| 10 | VSS signal | In | VR sensor, AC coupled into LM2903P conditioner |
| 11 | VSS reference | In | VR sensor return (not chassis ground) |
| 12 | Power ground | — | Solenoid return current path |
| 13 | Power ground | — | Solenoid return current path |
| 14 | Sensor ground | — | TFT, pressure switches, dedicated path |
| 15 | TCC PWM return | In | TCC PWM solenoid, PWM low-side switched |
| 16 | Spare | — | Future use (3-2 solenoid) |

Total: 15 active + 1 spare

## Connector 2 — Vehicle (14 pins)

All wires run to vehicle-side sources: fuse box, cabin switches, engine sensors.

| Pin | Wire | Direction | Notes |
| ----- | ------ | --------- | ------- |
| 1 | +12V ignition power | In | Main controller supply, fused at source |
| 2 | +12V ignition power | In | Parallel pin for current capacity |
| 3 | Power ground | — | Run all 3 to same chassis earth point |
| 4 | Power ground | — | Do not join at connector |
| 5 | Power ground | — | Keep as separate wires to earth stud |
| 6 | Brake switch | In | +12V switched, through 33k/8.2k divider on PCB |
| 7 | TPS signal | In | 0–5V from MS3 TPS wire, through 15k/24k divider on PCB |
| 8 | RPM signal | In | 0–12V square wave from MS3 tach, through 30k/10k divider on PCB |
| 9 | Range selector 1 | In | Ground-switched, 10k pullup to 3.3V on PCB |
| 10 | Range selector 2 | In | Ground-switched, 10k pullup to 3.3V on PCB |
| 11 | Range selector 3 | In | Ground-switched, 10k pullup to 3.3V on PCB |
| 12 | Range selector D | In | Ground-switched, 10k pullup to 3.3V on PCB |
| 13 | Signal ground | — | Return for TPS, RPM, brake dividers |
| 14 | Spare | — | Future use |

Total: 13 active + 1 spare

---

## Pin count summary

| Connector | Active | Spare | Total |
| --------- | ------ | ----- | ----- |
| Transmission | 15 | 1 | 16 |
| Vehicle | 13 | 1 | 14 |
| Overall | 28 | 2 | 30 |

---

## Build notes

### Grounding rules

- **Power grounds** (Conn 1 pins 12–13, Conn 2 pins 3–5): carry solenoid and controller supply current. Run as separate wires all the way to the chassis earth stud — do not join them at or near the connector.
- **Sensor ground** (Conn 1 pin 14): carries only milliamps from TFT, pressure switches. Runs a dedicated wire back to the PCB signal ground. On the PCB, sensor ground and power ground meet at a single star point near the Teensy ground pin.
- **Signal ground** (Conn 2 pin 13): return path for the voltage divider networks (TPS, RPM, brake). Same star-point rule applies on the PCB.

### Solenoid wiring

- The +12V solenoid feed (Conn 1 pin 5) goes to the transmission case connector and feeds all solenoids internally through the valve body harness. Fuse this wire (5A) close to the power source.
- Solenoid return wires (Conn 1 pins 1–4, 15) each carry up to ~1.1A when energised. The MOSFETs on the PCB switch these to ground.
- TCC PWM return (Conn 1 pin 15) is the progressive converter clutch apply solenoid. Uses PWM low-side switching with diode flyback (same topology as EPC) for smooth engagement.
- When all solenoids are off (SSA OFF, SSB OFF), the transmission defaults to 3rd gear (GM failsafe). This is the safe power-loss state.

### VSS sensor

- The VR sensor is a two-wire AC source — it is NOT ground-referenced. Run both VSS signal and VSS reference as a twisted pair from the sensor to the connector. Do not ground the VSS reference wire to chassis at the transmission end.

### Input protection

- All external-voltage inputs (brake 12V, TPS 5V, RPM 12V) have resistor dividers and dual BAT85 Schottky clamps on the PCB. The connector carries the raw signal — all conditioning happens on the board.
- Ground-switched inputs (range selector, pressure switches) have 10k pullups to 3.3V on the PCB. The connector just passes the switch wire through.

### Wire gauge recommendations

- Solenoid returns + 12V feed: 18–20 AWG (rated for ~1A continuous per wire)
- Power supply + grounds: 18 AWG minimum (multiple amps through controller supply)
- Sensor and signal wires: 22–24 AWG (milliamp currents)
- VSS twisted pair: 22 AWG shielded (noise rejection for VR sensor)

### Connector selection

- Panel-mount (bulkhead) type connectors rated for automotive vibration.
- Transmission connector: 5A+ per pin minimum (solenoid current).
- Vehicle connector: 3A+ per pin sufficient (mostly signal level).
- GX20-16 and GX20-14 (aviation style) or Deutsch DTM panel-mount are suitable options.
- Always use keyed/polarised connectors — a mis-mated plug on a transmission controller can destroy solenoids or the PCB.

### Spare pins

- One spare on the transmission connector allows adding the 3-2 shift solenoid later without changing connectors.
- One spare on the vehicle connector for future expansion (e.g. boost solenoid signal, shift light output).
