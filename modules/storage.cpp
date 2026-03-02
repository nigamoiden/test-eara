#include <Arduino.h>

void audio_init() {
    Serial.println("Audio Init");
}

void audio_set_volume(int level) {
    Serial.println("Set volume: " + String(level));
}

void audio_play() {
    Serial.println("Play");
}

void audio_pause() {
    Serial.println("Pause");
}
