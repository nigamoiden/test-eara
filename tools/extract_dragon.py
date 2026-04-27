"""
Extract one dragon from the 3-dragon C array image (1628x640 RGB888),
crop the middle dragon, resize to fit ESP32 TFT,
and output as LVGL C array in RGB565 format.
"""
import re
import struct
import sys

SRC = r"C:\Users\ADMIN\Downloads\z7676715440662_3f84a9cd7145a974a0c622cb0916f8c8.c"
OUT_W = 140  # output width
OUT_H = 160  # output height
ORIG_W = 1628
ORIG_H = 640

def read_c_array(path):
    """Parse hex bytes from the C array file."""
    with open(path, 'r', encoding='utf-8', errors='ignore') as f:
        text = f.read()

    # Find the array data between { and };
    start = text.index('{') + 1
    end = text.index('};')
    data_str = text[start:end]

    # Extract all hex values
    hex_vals = re.findall(r'0x([0-9a-fA-F]{2})', data_str)
    return bytes(int(h, 16) for h in hex_vals)

def crop_middle_dragon(pixels, orig_w, orig_h):
    """Crop the middle dragon (center 1/3 of the image)."""
    dragon_w = orig_w // 3
    x_start = dragon_w  # start of middle dragon
    x_end = dragon_w * 2

    cropped = bytearray()
    for y in range(orig_h):
        row_start = y * orig_w * 3
        for x in range(x_start, x_end):
            idx = row_start + x * 3
            cropped.extend(pixels[idx:idx+3])

    return bytes(cropped), (x_end - x_start), orig_h

def resize_nearest(pixels, src_w, src_h, dst_w, dst_h):
    """Simple nearest-neighbor resize for RGB888 data."""
    result = bytearray()
    for y in range(dst_h):
        sy = int(y * src_h / dst_h)
        for x in range(dst_w):
            sx = int(x * src_w / dst_w)
            idx = (sy * src_w + sx) * 3
            result.extend(pixels[idx:idx+3])
    return bytes(result)

def rgb888_to_rgb565_swapped(pixels):
    """Convert RGB888 to RGB565 with byte swap (for LV_COLOR_16_SWAP=1)."""
    result = bytearray()
    for i in range(0, len(pixels), 3):
        r = pixels[i]
        g = pixels[i+1]
        b = pixels[i+2]
        # RGB565: RRRRRGGG GGGBBBBB
        rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
        # Byte swap for LV_COLOR_16_SWAP
        hi = (rgb565 >> 8) & 0xFF
        lo = rgb565 & 0xFF
        result.append(hi)
        result.append(lo)
    return bytes(result)

def write_c_file(path, name, data, w, h):
    """Write LVGL 8.x compatible C array file."""
    with open(path, 'w') as f:
        f.write(f"""#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMAGE_{name.upper()}
#define LV_ATTRIBUTE_IMAGE_{name.upper()}
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMAGE_{name.upper()} uint8_t {name}_map[] = {{
""")
        # Write data in rows of 16 bytes
        for i in range(0, len(data), 16):
            chunk = data[i:i+16]
            hex_str = ', '.join(f'0x{b:02x}' for b in chunk)
            if i + 16 < len(data):
                f.write(f'  {hex_str},\n')
            else:
                f.write(f'  {hex_str}\n')

        f.write(f"""}};

const lv_img_dsc_t {name} = {{
  .header.cf = LV_IMG_CF_TRUE_COLOR,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = {w},
  .header.h = {h},
  .data_size = {len(data)},
  .data = {name}_map,
}};
""")

def main():
    print(f"Reading C array from source file...")
    pixels = read_c_array(SRC)
    print(f"  Total bytes: {len(pixels)} (expected {ORIG_W * ORIG_H * 3})")

    # Crop middle dragon
    print(f"Cropping middle dragon...")
    cropped, cw, ch = crop_middle_dragon(pixels, ORIG_W, ORIG_H)
    print(f"  Cropped size: {cw}x{ch}")

    # Remove background (find bounding box of non-background pixels)
    # The background is gray (~0xd0d0d0 or ~0xe6e6e6)
    # Find top/bottom bounds where dragon actually is
    print("Finding dragon bounds...")
    top = ch
    bottom = 0
    left = cw
    right = 0
    for y in range(ch):
        for x in range(cw):
            idx = (y * cw + x) * 3
            r, g, b = cropped[idx], cropped[idx+1], cropped[idx+2]
            # Check if not background (background is grayish > 0xC0)
            if not (r > 0xB0 and g > 0xB0 and b > 0xB0 and abs(r-g) < 20 and abs(g-b) < 20):
                top = min(top, y)
                bottom = max(bottom, y)
                left = min(left, x)
                right = max(right, x)

    # Add small padding
    pad = 5
    top = max(0, top - pad)
    bottom = min(ch - 1, bottom + pad)
    left = max(0, left - pad)
    right = min(cw - 1, right + pad)

    print(f"  Dragon bounds: ({left},{top}) to ({right},{bottom})")
    bw = right - left + 1
    bh = bottom - top + 1
    print(f"  Bounding box: {bw}x{bh}")

    # Crop to bounding box
    bounded = bytearray()
    for y in range(top, bottom + 1):
        for x in range(left, right + 1):
            idx = (y * cw + x) * 3
            bounded.extend(cropped[idx:idx+3])
    bounded = bytes(bounded)

    # Calculate output size maintaining aspect ratio
    aspect = bw / bh
    if aspect > OUT_W / OUT_H:
        # Width limited
        out_w = OUT_W
        out_h = int(OUT_W / aspect)
    else:
        # Height limited
        out_h = OUT_H
        out_w = int(OUT_H * aspect)

    # Make even numbers
    out_w = out_w & ~1
    out_h = out_h & ~1

    print(f"Resizing to {out_w}x{out_h}...")
    resized = resize_nearest(bounded, bw, bh, out_w, out_h)

    # Replace gray background with dark forest green
    print("Replacing background with forest green...")
    bg_r, bg_g, bg_b = 0x0a, 0x1f, 0x0a  # C_BG from our theme
    replaced = bytearray()
    for i in range(0, len(resized), 3):
        r, g, b = resized[i], resized[i+1], resized[i+2]
        # Detect gray background (all channels similar, > 0xA0)
        if r > 0xA0 and g > 0xA0 and b > 0xA0 and abs(int(r)-int(g)) < 25 and abs(int(g)-int(b)) < 25:
            replaced.extend([bg_r, bg_g, bg_b])
        else:
            replaced.extend([r, g, b])
    resized = bytes(replaced)

    # Convert to RGB565
    print("Converting to RGB565...")
    rgb565 = rgb888_to_rgb565_swapped(resized)
    print(f"  Output size: {len(rgb565)} bytes ({len(rgb565)/1024:.1f} KB)")

    # Write C file
    out_path = r"c:\Users\ADMIN\Downloads\LVGL_ESP32\dragon_img.c"
    print(f"Writing {out_path}...")
    write_c_file(out_path, "dragon_img", rgb565, out_w, out_h)

    print(f"\nDone! Dragon image: {out_w}x{out_h}, {len(rgb565)/1024:.1f} KB")
    print(f"Use in code: LV_IMG_DECLARE(dragon_img);")

if __name__ == '__main__':
    main()
