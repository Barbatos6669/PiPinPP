/**
 * @file gtest_arduino_compat_extended.cpp
 * @brief Extended GoogleTest unit tests for Arduino compatibility functions
 * 
 * Focuses on edge cases, error handling, and less common usage patterns
 * to improve code coverage of ArduinoCompat.cpp from 55% to 75%+.
 * 
 * @copyright Copyright (c) 2025 PiPinPP Project
 * @license MIT License
 */

#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "ArduinoCompat.hpp"
#include "exceptions.hpp"

using namespace std::chrono_literals;

// Test fixture for extended Arduino compatibility tests
class ArduinoCompatExtendedTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clean state
    }
    
    void TearDown() override {
        // Clean up pins
        try {
            pinMode(17, INPUT);
            pinMode(18, INPUT);
            pinMode(27, INPUT);
        } catch (...) {
            // Ignore cleanup errors
        }
    }
};

// ============================================================================
// EDGE CASES FOR DIGITAL I/O
// ============================================================================

// Test: digitalWrite on input pin (can fail - hardware dependent)
TEST_F(ArduinoCompatExtendedTest, DigitalWriteOnInputPin) {
    try {
        pinMode(17, INPUT);
        
        // Writing to input pin may work depending on hardware
        digitalWrite(17, HIGH);
        digitalWrite(17, LOW);
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        // This is hardware dependent - skip if not supported
        GTEST_SKIP() << "GPIO write on input pin not available: " << e.what();
    }
}

// Test: digitalRead on output pin (should work)
TEST_F(ArduinoCompatExtendedTest, DigitalReadOnOutputPin) {
    try {
        pinMode(17, OUTPUT);
        digitalWrite(17, HIGH);
        
        // Reading output pin should work and return current state
        int value = digitalRead(17);
        EXPECT_TRUE(value == HIGH || value == LOW);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Rapid pinMode changes
TEST_F(ArduinoCompatExtendedTest, RapidPinModeChanges) {
    try {
        // Rapid mode switching should work
        for (int i = 0; i < 10; i++) {
            pinMode(17, OUTPUT);
            pinMode(17, INPUT);
            pinMode(17, INPUT_PULLUP);
            pinMode(17, INPUT_PULLDOWN);
        }
        
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Multiple digitalWrite calls
TEST_F(ArduinoCompatExtendedTest, MultipleDigitalWriteCalls) {
    try {
        pinMode(17, OUTPUT);
        
        // Rapid writes
        for (int i = 0; i < 100; i++) {
            digitalWrite(17, i % 2);
        }
        
        // Should end in known state
        digitalWrite(17, LOW);
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Multiple digitalRead calls
TEST_F(ArduinoCompatExtendedTest, MultipleDigitalReadCalls) {
    try {
        pinMode(17, INPUT_PULLUP);
        
        // Multiple reads should be consistent
        int first = digitalRead(17);
        for (int i = 0; i < 10; i++) {
            int value = digitalRead(17);
            EXPECT_EQ(value, first) << "Digital read should be stable";
        }
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// ============================================================================
// EDGE CASES FOR ANALOG I/O (SOFTWARE PWM)
// ============================================================================

// Test: analogWrite with extreme values
TEST_F(ArduinoCompatExtendedTest, AnalogWriteExtremeValues) {
    try {
        pinMode(17, OUTPUT);
        
        // Test boundary values
        analogWrite(17, 0);      // 0% duty cycle
        delay(10);
        analogWrite(17, 255);    // 100% duty cycle
        delay(10);
        analogWrite(17, 128);    // 50% duty cycle
        delay(10);
        analogWrite(17, 1);      // Minimum non-zero
        delay(10);
        analogWrite(17, 254);    // Maximum before 100%
        delay(10);
        
        // Clean up
        analogWrite(17, 0);
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: analogWrite then digitalWrite (mode switching)
TEST_F(ArduinoCompatExtendedTest, AnalogWriteThenDigitalWrite) {
    try {
        pinMode(17, OUTPUT);
        
        // Start with PWM
        analogWrite(17, 128);
        delay(50);
        
        // Switch to digital
        digitalWrite(17, HIGH);
        delay(50);
        
        // Back to PWM
        analogWrite(17, 64);
        delay(50);
        
        // Clean up
        analogWrite(17, 0);
        
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Multiple analogWrite calls (no delay)
TEST_F(ArduinoCompatExtendedTest, RapidAnalogWriteCalls) {
    try {
        pinMode(17, OUTPUT);
        
        // Rapid PWM changes
        for (int i = 0; i < 50; i++) {
            analogWrite(17, i * 5);  // 0, 5, 10, ..., 245
        }
        
        // Clean up
        analogWrite(17, 0);
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: analogWrite on invalid pin
TEST_F(ArduinoCompatExtendedTest, AnalogWriteInvalidPin) {
    EXPECT_THROW({
        analogWrite(99, 128);
    }, InvalidPinError);
    
    EXPECT_THROW({
        analogWrite(-1, 128);
    }, InvalidPinError);
}

// Note: analogRead not implemented in PiPinPP (Raspberry Pi has no ADC)

// ============================================================================
// EDGE CASES FOR TIMING FUNCTIONS
// ============================================================================

// Test: delay with zero
TEST_F(ArduinoCompatExtendedTest, DelayZero) {
    auto start = std::chrono::steady_clock::now();
    delay(0);
    auto end = std::chrono::steady_clock::now();
    
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    EXPECT_LT(duration_ms, 10) << "delay(0) should be very fast";
}

// Test: delayMicroseconds with zero
TEST_F(ArduinoCompatExtendedTest, DelayMicrosecondsZero) {
    auto start = std::chrono::steady_clock::now();
    delayMicroseconds(0);
    auto end = std::chrono::steady_clock::now();
    
    auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    EXPECT_LT(duration_us, 100) << "delayMicroseconds(0) should be very fast";
}

// Test: delayMicroseconds with large value
TEST_F(ArduinoCompatExtendedTest, DelayMicrosecondsLarge) {
    auto start = std::chrono::steady_clock::now();
    delayMicroseconds(10000);  // 10ms
    auto end = std::chrono::steady_clock::now();
    
    auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    EXPECT_GE(duration_us, 9000) << "Should delay at least 9ms";
    EXPECT_LE(duration_us, 12000) << "Should delay no more than 12ms";
}

// Test: millis() increases
TEST_F(ArduinoCompatExtendedTest, MillisIncreases) {
    unsigned long t1 = millis();
    delay(10);
    unsigned long t2 = millis();
    
    EXPECT_GT(t2, t1) << "millis() should increase over time";
    EXPECT_GE(t2 - t1, 9) << "Should have elapsed at least 9ms";
    EXPECT_LE(t2 - t1, 20) << "Should have elapsed no more than 20ms";
}

// Test: micros() increases
TEST_F(ArduinoCompatExtendedTest, MicrosIncreases) {
    unsigned long t1 = micros();
    delayMicroseconds(1000);
    unsigned long t2 = micros();
    
    EXPECT_GT(t2, t1) << "micros() should increase over time";
    EXPECT_GE(t2 - t1, 900) << "Should have elapsed at least 900µs";
    EXPECT_LE(t2 - t1, 1500) << "Should have elapsed no more than 1500µs";
}

// Test: millis() and micros() correlation
TEST_F(ArduinoCompatExtendedTest, MillisMicrosCorrelation) {
    unsigned long ms1 = millis();
    unsigned long us1 = micros();
    
    delay(10);
    
    unsigned long ms2 = millis();
    unsigned long us2 = micros();
    
    unsigned long ms_elapsed = ms2 - ms1;
    unsigned long us_elapsed = us2 - us1;
    
    // microseconds elapsed should be ~1000x milliseconds elapsed
    double ratio = static_cast<double>(us_elapsed) / (ms_elapsed > 0 ? ms_elapsed : 1);
    EXPECT_GE(ratio, 800) << "micros should be ~1000x millis";
    EXPECT_LE(ratio, 1200) << "micros should be ~1000x millis";
}

// ============================================================================
// EDGE CASES FOR PIN VALIDATION
// ============================================================================

// Test: Reserved pin warnings (I2C pins 0, 1)
TEST_F(ArduinoCompatExtendedTest, ReservedI2CPins) {
    try {
        // Pins 0 and 1 should warn but work
        EXPECT_NO_THROW({
            pinMode(0, OUTPUT);
            pinMode(1, OUTPUT);
        });
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Reserved pin warnings (UART pins 14, 15)
TEST_F(ArduinoCompatExtendedTest, ReservedUARTPins) {
    try {
        // Pins 14 and 15 should warn but work
        EXPECT_NO_THROW({
            pinMode(14, OUTPUT);
            pinMode(15, OUTPUT);
        });
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Valid edge pins (0 and 27)
TEST_F(ArduinoCompatExtendedTest, EdgePinsValid) {
    try {
        // Pins 0 and 27 are valid endpoints
        EXPECT_NO_THROW({
            pinMode(0, INPUT);
            pinMode(27, INPUT);
        });
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// ============================================================================
// THREAD SAFETY TESTS
// ============================================================================

// Test: Concurrent pinMode on different pins
TEST_F(ArduinoCompatExtendedTest, ConcurrentPinModeDifferentPins) {
    try {
        std::thread t1([&]() {
            for (int i = 0; i < 50; i++) {
                pinMode(17, OUTPUT);
                digitalWrite(17, HIGH);
            }
        });
        
        std::thread t2([&]() {
            for (int i = 0; i < 50; i++) {
                pinMode(27, OUTPUT);
                digitalWrite(27, LOW);
            }
        });
        
        t1.join();
        t2.join();
        
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Concurrent digitalWrite on same pin
TEST_F(ArduinoCompatExtendedTest, ConcurrentDigitalWriteSamePin) {
    try {
        pinMode(17, OUTPUT);
        
        std::thread t1([&]() {
            for (int i = 0; i < 100; i++) {
                digitalWrite(17, HIGH);
            }
        });
        
        std::thread t2([&]() {
            for (int i = 0; i < 100; i++) {
                digitalWrite(17, LOW);
            }
        });
        
        t1.join();
        t2.join();
        
        // Should not crash
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Concurrent analogWrite on different pins
TEST_F(ArduinoCompatExtendedTest, ConcurrentAnalogWriteDifferentPins) {
    try {
        pinMode(22, OUTPUT);
        pinMode(23, OUTPUT);
        
        std::thread t1([&]() {
            try {
                for (int i = 0; i < 20; i++) {
                    analogWrite(22, 128);
                    delay(5);
                }
                analogWrite(22, 0);
            } catch (...) {}
        });
        
        std::thread t2([&]() {
            try {
                for (int i = 0; i < 20; i++) {
                    analogWrite(23, 64);
                    delay(5);
                }
                analogWrite(23, 0);
            } catch (...) {}
        });
        
        t1.join();
        t2.join();
        
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// ============================================================================
// PIN REGISTRY TESTS
// ============================================================================

// Test: Pin registry reuse (same pin accessed multiple times)
TEST_F(ArduinoCompatExtendedTest, PinRegistryReuse) {
    try {
        // Multiple pinMode calls on same pin should reuse Pin object
        pinMode(17, OUTPUT);
        pinMode(17, INPUT);
        pinMode(17, OUTPUT);
        pinMode(17, INPUT_PULLUP);
        
        // All should work without errors
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Multiple pins in registry
TEST_F(ArduinoCompatExtendedTest, MultiplePinsInRegistry) {
    try {
        // Create multiple pin objects
        pinMode(17, OUTPUT);
        pinMode(18, OUTPUT);
        pinMode(27, INPUT);
        
        // All should coexist
        digitalWrite(17, HIGH);
        digitalWrite(18, LOW);
        int val = digitalRead(27);
        (void)val;  // Suppress unused warning
        
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// ============================================================================
// ADVANCED FUNCTIONS: shiftOut, pulseIn, tone/noTone
// ============================================================================

// Test: shiftOut basic functionality
TEST_F(ArduinoCompatExtendedTest, ShiftOutMSBFirst) {
    try {
        pinMode(17, OUTPUT);  // Data pin
        pinMode(18, OUTPUT);  // Clock pin
        
        // Shift out value MSB first
        EXPECT_NO_THROW({
            shiftOut(17, 18, MSBFIRST, 0xAA);
            shiftOut(17, 18, MSBFIRST, 0x55);
        });
        
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: shiftOut LSB first
TEST_F(ArduinoCompatExtendedTest, ShiftOutLSBFirst) {
    try {
        pinMode(17, OUTPUT);
        pinMode(18, OUTPUT);
        
        EXPECT_NO_THROW({
            shiftOut(17, 18, LSBFIRST, 0xFF);
            shiftOut(17, 18, LSBFIRST, 0x00);
        });
        
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: shiftOut without pinMode
TEST_F(ArduinoCompatExtendedTest, ShiftOutWithoutPinMode) {
    // Don't set pinMode - should throw or handle gracefully
    EXPECT_ANY_THROW({
        shiftOut(17, 18, MSBFIRST, 0x42);
    });
}

// Test: shiftOut with invalid bit order
TEST_F(ArduinoCompatExtendedTest, ShiftOutInvalidBitOrder) {
    try {
        pinMode(17, OUTPUT);
        pinMode(18, OUTPUT);
        
        // Invalid bit order (not MSBFIRST or LSBFIRST)
        // Should default to LSBFIRST or handle gracefully
        EXPECT_NO_THROW({
            shiftOut(17, 18, 99, 0x42);
        });
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: pulseIn timeout
TEST_F(ArduinoCompatExtendedTest, PulseInTimeout) {
    try {
        pinMode(17, INPUT);
        
        // Timeout with no pulse (should return 0)
        unsigned long duration = pulseIn(17, HIGH, 1000);  // 1ms timeout
        EXPECT_EQ(duration, 0);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: pulseIn on output pin (should throw)
TEST_F(ArduinoCompatExtendedTest, PulseInOnOutputPin) {
    try {
        pinMode(17, OUTPUT);
        
        // pulseIn requires INPUT mode
        EXPECT_ANY_THROW({
            pulseIn(17, HIGH, 1000);
        });
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: pulseIn both states
TEST_F(ArduinoCompatExtendedTest, PulseInBothStates) {
    try {
        pinMode(17, INPUT);
        
        // Test HIGH and LOW states (both timeout)
        unsigned long high_pulse = pulseIn(17, HIGH, 100);
        unsigned long low_pulse = pulseIn(17, LOW, 100);
        
        // Both should timeout to 0
        EXPECT_EQ(high_pulse, 0);
        EXPECT_EQ(low_pulse, 0);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: tone basic functionality
TEST_F(ArduinoCompatExtendedTest, ToneBasicFunctionality) {
    try {
        pinMode(17, OUTPUT);
        
        // Start tone
        EXPECT_NO_THROW({
            tone(17, 1000);  // 1kHz tone
        });
        
        std::this_thread::sleep_for(10ms);
        
        // Stop tone
        EXPECT_NO_THROW({
            noTone(17);
        });
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: tone with duration
TEST_F(ArduinoCompatExtendedTest, ToneWithDuration) {
    try {
        pinMode(17, OUTPUT);
        
        // Tone with automatic stop after 100ms
        EXPECT_NO_THROW({
            tone(17, 2000, 100);  // 2kHz for 100ms
        });
        
        std::this_thread::sleep_for(150ms);  // Wait for tone to finish
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: tone without pinMode
TEST_F(ArduinoCompatExtendedTest, ToneWithoutPinMode) {
    // Don't call pinMode - should throw
    EXPECT_ANY_THROW({
        tone(17, 1000);
    });
}

// Test: tone frequency limits
TEST_F(ArduinoCompatExtendedTest, ToneFrequencyLimits) {
    try {
        pinMode(17, OUTPUT);
        
        // Very low frequency
        EXPECT_NO_THROW({
            tone(17, 20);  // 20 Hz
            noTone(17);
        });
        
        // Very high frequency
        EXPECT_NO_THROW({
            tone(17, 20000);  // 20 kHz
            noTone(17);
        });
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: noTone without tone
TEST_F(ArduinoCompatExtendedTest, NoToneWithoutTone) {
    try {
        pinMode(17, OUTPUT);
        
        // Call noTone without calling tone first
        EXPECT_NO_THROW({
            noTone(17);
        });
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Multiple tone calls on same pin
TEST_F(ArduinoCompatExtendedTest, MultipleToneSamePin) {
    try {
        pinMode(17, OUTPUT);
        
        // First tone
        tone(17, 1000);
        std::this_thread::sleep_for(20ms);
        
        // Stop first tone
        noTone(17);
        
        // Reinitialize for second tone
        pinMode(17, OUTPUT);
        
        // Second tone with different frequency
        tone(17, 2000);
        std::this_thread::sleep_for(20ms);
        
        noTone(17);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: tone on multiple pins
TEST_F(ArduinoCompatExtendedTest, ToneMultiplePins) {
    try {
        pinMode(17, OUTPUT);
        pinMode(18, OUTPUT);
        
        // Two tones simultaneously
        tone(17, 1000);
        tone(18, 2000);
        
        std::this_thread::sleep_for(50ms);
        
        noTone(17);
        noTone(18);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Pin reuse after tone
TEST_F(ArduinoCompatExtendedTest, PinReuseAfterTone) {
    try {
        pinMode(17, OUTPUT);
        
        // Use as tone
        tone(17, 1000);
        std::this_thread::sleep_for(20ms);
        noTone(17);
        
        // Reuse as digital output
        digitalWrite(17, HIGH);
        digitalWrite(17, LOW);
        
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}
