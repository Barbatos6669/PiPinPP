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
    try {
        // Remove existing pin if it exists
        globalPins.erase(pin);
        
        // Create new pin with specified mode
        if (mode == OUTPUT) {
            globalPins[pin] = std::make_unique<Pin>(pin, PinDirection::OUTPUT);
        } else if (mode == INPUT_PULLUP) {
            globalPins[pin] = std::make_unique<Pin>(pin, PinMode::INPUT_PULLUP);
        } else {
            globalPins[pin] = std::make_unique<Pin>(pin, PinDirection::INPUT);
        }
        
        const char* modeStr = (mode == OUTPUT) ? "OUTPUT" : 
                             (mode == INPUT_PULLUP) ? "INPUT_PULLUP" : "INPUT";
        std::cout << "pinMode: Set pin " << pin << " to " << modeStr << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "pinMode error for pin " << pin << ": " << e.what() << std::endl;
    }
}

void digitalWrite(int pin, bool value) 
{
    std::lock_guard<std::mutex> lock(globalPinsMutex);
    auto it = globalPins.find(pin);
    if (it != globalPins.end()) {
        bool success = it->second->write(value);
        if (!success) {
            std::cerr << "digitalWrite failed for pin " << pin << std::endl;
        }
    } else {
        std::cerr << "digitalWrite: Pin " << pin << " not initialized. Call pinMode() first." << std::endl;
    }
}

int digitalRead(int pin) 
{
    std::lock_guard<std::mutex> lock(globalPinsMutex);
    auto it = globalPins.find(pin);
    if (it != globalPins.end()) {
        return it->second->read();
    } else {
        std::cerr << "digitalRead: Pin " << pin << " not initialized. Call pinMode() first." << std::endl;
        return -1;
    }
}

void delay(unsigned long ms) 
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

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