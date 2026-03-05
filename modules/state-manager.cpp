#include <Arduino.h>

enum ScreenState
{
    HOME,
    MUSIC,
    CALL,
    SETTINGS
};

ScreenState currentScreen;

void state_init()
{
    currentScreen = HOME;
}

void set_state(ScreenState s)
{
    currentScreen = s;
}

ScreenState get_state()
{
    return currentScreen;
}