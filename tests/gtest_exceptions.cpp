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
 * @file gtest_exceptions.cpp
 * @brief GoogleTest-based tests for exception handling
 * 
 * Tests the custom exception hierarchy:
 * - PinError (base class)
 * - InvalidPinError (invalid pin numbers)
 * - GpioAccessError (GPIO access failures)
 */

#include "pin.hpp"
#include "ArduinoCompat.hpp"
#include "exceptions.hpp"
#include <gtest/gtest.h>

/**
 * Test that InvalidPinError is thrown for out-of-range pin numbers
 */
TEST(ExceptionTest, InvalidPinNumber) {
    EXPECT_THROW({
        Pin invalidPin(999, PinDirection::OUTPUT);
    }, InvalidPinError);
}

/**
 * Test that GpioAccessError is thrown for invalid chip path
 */
TEST(ExceptionTest, InvalidChipPath) {
    EXPECT_THROW({
        Pin invalidChip(17, PinDirection::OUTPUT, "/dev/nonexistent");
    }, GpioAccessError);
}

/**
 * Test that negative pin numbers throw InvalidPinError
 */
TEST(ExceptionTest, NegativePinNumber) {
    EXPECT_THROW({
        Pin invalidPin(-1, PinDirection::OUTPUT);
    }, InvalidPinError);
}

/**
 * Test Arduino digitalWrite without pinMode
 */
TEST(ExceptionTest, DigitalWriteWithoutPinMode) {
    EXPECT_THROW({
        digitalWrite(17, HIGH);
    }, InvalidPinError);
}

/**
 * Test Arduino digitalRead without pinMode
 */
TEST(ExceptionTest, DigitalReadWithoutPinMode) {
    EXPECT_THROW({
        digitalRead(27);
    }, InvalidPinError);
}

/**
 * Test exception hierarchy - InvalidPinError can be caught as PinError
 */
TEST(ExceptionTest, InvalidPinErrorAsPinError) {
    try {
        Pin invalidPin(999, PinDirection::OUTPUT);
        FAIL() << "Expected InvalidPinError to be thrown";
    }
    catch (const PinError& e) {
        // Success - caught as base class
        EXPECT_NE(std::string(e.what()).find("Invalid"), std::string::npos) 
            << "Error message should contain 'Invalid'";
    }
    catch (...) {
        FAIL() << "Expected PinError base class to catch InvalidPinError";
    }
}

/**
 * Test exception hierarchy - GpioAccessError can be caught as PinError
 */
TEST(ExceptionTest, GpioAccessErrorAsPinError) {
    try {
        Pin invalidChip(17, PinDirection::OUTPUT, "/dev/nonexistent");
        FAIL() << "Expected GpioAccessError to be thrown";
    }
    catch (const PinError& e) {
        // Success - caught as base class
        EXPECT_TRUE(true);
    }
    catch (...) {
        FAIL() << "Expected PinError base class to catch GpioAccessError";
    }
}

/**
 * Test that exception messages contain useful information
 */
TEST(ExceptionTest, ExceptionMessagesAreUseful) {
    try {
        Pin invalidPin(999, PinDirection::OUTPUT);
        FAIL() << "Expected exception to be thrown";
    }
    catch (const InvalidPinError& e) {
        std::string message = e.what();
        EXPECT_FALSE(message.empty()) << "Exception message should not be empty";
        EXPECT_NE(message.find("999"), std::string::npos) 
            << "Exception message should contain the invalid pin number";
    }
}

/**
 * Test that valid pins don't throw exceptions
 */
TEST(ExceptionTest, ValidPinDoesNotThrow) {
    // Note: This test requires /dev/gpiochip0 to exist
    // It will fail in CI environments without GPIO hardware
    // But it's good to have for local testing
    EXPECT_NO_THROW({
        try {
            Pin validPin(17, PinDirection::OUTPUT);
        }
        catch (const GpioAccessError& e) {
            // If we can't access GPIO hardware, skip this test
            GTEST_SKIP() << "GPIO hardware not available: " << e.what();
        }
    });
}

/**
 * Test exception with Arduino pinMode - invalid pin
 */
TEST(ExceptionTest, ArduinoPinModeInvalidPin) {
    EXPECT_THROW({
        pinMode(999, OUTPUT);
    }, InvalidPinError);
}

/**
 * Test that all exceptions inherit from std::exception
 */
TEST(ExceptionTest, ExceptionsInheritFromStdException) {
    try {
        Pin invalidPin(999, PinDirection::OUTPUT);
        FAIL() << "Expected exception to be thrown";
    }
    catch (const std::exception& e) {
        // Success - all our exceptions should be catchable as std::exception
        EXPECT_NE(std::string(e.what()).length(), 0);
    }
}
