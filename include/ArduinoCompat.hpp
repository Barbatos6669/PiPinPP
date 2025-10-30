#pragma once
#include "pin.hpp"

// Arduino-style constants
#define HIGH true
#define LOW false

// Arduino pin modes
enum ArduinoPinMode {
    INPUT = 0,
    OUTPUT = 1
};

// Arduino-style global functions for familiar API
/**
 * @brief Set pin mode (Arduino-style function)
 * 
 * @param pin GPIO pin number
 * @param mode INPUT (0) or OUTPUT (1)
 */
void pinMode(int pin, int mode);

/**
 * @brief Write digital value to pin (Arduino-style function)
 * 
 * @param pin GPIO pin number
 * @param value HIGH (true) or LOW (false)
 */
void digitalWrite(int pin, bool value);

/**
 * @brief Read digital value from pin (Arduino-style function)
 * 
 * @param pin GPIO pin number
 * @return int 1 for HIGH, 0 for LOW, -1 for error
 */
int digitalRead(int pin);