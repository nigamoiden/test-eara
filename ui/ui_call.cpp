#include <lvgl.h>
#include "../modules/bluetooth.h"

static void call_event(lv_event_t * e) {
    bluetooth_send_command("{\"type\":\"call\",\"number\":\"0123456789\"}");
}

void ui_show_call() {
    lv_obj_clean(lv_scr_act());

    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, 140, 60);
    lv_obj_center(btn);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Call 0123456789");
    lv_obj_center(label);

    lv_obj_add_event_cb(btn, call_event, LV_EVENT_CLICKED, NULL);
}
