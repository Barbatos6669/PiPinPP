/**
 * @file pin.cpp
 * @brief Implementation of object-oriented GPIO pin control
 * @author Barbatos6669
 * @date 2025
 * 
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
#include "exceptions.hpp"
#include <stdexcept>
#include <gpiod.h>
#include <iostream>

Pin::Pin(int pin, PinDirection direction, const std::string& chipname) 
: chip(nullptr), request(nullptr), currentDirection(direction), pinNumber(pin)
{
    validatePinNumber(pin);
    
    gpiod_line_direction gpio_dir = (direction == PinDirection::OUTPUT) 
                                     ? GPIOD_LINE_DIRECTION_OUTPUT 
                                     : GPIOD_LINE_DIRECTION_INPUT;
    
    initializeGpio(chipname, gpio_dir, GPIOD_LINE_BIAS_AS_IS);
    
    PIPINPP_LOG_INFO("Initializing pin " << pinNumber << " as " 
                     << ((direction == PinDirection::OUTPUT) ? "OUTPUT" : "INPUT")
                     << " on chip " << chipname);
}

Pin::Pin(int pin, PinMode mode, const std::string& chipname) 
: chip(nullptr), request(nullptr), 
  currentDirection(mode == PinMode::OUTPUT ? PinDirection::OUTPUT : PinDirection::INPUT), 
  pinNumber(pin)
{
    validatePinNumber(pin);
    
    // Determine GPIO direction and bias from mode
    gpiod_line_direction gpio_dir;
    gpiod_line_bias gpio_bias;
    
    if (mode == PinMode::OUTPUT) 
    {
        gpio_dir = GPIOD_LINE_DIRECTION_OUTPUT;
        gpio_bias = GPIOD_LINE_BIAS_AS_IS;
    } 
    else 
    {
        gpio_dir = GPIOD_LINE_DIRECTION_INPUT;
        
        // Set pull resistor if specified
        if (mode == PinMode::INPUT_PULLUP)
        {
            gpio_bias = GPIOD_LINE_BIAS_PULL_UP;
        }
        else if (mode == PinMode::INPUT_PULLDOWN)
        {
            gpio_bias = GPIOD_LINE_BIAS_PULL_DOWN;
        }
        else // INPUT mode uses default bias (disabled)
        {
            gpio_bias = GPIOD_LINE_BIAS_AS_IS;
        }
    }
    
    initializeGpio(chipname, gpio_dir, gpio_bias);
    
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

void Pin::initializeGpio(const std::string& chipname, 
                         gpiod_line_direction direction,
                         gpiod_line_bias bias,
                         gpiod_line_value initial_value)
{
    // Open GPIO chip
    chip = gpiod_chip_open(("/dev/" + chipname).c_str());
    if (!chip) {
        throw GpioAccessError("/dev/" + chipname, "Failed to open GPIO chip. Check permissions and device existence.");
    }

    // Validate pin number against hardware (v2 API)
    gpiod_chip_info* info = gpiod_chip_get_info(chip);
    if (info) {
        size_t num_lines = gpiod_chip_info_get_num_lines(info);
        if (static_cast<size_t>(pinNumber) >= num_lines) {
            gpiod_chip_info_free(info);
            gpiod_chip_close(chip);
            throw InvalidPinError(pinNumber, "Pin number exceeds available GPIO lines (" + std::to_string(num_lines) + ")");
        }
        gpiod_chip_info_free(info);
    }

    // Configure line settings (v2 API)
    gpiod_line_settings* settings = gpiod_line_settings_new();
    if (!settings)
    {
        gpiod_chip_close(chip);
        throw GpioAccessError("GPIO", "Failed to create line settings");
    }

    // Set direction
    gpiod_line_settings_set_direction(settings, direction);
    
    // Set initial value for output pins
    if (direction == GPIOD_LINE_DIRECTION_OUTPUT) 
    {
        gpiod_line_settings_set_output_value(settings, initial_value);
    }
    
    // Set bias (pull resistors)
    if (bias != GPIOD_LINE_BIAS_AS_IS)
    {
        gpiod_line_settings_set_bias(settings, bias);
    }

    // Create line config
    gpiod_line_config* line_cfg = gpiod_line_config_new();
    if (!line_cfg)
    {
        gpiod_line_settings_free(settings);
        gpiod_chip_close(chip);
        throw GpioAccessError("GPIO", "Failed to create line config");
    }

    gpiod_line_config_add_line_settings(line_cfg, &pinNumber, 1, settings);

    // Create request config
    gpiod_request_config* req_cfg = gpiod_request_config_new();
    if (!req_cfg)
    {
        gpiod_line_config_free(line_cfg);
        gpiod_line_settings_free(settings);
        gpiod_chip_close(chip);
        throw GpioAccessError("GPIO", "Failed to create request config");
    }
    gpiod_request_config_set_consumer(req_cfg, "PiPinPP");

    // Request the line
    request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

    // Clean up temporary objects
    gpiod_request_config_free(req_cfg);
    gpiod_line_config_free(line_cfg);
    gpiod_line_settings_free(settings);

    // Error handling for line request
    if (!request) 
    {
        gpiod_chip_close(chip);
        throw GpioAccessError("GPIO pin " + std::to_string(pinNumber), 
                            "Failed to request GPIO line. Pin may be in use or unavailable.");
    }
}

Pin::~Pin() 
{
    // Release the line request (v2 API)
    if (request)
    {
        gpiod_line_request_release(request);
    }

    // Close the chip
    if (chip)
    {
        gpiod_chip_close(chip);
    }
}

bool Pin::write(bool value) 
{
    if (!request) 
    {
        return false; // Request not initialized
    }

    // v2 API: set value using line request
    enum gpiod_line_value val = value ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE;
    return gpiod_line_request_set_value(request, pinNumber, val) == 0;
}

int Pin::read() 
{
    if (!request) 
    {
        return -1; // Request not initialized
    }

    // v2 API: get value using line request
    enum gpiod_line_value val = gpiod_line_request_get_value(request, pinNumber);
    if (val == GPIOD_LINE_VALUE_ERROR)
    {
        return -1;
    }
    return (val == GPIOD_LINE_VALUE_ACTIVE) ? 1 : 0;
}

void Pin::validatePinNumber(int pin) 
{
    // Raspberry Pi GPIO pins: 0-27 are generally valid
    // Some pins have special functions, but we'll allow them for flexibility
    if (pin < 0 || pin > 27) {
        throw InvalidPinError(pin, "Valid range is 0-27 for Raspberry Pi");
    }
    
    // Warn about commonly reserved pins (but don't block them)
    if (pin == 0 || pin == 1) {
        PIPINPP_LOG_WARNING("GPIO pin " << pin << " is typically reserved for I2C (ID_SD/ID_SC). Use with caution.");
    } else if (pin == 14 || pin == 15) {
        PIPINPP_LOG_WARNING("GPIO pin " << pin << " is typically used for UART (TXD/RXD). Use with caution.");
    }
}