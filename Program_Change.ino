#include <MIDI_controller.h>

#define DEBUG  // comment this out to use actual MIDI messages instead of text messages in the Serial Monitor

const uint8_t presetButtons[] = { 2, 3, 5, 7 };  // the pins with the push buttons (change this)
const size_t nbButtons = sizeof(presetButtons);
const uint8_t presets[nbButtons] = { 1, 2, 3, 4 };  // the presets that correspond to the buttons (change this)
uint8_t previousStates[nbButtons];

const uint8_t channel = 1;

void setup() {
  USBMidiController.blink(LED_BUILTIN);  // flash the built-in LED (pin 13 on most boards) on every message
  USBMidiController.setDelay(5);  // wait 5 ms after each message not to flood the connection
#ifdef DEBUG
  USBMidiController.begin(115200, true);  // Start Serial debug output @115200 baud
#else
  USBMidiController.begin();  // Initialise the USB MIDI connection
#endif

  memset(previousStates, 2, nbButtons);  // set all previous states to 2 (i.e. a number different from 0 (pressed) or 1 (released))

  for (uint8_t i = 0; i < nbButtons; i++) {  // enable the internal pull-up resistors for all buttons
    pinMode(presetButtons[i], INPUT_PULLUP);
  }
}

void loop() {
  for (uint8_t i = 0; i < nbButtons; i++) {  // loop over all buttons
    bool currentButtonState = digitalRead(presetButtons[i]);
    if (currentButtonState != previousStates[i] && currentButtonState == LOW) {
      // if the state of the button has changed form "released" to "pressed"
      USBMidiController.send(PROGRAM_CHANGE, channel, presets[i]);  // select preset for this button
    }
    previousStates[i] = currentButtonState;
  }
}
