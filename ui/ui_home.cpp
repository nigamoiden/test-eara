#include <lvgl.h>
#include "ui_music.h"
#include "ui_call.h"
#include "ui_settings.h"

static void music_event(lv_event_t *e)
{
    ui_music_create();
}

static void call_event(lv_event_t *e)
{
    ui_call_create();
}

static void settings_event(lv_event_t *e)
{
    ui_settings_create();
}

void ui_home_create()
{
    lv_obj_clean(lv_scr_act());

    lv_obj_t *title = lv_label_create(lv_scr_act());
    lv_label_set_text(title, "EARA");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t *btn_music = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn_music, 90, 90);
    lv_obj_align(btn_music, LV_ALIGN_CENTER, -100, 20);
    lv_obj_add_event_cb(btn_music, music_event, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label1 = lv_label_create(btn_music);
    lv_label_set_text(label1, "MUSIC");
    lv_obj_center(label1);

    lv_obj_t *btn_call = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn_call, 90, 90);
    lv_obj_align(btn_call, LV_ALIGN_CENTER, 0, 20);
    lv_obj_add_event_cb(btn_call, call_event, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label2 = lv_label_create(btn_call);
    lv_label_set_text(label2, "CALL");
    lv_obj_center(label2);

    lv_obj_t *btn_settings = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn_settings, 90, 90);
    lv_obj_align(btn_settings, LV_ALIGN_CENTER, 100, 20);
    lv_obj_add_event_cb(btn_settings, settings_event, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label3 = lv_label_create(btn_settings);
    lv_label_set_text(label3, "SET");
    lv_obj_center(label3);
}