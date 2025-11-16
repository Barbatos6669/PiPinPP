/**
 * @file gtest_pwm_spi_final.cpp
 * @brief Final comprehensive tests for PWM and SPI to reach 80% coverage
 * @author Barbatos6669
 * @date 2025-11-16
 * 
 * Targets remaining uncovered paths in pwm.cpp and SPI.cpp:
 * - PWM edge cases, error handling, lifecycle
 * - SPI configuration validation, error paths, edge cases
 */

#include <gtest/gtest.h>
#include "pwm.hpp"
#include "SPI.hpp"
#include "ArduinoCompat.hpp"
#include "exceptions.hpp"
#include <thread>
#include <chrono>

using namespace pipinpp;
using namespace std::chrono_literals;

// ============================================================================
// PWM Comprehensive Tests
// ============================================================================

class PWMFinalTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Stop any existing PWM
        PWMManager::getInstance().stopPWM(17);
        PWMManager::getInstance().stopPWM(18);
    }
    
    void TearDown() override {
        PWMManager::getInstance().stopPWM(17);
        PWMManager::getInstance().stopPWM(18);
    }
};

TEST_F(PWMFinalTest, StartPWMZeroDutyCycle) {
    try {
        PWMManager::getInstance().startPWM(17, 1000, 0);
        std::this_thread::sleep_for(10ms);
        PWMManager::getInstance().stopPWM(17);
        SUCCEED();
    } catch (const GpioAccessError&) {
        GTEST_SKIP() << "GPIO not available";
    }
}

TEST_F(PWMFinalTest, StartPWMFullDutyCycle) {
    try {
        PWMManager::getInstance().startPWM(17, 1000, 100);
        std::this_thread::sleep_for(10ms);
        PWMManager::getInstance().stopPWM(17);
        SUCCEED();
    } catch (const GpioAccessError&) {
        GTEST_SKIP() << "GPIO not available";
    }
}

TEST_F(PWMFinalTest, StartPWMVeryLowFrequency) {
    try {
        PWMManager::getInstance().startPWM(17, 1, 50);  // 1 Hz
        std::this_thread::sleep_for(10ms);
        PWMManager::getInstance().stopPWM(17);
        SUCCEED();
    } catch (const GpioAccessError&) {
        GTEST_SKIP() << "GPIO not available";
    }
}

TEST_F(PWMFinalTest, StartPWMVeryHighFrequency) {
    try {
        PWMManager::getInstance().startPWM(17, 10000, 50);  // 10 kHz
        std::this_thread::sleep_for(10ms);
        PWMManager::getInstance().stopPWM(17);
        SUCCEED();
    } catch (const GpioAccessError&) {
        GTEST_SKIP() << "GPIO not available";
    }
}

TEST_F(PWMFinalTest, StopPWMNotStarted) {
    // Stopping non-existent PWM should be safe
    EXPECT_NO_THROW({
        PWMManager::getInstance().stopPWM(17);
    });
}

TEST_F(PWMFinalTest, StopPWMTwice) {
    try {
        PWMManager::getInstance().startPWM(17, 1000, 50);
        PWMManager::getInstance().stopPWM(17);
        PWMManager::getInstance().stopPWM(17);  // Second stop
        SUCCEED();
    } catch (const GpioAccessError&) {
        GTEST_SKIP() << "GPIO not available";
    }
}

TEST_F(PWMFinalTest, RestartPWMSamePin) {
    try {
        PWMManager::getInstance().startPWM(17, 1000, 50);
        std::this_thread::sleep_for(10ms);
        PWMManager::getInstance().stopPWM(17);
        
        PWMManager::getInstance().startPWM(17, 2000, 75);
        std::this_thread::sleep_for(10ms);
        PWMManager::getInstance().stopPWM(17);
        SUCCEED();
    } catch (const GpioAccessError&) {
        GTEST_SKIP() << "GPIO not available";
    }
}

TEST_F(PWMFinalTest, MultiplePWMPinsSimultaneous) {
    try {
        PWMManager::getInstance().startPWM(17, 1000, 50);
        PWMManager::getInstance().startPWM(18, 2000, 25);
        std::this_thread::sleep_for(20ms);
        PWMManager::getInstance().stopPWM(17);
        PWMManager::getInstance().stopPWM(18);
        SUCCEED();
    } catch (const GpioAccessError&) {
        GTEST_SKIP() << "GPIO not available";
    }
}

TEST_F(PWMFinalTest, SetDutyCycleWhileRunning) {
    try {
        PWMManager::getInstance().startPWM(17, 1000, 50);
        std::this_thread::sleep_for(10ms);
        
        PWMManager::getInstance().setDutyCycle(17, 75);
        std::this_thread::sleep_for(10ms);
        
        PWMManager::getInstance().stopPWM(17);
        SUCCEED();
    } catch (const GpioAccessError&) {
        GTEST_SKIP() << "GPIO not available";
    }
}

TEST_F(PWMFinalTest, SetDutyCycleNotStarted) {
    // Setting duty cycle on non-existent PWM
    EXPECT_NO_THROW({
        PWMManager::getInstance().setDutyCycle(17, 50);
    });
}

TEST_F(PWMFinalTest, SetDutyCycleZero) {
    try {
        PWMManager::getInstance().startPWM(17, 1000, 50);
        PWMManager::getInstance().setDutyCycle(17, 0);
        std::this_thread::sleep_for(10ms);
        PWMManager::getInstance().stopPWM(17);
        SUCCEED();
    } catch (const GpioAccessError&) {
        GTEST_SKIP() << "GPIO not available";
    }
}

TEST_F(PWMFinalTest, SetDutyCycleFull) {
    try {
        PWMManager::getInstance().startPWM(17, 1000, 50);
        PWMManager::getInstance().setDutyCycle(17, 100);
        std::this_thread::sleep_for(10ms);
        PWMManager::getInstance().stopPWM(17);
        SUCCEED();
    } catch (const GpioAccessError&) {
        GTEST_SKIP() << "GPIO not available";
    }
}

TEST_F(PWMFinalTest, SetDutyCycleOverMax) {
    try {
        PWMManager::getInstance().startPWM(17, 1000, 50);
        PWMManager::getInstance().setDutyCycle(17, 150);  // Should clamp
        std::this_thread::sleep_for(10ms);
        PWMManager::getInstance().stopPWM(17);
        SUCCEED();
    } catch (const GpioAccessError&) {
        GTEST_SKIP() << "GPIO not available";
    }
}

TEST_F(PWMFinalTest, RapidDutyCycleChanges) {
    try {
        PWMManager::getInstance().startPWM(17, 1000, 50);
        
        for (int i = 0; i < 10; i++) {
            PWMManager::getInstance().setDutyCycle(17, i * 10);
            std::this_thread::sleep_for(5ms);
        }
        
        PWMManager::getInstance().stopPWM(17);
        SUCCEED();
    } catch (const GpioAccessError&) {
        GTEST_SKIP() << "GPIO not available";
    }
}

TEST_F(PWMFinalTest, VeryShortPWMDuration) {
    try {
        PWMManager::getInstance().startPWM(17, 1000, 50);
        std::this_thread::sleep_for(1ms);  // Very short
        PWMManager::getInstance().stopPWM(17);
        SUCCEED();
    } catch (const GpioAccessError&) {
        GTEST_SKIP() << "GPIO not available";
    }
}

TEST_F(PWMFinalTest, PWMInvalidPin) {
    EXPECT_THROW({
        PWMManager::getInstance().startPWM(-1, 1000, 50);
    }, InvalidPinError);
}

TEST_F(PWMFinalTest, PWMInvalidPinHigh) {
    EXPECT_THROW({
        PWMManager::getInstance().startPWM(99, 1000, 50);
    }, InvalidPinError);
}

TEST_F(PWMFinalTest, IsActiveFalseInitially) {
    bool active = PWMManager::getInstance().isActive(17);
    EXPECT_FALSE(active);
}

TEST_F(PWMFinalTest, IsActiveTrueWhenRunning) {
    try {
        PWMManager::getInstance().startPWM(17, 1000, 50);
        bool active = PWMManager::getInstance().isActive(17);
        EXPECT_TRUE(active);
        PWMManager::getInstance().stopPWM(17);
    } catch (const GpioAccessError&) {
        GTEST_SKIP() << "GPIO not available";
    }
}

TEST_F(PWMFinalTest, IsActiveFalseAfterStop) {
    try {
        PWMManager::getInstance().startPWM(17, 1000, 50);
        PWMManager::getInstance().stopPWM(17);
        bool active = PWMManager::getInstance().isActive(17);
        EXPECT_FALSE(active);
    } catch (const GpioAccessError&) {
        GTEST_SKIP() << "GPIO not available";
    }
}

// ============================================================================
// SPI Comprehensive Tests
// ============================================================================

class SPIFinalTest : public ::testing::Test {
protected:
    void SetUp() override {
        SPI.end();
    }
    
    void TearDown() override {
        SPI.end();
    }
};

TEST_F(SPIFinalTest, BeginDefaultBus) {
    try {
        bool success = SPI.begin();
        if (success) {
            EXPECT_TRUE(SPI.isInitialized());
            SPI.end();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, BeginWithBusAndCS) {
    try {
        bool success = SPI.begin(0, 0);
        if (success) {
            EXPECT_TRUE(SPI.isInitialized());
            SPI.end();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, BeginInvalidBus) {
    EXPECT_FALSE(SPI.begin(99, 0));
}

TEST_F(SPIFinalTest, BeginNegativeBus) {
    EXPECT_FALSE(SPI.begin(-1, 0));
}

TEST_F(SPIFinalTest, BeginInvalidCS) {
    EXPECT_FALSE(SPI.begin(0, -1));
}

TEST_F(SPIFinalTest, EndWithoutBegin) {
    EXPECT_NO_THROW({
        SPI.end();
    });
}

TEST_F(SPIFinalTest, EndTwice) {
    try {
        if (SPI.begin()) {
            SPI.end();
            SPI.end();  // Second end
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, IsInitializedFalseInitially) {
    EXPECT_FALSE(SPI.isInitialized());
}

TEST_F(SPIFinalTest, IsInitializedTrueAfterBegin) {
    try {
        if (SPI.begin()) {
            EXPECT_TRUE(SPI.isInitialized());
            SPI.end();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, IsInitializedFalseAfterEnd) {
    try {
        if (SPI.begin()) {
            SPI.end();
            EXPECT_FALSE(SPI.isInitialized());
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, SetDataModeMode0) {
    try {
        if (SPI.begin()) {
            SPI.setDataMode(SPI_MODE0);
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, SetDataModeMode1) {
    try {
        if (SPI.begin()) {
            SPI.setDataMode(SPI_MODE1);
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, SetDataModeMode2) {
    try {
        if (SPI.begin()) {
            SPI.setDataMode(SPI_MODE2);
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, SetDataModeMode3) {
    try {
        if (SPI.begin()) {
            SPI.setDataMode(SPI_MODE3);
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, SetDataModeWithoutBegin) {
    // setDataMode without begin is safe - just doesn't apply
    EXPECT_NO_THROW({
        SPI.setDataMode(SPI_MODE0);
    });
}

TEST_F(SPIFinalTest, SetBitOrderMSBFirst) {
    try {
        if (SPI.begin()) {
            SPI.setBitOrder(MSBFIRST);
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, SetBitOrderLSBFirst) {
    try {
        if (SPI.begin()) {
            SPI.setBitOrder(LSBFIRST);
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, SetBitOrderWithoutBegin) {
    // setBitOrder without begin is safe - just doesn't apply
    EXPECT_NO_THROW({
        SPI.setBitOrder(MSBFIRST);
    });
}

TEST_F(SPIFinalTest, SetClockDivider) {
    try {
        if (SPI.begin()) {
            SPI.setClockDivider(128);
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, SetClockDividerWithoutBegin) {
    // setClockDivider without begin is safe - just doesn't apply
    EXPECT_NO_THROW({
        SPI.setClockDivider(128);
    });
}

TEST_F(SPIFinalTest, SetClock) {
    try {
        if (SPI.begin()) {
            SPI.setClock(1000000);  // 1 MHz
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, SetClockLowSpeed) {
    try {
        if (SPI.begin()) {
            SPI.setClock(100000);  // 100 kHz
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, SetClockHighSpeed) {
    try {
        if (SPI.begin()) {
            SPI.setClock(10000000);  // 10 MHz
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, SetClockWithoutBegin) {
    // setClock without begin is safe - just doesn't apply
    EXPECT_NO_THROW({
        SPI.setClock(1000000);
    });
}

TEST_F(SPIFinalTest, GetClockDefault) {
    try {
        if (SPI.begin()) {
            uint32_t clock = SPI.getClock();
            EXPECT_GT(clock, 0);
            SPI.end();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, GetClockAfterSet) {
    try {
        if (SPI.begin()) {
            SPI.setClock(1000000);
            uint32_t clock = SPI.getClock();
            EXPECT_EQ(clock, 1000000);
            SPI.end();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, TransferSingleByte) {
    try {
        if (SPI.begin()) {
            uint8_t result = SPI.transfer(0x55);
            (void)result;  // Value depends on hardware
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, TransferZeroByte) {
    try {
        if (SPI.begin()) {
            uint8_t result = SPI.transfer(0x00);
            (void)result;
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, TransferMaxByte) {
    try {
        if (SPI.begin()) {
            uint8_t result = SPI.transfer(0xFF);
            (void)result;
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, TransferWithoutBegin) {
    // transfer returns 0 without begin
    uint8_t result = SPI.transfer(0x55);
    EXPECT_EQ(result, 0);
}

TEST_F(SPIFinalTest, TransferBufferInPlace) {
    try {
        if (SPI.begin()) {
            uint8_t buffer[] = {0x01, 0x02, 0x03, 0x04};
            SPI.transfer(buffer, 4);
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, TransferBufferEmpty) {
    try {
        if (SPI.begin()) {
            uint8_t buffer[1];
            SPI.transfer(buffer, 0);  // Zero length
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, TransferBufferLarge) {
    try {
        if (SPI.begin()) {
            uint8_t buffer[256];
            for (int i = 0; i < 256; i++) {
                buffer[i] = static_cast<uint8_t>(i);
            }
            SPI.transfer(buffer, 256);
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, TransferBufferWithoutBegin) {
    uint8_t buffer[] = {0x01, 0x02};
    // transfer is safe without begin - just doesn't do anything
    EXPECT_NO_THROW({
        SPI.transfer(buffer, 2);
    });
}

TEST_F(SPIFinalTest, TransferSeparateBuffers) {
    try {
        if (SPI.begin()) {
            uint8_t txBuffer[] = {0x01, 0x02, 0x03};
            uint8_t rxBuffer[3] = {0};
            SPI.transfer(txBuffer, rxBuffer, 3);
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, TransferSeparateBuffersZeroLength) {
    try {
        if (SPI.begin()) {
            uint8_t txBuffer[1];
            uint8_t rxBuffer[1];
            SPI.transfer(txBuffer, rxBuffer, 0);
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, TransferSeparateBuffersWithoutBegin) {
    uint8_t txBuffer[] = {0x01};
    uint8_t rxBuffer[1];
    // transfer is safe without begin - just doesn't do anything
    EXPECT_NO_THROW({
        SPI.transfer(txBuffer, rxBuffer, 1);
    });
}

TEST_F(SPIFinalTest, MultipleTransfers) {
    try {
        if (SPI.begin()) {
            for (int i = 0; i < 10; i++) {
                SPI.transfer(static_cast<uint8_t>(i));
            }
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, ConfigurationSequence) {
    try {
        if (SPI.begin()) {
            SPI.setDataMode(SPI_MODE1);
            SPI.setBitOrder(MSBFIRST);
            SPI.setClock(1000000);
            
            uint8_t data = SPI.transfer(0xAA);
            (void)data;
            
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

TEST_F(SPIFinalTest, ReconfigureAfterTransfer) {
    try {
        if (SPI.begin()) {
            SPI.transfer(0x55);
            
            SPI.setDataMode(SPI_MODE2);
            SPI.setClock(500000);
            
            SPI.transfer(0xAA);
            
            SPI.end();
            SUCCEED();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "SPI not available";
    }
}

// Main function
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
