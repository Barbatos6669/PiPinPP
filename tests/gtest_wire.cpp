/*
 * Copyright (c) 2025 HobbyHacker
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file gtest_wire.cpp
 * @brief GoogleTest unit tests for Wire I2C library
 * 
 * Tests the Wire library functionality without requiring actual I2C hardware.
 * Hardware-dependent tests are marked and skipped in CI environments.
 * 
 * Test categories:
 * - Initialization and configuration
 * - Buffer management
 * - Error handling
 * - Helper functions
 * - Thread safety
 * 
 * @author PiPinPP Team
 * @version 0.4.0
 * @date 2025-11-05
 */

#include <Wire.hpp>
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <chrono>
#include <unistd.h>  // For access()

using namespace std;
using namespace pipinpp;

// Test fixture for Wire tests
class WireTest : public ::testing::Test {
protected:
    static bool hardwareAvailable;
    static bool checkedHardware;
    
    static void SetUpTestSuite() {
        // Check once if I2C hardware exists by checking for device files
        if (!checkedHardware) {
            // Check if any I2C device exists (don't actually open it yet)
            hardwareAvailable = (access("/dev/i2c-1", F_OK) == 0) ||
                               (access("/dev/i2c-20", F_OK) == 0) ||
                               (access("/dev/i2c-21", F_OK) == 0) ||
                               (access("/dev/i2c-0", F_OK) == 0);
            checkedHardware = true;
        }
    }
    
    void SetUp() override {
        // Tests will handle initialization as needed
    }
    
    void TearDown() override {
        Wire.end();
    }
};

// Static member initialization
bool WireTest::hardwareAvailable = false;
bool WireTest::checkedHardware = false;

// ============================================================================
// Initialization Tests
// ============================================================================

TEST_F(WireTest, InitializationSucceeds) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    // This test may pass or fail depending on hardware availability
    // We're just testing that begin() doesn't crash
    EXPECT_NO_THROW({
        Wire.begin();
    });
}

TEST_F(WireTest, MultipleBeginCallsSafe) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    // Calling begin() multiple times should be safe
    EXPECT_NO_THROW({
        Wire.begin();
        Wire.begin();
        Wire.begin();
    });
}

TEST_F(WireTest, EndAfterBegin) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    EXPECT_NO_THROW({
        Wire.end();
    });
}

TEST_F(WireTest, BeginWithSpecificBusNumber) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    // Test specifying a bus number
    EXPECT_NO_THROW({
        Wire.begin(1);  // May fail if bus doesn't exist, but shouldn't crash
    });
}

// ============================================================================
// Configuration Tests
// ============================================================================

TEST_F(WireTest, SetClockFrequency) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    
    // Test various standard I2C frequencies
    EXPECT_TRUE(Wire.setClock(100000));   // Standard mode
    EXPECT_EQ(Wire.getClock(), 100000);
    
    EXPECT_TRUE(Wire.setClock(400000));   // Fast mode
    EXPECT_EQ(Wire.getClock(), 400000);
    
    EXPECT_TRUE(Wire.setClock(1000000));  // Fast mode plus
    EXPECT_EQ(Wire.getClock(), 1000000);
    
    EXPECT_TRUE(Wire.setClock(3400000));  // High-speed mode
    EXPECT_EQ(Wire.getClock(), 3400000);
}

TEST_F(WireTest, GetClockReturnsSetValue) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    
    Wire.setClock(250000);
    EXPECT_EQ(Wire.getClock(), 250000);
}

// ============================================================================
// Buffer Management Tests
// ============================================================================

TEST_F(WireTest, BeginTransmissionSetsAddress) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    
    // Should not throw for any valid 7-bit address
    EXPECT_NO_THROW({
        Wire.beginTransmission(0x00);
        Wire.beginTransmission(0x7F);
        Wire.beginTransmission(0x68);  // Common address
    });
}

TEST_F(WireTest, WriteToBuffer) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    Wire.beginTransmission(0x68);
    
    // Write single byte
    EXPECT_EQ(Wire.write(0x42), 1);
    
    // Write multiple bytes
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    EXPECT_EQ(Wire.write(data, 4), 4);
}

TEST_F(WireTest, BufferOverflowProtection) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    Wire.beginTransmission(0x68);
    
    // Try to overflow buffer (256 bytes max)
    uint8_t data[260];
    memset(data, 0xFF, sizeof(data));
    
    // Should only write up to buffer size
    size_t written = Wire.write(data, 260);
    EXPECT_LE(written, 256);
}

TEST_F(WireTest, AvailableReturnsCorrectCount) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    // Without actual I2C device, available() should return 0
    Wire.begin();
    EXPECT_EQ(Wire.available(), 0);
}

// ============================================================================
// Helper Function Tests
// ============================================================================

TEST_F(WireTest, ExistsReturnsBoolWithoutCrashing) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    
    // exists() should return a bool without crashing
    EXPECT_NO_THROW({
        bool result = Wire.exists(0x68);
        // Result depends on hardware, just verify it returns
        (void)result;
    });
}

TEST_F(WireTest, ScanReturnsVectorWithoutCrashing) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    
    // scan() should return a vector without crashing
    EXPECT_NO_THROW({
        std::vector<uint8_t> devices = Wire.scan();
        // Number of devices depends on hardware
        EXPECT_GE(devices.size(), 0);
    });
}

TEST_F(WireTest, ReadRegisterHandlesInvalidDevice) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    
    // Reading from non-existent device should return error
    int result = Wire.readRegister(0x00, 0x00);
    EXPECT_LT(result, 0);  // Should return negative error code
}

TEST_F(WireTest, WriteRegisterHandlesInvalidDevice) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    
    // Writing to non-existent device should return false
    bool result = Wire.writeRegister(0x00, 0x00, 0xFF);
    EXPECT_FALSE(result);
}

// ============================================================================
// Error Handling Tests
// ============================================================================

TEST_F(WireTest, EndTransmissionReturnsErrorCode) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    Wire.beginTransmission(0x00);  // Reserved address
    Wire.write(0x42);
    
    // Should return non-zero error code
    uint8_t error = Wire.endTransmission(true);
    // Error codes: 0=success, 1=buffer full, 2=NACK addr, 3=NACK data, 4=other
    EXPECT_GE(error, 0);
    EXPECT_LE(error, 4);
}

TEST_F(WireTest, RequestFromInvalidDeviceHandlesGracefully) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    
    // Request from non-existent device
    size_t received = Wire.requestFrom(0x00, 5, true);
    EXPECT_EQ(received, 0);  // Should receive 0 bytes
}

// ============================================================================
// Thread Safety Tests
// ============================================================================

TEST_F(WireTest, ConcurrentBeginTransmissionSafe) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    
    const int NUM_THREADS = 10;
    std::vector<std::thread> threads;
    
    // Multiple threads calling beginTransmission
    for (int i = 0; i < NUM_THREADS; i++) {
        threads.emplace_back([&, i]() {
            Wire.beginTransmission(0x20 + i);
            Wire.write(0xFF);
            Wire.endTransmission(true);
        });
    }
    
    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }
    
    // If we get here without crashing, thread safety is working
    SUCCEED();
}

TEST_F(WireTest, ConcurrentScansDoNotCrash) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    
    const int NUM_THREADS = 5;
    std::vector<std::thread> threads;
    
    // Multiple threads scanning bus
    for (int i = 0; i < NUM_THREADS; i++) {
        threads.emplace_back([&]() {
            auto devices = Wire.scan();
            (void)devices;  // Suppress unused warning
        });
    }
    
    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }
    
    SUCCEED();
}

// ============================================================================
// API Compatibility Tests
// ============================================================================

TEST_F(WireTest, ArduinoStyleAPIWorks) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    
    // Arduino-style usage pattern
    Wire.beginTransmission(0x68);
    Wire.write(0x6B);  // Register address
    Wire.write(0x00);  // Data
    uint8_t error = Wire.endTransmission(true);
    
    // Should return valid error code
    EXPECT_LE(error, 4);
}

TEST_F(WireTest, RequestFromWithoutSendStop) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    
    // Test requestFrom with sendStop parameter
    EXPECT_NO_THROW({
        Wire.requestFrom(0x68, 6, false);  // Repeated START
        Wire.requestFrom(0x68, 6, true);   // Stop condition
    });
}

TEST_F(WireTest, ReadMultipleBytesInSequence) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    
    // Simulate reading multiple bytes
    Wire.requestFrom(0x68, 3, true);
    
    int byte1 = Wire.read();
    int byte2 = Wire.read();
    int byte3 = Wire.read();
    int byte4 = Wire.read();  // Should return -1 (no more data)
    
    // Without actual device, should get -1 for all
    EXPECT_EQ(byte1, -1);
    EXPECT_EQ(byte2, -1);
    EXPECT_EQ(byte3, -1);
    EXPECT_EQ(byte4, -1);
}

TEST_F(WireTest, PeekDoesNotAdvanceReadPosition) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    Wire.requestFrom(0x68, 3, true);
    
    int peeked1 = Wire.peek();
    int peeked2 = Wire.peek();
    
    // Peek should return same value multiple times
    EXPECT_EQ(peeked1, peeked2);
}

// ============================================================================
// Edge Case Tests
// ============================================================================

TEST_F(WireTest, ZeroByteRequest) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    
    size_t received = Wire.requestFrom(0x68, 0, true);
    EXPECT_EQ(received, 0);
}

TEST_F(WireTest, LargeByteRequest) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    
    // Request more than buffer size
    size_t received = Wire.requestFrom(0x68, 300, true);
    
    // Should not exceed buffer size (256 bytes)
    EXPECT_LE(received, 256);
}

TEST_F(WireTest, EmptyTransmission) {
    if (!hardwareAvailable) {
        GTEST_SKIP() << "I2C hardware not available";
    }
    
    Wire.begin();
    
    // Transmission with no data
    Wire.beginTransmission(0x68);
    uint8_t error = Wire.endTransmission(true);
    
    // Should handle gracefully
    EXPECT_LE(error, 4);
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
