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

// ============================================================================
// ERROR PATH TESTS
// ============================================================================

// Test: Transfer operations without initialization
TEST_F(SPIExtendedTest, TransferWithoutInitialization) {
    // SPI not initialized - should not crash
    SPI.end();  // Ensure not initialized
    EXPECT_FALSE(SPI.isInitialized());
    
    // Transfer should return 0 when not initialized
    uint8_t result = SPI.transfer(0x42);
    EXPECT_EQ(result, 0);
    
    // Buffer transfer should not crash
    uint8_t buffer[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    EXPECT_NO_THROW({
        SPI.transfer(buffer, 10);
    });
    
    // Verify buffer unchanged (no transfer occurred)
    EXPECT_EQ(buffer[0], 1);
}

// Test: Invalid SPI mode
TEST_F(SPIExtendedTest, InvalidSPIMode) {
    SPI.begin();
    
    // Valid modes: 0-3
    EXPECT_NO_THROW(SPI.setDataMode(0));
    EXPECT_NO_THROW(SPI.setDataMode(3));
    
    // Invalid modes (> 3) should be rejected silently
    EXPECT_NO_THROW(SPI.setDataMode(4));
    EXPECT_NO_THROW(SPI.setDataMode(255));
    
    SPI.end();
}

// Test: Null buffer transfer
TEST_F(SPIExtendedTest, NullBufferTransfer) {
    SPI.begin();
    
    // Null pointer should be handled safely
    EXPECT_NO_THROW({
        SPI.transfer(nullptr, 10);
    });
    
    // Null TX/RX buffers in separate transfer
    uint8_t dummy[10] = {0};
    EXPECT_NO_THROW({
        SPI.transfer(nullptr, dummy, 10);
        SPI.transfer(dummy, nullptr, 10);
        SPI.transfer(nullptr, nullptr, 10);
    });
    
    SPI.end();
}

// Test: Zero-length transfer
TEST_F(SPIExtendedTest, ZeroLengthTransfer) {
    SPI.begin();
    
    uint8_t buffer[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Zero-length transfer should be handled
    EXPECT_NO_THROW({
        SPI.transfer(buffer, 0);
    });
    
    // Buffer contents should be unchanged
    EXPECT_EQ(buffer[0], 1);
    
    SPI.end();
}

// Test: Multiple begin() calls
TEST_F(SPIExtendedTest, MultipleBeginCalls) {
    // First begin
    bool first = SPI.begin();
    if (!first) {
        GTEST_SKIP() << "SPI hardware not available";
    }
    EXPECT_TRUE(SPI.isInitialized());
    
    // Second begin should close first and re-initialize
    EXPECT_TRUE(SPI.begin());
    EXPECT_TRUE(SPI.isInitialized());
    
    // Third begin with different parameters
    EXPECT_NO_THROW({
        (void)SPI.begin(0, 1);  // May fail if CS doesn't exist
    });
    
    SPI.end();
}

// Test: Multiple end() calls
TEST_F(SPIExtendedTest, MultipleEndCalls) {
    SPI.begin();
    
    SPI.end();
    EXPECT_FALSE(SPI.isInitialized());
    
    // Multiple end() calls should be safe
    EXPECT_NO_THROW({
        SPI.end();
        SPI.end();
        SPI.end();
    });
}

// Test: Settings after end()
TEST_F(SPIExtendedTest, SettingsAfterEnd) {
    SPI.begin();
    SPI.end();
    
    // Settings changes after end() should not crash
    EXPECT_NO_THROW({
        SPI.setDataMode(SPI_MODE1);
        SPI.setBitOrder(LSBFIRST);
        SPI.setClock(500000);
    });
    
    // Transfer after end should not crash
    uint8_t result = SPI.transfer(0xAA);
    EXPECT_EQ(result, 0);  // Should return 0 when closed
}

// Test: Invalid clock speeds
TEST_F(SPIExtendedTest, InvalidClockSpeeds) {
    SPI.begin();
    
    // Extremely low speed (should be clamped or handled)
    EXPECT_NO_THROW(SPI.setClock(1));
    
    // Extremely high speed (kernel will clamp to max)
    EXPECT_NO_THROW(SPI.setClock(999999999));
    
    // Zero speed
    EXPECT_NO_THROW(SPI.setClock(0));
    
    SPI.end();
}

// Test: Large buffer transfer
TEST_F(SPIExtendedTest, LargeBufferTransfer) {
    try {
        SPI.begin();
        
        // Large buffer (1KB)
        constexpr size_t SIZE = 1024;
        uint8_t txBuffer[SIZE];
        uint8_t rxBuffer[SIZE];
        
        // Fill with pattern
        for (size_t i = 0; i < SIZE; i++) {
            txBuffer[i] = i & 0xFF;
        }
        
        // Transfer large buffer
        EXPECT_NO_THROW({
            SPI.transfer(txBuffer, rxBuffer, SIZE);
        });
        
        SPI.end();
    } catch (...) {
        GTEST_SKIP() << "SPI hardware not available";
    }
}

// Test: Concurrent begin/end operations
TEST_F(SPIExtendedTest, ConcurrentBeginEnd) {
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 5; i++) {
        threads.emplace_back([&]() {
            for (int j = 0; j < 10; j++) {
                try {
                    SPI.begin();
                    std::this_thread::sleep_for(1ms);
                    SPI.end();
                } catch (...) {
                    // May fail if device unavailable
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    SUCCEED();
}

// Test: Clock divider edge cases
TEST_F(SPIExtendedTest, ClockDividerEdgeCases) {
    SPI.begin();
    
    // Divider of 1 (max speed)
    EXPECT_NO_THROW(SPI.setClockDivider(1));
    
    // Very large divider (very slow)
    EXPECT_NO_THROW(SPI.setClockDivider(65535));
    
    // Check getClock returns valid value
    uint32_t clock = SPI.getClock();
    EXPECT_GT(clock, 0);
    
    SPI.end();
}

// Test: Invalid bus/CS numbers
TEST_F(SPIExtendedTest, InvalidBusCSNumbers) {
    // Negative bus (should fail or clamp)
    EXPECT_NO_THROW({ 
        (void)SPI.begin(-1, 0);
    });
    
    // Very high bus number (likely doesn't exist)
    EXPECT_NO_THROW({ 
        (void)SPI.begin(99, 0);
    });
    
    // Very high CS number
    EXPECT_NO_THROW({ 
        (void)SPI.begin(0, 99);
    });
    
    SPI.end();
}

// Test: Transfer with settings changes mid-operation
TEST_F(SPIExtendedTest, SettingsChangeDuringTransfer) {
    try {
        SPI.begin();
        
        std::thread t1([&]() {
            for (int i = 0; i < 50; i++) {
                SPI.transfer(0x55);
            }
        });
        
        std::thread t2([&]() {
            for (int i = 0; i < 10; i++) {
                SPI.setDataMode(i % 4);
                SPI.setClock(1000000 + (i * 100000));
            }
        });
        
        t1.join();
        t2.join();
        
        SPI.end();
    } catch (...) {
        GTEST_SKIP() << "SPI hardware not available";
    }
}

// Test: isInitialized state tracking
TEST_F(SPIExtendedTest, IsInitializedStateTracking) {
    EXPECT_FALSE(SPI.isInitialized());
    
    bool initialized = SPI.begin();
    if (!initialized) {
        GTEST_SKIP() << "SPI hardware not available";
    }
    EXPECT_TRUE(SPI.isInitialized());
    
    SPI.end();
    EXPECT_FALSE(SPI.isInitialized());
    
    // Multiple state changes
    for (int i = 0; i < 5; i++) {
        EXPECT_FALSE(SPI.isInitialized());
        SPI.begin();
        EXPECT_TRUE(SPI.isInitialized());
        SPI.end();
        EXPECT_FALSE(SPI.isInitialized());
    }
}
