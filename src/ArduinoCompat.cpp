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

// Structure to store pin information
struct PinInfo {
    std::unique_ptr<Pin> pin;
    ArduinoPinMode mode;
    bool lastValue;
};

// Global storage for Arduino-style pins
static std::unordered_map<int, PinInfo> globalPins;
// Mutex to protect globalPins from concurrent access
static std::mutex globalPinsMutex;

void pinMode(int pin, int mode) 
{
    std::lock_guard<std::mutex> lock(globalPinsMutex);
    // Remove existing pin if it exists
    globalPins.erase(pin);
    
    // Create new pin with specified mode
    // Exceptions will propagate naturally (InvalidPinError, GpioAccessError)
    PinInfo pinInfo;
    if (mode == OUTPUT) {
        pinInfo.pin = std::make_unique<Pin>(pin, PinDirection::OUTPUT);
        pinInfo.mode = ArduinoPinMode::OUTPUT;
        pinInfo.lastValue = false;
    } else if (mode == INPUT_PULLUP) {
        pinInfo.pin = std::make_unique<Pin>(pin, PinMode::INPUT_PULLUP);
        pinInfo.mode = ArduinoPinMode::INPUT_PULLUP;
        pinInfo.lastValue = false;
    } else if (mode == INPUT_PULLDOWN) {
        pinInfo.pin = std::make_unique<Pin>(pin, PinMode::INPUT_PULLDOWN);
        pinInfo.mode = ArduinoPinMode::INPUT_PULLDOWN;
        pinInfo.lastValue = false;
    } else {
        pinInfo.pin = std::make_unique<Pin>(pin, PinDirection::INPUT);
        pinInfo.mode = ArduinoPinMode::INPUT;
        pinInfo.lastValue = false;
    }
    
    globalPins[pin] = std::move(pinInfo);
    
    PIPINPP_LOG_INFO("pinMode: Set pin " << pin << " to " 
                     << ((mode == OUTPUT) ? "OUTPUT" : 
                         (mode == INPUT_PULLUP) ? "INPUT_PULLUP" : 
                         (mode == INPUT_PULLDOWN) ? "INPUT_PULLDOWN" : "INPUT"));
}

void digitalWrite(int pin, bool value) 
{
    std::lock_guard<std::mutex> lock(globalPinsMutex);
    auto it = globalPins.find(pin);
    if (it == globalPins.end()) {
        throw InvalidPinError(pin, "Pin not initialized. Call pinMode() first.");
    }
    
    bool success = it->second.pin->write(value);
    if (success) {
        it->second.lastValue = value;  // Track the last written value
    } else {
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
    
    return it->second.pin->read();
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

/* ------------------------------------------------------------ */
/*                        PWM FUNCTIONS                         */
/* ------------------------------------------------------------ */

#include "pwm.hpp"

void analogWrite(int pin, int value) 
{
    // Clamp value to valid range
    if (value < 0) value = 0;
    if (value > 255) value = 255;
    
    // Start or update PWM
    PWMManager::getInstance().startPWM(pin, value);
    
    PIPINPP_LOG_INFO("analogWrite: Pin " << pin << " set to " << value);
}   

// ============================================================================
// PIN STATE QUERY FUNCTIONS (v0.3.1) Edited: 2025-11-04
// ============================================================================

bool isOutput(int pin) 
{
    std::lock_guard<std::mutex> lock(globalPinsMutex);
    auto it = globalPins.find(pin);
    if (it == globalPins.end()) {
        throw PinError("Pin " + std::to_string(pin) + " not initialized. Call pinMode() first.");
    }
    return it->second.mode == ArduinoPinMode::OUTPUT;
}

bool isInput(int pin) 
{
    std::lock_guard<std::mutex> lock(globalPinsMutex);
    auto it = globalPins.find(pin);
    if (it == globalPins.end()) {
        throw PinError("Pin " + std::to_string(pin) + " not initialized. Call pinMode() first.");
    }
    ArduinoPinMode mode = it->second.mode;
    return mode == ArduinoPinMode::INPUT || 
           mode == ArduinoPinMode::INPUT_PULLUP || 
           mode == ArduinoPinMode::INPUT_PULLDOWN;
}

ArduinoPinMode getMode(int pin) 
{
    std::lock_guard<std::mutex> lock(globalPinsMutex);
    auto it = globalPins.find(pin);
    if (it == globalPins.end()) {
        throw PinError("Pin " + std::to_string(pin) + " not initialized. Call pinMode() first.");
    }
    return it->second.mode;
}

void digitalToggle(int pin) 
{
    std::lock_guard<std::mutex> lock(globalPinsMutex);
    auto it = globalPins.find(pin);
    if (it == globalPins.end()) {
        throw PinError("Pin " + std::to_string(pin) + " not initialized. Call pinMode() first.");
    }
    
    if (it->second.mode != ArduinoPinMode::OUTPUT) {
        throw PinError("Pin " + std::to_string(pin) + " must be OUTPUT to toggle. "
                      "Current mode: " + std::to_string(static_cast<int>(it->second.mode)));
    }
    
    // Toggle efficiently - no need to read current value
    bool newValue = !it->second.lastValue;
    bool success = it->second.pin->write(newValue);
    if (success) {
        it->second.lastValue = newValue;  // Only update if write succeeded
    } else {
        throw GpioAccessError("pin " + std::to_string(pin), "Failed to toggle GPIO pin");
    }
}

/* ------------------------------------------------------------ */
/*                    RANDOM FUNCTIONS                          */
/* ------------------------------------------------------------ */

#include <random>

// Static random engine and distribution (thread-safe via mutex)
static std::mt19937 randomEngine;
static std::mutex randomMutex;

void randomSeed(unsigned long seed)
{
    std::lock_guard<std::mutex> lock(randomMutex);
    randomEngine.seed(seed);
}

long random(long max)
{
    if (max <= 0) {
        return 0;
    }
    std::lock_guard<std::mutex> lock(randomMutex);
    std::uniform_int_distribution<long> dist(0, max - 1);
    return dist(randomEngine);
}

long random(long min, long max)
{
    if (min >= max) {
        return min;
    }
    std::lock_guard<std::mutex> lock(randomMutex);
    std::uniform_int_distribution<long> dist(min, max - 1);
    return dist(randomEngine);
}