#include <lvgl.h>

void ui_call_create()
{
    lv_obj_clean(lv_scr_act());

    lv_obj_t *title = lv_label_create(lv_scr_act());
    lv_label_set_text(title, "CALL");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t *contact = lv_btn_create(lv_scr_act());
    lv_obj_set_size(contact, 200, 50);
    lv_obj_align(contact, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *label = lv_label_create(contact);
    lv_label_set_text(label, "John");
    lv_obj_center(label);
}