/**
 * LVGL 9.x configuration for ESP32 Audio Hub
 * ILI9341 320x240 SPI display with XPT2046 touch
 *
 * Copy this file to: Arduino/libraries/lv_conf.h
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/* ==================
 *  COLOR SETTINGS
 * ================== */
#define LV_COLOR_DEPTH          16
#define LV_COLOR_MIX_ROUND_OFS  0

/* ==================
 *  MEMORY
 * ================== */
#define LV_MEM_SIZE             (48U * 1024U)
#define LV_MEM_ADR              0
#define LV_MEM_CUSTOM           0

/* ==================
 *  OS & TICK
 * ================== */
#define LV_USE_OS               LV_OS_NONE
#define LV_TICK_CUSTOM          0
#define LV_DEF_REFR_PERIOD      16

/* ==================
 *  DISPLAY
 * ================== */
#define LV_DPI_DEF              130

/* ==================
 *  DRAW ENGINE
 * ================== */
#define LV_USE_DRAW_SW          1

/* ==================
 *  LOGGING
 * ================== */
#define LV_USE_LOG              0
#define LV_USE_PERF_MONITOR     0
#define LV_USE_MEM_MONITOR      0
#define LV_USE_SYSMON           0

/* ==================
 *  FONTS
 * ================== */
#define LV_FONT_MONTSERRAT_10   1
#define LV_FONT_MONTSERRAT_12   1
#define LV_FONT_MONTSERRAT_14   1
#define LV_FONT_MONTSERRAT_16   1
#define LV_FONT_MONTSERRAT_20   1
#define LV_FONT_MONTSERRAT_28   1
#define LV_FONT_MONTSERRAT_36   1
#define LV_FONT_DEFAULT         &lv_font_montserrat_14

/* ==================
 *  WIDGETS
 * ================== */
#define LV_USE_ARC              1
#define LV_USE_BAR              1
#define LV_USE_BTN              1
#define LV_USE_BTNMATRIX        1
#define LV_USE_LABEL            1
#define LV_USE_LINE             1
#define LV_USE_SLIDER           1
#define LV_USE_SWITCH           1
#define LV_USE_IMAGE            1
#define LV_USE_LED              1
#define LV_USE_LIST             1
#define LV_USE_MSGBOX           1
#define LV_USE_SPINNER          1
#define LV_USE_ROLLER           0
#define LV_USE_DROPDOWN         0
#define LV_USE_TEXTAREA         0
#define LV_USE_TABLE            0
#define LV_USE_CHART            0
#define LV_USE_CHECKBOX         0
#define LV_USE_METER            0
#define LV_USE_KEYBOARD         0
#define LV_USE_TABVIEW          0
#define LV_USE_ANIMIMAGE        0

/* ==================
 *  THEMES
 * ================== */
#define LV_USE_THEME_DEFAULT    1
#define LV_THEME_DEFAULT_DARK   1

/* ==================
 *  ANIMATIONS
 * ================== */
#define LV_USE_ANIM             1

/* ==================
 *  LAYOUTS
 * ================== */
#define LV_USE_FLEX             1
#define LV_USE_GRID             0

#endif /* LV_CONF_H */
