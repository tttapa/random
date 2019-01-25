#pragma once

#include <Arduino.h>

using pin_t = uint8_t;

/**
 * @brief   A class for reading and debouncing buttons and switches.
 */
class Button {
  public:
    /**
     * @brief   Construct a new Button object.
     *
     * @param   pin
     *          The digital pin to read from. The internal pull-up resistor
     *          will be enabled when `begin` is called.
     */
    Button(pin_t pin);

    void begin();

    /**
     * @brief Invert the state of all buttons (button pressed is HIGH instead of
     * LOW).
     *
     * @note This affects **all** Button objects.
     */
    static void invert();

    /**
     * @brief   An enumeration of the different states a button can be in.
     */
    enum State {
        Pressed = 0b00,  /** Input went from low to low   (0,0) */
        Released = 0b11, /** Input went from high to high (1,1) */
        Falling = 0b10,  /** Input went from high to low  (1,0) */
        Rising = 0b01    /** Input went from low to high  (0,1) */
    };

    /**
     * @brief Get the state of the button.
     *
     * The button is debounced, the debounce time can be set in
     * Settings.h: #BUTTON_DEBOUNCE_TIME.
     *
     * ```
     * Debounce time: ├────┤
     *
     * Raw input:
     *    HIGH  ──────┐      ┌──────┐ ┌─┐      ┌─┐ ┌──────┐ ┌────────
     *    LOW         └──────┘      └─┘ └──────┘ └─┘      └─┘
     *                ├────┤ ├────┤ ├─┼─┼────┤ ├─┼─┼────┤ ├─┼────┤
     *
     * Debounced output:
     *    HIGH  ──────┐      ┌──────┐          ┌──────────┐      ┌───
     *    LOW         └──────┘      └──────────┘          └──────┘
     *
     * States:
     *    HIGH  ────────────────┐                  ┌─────────────────
     *    LOW                   └──────────────────┘
     *            RELEASED   FALLING   PRESSED   RISING
     * ```
     *
     * @return  The state of the button, either Button::PRESSED,
     *          Button::RELEASED, Button::FALLING or Button::RISING.
     */
    State getState();

    /** 
     * @brief   Return the time in milliseconds that the button has been stable 
     *          for.
     */
    unsigned long stableTime();

    const pin_t pin;

  private:
    bool prevState = HIGH;
    bool debouncedState = HIGH;
    unsigned long prevBounceTime = 0;

    static bool invertState;

    constexpr static unsigned long debounceTime = 30; // milliseconds
};
