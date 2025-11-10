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
 * @file gtest_advanced_io.cpp
 * @brief Unit tests for advanced I/O functions (pulseIn, shiftIn/Out, tone)
 * 
 * Tests the Arduino-compatible advanced I/O functions in ArduinoCompat.hpp.
 * Hardware-dependent tests are skipped in CI environments.
 */

#include <gtest/gtest.h>
#include "ArduinoCompat.hpp"
#include "exceptions.hpp"
#include "pwm.hpp"
#include <thread>
#include <chrono>
#include <cmath>

// Test fixture for advanced I/O functions
class AdvancedIOTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Check if GPIO hardware is available
        hasGPIO = (access("/dev/gpiochip0", F_OK) == 0);
    }

    void TearDown() override
    {
        // Clean up any pins that might be in use
        // Note: Pin destructor handles cleanup automatically
    }

    bool hasGPIO = false;
};

// ============================================================================
// pulseIn() Tests
// ============================================================================

TEST_F(AdvancedIOTest, PulseInRequiresInputPin)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available";
    }

    const int TEST_PIN = 17;

    // Configure as OUTPUT - pulseIn should throw
    pinMode(TEST_PIN, OUTPUT);

    EXPECT_THROW(
        {
            pulseIn(TEST_PIN, HIGH, 100000);
        },
        PinError
    );
}

TEST_F(AdvancedIOTest, PulseInInvalidPinThrows)
{
    // Invalid pin number should throw
    EXPECT_THROW(
        {
            pinMode(99, INPUT);
            pulseIn(99, HIGH);
        },
        InvalidPinError
    );
}

TEST_F(AdvancedIOTest, PulseInTimeoutReturnsZero)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available";
    }

    const int TEST_PIN = 17;
    pinMode(TEST_PIN, INPUT);

    // With short timeout and no signal, should return 0
    unsigned long duration = pulseIn(TEST_PIN, HIGH, 1000); // 1ms timeout
    EXPECT_EQ(duration, 0UL);
}

TEST_F(AdvancedIOTest, PulseInLongIdenticalToPulseIn)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available";
    }

    const int TEST_PIN = 17;
    pinMode(TEST_PIN, INPUT);

    // Both should return same value (0 for timeout)
    unsigned long result1 = pulseIn(TEST_PIN, HIGH, 1000);
    unsigned long result2 = pulseInLong(TEST_PIN, HIGH, 1000);
    EXPECT_EQ(result1, result2);
}

// ============================================================================
// shiftOut() Tests
// ============================================================================

TEST_F(AdvancedIOTest, ShiftOutInvalidPinThrows)
{
    // Invalid data pin
    EXPECT_THROW(
        {
            pinMode(99, OUTPUT);
            pinMode(17, OUTPUT);
            shiftOut(99, 17, MSBFIRST, 0xFF);
        },
        InvalidPinError
    );

    // Invalid clock pin
    EXPECT_THROW(
        {
            pinMode(17, OUTPUT);
            pinMode(99, OUTPUT);
            shiftOut(17, 99, MSBFIRST, 0xFF);
        },
        InvalidPinError
    );
}

TEST_F(AdvancedIOTest, ShiftOutRequiresOutputPins)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available";
    }

    const int DATA_PIN = 17;
    const int CLOCK_PIN = 27;

    // Data pin as INPUT should throw
    pinMode(DATA_PIN, INPUT);
    pinMode(CLOCK_PIN, OUTPUT);

    EXPECT_THROW(
        {
            shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0xFF);
        },
        PinError
    );

    // Clock pin as INPUT should throw
    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLOCK_PIN, INPUT);

    EXPECT_THROW(
        {
            shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0xFF);
        },
        PinError
    );
}

TEST_F(AdvancedIOTest, ShiftOutMSBFIRSTBasic)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available";
    }

    const int DATA_PIN = 17;
    const int CLOCK_PIN = 27;

    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);

    // Should not throw
    EXPECT_NO_THROW(
        {
            shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0b10101010);
        }
    );
}

TEST_F(AdvancedIOTest, ShiftOutLSBFIRSTBasic)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available";
    }

    const int DATA_PIN = 17;
    const int CLOCK_PIN = 27;

    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);

    // Should not throw
    EXPECT_NO_THROW(
        {
            shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, 0b10101010);
        }
    );
}

// ============================================================================
// shiftIn() Tests
// ============================================================================

TEST_F(AdvancedIOTest, ShiftInInvalidPinThrows)
{
    // Invalid data pin
    EXPECT_THROW(
        {
            pinMode(99, INPUT);
            pinMode(17, OUTPUT);
            shiftIn(99, 17, MSBFIRST);
        },
        InvalidPinError
    );

    // Invalid clock pin
    EXPECT_THROW(
        {
            pinMode(17, INPUT);
            pinMode(99, OUTPUT);
            shiftIn(17, 99, MSBFIRST);
        },
        InvalidPinError
    );
}

TEST_F(AdvancedIOTest, ShiftInRequiresCorrectPinModes)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available";
    }

    const int DATA_PIN = 17;
    const int CLOCK_PIN = 27;

    // Data pin as OUTPUT should throw
    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);

    EXPECT_THROW(
        {
            shiftIn(DATA_PIN, CLOCK_PIN, MSBFIRST);
        },
        PinError
    );

    // Clock pin as INPUT should throw
    pinMode(DATA_PIN, INPUT);
    pinMode(CLOCK_PIN, INPUT);

    EXPECT_THROW(
        {
            shiftIn(DATA_PIN, CLOCK_PIN, MSBFIRST);
        },
        PinError
    );
}

TEST_F(AdvancedIOTest, ShiftInMSBFIRSTBasic)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available";
    }

    const int DATA_PIN = 17;
    const int CLOCK_PIN = 27;

    pinMode(DATA_PIN, INPUT);
    pinMode(CLOCK_PIN, OUTPUT);

    // Should not throw, returns some value
    unsigned char result = 0;
    EXPECT_NO_THROW(
        {
            result = shiftIn(DATA_PIN, CLOCK_PIN, MSBFIRST);
        }
    );

    // Result is valid (0-255)
    EXPECT_GE(result, 0);
    EXPECT_LE(result, 255);
}

TEST_F(AdvancedIOTest, ShiftInLSBFIRSTBasic)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available";
    }

    const int DATA_PIN = 17;
    const int CLOCK_PIN = 27;

    pinMode(DATA_PIN, INPUT);
    pinMode(CLOCK_PIN, OUTPUT);

    // Should not throw
    unsigned char result = 0;
    EXPECT_NO_THROW(
        {
            result = shiftIn(DATA_PIN, CLOCK_PIN, LSBFIRST);
        }
    );

    EXPECT_GE(result, 0);
    EXPECT_LE(result, 255);
}

// ============================================================================
// Shift Register Loopback Test (shiftOut → shiftIn)
// ============================================================================

TEST_F(AdvancedIOTest, ShiftOutInLoopback)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available (requires loopback wiring)";
    }

    // This test requires physical loopback:
    // Connect DATA_OUT to DATA_IN (e.g., GPIO17 to GPIO22)
    // Connect CLOCK_OUT to CLOCK_IN (e.g., GPIO27 to GPIO23)
    
    GTEST_SKIP() << "Loopback test requires specific hardware wiring";

    const int DATA_OUT = 17;
    const int CLOCK_OUT = 27;
    const int DATA_IN = 22;
    const int CLOCK_IN = 23;

    pinMode(DATA_OUT, OUTPUT);
    pinMode(CLOCK_OUT, OUTPUT);
    pinMode(DATA_IN, INPUT);
    pinMode(CLOCK_IN, INPUT);

    // Test pattern
    unsigned char testValue = 0b10110100;

    // Shift out
    shiftOut(DATA_OUT, CLOCK_OUT, MSBFIRST, testValue);

    // Small delay
    delayMicroseconds(100);

    // Shift in
    unsigned char received = shiftIn(DATA_IN, CLOCK_IN, MSBFIRST);

    EXPECT_EQ(received, testValue);
}

// ============================================================================
// tone() Tests
// ============================================================================

TEST_F(AdvancedIOTest, ToneInvalidPinThrows)
{
    EXPECT_THROW(
        {
            pinMode(99, OUTPUT);
            tone(99, 1000);
        },
        InvalidPinError
    );
}

TEST_F(AdvancedIOTest, ToneRequiresOutputPin)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available";
    }

    const int TEST_PIN = 17;
    pinMode(TEST_PIN, INPUT);

    EXPECT_THROW(
        {
            tone(TEST_PIN, 1000);
        },
        PinError
    );
}

TEST_F(AdvancedIOTest, ToneBasicOperation)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available";
    }

    const int TEST_PIN = 18;  // Use pin 18 to avoid conflicts with pin 17 used in other tests
    pinMode(TEST_PIN, OUTPUT);

    // Start tone - should not throw
    EXPECT_NO_THROW(
        {
            tone(TEST_PIN, 1000); // 1kHz
        }
    );

    // Verify PWM is active (frequency = 1000 Hz)
    auto& pwmMgr = PWMManager::getInstance();
    EXPECT_TRUE(pwmMgr.isActive(TEST_PIN));

    // Stop tone
    noTone(TEST_PIN);
    EXPECT_FALSE(pwmMgr.isActive(TEST_PIN));
}

TEST_F(AdvancedIOTest, ToneWithDuration)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available";
    }

    const int TEST_PIN = 23;  // Use pin 23 to avoid conflicts
    pinMode(TEST_PIN, OUTPUT);

    auto& pwmMgr = PWMManager::getInstance();
    
    // Start tone with 100ms duration (this blocks for the duration)
    EXPECT_NO_THROW(
        {
            tone(TEST_PIN, 1000, 100);
        }
    );

    // After tone() returns with duration, PWM should be stopped
    EXPECT_FALSE(pwmMgr.isActive(TEST_PIN));
}

TEST_F(AdvancedIOTest, ToneFrequencyRange)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available";
    }

    const int TEST_PIN = 24;  // Use pin 24 to avoid conflicts
    pinMode(TEST_PIN, OUTPUT);

    // Test various frequencies
    std::vector<unsigned int> frequencies = {100, 440, 1000, 2000, 5000};

    for (unsigned int freq : frequencies)
    {
        EXPECT_NO_THROW(
            {
                tone(TEST_PIN, freq);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                noTone(TEST_PIN);
            }
        ) << "Failed at frequency: " << freq;
    }
}

TEST_F(AdvancedIOTest, ToneZeroFrequencyThrows)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available";
    }

    const int TEST_PIN = 25;  // Use pin 25 to avoid conflicts
    pinMode(TEST_PIN, OUTPUT);

    // Zero frequency should throw
    EXPECT_THROW(
        {
            tone(TEST_PIN, 0);
        },
        std::invalid_argument
    );
}

// ============================================================================
// noTone() Tests
// ============================================================================

TEST_F(AdvancedIOTest, NoToneInvalidPinThrows)
{
    EXPECT_THROW(
        {
            noTone(99);
        },
        InvalidPinError
    );
}

TEST_F(AdvancedIOTest, NoToneStopsTone)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available";
    }

    const int TEST_PIN = 12;  // Use pin 12 to avoid conflicts
    pinMode(TEST_PIN, OUTPUT);

    // Start tone
    tone(TEST_PIN, 1000);

    auto& pwmMgr = PWMManager::getInstance();
    EXPECT_TRUE(pwmMgr.isActive(TEST_PIN));

    // Stop tone
    EXPECT_NO_THROW(
        {
            noTone(TEST_PIN);
        }
    );

    EXPECT_FALSE(pwmMgr.isActive(TEST_PIN));
}

TEST_F(AdvancedIOTest, NoToneOnInactivePin)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available";
    }

    const int TEST_PIN = 16;  // Use pin 16 to avoid conflicts
    pinMode(TEST_PIN, OUTPUT);

    // Call noTone without active tone - should not throw
    EXPECT_NO_THROW(
        {
            noTone(TEST_PIN);
        }
    );
}

// ============================================================================
// Multiple Tone Test
// ============================================================================

TEST_F(AdvancedIOTest, MultipleTonesDifferentPins)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available";
    }

    const int PIN1 = 20;  // Use pin 20 to avoid conflicts
    const int PIN2 = 21;  // Use pin 21 to avoid conflicts

    pinMode(PIN1, OUTPUT);
    pinMode(PIN2, OUTPUT);

    // Start tones on both pins
    EXPECT_NO_THROW(
        {
            tone(PIN1, 1000);
            tone(PIN2, 2000);
        }
    );

    auto& pwmMgr = PWMManager::getInstance();
    EXPECT_TRUE(pwmMgr.isActive(PIN1));
    EXPECT_TRUE(pwmMgr.isActive(PIN2));

    // Stop one
    noTone(PIN1);
    EXPECT_FALSE(pwmMgr.isActive(PIN1));
    EXPECT_TRUE(pwmMgr.isActive(PIN2));

    // Stop other
    noTone(PIN2);
    EXPECT_FALSE(pwmMgr.isActive(PIN2));
}

// ============================================================================
// Bit Order Constants Test
// ============================================================================

TEST(AdvancedIOConstants, BitOrderValues)
{
    // Verify bit order constants are defined correctly
    EXPECT_EQ(LSBFIRST, 0);
    EXPECT_EQ(MSBFIRST, 1);
}

// ============================================================================
// Integration Test: Musical Scale
// ============================================================================

TEST_F(AdvancedIOTest, MusicalScaleSequence)
{
    if (!hasGPIO)
    {
        GTEST_SKIP() << "GPIO hardware not available";
    }

    const int BUZZER_PIN = 13;  // Use pin 13 to avoid conflicts
    pinMode(BUZZER_PIN, OUTPUT);

    // Musical notes (C4, D4, E4, F4, G4, A4, B4, C5)
    unsigned int scale[] = {262, 294, 330, 349, 392, 440, 494, 523};

    // Play each note briefly
    for (unsigned int freq : scale)
    {
        EXPECT_NO_THROW(
            {
                tone(BUZZER_PIN, freq);
                delay(50); // 50ms per note
                noTone(BUZZER_PIN);
                delay(10); // 10ms gap
            }
        ) << "Failed at frequency: " << freq;
    }
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
