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

/**
 * @file button_input/main.cpp
 * @brief Button input reading with Arduino-style API and debouncing
 * 
 * This example demonstrates reading digital input from a button using the
 * Arduino-inspired API layer. It includes proper debouncing to handle
 * mechanical switch bounce and shows both immediate and debounced readings.
 * 
 * Hardware Setup:
 * - Connect button one terminal to GPIO pin 18
 * - Connect button other terminal to GND
 * - Pin 18 will use internal pull-up resistor (when implemented)
 * - For now, add external 10kΩ pull-up resistor to 3.3V
 * 
 * @author PiPinPP Team
 * @version 0.2.0
 * @date 2025-10-31
 */

#include "ArduinoCompat.hpp"
#include <iostream>
#include <chrono>

// Simple debouncing variables
bool lastButtonState = false;
bool currentButtonState = false;
auto lastDebounceTime = std::chrono::steady_clock::now();
const int debounceDelay = 50; // 50ms debounce delay

/**
 * @brief Read button state with debouncing
 * @param pin GPIO pin number for button
 * @return bool True if button is pressed (after debouncing)
 */
bool readButtonDebounced(int pin) {
    int reading = digitalRead(pin);
    auto currentTime = std::chrono::steady_clock::now();
    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - lastDebounceTime).count();
    
    // If the reading has changed, reset the debounce timer
    if (reading != lastButtonState) {
        lastDebounceTime = currentTime;
    }
    
    // If enough time has passed, accept the reading
    if (timeDiff > debounceDelay) {
        if (reading != currentButtonState) {
            currentButtonState = reading;
        }
    }
    
    lastButtonState = reading;
    return currentButtonState;
}

int main() 
{
    std::cout << "PiPinPP Button Input Example" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "Using Arduino-style API with debouncing" << std::endl;
    std::cout << "Button connected to GPIO pin 18" << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl << std::endl;

    // Initialize button pin as input
    pinMode(18, INPUT);
    std::cout << "Button pin initialized successfully!" << std::endl;
    std::cout << "Reading button state every 100ms..." << std::endl;
    std::cout << "Press and release the button to see debounced vs raw readings" << std::endl << std::endl;
    
    int pressCount = 0;
    bool lastDebouncedState = false;
    
    // Main loop - read button state continuously
    while (true) {
        // Read raw button state (immediate)
        int rawState = digitalRead(18);
        
        // Read debounced button state
        bool debouncedState = readButtonDebounced(18);
        
        // Detect button press (transition from released to pressed)
        if (debouncedState && !lastDebouncedState) {
            pressCount++;
            std::cout << "*** BUTTON PRESSED! (Press count: " << pressCount << ") ***" << std::endl;
        }
        
        // Detect button release (transition from pressed to released)
        if (!debouncedState && lastDebouncedState) {
            std::cout << "*** BUTTON RELEASED ***" << std::endl;
        }
        
        // Show both raw and debounced states (only when different)
        if (rawState != debouncedState) {
            std::cout << "Raw: " << rawState << " | Debounced: " << debouncedState 
                      << " (filtering bounce)" << std::endl;
        }
        
        lastDebouncedState = debouncedState;
        
        // Wait 100ms before next reading
        delay(100);
    }
    
    return 0;
}