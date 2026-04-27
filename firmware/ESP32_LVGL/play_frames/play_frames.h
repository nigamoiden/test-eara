#ifndef _PLAY_FRAMES_H_
#define _PLAY_FRAMES_H_

#include "lvgl.h"

extern const lv_image_dsc_t play_frame_0_dsc;
extern const lv_image_dsc_t play_frame_1_dsc;
extern const lv_image_dsc_t play_frame_2_dsc;
extern const lv_image_dsc_t play_frame_3_dsc;
extern const lv_image_dsc_t play_frame_4_dsc;
extern const lv_image_dsc_t play_frame_5_dsc;
extern const lv_image_dsc_t play_frame_6_dsc;
extern const lv_image_dsc_t play_frame_7_dsc;
extern const lv_image_dsc_t play_frame_8_dsc;

#define PLAY_FRAME_COUNT  9
#define PLAY_FRAME_DELAY  250  /* ms (4 fps) */

static const lv_image_dsc_t * play_frames[] = {
    &play_frame_0_dsc,
    &play_frame_1_dsc,
    &play_frame_2_dsc,
    &play_frame_3_dsc,
    &play_frame_4_dsc,
    &play_frame_5_dsc,
    &play_frame_6_dsc,
    &play_frame_7_dsc,
    &play_frame_8_dsc,
};

#endif /* _PLAY_FRAMES_H_ */