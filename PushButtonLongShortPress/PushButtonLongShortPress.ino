#include "PushButtonLongShortPress.h"

PushButtonLongShortPress buttons[] = { 4, 5, 6, 7 };

void setup() {
  Serial.begin(115200);

  for (PushButtonLongShortPress &button : buttons)
    button.begin();
}

void loop() {
  for (PushButtonLongShortPress &button : buttons) {
    PushButtonLongShortPress::State state = button.getState();

    switch (state) {
      case PushButtonLongShortPress::Pressed:
        break;

      case PushButtonLongShortPress::Released:
        break;

      case PushButtonLongShortPress::Falling:
        Serial.print("Pressed #"); 
        Serial.println(button.getPin());
        break;

      case PushButtonLongShortPress::LongPressRising:
        Serial.print("Long press (released) #"); 
        Serial.println(button.getPin());
        break;

      case PushButtonLongShortPress::ShortPressRising:
        Serial.print("Short press (released) #");
        Serial.println(button.getPin());
        break;

      case PushButtonLongShortPress::LongPress:
        Serial.print("Long press (still pressed) #");
        Serial.println(button.getPin());
        break;
    }
  }
}
