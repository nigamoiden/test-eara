#ifndef _HAPPY_FRAMES_H_
#define _HAPPY_FRAMES_H_

#include "lvgl.h"

extern const lv_image_dsc_t happy_frame_0_dsc;
extern const lv_image_dsc_t happy_frame_1_dsc;
extern const lv_image_dsc_t happy_frame_2_dsc;
extern const lv_image_dsc_t happy_frame_3_dsc;
extern const lv_image_dsc_t happy_frame_4_dsc;
extern const lv_image_dsc_t happy_frame_5_dsc;
extern const lv_image_dsc_t happy_frame_6_dsc;
extern const lv_image_dsc_t happy_frame_7_dsc;
extern const lv_image_dsc_t happy_frame_8_dsc;

#define HAPPY_FRAME_COUNT  9
#define HAPPY_FRAME_DELAY  250  /* ms (4 fps) */

static const lv_image_dsc_t * happy_frames[] = {
    &happy_frame_0_dsc,
    &happy_frame_1_dsc,
    &happy_frame_2_dsc,
    &happy_frame_3_dsc,
    &happy_frame_4_dsc,
    &happy_frame_5_dsc,
    &happy_frame_6_dsc,
    &happy_frame_7_dsc,
    &happy_frame_8_dsc,
};

#endif /* _HAPPY_FRAMES_H_ */