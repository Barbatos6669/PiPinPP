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

#include "pin.hpp"
#include "log.hpp"
#include <stdexcept>
#include <gpiod.h>
#include <iostream>

Pin::Pin(int pin, PinDirection direction, const std::string& chipname) 
: pinNumber(pin) 
{
    validatePinNumber(pin);
    
    chip = gpiod_chip_open_by_name(chipname.c_str()); 

    // Error handling for chip opening
    if (!chip) 
    {
        throw std::runtime_error("Failed to open GPIO chip: " + chipname);
    }

    // Get the GPIO line
    line = gpiod_chip_get_line(chip, pinNumber);
    if (!line)
    {
        gpiod_chip_close(chip);
        throw std::runtime_error("Failed to get GPIO line: " + std::to_string(pinNumber));
    }

    int requestResult;

    // Request the line as input or output
    if (direction == PinDirection::OUTPUT) 
    {
        requestResult = gpiod_line_request_output(line, "PiPinPP", 0);
    } 
    else // INPUT
    {
        requestResult = gpiod_line_request_input(line, "PiPinPP");
    }

    // Error handling for line request
    if (requestResult < 0) 
    {
        gpiod_chip_close(chip); // Clean up chip on failure
        throw std::runtime_error("Failed to request GPIO line: " + std::to_string(pinNumber));
    }

    // Initialize GPIO pin here
    PIPINPP_LOG_INFO("Initializing pin " << pinNumber << " as " 
                     << ((direction == PinDirection::OUTPUT) ? "OUTPUT" : "INPUT")
                     << " on chip " << chipname);
}

Pin::Pin(int pin, PinMode mode, const std::string& chipname) 
: pinNumber(pin) 
{
    validatePinNumber(pin);
    
    chip = gpiod_chip_open_by_name(chipname.c_str()); 

    // Error handling for chip opening
    if (!chip) 
    {
        throw std::runtime_error("Failed to open GPIO chip: " + chipname);
    }

    // Get the GPIO line
    line = gpiod_chip_get_line(chip, pinNumber);
    if (!line)
    {
        gpiod_chip_close(chip);
        throw std::runtime_error("Failed to get GPIO line: " + std::to_string(pinNumber));
    }

    int requestResult;

    // Request the line based on mode
    if (mode == PinMode::OUTPUT) 
    {
        requestResult = gpiod_line_request_output(line, "PiPinPP", 0);
    } 
    else if (mode == PinMode::INPUT_PULLUP)
    {
        requestResult = gpiod_line_request_input_flags(line, "PiPinPP", GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP);
    }
    else if (mode == PinMode::INPUT_PULLDOWN)
    {
        requestResult = gpiod_line_request_input_flags(line, "PiPinPP", GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_DOWN);
    }
    else // INPUT (no pull resistor)
    {
        requestResult = gpiod_line_request_input(line, "PiPinPP");
    }

    // Error handling for line request
    if (requestResult < 0) 
    {
        gpiod_chip_close(chip); // Clean up chip on failure
        throw std::runtime_error("Failed to request GPIO line: " + std::to_string(pinNumber));
    }

    // Log initialization
    #ifdef PIPINPP_ENABLE_LOGGING
    const char* modeStr;
    switch (mode) {
        case PinMode::OUTPUT: modeStr = "OUTPUT"; break;
        case PinMode::INPUT_PULLUP: modeStr = "INPUT_PULLUP"; break;
        case PinMode::INPUT_PULLDOWN: modeStr = "INPUT_PULLDOWN"; break;
        default: modeStr = "INPUT"; break;
    }
    PIPINPP_LOG_INFO("Initializing pin " << pinNumber << " as " << modeStr 
                     << " on chip " << chipname);
    #endif
}

Pin::~Pin() 
{
    // Release the line
    if (line)
    {
        gpiod_line_release(line);
    }

    // Close the chip
    if (chip)
    {
        gpiod_chip_close(chip);
    }

    PIPINPP_LOG_DEBUG("Releasing resources for pin " << pinNumber);
}

bool Pin::write(bool value) 
{
    if (!line) 
    {
        return false; // Line not initialized
    }

    return gpiod_line_set_value(line, value ? 1 : 0) == 0;
}

int Pin::read() 
{
    if (!line) 
    {
        return -1; // Line not initialized
    }

    return gpiod_line_get_value(line);
}

void Pin::validatePinNumber(int pin) 
{
    // Raspberry Pi GPIO pins: 0-27 are generally valid
    // Some pins have special functions, but we'll allow them for flexibility
    if (pin < 0 || pin > 27) {
        throw std::invalid_argument("Invalid GPIO pin number: " + std::to_string(pin) + 
                                   ". Valid range is 0-27 for Raspberry Pi.");
    }
    
    // Warn about commonly reserved pins (but don't block them)
    if (pin == 0 || pin == 1) {
        std::cerr << "Warning: GPIO pin " << pin << " is typically reserved for I2C (ID_SD/ID_SC)" << std::endl;
    } else if (pin == 14 || pin == 15) {
        std::cerr << "Warning: GPIO pin " << pin << " is typically used for UART (TXD/RXD)" << std::endl;
    }
}