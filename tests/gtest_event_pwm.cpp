/**
 * @file gtest_event_pwm.cpp
 * @brief Comprehensive tests for EventPWM class
 * @author Barbatos6669
 * @date 2025-11-16
 */

#include <gtest/gtest.h>
#include "event_pwm.hpp"
#include "exceptions.hpp"
#include <thread>
#include <chrono>
#include <vector>
#include <cmath>

using namespace pipinpp;

// Test fixture for EventPWM tests
class EventPWMTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Tests may skip if GPIO unavailable
    }

    void TearDown() override {
        // Cleanup happens in destructors
    }
};

// ============================================================================
// Constructor & Destructor Tests
// ============================================================================

TEST_F(EventPWMTest, ConstructorValidPin) {
    try {
        EventPWM pwm(17);
        EXPECT_EQ(pwm.getPin(), 17);
        EXPECT_FALSE(pwm.isActive());
        EXPECT_DOUBLE_EQ(pwm.getDutyCycle(), 0.0);
        EXPECT_DOUBLE_EQ(pwm.getFrequency(), 490.0);
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, ConstructorInvalidPin) {
    EXPECT_THROW({
        EventPWM pwm(-1);
    }, InvalidPinError);
    
    EXPECT_THROW({
        EventPWM pwm(28);
    }, InvalidPinError);
    
    EXPECT_THROW({
        EventPWM pwm(999);
    }, InvalidPinError);
}

TEST_F(EventPWMTest, DestructorStopsPWM) {
    try {
        {
            EventPWM pwm(17);
            pwm.begin(1000.0, 50.0);
            EXPECT_TRUE(pwm.isActive());
            // Destructor should stop PWM
        }
        // PWM should be stopped after scope exit
        SUCCEED();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

// ============================================================================
// begin() Method Tests
// ============================================================================

TEST_F(EventPWMTest, BeginWithValidParameters) {
    try {
        EventPWM pwm(17);
        EXPECT_TRUE(pwm.begin(1000.0, 50.0));
        EXPECT_TRUE(pwm.isActive());
        EXPECT_DOUBLE_EQ(pwm.getFrequency(), 1000.0);
        EXPECT_DOUBLE_EQ(pwm.getDutyCycle(), 50.0);
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, BeginFrequencyOutOfRange) {
    try {
        EventPWM pwm(17);
        
        // Too low
        EXPECT_FALSE(pwm.begin(10.0, 50.0));
        EXPECT_FALSE(pwm.isActive());
        
        // Too high
        EXPECT_FALSE(pwm.begin(20000.0, 50.0));
        EXPECT_FALSE(pwm.isActive());
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, BeginDutyCycleOutOfRange) {
    try {
        EventPWM pwm(17);
        
        // Negative
        EXPECT_FALSE(pwm.begin(1000.0, -10.0));
        EXPECT_FALSE(pwm.isActive());
        
        // Too high
        EXPECT_FALSE(pwm.begin(1000.0, 150.0));
        EXPECT_FALSE(pwm.isActive());
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, BeginUpdatesRunningPWM) {
    try {
        EventPWM pwm(17);
        EXPECT_TRUE(pwm.begin(1000.0, 50.0));
        
        // Call begin() again with different parameters
        EXPECT_TRUE(pwm.begin(2000.0, 75.0));
        EXPECT_TRUE(pwm.isActive());
        EXPECT_DOUBLE_EQ(pwm.getFrequency(), 2000.0);
        EXPECT_DOUBLE_EQ(pwm.getDutyCycle(), 75.0);
        
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, BeginEdgeFrequencies) {
    try {
        EventPWM pwm(17);
        
        // Minimum valid frequency
        EXPECT_TRUE(pwm.begin(50.0, 50.0));
        EXPECT_DOUBLE_EQ(pwm.getFrequency(), 50.0);
        pwm.end();
        
        // Maximum valid frequency
        EXPECT_TRUE(pwm.begin(10000.0, 50.0));
        EXPECT_DOUBLE_EQ(pwm.getFrequency(), 10000.0);
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

// ============================================================================
// end() Method Tests
// ============================================================================

TEST_F(EventPWMTest, EndStopsPWM) {
    try {
        EventPWM pwm(17);
        pwm.begin(1000.0, 50.0);
        EXPECT_TRUE(pwm.isActive());
        
        pwm.end();
        EXPECT_FALSE(pwm.isActive());
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, EndSafeWhenNotRunning) {
    try {
        EventPWM pwm(17);
        EXPECT_FALSE(pwm.isActive());
        
        // Should not crash
        pwm.end();
        EXPECT_FALSE(pwm.isActive());
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, EndMultipleTimes) {
    try {
        EventPWM pwm(17);
        pwm.begin(1000.0, 50.0);
        
        pwm.end();
        pwm.end();  // Second call should be safe
        pwm.end();  // Third call should be safe
        
        EXPECT_FALSE(pwm.isActive());
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

// ============================================================================
// setDutyCycle() Method Tests
// ============================================================================

TEST_F(EventPWMTest, SetDutyCycleValidValues) {
    try {
        EventPWM pwm(17);
        
        EXPECT_TRUE(pwm.setDutyCycle(0.0));
        EXPECT_DOUBLE_EQ(pwm.getDutyCycle(), 0.0);
        
        EXPECT_TRUE(pwm.setDutyCycle(25.0));
        EXPECT_DOUBLE_EQ(pwm.getDutyCycle(), 25.0);
        
        EXPECT_TRUE(pwm.setDutyCycle(50.0));
        EXPECT_DOUBLE_EQ(pwm.getDutyCycle(), 50.0);
        
        EXPECT_TRUE(pwm.setDutyCycle(75.0));
        EXPECT_DOUBLE_EQ(pwm.getDutyCycle(), 75.0);
        
        EXPECT_TRUE(pwm.setDutyCycle(100.0));
        EXPECT_DOUBLE_EQ(pwm.getDutyCycle(), 100.0);
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, SetDutyCycleOutOfRange) {
    try {
        EventPWM pwm(17);
        
        // Negative
        EXPECT_FALSE(pwm.setDutyCycle(-10.0));
        
        // Too high
        EXPECT_FALSE(pwm.setDutyCycle(150.0));
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, SetDutyCycleWhileRunning) {
    try {
        EventPWM pwm(17);
        pwm.begin(1000.0, 25.0);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        EXPECT_TRUE(pwm.setDutyCycle(75.0));
        EXPECT_DOUBLE_EQ(pwm.getDutyCycle(), 75.0);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

// ============================================================================
// setDutyCycle8Bit() Method Tests
// ============================================================================

TEST_F(EventPWMTest, SetDutyCycle8BitValidValues) {
    try {
        EventPWM pwm(17);
        
        EXPECT_TRUE(pwm.setDutyCycle8Bit(0));
        EXPECT_NEAR(pwm.getDutyCycle(), 0.0, 0.1);
        
        EXPECT_TRUE(pwm.setDutyCycle8Bit(64));
        EXPECT_NEAR(pwm.getDutyCycle(), 25.1, 0.1);
        
        EXPECT_TRUE(pwm.setDutyCycle8Bit(128));
        EXPECT_NEAR(pwm.getDutyCycle(), 50.2, 0.1);
        
        EXPECT_TRUE(pwm.setDutyCycle8Bit(192));
        EXPECT_NEAR(pwm.getDutyCycle(), 75.3, 0.1);
        
        EXPECT_TRUE(pwm.setDutyCycle8Bit(255));
        EXPECT_NEAR(pwm.getDutyCycle(), 100.0, 0.1);
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, SetDutyCycle8BitEdgeValues) {
    try {
        EventPWM pwm(17);
        
        EXPECT_TRUE(pwm.setDutyCycle8Bit(1));
        EXPECT_NEAR(pwm.getDutyCycle(), 0.39, 0.05);
        
        EXPECT_TRUE(pwm.setDutyCycle8Bit(254));
        EXPECT_NEAR(pwm.getDutyCycle(), 99.61, 0.05);
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

// ============================================================================
// setFrequency() Method Tests
// ============================================================================

TEST_F(EventPWMTest, SetFrequencyValidValues) {
    try {
        EventPWM pwm(17);
        
        EXPECT_TRUE(pwm.setFrequency(100.0));
        EXPECT_DOUBLE_EQ(pwm.getFrequency(), 100.0);
        
        EXPECT_TRUE(pwm.setFrequency(490.0));
        EXPECT_DOUBLE_EQ(pwm.getFrequency(), 490.0);
        
        EXPECT_TRUE(pwm.setFrequency(1000.0));
        EXPECT_DOUBLE_EQ(pwm.getFrequency(), 1000.0);
        
        EXPECT_TRUE(pwm.setFrequency(5000.0));
        EXPECT_DOUBLE_EQ(pwm.getFrequency(), 5000.0);
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, SetFrequencyOutOfRange) {
    try {
        EventPWM pwm(17);
        
        // Too low
        EXPECT_FALSE(pwm.setFrequency(10.0));
        
        // Too high
        EXPECT_FALSE(pwm.setFrequency(20000.0));
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, SetFrequencyWhileRunning) {
    try {
        EventPWM pwm(17);
        pwm.begin(1000.0, 50.0);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        EXPECT_TRUE(pwm.setFrequency(2000.0));
        EXPECT_DOUBLE_EQ(pwm.getFrequency(), 2000.0);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

// ============================================================================
// Edge Case Tests
// ============================================================================

TEST_F(EventPWMTest, DutyCycleZeroPercent) {
    try {
        EventPWM pwm(17);
        pwm.begin(1000.0, 0.0);
        EXPECT_TRUE(pwm.isActive());
        
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, DutyCycle100Percent) {
    try {
        EventPWM pwm(17);
        pwm.begin(1000.0, 100.0);
        EXPECT_TRUE(pwm.isActive());
        
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, DutyCycleNearZero) {
    try {
        EventPWM pwm(17);
        pwm.begin(1000.0, 0.05);  // 0.05% duty cycle
        EXPECT_TRUE(pwm.isActive());
        
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, DutyCycleNear100) {
    try {
        EventPWM pwm(17);
        pwm.begin(1000.0, 99.95);  // 99.95% duty cycle
        EXPECT_TRUE(pwm.isActive());
        
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, HighFrequency) {
    try {
        EventPWM pwm(17);
        pwm.begin(10000.0, 50.0);  // 10kHz max
        EXPECT_TRUE(pwm.isActive());
        
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, LowFrequency) {
    try {
        EventPWM pwm(17);
        pwm.begin(50.0, 50.0);  // 50Hz min
        EXPECT_TRUE(pwm.isActive());
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

// ============================================================================
// Timing & Transitions Tests
// ============================================================================

TEST_F(EventPWMTest, RapidDutyCycleChanges) {
    try {
        EventPWM pwm(17);
        pwm.begin(1000.0, 25.0);
        
        for (int i = 0; i < 10; i++) {
            pwm.setDutyCycle(25.0);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            pwm.setDutyCycle(75.0);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, RapidFrequencyChanges) {
    try {
        EventPWM pwm(17);
        pwm.begin(1000.0, 50.0);
        
        for (int i = 0; i < 10; i++) {
            pwm.setFrequency(500.0);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            pwm.setFrequency(2000.0);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, SmoothDutyCycleRamp) {
    try {
        EventPWM pwm(17);
        pwm.begin(1000.0, 0.0);
        
        // Ramp up
        for (int duty = 0; duty <= 100; duty += 10) {
            pwm.setDutyCycle(static_cast<double>(duty));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        // Ramp down
        for (int duty = 100; duty >= 0; duty -= 10) {
            pwm.setDutyCycle(static_cast<double>(duty));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, StartStopCycles) {
    try {
        EventPWM pwm(17);
        
        for (int i = 0; i < 5; i++) {
            pwm.begin(1000.0, 50.0);
            EXPECT_TRUE(pwm.isActive());
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            
            pwm.end();
            EXPECT_FALSE(pwm.isActive());
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

// ============================================================================
// Multiple Instances Tests
// ============================================================================

TEST_F(EventPWMTest, MultiplePinsSimultaneous) {
    try {
        EventPWM pwm1(17);
        EventPWM pwm2(22);
        EventPWM pwm3(27);
        
        pwm1.begin(1000.0, 25.0);
        pwm2.begin(1000.0, 50.0);
        pwm3.begin(1000.0, 75.0);
        
        EXPECT_TRUE(pwm1.isActive());
        EXPECT_TRUE(pwm2.isActive());
        EXPECT_TRUE(pwm3.isActive());
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        pwm1.end();
        pwm2.end();
        pwm3.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, MultiplePinsDifferentFrequencies) {
    try {
        EventPWM pwm1(17);
        EventPWM pwm2(22);
        
        pwm1.begin(500.0, 50.0);   // 500 Hz
        pwm2.begin(2000.0, 50.0);  // 2000 Hz
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        EXPECT_DOUBLE_EQ(pwm1.getFrequency(), 500.0);
        EXPECT_DOUBLE_EQ(pwm2.getFrequency(), 2000.0);
        
        pwm1.end();
        pwm2.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

// ============================================================================
// Thread Safety Tests
// ============================================================================

TEST_F(EventPWMTest, ConcurrentDutyCycleChanges) {
    try {
        EventPWM pwm(17);
        pwm.begin(1000.0, 50.0);
        
        std::vector<std::thread> threads;
        for (int i = 0; i < 5; i++) {
            threads.emplace_back([&pwm, i]() {
                for (int j = 0; j < 20; j++) {
                    double duty = 10.0 + (i * 15.0);
                    pwm.setDutyCycle(duty);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, ConcurrentFrequencyChanges) {
    try {
        EventPWM pwm(17);
        pwm.begin(1000.0, 50.0);
        
        std::vector<std::thread> threads;
        for (int i = 0; i < 3; i++) {
            threads.emplace_back([&pwm]() {
                for (int j = 0; j < 10; j++) {
                    pwm.setFrequency(500.0 + (j * 100.0));
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

// ============================================================================
// EventPWMManager Tests
// ============================================================================

TEST_F(EventPWMTest, ManagerSingleton) {
    auto& mgr1 = EventPWMManager::getInstance();
    auto& mgr2 = EventPWMManager::getInstance();
    
    EXPECT_EQ(&mgr1, &mgr2);  // Same instance
}

TEST_F(EventPWMTest, ManagerAnalogWriteEvent) {
    try {
        auto& mgr = EventPWMManager::getInstance();
        
        mgr.analogWriteEvent(17, 128, 1000);
        
        EXPECT_TRUE(mgr.isActive(17));
        EXPECT_EQ(mgr.getActiveCount(), 1);
        
        mgr.stopPWM(17);
        EXPECT_FALSE(mgr.isActive(17));
        EXPECT_EQ(mgr.getActiveCount(), 0);
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, ManagerMultipleChannels) {
    try {
        auto& mgr = EventPWMManager::getInstance();
        
        mgr.analogWriteEvent(17, 64, 1000);
        mgr.analogWriteEvent(22, 128, 1000);
        mgr.analogWriteEvent(27, 192, 1000);
        
        EXPECT_TRUE(mgr.isActive(17));
        EXPECT_TRUE(mgr.isActive(22));
        EXPECT_TRUE(mgr.isActive(27));
        EXPECT_EQ(mgr.getActiveCount(), 3);
        
        mgr.stopPWM(22);
        EXPECT_EQ(mgr.getActiveCount(), 2);
        
        mgr.stopPWM(17);
        mgr.stopPWM(27);
        EXPECT_EQ(mgr.getActiveCount(), 0);
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, ManagerUpdateExisting) {
    try {
        auto& mgr = EventPWMManager::getInstance();
        
        mgr.analogWriteEvent(17, 64, 1000);
        EXPECT_EQ(mgr.getActiveCount(), 1);
        
        // Update same pin (should not create new instance)
        mgr.analogWriteEvent(17, 192, 2000);
        EXPECT_EQ(mgr.getActiveCount(), 1);
        
        mgr.stopPWM(17);
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, ManagerClampValues) {
    try {
        auto& mgr = EventPWMManager::getInstance();
        
        // Value clamped to 0-255
        mgr.analogWriteEvent(17, -50, 1000);   // Should clamp to 0
        mgr.analogWriteEvent(22, 500, 1000);   // Should clamp to 255
        
        EXPECT_TRUE(mgr.isActive(17));
        EXPECT_TRUE(mgr.isActive(22));
        
        mgr.stopPWM(17);
        mgr.stopPWM(22);
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, ManagerStopNonexistent) {
    auto& mgr = EventPWMManager::getInstance();
    
    EXPECT_FALSE(mgr.stopPWM(99));  // Pin not active
    EXPECT_FALSE(mgr.isActive(99));
}

// ============================================================================
// Stress Tests
// ============================================================================

TEST_F(EventPWMTest, StressRapidUpdates) {
    try {
        EventPWM pwm(17);
        pwm.begin(1000.0, 50.0);
        
        // 200 rapid updates
        for (int i = 0; i < 200; i++) {
            double duty = 10.0 + (i % 80);
            pwm.setDutyCycle(duty);
        }
        
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, StressSineWavePattern) {
    try {
        EventPWM pwm(17);
        pwm.begin(1000.0, 50.0);
        
        // Generate sine wave duty cycle pattern
        for (int i = 0; i < 100; i++) {
            double angle = (i * 2.0 * M_PI) / 100.0;
            double duty = 50.0 + (50.0 * std::sin(angle));
            pwm.setDutyCycle(duty);
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
        
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}

TEST_F(EventPWMTest, StressLongRunDuration) {
    try {
        EventPWM pwm(17);
        pwm.begin(1000.0, 50.0);
        
        // Run for 1 second
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        
        EXPECT_TRUE(pwm.isActive());
        pwm.end();
    } catch (const GpioAccessError& e) {
        GTEST_SKIP() << "GPIO access unavailable: " << e.what();
    }
}
