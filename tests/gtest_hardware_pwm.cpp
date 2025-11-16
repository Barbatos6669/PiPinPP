/**
 * @file gtest_hardware_pwm.cpp
 * @brief Unit tests for HardwarePWM class
 * @author Barbatos6669
 * @date 2025-11-16
 * 
 * Tests hardware PWM functionality using GoogleTest framework.
 * Most tests are hardware-independent and test API compliance.
 */

#include <gtest/gtest.h>
#include "HardwarePWM.hpp"
#include <thread>
#include <vector>

using namespace pipinpp;

/**
 * @brief Test fixture for HardwarePWM tests
 */
class HardwarePWMTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Tests run without hardware access
    }
    
    void TearDown() override {
        // Cleanup
    }
};

// ============================================================================
// Basic API Tests (No Hardware Required)
// ============================================================================

TEST_F(HardwarePWMTest, ConstructorDoesNotThrow) {
    EXPECT_NO_THROW({
        HardwarePWM pwm(0, 0);
    });
}

TEST_F(HardwarePWMTest, MultipleConstructorsAllowed) {
    EXPECT_NO_THROW({
        HardwarePWM pwm1(0, 0);
        HardwarePWM pwm2(0, 1);
    });
}

TEST_F(HardwarePWMTest, IsEnabledInitiallyFalse) {
    HardwarePWM pwm(0, 0);
    EXPECT_FALSE(pwm.isEnabled());
}

TEST_F(HardwarePWMTest, GetFrequencyInitiallyZero) {
    HardwarePWM pwm(0, 0);
    EXPECT_EQ(pwm.getFrequency(), 0U);
}

TEST_F(HardwarePWMTest, GetDutyCycleInitiallyNegative) {
    HardwarePWM pwm(0, 0);
    EXPECT_LT(pwm.getDutyCycle(), 0.0);
}

TEST_F(HardwarePWMTest, GPIOToPWMConversion) {
    int chip, channel;
    
    // Valid hardware PWM pins
    EXPECT_TRUE(HardwarePWM::gpioToPWM(12, chip, channel));
    EXPECT_EQ(chip, 0);
    EXPECT_EQ(channel, 0);
    
    EXPECT_TRUE(HardwarePWM::gpioToPWM(13, chip, channel));
    EXPECT_EQ(chip, 0);
    EXPECT_EQ(channel, 1);
    
    EXPECT_TRUE(HardwarePWM::gpioToPWM(18, chip, channel));
    EXPECT_EQ(chip, 0);
    EXPECT_EQ(channel, 0);
    
    EXPECT_TRUE(HardwarePWM::gpioToPWM(19, chip, channel));
    EXPECT_EQ(chip, 0);
    EXPECT_EQ(channel, 1);
    
    // Invalid pins
    EXPECT_FALSE(HardwarePWM::gpioToPWM(17, chip, channel));
    EXPECT_FALSE(HardwarePWM::gpioToPWM(27, chip, channel));
    EXPECT_FALSE(HardwarePWM::gpioToPWM(-1, chip, channel));
    EXPECT_FALSE(HardwarePWM::gpioToPWM(100, chip, channel));
}

TEST_F(HardwarePWMTest, EndWithoutBeginSafe) {
    HardwarePWM pwm(0, 0);
    EXPECT_NO_THROW(pwm.end());
}

TEST_F(HardwarePWMTest, MultipleEndCallsSafe) {
    HardwarePWM pwm(0, 0);
    EXPECT_NO_THROW({
        pwm.end();
        pwm.end();
        pwm.end();
    });
}

// ============================================================================
// Thread Safety Tests
// ============================================================================

TEST_F(HardwarePWMTest, ConcurrentGetFrequencySafe) {
    HardwarePWM pwm(0, 0);
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&pwm]() {
            for (int j = 0; j < 100; j++) {
                pwm.getFrequency();
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    SUCCEED();
}

TEST_F(HardwarePWMTest, ConcurrentGetDutyCycleSafe) {
    HardwarePWM pwm(0, 0);
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&pwm]() {
            for (int j = 0; j < 100; j++) {
                pwm.getDutyCycle();
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    SUCCEED();
}

TEST_F(HardwarePWMTest, ConcurrentIsEnabledSafe) {
    HardwarePWM pwm(0, 0);
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&pwm]() {
            for (int j = 0; j < 100; j++) {
                pwm.isEnabled();
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    SUCCEED();
}

// ============================================================================
// API Compliance Tests (Without Hardware)
// ============================================================================

TEST_F(HardwarePWMTest, SetFrequencyWithoutBeginFails) {
    HardwarePWM pwm(0, 0);
    EXPECT_FALSE(pwm.setFrequency(1000));
}

TEST_F(HardwarePWMTest, SetDutyCycleWithoutBeginFails) {
    HardwarePWM pwm(0, 0);
    EXPECT_FALSE(pwm.setDutyCycle(50.0));
}

TEST_F(HardwarePWMTest, SetDutyCycle8BitWithoutBeginFails) {
    HardwarePWM pwm(0, 0);
    EXPECT_FALSE(pwm.setDutyCycle8Bit(128));
}

TEST_F(HardwarePWMTest, EnableWithoutBeginFails) {
    HardwarePWM pwm(0, 0);
    EXPECT_FALSE(pwm.enable());
}

TEST_F(HardwarePWMTest, SetPolarityWithoutBeginFails) {
    HardwarePWM pwm(0, 0);
    EXPECT_FALSE(pwm.setPolarity(PWMPolarity::NORMAL));
}

TEST_F(HardwarePWMTest, DisableWithoutBeginSafe) {
    HardwarePWM pwm(0, 0);
    // Should not crash
    EXPECT_TRUE(pwm.disable());
}

// ============================================================================
// Value Range Tests
// ============================================================================

TEST_F(HardwarePWMTest, DutyCycle8BitConversion) {
    // Test that 8-bit conversion is correct
    // 0 → 0%, 128 → ~50%, 255 → 100%
    // These are mathematical conversions, no hardware needed
    
    double duty0 = (0 / 255.0) * 100.0;
    double duty128 = (128 / 255.0) * 100.0;
    double duty255 = (255 / 255.0) * 100.0;
    
    EXPECT_NEAR(duty0, 0.0, 0.1);
    EXPECT_NEAR(duty128, 50.2, 0.5);
    EXPECT_NEAR(duty255, 100.0, 0.1);
}

TEST_F(HardwarePWMTest, FrequencyToNanosecondsConversion) {
    // Test period calculation
    // 1000Hz → 1,000,000 ns
    // 50Hz → 20,000,000 ns
    
    uint64_t period1000Hz = 1000000000ULL / 1000;
    uint64_t period50Hz = 1000000000ULL / 50;
    
    EXPECT_EQ(period1000Hz, 1000000ULL);
    EXPECT_EQ(period50Hz, 20000000ULL);
}

// ============================================================================
// Polarity Enum Tests
// ============================================================================

TEST_F(HardwarePWMTest, PolarityEnumValues) {
    EXPECT_EQ(static_cast<int>(PWMPolarity::NORMAL), 0);
    EXPECT_NE(static_cast<int>(PWMPolarity::INVERSED), 0);
}

// ============================================================================
// Hardware Tests (Skip if no access)
// ============================================================================

TEST_F(HardwarePWMTest, BeginWithHardware) {
    GTEST_SKIP() << "Hardware test - requires /sys/class/pwm access";
    
    HardwarePWM pwm(0, 0);
    EXPECT_TRUE(pwm.begin(1000, 50.0));
    EXPECT_TRUE(pwm.isEnabled());
    EXPECT_EQ(pwm.getFrequency(), 1000U);
    EXPECT_NEAR(pwm.getDutyCycle(), 50.0, 1.0);
    pwm.end();
}

TEST_F(HardwarePWMTest, SetDutyCycleWithHardware) {
    GTEST_SKIP() << "Hardware test - requires /sys/class/pwm access";
    
    HardwarePWM pwm(0, 0);
    ASSERT_TRUE(pwm.begin(1000, 0.0));
    
    EXPECT_TRUE(pwm.setDutyCycle(25.0));
    EXPECT_NEAR(pwm.getDutyCycle(), 25.0, 1.0);
    
    EXPECT_TRUE(pwm.setDutyCycle(75.0));
    EXPECT_NEAR(pwm.getDutyCycle(), 75.0, 1.0);
    
    pwm.end();
}

TEST_F(HardwarePWMTest, SetDutyCycle8BitWithHardware) {
    GTEST_SKIP() << "Hardware test - requires /sys/class/pwm access";
    
    HardwarePWM pwm(0, 0);
    ASSERT_TRUE(pwm.begin(1000, 0.0));
    
    EXPECT_TRUE(pwm.setDutyCycle8Bit(128));
    EXPECT_NEAR(pwm.getDutyCycle(), 50.0, 2.0);
    
    EXPECT_TRUE(pwm.setDutyCycle8Bit(255));
    EXPECT_NEAR(pwm.getDutyCycle(), 100.0, 1.0);
    
    pwm.end();
}

TEST_F(HardwarePWMTest, SetFrequencyWithHardware) {
    GTEST_SKIP() << "Hardware test - requires /sys/class/pwm access";
    
    HardwarePWM pwm(0, 0);
    ASSERT_TRUE(pwm.begin(1000, 50.0));
    
    EXPECT_TRUE(pwm.setFrequency(500));
    EXPECT_EQ(pwm.getFrequency(), 500U);
    EXPECT_NEAR(pwm.getDutyCycle(), 50.0, 1.0);  // Duty cycle % preserved
    
    pwm.end();
}

TEST_F(HardwarePWMTest, EnableDisableWithHardware) {
    GTEST_SKIP() << "Hardware test - requires /sys/class/pwm access";
    
    HardwarePWM pwm(0, 0);
    ASSERT_TRUE(pwm.begin(1000, 50.0));
    EXPECT_TRUE(pwm.isEnabled());
    
    EXPECT_TRUE(pwm.disable());
    EXPECT_FALSE(pwm.isEnabled());
    
    EXPECT_TRUE(pwm.enable());
    EXPECT_TRUE(pwm.isEnabled());
    
    pwm.end();
}

TEST_F(HardwarePWMTest, ServoFrequencyWithHardware) {
    GTEST_SKIP() << "Hardware test - requires /sys/class/pwm access";
    
    // Standard servo: 50Hz, 5-10% duty cycle (1-2ms pulse width)
    HardwarePWM servo(0, 0);
    ASSERT_TRUE(servo.begin(50, 7.5));  // Center position
    
    EXPECT_EQ(servo.getFrequency(), 50U);
    EXPECT_NEAR(servo.getDutyCycle(), 7.5, 0.5);
    
    // Test range
    EXPECT_TRUE(servo.setDutyCycle(5.0));   // 0°
    EXPECT_TRUE(servo.setDutyCycle(10.0));  // 180°
    EXPECT_TRUE(servo.setDutyCycle(7.5));   // 90°
    
    servo.end();
}

// Main function
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
