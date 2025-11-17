/**
 * @file ArduinoCompat.cpp
 * @brief Implementation of Arduino-compatible API for Raspberry Pi GPIO
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

#include "ArduinoCompat.hpp"
#include "exceptions.hpp"
#include "log.hpp"
#include <unordered_map>
#include <memory>
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
            throw InvalidPinError("Invalid interrupt mode: " + std::to_string(mode) + 
                                ". Use RISING, FALLING, or CHANGE");
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

/* ------------------------------------------------------------ */
/*                  ADVANCED I/O FUNCTIONS                      */
/* ------------------------------------------------------------ */

unsigned long pulseIn(int pin, bool state, unsigned long timeout)
{
    // Validate pin number first (before acquiring mutex)
    if (pin < 0 || pin > 27) {
        throw InvalidPinError("Invalid pin number: " + std::to_string(pin) + 
                             ". Valid range is 0-27.");
    }
    
    // Verify pin is INPUT (don't auto-configure to avoid deadlock)
    {
        std::lock_guard<std::mutex> lock(globalPinsMutex);
        auto it = globalPins.find(pin);
        if (it == globalPins.end()) {
            throw PinError("Pin " + std::to_string(pin) + 
                          " not initialized. Call pinMode(pin, INPUT) first.");
        } else if (it->second.mode == ArduinoPinMode::OUTPUT) {
            throw PinError("Pin " + std::to_string(pin) + " is configured as OUTPUT. "
                          "Cannot use pulseIn() on output pins.");
        }
    }
    
    auto start = std::chrono::steady_clock::now();
    auto timeoutDuration = std::chrono::microseconds(timeout);
    
    // Wait for pin to go to opposite state (start of pulse)
    while (digitalRead(pin) == state) {
        if (std::chrono::steady_clock::now() - start >= timeoutDuration) {
            return 0;  // Timeout waiting for pulse start
        }
    }
    
    // Wait for pin to change to desired state (beginning of pulse)
    while (digitalRead(pin) != state) {
        if (std::chrono::steady_clock::now() - start >= timeoutDuration) {
            return 0;  // Timeout waiting for pulse
        }
    }
    
    // Measure pulse duration
    auto pulseStart = std::chrono::steady_clock::now();
    while (digitalRead(pin) == state) {
        if (std::chrono::steady_clock::now() - start >= timeoutDuration) {
            return 0;  // Timeout during pulse
        }
    }
    auto pulseEnd = std::chrono::steady_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(pulseEnd - pulseStart);
    return static_cast<unsigned long>(duration.count());
}

void shiftOut(int dataPin, int clockPin, int bitOrder, unsigned char value)
{
    // Validate pin numbers first (before acquiring mutex)
    if (dataPin < 0 || dataPin > 27) {
        throw InvalidPinError("Invalid data pin number: " + std::to_string(dataPin) + 
                             ". Valid range is 0-27.");
    }
    if (clockPin < 0 || clockPin > 27) {
        throw InvalidPinError("Invalid clock pin number: " + std::to_string(clockPin) + 
                             ". Valid range is 0-27.");
    }
    
    // Verify pins are OUTPUT. Don't auto-configure with pinMode() here, as that
    // would acquire globalPinsMutex while it is already held, causing a deadlock.
    // (pinMode() locks globalPinsMutex internally.)
    {
        std::lock_guard<std::mutex> lock(globalPinsMutex);
        if (globalPins.find(dataPin) == globalPins.end() || 
            globalPins[dataPin].mode != ArduinoPinMode::OUTPUT) {
            throw PinError("Pin " + std::to_string(dataPin) + 
                          " must be OUTPUT for shiftOut(). Call pinMode() first.");
        }
        if (globalPins.find(clockPin) == globalPins.end() || 
            globalPins[clockPin].mode != ArduinoPinMode::OUTPUT) {
            throw PinError("Pin " + std::to_string(clockPin) + 
                          " must be OUTPUT for shiftOut(). Call pinMode() first.");
        }
    }
    
    for (int i = 0; i < 8; i++) {
        bool bitValue;
        if (bitOrder == LSBFIRST) {
            bitValue = !!(value & (1 << i));  // LSB first
        } else {
            bitValue = !!(value & (1 << (7 - i)));  // MSB first
        }
        
        digitalWrite(dataPin, bitValue);
        digitalWrite(clockPin, HIGH);
        // Small delay for clock pulse (optional, can be removed for speed)
        delayMicroseconds(1);
        digitalWrite(clockPin, LOW);
    }
}

unsigned char shiftIn(int dataPin, int clockPin, int bitOrder)
{
    // Validate pin numbers first (before acquiring mutex)
    if (dataPin < 0 || dataPin > 27) {
        throw InvalidPinError("Invalid data pin number: " + std::to_string(dataPin) + 
                             ". Valid range is 0-27.");
    }
    if (clockPin < 0 || clockPin > 27) {
        throw InvalidPinError("Invalid clock pin number: " + std::to_string(clockPin) + 
                             ". Valid range is 0-27.");
    }
    
    // Verify dataPin is INPUT and clockPin is OUTPUT. Don't auto-configure with
    // pinMode() here, as that would acquire globalPinsMutex while it is already
    // held, causing a deadlock. (pinMode() locks globalPinsMutex internally.)
    // Also can't call isInput() helper as it also locks the mutex.
    {
        std::lock_guard<std::mutex> lock(globalPinsMutex);
        auto dataIt = globalPins.find(dataPin);
        if (dataIt == globalPins.end()) {
            throw PinError("Pin " + std::to_string(dataPin) + 
                          " not initialized. Call pinMode() first.");
        }
        ArduinoPinMode dataMode = dataIt->second.mode;
        bool isInputMode = (dataMode == ArduinoPinMode::INPUT || 
                           dataMode == ArduinoPinMode::INPUT_PULLUP || 
                           dataMode == ArduinoPinMode::INPUT_PULLDOWN);
        if (!isInputMode) {
            throw PinError("Pin " + std::to_string(dataPin) + 
                          " must be INPUT for shiftIn(). Call pinMode() first.");
        }
        
        if (globalPins.find(clockPin) == globalPins.end() || 
            globalPins[clockPin].mode != ArduinoPinMode::OUTPUT) {
            throw PinError("Pin " + std::to_string(clockPin) + 
                          " must be OUTPUT for shiftIn(). Call pinMode() first.");
        }
    }
    
    unsigned char value = 0;
    
    for (int i = 0; i < 8; i++) {
        digitalWrite(clockPin, HIGH);
        delayMicroseconds(1);  // Small delay for data to stabilize
        
        bool bitValue = digitalRead(dataPin);
        
        if (bitOrder == LSBFIRST) {
            value |= (bitValue << i);  // LSB first
        } else {
            value |= (bitValue << (7 - i));  // MSB first
        }
        
        digitalWrite(clockPin, LOW);
        delayMicroseconds(1);
    }
    
    return value;
}

// Tone generation - uses PWM with 50% duty cycle
void tone(int pin, unsigned int frequency, unsigned long duration)
{
    // Validate pin number first (before any other checks)
    if (pin < 0 || pin > 27) {
        throw InvalidPinError("Invalid pin number: " + std::to_string(pin) + 
                             ". Valid range is 0-27.");
    }
    
    if (frequency == 0 || frequency > 65535) {
        throw std::invalid_argument("Frequency must be between 1 and 65535 Hz");
    }
    
    // Verify pin is configured as OUTPUT. pinMode() must be called first.
    // If pin is held by pinMode(), release it so PWM can create its own Pin object.
    {
        std::lock_guard<std::mutex> lock(globalPinsMutex);
        auto it = globalPins.find(pin);
        if (it == globalPins.end()) {
            throw PinError("Pin " + std::to_string(pin) + 
                          " must be configured with pinMode(pin, OUTPUT) before calling tone().");
        }
        if (it->second.mode != ArduinoPinMode::OUTPUT) {
            throw PinError("Pin " + std::to_string(pin) + 
                          " must be OUTPUT for tone(). Call pinMode(pin, OUTPUT) first.");
        }
        // Release the pin so PWM can take control (creates its own Pin object)
        globalPins.erase(it);
    }
    
    // Start PWM at 50% duty cycle with specified frequency
    // PWMManager will create its own Pin object
    PWMManager::getInstance().startPWM(pin, 128, frequency);  // 128 = 50% of 255
    
    // If duration specified, wait and then stop
    if (duration > 0) {
        delay(duration);
        noTone(pin);
    }
    
    PIPINPP_LOG_INFO("tone: Pin " << pin << " frequency=" << frequency 
                     << "Hz duration=" << duration << "ms");
}

void noTone(int pin)
{
    // Validate pin number
    if (pin < 0 || pin > 27) {
        throw InvalidPinError("Invalid pin number: " + std::to_string(pin) + 
                             ". Valid range is 0-27.");
    }
    
    // Stop PWM (which stops the tone)
    // PWMManager's stopPWM() will set pin LOW and clean up the PWM channel
    PWMManager::getInstance().stopPWM(pin);
    
    // Re-add pin to globalPins so it can be used again with tone() or digitalWrite()
    // Note: tone() erased it from globalPins to let PWM have exclusive control
    {
        std::lock_guard<std::mutex> lock(globalPinsMutex);
        if (globalPins.find(pin) == globalPins.end()) {
            // Pin was erased by tone(), recreate it as OUTPUT
            PinInfo pinInfo;
            pinInfo.pin = std::make_unique<Pin>(pin, PinDirection::OUTPUT);
            pinInfo.mode = ArduinoPinMode::OUTPUT;
            pinInfo.lastValue = false;
            globalPins[pin] = std::move(pinInfo);
        }
    }
    
    PIPINPP_LOG_INFO("noTone: Stopped tone on pin " << pin);
}