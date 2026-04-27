"""Export the dragon_img C array to a PNG for HTML preview."""
import re
from PIL import Image

SRC = r"c:\Users\ADMIN\Downloads\LVGL_ESP32\dragon_img.c"
OUT = r"c:\Users\ADMIN\Downloads\LVGL_ESP32\dragon_preview.png"

with open(SRC, 'r') as f:
    text = f.read()

# Get dimensions
w = int(re.search(r'\.header\.w = (\d+)', text).group(1))
h = int(re.search(r'\.header\.h = (\d+)', text).group(1))
print(f"Image size: {w}x{h}")

# Parse hex bytes
start = text.index('{') + 1
end = text.index('};')
data_str = text[start:end]
hex_vals = re.findall(r'0x([0-9a-fA-F]{2})', data_str)
raw = bytes(int(hv, 16) for hv in hex_vals)
print(f"Data bytes: {len(raw)} (expected {w*h*2})")

# Convert RGB565 swapped to RGB888
img = Image.new('RGB', (w, h))
pixels = []
for i in range(0, len(raw), 2):
    hi = raw[i]
    lo = raw[i+1]
    rgb565 = (hi << 8) | lo
    r = ((rgb565 >> 11) & 0x1F) << 3
    g = ((rgb565 >> 5) & 0x3F) << 2
    b = (rgb565 & 0x1F) << 3
    pixels.append((r, g, b))

img.putdata(pixels)
img.save(OUT)
print(f"Saved to {OUT}")
