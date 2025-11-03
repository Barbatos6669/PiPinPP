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

/*
 * Test timing functions: millis(), micros(), delayMicroseconds()
 */

#include "ArduinoCompat.hpp"
#include <iostream>
#include <cmath>

int main() 
{
    std::cout << "Testing PiPinPP Timing Functions\n";
    std::cout << "=================================\n\n";

    // Test 1: millis() monotonicity
    std::cout << "Test 1: millis() monotonicity...\n";
    unsigned long t1 = millis();
    delay(100);  // Wait 100ms
    unsigned long t2 = millis();
    unsigned long elapsed_ms = t2 - t1;
    
    std::cout << "  Start: " << t1 << " ms\n";
    std::cout << "  After 100ms delay: " << t2 << " ms\n";
    std::cout << "  Elapsed: " << elapsed_ms << " ms\n";
    
    if (elapsed_ms >= 100 && elapsed_ms < 120) {
        std::cout << "  ✓ PASS: delay(100) took " << elapsed_ms << " ms\n\n";
    } else {
        std::cout << "  ✗ FAIL: Expected ~100ms, got " << elapsed_ms << " ms\n\n";
        return 1;
    }

    // Test 2: micros() monotonicity and precision
    std::cout << "Test 2: micros() monotonicity...\n";
    unsigned long u1 = micros();
    delayMicroseconds(1000);  // Wait 1000 microseconds (1ms)
    unsigned long u2 = micros();
    unsigned long elapsed_us = u2 - u1;
    
    std::cout << "  Start: " << u1 << " µs\n";
    std::cout << "  After 1000µs delay: " << u2 << " µs\n";
    std::cout << "  Elapsed: " << elapsed_us << " µs\n";
    
    if (elapsed_us >= 1000 && elapsed_us < 1500) {
        std::cout << "  ✓ PASS: delayMicroseconds(1000) took " << elapsed_us << " µs\n\n";
    } else {
        std::cout << "  ✗ FAIL: Expected ~1000µs, got " << elapsed_us << " µs\n\n";
        return 1;
    }

    // Test 3: millis() and micros() consistency
    std::cout << "Test 3: millis() and micros() consistency...\n";
    unsigned long m1 = millis();
    unsigned long u1_test = micros();
    delay(50);
    unsigned long m2 = millis();
    unsigned long u2_test = micros();
    
    unsigned long ms_elapsed = m2 - m1;
    unsigned long us_elapsed = u2_test - u1_test;
    unsigned long us_from_ms = ms_elapsed * 1000;
    
    std::cout << "  millis() elapsed: " << ms_elapsed << " ms\n";
    std::cout << "  micros() elapsed: " << us_elapsed << " µs\n";
    std::cout << "  Difference: " << std::abs(static_cast<long>(us_elapsed - us_from_ms)) << " µs\n";
    
    // Allow 10% tolerance
    if (std::abs(static_cast<long>(us_elapsed - us_from_ms)) < us_from_ms * 0.1) {
        std::cout << "  ✓ PASS: millis() and micros() are consistent\n\n";
    } else {
        std::cout << "  ✗ FAIL: millis() and micros() are inconsistent\n\n";
        return 1;
    }

    // Test 4: Short delay precision
    std::cout << "Test 4: Short delay precision (100µs)...\n";
    unsigned long start = micros();
    delayMicroseconds(100);
    unsigned long end = micros();
    unsigned long actual = end - start;
    
    std::cout << "  Expected: 100 µs\n";
    std::cout << "  Actual: " << actual << " µs\n";
    
    if (actual >= 100 && actual < 200) {
        std::cout << "  ✓ PASS: Short delay is accurate\n\n";
    } else {
        std::cout << "  ✗ FAIL: Short delay off by " << (actual - 100) << " µs\n\n";
        return 1;
    }

    std::cout << "=================================\n";
    std::cout << "All timing tests PASSED! ✓\n";
    return 0;
}
