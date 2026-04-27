#ifndef _ANGRY_FRAMES_H_
#define _ANGRY_FRAMES_H_

#include "lvgl.h"

extern const lv_image_dsc_t angry_frame_0_dsc;
extern const lv_image_dsc_t angry_frame_1_dsc;
extern const lv_image_dsc_t angry_frame_2_dsc;
extern const lv_image_dsc_t angry_frame_3_dsc;
extern const lv_image_dsc_t angry_frame_4_dsc;
extern const lv_image_dsc_t angry_frame_5_dsc;
extern const lv_image_dsc_t angry_frame_6_dsc;
extern const lv_image_dsc_t angry_frame_7_dsc;
extern const lv_image_dsc_t angry_frame_8_dsc;

#define ANGRY_FRAME_COUNT  9
#define ANGRY_FRAME_DELAY  250  /* ms (4 fps) */

static const lv_image_dsc_t * angry_frames[] = {
    &angry_frame_0_dsc,
    &angry_frame_1_dsc,
    &angry_frame_2_dsc,
    &angry_frame_3_dsc,
    &angry_frame_4_dsc,
    &angry_frame_5_dsc,
    &angry_frame_6_dsc,
    &angry_frame_7_dsc,
    &angry_frame_8_dsc,
};

#endif /* _ANGRY_FRAMES_H_ */