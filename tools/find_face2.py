"""Zoom into dragon head to find exact eye/mouth positions."""
from PIL import Image, ImageDraw

img = Image.open(r"c:\Users\ADMIN\Downloads\LVGL_ESP32\dragon_preview.png")

# Crop head region: roughly x=50-130, y=0-70
head = img.crop((50, 0, 136, 75))

# Scale up 4x for visibility
head_big = head.resize((head.width * 4, head.height * 4), Image.NEAREST)
draw = ImageDraw.Draw(head_big)

# Draw grid (every 5px in original = every 20px in scaled)
for x in range(0, head.width, 5):
    sx = x * 4
    draw.line([(sx,0),(sx,head_big.height)], fill=(80,80,80), width=1)
    draw.text((sx+1, 1), str(x+50), fill=(200,200,200))
for y in range(0, head.height, 5):
    sy = y * 4
    draw.line([(0,sy),(head_big.width,sy)], fill=(80,80,80), width=1)
    draw.text((1, sy+1), str(y), fill=(200,200,200))

head_big.save(r"c:\Users\ADMIN\Downloads\LVGL_ESP32\dragon_head_zoom.png")
print("Saved zoomed head")

# Also find the darkest pixels specifically
import numpy as np
arr = np.array(img)
# Head area y=10-50, x=60-120
region = arr[10:50, 60:120, :]
brightness = region.sum(axis=2)
# Find local minima (darkest spots = likely eyes)
for y in range(region.shape[0]):
    for x in range(region.shape[1]):
        if brightness[y,x] < 60:  # very dark
            abs_x = x + 60
            abs_y = y + 10
            r,g,b = arr[abs_y, abs_x]
            print(f"  Dark pixel at ({abs_x},{abs_y}): rgb=({r},{g},{b}) bright={brightness[y,x]}")
