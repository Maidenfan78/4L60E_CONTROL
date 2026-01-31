# KiCad CLI Guide (AI-Optimized)

This is a condensed, task-focused guide for using KiCad 9.0 CLI (`kicad-cli`) in this repo.
It assumes Windows and KiCad installed at `C:\Program Files\KiCad\9.0\bin`.

## 1) Always use the full path

```powershell
$cli = 'C:\Program Files\KiCad\9.0\bin\kicad-cli.exe'
& $cli --help
```

## 2) Mental model

`kicad-cli` has top-level subcommands:
- `fp` (footprints)
- `jobset`
- `pcb`
- `sch` (schematics)
- `sym` (symbols)
- `version`

Each subcommand has its own `--help` and nested subcommands.

```powershell
& $cli sch --help
& $cli sch export pdf --help
```

## 3) Schematic tasks (most used)

### 3.1 ERC (Electrical Rules Check)

```powershell
& $cli sch erc --output kicad_4l60e\cli_out\inputs_erc.rpt kicad_4l60e\kicad_4l60e.kicad_sch
```

Options you will likely use:
- `--format report|json`
- `--severity-error|--severity-warning|--severity-all`
- `--exit-code-violations` (exit 5 if any violations)

### 3.2 Export schematic to PDF

```powershell
& $cli sch export pdf --output kicad_4l60e\cli_out\inputs.pdf kicad_4l60e\kicad_4l60e.kicad_sch
```

Useful flags:
- `--black-and-white`
- `--exclude-drawing-sheet`
- `--no-background-color`
- `--pages 1,2` (if multi-page)

### 3.3 Export schematic to SVG (per-sheet)

```powershell
& $cli sch export svg --output kicad_4l60e\cli_out kicad_4l60e\kicad_4l60e.kicad_sch
```

### 3.4 Export netlist

```powershell
& $cli sch export netlist --format kicadsexpr --output kicad_4l60e\cli_out\inputs.net kicad_4l60e\kicad_4l60e.kicad_sch
```

### 3.5 Export BOM (from symbol fields table)

```powershell
& $cli sch export bom --output kicad_4l60e\cli_out\inputs_bom.csv kicad_4l60e\kicad_4l60e.kicad_sch
```

Common controls:
- `--fields Reference,Value,Footprint,${QUANTITY},${DNP}`
- `--labels Refs,Value,Footprint,Qty,DNP`
- `--exclude-dnp`

## 4) PCB tasks (export outputs)

### 4.1 Gerbers

```powershell
& $cli pcb export gerbers --output kicad_4l60e\cli_out kicad_4l60e\kicad_4l60e.kicad_pcb
```

### 4.2 Drill files

```powershell
& $cli pcb export drill --output kicad_4l60e\cli_out kicad_4l60e\kicad_4l60e.kicad_pcb
```

### 4.3 STEP / SVG / PDF

```powershell
& $cli pcb export step --output kicad_4l60e\cli_out\board.step kicad_4l60e\kicad_4l60e.kicad_pcb
& $cli pcb export svg --output kicad_4l60e\cli_out kicad_4l60e\kicad_4l60e.kicad_pcb
& $cli pcb export pdf --output kicad_4l60e\cli_out\board.pdf kicad_4l60e\kicad_4l60e.kicad_pcb
```

## 5) Variables, themes, and drawing sheets

- Override project variables:

```powershell
& $cli sch export pdf -D PROJECT_REV=A -D AUTHOR=Sav --output out.pdf in.kicad_sch
```

- Override drawing sheet:

```powershell
& $cli sch export pdf --drawing-sheet C:\path\to\sheet.kicad_wks --output out.pdf in.kicad_sch
```

- Export with a specific theme:

```powershell
& $cli sch export pdf --theme KiCad Default --output out.pdf in.kicad_sch
```

## 6) AI usage pattern (recommended)

1) Always run `--help` for the specific subcommand you are about to use.
2) Prefer explicit `--output` paths.
3) Put generated outputs under `kicad_4l60e\cli_out` so they are easy to clean up.
4) Capture ERC output and fail if violations are unexpected.

## 7) Automation snippet (PowerShell batch exports)

```powershell
$cli = 'C:\Program Files\KiCad\9.0\bin\kicad-cli.exe'
$sch = 'kicad_4l60e\kicad_4l60e.kicad_sch'
$out = 'kicad_4l60e\cli_out'
New-Item -ItemType Directory -Force -Path $out | Out-Null

& $cli sch erc --exit-code-violations --output (Join-Path $out 'erc.rpt') $sch
& $cli sch export pdf --output (Join-Path $out 'schematic.pdf') $sch
& $cli sch export bom --output (Join-Path $out 'bom.csv') $sch
& $cli sch export netlist --format kicadsexpr --output (Join-Path $out 'netlist.net') $sch
```

## 7) Verified in this repo

These were executed successfully:

```powershell
& 'C:\Program Files\KiCad\9.0\bin\kicad-cli.exe' sch export pdf --output kicad_4l60e\cli_out\inputs.pdf kicad_4l60e\kicad_4l60e.kicad_sch
& 'C:\Program Files\KiCad\9.0\bin\kicad-cli.exe' sch erc --output kicad_4l60e\cli_out\inputs_erc.rpt kicad_4l60e\kicad_4l60e.kicad_sch
```

Outputs exist in `kicad_4l60e\cli_out`.

## 8) Troubleshooting

- If `kicad-cli` is not found, use the full path shown above.
- If PDF/SVG exports are blank, try `--no-background-color` or specify a theme.
- If ERC fails, add `--exit-code-violations` to detect errors in automation.

## 9) Local issues encountered (knowledge base)

- `kicad-cli` was not on `PATH` in this environment; fixed by calling `C:\Program Files\KiCad\9.0\bin\kicad-cli.exe` directly.
- Export outputs were easiest to manage by routing them into `kicad_4l60e\cli_out`.
- In PowerShell, use single quotes around paths with spaces (e.g. `$cli = 'C:\Program Files\KiCad\9.0\bin\kicad-cli.exe'`) to avoid command parsing errors.
- A hand-authored hierarchical overview schematic failed to load in `kicad-cli`; exported each input sheet individually and used a standalone overview schematic with a text index instead.

## 10) AI-generated schematic issues

### Problem: Wire routing doesn't match symbol pin positions

When programmatically generating `.kicad_sch` files, wire endpoints must exactly match symbol pin connection points. KiCad symbols have pins at specific offsets from their origin:

**Symbol pin positions (from KiCad standard libraries):**

| Symbol | Pin | Local Position | Direction |
|--------|-----|----------------|-----------|
| `Connector_Generic:Conn_01x02` | 1 | (-5.08, 0) | 0° (right) |
| `Connector_Generic:Conn_01x02` | 2 | (-5.08, -2.54) | 0° (right) |
| `Device:R` | 1 | (0, 3.81) | 270° (down) |
| `Device:R` | 2 | (0, -3.81) | 90° (up) |
| `Device:C` | 1 | (0, 2.54) | 270° (down) |
| `Device:C` | 2 | (0, -2.54) | 90° (up) |

**Calculating absolute pin position:**
```
absolute_pin = symbol_position + rotate(local_pin_position, symbol_rotation)
```

**Example:** Resistor R1 at (38.1, 38.1) rotated 90° CCW:
- Pin 1 local: (0, 3.81) → rotated 90° CCW → (-3.81, 0) → absolute: (34.29, 38.1)
- Pin 2 local: (0, -3.81) → rotated 90° CCW → (3.81, 0) → absolute: (41.91, 38.1)

### Problem: Empty `lib_symbols` section

KiCad schematics require symbol definitions in the `lib_symbols` section. If `lib_symbols` is empty:
- Symbols display as squares with "?" marks
- **ERC will report pin connection errors** even with correct wire routing
- CLI export (PDF/SVG) still works (resolves from system libraries)

### Fix: Embed full symbol definitions

The working solution is to embed complete symbol definitions from KiCad's standard libraries into the `lib_symbols` section. Each symbol needs:
- Property definitions (Reference, Value, Footprint, Datasheet, Description)
- Drawing primitives (rectangles, polylines, etc.)
- Pin definitions with positions and types

**Example structure:**
```
(lib_symbols
    (symbol "Device:R"
        (pin_numbers (hide yes))
        (pin_names (offset 0))
        (property "Reference" "R" ...)
        (property "Value" "R" ...)
        (symbol "R_0_1"
            (rectangle (start -1.016 -2.54) (end 1.016 2.54) ...)
        )
        (symbol "R_1_1"
            (pin passive line (at 0 3.81 270) (length 1.27) (number "1" ...))
            (pin passive line (at 0 -3.81 90) (length 1.27) (number "2" ...))
        )
    )
)
```

Symbol definitions can be extracted from:
```
C:\Program Files\KiCad\9.0\share\kicad\symbols\Device.kicad_sym
C:\Program Files\KiCad\9.0\share\kicad\symbols\Connector_Generic.kicad_sym
C:\Program Files\KiCad\9.0\share\kicad\symbols\power.kicad_sym
```

### Pin instance UUIDs

Each symbol instance needs `(pin "N" (uuid "..."))` entries for each pin. These are required for ERC to track connections.

### Expected ERC warnings for standalone sheets

When testing individual schematic sheets, expect these warnings:
- `power_pin_not_driven`: GND symbols have no power source (resolved when integrated)
- `global_label_dangling`: Labels connect to other sheets
- `lib_symbol_mismatch`: Embedded symbols may differ slightly from library versions

### Looking up symbol definitions

Symbol libraries are located at:
```
C:\Program Files\KiCad\9.0\share\kicad\symbols\
```

To find a symbol's pin positions:
```powershell
grep -A 100 'symbol "R"' "C:\Program Files\KiCad\9.0\share\kicad\symbols\Device.kicad_sym"
```

Look for `(pin ... (at X Y angle) ...)` entries to find connection points.
