#include <lvgl.h>
#include "../modules/bluetooth.h"
#include "../modules/audio.h"

static void play_event(lv_event_t * e) {
    bluetooth_send_command("{\"type\":\"play\"}");
    audio_play();
}

void ui_show_music() {
    lv_obj_clean(lv_scr_act());

    lv_obj_t * title = lv_label_create(lv_scr_act());
    lv_label_set_text(title, "Now Playing: None");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t * playBtn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(playBtn, 100, 60);
    lv_obj_align(playBtn, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * label = lv_label_create(playBtn);
    lv_label_set_text(label, "Play");
    lv_obj_center(label);

    lv_obj_add_event_cb(playBtn, play_event, LV_EVENT_CLICKED, NULL);
}
