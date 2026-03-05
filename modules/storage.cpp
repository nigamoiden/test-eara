#include <Arduino.h>
#include <Preferences.h>

Preferences prefs;

void storage_init()
{
    prefs.begin("eara", false);
}

void save_history(String song)
{
    prefs.putString("last_song", song);
}

String load_history()
{
    return prefs.getString("last_song", "");
}