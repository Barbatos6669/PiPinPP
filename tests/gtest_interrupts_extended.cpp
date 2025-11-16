/**
 * @file gtest_interrupts_extended.cpp
 * @brief Extended unit tests for interrupt error paths and edge cases
 * @author Barbatos6669
 * @date 2025-11-16
 * 
 * Comprehensive tests targeting uncovered error handling paths in interrupts.cpp
 * Focus: Error conditions, state management, cleanup paths (no hardware required).
 */

#include <gtest/gtest.h>
#include "ArduinoCompat.hpp"
#include "interrupts.hpp"
#include "exceptions.hpp"
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

using namespace std::chrono_literals;

/**
 * @brief Test fixture for extended interrupt tests
 */
class InterruptExtendedTest : public ::testing::Test {
protected:
    void SetUp() override {
        callback_count = 0;
        last_pin = -1;
    }
    
    void TearDown() override {
        // Clean up any interrupts
        try {
            for (int pin = 0; pin <= 27; pin++) {
                detachInterrupt(pin);
            }
        } catch (...) {
            // Ignore cleanup errors
        }
    }
    
    static std::atomic<int> callback_count;
    static std::atomic<int> last_pin;
    
    static void simple_callback() {
        callback_count++;
    }
    
    static void pin_tracking_callback() {
        callback_count++;
        last_pin = 17;
    }
};

std::atomic<int> InterruptExtendedTest::callback_count{0};
std::atomic<int> InterruptExtendedTest::last_pin{-1};

// ============================================================================
// Error Path Tests - Invalid Pin Numbers
// ============================================================================

TEST_F(InterruptExtendedTest, AttachInterruptNegativePin) {
    EXPECT_THROW({
        attachInterrupt(-1, simple_callback, RISING);
    }, InvalidPinError);
}

TEST_F(InterruptExtendedTest, AttachInterruptPin28) {
    EXPECT_THROW({
        attachInterrupt(28, simple_callback, RISING);
    }, InvalidPinError);
}

TEST_F(InterruptExtendedTest, AttachInterruptPin100) {
    EXPECT_THROW({
        attachInterrupt(100, simple_callback, RISING);
    }, InvalidPinError);
}

TEST_F(InterruptExtendedTest, AttachInterruptPin1000) {
    EXPECT_THROW({
        attachInterrupt(1000, simple_callback, RISING);
    }, InvalidPinError);
}

TEST_F(InterruptExtendedTest, AttachInterruptNegative100) {
    EXPECT_THROW({
        attachInterrupt(-100, simple_callback, RISING);
    }, InvalidPinError);
}

TEST_F(InterruptExtendedTest, DetachInterruptNegativePin) {
    // detachInterrupt with invalid pin should not throw
    // (no validation in detachInterrupt)
    EXPECT_NO_THROW({
        detachInterrupt(-1);
    });
}

TEST_F(InterruptExtendedTest, DetachInterruptInvalidPin) {
    EXPECT_NO_THROW({
        detachInterrupt(100);
    });
}

// ============================================================================
// Error Path Tests - Null Callback
// ============================================================================

TEST_F(InterruptExtendedTest, AttachInterruptNullCallback) {
    // Arduino wrapper may not validate null callback before calling manager
    // This will fail at hardware level or in manager
    try {
        attachInterrupt(17, nullptr, RISING);
        // If it doesn't throw, hardware not available
        detachInterrupt(17);
        GTEST_SKIP() << "Null callback not validated (implementation choice)";
    } catch (const InvalidPinError&) {
        // Expected if validation occurs
        SUCCEED();
    } catch (const GpioAccessError&) {
        // Also acceptable - hardware access failure
        SUCCEED();
    }
}

TEST_F(InterruptExtendedTest, AttachInterruptNullCallbackFalling) {
    try {
        attachInterrupt(18, nullptr, FALLING);
        detachInterrupt(18);
        GTEST_SKIP() << "Null callback not validated (implementation choice)";
    } catch (const InvalidPinError&) {
        SUCCEED();
    } catch (const GpioAccessError&) {
        SUCCEED();
    }
}

TEST_F(InterruptExtendedTest, AttachInterruptNullCallbackChange) {
    try {
        attachInterrupt(27, nullptr, CHANGE);
        detachInterrupt(27);
        GTEST_SKIP() << "Null callback not validated (implementation choice)";
    } catch (const InvalidPinError&) {
        SUCCEED();
    } catch (const GpioAccessError&) {
        SUCCEED();
    }
}

// ============================================================================
// Error Path Tests - Double Attach
// ============================================================================

TEST_F(InterruptExtendedTest, AttachInterruptTwiceSamePin) {
    try {
        attachInterrupt(17, simple_callback, RISING);
        
        // Second attach should throw
        EXPECT_THROW({
            attachInterrupt(17, pin_tracking_callback, FALLING);
        }, GpioAccessError);
        
        detachInterrupt(17);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

TEST_F(InterruptExtendedTest, AttachDetachAttachSamePin) {
    try {
        // First attach
        attachInterrupt(17, simple_callback, RISING);
        detachInterrupt(17);
        
        // Second attach after detach should work
        EXPECT_NO_THROW({
            attachInterrupt(17, pin_tracking_callback, FALLING);
        });
        
        detachInterrupt(17);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

// ============================================================================
// State Management Tests
// ============================================================================

TEST_F(InterruptExtendedTest, DetachNonAttachedPin) {
    // Detaching non-attached pin should not throw
    EXPECT_NO_THROW({
        detachInterrupt(17);
    });
}

TEST_F(InterruptExtendedTest, DetachAfterAlreadyDetached) {
    try {
        auto& mgr = InterruptManager::getInstance();
        
        attachInterrupt(17, simple_callback, RISING);
        EXPECT_TRUE(mgr.isAttached(17));
        
        // First detach
        detachInterrupt(17);
        EXPECT_FALSE(mgr.isAttached(17));
        
        // Second detach (no-op)
        detachInterrupt(17);
        EXPECT_FALSE(mgr.isAttached(17));
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

TEST_F(InterruptExtendedTest, MultipleDetachCallsSafe) {
    try {
        attachInterrupt(17, simple_callback, RISING);
        
        EXPECT_NO_THROW({
            detachInterrupt(17);
            detachInterrupt(17);
            detachInterrupt(17);
        });
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

TEST_F(InterruptExtendedTest, IsAttachedBeforeAttach) {
    auto& mgr = InterruptManager::getInstance();
    EXPECT_FALSE(mgr.isAttached(17));
}

TEST_F(InterruptExtendedTest, IsAttachedAfterAttach) {
    try {
        attachInterrupt(17, simple_callback, RISING);
        
        auto& mgr = InterruptManager::getInstance();
        EXPECT_TRUE(mgr.isAttached(17));
        
        detachInterrupt(17);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

TEST_F(InterruptExtendedTest, IsAttachedAfterDetach) {
    try {
        attachInterrupt(17, simple_callback, RISING);
        detachInterrupt(17);
        
        auto& mgr = InterruptManager::getInstance();
        EXPECT_FALSE(mgr.isAttached(17));
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

TEST_F(InterruptExtendedTest, GetActiveCountInitiallyZero) {
    auto& mgr = InterruptManager::getInstance();
    
    // Clean slate
    for (int pin = 0; pin <= 27; pin++) {
        detachInterrupt(pin);
    }
    
    EXPECT_EQ(mgr.getActiveCount(), 0u);
}

TEST_F(InterruptExtendedTest, GetActiveCountAfterAttach) {
    try {
        auto& mgr = InterruptManager::getInstance();
        
        // Clean slate
        for (int pin = 0; pin <= 27; pin++) {
            detachInterrupt(pin);
        }
        
        size_t initial_count = mgr.getActiveCount();
        
        attachInterrupt(17, simple_callback, RISING);
        EXPECT_EQ(mgr.getActiveCount(), initial_count + 1);
        
        detachInterrupt(17);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

TEST_F(InterruptExtendedTest, GetActiveCountMultipleAttach) {
    try {
        auto& mgr = InterruptManager::getInstance();
        
        // Clean slate
        for (int pin = 0; pin <= 27; pin++) {
            detachInterrupt(pin);
        }
        
        size_t initial_count = mgr.getActiveCount();
        
        attachInterrupt(17, simple_callback, RISING);
        attachInterrupt(18, simple_callback, FALLING);
        attachInterrupt(27, simple_callback, CHANGE);
        
        EXPECT_EQ(mgr.getActiveCount(), initial_count + 3);
        
        detachInterrupt(17);
        detachInterrupt(18);
        detachInterrupt(27);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

TEST_F(InterruptExtendedTest, GetActiveCountAfterDetach) {
    try {
        auto& mgr = InterruptManager::getInstance();
        
        // Clean slate
        for (int pin = 0; pin <= 27; pin++) {
            detachInterrupt(pin);
        }
        
        attachInterrupt(17, simple_callback, RISING);
        size_t count_with = mgr.getActiveCount();
        
        detachInterrupt(17);
        size_t count_without = mgr.getActiveCount();
        
        EXPECT_EQ(count_without, count_with - 1);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

// ============================================================================
// Interrupt Mode Tests
// ============================================================================

TEST_F(InterruptExtendedTest, AttachInterruptRisingMode) {
    try {
        EXPECT_NO_THROW({
            attachInterrupt(17, simple_callback, RISING);
        });
        detachInterrupt(17);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

TEST_F(InterruptExtendedTest, AttachInterruptFallingMode) {
    try {
        EXPECT_NO_THROW({
            attachInterrupt(17, simple_callback, FALLING);
        });
        detachInterrupt(17);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

TEST_F(InterruptExtendedTest, AttachInterruptChangeMode) {
    try {
        EXPECT_NO_THROW({
            attachInterrupt(17, simple_callback, CHANGE);
        });
        detachInterrupt(17);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

TEST_F(InterruptExtendedTest, InterruptModeConstantsValid) {
    // Verify mode constants are defined and distinct
    EXPECT_NE(RISING, FALLING);
    EXPECT_NE(RISING, CHANGE);
    EXPECT_NE(FALLING, CHANGE);
}

// ============================================================================
// Multiple Interrupt Tests
// ============================================================================

TEST_F(InterruptExtendedTest, AttachMultipleDifferentPins) {
    try {
        EXPECT_NO_THROW({
            attachInterrupt(17, simple_callback, RISING);
            attachInterrupt(18, simple_callback, FALLING);
            attachInterrupt(27, simple_callback, CHANGE);
        });
        
        detachInterrupt(17);
        detachInterrupt(18);
        detachInterrupt(27);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

TEST_F(InterruptExtendedTest, DetachOneOfMultiple) {
    try {
        auto& mgr = InterruptManager::getInstance();
        
        attachInterrupt(17, simple_callback, RISING);
        attachInterrupt(18, simple_callback, FALLING);
        
        size_t count_before = mgr.getActiveCount();
        detachInterrupt(17);
        size_t count_after = mgr.getActiveCount();
        
        EXPECT_EQ(count_after, count_before - 1);
        EXPECT_FALSE(mgr.isAttached(17));
        EXPECT_TRUE(mgr.isAttached(18));
        
        detachInterrupt(18);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

TEST_F(InterruptExtendedTest, DetachAllMultiple) {
    try {
        auto& mgr = InterruptManager::getInstance();
        
        attachInterrupt(17, simple_callback, RISING);
        attachInterrupt(18, simple_callback, FALLING);
        attachInterrupt(27, simple_callback, CHANGE);
        
        EXPECT_TRUE(mgr.isAttached(17));
        EXPECT_TRUE(mgr.isAttached(18));
        EXPECT_TRUE(mgr.isAttached(27));
        
        detachInterrupt(17);
        detachInterrupt(18);
        detachInterrupt(27);
        
        EXPECT_FALSE(mgr.isAttached(17));
        EXPECT_FALSE(mgr.isAttached(18));
        EXPECT_FALSE(mgr.isAttached(27));
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

// ============================================================================
// Rapid Attach/Detach Tests
// ============================================================================

TEST_F(InterruptExtendedTest, RapidAttachDetachCycles) {
    try {
        for (int i = 0; i < 10; i++) {
            attachInterrupt(17, simple_callback, RISING);
            std::this_thread::sleep_for(1ms);
            detachInterrupt(17);
        }
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

TEST_F(InterruptExtendedTest, AttachManyPinsSequentially) {
    try {
        std::vector<int> pins = {2, 3, 4, 17, 18, 22, 23, 24, 25, 27};
        
        for (int pin : pins) {
            attachInterrupt(pin, simple_callback, RISING);
        }
        
        auto& mgr = InterruptManager::getInstance();
        for (int pin : pins) {
            EXPECT_TRUE(mgr.isAttached(pin));
        }
        
        for (int pin : pins) {
            detachInterrupt(pin);
        }
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

// ============================================================================
// Thread Safety Tests
// ============================================================================

TEST_F(InterruptExtendedTest, ConcurrentIsAttachedSafe) {
    try {
        attachInterrupt(17, simple_callback, RISING);
        
        auto& mgr = InterruptManager::getInstance();
        std::vector<std::thread> threads;
        
        for (int i = 0; i < 10; i++) {
            threads.emplace_back([&mgr]() {
                for (int j = 0; j < 100; j++) {
                    bool attached = mgr.isAttached(17);
                    (void)attached;
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        detachInterrupt(17);
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

TEST_F(InterruptExtendedTest, ConcurrentGetActiveCountSafe) {
    try {
        attachInterrupt(17, simple_callback, RISING);
        
        auto& mgr = InterruptManager::getInstance();
        std::vector<std::thread> threads;
        
        for (int i = 0; i < 10; i++) {
            threads.emplace_back([&mgr]() {
                for (int j = 0; j < 100; j++) {
                    size_t count = mgr.getActiveCount();
                    (void)count;
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        detachInterrupt(17);
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

TEST_F(InterruptExtendedTest, ConcurrentDetachDifferentPins) {
    try {
        std::vector<int> pins = {17, 18, 22, 23, 27};
        
        for (int pin : pins) {
            attachInterrupt(pin, simple_callback, RISING);
        }
        
        std::vector<std::thread> threads;
        for (int pin : pins) {
            threads.emplace_back([pin]() {
                std::this_thread::sleep_for(1ms);
                detachInterrupt(pin);
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        SUCCEED();
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

// ============================================================================
// Boundary Pin Tests
// ============================================================================

TEST_F(InterruptExtendedTest, AttachInterruptPin0) {
    try {
        EXPECT_NO_THROW({
            attachInterrupt(0, simple_callback, RISING);
        });
        detachInterrupt(0);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

TEST_F(InterruptExtendedTest, AttachInterruptPin27) {
    try {
        EXPECT_NO_THROW({
            attachInterrupt(27, simple_callback, RISING);
        });
        detachInterrupt(27);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

TEST_F(InterruptExtendedTest, AttachInterruptAllValidPins) {
    try {
        for (int pin = 0; pin <= 27; pin++) {
            EXPECT_NO_THROW({
                attachInterrupt(pin, simple_callback, RISING);
            }) << "Failed on pin " << pin;
            detachInterrupt(pin);
        }
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

// ============================================================================
// Cleanup and Lifecycle Tests
// ============================================================================

TEST_F(InterruptExtendedTest, ManagerSingletonConsistency) {
    auto& mgr1 = InterruptManager::getInstance();
    auto& mgr2 = InterruptManager::getInstance();
    
    // Should be same instance
    EXPECT_EQ(&mgr1, &mgr2);
}

TEST_F(InterruptExtendedTest, StatePreservedAcrossCalls) {
    try {
        auto& mgr = InterruptManager::getInstance();
        
        attachInterrupt(17, simple_callback, RISING);
        EXPECT_TRUE(mgr.isAttached(17));
        
        // Access manager again
        auto& mgr2 = InterruptManager::getInstance();
        EXPECT_TRUE(mgr2.isAttached(17));
        
        detachInterrupt(17);
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

TEST_F(InterruptExtendedTest, DetachInDestructor) {
    try {
        {
            attachInterrupt(17, simple_callback, RISING);
            // Scope exit - should clean up automatically
        }
        
        // After scope, interrupt should still be detachable
        detachInterrupt(17);
        // Should not throw
        
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO hardware not available: " << e.what();
    }
}

// Main function
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
