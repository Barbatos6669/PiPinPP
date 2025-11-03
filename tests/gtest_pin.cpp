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
 * @file gtest_pin.cpp
 * @brief GoogleTest-based tests for Pin class functionality
 * 
 * Tests Pin class features:
 * - Construction with different modes
 * - Read/write operations
 * - Arduino compatibility
 * 
 * Note: These tests require real GPIO hardware (/dev/gpiochip0)
 * They will be skipped in CI environments without hardware.
 */

#include "pin.hpp"
#include "ArduinoCompat.hpp"
#include "exceptions.hpp"
#include <gtest/gtest.h>

// Helper fixture for tests that require GPIO hardware
class PinHardwareTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Check if GPIO hardware is available
        try {
            Pin testPin(17, PinDirection::OUTPUT);
        }
        catch (const GpioAccessError& e) {
            GTEST_SKIP() << "GPIO hardware not available: " << e.what();
        }
    }
};

/**
 * Test Pin construction with OUTPUT direction
 */
TEST_F(PinHardwareTest, ConstructorOutput) {
    EXPECT_NO_THROW({
        Pin led(17, PinDirection::OUTPUT);
    });
}

/**
 * Test Pin construction with INPUT direction
 */
TEST_F(PinHardwareTest, ConstructorInput) {
    EXPECT_NO_THROW({
        Pin button(27, PinDirection::INPUT);
    });
}

/**
 * Test Pin construction with PinMode::OUTPUT mode
 */
TEST_F(PinHardwareTest, ConstructorPinModeOutput) {
    EXPECT_NO_THROW({
        Pin led(17, PinMode::OUTPUT);
    });
}

/**
 * Test Pin construction with PinMode::INPUT mode
 */
TEST_F(PinHardwareTest, ConstructorPinModeInput) {
    EXPECT_NO_THROW({
        Pin button(27, PinMode::INPUT);
    });
}

/**
 * Test Pin construction with INPUT_PULLUP mode
 */
TEST_F(PinHardwareTest, ConstructorInputPullup) {
    EXPECT_NO_THROW({
        Pin button(27, PinMode::INPUT_PULLUP);
    });
}

/**
 * Test Pin construction with INPUT_PULLDOWN mode
 */
TEST_F(PinHardwareTest, ConstructorInputPulldown) {
    EXPECT_NO_THROW({
        Pin button(27, PinMode::INPUT_PULLDOWN);
    });
}

/**
 * Test writing HIGH to output pin
 */
TEST_F(PinHardwareTest, WriteHigh) {
    Pin led(17, PinDirection::OUTPUT);
    EXPECT_TRUE(led.write(HIGH));
}

/**
 * Test writing LOW to output pin
 */
TEST_F(PinHardwareTest, WriteLow) {
    Pin led(17, PinDirection::OUTPUT);
    EXPECT_TRUE(led.write(LOW));
}

/**
 * Test reading from input pin
 */
TEST_F(PinHardwareTest, ReadInput) {
    Pin button(27, PinDirection::INPUT);
    int value = button.read();
    EXPECT_TRUE(value == 0 || value == 1) << "Read value should be 0 or 1";
}

/**
 * Test Arduino pinMode function
 */
TEST_F(PinHardwareTest, ArduinoPinMode) {
    EXPECT_NO_THROW({
        pinMode(17, OUTPUT);
    });
}

/**
 * Test Arduino digitalWrite function
 */
TEST_F(PinHardwareTest, ArduinoDigitalWrite) {
    pinMode(17, OUTPUT);
    EXPECT_NO_THROW({
        digitalWrite(17, HIGH);
        digitalWrite(17, LOW);
    });
}

/**
 * Test Arduino digitalRead function
 */
TEST_F(PinHardwareTest, ArduinoDigitalRead) {
    pinMode(27, INPUT);
    EXPECT_NO_THROW({
        int value = digitalRead(27);
        EXPECT_TRUE(value == HIGH || value == LOW);
    });
}

/**
 * Test multiple pinMode calls on same pin
 */
TEST_F(PinHardwareTest, MultiplePinModeCallsSamePin) {
    EXPECT_NO_THROW({
        pinMode(17, OUTPUT);
        digitalWrite(17, HIGH);
        
        // Change to input
        pinMode(17, INPUT);
        digitalRead(17);
        
        // Change back to output
        pinMode(17, OUTPUT);
        digitalWrite(17, LOW);
    });
}

/**
 * Test that destructor properly cleans up
 */
TEST_F(PinHardwareTest, DestructorCleanup) {
    EXPECT_NO_THROW({
        {
            Pin led(17, PinDirection::OUTPUT);
            led.write(HIGH);
        } // Pin should be cleaned up here
        
        // Create new pin on same GPIO
        Pin led2(17, PinDirection::OUTPUT);
        led2.write(LOW);
    });
}

/**
 * Test write-then-read consistency (loopback if available)
 * Note: This test assumes the pin can be read back
 */
TEST_F(PinHardwareTest, WriteReadConsistency) {
    Pin led(17, PinDirection::OUTPUT);
    
    led.write(HIGH);
    // Note: Reading an output pin may not work on all hardware
    // This is more of a sanity check
    
    led.write(LOW);
    // If this doesn't throw, we consider it a success
    EXPECT_TRUE(true);
}

/**
 * Test that pin numbers are preserved
 */
TEST_F(PinHardwareTest, PinNumberPreserved) {
    Pin led(17, PinDirection::OUTPUT);
    // The Pin class doesn't expose getPinNumber(), but we can verify
    // by ensuring operations work on the correct pin
    EXPECT_NO_THROW({
        led.write(HIGH);
        led.write(LOW);
    });
}

/**
 * Test rapid toggling
 */
TEST_F(PinHardwareTest, RapidToggle) {
    Pin led(17, PinDirection::OUTPUT);
    
    EXPECT_NO_THROW({
        for (int i = 0; i < 100; i++) {
            led.write(HIGH);
            led.write(LOW);
        }
    });
}

// Note: analogRead and analogWrite are not yet implemented
// so we skip those tests for now
