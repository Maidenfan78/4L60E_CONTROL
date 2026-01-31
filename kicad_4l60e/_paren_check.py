from pathlib import Path
text = Path('kicad_4l60e/kicad_4l60e.kicad_sch').read_text(encoding='utf-8')
count = 0
neg = None
for i, ch in enumerate(text):
 if ch == '(':
  count += 1
 elif ch == ')':
  count -= 1
  if count < 0:
   neg = i
   break
print('final', count)
print('negative', neg)
