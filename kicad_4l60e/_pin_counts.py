from pathlib import Path

dq = chr(34)
text = Path('kicad_4l60e/inputs_rpm.kicad_sch').read_text(encoding='utf-8')

def extract_block(text, start_idx):
 depth = 0
 i = start_idx
 while i < len(text):
 ch = text[i]
 if ch == '(':
 depth += 1
 elif ch == ')':
 depth -= 1
 if depth == 0:
 return text[start_idx:i+1], i + 1
 i += 1
 raise ValueError('Unbalanced block')

def find_lib_symbols_block(text):
 start = text.find('(lib_symbols')
 block, _ = extract_block(text, start)
 return block

def parse_symbol_blocks(lib_block):
 blocks = {}
 idx = 0
 while True:
 start = lib_block.find('(symbol ' + dq, idx)
 if start == -1:
 break
 block, next_idx = extract_block(lib_block, start)
 name = block.split(dq)[1]
 blocks[name] = block
 idx = next_idx
 return blocks

def parse_pin_positions(symbol_name, block):
 pin_map = {}
 lines = block.splitlines()
 in_sub = False
 sub_depth = 0
 sub_unit = None
 in_pin = False
 pin_depth = 0
 pin_num = None
 pin_x = None
 pin_y = None
 for ln in lines:
 line = ln.strip()
 if line.startswith('(symbol ' + dq) and not line.startswith('(symbol ' + dq + symbol_name + dq):
 name = line.split(dq)[1]
 parts = name.split('_')
 if len(parts) >= 3 and parts[-1].isdigit() and parts[-2].isdigit():
 sub_unit = int(parts[-2])
 else:
 sub_unit = None
 in_sub = True
 sub_depth = line.count('(') - line.count(')')
 continue
 if in_sub:
 sub_depth += line.count('(') - line.count(')')
 if line.startswith('(pin '):
 in_pin = True
 pin_depth = line.count('(') - line.count(')')
 pin_num = None
 pin_x = None
 pin_y = None
 if in_pin:
 pin_depth += line.count('(') - line.count(')')
 if line.startswith('(at '):
 parts = line.replace('(', ' ').replace(')', ' ').split()
 if len(parts) >= 3 and parts[0] == 'at':
 pin_x = float(parts[1])
 pin_y = float(parts[2])
 if line.startswith('(number '):
 parts = line.split(dq)
 if len(parts) >= 2:
 pin_num = parts[1]
 if pin_depth == 0:
 if pin_num and pin_x is not None and pin_y is not None and sub_unit is not None:
 pin_map.setdefault(sub_unit, []).append((pin_num, pin_x, pin_y))
 in_pin = False
 pin_num = None
 pin_x = None
 pin_y = None
 if sub_depth == 0:
 in_sub = False
 sub_unit = None
 return pin_map

lib_block = find_lib_symbols_block(text)
blocks = parse_symbol_blocks(lib_block)
for name, block in blocks.items():
 pm = parse_pin_positions(name, block)
 print(name, {k: len(v) for k,v in pm.items()})
