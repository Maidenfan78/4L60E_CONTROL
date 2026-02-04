# Controller Connector Pinout

External connector pinout for the 4L60E TCM controller enclosure.
Two panel-mount connectors separate high-current solenoid circuits from low-current signals.

---

## Connector 1 — Transmission (18 pins)

All wires run to/from the transmission case connector, VSS sensor, and pressure switches.
Solenoid returns are low-side MOSFET switched outputs — current flows from the +12V feed through the solenoid and returns through these pins to ground on the PCB.

| Pin | Wire | Direction | Notes |
| --- | ---- | --------- | ----- |
| 1 | SSA return | Out | Shift solenoid A, low-side switched |
| 2 | SSB return | Out | Shift solenoid B, low-side switched |
| 3 | EPC return | Out | EPC solenoid, PWM low-side switched |
| 4 | TCC return | Out | TCC solenoid, low-side switched |
| 5 | TCC PWM return | Out | TCC PWM solenoid, PWM low-side switched (diode flyback) |
| 6 | +12V solenoid feed | — | Ignition-switched, fused, to solenoid common |
| 7 | +12V solenoid feed | — | Parallel pin for current capacity |
| 8 | Power ground | — | Solenoid return current path |
| 9 | Power ground | — | Solenoid return current path |
| 10 | Power ground | — | Solenoid return current path |
| 11 | Pressure switch A | In | Ground-switched, 10k pullup to 3.3V on PCB |
| 12 | Pressure switch B | In | Ground-switched, 10k pullup to 3.3V on PCB |
| 13 | Pressure switch C | In | Ground-switched, 10k pullup to 3.3V on PCB |
| 14 | TFT sensor signal | In | NTC thermistor, 4.7k pullup to 3.3V on PCB |
| 15 | Sensor ground | — | TFT, pressure switches, dedicated path |
| 16 | VSS signal | In | VR sensor, AC coupled into LM2903P conditioner |
| 17 | VSS reference | In | VR sensor return (not chassis ground) |
| 18 | Spare | — | Future use (3-2 shift solenoid) |

Total: 17 active + 1 spare

## Connector 2 — Vehicle (16 pins)

All wires run to vehicle-side sources: fuse box, cabin switches, engine sensors, and external peripherals.

| Pin | Wire | Direction | Notes |
| --- | ---- | --------- | ----- |
| 1 | +12V ignition power | In | Main controller supply, fused at source |
| 2 | +12V ignition power | In | Parallel pin for current capacity |
| 3 | Power ground | — | Run both to same chassis earth point |
| 4 | Power ground | — | Do not join at connector |
| 5 | Brake switch | In | +12V switched, through 33k/8.2k divider on PCB |
| 6 | TPS signal | In | 0–5V from MS3 TPS wire, through 15k/24k divider on PCB |
| 7 | RPM signal | In | 0–12V square wave from MS3 tach, through 30k/10k divider on PCB |
| 8 | Signal ground | — | Return for TPS, RPM, brake dividers |
| 9 | Range selector 1 | In | Ground-switched, 10k pullup to 3.3V on PCB |
| 10 | Range selector 2 | In | Ground-switched, 10k pullup to 3.3V on PCB |
| 11 | Range selector 3 | In | Ground-switched, 10k pullup to 3.3V on PCB |
| 12 | Range selector D | In | Ground-switched, 10k pullup to 3.3V on PCB |
| 13 | UART TX | Out | Teensy Serial2 TX (3.3V TTL), 1k series resistor on PCB |
| 14 | UART RX | In | Teensy Serial2 RX (3.3V TTL), 1k series resistor on PCB |
| 15 | UART ground | — | Dedicated return for external UART device |
| 16 | Spare | — | Future use |

Total: 15 active + 1 spare

---

## Pin count summary

| Connector | Active | Spare | Total |
| --------- | ------ | ----- | ----- |
| Transmission | 17 | 1 | 18 |
| Vehicle | 15 | 1 | 16 |
| Overall | 32 | 2 | 34 |

---

## Build notes

### Grounding rules

- **Power grounds** (Conn 1 pins 8–10, Conn 2 pins 3–4): carry solenoid and controller supply current. Run as separate wires all the way to the chassis earth stud — do not join them at or near the connector.
- **Sensor ground** (Conn 1 pin 15): carries only milliamps from TFT, pressure switches. Runs a dedicated wire back to the PCB signal ground. On the PCB, sensor ground and power ground meet at a single star point near the Teensy ground pin.
- **Signal ground** (Conn 2 pin 8): return path for the voltage divider networks (TPS, RPM, brake). Same star-point rule applies on the PCB.
- **UART ground** (Conn 2 pin 15): dedicated return for the external UART device. Joins signal ground at the PCB star point. Do not share this pin with other ground returns at the connector end.

### Solenoid wiring

- The +12V solenoid feed (Conn 1 pins 6–7) goes to the transmission case connector and feeds all solenoids internally through the valve body harness. Fuse this wire (5A) close to the power source. Two pins are used to share the current load — join them at the fuse output, not at the connector.
- Solenoid return wires (Conn 1 pins 1–5) each carry up to ~1.1A when energised. The MOSFETs on the PCB switch these to ground.
- TCC PWM return (Conn 1 pin 5) is the progressive converter clutch apply solenoid. Uses PWM low-side switching with diode flyback (same topology as EPC) for smooth engagement.
- When all solenoids are off (SSA OFF, SSB OFF), the transmission defaults to 3rd gear (GM failsafe). This is the safe power-loss state.

### VSS sensor

- The VR sensor is a two-wire AC source — it is NOT ground-referenced. Run both VSS signal and VSS reference as a twisted pair from the sensor to the connector. Do not ground the VSS reference wire to chassis at the transmission end.

### UART breakout

- The external UART port uses Teensy Serial2 (TX2/RX2), independent of the internal Pi-Teensy link on Serial1.
- 3.3V TTL levels only. If connecting to a 5V device, use an external level shifter.
- 1k series resistors on both TX and RX are on the PCB for protection.
- Suitable for: USB-TTL debug adapter, Bluetooth serial module (HC-05/06), dash data logger, or external display.
- Default baud rate: TBD (match firmware Serial2 configuration).

### Input protection

- All external-voltage inputs (brake 12V, TPS 5V, RPM 12V) have resistor dividers and dual BAT85 Schottky clamps on the PCB. The connector carries the raw signal — all conditioning happens on the board.
- Ground-switched inputs (range selector, pressure switches) have 10k pullups to 3.3V on the PCB. The connector just passes the switch wire through.

### Wire gauge recommendations

- Solenoid returns + 12V feed: 18–20 AWG (rated for ~1A continuous per wire)
- Power supply + grounds: 18 AWG minimum (multiple amps through controller supply)
- Sensor and signal wires: 22–24 AWG (milliamp currents)
- VSS twisted pair: 22 AWG shielded (noise rejection for VR sensor)
- UART wires: 22–24 AWG (milliamp currents, keep under 2m for reliable comms)

### Connector selection

- Panel-mount (bulkhead) type connectors rated for automotive vibration.
- Transmission connector (18-pin): 5A+ per pin minimum (solenoid current).
- Vehicle connector (16-pin): 3A+ per pin sufficient (mostly signal level).
- Deutsch DTM or DT series panel-mount are the industry standard for aftermarket ECUs. GX20 aviation style connectors are a cheaper alternative.
- Always use keyed/polarised connectors — a mis-mated plug on a transmission controller can destroy solenoids or the PCB.

### Spare pins

- One spare on the transmission connector allows adding the 3-2 shift solenoid later without changing connectors.
- One spare on the vehicle connector for future expansion (e.g. boost solenoid signal, shift light output).
