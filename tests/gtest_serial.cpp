/**
 * @file gtest_serial.cpp
 * @brief Unit tests for Serial (UART) communication
 * @author Barbatos6669
 * @date 2025-11-16
 * 
 * Tests Serial API without requiring actual hardware.
 * Hardware-dependent tests are skipped if no device available.
 */

#include <gtest/gtest.h>
#include "Serial.hpp"
#include <string>
#include <thread>
#include <chrono>

using namespace pipinpp;

/**
 * Test fixture for Serial tests
 */
class SerialTest : public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        // Reset global Serial instance
        Serial.end();
    }
    
    void TearDown() override 
    {
        // Clean up after each test
        Serial.end();
    }
};

// ============================================================================
// Basic API Tests (No Hardware Required)
// ============================================================================

TEST_F(SerialTest, InitiallyNotOpen)
{
    EXPECT_FALSE(Serial.isOpen());
}

TEST_F(SerialTest, BeginReturnsBoolean)
{
    // Should return false if device doesn't exist
    bool result = Serial.begin(9600, "/dev/nonexistent_serial_device_12345");
    EXPECT_FALSE(result);
    EXPECT_FALSE(Serial.isOpen());
}

TEST_F(SerialTest, BeginWithInvalidDevice)
{
    EXPECT_FALSE(Serial.begin(9600, "/invalid/path/to/serial"));
    EXPECT_FALSE(Serial.isOpen());
}

TEST_F(SerialTest, EndWithoutBeginSafe)
{
    EXPECT_NO_THROW(Serial.end());
}

TEST_F(SerialTest, MultipleEndCallsSafe)
{
    Serial.end();
    EXPECT_NO_THROW(Serial.end());
    EXPECT_NO_THROW(Serial.end());
}

TEST_F(SerialTest, GetBaudRateWhenNotOpen)
{
    EXPECT_EQ(Serial.getBaudRate(), 0);
}

TEST_F(SerialTest, AvailableReturnsZeroWhenNotOpen)
{
    EXPECT_EQ(Serial.available(), 0);
}

TEST_F(SerialTest, ReadReturnsErrorWhenNotOpen)
{
    EXPECT_EQ(Serial.read(), -1);
}

TEST_F(SerialTest, PeekReturnsErrorWhenNotOpen)
{
    EXPECT_EQ(Serial.peek(), -1);
}

TEST_F(SerialTest, WriteReturnsZeroWhenNotOpen)
{
    EXPECT_EQ(Serial.write((uint8_t)0x42), 0);
}

TEST_F(SerialTest, WriteStringReturnsZeroWhenNotOpen)
{
    EXPECT_EQ(Serial.write("test"), 0);
}

TEST_F(SerialTest, PrintReturnsZeroWhenNotOpen)
{
    EXPECT_EQ(Serial.print("test"), 0);
    EXPECT_EQ(Serial.print(123), 0);
    EXPECT_EQ(Serial.print(3.14), 0);
}

TEST_F(SerialTest, PrintlnReturnsZeroWhenNotOpen)
{
    EXPECT_EQ(Serial.println("test"), 0);
    EXPECT_EQ(Serial.println(456), 0);
    EXPECT_EQ(Serial.println(2.718), 0);
    EXPECT_EQ(Serial.println(), 0);
}

TEST_F(SerialTest, ReadStringReturnsEmptyWhenNotOpen)
{
    std::string result = Serial.readString();
    EXPECT_TRUE(result.empty());
}

TEST_F(SerialTest, ReadStringUntilReturnsEmptyWhenNotOpen)
{
    std::string result = Serial.readStringUntil('\n');
    EXPECT_TRUE(result.empty());
}

TEST_F(SerialTest, SetTimeoutWithoutCrashing)
{
    EXPECT_NO_THROW(Serial.setTimeout(500));
    EXPECT_NO_THROW(Serial.setTimeout(0));
    EXPECT_NO_THROW(Serial.setTimeout(10000));
}

TEST_F(SerialTest, FlushWithoutBeginSafe)
{
    EXPECT_NO_THROW(Serial.flush());
}

// ============================================================================
// Thread Safety Tests
// ============================================================================

TEST_F(SerialTest, ConcurrentBeginCallsSafe)
{
    auto beginTask = []() {
        for (int i = 0; i < 10; ++i) {
            Serial.begin(9600, "/dev/nonexistent");
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    };
    
    std::thread t1(beginTask);
    std::thread t2(beginTask);
    
    t1.join();
    t2.join();
    
    // Should complete without crashing
    SUCCEED();
}

TEST_F(SerialTest, ConcurrentEndCallsSafe)
{
    auto endTask = []() {
        for (int i = 0; i < 10; ++i) {
            Serial.end();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    };
    
    std::thread t1(endTask);
    std::thread t2(endTask);
    
    t1.join();
    t2.join();
    
    SUCCEED();
}

TEST_F(SerialTest, ConcurrentIsOpenSafe)
{
    auto checkTask = []() {
        for (int i = 0; i < 100; ++i) {
            bool status = Serial.isOpen();
            (void)status;  // Suppress unused warning
        }
    };
    
    std::thread t1(checkTask);
    std::thread t2(checkTask);
    std::thread t3(checkTask);
    
    t1.join();
    t2.join();
    t3.join();
    
    SUCCEED();
}

// ============================================================================
// Arduino API Compatibility Tests
// ============================================================================

TEST_F(SerialTest, ArduinoStyleAPIExists)
{
    // Verify all Arduino Serial methods compile and exist
    EXPECT_NO_THROW({
        Serial.begin(9600);
        Serial.isOpen();
        Serial.available();
        Serial.read();
        Serial.peek();
        Serial.write('A');
        Serial.print("test");
        Serial.println("test");
        Serial.setTimeout(1000);
        Serial.flush();
        Serial.end();
    });
}

TEST_F(SerialTest, PrintNumberFormatting)
{
    // Even without open port, verify print formatting works
    // (Returns 0 bytes written but doesn't crash)
    EXPECT_EQ(Serial.print(123), 0);
    EXPECT_EQ(Serial.print(-456), 0);
    EXPECT_EQ(Serial.print(3.14159, 2), 0);
    EXPECT_EQ(Serial.print(3.14159, 4), 0);
    EXPECT_EQ(Serial.println(789), 0);
}

// ============================================================================
// Hardware Tests (Skipped if no device available)
// ============================================================================

TEST_F(SerialTest, LoopbackTest)
{
    // This test requires a physical loopback (TX connected to RX)
    // or a USB-to-serial adapter with RX/TX shorted
    
    // Try common serial devices
    bool opened = false;
    std::vector<std::string> devices = {
        "/dev/ttyUSB0",
        "/dev/ttyACM0",
        "/dev/ttyAMA0"
    };
    
    for (const auto& device : devices) {
        if (Serial.begin(115200, device)) {
            opened = true;
            break;
        }
    }
    
    if (!opened) {
        GTEST_SKIP() << "No serial device available for hardware test";
    }
    
    // Small delay for port to stabilize
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Clear any existing data
    while (Serial.available()) {
        Serial.read();
    }
    
    // Write test data
    const char testByte = 'A';
    size_t written = Serial.write((uint8_t)testByte);
    EXPECT_EQ(written, 1);
    
    // Flush to ensure transmission
    Serial.flush();
    
    // Wait for loopback
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Read back (only works if RX connected to TX)
    if (Serial.available() > 0) {
        int received = Serial.read();
        EXPECT_EQ(received, testByte);
    } else {
        GTEST_SKIP() << "No loopback detected (connect RX to TX for full test)";
    }
    
    Serial.end();
}

TEST_F(SerialTest, BaudRatePreserved)
{
    // Try to open any available device
    std::vector<std::string> devices = {"/dev/ttyUSB0", "/dev/ttyACM0"};
    
    for (const auto& device : devices) {
        if (Serial.begin(115200, device)) {
            EXPECT_EQ(Serial.getBaudRate(), 115200);
            Serial.end();
            return;
        }
    }
    
    GTEST_SKIP() << "No serial device available";
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(SerialTest, WriteEmptyString)
{
    EXPECT_EQ(Serial.write(""), 0);
}

TEST_F(SerialTest, WriteNullBuffer)
{
    EXPECT_EQ(Serial.write(nullptr, 10), 0);
}

TEST_F(SerialTest, WriteZeroBytes)
{
    uint8_t buffer[] = {1, 2, 3};
    EXPECT_EQ(Serial.write(buffer, 0), 0);
}

TEST_F(SerialTest, ReadStringUntilWithUncommonTerminator)
{
    // Should handle any terminator character
    EXPECT_NO_THROW({
        Serial.readStringUntil('\0');
        Serial.readStringUntil('\r');
        Serial.readStringUntil('|');
    });
}

TEST_F(SerialTest, ZeroTimeoutBehavior)
{
    Serial.setTimeout(0);
    // Read with zero timeout should return immediately
    EXPECT_EQ(Serial.read(), -1);
}

TEST_F(SerialTest, LargeTimeoutValue)
{
    EXPECT_NO_THROW(Serial.setTimeout(999999999));
}

// ============================================================================
// State Consistency Tests
// ============================================================================

TEST_F(SerialTest, StateConsistencyAfterFailedBegin)
{
    EXPECT_FALSE(Serial.begin(9600, "/dev/nonexistent"));
    EXPECT_FALSE(Serial.isOpen());
    EXPECT_EQ(Serial.getBaudRate(), 0);
    EXPECT_EQ(Serial.available(), 0);
}

TEST_F(SerialTest, StateConsistencyAfterEnd)
{
    // Even if begin failed, end should leave consistent state
    Serial.begin(9600, "/dev/nonexistent");
    Serial.end();
    
    EXPECT_FALSE(Serial.isOpen());
    EXPECT_EQ(Serial.getBaudRate(), 0);
    EXPECT_EQ(Serial.available(), 0);
}

TEST_F(SerialTest, RepeatedBeginEndCycle)
{
    for (int i = 0; i < 10; ++i) {
        Serial.begin(9600, "/dev/nonexistent");
        Serial.end();
    }
    
    EXPECT_FALSE(Serial.isOpen());
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char** argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
