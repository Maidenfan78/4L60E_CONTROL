from pathlib import Path

path = Path('kicad_4l60e/inputs_tps.kicad_sch')
lines = path.read_text(encoding='utf-8').splitlines()


def paren_delta(line):
 return line.count('(') - line.count(')')


def extract_path_uuid(line):
 needle = '(path '
 idx = line.find(needle)
 if idx == -1:
  return None
 start = idx + len(needle)
 end = line.find('', start)
 if end == -1:
  return None
 return line[start:end]

symbol_uuids = set()
in_symbol = False
depth = 0
for ln in lines:
 if not in_symbol and '(symbol' in ln:
  in_symbol = True
  depth = paren_delta(ln)
 elif in_symbol:
  depth += paren_delta(ln)
 if in_symbol and '(uuid ' in ln:
  parts = ln.strip().split()
  if len(parts) >= 2:
   symbol_uuids.add(parts[1].strip(')'))
 if in_symbol and depth == 0:
  in_symbol = False

instance_uuids = []
for ln in lines:
 uuid = extract_path_uuid(ln)
 if uuid:
  instance_uuids.append(uuid)

print('symbol_uuids', symbol_uuids)
print('instance_uuids', instance_uuids)
