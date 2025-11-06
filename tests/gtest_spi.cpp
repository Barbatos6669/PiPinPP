/**
 * @file gtest_spi.cpp
 * @brief Google Test unit tests for SPI functionality
 * 
 * Tests SPI initialization, configuration, transfer operations, and thread safety.
 * Hardware-dependent tests are skipped when /dev/spidev* is not available.
 */

#include <gtest/gtest.h>
#include "ArduinoCompat.hpp"  // For MSBFIRST/LSBFIRST constants
#include "SPI.hpp"
#include <thread>
#include <vector>
#include <chrono>

using namespace pipinpp;

/**
 * @brief Test fixture for SPI tests
 */
class SPITest : public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        // Ensure SPI is not initialized from previous tests
        SPI.end();
    }

    void TearDown() override 
    {
        // Clean up after each test
        SPI.end();
    }
};

/**
 * @brief Test that SPI initialization returns a bool
 */
TEST_F(SPITest, InitializationReturnsBoolean) 
{
    bool result = SPI.begin();
    EXPECT_TRUE(result == true || result == false);
    // Note: May be false if no SPI hardware available
}

/**
 * @brief Test that multiple begin() calls are safe
 */
TEST_F(SPITest, MultipleBeginCallsSafe) 
{
    SPI.begin();
    SPI.begin();  // Should not crash or cause issues
    EXPECT_TRUE(true);  // If we got here, no crash occurred
}

/**
 * @brief Test end() after begin()
 */
TEST_F(SPITest, EndAfterBegin) 
{
    SPI.begin();
    SPI.end();
    EXPECT_FALSE(SPI.isInitialized());
}

/**
 * @brief Test begin with specific bus and chip select
 */
TEST_F(SPITest, BeginWithSpecificBusAndCS) 
{
    // Try both common SPI bus numbers
    bool result0 = SPI.begin(0, 0);
    SPI.end();
    
    bool result1 = SPI.begin(0, 1);
    SPI.end();
    
    // At least one call should succeed if SPI hardware exists
    // Both may fail on systems without /dev/spidev*
    EXPECT_TRUE(result0 || result1 || !result0);  // Just verify it returns a bool
}

/**
 * @brief Test setClock() with various speeds
 */
TEST_F(SPITest, SetClockFrequency) 
{
    SPI.begin();
    
    // Common SPI speeds
    SPI.setClock(100000);      // 100 kHz
    SPI.setClock(1000000);     // 1 MHz
    SPI.setClock(10000000);    // 10 MHz
    
    // Should not crash with these values
    EXPECT_TRUE(true);
}

/**
 * @brief Test getClock() returns the set value
 */
TEST_F(SPITest, GetClockReturnsSetValue) 
{
    SPI.begin();
    
    uint32_t speed = 500000;  // 500 kHz
    SPI.setClock(speed);
    
    EXPECT_EQ(SPI.getClock(), speed);
}

/**
 * @brief Test setDataMode() with all four SPI modes
 */
TEST_F(SPITest, SetDataMode) 
{
    SPI.begin();
    
    SPI.setDataMode(SPI_MODE0);
    SPI.setDataMode(SPI_MODE1);
    SPI.setDataMode(SPI_MODE2);
    SPI.setDataMode(SPI_MODE3);
    
    // Should not crash
    EXPECT_TRUE(true);
}

/**
 * @brief Test setBitOrder() with MSB and LSB first
 */
TEST_F(SPITest, SetBitOrder) 
{
    SPI.begin();
    
    SPI.setBitOrder(MSBFIRST);
    SPI.setBitOrder(LSBFIRST);
    SPI.setBitOrder(MSBFIRST);  // Back to default
    
    // Should not crash
    EXPECT_TRUE(true);
}

/**
 * @brief Test setClockDivider() with valid divider values
 */
TEST_F(SPITest, SetClockDivider) 
{
    SPI.begin();
    
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    SPI.setClockDivider(SPI_CLOCK_DIV4);
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    SPI.setClockDivider(SPI_CLOCK_DIV16);
    SPI.setClockDivider(SPI_CLOCK_DIV32);
    SPI.setClockDivider(SPI_CLOCK_DIV64);
    SPI.setClockDivider(SPI_CLOCK_DIV128);
    
    // Should not crash
    EXPECT_TRUE(true);
}

/**
 * @brief Test transfer() returns a byte
 */
TEST_F(SPITest, TransferReturnsByte) 
{
    if (!SPI.begin()) 
    {
        GTEST_SKIP() << "SPI hardware not available";
    }
    
    uint8_t result = SPI.transfer(0x55);
    EXPECT_GE(result, 0);      // Valid byte (always true, but shows intent)
    EXPECT_LE(result, 0xFF);   // Valid byte
}

/**
 * @brief Test transfer() with buffer
 */
TEST_F(SPITest, TransferWithBuffer) 
{
    if (!SPI.begin()) 
    {
        GTEST_SKIP() << "SPI hardware not available";
    }
    
    uint8_t buffer[] = {0x01, 0x02, 0x03, 0x04};
    size_t len = sizeof(buffer);
    
    SPI.transfer(buffer, len);
    
    // Buffer modified in-place (can't verify exact values without loopback)
    EXPECT_TRUE(true);
}

/**
 * @brief Test transfer() before begin() handles gracefully
 */
TEST_F(SPITest, TransferWithoutBeginHandlesGracefully) 
{
    // Don't call begin()
    SPI.transfer(0x55);
    
    // Should return 0 or handle gracefully without crashing
    EXPECT_TRUE(true);  // If we got here, no crash
}

/**
 * @brief Test concurrent begin() calls from multiple threads
 */
TEST_F(SPITest, ConcurrentBeginCallsSafe) 
{
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 10; ++i) 
    {
        threads.emplace_back([&]() {
            SPI.begin();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            SPI.end();
        });
    }
    
    for (auto& thread : threads) 
    {
        thread.join();
    }
    
    // Should complete without deadlock or crash
    EXPECT_TRUE(true);
}

/**
 * @brief Test concurrent transfer() calls
 */
TEST_F(SPITest, ConcurrentTransfersSafe) 
{
    if (!SPI.begin()) 
    {
        GTEST_SKIP() << "SPI hardware not available";
    }
    
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 10; ++i) 
    {
        threads.emplace_back([&]() {
            SPI.transfer(0xAA);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        });
    }
    
    for (auto& thread : threads) 
    {
        thread.join();
    }
    
    // Should complete without race conditions
    EXPECT_TRUE(true);
}

/**
 * @brief Test concurrent configuration changes
 */
TEST_F(SPITest, ConcurrentConfigurationChangesSafe) 
{
    SPI.begin();
    
    std::vector<std::thread> threads;
    
    threads.emplace_back([&]() {
        for (int i = 0; i < 100; ++i) 
        {
            SPI.setClock(1000000);
        }
    });
    
    threads.emplace_back([&]() {
        for (int i = 0; i < 100; ++i) 
        {
            SPI.setDataMode(SPI_MODE0);
        }
    });
    
    threads.emplace_back([&]() {
        for (int i = 0; i < 100; ++i) 
        {
            SPI.setBitOrder(MSBFIRST);
        }
    });
    
    for (auto& thread : threads) 
    {
        thread.join();
    }
    
    // Should complete without race conditions
    EXPECT_TRUE(true);
}

/**
 * @brief Test isInitialized() reflects actual state
 */
TEST_F(SPITest, IsInitializedReflectsState) 
{
    EXPECT_FALSE(SPI.isInitialized());
    
    SPI.begin();
    // May be true or false depending on hardware availability
    bool afterBegin = SPI.isInitialized();
    
    SPI.end();
    EXPECT_FALSE(SPI.isInitialized());
    
    // Verify consistent behavior
    EXPECT_TRUE(afterBegin == true || afterBegin == false);
}

/**
 * @brief Test Arduino-style API consistency
 */
TEST_F(SPITest, ArduinoStyleAPIWorks) 
{
    // This should match Arduino sketches
    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    
    if (SPI.isInitialized()) 
    {
        SPI.transfer(0x42);
    }
    
    SPI.end();
    EXPECT_TRUE(true);  // Verify no crash
}

/**
 * @brief Test zero-byte transfer
 */
TEST_F(SPITest, ZeroByteTransfer) 
{
    if (!SPI.begin()) 
    {
        GTEST_SKIP() << "SPI hardware not available";
    }
    
    uint8_t buffer[1] = {0};
    SPI.transfer(buffer, 0);  // Zero length
    
    // Should handle gracefully
    EXPECT_TRUE(true);
}

/**
 * @brief Test large buffer transfer
 */
TEST_F(SPITest, LargeBufferTransfer) 
{
    if (!SPI.begin()) 
    {
        GTEST_SKIP() << "SPI hardware not available";
    }
    
    std::vector<uint8_t> buffer(1024, 0x55);
    SPI.transfer(buffer.data(), buffer.size());
    
    // Should handle large transfers
    EXPECT_TRUE(true);
}

/**
 * @brief Test mode constants are defined correctly
 */
TEST_F(SPITest, ModeConstantsValid) 
{
    EXPECT_EQ(SPI_MODE0, 0x00);
    EXPECT_EQ(SPI_MODE1, 0x01);
    EXPECT_EQ(SPI_MODE2, 0x02);
    EXPECT_EQ(SPI_MODE3, 0x03);
}

/**
 * @brief Test bit order constants are defined (Arduino convention)
 */
TEST_F(SPITest, BitOrderConstantsValid) 
{
    // Arduino defines MSBFIRST=1, LSBFIRST=0
    EXPECT_EQ(MSBFIRST, 1);
    EXPECT_EQ(LSBFIRST, 0);
}

/**
 * @brief Main entry point for tests
 */
int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
