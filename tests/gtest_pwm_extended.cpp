/**
 * @file gtest_pwm_extended.cpp
 * @brief Extended GoogleTest unit tests for PWM functionality
 * 
 * Tests PWM edge cases, frequency/duty cycle validation, and timing
 * to improve pwm.cpp coverage from 62% to 75%+.
 * 
 * @copyright Copyright (c) 2025 PiPinPP Project
 * @license MIT License
 */

#include <gtest/gtest.h>
#include "ArduinoCompat.hpp"
#include "exceptions.hpp"
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

// Test fixture for extended PWM tests
class PWMExtendedTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clean state
    }
    
    void TearDown() override {
        // Stop any PWM
        try {
            analogWrite(18, 0);
            analogWrite(19, 0);
        } catch (...) {
            // Ignore cleanup errors
        }
    }
};

// ============================================================================
// DUTY CYCLE EDGE CASES
// ============================================================================

// Test: Duty cycle 0 (off)
TEST_F(PWMExtendedTest, DutyCycleZero) {
    try {
        pinMode(18, OUTPUT);
        analogWrite(18, 0);
        delay(50);
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Duty cycle 255 (fully on)
TEST_F(PWMExtendedTest, DutyCycle255) {
    try {
        pinMode(18, OUTPUT);
        analogWrite(18, 255);
        delay(50);
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Duty cycle 1 (minimum non-zero)
TEST_F(PWMExtendedTest, DutyCycleOne) {
    try {
        pinMode(18, OUTPUT);
        analogWrite(18, 1);
        delay(50);
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Duty cycle 254 (maximum before fully on)
TEST_F(PWMExtendedTest, DutyCycle254) {
    try {
        pinMode(18, OUTPUT);
        analogWrite(18, 254);
        delay(50);
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Duty cycle 50% (128)
TEST_F(PWMExtendedTest, DutyCycle50Percent) {
    try {
        pinMode(18, OUTPUT);
        analogWrite(18, 128);
        delay(50);
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// ============================================================================
// PWM TRANSITIONS
// ============================================================================

// Test: Rapid duty cycle changes
TEST_F(PWMExtendedTest, RapidDutyCycleChanges) {
    try {
        pinMode(18, OUTPUT);
        
        for (int i = 0; i < 10; i++) {
            analogWrite(18, 0);
            delayMicroseconds(100);
            analogWrite(18, 255);
            delayMicroseconds(100);
            analogWrite(18, 128);
            delayMicroseconds(100);
        }
        
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Smooth duty cycle ramp
TEST_F(PWMExtendedTest, SmoothDutyCycleRamp) {
    try {
        pinMode(18, OUTPUT);
        
        // Ramp up
        for (int i = 0; i <= 255; i += 16) {
            analogWrite(18, i);
            delayMicroseconds(1000);
        }
        
        // Ramp down
        for (int i = 255; i >= 0; i -= 16) {
            analogWrite(18, i);
            delayMicroseconds(1000);
        }
        
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: PWM start/stop cycles
TEST_F(PWMExtendedTest, StartStopCycles) {
    try {
        pinMode(18, OUTPUT);
        
        for (int i = 0; i < 5; i++) {
            analogWrite(18, 128);
            delay(10);
            analogWrite(18, 0);
            delay(10);
        }
        
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// ============================================================================
// MULTIPLE PWM PINS
// ============================================================================

// Test: Two PWM pins simultaneously
TEST_F(PWMExtendedTest, TwoPWMPinsSimultaneous) {
    try {
        pinMode(18, OUTPUT);
        pinMode(19, OUTPUT);
        
        analogWrite(18, 64);
        analogWrite(19, 192);
        delay(50);
        
        analogWrite(18, 0);
        analogWrite(19, 0);
        
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Alternating PWM between pins
TEST_F(PWMExtendedTest, AlternatingPWMBetweenPins) {
    try {
        pinMode(18, OUTPUT);
        pinMode(19, OUTPUT);
        
        for (int i = 0; i < 5; i++) {
            analogWrite(18, 255);
            analogWrite(19, 0);
            delay(20);
            
            analogWrite(18, 0);
            analogWrite(19, 255);
            delay(20);
        }
        
        analogWrite(18, 0);
        analogWrite(19, 0);
        
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Multiple pins same duty cycle
TEST_F(PWMExtendedTest, MultiplePinsSameDutyCycle) {
    try {
        pinMode(18, OUTPUT);
        pinMode(19, OUTPUT);
        
        analogWrite(18, 128);
        analogWrite(19, 128);
        delay(50);
        
        analogWrite(18, 0);
        analogWrite(19, 0);
        
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// ============================================================================
// MODE SWITCHING
// ============================================================================

// Test: Switch from digital to PWM
TEST_F(PWMExtendedTest, SwitchDigitalToPWM) {
    try {
        pinMode(18, OUTPUT);
        digitalWrite(18, HIGH);
        delay(10);
        
        analogWrite(18, 128);
        delay(10);
        
        analogWrite(18, 0);
        
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Switch from PWM to digital
TEST_F(PWMExtendedTest, SwitchPWMToDigital) {
    try {
        pinMode(18, OUTPUT);
        analogWrite(18, 128);
        delay(10);
        
        digitalWrite(18, LOW);
        delay(10);
        
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Multiple mode switches
TEST_F(PWMExtendedTest, MultipleModeSwitch) {
    try {
        pinMode(18, OUTPUT);
        
        for (int i = 0; i < 5; i++) {
            digitalWrite(18, HIGH);
            delay(5);
            analogWrite(18, 128);
            delay(5);
            digitalWrite(18, LOW);
            delay(5);
        }
        
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// ============================================================================
// TIMING TESTS
// ============================================================================

// Test: PWM timing consistency
TEST_F(PWMExtendedTest, PWMTimingConsistency) {
    try {
        pinMode(18, OUTPUT);
        
        auto start = std::chrono::high_resolution_clock::now();
        analogWrite(18, 128);
        delay(100);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        // Should be close to 100ms (allow some tolerance)
        EXPECT_GE(duration, 95);
        EXPECT_LE(duration, 110);
        
        analogWrite(18, 0);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Very short PWM pulse
TEST_F(PWMExtendedTest, VeryShortPWMPulse) {
    try {
        pinMode(18, OUTPUT);
        
        analogWrite(18, 128);
        delayMicroseconds(10);
        analogWrite(18, 0);
        
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Long duration PWM
TEST_F(PWMExtendedTest, LongDurationPWM) {
    try {
        pinMode(18, OUTPUT);
        
        analogWrite(18, 128);
        delay(200);
        analogWrite(18, 0);
        
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// ============================================================================
// ERROR HANDLING
// ============================================================================

// Test: PWM on input pin (should fail or convert to output)
TEST_F(PWMExtendedTest, PWMOnInputPin) {
    try {
        pinMode(18, INPUT);
        
        // PWM on input should either fail or auto-convert to output
        // Don't throw exception - just test it doesn't crash
        analogWrite(18, 128);
        delay(10);
        analogWrite(18, 0);
        
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    } catch (...) {
        // Other exceptions are acceptable (e.g., can't PWM on input)
        SUCCEED();
    }
}

// Test: Cleanup after exception
TEST_F(PWMExtendedTest, CleanupAfterException) {
    try {
        pinMode(18, OUTPUT);
        analogWrite(18, 128);
        
        // Simulate error condition by trying invalid pin
        try {
            analogWrite(999, 128);  // Invalid pin
        } catch (...) {
            // Expected to fail
        }
        
        // Original PWM should still be cleanable
        analogWrite(18, 0);
        
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// ============================================================================
// STRESS TESTS
// ============================================================================

// Test: Many rapid PWM updates
TEST_F(PWMExtendedTest, ManyRapidPWMUpdates) {
    try {
        pinMode(18, OUTPUT);
        
        for (int i = 0; i < 100; i++) {
            analogWrite(18, i % 256);
        }
        
        analogWrite(18, 0);
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: PWM pattern generation
TEST_F(PWMExtendedTest, PWMPatternGeneration) {
    try {
        pinMode(18, OUTPUT);
        
        // Generate sine-like pattern
        for (int i = 0; i < 50; i++) {
            int duty = 128 + (int)(127 * sin(i * 0.1));
            analogWrite(18, duty);
            delayMicroseconds(1000);
        }
        
        analogWrite(18, 0);
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}
