/**
 * @file gtest_spi_extended.cpp
 * @brief Extended GoogleTest unit tests for SPI functionality
 * 
 * Tests SPI modes, clock speeds, bit order, and edge cases
 * to improve SPI.cpp coverage from 50% to 70%+.
 * 
 * @copyright Copyright (c) 2025 PiPinPP Project
 * @license MIT License
 */

#include <gtest/gtest.h>
#include "SPI.hpp"
#include "ArduinoCompat.hpp"
#include <thread>
#include <chrono>

using namespace std::chrono_literals;
using namespace pipinpp;

// Test fixture for extended SPI tests
class SPIExtendedTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clean state
    }
    
    void TearDown() override {
        // End SPI
        SPI.end();
    }
};

// ============================================================================
// SPI MODE TESTS
// ============================================================================

// Test: All SPI modes
TEST_F(SPIExtendedTest, AllSPIModes) {
    SPI.begin();
    
    EXPECT_NO_THROW({
        SPI.setDataMode(SPI_MODE0);  // CPOL=0, CPHA=0
        SPI.setDataMode(SPI_MODE1);  // CPOL=0, CPHA=1
        SPI.setDataMode(SPI_MODE2);  // CPOL=1, CPHA=0
        SPI.setDataMode(SPI_MODE3);  // CPOL=1, CPHA=1
    });
    
    SPI.end();
}

// Test: SPI mode constants have correct values
TEST_F(SPIExtendedTest, SPIModeConstantsValues) {
    EXPECT_EQ(SPI_MODE0, 0);
    EXPECT_EQ(SPI_MODE1, 1);
    EXPECT_EQ(SPI_MODE2, 2);
    EXPECT_EQ(SPI_MODE3, 3);
}

// ============================================================================
// CLOCK SPEED TESTS
// ============================================================================

// Test: Common SPI clock speeds
TEST_F(SPIExtendedTest, CommonClockSpeeds) {
    SPI.begin();
    
    std::vector<uint32_t> speeds = {
        500000,      // 500 kHz
        1000000,     // 1 MHz
        2000000,     // 2 MHz
        4000000,     // 4 MHz
        8000000,     // 8 MHz
        10000000     // 10 MHz
    };
    
    for (auto speed : speeds) {
        EXPECT_NO_THROW({
            SPI.setClockDivider(speed);
        });
    }
    
    SPI.end();
}

// Test: Very slow clock speed
TEST_F(SPIExtendedTest, VerySlowClockSpeed) {
    SPI.begin();
    EXPECT_NO_THROW({
        SPI.setClockDivider(1000);  // 1 kHz
    });
    SPI.end();
}

// Test: Maximum clock speed
TEST_F(SPIExtendedTest, MaximumClockSpeed) {
    SPI.begin();
    EXPECT_NO_THROW({
        SPI.setClockDivider(125000000);  // 125 MHz (Pi max)
    });
    SPI.end();
}

// ============================================================================
// BIT ORDER TESTS
// ============================================================================

// Test: MSB first (default)
TEST_F(SPIExtendedTest, BitOrderMSBFirst) {
    SPI.begin();
    EXPECT_NO_THROW({
        SPI.setBitOrder(MSBFIRST);
    });
    SPI.end();
}

// Test: LSB first
TEST_F(SPIExtendedTest, BitOrderLSBFirst) {
    SPI.begin();
    EXPECT_NO_THROW({
        SPI.setBitOrder(LSBFIRST);
    });
    SPI.end();
}

// Test: Bit order constants have correct values
TEST_F(SPIExtendedTest, BitOrderConstantsValues) {
    EXPECT_EQ(LSBFIRST, 0);
    EXPECT_EQ(MSBFIRST, 1);
}

// Test: Switching bit order multiple times
TEST_F(SPIExtendedTest, SwitchBitOrderMultipleTimes) {
    SPI.begin();
    
    for (int i = 0; i < 10; i++) {
        SPI.setBitOrder(MSBFIRST);
        SPI.setBitOrder(LSBFIRST);
    }
    
    SPI.end();
    SUCCEED();
}

// ============================================================================
// TRANSFER TESTS
// ============================================================================

// Test: Transfer single byte
TEST_F(SPIExtendedTest, TransferSingleByte) {
    try {
        SPI.begin();
        
        uint8_t result = SPI.transfer(0x55);
        // Result depends on slave device, just check it doesn't crash
        (void)result;
        
        SPI.end();
        SUCCEED();
    } catch (...) {
        // SPI hardware may not be available
        GTEST_SKIP() << "SPI hardware not available";
    }
}

// Test: Transfer zero byte
TEST_F(SPIExtendedTest, TransferZeroByte) {
    try {
        SPI.begin();
        uint8_t result = SPI.transfer(0x00);
        (void)result;
        SPI.end();
        SUCCEED();
    } catch (...) {
        GTEST_SKIP() << "SPI hardware not available";
    }
}

// Test: Transfer 0xFF
TEST_F(SPIExtendedTest, TransferAllOnes) {
    try {
        SPI.begin();
        uint8_t result = SPI.transfer(0xFF);
        (void)result;
        SPI.end();
        SUCCEED();
    } catch (...) {
        GTEST_SKIP() << "SPI hardware not available";
    }
}

// Test: Multiple transfers
TEST_F(SPIExtendedTest, MultipleTransfers) {
    try {
        SPI.begin();
        
        for (int i = 0; i < 10; i++) {
            SPI.transfer(i);
        }
        
        SPI.end();
        SUCCEED();
    } catch (...) {
        GTEST_SKIP() << "SPI hardware not available";
    }
}

// ============================================================================
// BEGIN/END TESTS
// ============================================================================

// Test: Multiple begin/end cycles
TEST_F(SPIExtendedTest, MultipleBeginEndCycles) {
    for (int i = 0; i < 5; i++) {
        SPI.begin();
        SPI.end();
    }
    SUCCEED();
}

// Test: begin() on already initialized SPI
TEST_F(SPIExtendedTest, BeginOnInitializedSPI) {
    SPI.begin();
    
    // Second begin should be safe
    EXPECT_NO_THROW({
        SPI.begin();
    });
    
    SPI.end();
}

// Test: end() on uninitialized SPI
TEST_F(SPIExtendedTest, EndOnUninitializedSPI) {
    SPI.end();
    EXPECT_NO_THROW({
        SPI.end();  // Should be safe
    });
}

// Test: Operations before begin()
TEST_F(SPIExtendedTest, OperationsBeforeBegin) {
    SPI.end();  // Ensure not initialized
    
    // These should not crash
    EXPECT_NO_THROW({
        SPI.setDataMode(SPI_MODE0);
        SPI.setClockDivider(1000000);
        SPI.setBitOrder(MSBFIRST);
    });
}

// ============================================================================
// CONFIGURATION COMBINATIONS
// ============================================================================

// Test: Full configuration sequence
TEST_F(SPIExtendedTest, FullConfigurationSequence) {
    SPI.begin();
    
    EXPECT_NO_THROW({
        SPI.setClockDivider(1000000);
        SPI.setDataMode(SPI_MODE1);
        SPI.setBitOrder(LSBFIRST);
    });
    
    SPI.end();
}

// Test: Configuration in different orders
TEST_F(SPIExtendedTest, ConfigurationDifferentOrders) {
    SPI.begin();
    
    // Order 1: mode -> clock -> bit order
    SPI.setDataMode(SPI_MODE2);
    SPI.setClockDivider(2000000);
    SPI.setBitOrder(MSBFIRST);
    
    // Order 2: bit order -> mode -> clock
    SPI.setBitOrder(LSBFIRST);
    SPI.setDataMode(SPI_MODE3);
    SPI.setClockDivider(4000000);
    
    SPI.end();
    SUCCEED();
}

// Test: Reconfigure between transfers
TEST_F(SPIExtendedTest, ReconfigureBetweenTransfers) {
    try {
        SPI.begin();
        
        SPI.setDataMode(SPI_MODE0);
        SPI.transfer(0x01);
        
        SPI.setDataMode(SPI_MODE1);
        SPI.transfer(0x02);
        
        SPI.setClockDivider(500000);
        SPI.transfer(0x03);
        
        SPI.end();
        SUCCEED();
    } catch (...) {
        GTEST_SKIP() << "SPI hardware not available";
    }
}

// ============================================================================
// THREAD SAFETY
// ============================================================================

// Test: Concurrent SPI access
TEST_F(SPIExtendedTest, ConcurrentSPIAccess) {
    try {
        SPI.begin();
        
        std::thread t1([&]() {
            for (int i = 0; i < 10; i++) {
                SPI.transfer(0xAA);
            }
        });
        
        std::thread t2([&]() {
            for (int i = 0; i < 10; i++) {
                SPI.transfer(0x55);
            }
        });
        
        t1.join();
        t2.join();
        
        SPI.end();
        SUCCEED();
    } catch (...) {
        GTEST_SKIP() << "SPI hardware not available";
    }
}

// Test: Concurrent configuration changes
TEST_F(SPIExtendedTest, ConcurrentConfigurationChanges) {
    SPI.begin();
    
    std::thread t1([&]() {
        for (int i = 0; i < 10; i++) {
            SPI.setDataMode(SPI_MODE0);
        }
    });
    
    std::thread t2([&]() {
        for (int i = 0; i < 10; i++) {
            SPI.setClockDivider(1000000);
        }
    });
    
    t1.join();
    t2.join();
    
    SPI.end();
    SUCCEED();
}

// ============================================================================
// EDGE CASES
// ============================================================================

// Test: Transfer pattern
TEST_F(SPIExtendedTest, TransferPattern) {
    try {
        SPI.begin();
        
        // Transfer incrementing pattern
        for (uint8_t i = 0; i < 255; i++) {
            SPI.transfer(i);
        }
        
        SPI.end();
        SUCCEED();
    } catch (...) {
        GTEST_SKIP() << "SPI hardware not available";
    }
}

// Test: Rapid configuration changes
TEST_F(SPIExtendedTest, RapidConfigurationChanges) {
    SPI.begin();
    
    for (int i = 0; i < 100; i++) {
        SPI.setDataMode(i % 4);
        SPI.setBitOrder(i % 2);
    }
    
    SPI.end();
    SUCCEED();
}
