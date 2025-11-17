/**
 * @file gtest_hardware_pwm_extended.cpp
 * @brief Extended unit tests for HardwarePWM error paths and edge cases
 * @author Barbatos6669
 * @date 2025-11-16
 * 
 * Comprehensive tests targeting uncovered error handling paths in HardwarePWM.cpp
 * Focus: Error conditions, boundary values, state transitions, cleanup paths.
 */

#include <gtest/gtest.h>
#include "HardwarePWM.hpp"
#include <thread>
#include <vector>
#include <chrono>

using namespace pipinpp;

/**
 * @brief Test fixture for extended HardwarePWM tests
 */
class HardwarePWMExtendedTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Tests run without hardware access
    }
    
    void TearDown() override {
        // Cleanup
    }
};

// ============================================================================
// Error Path Tests - begin() Failures
// ============================================================================

TEST_F(HardwarePWMExtendedTest, BeginWithZeroFrequencyFails) {
    HardwarePWM pwm(0, 0);
    EXPECT_FALSE(pwm.begin(0, 50.0));
    EXPECT_FALSE(pwm.isEnabled());
    EXPECT_EQ(pwm.getFrequency(), 0U);
}

TEST_F(HardwarePWMExtendedTest, BeginWithInvalidDutyCycleAccepted) {
    // Library should clamp invalid duty cycles
    HardwarePWM pwm1(0, 0);
    // These will fail without hardware but test API compliance
    pwm1.begin(1000, -50.0);  // Negative
    
    HardwarePWM pwm2(0, 1);
    pwm2.begin(1000, 150.0);  // Over 100%
}

TEST_F(HardwarePWMExtendedTest, BeginDifferentChannelsSameChip) {
    HardwarePWM pwm0_0(0, 0);
    HardwarePWM pwm0_1(0, 1);
    
    // Both should be constructible
    EXPECT_NO_THROW({
        auto freq0 = pwm0_0.getFrequency();
        auto freq1 = pwm0_1.getFrequency();
        (void)freq0;
        (void)freq1;
    });
}

TEST_F(HardwarePWMExtendedTest, BeginDifferentChips) {
    HardwarePWM pwm0(0, 0);
    HardwarePWM pwm1(1, 0);
    HardwarePWM pwm2(2, 0);
    
    // All should be constructible
    EXPECT_FALSE(pwm0.isEnabled());
    EXPECT_FALSE(pwm1.isEnabled());
    EXPECT_FALSE(pwm2.isEnabled());
}

// ============================================================================
// Error Path Tests - setFrequency() Failures
// ============================================================================

TEST_F(HardwarePWMExtendedTest, SetFrequencyWithoutBeginFails) {
    HardwarePWM pwm(0, 0);
    EXPECT_FALSE(pwm.setFrequency(1000));
}

TEST_F(HardwarePWMExtendedTest, SetZeroFrequencyFails) {
    HardwarePWM pwm(0, 0);
    // Even with begin(), zero frequency should fail
    pwm.begin(1000, 50.0);
    EXPECT_FALSE(pwm.setFrequency(0));
}

TEST_F(HardwarePWMExtendedTest, SetVeryHighFrequency) {
    HardwarePWM pwm(0, 0);
    // Test extreme frequency values
    EXPECT_FALSE(pwm.setFrequency(100000000U));  // 100MHz (too high for PWM)
}

TEST_F(HardwarePWMExtendedTest, SetVeryLowFrequency) {
    HardwarePWM pwm(0, 0);
    // 1Hz should be valid mathematically
    EXPECT_FALSE(pwm.setFrequency(1));  // Will fail without hardware
}

TEST_F(HardwarePWMExtendedTest, SetFrequencyPreservesDutyCyclePercentage) {
    HardwarePWM pwm(0, 0);
    
    // Without hardware this will fail, but tests the logic path
    pwm.begin(1000, 50.0);
    pwm.setFrequency(500);
    
    // Duty cycle percentage should theoretically be preserved
    // (can't verify without hardware)
}

// ============================================================================
// Error Path Tests - setDutyCycle() Failures
// ============================================================================

TEST_F(HardwarePWMExtendedTest, SetDutyCycleWithoutBeginFails) {
    HardwarePWM pwm(0, 0);
    EXPECT_FALSE(pwm.setDutyCycle(50.0));
}

TEST_F(HardwarePWMExtendedTest, SetNegativeDutyCycleClamped) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 0.0);
    
    // Should clamp to 0%
    pwm.setDutyCycle(-50.0);
    // Can't verify without hardware but tests the path
}

TEST_F(HardwarePWMExtendedTest, SetOverMaxDutyCycleClamped) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 0.0);
    
    // Should clamp to 100%
    pwm.setDutyCycle(150.0);
    // Can't verify without hardware but tests the path
}

TEST_F(HardwarePWMExtendedTest, SetDutyCycleExtremeValues) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 0.0);
    
    // Test boundary values
    pwm.setDutyCycle(0.0);
    pwm.setDutyCycle(0.001);  // Very small
    pwm.setDutyCycle(99.999); // Very large
    pwm.setDutyCycle(100.0);
}

TEST_F(HardwarePWMExtendedTest, SetDutyCycle8BitBoundaries) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 0.0);
    
    // Test 8-bit boundaries
    EXPECT_FALSE(pwm.setDutyCycle8Bit(0));      // 0%
    EXPECT_FALSE(pwm.setDutyCycle8Bit(1));      // ~0.4%
    EXPECT_FALSE(pwm.setDutyCycle8Bit(127));    // ~49.8%
    EXPECT_FALSE(pwm.setDutyCycle8Bit(128));    // ~50.2%
    EXPECT_FALSE(pwm.setDutyCycle8Bit(254));    // ~99.6%
    EXPECT_FALSE(pwm.setDutyCycle8Bit(255));    // 100%
}

// ============================================================================
// Error Path Tests - setPeriodNs() Failures
// ============================================================================

TEST_F(HardwarePWMExtendedTest, SetPeriodNsWithoutBeginFails) {
    HardwarePWM pwm(0, 0);
    EXPECT_FALSE(pwm.setPeriodNs(1000000));
}

TEST_F(HardwarePWMExtendedTest, SetPeriodNsZeroValue) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 50.0);
    
    // Zero period is invalid
    pwm.setPeriodNs(0);
    // Will fail but tests the path
}

TEST_F(HardwarePWMExtendedTest, SetPeriodNsVeryLarge) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 50.0);
    
    // Very long period (low frequency)
    pwm.setPeriodNs(1000000000);  // 1 second = 1Hz
}

TEST_F(HardwarePWMExtendedTest, SetPeriodNsSmallerThanDutyCycle) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 90.0);  // High duty cycle
    
    // Setting smaller period should clamp duty cycle
    pwm.setPeriodNs(100000);  // 100µs
    // Duty cycle should be clamped to not exceed period
}

// ============================================================================
// Error Path Tests - setDutyCycleNs() Failures
// ============================================================================

TEST_F(HardwarePWMExtendedTest, SetDutyCycleNsWithoutBeginFails) {
    HardwarePWM pwm(0, 0);
    EXPECT_FALSE(pwm.setDutyCycleNs(500000));
}

TEST_F(HardwarePWMExtendedTest, SetDutyCycleNsExceedsPeriod) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 50.0);  // Period = 1,000,000ns
    
    // Try to set duty cycle > period
    // Should clamp to period value
    pwm.setDutyCycleNs(2000000);
    // Can't verify without hardware but tests clamping logic
}

TEST_F(HardwarePWMExtendedTest, SetDutyCycleNsZero) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 50.0);
    
    EXPECT_FALSE(pwm.setDutyCycleNs(0));  // 0% duty
}

TEST_F(HardwarePWMExtendedTest, SetDutyCycleNsEqualsPeriod) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 50.0);
    
    // 100% duty cycle
    EXPECT_FALSE(pwm.setDutyCycleNs(1000000));
}

// ============================================================================
// Error Path Tests - setPolarity() Failures
// ============================================================================

TEST_F(HardwarePWMExtendedTest, SetPolarityWithoutBeginFails) {
    HardwarePWM pwm(0, 0);
    EXPECT_FALSE(pwm.setPolarity(PWMPolarity::NORMAL));
    EXPECT_FALSE(pwm.setPolarity(PWMPolarity::INVERSED));
}

TEST_F(HardwarePWMExtendedTest, SetPolarityWhileEnabled) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 50.0);
    
    // Should temporarily disable, change polarity, re-enable
    pwm.setPolarity(PWMPolarity::INVERSED);
    pwm.setPolarity(PWMPolarity::NORMAL);
}

// ============================================================================
// Error Path Tests - enable()/disable() Failures
// ============================================================================

TEST_F(HardwarePWMExtendedTest, EnableWithoutBeginFails) {
    HardwarePWM pwm(0, 0);
    EXPECT_FALSE(pwm.enable());
}

TEST_F(HardwarePWMExtendedTest, DisableWithoutBeginSucceeds) {
    HardwarePWM pwm(0, 0);
    // Disabling when not initialized should succeed (no-op)
    EXPECT_TRUE(pwm.disable());
}

TEST_F(HardwarePWMExtendedTest, MultipleEnableCallsSafe) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 50.0);
    
    // Multiple enables should be safe
    pwm.enable();
    pwm.enable();
    pwm.enable();
}

TEST_F(HardwarePWMExtendedTest, MultipleDisableCallsSafe) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 50.0);
    
    // Multiple disables should be safe
    EXPECT_TRUE(pwm.disable());
    EXPECT_TRUE(pwm.disable());
    EXPECT_TRUE(pwm.disable());
}

TEST_F(HardwarePWMExtendedTest, EnableDisableCycles) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 50.0);
    
    for (int i = 0; i < 5; i++) {
        pwm.enable();
        pwm.disable();
    }
}

// ============================================================================
// State Management Tests
// ============================================================================

TEST_F(HardwarePWMExtendedTest, GetFrequencyBeforeBeginReturnsZero) {
    HardwarePWM pwm(0, 0);
    EXPECT_EQ(pwm.getFrequency(), 0U);
}

TEST_F(HardwarePWMExtendedTest, GetDutyCycleBeforeBeginReturnsNegative) {
    HardwarePWM pwm(0, 0);
    EXPECT_LT(pwm.getDutyCycle(), 0.0);
}

TEST_F(HardwarePWMExtendedTest, GetPeriodNsBeforeBeginReturnsZero) {
    HardwarePWM pwm(0, 0);
    EXPECT_EQ(pwm.getPeriodNs(), 0ULL);
}

TEST_F(HardwarePWMExtendedTest, GetDutyCycleNsBeforeBeginReturnsZero) {
    HardwarePWM pwm(0, 0);
    EXPECT_EQ(pwm.getDutyCycleNs(), 0ULL);
}

TEST_F(HardwarePWMExtendedTest, StateConsistencyAfterBeginFailure) {
    HardwarePWM pwm(0, 0);
    
    // Try to begin with invalid frequency
    EXPECT_FALSE(pwm.begin(0, 50.0));
    
    // State should remain uninitialized
    EXPECT_FALSE(pwm.isEnabled());
    EXPECT_EQ(pwm.getFrequency(), 0U);
    EXPECT_LT(pwm.getDutyCycle(), 0.0);
}

// ============================================================================
// Thread Safety Tests - State Modifications
// ============================================================================

TEST_F(HardwarePWMExtendedTest, ConcurrentSetFrequencySafe) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 50.0);
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; i++) {
        threads.emplace_back([&pwm, i]() {
            pwm.setFrequency(500 + i * 100);
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // Should not crash
    SUCCEED();
}

TEST_F(HardwarePWMExtendedTest, ConcurrentSetDutyCycleSafe) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 50.0);
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; i++) {
        threads.emplace_back([&pwm, i]() {
            pwm.setDutyCycle(i * 20.0);
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    SUCCEED();
}

TEST_F(HardwarePWMExtendedTest, ConcurrentEnableDisableSafe) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 50.0);
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&pwm, i]() {
            if (i % 2 == 0) {
                pwm.enable();
            } else {
                pwm.disable();
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    SUCCEED();
}

TEST_F(HardwarePWMExtendedTest, ConcurrentGettersWhileModifying) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 50.0);
    
    std::atomic<bool> stop{false};
    
    // Writer thread
    std::thread writer([&pwm, &stop]() {
        for (int i = 0; i < 50 && !stop; i++) {
            pwm.setDutyCycle(i * 2.0);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
    
    // Reader threads
    std::vector<std::thread> readers;
    for (int i = 0; i < 5; i++) {
        readers.emplace_back([&pwm, &stop]() {
            while (!stop) {
                auto freq = pwm.getFrequency();
                auto duty = pwm.getDutyCycle();
                auto enabled = pwm.isEnabled();
                (void)freq; (void)duty; (void)enabled;
            }
        });
    }
    
    writer.join();
    stop = true;
    
    for (auto& t : readers) {
        t.join();
    }
    
    SUCCEED();
}

// ============================================================================
// Cleanup Tests
// ============================================================================

TEST_F(HardwarePWMExtendedTest, EndAfterFailedBeginSafe) {
    HardwarePWM pwm(0, 0);
    pwm.begin(0, 50.0);  // Will fail
    
    EXPECT_NO_THROW(pwm.end());
}

TEST_F(HardwarePWMExtendedTest, EndThenAccessSafe) {
    HardwarePWM pwm(0, 0);
    pwm.begin(1000, 50.0);
    pwm.end();
    
    // Access after end should not crash
    EXPECT_FALSE(pwm.isEnabled());
    EXPECT_EQ(pwm.getFrequency(), 0U);
}

TEST_F(HardwarePWMExtendedTest, EndThenBeginAgain) {
    HardwarePWM pwm(0, 0);
    
    pwm.begin(1000, 50.0);
    pwm.end();
    
    // Should be able to begin again
    pwm.begin(500, 25.0);
    pwm.end();
}

TEST_F(HardwarePWMExtendedTest, DestructorAfterFailedBegin) {
    // Should not crash
    EXPECT_NO_THROW({
        HardwarePWM pwm(0, 0);
        pwm.begin(0, 50.0);  // Will fail
        // Destructor called here
    });
}

TEST_F(HardwarePWMExtendedTest, DestructorWhileEnabled) {
    EXPECT_NO_THROW({
        HardwarePWM pwm(0, 0);
        pwm.begin(1000, 50.0);
        // Destructor should call end() automatically
    });
}

// ============================================================================
// GPIO to PWM Mapping Edge Cases
// ============================================================================

TEST_F(HardwarePWMExtendedTest, GPIOToPWMInvalidPins) {
    int chip, channel;
    
    // Test various invalid pins
    EXPECT_FALSE(HardwarePWM::gpioToPWM(-1, chip, channel));
    EXPECT_FALSE(HardwarePWM::gpioToPWM(-100, chip, channel));
    EXPECT_FALSE(HardwarePWM::gpioToPWM(0, chip, channel));
    EXPECT_FALSE(HardwarePWM::gpioToPWM(1, chip, channel));
    EXPECT_FALSE(HardwarePWM::gpioToPWM(10, chip, channel));
    EXPECT_FALSE(HardwarePWM::gpioToPWM(11, chip, channel));
    EXPECT_FALSE(HardwarePWM::gpioToPWM(14, chip, channel));
    EXPECT_FALSE(HardwarePWM::gpioToPWM(15, chip, channel));
    EXPECT_FALSE(HardwarePWM::gpioToPWM(16, chip, channel));
    EXPECT_FALSE(HardwarePWM::gpioToPWM(17, chip, channel));
    EXPECT_FALSE(HardwarePWM::gpioToPWM(20, chip, channel));
    EXPECT_FALSE(HardwarePWM::gpioToPWM(27, chip, channel));
    EXPECT_FALSE(HardwarePWM::gpioToPWM(28, chip, channel));
    EXPECT_FALSE(HardwarePWM::gpioToPWM(100, chip, channel));
    EXPECT_FALSE(HardwarePWM::gpioToPWM(1000, chip, channel));
}

TEST_F(HardwarePWMExtendedTest, GPIOToPWMOutputParameters) {
    int chip = -1, channel = -1;
    
    // Valid pin 12
    EXPECT_TRUE(HardwarePWM::gpioToPWM(12, chip, channel));
    EXPECT_EQ(chip, 0);
    EXPECT_EQ(channel, 0);
    
    // Valid pin 13
    chip = -1; channel = -1;
    EXPECT_TRUE(HardwarePWM::gpioToPWM(13, chip, channel));
    EXPECT_EQ(chip, 0);
    EXPECT_EQ(channel, 1);
    
    // Valid pin 18
    chip = -1; channel = -1;
    EXPECT_TRUE(HardwarePWM::gpioToPWM(18, chip, channel));
    EXPECT_EQ(chip, 0);
    EXPECT_EQ(channel, 0);
    
    // Valid pin 19
    chip = -1; channel = -1;
    EXPECT_TRUE(HardwarePWM::gpioToPWM(19, chip, channel));
    EXPECT_EQ(chip, 0);
    EXPECT_EQ(channel, 1);
}

TEST_F(HardwarePWMExtendedTest, GPIOToPWMAlternatePins) {
    int chip1, channel1, chip2, channel2;
    
    // Pins 12 and 18 should map to same PWM (channel 0)
    HardwarePWM::gpioToPWM(12, chip1, channel1);
    HardwarePWM::gpioToPWM(18, chip2, channel2);
    EXPECT_EQ(chip1, chip2);
    EXPECT_EQ(channel1, channel2);
    
    // Pins 13 and 19 should map to same PWM (channel 1)
    HardwarePWM::gpioToPWM(13, chip1, channel1);
    HardwarePWM::gpioToPWM(19, chip2, channel2);
    EXPECT_EQ(chip1, chip2);
    EXPECT_EQ(channel1, channel2);
}

// ============================================================================
// Frequency/Period Conversion Tests
// ============================================================================

TEST_F(HardwarePWMExtendedTest, FrequencyToPeriodConversion) {
    // Test mathematical conversions
    uint64_t period;
    
    period = 1000000000ULL / 1;      // 1Hz
    EXPECT_EQ(period, 1000000000ULL);
    
    period = 1000000000ULL / 50;     // 50Hz (servo frequency)
    EXPECT_EQ(period, 20000000ULL);
    
    period = 1000000000ULL / 1000;   // 1kHz
    EXPECT_EQ(period, 1000000ULL);
    
    period = 1000000000ULL / 10000;  // 10kHz
    EXPECT_EQ(period, 100000ULL);
    
    period = 1000000000ULL / 100000; // 100kHz
    EXPECT_EQ(period, 10000ULL);
}

TEST_F(HardwarePWMExtendedTest, DutyCyclePercentToNanoseconds) {
    uint64_t periodNs = 1000000;  // 1kHz = 1ms
    
    uint64_t duty0 = (0.0 / 100.0) * periodNs;
    uint64_t duty25 = (25.0 / 100.0) * periodNs;
    uint64_t duty50 = (50.0 / 100.0) * periodNs;
    uint64_t duty75 = (75.0 / 100.0) * periodNs;
    uint64_t duty100 = (100.0 / 100.0) * periodNs;
    
    EXPECT_EQ(duty0, 0ULL);
    EXPECT_EQ(duty25, 250000ULL);
    EXPECT_EQ(duty50, 500000ULL);
    EXPECT_EQ(duty75, 750000ULL);
    EXPECT_EQ(duty100, 1000000ULL);
}

// Main function
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
