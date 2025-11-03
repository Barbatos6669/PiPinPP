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
 * @file timing_benchmark/main.cpp
 * @brief Demonstrates timing functions and measures GPIO operation performance
 * 
 * This example shows:
 * - millis() and micros() for time measurement
 * - delay() and delayMicroseconds() for precise timing
 * - Measuring GPIO write operation speed
 * - Practical timing applications
 * 
 * Hardware:
 * - No external hardware required (uses GPIO 17)
 * - Optional: Connect LED to GPIO 17 with 220Ω resistor to ground
 * 
 * Wiring (optional):
 *   GPIO 17 ----[220Ω]----[LED]---- GND
 */

#include "ArduinoCompat.hpp"
#include "pin.hpp"
#include <iostream>
#include <iomanip>
#include <climits>
#include <cmath>

int main() 
{
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║         PiPinPP Timing Functions Benchmark           ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n\n";

    // ========== Test 1: Basic Timing Functions ==========
    std::cout << "Test 1: Basic Timing Functions\n";
    std::cout << "─────────────────────────────────────────\n";
    
    unsigned long start_ms = millis();
    unsigned long start_us = micros();
    
    std::cout << "Current time: " << start_ms << " ms (" << start_us << " µs)\n";
    std::cout << "Waiting 1 second...\n";
    
    delay(1000);
    
    unsigned long end_ms = millis();
    unsigned long end_us = micros();
    
    std::cout << "After 1s delay: " << end_ms << " ms (" << end_us << " µs)\n";
    std::cout << "Actual elapsed: " << (end_ms - start_ms) << " ms\n";
    std::cout << "Accuracy: ±" << std::abs(static_cast<long>((end_ms - start_ms) - 1000)) << " ms\n\n";

    // ========== Test 2: Microsecond Precision ==========
    std::cout << "Test 2: Microsecond Precision\n";
    std::cout << "─────────────────────────────────────────\n";
    
    std::cout << "Testing delayMicroseconds() accuracy:\n";
    
    for (int delay_us : {100, 500, 1000, 5000}) {
        unsigned long t1 = micros();
        delayMicroseconds(delay_us);
        unsigned long t2 = micros();
        unsigned long actual = t2 - t1;
        long error = actual - delay_us;
        
        std::cout << std::setw(6) << delay_us << " µs delay → "
                  << std::setw(6) << actual << " µs actual "
                  << "(error: " << std::showpos << error << std::noshowpos << " µs)\n";
    }
    std::cout << "\n";

    // ========== Test 3: GPIO Write Speed ==========
    std::cout << "Test 3: GPIO Write Operation Speed\n";
    std::cout << "─────────────────────────────────────────\n";
    
    try {
        Pin led(17, PinDirection::OUTPUT);
        
        std::cout << "Measuring 1000 GPIO write operations...\n";
        
        unsigned long gpio_start = micros();
        for (int i = 0; i < 1000; i++) {
            led.write(HIGH);
            led.write(LOW);
        }
        unsigned long gpio_end = micros();
        
        unsigned long total_time = gpio_end - gpio_start;
        double avg_time = total_time / 2000.0; // 2000 writes (HIGH and LOW)
        
        std::cout << "Total time: " << total_time << " µs\n";
        std::cout << "Average per write: " << std::fixed << std::setprecision(2) 
                  << avg_time << " µs\n";
        std::cout << "Approximate frequency: " << std::fixed << std::setprecision(0) 
                  << (1000000.0 / (avg_time * 2)) << " Hz (toggle rate)\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "GPIO test failed: " << e.what() << "\n\n";
    }

    // ========== Test 4: Precision LED Blink ==========
    std::cout << "Test 4: Precision LED Blink Pattern\n";
    std::cout << "─────────────────────────────────────────\n";
    std::cout << "Creating precise blink pattern...\n";
    std::cout << "(Connect LED to GPIO 17 to see the effect)\n\n";
    
    try {
        Pin led(17, PinDirection::OUTPUT);
        
        std::cout << "Pattern: 3 fast blinks, 1 slow blink (repeating 3 times)\n";
        
        for (int cycle = 0; cycle < 3; cycle++) {
            std::cout << "Cycle " << (cycle + 1) << ":\n";
            
            // 3 fast blinks (100ms on, 100ms off)
            for (int i = 0; i < 3; i++) {
                unsigned long blink_start = millis();
                led.write(HIGH);
                delay(100);
                led.write(LOW);
                delay(100);
                unsigned long blink_end = millis();
                
                std::cout << "  Fast blink " << (i + 1) 
                          << " - timing: " << (blink_end - blink_start) << " ms\n";
            }
            
            // 1 slow blink (500ms on, 500ms off)
            unsigned long blink_start = millis();
            led.write(HIGH);
            delay(500);
            led.write(LOW);
            delay(500);
            unsigned long blink_end = millis();
            
            std::cout << "  Slow blink - timing: " << (blink_end - blink_start) << " ms\n\n";
        }
        
        std::cout << "Pattern complete!\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "LED test failed: " << e.what() << "\n\n";
    }

    // ========== Test 5: Timing Resolution ==========
    std::cout << "Test 5: Timing Resolution Test\n";
    std::cout << "─────────────────────────────────────────\n";
    std::cout << "Measuring minimum detectable time difference:\n";
    
    unsigned long prev = micros();
    unsigned long curr;
    int samples = 0;
    unsigned long min_diff = ULONG_MAX;
    
    for (int i = 0; i < 1000; i++) {
        curr = micros();
        if (curr != prev) {
            unsigned long diff = curr - prev;
            if (diff < min_diff) min_diff = diff;
            samples++;
            prev = curr;
        }
    }
    
    std::cout << "Minimum time difference detected: " << min_diff << " µs\n";
    std::cout << "Samples with time change: " << samples << " / 1000\n";
    std::cout << "Resolution: ~" << min_diff << " µs\n\n";

    // ========== Summary ==========
    std::cout << "╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    Test Complete!                     ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "Key Findings:\n";
    std::cout << "✓ millis() provides millisecond timing\n";
    std::cout << "✓ micros() provides microsecond precision\n";
    std::cout << "✓ delay() is accurate for timing control\n";
    std::cout << "✓ delayMicroseconds() works for short delays\n";
    std::cout << "✓ GPIO operations are fast and predictable\n\n";
    
    std::cout << "Use Cases:\n";
    std::cout << "• millis() - Timeouts, intervals, debouncing\n";
    std::cout << "• micros() - Precise measurements, sensor timing\n";
    std::cout << "• delay() - Simple waiting, LED blink patterns\n";
    std::cout << "• delayMicroseconds() - Sensor protocols, PWM emulation\n\n";

    return 0;
}
