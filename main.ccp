#include <Arduino.h>
#include <lvgl.h>
#include "modules/state_manager.h"
#include "modules/bluetooth.h"
#include "modules/audio.h"
#include "modules/storage.h"

TaskHandle_t uiTaskHandle;
TaskHandle_t systemTaskHandle;

void uiTask(void *pvParameters) {
    lv_init();
    state_init();     // khởi tạo state machine
    ui_show_home();   // màn hình đầu tiên

    while (1) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void systemTask(void *pvParameters) {
    bluetooth_init();
    audio_init();
    storage_init();

    while (1) {
        bluetooth_loop();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void setup() {
    Serial.begin(115200);

    xTaskCreatePinnedToCore(
        uiTask,
        "UI Task",
        8192,
        NULL,
        1,
        &uiTaskHandle,
        1
    );

    xTaskCreatePinnedToCore(
        systemTask,
        "System Task",
        8192,
        NULL,
        1,
        &systemTaskHandle,
        0
    );
}

void loop() {}
