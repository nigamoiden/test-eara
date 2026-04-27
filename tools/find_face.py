"""Find the approximate eye and mouth positions on the dragon image."""
from PIL import Image
import numpy as np

img = Image.open(r"c:\Users\ADMIN\Downloads\LVGL_ESP32\dragon_preview.png")
arr = np.array(img)
h, w = arr.shape[:2]
print(f"Image: {w}x{h}")

# The dragon head is in the upper portion, slightly right of center
# Eyes are the darkest small spots in the head area
# Let's scan the upper-right quadrant for dark pixels (eyes)

# Look at the upper 60% of the image, right 70%
head_region = arr[0:int(h*0.5), int(w*0.3):, :]

# Find very dark pixels (potential eyes)
darkness = head_region[:,:,0].astype(int) + head_region[:,:,1].astype(int) + head_region[:,:,2].astype(int)

# Find darkest clusters
from scipy import ndimage
dark_mask = darkness < 120  # very dark pixels

# Label connected components
labeled, num = ndimage.label(dark_mask)
print(f"Found {num} dark clusters in head region")

# Get centroids of dark clusters
for i in range(1, min(num+1, 20)):
    ys, xs = np.where(labeled == i)
    if len(ys) < 3:  # too small
        continue
    cy = int(np.mean(ys))
    cx = int(np.mean(xs)) + int(w*0.3)
    area = len(ys)
    cy_abs = cy  # already in absolute coords for y (started from 0)
    print(f"  Cluster {i}: center=({cx},{cy_abs}), area={area}px, color=({arr[cy_abs,cx,0]},{arr[cy_abs,cx,1]},{arr[cy_abs,cx,2]})")

# Also visualize by saving annotated image
from PIL import ImageDraw
annotated = img.copy()
draw = ImageDraw.Draw(annotated)

# Draw grid for reference
for x in range(0, w, 20):
    draw.line([(x,0),(x,h)], fill=(50,50,50), width=1)
    draw.text((x+1, 1), str(x), fill=(100,100,100))
for y in range(0, h, 20):
    draw.line([(0,y),(w,y)], fill=(50,50,50), width=1)
    draw.text((1, y+1), str(y), fill=(100,100,100))

annotated.save(r"c:\Users\ADMIN\Downloads\LVGL_ESP32\dragon_annotated.png")
print("\nSaved annotated image with grid")
