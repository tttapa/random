#include <MemoryFree.h>

#include <MIDI_controller.h> // include the library

#include "BankSelector.h"

const static byte Channel_Volume = 0x7; // controller number 7 is defined as Channel Volume in the MIDI implementation.
const static size_t analogAverage = 8; // Use the average of 8 samples to get smooth transitions and prevent noise

// const uint8_t switches[] = {2, 3, 5, 7};
const uint8_t switches[] = {0, 1, 2, 3};
// const uint8_t switches[] = {EXT_PIN(0), EXT_PIN(1), EXT_PIN(2), EXT_PIN(3)};
// const uint8_t leds[] = {16, 18, 20, 22};
const uint8_t leds[] = {EXT_PIN(16), EXT_PIN(18), EXT_PIN(20), EXT_PIN(22)};

Bank b;
// BankSelector bs(b, switches[0]);                                        // SINGLE_BUTTON
// BankSelector bs(b, switches[0], BankSelector::MOMENTARY);               // SINGLE_BUTTON
// BankSelector bs(b, switches[0], BankSelector::TOGGLE);                  // SINGLE_SWITCH
// BankSelector bs(b, switches[0], LED_BUILTIN);                           // SINGLE_BUTTON_LED
// BankSelector bs(b, switches[0], LED_BUILTIN, BankSelector::MOMENTARY);  // SINGLE_BUTTON_LED
// BankSelector bs(b, switches[0], LED_BUILTIN, BankSelector::TOGGLE);     // SINGLE_SWITCH_LED
// BankSelector bs(b, switches);                                           // MULTIPLE_BUTTONS
// BankSelector bs(b, switches, leds);                                     // MULTIPLE_BUTTONS_LEDS
// BankSelector bs(b, 0, 1, 4);                                            // INCREMENT_DECREMENT
// BankSelector bs(b, 0, 1, leds);                                         // INCREMENT_DECREMENT_LEDS
BankSelector bs(b, EXT_PIN(0), EXT_PIN(1), leds);                                         // INCREMENT_DECREMENT_LEDS


void setup() {
  USBMidiController.setDelay(15);  // wait 15 ms after each message not to flood the connection
  USBMidiController.begin(115200, 1);  // Initialise the USB MIDI connection
  while (!Serial);
  delay(1000);

  bs.setDigitalWriteExt(digW);
  bs.setDigitalReadExt(digR);
  bs.setPinModeExt(pinM);
  bs.init();

  b.add(new Analog(A0, Channel_Volume, 1));
  b.add(new Analog(A1, Channel_Volume + 1, 1));
  b.average(8);
#ifdef __AVR__
  Serial.print("freeMemory()=");
  Serial.println(freeMemory());
#endif
  Serial.println(bs.getMode());
  Serial.println(NUM_DIGITAL_PINS);
}

//________________________________________________________________________________________________________________________________

void loop() {
  // for (int i = 0; i < 10; i++)
  bs.refresh();
  // while (1);
}

uint8_t outputs[3] = {};

template <typename T, size_t N> void printBin(T (&arr)[N]) {
  for (int i = N - 1; i >= 0; i--) {
    for (int j = 7; j >= 0; j--) {
      char bit = ((arr[i] >> j) & 1) + '0';
      Serial.write(bit);
    }
    Serial.print(' ');
  }
  Serial.println();
}

uint8_t pinToIndex(uint8_t pin) {
  return pin / 8;
}
uint8_t pinToBit(uint8_t pin) {
  return 1 << (pin % 8);
}

void digW(uint8_t pin, uint8_t val) {
  Serial.print("digitalWrite(");
  Serial.print(pin);
  Serial.print(", ");
  Serial.print(val);
  Serial.print(");\r\n");
  if (val)
    outputs[pinToIndex(pin)] |= pinToBit(pin);
  else
    outputs[pinToIndex(pin)] &= ~pinToBit(pin);
  printBin(outputs);
  // Serial.println(outputs[2], HEX);
  // Serial.println(pinToIndex(pin));
  // Serial.println(pinToBit(pin));
  digitalWrite(pin, val);
}

void pinM(uint8_t pin, uint8_t mode) {
  Serial.print("pinMode(");
  Serial.print(pin);
  Serial.print(", ");
  Serial.print(mode);
  Serial.print(");\r\n");
  pinMode(pin, mode);
}

int digR(uint8_t pin) {
  return digitalRead(pin);
}

template <typename T> void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, T val) {
  if (bitOrder == LSBFIRST) {
    for (int i = 0; i < sizeof(val); i++) {
      shiftOut(dataPin, clockPin, LSBFIRST, (uint8_t) (val >> (i * 8)));
    }
  } else {
    for (int i = sizeof(val) - 1; i >= 0; i--) {
      shiftOut(dataPin, clockPin, MSBFIRST, (uint8_t) (val >> (i * 8)));
    }
  }
}



/*
  template <typename T> void printBin(T val) {
  for (int i = sizeof(val) - 1; i >= 0; i--) {
    for (int j = 7; j >= 0; j--) {
      char bit = ((((uint8_t*)&val)[i] >> j) & 1) + '0';
      Serial.write(bit);
    }
    Serial.print(' ');
  }
  Serial.println();
  }*/

