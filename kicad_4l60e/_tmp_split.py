from pathlib import Path
path = Path('kicad_4l60e\\kicad_4l60e.kicad_sch')
lines = path.read_text(encoding='utf-8').splitlines()
values = ['TPS_IN','BRAKE_IN','VSS_VR','RPM_IN','RANGE_SEL','PSW_ABC','TRANS_TEMP','LINE_PRESS']
starts = {}
for value in values:
 target = f'(property Value {value}'
 idx = next((i for i,l in enumerate(lines) if target in l), None)
 if idx is None:
 starts[value] = None
 continue
 j = idx
 while j >= 0 and not lines[j].lstrip().startswith('(symbol'):
 j -= 1
 starts[value] = j
for k,v in starts.items():
 print(k, v)
