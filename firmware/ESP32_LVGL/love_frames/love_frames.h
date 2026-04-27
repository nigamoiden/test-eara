#ifndef _LOVE_FRAMES_H_
#define _LOVE_FRAMES_H_

#include "lvgl.h"

extern const lv_image_dsc_t love_frame_0_dsc;
extern const lv_image_dsc_t love_frame_1_dsc;
extern const lv_image_dsc_t love_frame_2_dsc;
extern const lv_image_dsc_t love_frame_3_dsc;
extern const lv_image_dsc_t love_frame_4_dsc;
extern const lv_image_dsc_t love_frame_5_dsc;
extern const lv_image_dsc_t love_frame_6_dsc;
extern const lv_image_dsc_t love_frame_7_dsc;
extern const lv_image_dsc_t love_frame_8_dsc;

#define LOVE_FRAME_COUNT  9
#define LOVE_FRAME_DELAY  250  /* ms (4 fps) */

static const lv_image_dsc_t * love_frames[] = {
    &love_frame_0_dsc,
    &love_frame_1_dsc,
    &love_frame_2_dsc,
    &love_frame_3_dsc,
    &love_frame_4_dsc,
    &love_frame_5_dsc,
    &love_frame_6_dsc,
    &love_frame_7_dsc,
    &love_frame_8_dsc,
};

#endif /* _LOVE_FRAMES_H_ */