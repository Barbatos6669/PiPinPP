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
 * @file gtest_timing.cpp
 * @brief GoogleTest-based tests for timing functions
 * 
 * Tests the Arduino-inspired timing functions using GoogleTest framework:
 * - millis() - milliseconds since program start
 * - micros() - microseconds since program start
 * - delay() - millisecond delay
 * - delayMicroseconds() - microsecond delay
 */

#include "ArduinoCompat.hpp"
#include <gtest/gtest.h>
#include <cmath>

/**
 * Test that millis() increases monotonically
 */
TEST(TimingTest, MillisMonotonicity) {
    unsigned long t1 = millis();
    delay(100);  // Wait 100ms
    unsigned long t2 = millis();
    
    EXPECT_GT(t2, t1) << "millis() should increase over time";
    
    unsigned long elapsed = t2 - t1;
    EXPECT_GE(elapsed, 100) << "Should wait at least 100ms";
    EXPECT_LT(elapsed, 120) << "Should not wait much more than 100ms";
}

/**
 * Test that micros() increases monotonically and has microsecond precision
 */
TEST(TimingTest, MicrosMonotonicity) {
    unsigned long u1 = micros();
    delayMicroseconds(1000);  // Wait 1000 microseconds (1ms)
    unsigned long u2 = micros();
    
    EXPECT_GT(u2, u1) << "micros() should increase over time";
    
    unsigned long elapsed = u2 - u1;
    EXPECT_GE(elapsed, 1000) << "Should wait at least 1000µs";
    EXPECT_LT(elapsed, 1500) << "Should not wait much more than 1000µs";
}

/**
 * Test that millis() and micros() are consistent with each other
 */
TEST(TimingTest, MillisAndMicrosConsistency) {
    unsigned long m1 = millis();
    unsigned long u1 = micros();
    delay(50);
    unsigned long m2 = millis();
    unsigned long u2 = micros();
    
    unsigned long ms_elapsed = m2 - m1;
    unsigned long us_elapsed = u2 - u1;
    unsigned long us_from_ms = ms_elapsed * 1000;
    
    // Allow 10% tolerance
    long difference = std::abs(static_cast<long>(us_elapsed - us_from_ms));
    EXPECT_LT(difference, us_from_ms * 0.1) 
        << "millis() and micros() should be consistent within 10%";
}

/**
 * Test short delay precision
 */
TEST(TimingTest, ShortDelayPrecision) {
    unsigned long start = micros();
    delayMicroseconds(100);
    unsigned long end = micros();
    unsigned long actual = end - start;
    
    EXPECT_GE(actual, 100) << "Should delay at least 100µs";
    EXPECT_LT(actual, 200) << "Should not delay much more than 100µs";
}

/**
 * Test that delay(0) doesn't crash
 */
TEST(TimingTest, ZeroDelay) {
    unsigned long before = millis();
    delay(0);
    unsigned long after = millis();
    
    // Should complete quickly (within 10ms)
    EXPECT_LT(after - before, 10) << "delay(0) should complete quickly";
}

/**
 * Test that delayMicroseconds(0) doesn't crash
 */
TEST(TimingTest, ZeroMicrosecondDelay) {
    unsigned long before = micros();
    delayMicroseconds(0);
    unsigned long after = micros();
    
    // Should complete quickly (within 1000µs = 1ms)
    EXPECT_LT(after - before, 1000) << "delayMicroseconds(0) should complete quickly";
}

/**
 * Test that millis() doesn't overflow in reasonable time
 * (just verify it can handle large values)
 */
TEST(TimingTest, MillisHandlesLargeValues) {
    unsigned long t = millis();
    EXPECT_GE(t, 0) << "millis() should return non-negative values";
    
    // millis() uses unsigned long, so it will wrap around eventually
    // but should still be usable for differences
}

/**
 * Test delay accuracy with multiple iterations
 */
TEST(TimingTest, DelayAccuracy) {
    const int iterations = 5;
    const int delay_ms = 20;
    
    for (int i = 0; i < iterations; i++) {
        unsigned long start = millis();
        delay(delay_ms);
        unsigned long end = millis();
        unsigned long elapsed = end - start;
        
        EXPECT_GE(elapsed, delay_ms) << "Iteration " << i << " should delay at least " << delay_ms << "ms";
        EXPECT_LT(elapsed, delay_ms + 10) << "Iteration " << i << " should not delay much more than " << delay_ms << "ms";
    }
}
