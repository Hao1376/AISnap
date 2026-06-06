"""Check the RGB565 data in test_image_display.h"""
import os

header_path = os.path.join(os.path.dirname(__file__), '..', 'App', 'plam_search', 'AI', 'test_image_display.h')

with open(header_path, 'r') as f:
    lines = f.readlines()

# Find array start
start = -1
for i, line in enumerate(lines):
    if 'test_image_display_rgb565' in line:
        start = i
        break

if start < 0:
    print("ERROR: Array not found!")
    exit(1)

print(f"Array declaration line {start+1}: {lines[start].strip()}")

# Collect all hex values
vals = []
for i in range(start + 1, start + 5):
    text = lines[i].strip().rstrip(',')
    for token in text.split(','):
        token = token.strip()
        if token.startswith('0x') or token.startswith('0X'):
            vals.append(token)
            if len(vals) >= 16:
                break
    if len(vals) >= 16:
        break

print(f"\nFirst {len(vals)} pixel values:")
print("  RAW        R*8  G*4  B*8  (approximate RGB888)")
for v in vals:
    val = int(v, 16)
    r = (val >> 11) & 0x1F
    g = (val >> 5) & 0x3F
    b = val & 0x1F
    print(f"  {v}  -> {r*8:3d} {g*4:3d} {b*8:3d}")

# Check a range in the middle of the image (should be skin tones)
mid_line = start + 640 * 240  # middle of image
if mid_line < len(lines):
    print(f"\nPixel at row 240, col 0 (line {mid_line+1}):")
    text = lines[mid_line].strip().rstrip(',')
    tokens = [t.strip() for t in text.split(',') if t.strip().startswith('0x')]
    if tokens:
        val = int(tokens[0], 16)
        r = (val >> 11) & 0x1F
        g = (val >> 5) & 0x3F
        b = val & 0x1F
        print(f"  {tokens[0]} -> R={r}*8={r*8}, G={g}*4={g*4}, B={b}*8={b*8}")

# Check last pixel (row 479, col 639)
last_line = start + 640 * 479 + (640 // 8)  # approximate
if last_line < len(lines):
    print(f"\nLast pixels (line approx {last_line+1}):")
    text = lines[last_line].strip().rstrip(',')
    tokens = [t.strip() for t in text.split(',') if t.strip().startswith('0x')]
    if tokens:
        val = int(tokens[-1], 16) if tokens else 0
        r = (val >> 11) & 0x1F
        g = (val >> 5) & 0x3F
        b = val & 0x1F
        print(f"  {tokens[-1] if tokens else 'N/A'} -> R={r}*8={r*8}, G={g}*4={g*4}, B={b}*8={b*8}")

# Check total count
total = 0
for i in range(start + 1, len(lines)):
    text = lines[i].strip().rstrip(',')
    if text == '};':
        break
    for token in text.split(','):
        token = token.strip()
        if token.startswith('0x') or token.startswith('0X'):
            total += 1

print(f"\nTotal pixels: {total} (expected 640*480 = {640*480})")
if total == 640*480:
    print("Count OK!")
else:
    print("COUNT MISMATCH!")
