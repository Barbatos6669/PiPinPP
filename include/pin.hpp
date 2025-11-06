/*
 * Copyright (c) 2025 HobbyHacker
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file pin.hpp
 * @brief Object-oriented GPIO pin control using libgpiod on Raspberry Pi
 * @author Barbatos6669
 * @date 2025
 * 
 * @details
 * The Pin class provides a clean, Arduino-style C++ interface for interacting with
 * GPIO pins via the libgpiod library. It abstracts low-level chip and line operations
 * into a simple and reusable class that supports both input and output modes.
 *
 * Features:
 * - Simple constructor for selecting pin and direction
 * - Safe, RAII-based GPIO line management
 * - High-level methods to write to or read from pins
 * - Configurable chip name (default: "gpiochip0")
 *
 * Example usage:
 * @code
 * #include "pin.hpp"
 * 
 * // Object-oriented style
 * Pin led(17, PinDirection::OUTPUT);
 * led.write(true);   // Set GPIO17 HIGH
 * led.write(false);  // Set GPIO17 LOW
 * 
 * Pin button(18, PinDirection::INPUT);
 * int state = button.read();
 * @endcode
 * 
 * For Arduino-style compatibility, see ArduinoCompat.hpp
 * 
 * @author  
 * HobbyHacker / Barbatos6669  
 * @version 0.2.0
 * @date    2025-10-31
 */

#pragma once

#include <gpiod.h>
#include <string>

enum class PinDirection { INPUT, OUTPUT };

/**
 * @brief Pin mode configuration including pull resistor settings
 */
enum class PinMode { 
    INPUT,              ///< Input without pull resistor
    OUTPUT,             ///< Output mode
    INPUT_PULLUP,       ///< Input with internal pull-up resistor
    INPUT_PULLDOWN      ///< Input with internal pull-down resistor
};

/**
 * @brief A class for controlling GPIO pins on Raspberry Pi
 * 
 * This class provides a simple interface for controlling GPIO pins,
 * allowing you to set pins high/low and read their current state.
 */
class Pin
{
public:
    /**
     * @brief Construct a new Pin object with direction
     * 
     * @param pin The GPIO pin number to control (0-27 for Raspberry Pi)
     * @param direction The direction of the pin (INPUT or OUTPUT)
     * @param chipname The name of the GPIO chip (default: "gpiochip0")
     * @throws std::invalid_argument if pin number is invalid (outside 0-27 range)
     * @throws std::runtime_error if GPIO chip cannot be opened or line cannot be requested
     * 
     * @note This constructor does not configure pull resistors. Use PinMode constructor for pull resistors.
     * @warning Some pins (0,1,14,15) have special functions and may conflict with other uses.
     */
    Pin(int pin, PinDirection direction, const std::string& chipname = "gpiochip0");

    /**
     * @brief Construct a new Pin object with mode (including pull resistors)
     * 
     * @param pin The GPIO pin number to control (0-27 for Raspberry Pi)
     * @param mode The pin mode including pull resistor configuration
     * @param chipname The name of the GPIO chip (default: "gpiochip0")
     * @throws std::invalid_argument if pin number is invalid (outside 0-27 range)  
     * @throws std::runtime_error if GPIO chip cannot be opened or line cannot be requested
     * 
     * @note Pull resistor support requires libgpiod 1.4+ and compatible hardware
     * @warning Some pins (0,1,14,15) have special functions and may conflict with other uses.
     */
    Pin(int pin, PinMode mode, const std::string& chipname = "gpiochip0");

    /**
     * @brief Destroy the Pin object
     * 
     * This destructor releases any resources held by the Pin object.
     * Automatically releases the GPIO line and closes the chip handle.
     * Safe to call multiple times.
     */
    ~Pin();
    
    /**
     * @brief Write a value to the pin
     * 
     * Sets the output state of a GPIO pin configured as OUTPUT.
     * 
     * @param value true to set the pin HIGH (3.3V), false to set it LOW (0V)
     * @return bool Returns true on success, false on failure
     * 
     * @note Only works on pins configured as OUTPUT. Input pins will return false.
     * @warning Ensure external circuitry can handle 3.3V logic levels.
     * 
     * @code
     * Pin led(17, PinDirection::OUTPUT);
     * led.write(true);  // Turn LED on
     * led.write(false); // Turn LED off
     * @endcode
     */
    bool write(bool value);
    
    /**
     * @brief Read the current state of the pin
     * 
     * Reads the digital state of a GPIO pin configured as INPUT.
     * 
     * @return int Returns 1 if the pin is HIGH (3.3V), 0 if the pin is LOW (0V), -1 on error
     * 
     * @note Works on pins configured as INPUT, INPUT_PULLUP, or INPUT_PULLDOWN.
     * @note For floating inputs without pull resistors, readings may be unstable.
     * 
     * @code
     * Pin button(18, PinMode::INPUT_PULLUP);
     * int state = button.read();
     * if (state == 0) {
     *     // Button is pressed (connected to GND)
     * }
     * @endcode
     */
    int read();
    
private:
    gpiod_chip* chip; ///< The GPIO chip being used
    gpiod_line_request* request; ///< The GPIO line request (v2 API)
    PinDirection currentDirection; ///< Current pin direction

    unsigned int pinNumber; ///< The GPIO pin number being controlled 
    
    /**
     * @brief Validate that the pin number is valid for Raspberry Pi
     * @param pin The GPIO pin number to validate (must be 0-27)
     * @throws std::invalid_argument if pin number is invalid
     * 
     * @note Warns about commonly reserved pins but allows their use for flexibility
     * @warning Pins 0,1 are typically I2C, pins 14,15 are typically UART
     */
    static void validatePinNumber(int pin);
};