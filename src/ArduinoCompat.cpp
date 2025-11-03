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

#include "ArduinoCompat.hpp"
#include "exceptions.hpp"
#include "log.hpp"
#include <unordered_map>
#include <memory>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

// Global storage for Arduino-style pins
static std::unordered_map<int, std::unique_ptr<Pin>> globalPins;
// Mutex to protect globalPins from concurrent access
static std::mutex globalPinsMutex;

void pinMode(int pin, int mode) 
{
    std::lock_guard<std::mutex> lock(globalPinsMutex);
    // Remove existing pin if it exists
    globalPins.erase(pin);
    
    // Create new pin with specified mode
    // Exceptions will propagate naturally (InvalidPinError, GpioAccessError)
    if (mode == OUTPUT) {
        globalPins[pin] = std::make_unique<Pin>(pin, PinDirection::OUTPUT);
    } else if (mode == INPUT_PULLUP) {
        globalPins[pin] = std::make_unique<Pin>(pin, PinMode::INPUT_PULLUP);
    } else {
        globalPins[pin] = std::make_unique<Pin>(pin, PinDirection::INPUT);
    }
    
    PIPINPP_LOG_INFO("pinMode: Set pin " << pin << " to " 
                     << ((mode == OUTPUT) ? "OUTPUT" : 
                         (mode == INPUT_PULLUP) ? "INPUT_PULLUP" : "INPUT"));
}

void digitalWrite(int pin, bool value) 
{
    std::lock_guard<std::mutex> lock(globalPinsMutex);
    auto it = globalPins.find(pin);
    if (it == globalPins.end()) {
        throw InvalidPinError(pin, "Pin not initialized. Call pinMode() first.");
    }
    
    bool success = it->second->write(value);
    if (!success) {
        throw GpioAccessError("pin " + std::to_string(pin), "Failed to write to GPIO pin");
    }
}

int digitalRead(int pin) 
{
    std::lock_guard<std::mutex> lock(globalPinsMutex);
    auto it = globalPins.find(pin);
    if (it == globalPins.end()) {
        throw InvalidPinError(pin, "Pin not initialized. Call pinMode() first.");
    }
    
    return it->second->read();
}

// Removed duplicate delay(unsigned long ms) implementation

/* ------------------------------------------------------------ */
/*                        MATH FUNCTIONS                       */
/* ------------------------------------------------------------ */

int constrain(int x, int min, int max) 
{
    if (x < min) {
        return min;
    } else if (x > max) {
        return max;
    } else {
        return x;
    }
}

long constrain(long x, long min, long max) 
{
    if (x < min) {
        return min;
    } else if (x > max) {
        return max;
    } else {
        return x;
    }
}

float constrain(float x, float min, float max) 
{
    if (x < min) {
        return min;
    } else if (x > max) {
        return max;
    } else {
        return x;
    }
}

long map(long x, long in_min, long in_max, long out_min, long out_max) 
{
    // Prevent division by zero
    if (in_max == in_min) {
        return out_min;  // Return minimum output if input range is zero
    }
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/* ------------------------------------------------------------ */
/*                        TIMING FUNCTIONS                      */
/* ------------------------------------------------------------ */

unsigned long millis() 
{
    static const auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
    return static_cast<unsigned long>(elapsed);
}

unsigned long micros() 
{
    static const auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
    return static_cast<unsigned long>(elapsed);
}

void delayMicroseconds(unsigned int us) 
{
    auto start = std::chrono::steady_clock::now();
    auto duration = std::chrono::microseconds(us);
    while (std::chrono::steady_clock::now() - start < duration) {
        // Busy-wait for microsecond precision
    }
}

void delay(unsigned long ms) 
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

/* ------------------------------------------------------------ */
/*                    INTERRUPT FUNCTIONS                       */
/* ------------------------------------------------------------ */

#include "interrupts.hpp"

void attachInterrupt(int pin, void (*callback)(), int mode) 
{
    // Convert Arduino interrupt mode to InterruptMode enum
    InterruptMode intMode;
    switch (mode) {
        case RISING:
            intMode = InterruptMode::RISING;
            break;
        case FALLING:
            intMode = InterruptMode::FALLING;
            break;
        case CHANGE:
            intMode = InterruptMode::CHANGE;
            break;
        default:
            throw std::invalid_argument("Invalid interrupt mode: " + std::to_string(mode));
    }
    
    // Wrap the C-style function pointer in a std::function
    InterruptCallback wrappedCallback = [callback]() {
        if (callback) {
            callback();
        }
    };
    
    // Attach interrupt using InterruptManager
    InterruptManager::getInstance().attachInterrupt(pin, wrappedCallback, intMode);
    
    PIPINPP_LOG_INFO("attachInterrupt: Attached to pin " << pin 
                     << " mode=" << mode);
}

void detachInterrupt(int pin) 
{
    InterruptManager::getInstance().detachInterrupt(pin);
    PIPINPP_LOG_INFO("detachInterrupt: Detached from pin " << pin);
}   