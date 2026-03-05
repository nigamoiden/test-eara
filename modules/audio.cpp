#include <Arduino.h>

void audio_init()
{
    Serial.println("Audio init");
}

void audio_play(String url)
{
    Serial.println("Play: " + url);
}

void audio_pause()
{
    Serial.println("Pause");
}

void audio_next()
{
    Serial.println("Next song");
}