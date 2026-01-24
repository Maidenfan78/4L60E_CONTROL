# TODO

## Firmware
- Define VSS interrupt capture and speed calculation.
- Implement TPS-based EPC base pressure mapping.
- Add TCC enable/disable logic with brake + speed checks.
- Build the manual-mode gear state machine and shift bump.
- Add sensor validation and failsafe escalation paths.

## Hardware
- Finalize solenoid driver schematic and BOM.
- Confirm pin mapping against Teensy 4.1 board layout.
- Decide on VSS conditioning hardware (VR/Hall).
- Specify power protection components (TVS, reverse polarity).

## Tooling
- Add a minimal flashing/monitor script for PlatformIO.
- Add a basic host-side unit test harness.
