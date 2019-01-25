#pragma once

#include "Button.h"

class PushButtonLongShortPress {
  public:
    PushButtonLongShortPress(pin_t pin) : button(pin) {}
    void begin() {
      button.begin();
    }

    enum State : uint8_t {
      Pressed          = 0b00,
      Released         = 0b11,
      Falling          = 0b10,
      LongPressRising  = 0b01,
      ShortPressRising = 0b100,
      LongPress        = 0b101
    };

    State getState() {
      Button::State state = button.getState();
      switch (state) {
        case Button::Falling:
          longPress = false;
          return Falling;
        case Button::Rising:
          return longPress ? LongPressRising : ShortPressRising;
        case Button::Pressed:
          if (!longPress && button.stableTime() > longPressDuration) {
            longPress = true;
            return LongPress;
          }
          return Pressed;
        case Button::Released:
          return Released;
      }
    }

    pin_t getPin() const { return button.pin; }
    
  private:
    static constexpr unsigned long longPressDuration = 300; // ms
    Button button;
    bool longPress = false;
};
