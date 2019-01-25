#include "Button.h"

Button::Button(pin_t pin) : pin(pin) {}

void Button::begin() { pinMode(pin, INPUT_PULLUP); }

void Button::invert() { invertState = true; }

bool Button::invertState = false;

Button::State Button::getState() {
    State rstate;
    // read the button state and invert it if "invertState" is true
    bool state = digitalRead(pin) ^ invertState;
    unsigned long now = millis();
    if (now - prevBounceTime > debounceTime) { // wait for state to stabilize
        rstate = static_cast<State>((debouncedState << 1) | state);
        debouncedState = state;
    } else {
        rstate = static_cast<State>((debouncedState << 1) | debouncedState);
    }
    if (state != prevState) { // Button is pressed, released or bounces
        prevBounceTime = now;
        prevState = state;
    }
    return rstate;
}

unsigned long Button::stableTime() { return millis() - prevBounceTime; }
