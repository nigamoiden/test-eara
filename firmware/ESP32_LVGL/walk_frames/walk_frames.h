#ifndef _WALK_FRAMES_H_
#define _WALK_FRAMES_H_

#include "lvgl.h"

extern const lv_image_dsc_t walk_frame_0_dsc;
extern const lv_image_dsc_t walk_frame_1_dsc;
extern const lv_image_dsc_t walk_frame_2_dsc;
extern const lv_image_dsc_t walk_frame_3_dsc;
extern const lv_image_dsc_t walk_frame_4_dsc;
extern const lv_image_dsc_t walk_frame_5_dsc;
extern const lv_image_dsc_t walk_frame_6_dsc;
extern const lv_image_dsc_t walk_frame_7_dsc;
extern const lv_image_dsc_t walk_frame_8_dsc;

#define WALK_FRAME_COUNT  9
#define WALK_FRAME_DELAY  250  /* ms (4 fps) */

static const lv_image_dsc_t * walk_frames[] = {
    &walk_frame_0_dsc,
    &walk_frame_1_dsc,
    &walk_frame_2_dsc,
    &walk_frame_3_dsc,
    &walk_frame_4_dsc,
    &walk_frame_5_dsc,
    &walk_frame_6_dsc,
    &walk_frame_7_dsc,
    &walk_frame_8_dsc,
};

#endif /* _WALK_FRAMES_H_ */