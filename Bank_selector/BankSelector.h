#include "Arduino.h"

#define EXT_PIN(x) (x + NUM_DIGITAL_PINS)

class BankSelector {
  public:
    enum ButtonType {
      TOGGLE, MOMENTARY
    };

    BankSelector(Bank& bank, uint8_t switchPin, ButtonType buttonType = MOMENTARY) // One switch or button, no LEDs
      : bank(bank), switchPin(switchPin) {
      mode = buttonType == TOGGLE ? SINGLE_SWITCH : SINGLE_BUTTON;
    }
    BankSelector(Bank& bank, uint8_t switchPin, uint8_t ledPin, ButtonType buttonType = MOMENTARY) // One switch or button, one LED
      : bank(bank), switchPin(switchPin), ledPin(ledPin) {
      mode = buttonType == TOGGLE ? SINGLE_SWITCH_LED : SINGLE_BUTTON_LED;
    }
    template<size_t N>
    BankSelector(Bank& bank, const uint8_t (&switchPins)[N]) // Multiple buttons, no LEDs
      : bank(bank), switchPins(switchPins), nb_banks(N) {
      mode = MULTIPLE_BUTTONS;
    }
    template<size_t N>
    BankSelector(Bank& bank, const uint8_t (&switchPins)[N], const uint8_t (&ledPins)[N]) // Multiple buttons, multiple LEDs
      : bank(bank), switchPins(switchPins), ledPins(ledPins), nb_banks(N) {
      mode = MULTIPLE_BUTTONS_LEDS;
    }
    BankSelector(Bank& bank, uint8_t pinIncrement, uint8_t pinDecrement, uint8_t nb_banks) // Two buttons (+1, -1), no LEDs
      : bank(bank), pinIncrement(pinIncrement), pinDecrement(pinDecrement), nb_banks(nb_banks) {
      mode = INCREMENT_DECREMENT;
    }
    template<size_t N>
    BankSelector(Bank& bank, uint8_t pinIncrement, uint8_t pinDecrement, const uint8_t (&ledPins)[N]) // Two buttons (+1, -1), multiple LEDs
      : bank(bank), pinIncrement(pinIncrement), pinDecrement(pinDecrement), ledPins(ledPins), nb_banks(N) {
      mode = INCREMENT_DECREMENT_LEDS;
    }

    void init() {
      switch (mode) {
        case SINGLE_SWITCH:
        case SINGLE_BUTTON:
          pinMode(switchPin, INPUT_PULLUP);
          break;
        case SINGLE_SWITCH_LED:
        case SINGLE_BUTTON_LED:
          pinMode(switchPin, INPUT_PULLUP);
          pinMode(ledPin, OUTPUT);
          break;
        case MULTIPLE_BUTTONS:
          for (uint8_t i = 0; i < nb_banks; i++) {
            pinMode(switchPins[i], INPUT_PULLUP);
          }
          break;
        case MULTIPLE_BUTTONS_LEDS:
          for (uint8_t i = 0; i < nb_banks; i++) {
            pinMode(switchPins[i], INPUT_PULLUP);
            pinMode(ledPins[i], OUTPUT);
          }
          digitalWrite(ledPins[0], HIGH);
          break;
        case INCREMENT_DECREMENT:
          pinMode(pinIncrement, INPUT_PULLUP);
          pinMode(pinDecrement, INPUT_PULLUP);
          break;
        case INCREMENT_DECREMENT_LEDS:
          pinMode(pinIncrement, INPUT_PULLUP);
          pinMode(pinDecrement, INPUT_PULLUP);
          for (uint8_t i = 0; i < nb_banks; i++) {
            pinMode(ledPins[i], OUTPUT);
          }
          digitalWrite(ledPins[0], HIGH);
          break;
      }
    }

    void refresh() {
      uint8_t newChannel = channel;
      switch (mode) {
        case SINGLE_SWITCH:
        case SINGLE_SWITCH_LED:
          {
            newChannel = (uint8_t)(!digitalRead(switchPin)) + 1;
          }
          break;

        case SINGLE_BUTTON:
        case SINGLE_BUTTON_LED:
          {
            bool ToggleState = digitalRead(switchPin);

            int8_t stateChangeToggle = ToggleState - prevToggleState;

            if (stateChangeToggle == falling) { // Button is pushed
              if (millis() - prevBounceTime > debounceTime) {
                newChannel = !(channel - 1) + 1; // Toggle channel between 1 and 2
                prevBounceTime = millis();
              }
            }
            if (stateChangeToggle == rising) { // Button is released
              prevBounceTime = millis();
            }
            prevToggleState = ToggleState;
          }
          break;

        case MULTIPLE_BUTTONS:
        case MULTIPLE_BUTTONS_LEDS:
          {
            for (uint8_t i = 0; i < nb_banks; i++) {
              if (digitalRead(switchPins[i]) == LOW) {
                newChannel = i + 1;
                break;
              }
            }
          }
          break;

        case INCREMENT_DECREMENT_LEDS:
        case INCREMENT_DECREMENT:
          {
            bool IncrementState = digitalRead(pinIncrement);
            bool DecrementState = digitalRead(pinDecrement);

            int8_t stateChangeIncrement = IncrementState - prevIncrementState;
            int8_t stateChangeDecrement = DecrementState - prevDecrementState;

            if (stateChangeIncrement == falling) { // Increment button is pushed
              if (millis() - prevBounceTime > debounceTime) {
                newChannel = channel == nb_banks ? 1 : channel + 1; // Increment channel number or wrap around
                prevBounceTime = millis();
              }
            }
            if (stateChangeDecrement == falling) { // Decrement button is pushed
              if (millis() - prevBounceTime > debounceTime) {
                newChannel = channel == 1 ? nb_banks : channel - 1; // Decrement channel number or wrap around
                prevBounceTime = millis();
              }
            }
            if (stateChangeIncrement == rising || stateChangeDecrement == rising) { // One of the buttons is released
              prevBounceTime = millis();
            }
            prevIncrementState = IncrementState;
            prevDecrementState = DecrementState;
          }
          break;
      }
      if (newChannel != channel) {
        Serial.print("New channel:\t"); Serial.println(newChannel);
        refreshLEDs(newChannel);
        channel = newChannel;
        bank.setChannel(channel);
      }
      bank.refresh();
    }

    uint8_t getChannel() {
      return channel;
    }
    void setChannel(uint8_t newChannel) {
      refreshLEDs(newChannel);
      this->channel = newChannel;
    }

    const char* getMode() {
      if (mode == SINGLE_BUTTON)
        return "SINGLE_BUTTON";
      if (mode == SINGLE_BUTTON_LED)
        return "SINGLE_BUTTON_LED";
      if (mode == SINGLE_SWITCH)
        return "SINGLE_SWITCH";
      if (mode == SINGLE_SWITCH_LED)
        return "SINGLE_SWITCH_LED";
      if (mode == MULTIPLE_BUTTONS)
        return "MULTIPLE_BUTTONS";
      if (mode == MULTIPLE_BUTTONS_LEDS)
        return "MULTIPLE_BUTTONS_LEDS";
      if (mode == INCREMENT_DECREMENT)
        return "INCREMENT_DECREMENT";
      if (mode == INCREMENT_DECREMENT_LEDS)
        return "INCREMENT_DECREMENT_LEDS";
      return "";
    }

    void setPinModeExt(void (*fn)(uint8_t, uint8_t)) {
      pinModeExt = fn;
    }
    void setDigitalWriteExt(void (*fn)(uint8_t, uint8_t)) {
      digitalWriteExt = fn;
    }
    void setDigitalReadExt(int (*fn)(uint8_t)) {
      digitalReadExt = fn;
    }

  private:
    Bank& bank;
    uint8_t channel = 1;
    uint8_t switchPin, ledPin, pinIncrement, pinDecrement;
    const uint8_t* switchPins, *ledPins;
    uint8_t nb_banks;

    unsigned long prevBounceTime = 0;
    bool prevIncrementState = HIGH;
    bool prevDecrementState = HIGH;

    bool prevToggleState = HIGH;

    const unsigned long debounceTime = 25;

    const int8_t falling = LOW - HIGH;
    const int8_t rising = HIGH - LOW;

    void (*pinModeExt)(uint8_t, uint8_t) = nullptr;
    void (*digitalWriteExt)(uint8_t, uint8_t) = nullptr;
    int (*digitalReadExt)(uint8_t) = nullptr;

    enum BankSelectorMode {
      SINGLE_BUTTON, SINGLE_BUTTON_LED, SINGLE_SWITCH, SINGLE_SWITCH_LED, MULTIPLE_BUTTONS, MULTIPLE_BUTTONS_LEDS, INCREMENT_DECREMENT, INCREMENT_DECREMENT_LEDS
    } mode;

    void refreshLEDs(uint8_t newChannel) {
      switch (mode) {
        case SINGLE_SWITCH_LED:
        case SINGLE_BUTTON_LED:
          {
            digitalWrite(ledPin, newChannel - 1);
          }
          break;

        case MULTIPLE_BUTTONS_LEDS:
        case INCREMENT_DECREMENT_LEDS:
          {
            digitalWrite(ledPins[channel - 1], LOW);
            digitalWrite(ledPins[newChannel - 1], HIGH);
          }
          break;
      }
    }

    void pinMode(uint8_t pin, uint8_t mode) {
      if (pin < NUM_DIGITAL_PINS)
        ::pinMode(pin, mode);
      else if (pinModeExt != nullptr)
        pinModeExt(pin - NUM_DIGITAL_PINS, mode);
    }
    void digitalWrite(uint8_t pin, uint8_t val) {
      if (pin < NUM_DIGITAL_PINS)
        ::digitalWrite(pin, val);
      else if (digitalWriteExt != nullptr)
        digitalWriteExt(pin - NUM_DIGITAL_PINS, val);
    }
    int digitalRead(uint8_t pin) {
      if (pin < NUM_DIGITAL_PINS)
        return ::digitalRead(pin);
      if (digitalReadExt != nullptr)
        return digitalReadExt(pin - NUM_DIGITAL_PINS);
      return 0;
    }
};
