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

#ifndef PIPINPP_EXCEPTIONS_HPP
#define PIPINPP_EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

/**
 * @brief Base exception class for all Pin-related errors
 */
class PinError : public std::runtime_error {
public:
    explicit PinError(const std::string& message) 
        : std::runtime_error(message) {}
};

/**
 * @brief Exception thrown when an invalid pin number is used
 * 
 * This is thrown when trying to access a pin number that doesn't exist
 * on the GPIO chip (e.g., pin 99 on a chip with only 58 pins).
 */
class InvalidPinError : public PinError {
public:
    explicit InvalidPinError(const std::string& message) 
        : PinError(message) {}
    
    InvalidPinError(int pin, const std::string& reason)
        : PinError("Invalid pin " + std::to_string(pin) + ": " + reason) {}
};

/**
 * @brief Exception thrown when GPIO access fails
 * 
 * This is thrown when the library cannot access GPIO hardware, usually due to:
 * - Missing permissions (user not in 'gpio' group)
 * - GPIO chip device not found (/dev/gpiochip* missing)
 * - Hardware-level errors
 */
class GpioAccessError : public PinError {
public:
    explicit GpioAccessError(const std::string& message) 
        : PinError(message) {}
    
    GpioAccessError(const std::string& device, const std::string& reason)
        : PinError("Cannot access " + device + ": " + reason) {}
};

#endif // PIPINPP_EXCEPTIONS_HPP