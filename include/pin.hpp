#pragma once

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
     * @param number The GPIO pin number to control
     */
    Pin(int number);
    
    /**
     * @brief Set the pin to HIGH (3.3V)
     * 
     * This method sets the GPIO pin to a high voltage state (3.3V).
     * The pin must be configured as an output for this to have effect.
     */
    void setHigh();
    
    /**
     * @brief Set the pin to LOW (0V)
     * 
     * This method sets the GPIO pin to a low voltage state (0V/ground).
     * The pin must be configured as an output for this to have effect.
     */
    void setLow();
    
    /**
     * @brief Read the current state of the pin
     * 
     * @return int Returns 1 if the pin is HIGH, 0 if the pin is LOW
     */
    int read();
    
private:
    int pinNumber; ///< The GPIO pin number being controlled
};