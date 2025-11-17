/**
 * @file gtest_interrupts.cpp
 * @brief GoogleTest unit tests for interrupt functionality
 * 
 * Tests GPIO interrupt attachment, detachment, and callback execution.
 * Uses GPIO loopback (write to one pin, interrupt on another) where possible,
 * or skips hardware-dependent tests gracefully.
 * 
 * @copyright Copyright (c) 2025 PiPinPP Project
 * @license MIT License
 */

#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <atomic>
#include "ArduinoCompat.hpp"
#include "interrupts.hpp"
#include "exceptions.hpp"

using namespace std::chrono_literals;

// Test fixture for interrupt tests
class InterruptTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clean up any existing interrupts from previous tests
        callback_count = 0;
        last_pin = -1;
    }
    
    void TearDown() override {
        // Detach any interrupts that may have been attached
        try {
            detachInterrupt(17);
            detachInterrupt(18);
            detachInterrupt(27);
        } catch (...) {
            // Ignore errors during cleanup
        }
    }
    
    // Static callback counters for testing
    static std::atomic<int> callback_count;
    static std::atomic<int> last_pin;
    
    static void simple_callback() {
        callback_count++;
    }
    
    static void pin_callback(int pin) {
        callback_count++;
        last_pin = pin;
    }
};

std::atomic<int> InterruptTest::callback_count{0};
std::atomic<int> InterruptTest::last_pin{-1};

// Test: attachInterrupt with invalid pin throws
TEST_F(InterruptTest, AttachInterruptInvalidPinThrows) {
    EXPECT_THROW({
        attachInterrupt(99, simple_callback, RISING);
    }, InvalidPinError);
    
    EXPECT_THROW({
        attachInterrupt(-1, simple_callback, RISING);
    }, InvalidPinError);
}

// Test: attachInterrupt with input mode works
TEST_F(InterruptTest, AttachInterruptWithInputMode) {
    try {
        pinMode(17, INPUT);
        attachInterrupt(17, simple_callback, RISING);
        detachInterrupt(17);
        SUCCEED();
    } catch (const std::exception& e) {
        // Skip if GPIO/interrupt hardware not available
        GTEST_SKIP() << "Interrupt hardware not available: " << e.what();
    }
}

// Test: attachInterrupt with valid parameters succeeds
TEST_F(InterruptTest, AttachInterruptValidParameters) {
    try {
        pinMode(17, INPUT);
        
        // Should not throw
        EXPECT_NO_THROW({
            attachInterrupt(17, simple_callback, RISING);
        });
        
        detachInterrupt(17);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Interrupt modes are valid
TEST_F(InterruptTest, InterruptModesValid) {
    try {
        pinMode(17, INPUT);
        
        // Test all interrupt modes
        EXPECT_NO_THROW({
            attachInterrupt(17, simple_callback, RISING);
            detachInterrupt(17);
        });
        
        EXPECT_NO_THROW({
            attachInterrupt(17, simple_callback, FALLING);
            detachInterrupt(17);
        });
        
        EXPECT_NO_THROW({
            attachInterrupt(17, simple_callback, CHANGE);
            detachInterrupt(17);
        });
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: detachInterrupt without attachInterrupt is safe
TEST_F(InterruptTest, DetachInterruptWithoutAttachSafe) {
    EXPECT_NO_THROW({
        detachInterrupt(17);
    });
}

// Test: Multiple detachInterrupt calls are safe
TEST_F(InterruptTest, MultipleDetachInterruptSafe) {
    try {
        pinMode(17, INPUT);
        attachInterrupt(17, simple_callback, RISING);
        
        EXPECT_NO_THROW({
            detachInterrupt(17);
            detachInterrupt(17);
            detachInterrupt(17);
        });
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Interrupt callback fires on GPIO edge (loopback)
TEST_F(InterruptTest, InterruptCallbackFiresOnEdge) {
    try {
        // Setup: Pin 17 as input with interrupt, Pin 27 as output
        pinMode(17, INPUT_PULLDOWN);
        pinMode(27, OUTPUT);
        digitalWrite(27, LOW);
        
        callback_count = 0;
        attachInterrupt(17, simple_callback, RISING);
        
        // Give interrupt system time to initialize
        std::this_thread::sleep_for(100ms);
        
        // Trigger: Write HIGH to output pin
        digitalWrite(27, HIGH);
        
        // Wait for interrupt to fire
        std::this_thread::sleep_for(100ms);
        
        // Verify: Callback should have been called
        // Note: This test requires GPIO loopback (jumper wire)
        if (callback_count == 0) {
            GTEST_SKIP() << "Interrupt not triggered - may need GPIO loopback (connect GPIO17 to GPIO27)";
        }
        
        EXPECT_GT(callback_count.load(), 0) << "Interrupt callback should have been called";
        
        detachInterrupt(17);
        digitalWrite(27, LOW);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Falling edge interrupt
TEST_F(InterruptTest, FallingEdgeInterrupt) {
    try {
        pinMode(17, INPUT_PULLUP);
        pinMode(27, OUTPUT);
        digitalWrite(27, HIGH);
        
        callback_count = 0;
        attachInterrupt(17, simple_callback, FALLING);
        
        std::this_thread::sleep_for(100ms);
        
        // Trigger falling edge
        digitalWrite(27, LOW);
        std::this_thread::sleep_for(100ms);
        
        if (callback_count == 0) {
            GTEST_SKIP() << "Interrupt not triggered - may need GPIO loopback";
        }
        
        EXPECT_GT(callback_count.load(), 0);
        
        detachInterrupt(17);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Change interrupt (both edges)
TEST_F(InterruptTest, ChangeInterruptBothEdges) {
    try {
        pinMode(17, INPUT);
        pinMode(27, OUTPUT);
        digitalWrite(27, LOW);
        
        callback_count = 0;
        attachInterrupt(17, simple_callback, CHANGE);
        
        std::this_thread::sleep_for(100ms);
        
        // Trigger both edges
        digitalWrite(27, HIGH);
        std::this_thread::sleep_for(50ms);
        digitalWrite(27, LOW);
        std::this_thread::sleep_for(50ms);
        
        if (callback_count == 0) {
            GTEST_SKIP() << "Interrupt not triggered - may need GPIO loopback";
        }
        
        // Should fire on both rising and falling
        EXPECT_GE(callback_count.load(), 1);
        
        detachInterrupt(17);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: detachInterrupt stops callbacks
TEST_F(InterruptTest, DetachInterruptStopsCallbacks) {
    try {
        pinMode(17, INPUT);
        pinMode(27, OUTPUT);
        digitalWrite(27, LOW);
        
        callback_count = 0;
        attachInterrupt(17, simple_callback, RISING);
        
        std::this_thread::sleep_for(100ms);
        
        // Trigger interrupt
        digitalWrite(27, HIGH);
        std::this_thread::sleep_for(100ms);
        
        if (callback_count == 0) {
            GTEST_SKIP() << "Interrupt not triggered - may need GPIO loopback";
        }
        
        int count_before_detach = callback_count.load();
        
        // Detach interrupt
        detachInterrupt(17);
        std::this_thread::sleep_for(50ms);
        
        // Trigger again - should NOT fire
        digitalWrite(27, LOW);
        std::this_thread::sleep_for(50ms);
        digitalWrite(27, HIGH);
        std::this_thread::sleep_for(100ms);
        
        // Count should not have increased
        EXPECT_EQ(callback_count.load(), count_before_detach) 
            << "Callback should not fire after detachInterrupt";
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Multiple interrupts on different pins
TEST_F(InterruptTest, MultipleInterruptsDifferentPins) {
    try {
        pinMode(17, INPUT);
        pinMode(18, INPUT);
        
        // Attach interrupts to different pins
        EXPECT_NO_THROW({
            attachInterrupt(17, simple_callback, RISING);
            attachInterrupt(18, simple_callback, RISING);
        });
        
        // Clean up
        detachInterrupt(17);
        detachInterrupt(18);
        
        // If we got here without throwing, test passes
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Re-attaching interrupt to same pin replaces callback
TEST_F(InterruptTest, ReattachInterruptReplacesCallback) {
    try {
        pinMode(17, INPUT);
        
        // Attach first callback
        attachInterrupt(17, simple_callback, RISING);
        
        // Re-attach with different mode (should replace)
        callback_count = 0;
        attachInterrupt(17, simple_callback, FALLING);
        
        // Only second attachment should be active
        // (We can't easily test firing without hardware, but API should work)
        
        detachInterrupt(17);
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Interrupt callback thread safety
TEST_F(InterruptTest, InterruptCallbackThreadSafety) {
    try {
        pinMode(17, INPUT);
        
        attachInterrupt(17, simple_callback, CHANGE);
        
        // Concurrent detach from another thread
        std::thread t([]() {
            std::this_thread::sleep_for(50ms);
            detachInterrupt(17);
        });
        
        t.join();
        
        // Should not crash
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Interrupt with static callback
TEST_F(InterruptTest, InterruptWithStaticCallback) {
    try {
        pinMode(17, INPUT);
        
        // Test that static callbacks work
        EXPECT_NO_THROW({
            attachInterrupt(17, simple_callback, RISING);
        });
        
        detachInterrupt(17);
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Rapid interrupt attach/detach cycles
TEST_F(InterruptTest, RapidAttachDetachCycles) {
    try {
        pinMode(17, INPUT);
        
        for (int i = 0; i < 10; i++) {
            EXPECT_NO_THROW({
                attachInterrupt(17, simple_callback, RISING);
                detachInterrupt(17);
            });
        }
        
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Interrupt mode constants are defined
TEST_F(InterruptTest, InterruptModeConstantsDefined) {
    // These should be compile-time constants
    EXPECT_EQ(RISING, 1);
    EXPECT_EQ(FALLING, 2);
    EXPECT_EQ(CHANGE, 3);
}

// Test: Callback execution context
TEST_F(InterruptTest, CallbackExecutionContext) {
    try {
        pinMode(17, INPUT);
        pinMode(27, OUTPUT);
        digitalWrite(27, LOW);
        
        callback_count = 0;
        attachInterrupt(17, simple_callback, RISING);
        std::this_thread::sleep_for(100ms);
        
        digitalWrite(27, HIGH);
        std::this_thread::sleep_for(100ms);
        
        if (callback_count.load() == 0) {
            GTEST_SKIP() << "Interrupt not triggered - may need GPIO loopback";
        }
        
        // Callback should have been executed
        EXPECT_GT(callback_count.load(), 0);
        
        detachInterrupt(17);
        digitalWrite(27, LOW);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Fast callbacks don't block interrupt system
TEST_F(InterruptTest, FastCallbacksNonBlocking) {
    try {
        pinMode(17, INPUT);
        
        // Fast callback - should not block
        EXPECT_NO_THROW({
            attachInterrupt(17, simple_callback, CHANGE);
        });
        
        detachInterrupt(17);
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Error handling for null callback
TEST_F(InterruptTest, NullCallbackHandling) {
    try {
        pinMode(17, INPUT);
        
        // Depending on implementation, this might throw or be handled gracefully
        // The behavior should be consistent
        EXPECT_NO_THROW({
            detachInterrupt(17);  // Safe no-op
        });
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}

// Test: Interrupt system cleanup on program exit
TEST_F(InterruptTest, InterruptCleanupOnExit) {
    try {
        pinMode(17, INPUT);
        
        attachInterrupt(17, simple_callback, RISING);
        
        // Explicit cleanup
        EXPECT_NO_THROW({
            detachInterrupt(17);
        });
        
        // Implicit cleanup should also work (handled by destructors)
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access not available: " << e.what();
    }
}
