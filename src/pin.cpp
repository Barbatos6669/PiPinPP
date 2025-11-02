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
: chip(nullptr), request(nullptr), currentDirection(direction), pinNumber(pin)
{
    validatePinNumber(pin);
    
    // Open GPIO chip (v2 API)
    chip = gpiod_chip_open(("/dev/" + chipname).c_str()); 

    // Error handling for chip opening
    if (!chip) 
    {
        throw std::runtime_error("Failed to open GPIO chip: " + chipname);
    }

    // Configure line settings (v2 API)
    gpiod_line_settings* settings = gpiod_line_settings_new();
    if (!settings)
    {
        gpiod_chip_close(chip);
        throw std::runtime_error("Failed to create line settings");
    }

    // Set direction
    if (direction == PinDirection::OUTPUT) 
    {
        gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
        gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE);
    } 
    else // INPUT
    {
        gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
    }

    // Create line config
    gpiod_line_config* line_cfg = gpiod_line_config_new();
    if (!line_cfg)
    {
        gpiod_line_settings_free(settings);
        gpiod_chip_close(chip);
        throw std::runtime_error("Failed to create line config");
    }

    gpiod_line_config_add_line_settings(line_cfg, &pinNumber, 1, settings);

    // Create request config
    gpiod_request_config* req_cfg = gpiod_request_config_new();
    if (!req_cfg)
    {
        gpiod_line_config_free(line_cfg);
        gpiod_line_settings_free(settings);
        gpiod_chip_close(chip);
        throw std::runtime_error("Failed to create request config");
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
        throw std::runtime_error("Failed to request GPIO line: " + std::to_string(pinNumber));
    }

    // Initialize GPIO pin here
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
    
    // Open GPIO chip (v2 API)
    chip = gpiod_chip_open(("/dev/" + chipname).c_str()); 

    // Error handling for chip opening
    if (!chip) 
    {
        throw std::runtime_error("Failed to open GPIO chip: " + chipname);
    }

    // Configure line settings (v2 API)
    gpiod_line_settings* settings = gpiod_line_settings_new();
    if (!settings)
    {
        gpiod_chip_close(chip);
        throw std::runtime_error("Failed to create line settings");
    }

    // Set direction and bias based on mode
    if (mode == PinMode::OUTPUT) 
    {
        gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
        gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE);
    } 
    else 
    {
        gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
        
        // Set pull resistor if specified
        if (mode == PinMode::INPUT_PULLUP)
        {
            gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_UP);
        }
        else if (mode == PinMode::INPUT_PULLDOWN)
        {
            gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_DOWN);
        }
        // INPUT mode uses default bias (disabled)
    }

    // Create line config
    gpiod_line_config* line_cfg = gpiod_line_config_new();
    if (!line_cfg)
    {
        gpiod_line_settings_free(settings);
        gpiod_chip_close(chip);
        throw std::runtime_error("Failed to create line config");
    }

    gpiod_line_config_add_line_settings(line_cfg, &pinNumber, 1, settings);

    // Create request config
    gpiod_request_config* req_cfg = gpiod_request_config_new();
    if (!req_cfg)
    {
        gpiod_line_config_free(line_cfg);
        gpiod_line_settings_free(settings);
        gpiod_chip_close(chip);
        throw std::runtime_error("Failed to create request config");
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

    PIPINPP_LOG_DEBUG("Releasing resources for pin " << pinNumber);
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