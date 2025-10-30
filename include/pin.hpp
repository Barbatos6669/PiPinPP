/**
 * @file Pin.hpp
 * @brief Object-oriented GPIO pin control using libgpiod on Raspberry Pi
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
 * @version 0.1.0
 * @date    2025-10-30
 */

#pragma once

#include <gpiod.h>
#include <string>

enum class PinDirection { INPUT, OUTPUT};

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
     * @brief Construct a new Pin object
     * 
     * @param pin The GPIO pin number to control
     * @param direction The direction of the pin (INPUT or OUTPUT)
     * @param chipname The name of the GPIO chip (default: "gpiochip0")
     */
    Pin(int pin, PinDirection direction, const std::string& chipname = "gpiochip0");

    /**
     * @brief Destroy the Pin object
     * 
     * This destructor releases any resources held by the Pin object.
     */
    ~Pin();
    
    /**
     * @brief Write a value to the pin
     * 
     * @param value true to set the pin HIGH, false to set it LOW
     * @return bool Returns true on success, false on failure
     */
    bool write(bool value);
    
    /**
     * @brief Read the current state of the pin
     * 
     * @return int Returns 1 if the pin is HIGH, 0 if the pin is LOW
     */
    int read();
    
private:
    gpiod_chip* chip; ///< The GPIO chip being used
    gpiod_line* line; ///< The GPIO line being controlled

    int pinNumber; ///< The GPIO pin number being controlled 
};