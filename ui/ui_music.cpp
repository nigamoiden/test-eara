#include <lvgl.h>
#include "../modules/audio.cpp"

static lv_obj_t *song_label;

static void play_event(lv_event_t *e)
{
    audio_play("sample.mp3");
    lv_label_set_text(song_label, "Playing: sample.mp3");
}

void ui_music_create()
{
    lv_obj_clean(lv_scr_act());

    lv_obj_t *title = lv_label_create(lv_scr_act());
    lv_label_set_text(title, "MUSIC");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    song_label = lv_label_create(lv_scr_act());
    lv_label_set_text(song_label, "No song");
    lv_obj_align(song_label, LV_ALIGN_CENTER, 0, -40);

    lv_obj_t *play_btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(play_btn, 80, 80);
    lv_obj_align(play_btn, LV_ALIGN_CENTER, 0, 40);

    lv_obj_add_event_cb(play_btn, play_event, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label = lv_label_create(play_btn);
    lv_label_set_text(label, "PLAY");
    lv_obj_center(label);
}