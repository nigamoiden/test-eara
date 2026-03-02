#include <lvgl.h>
#include "../modules/state_manager.h"

static void btn_event(lv_event_t * e) {
    const char * txt = lv_label_get_text((lv_obj_t*)lv_event_get_user_data(e));

    if(strcmp(txt, "Music") == 0) {
        state_set(STATE_MUSIC);
        ui_show_music();
    }
    else if(strcmp(txt, "Call") == 0) {
        state_set(STATE_CALL);
        ui_show_call();
    }
    else if(strcmp(txt, "Settings") == 0) {
        state_set(STATE_SETTINGS);
        ui_show_settings();
    }
}

void ui_show_home() {
    lv_obj_clean(lv_scr_act());

    const char * labels[] = {"Music", "Call", "Settings"};

    for(int i = 0; i < 3; i++) {
        lv_obj_t * btn = lv_btn_create(lv_scr_act());
        lv_obj_set_size(btn, 100, 60);
        lv_obj_align(btn, LV_ALIGN_CENTER, (i-1)*110, 0);

        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text(label, labels[i]);
        lv_obj_center(label);

        lv_obj_add_event_cb(btn, btn_event, LV_EVENT_CLICKED, label);
    }
}
