/**
 * Background image header
 *
 * QUAN TRONG: Anh background goc (1628x640) qua lon cho ESP32!
 * Can resize ve 320x240 truoc khi dung.
 *
 * Cach lam:
 * 1. Mo https://lvgl.io/tools/imageconverter (LVGL 9 converter)
 * 2. Upload file z7676715447410_f6b015c59320017ac104d98fcc6f90fb
 * 3. Chon: Output format = C array, Color format = RGB565A8
 * 4. Resize = 320x240
 * 5. Download file .c va dat vao thu muc nay
 * 6. Doi ten bien thanh "bg_image"
 * 7. Uncomment #define USE_BG_IMAGE trong ESP32_LVGL.ino
 */

#ifndef BG_IMAGE_H
#define BG_IMAGE_H

#include "lvgl.h"

extern const lv_image_dsc_t bg_image;

#endif
