#include <Arduino.h>
#include "BluetoothA2DPSource.h"

BluetoothA2DPSource a2dp;

void bluetooth_init() {
    a2dp.start("Eara_Device");
}

void bluetooth_send_command(String json) {
    Serial.println("Sending: " + json);
    // Sau này gửi BLE JSON thật
}

void bluetooth_loop() {
    // xử lý event bluetooth
}
