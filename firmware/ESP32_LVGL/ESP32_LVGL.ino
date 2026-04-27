// ==========================================================
// ESP32 LVGL Audio Hub — 6 Screens, 320x240 ILI9341
// BT Classic HFP + WiFi HTTP Music Streamer + LeafDra Pet
//
// LVGL 9.x + TFT_eSPI + XPT2046
// Dark theme, smooth animations, pet character system
//
// Screens: Home (with Pet), Music Browser, Now Playing,
//          Incoming Call, In-Call, Settings
//
// Thu vien can cai (Arduino Library Manager):
//   - lvgl (9.x)
//   - TFT_eSPI
//   - XPT2046_Touchscreen
//
// Copy lv_conf.h vao thu muc Arduino/libraries/
// Copy cac folder *_frames/ vao thu muc sketch
// ==========================================================

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

// --- Animation frames ---
#include "walk_frames/walk_frames.h"
#include "play_frames/play_frames.h"
#include "happy_frames/happy_frames.h"
#include "angry_frames/angry_frames.h"
#include "love_frames/love_frames.h"

// --- Background (uncomment khi da resize ve 320x240) ---
// #define USE_BG_IMAGE
#ifdef USE_BG_IMAGE
#include "bg_image.h"
#endif

// --- Pin config ---
#define TOUCH_CS  21
#define TFT_BL    15

// --- Display ---
#define SCREEN_W  320
#define SCREEN_H  240

TFT_eSPI tft = TFT_eSPI();
XPT2046_Touchscreen ts(TOUCH_CS);

// LVGL 9 display buffer
static uint8_t draw_buf[SCREEN_W * 20 * 2];

// ===================== THEME COLORS =====================
#define C_BG        lv_color_hex(0x0f0f1a)
#define C_CARD      lv_color_hex(0x1a1a2e)
#define C_CARD2     lv_color_hex(0x232342)
#define C_PRI       lv_color_hex(0x7b68ee)
#define C_PRI_DARK  lv_color_hex(0x5b48ce)
#define C_PRI_LIGHT lv_color_hex(0x9d8fff)
#define C_GREEN     lv_color_hex(0x00e676)
#define C_RED       lv_color_hex(0xff5252)
#define C_ORANGE    lv_color_hex(0xffab40)
#define C_CYAN      lv_color_hex(0x18ffff)
#define C_YELLOW    lv_color_hex(0xffd740)
#define C_TEXT      lv_color_hex(0xe0e0f0)
#define C_TEXT_DIM  lv_color_hex(0x7a7a9c)
#define C_DARK      lv_color_hex(0x0a0a18)
#define C_BORDER    lv_color_hex(0x2a2a4c)
#define C_SURFACE   lv_color_hex(0x16162e)
#define C_BG_GREEN  lv_color_hex(0x0d2818)

// ===================== PET SYSTEM =====================
typedef enum {
  MOOD_WALK = 0,
  MOOD_PLAY,
  MOOD_HAPPY,
  MOOD_ANGRY,
  MOOD_LOVE
} pet_mood_t;

static pet_mood_t current_mood     = MOOD_WALK;
static pet_mood_t selected_emotion = MOOD_HAPPY;
static int  pet_frame              = 0;
static bool pet_play_once          = false;

static const lv_image_dsc_t **anim_sets[] = {
  walk_frames, play_frames, happy_frames, angry_frames, love_frames
};
static const int anim_counts[] = {
  WALK_FRAME_COUNT, PLAY_FRAME_COUNT, HAPPY_FRAME_COUNT,
  ANGRY_FRAME_COUNT, LOVE_FRAME_COUNT
};

// ===================== STATE =====================
// Music
static bool is_playing = false;
static int  cur_song   = 0;
static int  cur_time   = 0;
static int  volume     = 70;
static int  active_tab = 0;

// Call
static int  call_time   = 0;
static bool in_call     = false;
static bool mic_muted   = false;
static bool speaker_on  = false;

// System
static bool bt_on      = true;
static bool wifi_on    = true;
static int  brightness = 80;

// ===================== DEMO DATA =====================
#define NUM_SONGS 6
static const char *songs[]   = {"Blinding Lights", "Levitating", "Save Your Tears",
                                 "Peaches", "Stay", "Heat Waves"};
static const char *artists[] = {"The Weeknd", "Dua Lipa", "The Weeknd",
                                 "Justin Bieber", "Kid LAROI", "Glass Animals"};
static const int durations[] = {222, 203, 215, 198, 142, 239};
static bool downloaded[]     = {true, false, false, true, false, true};
static const char *caller_number = "+84 912 345 678";

// ===================== SCREENS =====================
static lv_obj_t *scr_home     = NULL;
static lv_obj_t *scr_browser  = NULL;
static lv_obj_t *scr_nplay    = NULL;
static lv_obj_t *scr_incoming = NULL;
static lv_obj_t *scr_incall   = NULL;
static lv_obj_t *scr_settings = NULL;

// ===================== KEY WIDGETS =====================
// Home / Pet
static lv_obj_t *lbl_clock       = NULL;
static lv_obj_t *lbl_home_song   = NULL;
static lv_obj_t *lbl_home_play   = NULL;
static lv_obj_t *lbl_bt_icon     = NULL;
static lv_obj_t *lbl_wifi_icon   = NULL;
static lv_obj_t *img_pet         = NULL;
static lv_obj_t *lbl_mood_ind    = NULL;
static lv_obj_t *btn_mood_happy  = NULL;
static lv_obj_t *btn_mood_angry  = NULL;
static lv_obj_t *btn_mood_love   = NULL;

// Browser
static lv_obj_t *tab_online      = NULL;
static lv_obj_t *tab_downloaded  = NULL;
static lv_obj_t *list_online     = NULL;
static lv_obj_t *list_downloaded = NULL;
static lv_obj_t *modal_listen    = NULL;

// Now Playing
static lv_obj_t *lbl_np_song     = NULL;
static lv_obj_t *lbl_np_artist   = NULL;
static lv_obj_t *bar_progress    = NULL;
static lv_obj_t *lbl_cur_time    = NULL;
static lv_obj_t *lbl_total_time  = NULL;
static lv_obj_t *lbl_play_icon   = NULL;
static lv_obj_t *slider_vol      = NULL;
static lv_obj_t *lbl_vol_val     = NULL;
static lv_obj_t *bars_eq[7];

// In-Call
static lv_obj_t *lbl_call_timer  = NULL;
static lv_obj_t *btn_mic         = NULL;
static lv_obj_t *lbl_mic_icon    = NULL;
static lv_obj_t *btn_speaker     = NULL;
static lv_obj_t *lbl_spk_icon    = NULL;

// Settings
static lv_obj_t *sw_bt           = NULL;
static lv_obj_t *sw_wifi         = NULL;

// Timers
static lv_timer_t *timer_music = NULL;
static lv_timer_t *timer_call  = NULL;
static lv_timer_t *timer_pet   = NULL;

// ===================== STYLES =====================
static lv_style_t style_card;
static lv_style_t style_btn_ghost;
static lv_style_t style_list_row;

static void init_styles() {
  lv_style_init(&style_card);
  lv_style_set_bg_color(&style_card, C_CARD);
  lv_style_set_bg_opa(&style_card, LV_OPA_COVER);
  lv_style_set_radius(&style_card, 12);
  lv_style_set_border_width(&style_card, 1);
  lv_style_set_border_color(&style_card, C_BORDER);
  lv_style_set_pad_all(&style_card, 10);

  lv_style_init(&style_btn_ghost);
  lv_style_set_bg_opa(&style_btn_ghost, LV_OPA_TRANSP);
  lv_style_set_border_width(&style_btn_ghost, 0);
  lv_style_set_shadow_width(&style_btn_ghost, 0);
  lv_style_set_text_color(&style_btn_ghost, C_TEXT);

  lv_style_init(&style_list_row);
  lv_style_set_bg_color(&style_list_row, C_CARD2);
  lv_style_set_bg_opa(&style_list_row, LV_OPA_COVER);
  lv_style_set_radius(&style_list_row, 8);
  lv_style_set_pad_all(&style_list_row, 6);
}

// ===================== HELPERS =====================
static void fmt_time(int sec, char *buf, int len) {
  snprintf(buf, len, "%d:%02d", sec / 60, sec % 60);
}

static lv_obj_t *create_status_bar(lv_obj_t *parent) {
  lv_obj_t *bar = lv_obj_create(parent);
  lv_obj_set_size(bar, SCREEN_W, 22);
  lv_obj_set_pos(bar, 0, 0);
  lv_obj_set_style_bg_color(bar, C_DARK, 0);
  lv_obj_set_style_bg_opa(bar, LV_OPA_80, 0);
  lv_obj_set_style_border_width(bar, 0, 0);
  lv_obj_set_style_radius(bar, 0, 0);
  lv_obj_set_style_pad_hor(bar, 8, 0);
  lv_obj_set_style_pad_ver(bar, 2, 0);
  lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_flex_flow(bar, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(bar, LV_FLEX_ALIGN_SPACE_BETWEEN,
                         LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  return bar;
}

static lv_obj_t *create_back_btn(lv_obj_t *parent, lv_event_cb_t cb) {
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_add_style(btn, &style_btn_ghost, 0);
  lv_obj_set_size(btn, 36, 28);
  lv_obj_t *lbl = lv_label_create(btn);
  lv_label_set_text(lbl, LV_SYMBOL_LEFT);
  lv_obj_set_style_text_font(lbl, &lv_font_montserrat_16, 0);
  lv_obj_center(lbl);
  lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, NULL);
  return btn;
}

// ===================== PET FUNCTIONS =====================
static void set_pet_mood(pet_mood_t mood) {
  current_mood = mood;
  pet_frame = 0;
  pet_play_once = (mood == MOOD_PLAY);
  if (img_pet) lv_image_set_src(img_pet, anim_sets[mood][0]);
}

// Server goi ham nay de set emotion tu chatbot response
// Vi du: set_emotion_from_server("happy") -> MOOD_HAPPY
void set_emotion_from_server(const char *emotion) {
  if (strcmp(emotion, "happy") == 0) {
    selected_emotion = MOOD_HAPPY;
    set_pet_mood(MOOD_HAPPY);
  } else if (strcmp(emotion, "angry") == 0) {
    selected_emotion = MOOD_ANGRY;
    set_pet_mood(MOOD_ANGRY);
  } else if (strcmp(emotion, "love") == 0) {
    selected_emotion = MOOD_LOVE;
    set_pet_mood(MOOD_LOVE);
  }
  if (lbl_mood_ind) {
    const char *names[] = {"Walk","Play","Happy","Angry","Love"};
    lv_label_set_text(lbl_mood_ind, names[selected_emotion]);
  }
}

// Tra ve emotion string de gui trong HTTP request
const char *get_current_emotion() {
  switch (selected_emotion) {
    case MOOD_HAPPY: return "happy";
    case MOOD_ANGRY: return "angry";
    case MOOD_LOVE:  return "love";
    default:         return "happy";
  }
}

// ===================== NAVIGATION =====================
static void goto_home(void *u)     { lv_scr_load_anim(scr_home, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false); }
static void goto_browser(void *u)  { lv_scr_load_anim(scr_browser, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false); }
static void goto_nplay(void *u)    { lv_scr_load_anim(scr_nplay, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false); }
static void goto_incoming(void *u) { lv_scr_load_anim(scr_incoming, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false); }
static void goto_incall(void *u)   { lv_scr_load_anim(scr_incall, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false); }
static void goto_settings(void *u) { lv_scr_load_anim(scr_settings, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false); }

static void ev_goto_home(lv_event_t *e)     { goto_home(NULL); }
static void ev_goto_browser(lv_event_t *e)  { goto_browser(NULL); }
static void ev_goto_nplay(lv_event_t *e)    { goto_nplay(NULL); }
static void ev_goto_settings(lv_event_t *e) { goto_settings(NULL); }

// ===================== UPDATE HELPERS =====================
static void update_np_info() {
  if (!lbl_np_song) return;
  lv_label_set_text(lbl_np_song, songs[cur_song]);
  lv_label_set_text(lbl_np_artist, artists[cur_song]);
  char buf[12];
  fmt_time(durations[cur_song], buf, sizeof(buf));
  lv_label_set_text(lbl_total_time, buf);
  lv_bar_set_range(bar_progress, 0, durations[cur_song]);
  lv_bar_set_value(bar_progress, cur_time, LV_ANIM_ON);
  fmt_time(cur_time, buf, sizeof(buf));
  lv_label_set_text(lbl_cur_time, buf);
  lv_label_set_text(lbl_play_icon, is_playing ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);
}

static void update_home_mini() {
  if (!lbl_home_song) return;
  char buf[64];
  snprintf(buf, sizeof(buf), "%s - %s", songs[cur_song], artists[cur_song]);
  lv_label_set_text(lbl_home_song, buf);
  lv_label_set_text(lbl_home_play, is_playing ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);
}

// ===================== CALLBACKS =====================
// Pet
static void cb_pet_tap(lv_event_t *e) {
  if (current_mood != MOOD_PLAY) set_pet_mood(MOOD_PLAY);
}

static void update_mood_borders() {
  lv_obj_set_style_border_width(btn_mood_happy, selected_emotion == MOOD_HAPPY ? 2 : 0, 0);
  lv_obj_set_style_border_width(btn_mood_angry, selected_emotion == MOOD_ANGRY ? 2 : 0, 0);
  lv_obj_set_style_border_width(btn_mood_love,  selected_emotion == MOOD_LOVE  ? 2 : 0, 0);
}

static void cb_mood_happy(lv_event_t *e) {
  selected_emotion = MOOD_HAPPY;
  set_pet_mood(MOOD_HAPPY);
  if (lbl_mood_ind) lv_label_set_text(lbl_mood_ind, "Happy");
  update_mood_borders();
}
static void cb_mood_angry(lv_event_t *e) {
  selected_emotion = MOOD_ANGRY;
  set_pet_mood(MOOD_ANGRY);
  if (lbl_mood_ind) lv_label_set_text(lbl_mood_ind, "Angry");
  update_mood_borders();
}
static void cb_mood_love(lv_event_t *e) {
  selected_emotion = MOOD_LOVE;
  set_pet_mood(MOOD_LOVE);
  if (lbl_mood_ind) lv_label_set_text(lbl_mood_ind, "Love");
  update_mood_borders();
}

// Music
static void cb_play_pause(lv_event_t *e) {
  is_playing = !is_playing;
  update_np_info(); update_home_mini();
}
static void cb_next_song(lv_event_t *e) {
  cur_song = (cur_song + 1) % NUM_SONGS;
  cur_time = 0; is_playing = true;
  update_np_info(); update_home_mini();
}
static void cb_prev_song(lv_event_t *e) {
  if (cur_time > 3) cur_time = 0;
  else { cur_song = (cur_song - 1 + NUM_SONGS) % NUM_SONGS; cur_time = 0; }
  is_playing = true;
  update_np_info(); update_home_mini();
}
static void cb_volume(lv_event_t *e) {
  volume = lv_slider_get_value(lv_event_get_target(e));
  char buf[8]; snprintf(buf, sizeof(buf), "%d%%", volume);
  if (lbl_vol_val) lv_label_set_text(lbl_vol_val, buf);
}
static void cb_home_play(lv_event_t *e) {
  is_playing = !is_playing;
  update_home_mini(); update_np_info();
}
static void cb_mini_tap(lv_event_t *e) { goto_nplay(NULL); }

static void cb_song_tap(lv_event_t *e) {
  int idx = (int)(intptr_t)lv_event_get_user_data(e);
  cur_song = idx; cur_time = 0; is_playing = true;
  update_np_info(); update_home_mini();
  goto_nplay(NULL);
}
static void cb_download(lv_event_t *e) {
  int idx = (int)(intptr_t)lv_event_get_user_data(e);
  downloaded[idx] = true;
  lv_obj_t *lbl = lv_obj_get_child(lv_event_get_target(e), 0);
  lv_label_set_text(lbl, LV_SYMBOL_OK);
  lv_obj_set_style_text_color(lbl, C_GREEN, 0);
}
static void cb_delete_song(lv_event_t *e) {
  int idx = (int)(intptr_t)lv_event_get_user_data(e);
  downloaded[idx] = false;
}
static void cb_voice_search(lv_event_t *e) {
  if (modal_listen) lv_obj_clear_flag(modal_listen, LV_OBJ_FLAG_HIDDEN);
}

// Call
static void cb_accept_call(lv_event_t *e) {
  in_call = true; call_time = 0;
  if (is_playing) { is_playing = false; update_np_info(); update_home_mini(); }
  goto_incall(NULL);
}
static void cb_reject_call(lv_event_t *e) { goto_home(NULL); }
static void cb_end_call(lv_event_t *e) {
  in_call = false; call_time = 0;
  mic_muted = false; speaker_on = false;
  goto_home(NULL);
}
static void cb_toggle_mic(lv_event_t *e) {
  mic_muted = !mic_muted;
  lv_label_set_text(lbl_mic_icon, mic_muted ? LV_SYMBOL_MUTE : LV_SYMBOL_AUDIO);
  lv_obj_set_style_bg_color(btn_mic, mic_muted ? C_RED : C_CARD2, 0);
}
static void cb_toggle_speaker(lv_event_t *e) {
  speaker_on = !speaker_on;
  lv_label_set_text(lbl_spk_icon, speaker_on ? LV_SYMBOL_VOLUME_MAX : LV_SYMBOL_VOLUME_MID);
  lv_obj_set_style_bg_color(btn_speaker, speaker_on ? C_PRI : C_CARD2, 0);
}

// Settings
static void cb_bt_toggle(lv_event_t *e) {
  bt_on = lv_obj_has_state(sw_bt, LV_STATE_CHECKED);
  if (lbl_bt_icon) lv_obj_set_style_text_color(lbl_bt_icon, bt_on ? C_CYAN : C_TEXT_DIM, 0);
}
static void cb_wifi_toggle(lv_event_t *e) {
  wifi_on = lv_obj_has_state(sw_wifi, LV_STATE_CHECKED);
  if (lbl_wifi_icon) lv_obj_set_style_text_color(lbl_wifi_icon, wifi_on ? C_GREEN : C_TEXT_DIM, 0);
}
static void cb_brightness(lv_event_t *e) {
  brightness = lv_slider_get_value(lv_event_get_target(e));
  analogWrite(TFT_BL, map(brightness, 0, 100, 0, 255));
}

// Tabs
static void cb_tab_online(lv_event_t *e) {
  active_tab = 0;
  lv_obj_clear_flag(list_online, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(list_downloaded, LV_OBJ_FLAG_HIDDEN);
  lv_obj_set_style_bg_color(tab_online, C_PRI, 0);
  lv_obj_set_style_bg_color(tab_downloaded, C_CARD2, 0);
}
static void cb_tab_downloaded(lv_event_t *e) {
  active_tab = 1;
  lv_obj_add_flag(list_online, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(list_downloaded, LV_OBJ_FLAG_HIDDEN);
  lv_obj_set_style_bg_color(tab_downloaded, C_PRI, 0);
  lv_obj_set_style_bg_color(tab_online, C_CARD2, 0);
}

// ===================== TIMER CALLBACKS =====================
static void music_timer_cb(lv_timer_t *t) {
  if (!is_playing) return;
  cur_time++;
  if (cur_time >= durations[cur_song]) {
    cur_song = (cur_song + 1) % NUM_SONGS;
    cur_time = 0; update_home_mini();
  }
  if (bar_progress) lv_bar_set_value(bar_progress, cur_time, LV_ANIM_ON);
  char buf[12]; fmt_time(cur_time, buf, sizeof(buf));
  if (lbl_cur_time) lv_label_set_text(lbl_cur_time, buf);
}

static void call_timer_cb(lv_timer_t *t) {
  if (!in_call) return;
  call_time++;
  if (lbl_call_timer) {
    char buf[12];
    snprintf(buf, sizeof(buf), "%02d:%02d", call_time / 60, call_time % 60);
    lv_label_set_text(lbl_call_timer, buf);
  }
}

static void pet_anim_cb(lv_timer_t *t) {
  if (!img_pet) return;
  int count = anim_counts[current_mood];
  pet_frame++;
  if (pet_play_once && pet_frame >= count) {
    set_pet_mood(MOOD_WALK);
    return;
  }
  pet_frame = pet_frame % count;
  lv_image_set_src(img_pet, anim_sets[current_mood][pet_frame]);
}

static void eq_timer_cb(lv_timer_t *t) {
  if (!is_playing) {
    for (int i = 0; i < 7; i++) if (bars_eq[i]) lv_obj_set_height(bars_eq[i], 4);
    return;
  }
  for (int i = 0; i < 7; i++) {
    if (bars_eq[i]) lv_obj_set_height(bars_eq[i], 6 + (esp_random() % 34));
  }
}

static void clock_timer_cb(lv_timer_t *t) {
  static int hh = 14, mm = 30;
  mm++;
  if (mm >= 60) { mm = 0; hh = (hh + 1) % 24; }
  char buf[8]; snprintf(buf, sizeof(buf), "%02d:%02d", hh, mm);
  if (lbl_clock) lv_label_set_text(lbl_clock, buf);
}

// ===================== BUILD HOME =====================
static void build_home() {
  scr_home = lv_obj_create(NULL);
  lv_obj_clear_flag(scr_home, LV_OBJ_FLAG_SCROLLABLE);

#ifdef USE_BG_IMAGE
  lv_obj_t *bg = lv_image_create(scr_home);
  lv_image_set_src(bg, &bg_image);
  lv_obj_set_size(bg, SCREEN_W, SCREEN_H);
  lv_obj_set_pos(bg, 0, 0);
#else
  lv_obj_set_style_bg_color(scr_home, C_BG_GREEN, 0);
  lv_obj_set_style_bg_grad_color(scr_home, C_DARK, 0);
  lv_obj_set_style_bg_grad_dir(scr_home, LV_GRAD_DIR_VER, 0);
#endif

  // --- Status bar ---
  lv_obj_t *sbar = create_status_bar(scr_home);

  lbl_bt_icon = lv_label_create(sbar);
  lv_label_set_text(lbl_bt_icon, LV_SYMBOL_BLUETOOTH);
  lv_obj_set_style_text_color(lbl_bt_icon, bt_on ? C_CYAN : C_TEXT_DIM, 0);
  lv_obj_set_style_text_font(lbl_bt_icon, &lv_font_montserrat_12, 0);

  lbl_clock = lv_label_create(sbar);
  lv_label_set_text(lbl_clock, "14:30");
  lv_obj_set_style_text_color(lbl_clock, C_TEXT, 0);
  lv_obj_set_style_text_font(lbl_clock, &lv_font_montserrat_14, 0);

  lv_obj_t *rg = lv_obj_create(sbar);
  lv_obj_set_size(rg, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_opa(rg, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(rg, 0, 0);
  lv_obj_set_style_pad_all(rg, 0, 0);
  lv_obj_set_style_pad_gap(rg, 6, 0);
  lv_obj_set_flex_flow(rg, LV_FLEX_FLOW_ROW);
  lv_obj_clear_flag(rg, LV_OBJ_FLAG_SCROLLABLE);

  lbl_wifi_icon = lv_label_create(rg);
  lv_label_set_text(lbl_wifi_icon, LV_SYMBOL_WIFI);
  lv_obj_set_style_text_color(lbl_wifi_icon, wifi_on ? C_GREEN : C_TEXT_DIM, 0);
  lv_obj_set_style_text_font(lbl_wifi_icon, &lv_font_montserrat_12, 0);

  lv_obj_t *bat = lv_label_create(rg);
  lv_label_set_text(bat, LV_SYMBOL_BATTERY_FULL);
  lv_obj_set_style_text_color(bat, C_GREEN, 0);
  lv_obj_set_style_text_font(bat, &lv_font_montserrat_12, 0);

  // --- Pet character (center) ---
  img_pet = lv_image_create(scr_home);
  lv_image_set_src(img_pet, walk_frames[0]);
  lv_obj_align(img_pet, LV_ALIGN_CENTER, -20, -10);
  lv_obj_add_flag(img_pet, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(img_pet, cb_pet_tap, LV_EVENT_CLICKED, NULL);

  // Mood label
  lbl_mood_ind = lv_label_create(scr_home);
  lv_label_set_text(lbl_mood_ind, "Happy");
  lv_obj_set_style_text_font(lbl_mood_ind, &lv_font_montserrat_10, 0);
  lv_obj_set_style_text_color(lbl_mood_ind, C_TEXT_DIM, 0);
  lv_obj_align_to(lbl_mood_ind, img_pet, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);

  // --- Mood buttons (right side) ---
  lv_obj_t *mood_col = lv_obj_create(scr_home);
  lv_obj_set_size(mood_col, 46, 138);
  lv_obj_align(mood_col, LV_ALIGN_RIGHT_MID, -4, -10);
  lv_obj_set_style_bg_color(mood_col, C_DARK, 0);
  lv_obj_set_style_bg_opa(mood_col, LV_OPA_60, 0);
  lv_obj_set_style_radius(mood_col, 16, 0);
  lv_obj_set_style_border_width(mood_col, 1, 0);
  lv_obj_set_style_border_color(mood_col, C_BORDER, 0);
  lv_obj_set_style_pad_all(mood_col, 4, 0);
  lv_obj_set_style_pad_gap(mood_col, 6, 0);
  lv_obj_set_flex_flow(mood_col, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(mood_col, LV_FLEX_ALIGN_CENTER,
                         LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_clear_flag(mood_col, LV_OBJ_FLAG_SCROLLABLE);

  // Happy
  btn_mood_happy = lv_btn_create(mood_col);
  lv_obj_set_size(btn_mood_happy, 34, 34);
  lv_obj_set_style_bg_color(btn_mood_happy, lv_color_hex(0x2e7d32), 0);
  lv_obj_set_style_radius(btn_mood_happy, 17, 0);
  lv_obj_set_style_border_color(btn_mood_happy, C_GREEN, 0);
  lv_obj_set_style_border_width(btn_mood_happy, 2, 0);
  lv_obj_t *lh = lv_label_create(btn_mood_happy);
  lv_label_set_text(lh, LV_SYMBOL_OK);
  lv_obj_set_style_text_font(lh, &lv_font_montserrat_14, 0);
  lv_obj_center(lh);
  lv_obj_add_event_cb(btn_mood_happy, cb_mood_happy, LV_EVENT_CLICKED, NULL);

  // Angry
  btn_mood_angry = lv_btn_create(mood_col);
  lv_obj_set_size(btn_mood_angry, 34, 34);
  lv_obj_set_style_bg_color(btn_mood_angry, lv_color_hex(0xc62828), 0);
  lv_obj_set_style_radius(btn_mood_angry, 17, 0);
  lv_obj_set_style_border_color(btn_mood_angry, C_RED, 0);
  lv_obj_set_style_border_width(btn_mood_angry, 0, 0);
  lv_obj_t *la = lv_label_create(btn_mood_angry);
  lv_label_set_text(la, LV_SYMBOL_CLOSE);
  lv_obj_set_style_text_font(la, &lv_font_montserrat_14, 0);
  lv_obj_center(la);
  lv_obj_add_event_cb(btn_mood_angry, cb_mood_angry, LV_EVENT_CLICKED, NULL);

  // Love
  btn_mood_love = lv_btn_create(mood_col);
  lv_obj_set_size(btn_mood_love, 34, 34);
  lv_obj_set_style_bg_color(btn_mood_love, lv_color_hex(0xad1457), 0);
  lv_obj_set_style_radius(btn_mood_love, 17, 0);
  lv_obj_set_style_border_color(btn_mood_love, lv_color_hex(0xff4081), 0);
  lv_obj_set_style_border_width(btn_mood_love, 0, 0);
  lv_obj_t *ll = lv_label_create(btn_mood_love);
  lv_label_set_text(ll, LV_SYMBOL_BELL);
  lv_obj_set_style_text_font(ll, &lv_font_montserrat_14, 0);
  lv_obj_center(ll);
  lv_obj_add_event_cb(btn_mood_love, cb_mood_love, LV_EVENT_CLICKED, NULL);

  // --- Bottom nav bar ---
  lv_obj_t *nav = lv_obj_create(scr_home);
  lv_obj_set_size(nav, SCREEN_W, 48);
  lv_obj_align(nav, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(nav, C_DARK, 0);
  lv_obj_set_style_bg_opa(nav, LV_OPA_90, 0);
  lv_obj_set_style_radius(nav, 0, 0);
  lv_obj_set_style_border_width(nav, 0, 0);
  lv_obj_set_style_pad_hor(nav, 6, 0);
  lv_obj_set_style_pad_ver(nav, 4, 0);
  lv_obj_set_flex_flow(nav, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(nav, LV_FLEX_ALIGN_SPACE_BETWEEN,
                         LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_clear_flag(nav, LV_OBJ_FLAG_SCROLLABLE);

  // Music btn
  lv_obj_t *bm = lv_btn_create(nav);
  lv_obj_set_size(bm, 42, 38);
  lv_obj_set_style_bg_color(bm, C_PRI_DARK, 0);
  lv_obj_set_style_radius(bm, 10, 0);
  lv_obj_t *im = lv_label_create(bm);
  lv_label_set_text(im, LV_SYMBOL_AUDIO);
  lv_obj_set_style_text_font(im, &lv_font_montserrat_16, 0);
  lv_obj_center(im);
  lv_obj_add_event_cb(bm, ev_goto_browser, LV_EVENT_CLICKED, NULL);

  // Mini player
  lv_obj_t *mini = lv_obj_create(nav);
  lv_obj_set_flex_grow(mini, 1);
  lv_obj_set_height(mini, 38);
  lv_obj_set_style_bg_color(mini, C_CARD, 0);
  lv_obj_set_style_bg_opa(mini, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(mini, 10, 0);
  lv_obj_set_style_border_width(mini, 0, 0);
  lv_obj_set_style_pad_hor(mini, 6, 0);
  lv_obj_set_style_pad_ver(mini, 2, 0);
  lv_obj_set_flex_flow(mini, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(mini, LV_FLEX_ALIGN_SPACE_BETWEEN,
                         LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_clear_flag(mini, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(mini, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(mini, cb_mini_tap, LV_EVENT_CLICKED, NULL);

  lbl_home_song = lv_label_create(mini);
  lv_obj_set_width(lbl_home_song, 115);
  lv_label_set_long_mode(lbl_home_song, LV_LABEL_LONG_SCROLL_CIRCULAR);
  lv_obj_set_style_text_font(lbl_home_song, &lv_font_montserrat_10, 0);
  lv_obj_set_style_text_color(lbl_home_song, C_TEXT, 0);

  lv_obj_t *bhp = lv_btn_create(mini);
  lv_obj_add_style(bhp, &style_btn_ghost, 0);
  lv_obj_set_size(bhp, 26, 26);
  lbl_home_play = lv_label_create(bhp);
  lv_obj_set_style_text_font(lbl_home_play, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(lbl_home_play, C_PRI_LIGHT, 0);
  lv_obj_center(lbl_home_play);
  lv_obj_add_event_cb(bhp, cb_home_play, LV_EVENT_CLICKED, NULL);

  // Phone btn
  lv_obj_t *bp = lv_btn_create(nav);
  lv_obj_set_size(bp, 42, 38);
  lv_obj_set_style_bg_color(bp, lv_color_hex(0x1b5e20), 0);
  lv_obj_set_style_radius(bp, 10, 0);
  lv_obj_t *ip = lv_label_create(bp);
  lv_label_set_text(ip, LV_SYMBOL_CALL);
  lv_obj_set_style_text_font(ip, &lv_font_montserrat_16, 0);
  lv_obj_center(ip);
  lv_obj_add_event_cb(bp, [](lv_event_t *e){ goto_incoming(NULL); }, LV_EVENT_CLICKED, NULL);

  // Settings btn
  lv_obj_t *bs = lv_btn_create(nav);
  lv_obj_set_size(bs, 42, 38);
  lv_obj_set_style_bg_color(bs, C_CARD2, 0);
  lv_obj_set_style_radius(bs, 10, 0);
  lv_obj_t *is2 = lv_label_create(bs);
  lv_label_set_text(is2, LV_SYMBOL_SETTINGS);
  lv_obj_set_style_text_font(is2, &lv_font_montserrat_16, 0);
  lv_obj_center(is2);
  lv_obj_add_event_cb(bs, ev_goto_settings, LV_EVENT_CLICKED, NULL);

  update_home_mini();
}

// ===================== BUILD MUSIC BROWSER =====================
static void build_song_row(lv_obj_t *parent, int idx, bool show_dl) {
  lv_obj_t *row = lv_obj_create(parent);
  lv_obj_set_size(row, lv_pct(100), 42);
  lv_obj_add_style(row, &style_list_row, 0);
  lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START,
                         LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_gap(row, 6, 0);
  lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

  // Play btn
  lv_obj_t *bp = lv_btn_create(row);
  lv_obj_set_size(bp, 26, 26);
  lv_obj_set_style_bg_color(bp, C_PRI, 0);
  lv_obj_set_style_radius(bp, 13, 0);
  lv_obj_set_style_pad_all(bp, 0, 0);
  lv_obj_t *ip = lv_label_create(bp);
  lv_label_set_text(ip, LV_SYMBOL_PLAY);
  lv_obj_set_style_text_font(ip, &lv_font_montserrat_10, 0);
  lv_obj_center(ip);
  lv_obj_add_event_cb(bp, cb_song_tap, LV_EVENT_CLICKED, (void*)(intptr_t)idx);

  // Info
  lv_obj_t *col = lv_obj_create(row);
  lv_obj_set_flex_grow(col, 1);
  lv_obj_set_height(col, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_opa(col, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(col, 0, 0);
  lv_obj_set_style_pad_all(col, 0, 0);
  lv_obj_set_style_pad_gap(col, 0, 0);
  lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
  lv_obj_clear_flag(col, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(col, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(col, cb_song_tap, LV_EVENT_CLICKED, (void*)(intptr_t)idx);

  lv_obj_t *n = lv_label_create(col);
  lv_label_set_text(n, songs[idx]);
  lv_obj_set_style_text_font(n, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(n, C_TEXT, 0);
  lv_label_set_long_mode(n, LV_LABEL_LONG_DOT);
  lv_obj_set_width(n, 165);

  char sub[48]; char dur[8];
  fmt_time(durations[idx], dur, sizeof(dur));
  snprintf(sub, sizeof(sub), "%s  %s", artists[idx], dur);
  lv_obj_t *s = lv_label_create(col);
  lv_label_set_text(s, sub);
  lv_obj_set_style_text_font(s, &lv_font_montserrat_10, 0);
  lv_obj_set_style_text_color(s, C_TEXT_DIM, 0);

  // Action
  lv_obj_t *ba = lv_btn_create(row);
  lv_obj_set_size(ba, 26, 26);
  lv_obj_set_style_bg_color(ba, C_SURFACE, 0);
  lv_obj_set_style_radius(ba, 13, 0);
  lv_obj_set_style_pad_all(ba, 0, 0);
  lv_obj_t *ia = lv_label_create(ba);
  lv_obj_set_style_text_font(ia, &lv_font_montserrat_10, 0);
  lv_obj_center(ia);

  if (show_dl) {
    if (downloaded[idx]) {
      lv_label_set_text(ia, LV_SYMBOL_OK);
      lv_obj_set_style_text_color(ia, C_GREEN, 0);
    } else {
      lv_label_set_text(ia, LV_SYMBOL_DOWNLOAD);
      lv_obj_set_style_text_color(ia, C_CYAN, 0);
      lv_obj_add_event_cb(ba, cb_download, LV_EVENT_CLICKED, (void*)(intptr_t)idx);
    }
  } else {
    lv_label_set_text(ia, LV_SYMBOL_TRASH);
    lv_obj_set_style_text_color(ia, C_RED, 0);
    lv_obj_add_event_cb(ba, cb_delete_song, LV_EVENT_CLICKED, (void*)(intptr_t)idx);
  }
}

static void build_browser() {
  scr_browser = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr_browser, C_BG, 0);
  lv_obj_clear_flag(scr_browser, LV_OBJ_FLAG_SCROLLABLE);

  // Top
  lv_obj_t *top = lv_obj_create(scr_browser);
  lv_obj_set_size(top, SCREEN_W, 34);
  lv_obj_set_pos(top, 0, 0);
  lv_obj_set_style_bg_color(top, C_DARK, 0);
  lv_obj_set_style_bg_opa(top, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(top, 0, 0);
  lv_obj_set_style_radius(top, 0, 0);
  lv_obj_set_style_pad_hor(top, 4, 0);
  lv_obj_set_flex_flow(top, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(top, LV_FLEX_ALIGN_SPACE_BETWEEN,
                         LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_clear_flag(top, LV_OBJ_FLAG_SCROLLABLE);

  create_back_btn(top, ev_goto_home);

  lv_obj_t *t = lv_label_create(top);
  lv_label_set_text(t, "Music");
  lv_obj_set_style_text_font(t, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(t, C_TEXT, 0);

  // Voice search
  lv_obj_t *bmic = lv_btn_create(top);
  lv_obj_add_style(bmic, &style_btn_ghost, 0);
  lv_obj_set_size(bmic, 32, 28);
  lv_obj_t *imic = lv_label_create(bmic);
  lv_label_set_text(imic, LV_SYMBOL_AUDIO);
  lv_obj_set_style_text_font(imic, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(imic, C_CYAN, 0);
  lv_obj_center(imic);
  lv_obj_add_event_cb(bmic, cb_voice_search, LV_EVENT_CLICKED, NULL);

  // Tabs
  lv_obj_t *tabs = lv_obj_create(scr_browser);
  lv_obj_set_size(tabs, SCREEN_W - 12, 28);
  lv_obj_set_pos(tabs, 6, 36);
  lv_obj_set_style_bg_opa(tabs, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(tabs, 0, 0);
  lv_obj_set_style_pad_all(tabs, 0, 0);
  lv_obj_set_style_pad_gap(tabs, 6, 0);
  lv_obj_set_flex_flow(tabs, LV_FLEX_FLOW_ROW);
  lv_obj_clear_flag(tabs, LV_OBJ_FLAG_SCROLLABLE);

  tab_online = lv_btn_create(tabs);
  lv_obj_set_size(tab_online, 148, 24);
  lv_obj_set_style_bg_color(tab_online, C_PRI, 0);
  lv_obj_set_style_radius(tab_online, 12, 0);
  lv_obj_t *lt1 = lv_label_create(tab_online);
  lv_label_set_text(lt1, LV_SYMBOL_WIFI " Online");
  lv_obj_set_style_text_font(lt1, &lv_font_montserrat_12, 0);
  lv_obj_center(lt1);
  lv_obj_add_event_cb(tab_online, cb_tab_online, LV_EVENT_CLICKED, NULL);

  tab_downloaded = lv_btn_create(tabs);
  lv_obj_set_size(tab_downloaded, 148, 24);
  lv_obj_set_style_bg_color(tab_downloaded, C_CARD2, 0);
  lv_obj_set_style_radius(tab_downloaded, 12, 0);
  lv_obj_t *lt2 = lv_label_create(tab_downloaded);
  lv_label_set_text(lt2, LV_SYMBOL_SD_CARD " Downloaded");
  lv_obj_set_style_text_font(lt2, &lv_font_montserrat_12, 0);
  lv_obj_center(lt2);
  lv_obj_add_event_cb(tab_downloaded, cb_tab_downloaded, LV_EVENT_CLICKED, NULL);

  // Lists
  list_online = lv_obj_create(scr_browser);
  lv_obj_set_size(list_online, SCREEN_W - 6, SCREEN_H - 68);
  lv_obj_set_pos(list_online, 3, 66);
  lv_obj_set_style_bg_opa(list_online, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(list_online, 0, 0);
  lv_obj_set_style_pad_all(list_online, 3, 0);
  lv_obj_set_style_pad_gap(list_online, 3, 0);
  lv_obj_set_flex_flow(list_online, LV_FLEX_FLOW_COLUMN);
  for (int i = 0; i < NUM_SONGS; i++) build_song_row(list_online, i, true);

  list_downloaded = lv_obj_create(scr_browser);
  lv_obj_set_size(list_downloaded, SCREEN_W - 6, SCREEN_H - 68);
  lv_obj_set_pos(list_downloaded, 3, 66);
  lv_obj_set_style_bg_opa(list_downloaded, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(list_downloaded, 0, 0);
  lv_obj_set_style_pad_all(list_downloaded, 3, 0);
  lv_obj_set_style_pad_gap(list_downloaded, 3, 0);
  lv_obj_set_flex_flow(list_downloaded, LV_FLEX_FLOW_COLUMN);
  lv_obj_add_flag(list_downloaded, LV_OBJ_FLAG_HIDDEN);
  for (int i = 0; i < NUM_SONGS; i++)
    if (downloaded[i]) build_song_row(list_downloaded, i, false);

  // Listening modal
  modal_listen = lv_obj_create(scr_browser);
  lv_obj_set_size(modal_listen, 160, 88);
  lv_obj_center(modal_listen);
  lv_obj_set_style_bg_color(modal_listen, C_CARD, 0);
  lv_obj_set_style_bg_opa(modal_listen, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(modal_listen, 16, 0);
  lv_obj_set_style_border_width(modal_listen, 2, 0);
  lv_obj_set_style_border_color(modal_listen, C_CYAN, 0);
  lv_obj_set_style_shadow_width(modal_listen, 20, 0);
  lv_obj_set_style_shadow_color(modal_listen, C_CYAN, 0);
  lv_obj_set_style_shadow_opa(modal_listen, LV_OPA_30, 0);
  lv_obj_set_flex_flow(modal_listen, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(modal_listen, LV_FLEX_ALIGN_CENTER,
                         LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_clear_flag(modal_listen, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(modal_listen, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(modal_listen, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(modal_listen, [](lv_event_t *e) {
    lv_obj_add_flag(lv_event_get_target(e), LV_OBJ_FLAG_HIDDEN);
  }, LV_EVENT_CLICKED, NULL);

  lv_obj_t *mb = lv_label_create(modal_listen);
  lv_label_set_text(mb, LV_SYMBOL_AUDIO);
  lv_obj_set_style_text_font(mb, &lv_font_montserrat_28, 0);
  lv_obj_set_style_text_color(mb, C_CYAN, 0);

  lv_obj_t *ml = lv_label_create(modal_listen);
  lv_label_set_text(ml, "Listening...");
  lv_obj_set_style_text_font(ml, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(ml, C_TEXT, 0);
}

// ===================== BUILD NOW PLAYING =====================
static void build_nplay() {
  scr_nplay = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr_nplay, C_BG, 0);
  lv_obj_clear_flag(scr_nplay, LV_OBJ_FLAG_SCROLLABLE);

  create_back_btn(scr_nplay, ev_goto_browser);
  lv_obj_set_pos(lv_obj_get_child(scr_nplay, 0), 4, 4);

  lbl_np_song = lv_label_create(scr_nplay);
  lv_obj_set_width(lbl_np_song, SCREEN_W - 80);
  lv_label_set_long_mode(lbl_np_song, LV_LABEL_LONG_SCROLL_CIRCULAR);
  lv_obj_set_style_text_font(lbl_np_song, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(lbl_np_song, C_TEXT, 0);
  lv_obj_align(lbl_np_song, LV_ALIGN_TOP_MID, 0, 10);

  lbl_np_artist = lv_label_create(scr_nplay);
  lv_obj_set_style_text_font(lbl_np_artist, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(lbl_np_artist, C_TEXT_DIM, 0);
  lv_obj_align(lbl_np_artist, LV_ALIGN_TOP_MID, 0, 36);

  // EQ
  lv_obj_t *eq = lv_obj_create(scr_nplay);
  lv_obj_set_size(eq, 180, 50);
  lv_obj_align(eq, LV_ALIGN_TOP_MID, 0, 58);
  lv_obj_set_style_bg_opa(eq, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(eq, 0, 0);
  lv_obj_set_style_pad_all(eq, 0, 0);
  lv_obj_set_flex_flow(eq, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(eq, LV_FLEX_ALIGN_CENTER,
                         LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_gap(eq, 8, 0);
  lv_obj_clear_flag(eq, LV_OBJ_FLAG_SCROLLABLE);

  lv_color_t ec[] = {C_PRI_LIGHT, C_CYAN, C_PRI, C_GREEN, C_PRI_LIGHT, C_CYAN, C_PRI};
  for (int i = 0; i < 7; i++) {
    bars_eq[i] = lv_obj_create(eq);
    lv_obj_set_size(bars_eq[i], 12, 8);
    lv_obj_set_style_bg_color(bars_eq[i], ec[i], 0);
    lv_obj_set_style_bg_opa(bars_eq[i], LV_OPA_COVER, 0);
    lv_obj_set_style_radius(bars_eq[i], 4, 0);
    lv_obj_set_style_border_width(bars_eq[i], 0, 0);
    lv_obj_clear_flag(bars_eq[i], LV_OBJ_FLAG_SCROLLABLE);
  }

  bar_progress = lv_bar_create(scr_nplay);
  lv_obj_set_size(bar_progress, SCREEN_W - 60, 6);
  lv_obj_align(bar_progress, LV_ALIGN_TOP_MID, 0, 116);
  lv_obj_set_style_bg_color(bar_progress, C_CARD2, LV_PART_MAIN);
  lv_obj_set_style_bg_color(bar_progress, C_PRI, LV_PART_INDICATOR);
  lv_obj_set_style_radius(bar_progress, 3, LV_PART_MAIN);
  lv_obj_set_style_radius(bar_progress, 3, LV_PART_INDICATOR);

  lbl_cur_time = lv_label_create(scr_nplay);
  lv_obj_set_style_text_font(lbl_cur_time, &lv_font_montserrat_10, 0);
  lv_obj_set_style_text_color(lbl_cur_time, C_TEXT_DIM, 0);
  lv_obj_align(lbl_cur_time, LV_ALIGN_TOP_LEFT, 30, 126);
  lv_label_set_text(lbl_cur_time, "0:00");

  lbl_total_time = lv_label_create(scr_nplay);
  lv_obj_set_style_text_font(lbl_total_time, &lv_font_montserrat_10, 0);
  lv_obj_set_style_text_color(lbl_total_time, C_TEXT_DIM, 0);
  lv_obj_align(lbl_total_time, LV_ALIGN_TOP_RIGHT, -30, 126);

  // Controls
  lv_obj_t *ctrl = lv_obj_create(scr_nplay);
  lv_obj_set_size(ctrl, 220, 50);
  lv_obj_align(ctrl, LV_ALIGN_TOP_MID, 0, 140);
  lv_obj_set_style_bg_opa(ctrl, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(ctrl, 0, 0);
  lv_obj_set_style_pad_all(ctrl, 0, 0);
  lv_obj_set_flex_flow(ctrl, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(ctrl, LV_FLEX_ALIGN_SPACE_EVENLY,
                         LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_clear_flag(ctrl, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *bprev = lv_btn_create(ctrl);
  lv_obj_add_style(bprev, &style_btn_ghost, 0);
  lv_obj_set_size(bprev, 44, 44);
  lv_obj_t *iprev = lv_label_create(bprev);
  lv_label_set_text(iprev, LV_SYMBOL_PREV);
  lv_obj_set_style_text_font(iprev, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(iprev, C_TEXT, 0);
  lv_obj_center(iprev);
  lv_obj_add_event_cb(bprev, cb_prev_song, LV_EVENT_CLICKED, NULL);

  lv_obj_t *bplay = lv_btn_create(ctrl);
  lv_obj_set_size(bplay, 52, 52);
  lv_obj_set_style_bg_color(bplay, C_PRI, 0);
  lv_obj_set_style_radius(bplay, 26, 0);
  lv_obj_set_style_shadow_width(bplay, 16, 0);
  lv_obj_set_style_shadow_color(bplay, C_PRI_DARK, 0);
  lv_obj_set_style_shadow_opa(bplay, LV_OPA_50, 0);
  lbl_play_icon = lv_label_create(bplay);
  lv_obj_set_style_text_font(lbl_play_icon, &lv_font_montserrat_20, 0);
  lv_obj_center(lbl_play_icon);
  lv_obj_add_event_cb(bplay, cb_play_pause, LV_EVENT_CLICKED, NULL);

  lv_obj_t *bnext = lv_btn_create(ctrl);
  lv_obj_add_style(bnext, &style_btn_ghost, 0);
  lv_obj_set_size(bnext, 44, 44);
  lv_obj_t *inext = lv_label_create(bnext);
  lv_label_set_text(inext, LV_SYMBOL_NEXT);
  lv_obj_set_style_text_font(inext, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(inext, C_TEXT, 0);
  lv_obj_center(inext);
  lv_obj_add_event_cb(bnext, cb_next_song, LV_EVENT_CLICKED, NULL);

  // Volume
  lv_obj_t *vr = lv_obj_create(scr_nplay);
  lv_obj_set_size(vr, SCREEN_W - 40, 32);
  lv_obj_align(vr, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_set_style_bg_opa(vr, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(vr, 0, 0);
  lv_obj_set_style_pad_all(vr, 0, 0);
  lv_obj_set_flex_flow(vr, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(vr, LV_FLEX_ALIGN_CENTER,
                         LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_gap(vr, 8, 0);
  lv_obj_clear_flag(vr, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *iv = lv_label_create(vr);
  lv_label_set_text(iv, LV_SYMBOL_VOLUME_MID);
  lv_obj_set_style_text_color(iv, C_TEXT_DIM, 0);
  lv_obj_set_style_text_font(iv, &lv_font_montserrat_14, 0);

  slider_vol = lv_slider_create(vr);
  lv_obj_set_width(slider_vol, 160);
  lv_obj_set_height(slider_vol, 6);
  lv_slider_set_range(slider_vol, 0, 100);
  lv_slider_set_value(slider_vol, volume, LV_ANIM_OFF);
  lv_obj_set_style_bg_color(slider_vol, C_CARD2, LV_PART_MAIN);
  lv_obj_set_style_bg_color(slider_vol, C_PRI, LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(slider_vol, C_PRI_LIGHT, LV_PART_KNOB);
  lv_obj_set_style_pad_all(slider_vol, 4, LV_PART_KNOB);
  lv_obj_add_event_cb(slider_vol, cb_volume, LV_EVENT_VALUE_CHANGED, NULL);

  lbl_vol_val = lv_label_create(vr);
  char vb[8]; snprintf(vb, sizeof(vb), "%d%%", volume);
  lv_label_set_text(lbl_vol_val, vb);
  lv_obj_set_style_text_font(lbl_vol_val, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(lbl_vol_val, C_TEXT_DIM, 0);

  update_np_info();
}

// ===================== BUILD INCOMING CALL =====================
static void build_incoming() {
  scr_incoming = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr_incoming, C_DARK, 0);
  lv_obj_clear_flag(scr_incoming, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *c = lv_obj_create(scr_incoming);
  lv_obj_set_size(c, SCREEN_W, SCREEN_H);
  lv_obj_center(c);
  lv_obj_set_style_bg_opa(c, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(c, 0, 0);
  lv_obj_set_flex_flow(c, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(c, LV_FLEX_ALIGN_CENTER,
                         LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_gap(c, 10, 0);
  lv_obj_clear_flag(c, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *rc = lv_obj_create(c);
  lv_obj_set_size(rc, 60, 60);
  lv_obj_set_style_bg_color(rc, C_CARD, 0);
  lv_obj_set_style_bg_opa(rc, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(rc, 30, 0);
  lv_obj_set_style_border_width(rc, 2, 0);
  lv_obj_set_style_border_color(rc, C_GREEN, 0);
  lv_obj_clear_flag(rc, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_t *ir = lv_label_create(rc);
  lv_label_set_text(ir, LV_SYMBOL_CALL);
  lv_obj_set_style_text_font(ir, &lv_font_montserrat_28, 0);
  lv_obj_set_style_text_color(ir, C_GREEN, 0);
  lv_obj_center(ir);

  lv_obj_t *li = lv_label_create(c);
  lv_label_set_text(li, "Incoming Call");
  lv_obj_set_style_text_font(li, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(li, C_TEXT_DIM, 0);

  lv_obj_t *ln = lv_label_create(c);
  lv_label_set_text(ln, caller_number);
  lv_obj_set_style_text_font(ln, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(ln, C_TEXT, 0);

  lv_obj_t *sp = lv_obj_create(c);
  lv_obj_set_size(sp, 1, 10);
  lv_obj_set_style_bg_opa(sp, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(sp, 0, 0);

  lv_obj_t *btns = lv_obj_create(c);
  lv_obj_set_size(btns, 200, 60);
  lv_obj_set_style_bg_opa(btns, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(btns, 0, 0);
  lv_obj_set_style_pad_all(btns, 0, 0);
  lv_obj_set_flex_flow(btns, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(btns, LV_FLEX_ALIGN_SPACE_EVENLY,
                         LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_clear_flag(btns, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *br = lv_btn_create(btns);
  lv_obj_set_size(br, 56, 56);
  lv_obj_set_style_bg_color(br, C_RED, 0);
  lv_obj_set_style_radius(br, 28, 0);
  lv_obj_set_style_shadow_width(br, 12, 0);
  lv_obj_set_style_shadow_color(br, C_RED, 0);
  lv_obj_set_style_shadow_opa(br, LV_OPA_40, 0);
  lv_obj_t *ire = lv_label_create(br);
  lv_label_set_text(ire, LV_SYMBOL_CLOSE);
  lv_obj_set_style_text_font(ire, &lv_font_montserrat_20, 0);
  lv_obj_center(ire);
  lv_obj_add_event_cb(br, cb_reject_call, LV_EVENT_CLICKED, NULL);

  lv_obj_t *ba = lv_btn_create(btns);
  lv_obj_set_size(ba, 56, 56);
  lv_obj_set_style_bg_color(ba, C_GREEN, 0);
  lv_obj_set_style_radius(ba, 28, 0);
  lv_obj_set_style_shadow_width(ba, 12, 0);
  lv_obj_set_style_shadow_color(ba, C_GREEN, 0);
  lv_obj_set_style_shadow_opa(ba, LV_OPA_40, 0);
  lv_obj_t *iac = lv_label_create(ba);
  lv_label_set_text(iac, LV_SYMBOL_CALL);
  lv_obj_set_style_text_font(iac, &lv_font_montserrat_20, 0);
  lv_obj_center(iac);
  lv_obj_add_event_cb(ba, cb_accept_call, LV_EVENT_CLICKED, NULL);
}

// ===================== BUILD IN-CALL =====================
static void build_incall() {
  scr_incall = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr_incall, C_DARK, 0);
  lv_obj_clear_flag(scr_incall, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *c = lv_obj_create(scr_incall);
  lv_obj_set_size(c, SCREEN_W, SCREEN_H);
  lv_obj_center(c);
  lv_obj_set_style_bg_opa(c, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(c, 0, 0);
  lv_obj_set_flex_flow(c, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(c, LV_FLEX_ALIGN_CENTER,
                         LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_gap(c, 8, 0);
  lv_obj_clear_flag(c, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *iph = lv_label_create(c);
  lv_label_set_text(iph, LV_SYMBOL_CALL);
  lv_obj_set_style_text_font(iph, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(iph, C_GREEN, 0);

  lv_obj_t *loc = lv_label_create(c);
  lv_label_set_text(loc, "On Call");
  lv_obj_set_style_text_font(loc, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(loc, C_GREEN, 0);

  lv_obj_t *lnum = lv_label_create(c);
  lv_label_set_text(lnum, caller_number);
  lv_obj_set_style_text_font(lnum, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(lnum, C_TEXT, 0);

  lbl_call_timer = lv_label_create(c);
  lv_label_set_text(lbl_call_timer, "00:00");
  lv_obj_set_style_text_font(lbl_call_timer, &lv_font_montserrat_28, 0);
  lv_obj_set_style_text_color(lbl_call_timer, C_TEXT, 0);

  lv_obj_t *sp = lv_obj_create(c);
  lv_obj_set_size(sp, 1, 8);
  lv_obj_set_style_bg_opa(sp, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(sp, 0, 0);

  lv_obj_t *btns = lv_obj_create(c);
  lv_obj_set_size(btns, 230, 56);
  lv_obj_set_style_bg_opa(btns, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(btns, 0, 0);
  lv_obj_set_style_pad_all(btns, 0, 0);
  lv_obj_set_flex_flow(btns, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(btns, LV_FLEX_ALIGN_SPACE_EVENLY,
                         LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_clear_flag(btns, LV_OBJ_FLAG_SCROLLABLE);

  btn_mic = lv_btn_create(btns);
  lv_obj_set_size(btn_mic, 50, 50);
  lv_obj_set_style_bg_color(btn_mic, C_CARD2, 0);
  lv_obj_set_style_radius(btn_mic, 25, 0);
  lbl_mic_icon = lv_label_create(btn_mic);
  lv_label_set_text(lbl_mic_icon, LV_SYMBOL_AUDIO);
  lv_obj_set_style_text_font(lbl_mic_icon, &lv_font_montserrat_20, 0);
  lv_obj_center(lbl_mic_icon);
  lv_obj_add_event_cb(btn_mic, cb_toggle_mic, LV_EVENT_CLICKED, NULL);

  btn_speaker = lv_btn_create(btns);
  lv_obj_set_size(btn_speaker, 50, 50);
  lv_obj_set_style_bg_color(btn_speaker, C_CARD2, 0);
  lv_obj_set_style_radius(btn_speaker, 25, 0);
  lbl_spk_icon = lv_label_create(btn_speaker);
  lv_label_set_text(lbl_spk_icon, LV_SYMBOL_VOLUME_MID);
  lv_obj_set_style_text_font(lbl_spk_icon, &lv_font_montserrat_20, 0);
  lv_obj_center(lbl_spk_icon);
  lv_obj_add_event_cb(btn_speaker, cb_toggle_speaker, LV_EVENT_CLICKED, NULL);

  lv_obj_t *be = lv_btn_create(btns);
  lv_obj_set_size(be, 50, 50);
  lv_obj_set_style_bg_color(be, C_RED, 0);
  lv_obj_set_style_radius(be, 25, 0);
  lv_obj_set_style_shadow_width(be, 12, 0);
  lv_obj_set_style_shadow_color(be, C_RED, 0);
  lv_obj_set_style_shadow_opa(be, LV_OPA_40, 0);
  lv_obj_t *ie = lv_label_create(be);
  lv_label_set_text(ie, LV_SYMBOL_CALL);
  lv_obj_set_style_text_font(ie, &lv_font_montserrat_20, 0);
  lv_obj_center(ie);
  lv_obj_add_event_cb(be, cb_end_call, LV_EVENT_CLICKED, NULL);
}

// ===================== BUILD SETTINGS =====================
static lv_obj_t *settings_row(lv_obj_t *p) {
  lv_obj_t *r = lv_obj_create(p);
  lv_obj_set_size(r, lv_pct(100), LV_SIZE_CONTENT);
  lv_obj_set_style_bg_color(r, C_CARD2, 0);
  lv_obj_set_style_bg_opa(r, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(r, 10, 0);
  lv_obj_set_style_border_width(r, 0, 0);
  lv_obj_set_style_pad_all(r, 8, 0);
  lv_obj_set_flex_flow(r, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(r, LV_FLEX_ALIGN_SPACE_BETWEEN,
                         LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_clear_flag(r, LV_OBJ_FLAG_SCROLLABLE);
  return r;
}

static void settings_section(lv_obj_t *p, const char *t) {
  lv_obj_t *l = lv_label_create(p);
  lv_label_set_text(l, t);
  lv_obj_set_style_text_font(l, &lv_font_montserrat_10, 0);
  lv_obj_set_style_text_color(l, C_PRI_LIGHT, 0);
  lv_obj_set_style_pad_top(l, 4, 0);
}

static void build_settings() {
  scr_settings = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr_settings, C_BG, 0);
  lv_obj_clear_flag(scr_settings, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *top = lv_obj_create(scr_settings);
  lv_obj_set_size(top, SCREEN_W, 34);
  lv_obj_set_pos(top, 0, 0);
  lv_obj_set_style_bg_color(top, C_DARK, 0);
  lv_obj_set_style_bg_opa(top, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(top, 0, 0);
  lv_obj_set_style_radius(top, 0, 0);
  lv_obj_set_style_pad_hor(top, 4, 0);
  lv_obj_set_flex_flow(top, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(top, LV_FLEX_ALIGN_START,
                         LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_gap(top, 8, 0);
  lv_obj_clear_flag(top, LV_OBJ_FLAG_SCROLLABLE);

  create_back_btn(top, ev_goto_home);
  lv_obj_t *tt = lv_label_create(top);
  lv_label_set_text(tt, LV_SYMBOL_SETTINGS " Settings");
  lv_obj_set_style_text_font(tt, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(tt, C_TEXT, 0);

  lv_obj_t *sc = lv_obj_create(scr_settings);
  lv_obj_set_size(sc, SCREEN_W - 8, SCREEN_H - 38);
  lv_obj_set_pos(sc, 4, 36);
  lv_obj_set_style_bg_opa(sc, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(sc, 0, 0);
  lv_obj_set_style_pad_all(sc, 4, 0);
  lv_obj_set_style_pad_gap(sc, 4, 0);
  lv_obj_set_flex_flow(sc, LV_FLEX_FLOW_COLUMN);

  // BT
  settings_section(sc, "BLUETOOTH");
  lv_obj_t *rbt = settings_row(sc);
  lv_obj_t *btl = lv_obj_create(rbt);
  lv_obj_set_size(btl, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_opa(btl, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(btl, 0, 0);
  lv_obj_set_style_pad_all(btl, 0, 0);
  lv_obj_set_style_pad_gap(btl, 0, 0);
  lv_obj_set_flex_flow(btl, LV_FLEX_FLOW_COLUMN);
  lv_obj_clear_flag(btl, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_t *lb = lv_label_create(btl);
  lv_label_set_text(lb, LV_SYMBOL_BLUETOOTH " Bluetooth");
  lv_obj_set_style_text_font(lb, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(lb, C_TEXT, 0);
  lv_obj_t *ld = lv_label_create(btl);
  lv_label_set_text(ld, "iPhone 15 Pro");
  lv_obj_set_style_text_font(ld, &lv_font_montserrat_10, 0);
  lv_obj_set_style_text_color(ld, C_TEXT_DIM, 0);

  sw_bt = lv_switch_create(rbt);
  lv_obj_set_size(sw_bt, 40, 22);
  if (bt_on) lv_obj_add_state(sw_bt, LV_STATE_CHECKED);
  lv_obj_set_style_bg_color(sw_bt, C_CARD, LV_PART_MAIN);
  lv_obj_set_style_bg_color(sw_bt, C_PRI, LV_PART_INDICATOR | LV_STATE_CHECKED);
  lv_obj_add_event_cb(sw_bt, cb_bt_toggle, LV_EVENT_VALUE_CHANGED, NULL);

  // WiFi
  settings_section(sc, "WIFI");
  lv_obj_t *rwf = settings_row(sc);
  lv_obj_t *wfl = lv_obj_create(rwf);
  lv_obj_set_size(wfl, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_opa(wfl, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(wfl, 0, 0);
  lv_obj_set_style_pad_all(wfl, 0, 0);
  lv_obj_set_style_pad_gap(wfl, 0, 0);
  lv_obj_set_flex_flow(wfl, LV_FLEX_FLOW_COLUMN);
  lv_obj_clear_flag(wfl, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_t *lw = lv_label_create(wfl);
  lv_label_set_text(lw, LV_SYMBOL_WIFI " WiFi");
  lv_obj_set_style_text_font(lw, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(lw, C_TEXT, 0);
  lv_obj_t *ls = lv_label_create(wfl);
  lv_label_set_text(ls, "Home_WiFi_5G");
  lv_obj_set_style_text_font(ls, &lv_font_montserrat_10, 0);
  lv_obj_set_style_text_color(ls, C_TEXT_DIM, 0);

  sw_wifi = lv_switch_create(rwf);
  lv_obj_set_size(sw_wifi, 40, 22);
  if (wifi_on) lv_obj_add_state(sw_wifi, LV_STATE_CHECKED);
  lv_obj_set_style_bg_color(sw_wifi, C_CARD, LV_PART_MAIN);
  lv_obj_set_style_bg_color(sw_wifi, C_GREEN, LV_PART_INDICATOR | LV_STATE_CHECKED);
  lv_obj_add_event_cb(sw_wifi, cb_wifi_toggle, LV_EVENT_VALUE_CHANGED, NULL);

  // Brightness
  settings_section(sc, "DISPLAY");
  lv_obj_t *rbr = settings_row(sc);
  lv_obj_t *lbr = lv_label_create(rbr);
  lv_label_set_text(lbr, LV_SYMBOL_IMAGE " Brightness");
  lv_obj_set_style_text_font(lbr, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(lbr, C_TEXT, 0);
  lv_obj_t *sbr = lv_slider_create(rbr);
  lv_obj_set_width(sbr, 130); lv_obj_set_height(sbr, 6);
  lv_slider_set_range(sbr, 10, 100);
  lv_slider_set_value(sbr, brightness, LV_ANIM_OFF);
  lv_obj_set_style_bg_color(sbr, C_CARD, LV_PART_MAIN);
  lv_obj_set_style_bg_color(sbr, C_ORANGE, LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(sbr, C_ORANGE, LV_PART_KNOB);
  lv_obj_set_style_pad_all(sbr, 4, LV_PART_KNOB);
  lv_obj_add_event_cb(sbr, cb_brightness, LV_EVENT_VALUE_CHANGED, NULL);

  // Volume
  lv_obj_t *rvl = settings_row(sc);
  lv_obj_t *lvl = lv_label_create(rvl);
  lv_label_set_text(lvl, LV_SYMBOL_VOLUME_MAX " Volume");
  lv_obj_set_style_text_font(lvl, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(lvl, C_TEXT, 0);
  lv_obj_t *svl = lv_slider_create(rvl);
  lv_obj_set_width(svl, 130); lv_obj_set_height(svl, 6);
  lv_slider_set_range(svl, 0, 100);
  lv_slider_set_value(svl, volume, LV_ANIM_OFF);
  lv_obj_set_style_bg_color(svl, C_CARD, LV_PART_MAIN);
  lv_obj_set_style_bg_color(svl, C_PRI, LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(svl, C_PRI_LIGHT, LV_PART_KNOB);
  lv_obj_set_style_pad_all(svl, 4, LV_PART_KNOB);
  lv_obj_add_event_cb(svl, [](lv_event_t *e) {
    volume = lv_slider_get_value(lv_event_get_target(e));
    if (slider_vol) lv_slider_set_value(slider_vol, volume, LV_ANIM_ON);
    char b[8]; snprintf(b, sizeof(b), "%d%%", volume);
    if (lbl_vol_val) lv_label_set_text(lbl_vol_val, b);
  }, LV_EVENT_VALUE_CHANGED, NULL);

  // Storage
  settings_section(sc, "STORAGE");
  lv_obj_t *rsd = settings_row(sc);
  lv_obj_set_flex_flow(rsd, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(rsd, LV_FLEX_ALIGN_START,
                         LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  lv_obj_t *sdtop = lv_obj_create(rsd);
  lv_obj_set_size(sdtop, lv_pct(100), LV_SIZE_CONTENT);
  lv_obj_set_style_bg_opa(sdtop, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(sdtop, 0, 0);
  lv_obj_set_style_pad_all(sdtop, 0, 0);
  lv_obj_set_flex_flow(sdtop, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(sdtop, LV_FLEX_ALIGN_SPACE_BETWEEN,
                         LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_clear_flag(sdtop, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_t *lsd = lv_label_create(sdtop);
  lv_label_set_text(lsd, LV_SYMBOL_SD_CARD " SD Card");
  lv_obj_set_style_text_font(lsd, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(lsd, C_TEXT, 0);
  lv_obj_t *lsz = lv_label_create(sdtop);
  lv_label_set_text(lsz, "2.1 / 8.0 GB");
  lv_obj_set_style_text_font(lsz, &lv_font_montserrat_10, 0);
  lv_obj_set_style_text_color(lsz, C_TEXT_DIM, 0);
  lv_obj_t *bsd = lv_bar_create(rsd);
  lv_obj_set_size(bsd, lv_pct(100), 8);
  lv_bar_set_range(bsd, 0, 100);
  lv_bar_set_value(bsd, 26, LV_ANIM_OFF);
  lv_obj_set_style_bg_color(bsd, C_CARD, LV_PART_MAIN);
  lv_obj_set_style_bg_color(bsd, C_CYAN, LV_PART_INDICATOR);
  lv_obj_set_style_radius(bsd, 4, LV_PART_MAIN);
  lv_obj_set_style_radius(bsd, 4, LV_PART_INDICATOR);

  // About
  settings_section(sc, "ABOUT");
  lv_obj_t *rab = settings_row(sc);
  lv_obj_set_flex_flow(rab, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(rab, LV_FLEX_ALIGN_START,
                         LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_gap(rab, 2, 0);
  lv_obj_t *ldev = lv_label_create(rab);
  lv_label_set_text(ldev, "ESP32 Audio Hub v1.0");
  lv_obj_set_style_text_font(ldev, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(ldev, C_TEXT, 0);
  lv_obj_t *lfw = lv_label_create(rab);
  lv_label_set_text(lfw, "LVGL 9.x | ILI9341 320x240");
  lv_obj_set_style_text_font(lfw, &lv_font_montserrat_10, 0);
  lv_obj_set_style_text_color(lfw, C_TEXT_DIM, 0);
}

// ===================== DISPLAY & TOUCH (LVGL 9) =====================
static void disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  uint32_t w = lv_area_get_width(area);
  uint32_t h = lv_area_get_height(area);
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)px_map, w * h, true);
  tft.endWrite();
  lv_display_flush_ready(disp);
}

static void touch_read(lv_indev_t *indev, lv_indev_data_t *data) {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    data->point.x = map(p.x, 200, 3800, 0, SCREEN_W - 1);
    data->point.y = map(p.y, 200, 3800, 0, SCREEN_H - 1);
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);

  pinMode(TFT_BL, OUTPUT);
  analogWrite(TFT_BL, map(brightness, 0, 100, 0, 255));

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  ts.begin();
  ts.setRotation(1);

  lv_init();

  lv_display_t *disp = lv_display_create(SCREEN_W, SCREEN_H);
  lv_display_set_flush_cb(disp, disp_flush);
  lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf),
                          LV_DISPLAY_RENDER_MODE_PARTIAL);

  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touch_read);

  init_styles();

  build_home();
  build_browser();
  build_nplay();
  build_incoming();
  build_incall();
  build_settings();

  timer_music = lv_timer_create(music_timer_cb, 1000, NULL);
  timer_call  = lv_timer_create(call_timer_cb, 1000, NULL);
  timer_pet   = lv_timer_create(pet_anim_cb, 250, NULL);
  lv_timer_create(eq_timer_cb, 150, NULL);
  lv_timer_create(clock_timer_cb, 60000, NULL);

  lv_scr_load(scr_home);
  Serial.println("ESP32 Audio Hub v1.0 — LVGL 9 + LeafDra Pet");
}

// ===================== LOOP =====================
void loop() {
  lv_timer_handler();
  delay(5);
}
