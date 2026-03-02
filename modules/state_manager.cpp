#include <Arduino.h>

enum SystemState {
    STATE_HOME,
    STATE_MUSIC,
    STATE_CALL,
    STATE_SETTINGS
};

static SystemState currentState;

void state_init() {
    currentState = STATE_HOME;
}

void state_set(SystemState newState) {
    currentState = newState;
}

SystemState state_get() {
    return currentState;
}
