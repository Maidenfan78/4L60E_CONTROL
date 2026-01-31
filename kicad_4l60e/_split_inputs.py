from pathlib import Path
import uuid

source = Path('kicad_4l60e/_backup_080732/kicad_4l60e.kicad_sch')
lines = source.read_text(encoding='utf-8').splitlines()

header_end = next(i for i,l in enumerate(lines) if '(lib_symbols' in l)
section_end = next(i for i,l in enumerate(lines) if '(sheet_instances' in l)
symbol_start = next(i for i,l in enumerate(lines) if '(symbol_instances' in l)
embedded_start = next(i for i,l in enumerate(lines) if '(embedded_fonts' in l)

header_lines = lines[:header_end+1]
embedded_line = lines[embedded_start]

symbol_lines = lines[symbol_start+1:embedded_start-1]

value_order = [
 ('TPS_IN', 'TPS', 'inputs_tps.kicad_sch'),
 ('BRAKE_IN', 'Brake', 'inputs_brake.kicad_sch'),
 ('VSS_VR', 'VSS', 'inputs_vss.kicad_sch'),
 ('RPM_IN', 'RPM', 'inputs_rpm.kicad_sch'),
 ('RANGE_SEL', 'Range Selector', 'inputs_range.kicad_sch'),
 ('PSW_ABC', 'Pressure Switches', 'inputs_pressure_switches.kicad_sch'),
 ('TRANS_TEMP', 'Trans Temp', 'inputs_temp.kicad_sch'),
 ('LINE_PRESS', 'Line Pressure', 'inputs_line_pressure.kicad_sch'),
]

starts = []
for value, name, fname in value_order:
 idx = next(i for i,l in enumerate(lines) if value in l)
 j = idx
 while j >= 0 and not lines[j].lstrip().startswith('(symbol'):
 j -= 1
 starts.append((j, value, name, fname))

starts.sort(key=lambda x: x[0])
sections = []
for i, (start_idx, value, name, fname) in enumerate(starts):
 end_idx = section_end if i == len(starts)-1 else starts[i+1][0]
 sections.append((name, fname, start_idx, end_idx))


def update_header(title, comment):
 out = []
 new_uuid = str(uuid.uuid4())
 for line in header_lines:
 if line.lstrip().startswith('(uuid '):
 indent = line[:len(line) - len(line.lstrip())]
 out.append(f'{indent}(uuid {new_uuid})')
 elif line.lstrip().startswith('(title '):
 indent = line[:len(line) - len(line.lstrip())]
 out.append(f'{indent}(title {title})')
 elif line.lstrip().startswith('(comment 1 '):
 indent = line[:len(line) - len(line.lstrip())]
 out.append(f'{indent}(comment 1 {comment})')
 else:
 out.append(line)
 if not any('(comment 1 ' in l for l in out):
 for i, l in enumerate(out):
 if '(rev ' in l:
 indent = l[:len(l) - len(l.lstrip())]
 out.insert(i+1, f'{indent}(comment 1 {comment})')
 break
 return out


def refs_in_content(content):
 refs = set()
 for line in content:
 if '(property Reference' in line:
 parts = line.split('Reference ', 1)
            if len(parts) == 2:
                ref = parts[1].split('', 1)[0]
 refs.add(ref)
 return refs


# Write child sheets
for name, fname, start_idx, end_idx in sections:
 content = lines[start_idx:end_idx]
 refs = refs_in_content(content)
 filtered = []
 for line in symbol_lines:
 if '(reference ' in line:
            ref = line.split('reference ', 1)[1].split('', 1)[0]
            if ref in refs:
                filtered.append(line)
        else:
            filtered.append(line)

    out = []
    out += update_header('4L60E Controller - Input Conditioning', name)
    out.append('')
    out += content
    out.append('')
    out += [
        '  (sheet_instances',
        '    (path /',
        '      (page 1)',
        '    )',
        '  )',
        '',
        '  (symbol_instances',
    ]
    out += filtered
    out += [
        '  )',
        '',
        embedded_line,
        ')',
    ]
    Path('kicad_4l60e', fname).write_text('\n'.join(out) + '\n', encoding='utf-8')

# Write root overview sheet
root_out = []
root_out += update_header('4L60E Controller - Input Conditioning (Overview)', 'Input sheets')
root_out.append('')

sheet_x = 20
sheet_y = 40
sheet_w = 80
sheet_h = 20
sheet_gap = 8

for i, (name, fname, _, _) in enumerate(sections):
    row = i // 2
    col = i % 2
    x = sheet_x + col * (sheet_w + 20)
    y = sheet_y + row * (sheet_h + sheet_gap)
    uid = str(uuid.uuid4())
    root_out += [
        '  (sheet',
        f'    (at {x} {y} 0)',
        f'    (size {sheet_w} {sheet_h})',
        f'    (uuid {uid})',
        f'    (property Sheet name {name}',
        f'      (at {x} {y-2} 0)',
        '      (effects (font (size 1.27 1.27)))',
        '    )',
        f'    (property Sheet file {fname}',
        f'      (at {x} {y-4} 0)',
        '      (effects (font (size 1.27 1.27)))',
        '    )',
        '  )',
        '',
    ]

root_out += [
    '  (sheet_instances',
    '    (path /',
    '      (page 1)',
    '    )',
    '  )',
    '',
    '  (symbol_instances',
    '  )',
    '',
    embedded_line,
    ')',
]

Path('kicad_4l60e', 'kicad_4l60e.kicad_sch').write_text('\n'.join(root_out) + '\n', encoding='utf-8')
