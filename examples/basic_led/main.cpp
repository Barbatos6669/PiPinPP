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
 * @file basic_led/main.cpp
 * @brief Basic LED control using object-oriented Pin class
 * 
 * This example demonstrates the core Pin class functionality for controlling
 * an LED connected to GPIO pin 17. It shows the object-oriented approach
 * to GPIO control without Arduino compatibility layer.
 * 
 * Hardware Setup:
 * - Connect LED positive (long leg) to GPIO pin 17
 * - Connect LED negative (short leg) to GND through 220Ω resistor
 * 
 * @author PiPinPP Team
 * @version 0.2.0
 * @date 2025-10-31
 */

#include "pin.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() 
{
    std::cout << "PiPinPP Basic LED Example" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << "Using object-oriented Pin class approach" << std::endl;
    std::cout << "LED connected to GPIO pin 17" << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl << std::endl;

    try {
        // Create an output pin for the LED
        Pin led(17, PinDirection::OUTPUT);
        
        std::cout << "LED initialized successfully!" << std::endl;
        std::cout << "Starting blink sequence..." << std::endl << std::endl;
        
        // Blink the LED 10 times
        for (int i = 0; i < 10; ++i) {
            // Turn LED on
            if (led.write(true)) {
                std::cout << "Blink " << (i + 1) << "/10: LED ON" << std::endl;
            } else {
                std::cerr << "Error: Failed to turn LED on" << std::endl;
                return 1;
            }
            
            // Wait 500ms
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            // Turn LED off
            if (led.write(false)) {
                std::cout << "Blink " << (i + 1) << "/10: LED OFF" << std::endl;
            } else {
                std::cerr << "Error: Failed to turn LED off" << std::endl;
                return 1;
            }
            
            // Wait 500ms
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
        std::cout << std::endl << "Blink sequence complete!" << std::endl;
        std::cout << "LED will remain OFF" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << std::endl << "Troubleshooting:" << std::endl;
        std::cerr << "1. Make sure you're running as root (sudo)" << std::endl;
        std::cerr << "2. Check that GPIO pin 17 is available" << std::endl;
        std::cerr << "3. Verify libgpiod is installed" << std::endl;
        return 1;
    }
    
    return 0;
}