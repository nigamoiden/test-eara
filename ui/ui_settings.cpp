#include <lvgl.h>
#include "../modules/audio.h"

static void volume_event(lv_event_t * e) {
    audio_set_volume(5);
}

void ui_show_settings() {
    lv_obj_clean(lv_scr_act());

    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, 120, 60);
    lv_obj_center(btn);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Volume +");
    lv_obj_center(label);

    lv_obj_add_event_cb(btn, volume_event, LV_EVENT_CLICKED, NULL);
}
