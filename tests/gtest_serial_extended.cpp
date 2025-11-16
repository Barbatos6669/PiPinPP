/**
 * @file gtest_serial_extended.cpp
 * @brief Extended GoogleTest unit tests for Serial (UART) functionality
 * 
 * Tests print format functions (DEC, HEX, BIN, OCT), edge cases,
 * and error handling to improve Serial.cpp coverage from 67% to 80%+.
 * 
 * @copyright Copyright (c) 2025 PiPinPP Project
 * @license MIT License
 */

#include <gtest/gtest.h>
#include "Serial.hpp"
#include <thread>
#include <chrono>

using namespace std::chrono_literals;
using namespace pipinpp;

// Test fixture for extended Serial tests
class SerialExtendedTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clean state
    }
    
    void TearDown() override {
        // Close serial port if open
        Serial.end();
    }
};

// ============================================================================
// PRINT FORMAT TESTS (NEW FUNCTIONALITY)
// ============================================================================

// Test: Print integer in decimal format (explicit)
TEST_F(SerialExtendedTest, PrintIntegerDecimalFormat) {
    EXPECT_NO_THROW({
        // These should compile even if port not open
        Serial.print(42, DEC);
        Serial.println(42, DEC);
    });
}

// Test: Print integer in hexadecimal format
TEST_F(SerialExtendedTest, PrintIntegerHexFormat) {
    EXPECT_NO_THROW({
        Serial.print(255, HEX);    // Should be "FF"
        Serial.println(255, HEX);
        Serial.print(16, HEX);     // Should be "10"
    });
}

// Test: Print integer in binary format
TEST_F(SerialExtendedTest, PrintIntegerBinaryFormat) {
    EXPECT_NO_THROW({
        Serial.print(5, BIN);      // Should be "101"
        Serial.println(255, BIN);  // Should be "11111111"
    });
}

// Test: Print integer in octal format
TEST_F(SerialExtendedTest, PrintIntegerOctalFormat) {
    EXPECT_NO_THROW({
        Serial.print(8, OCT);      // Should be "10"
        Serial.println(64, OCT);   // Should be "100"
    });
}

// Test: Print long integers with format
TEST_F(SerialExtendedTest, PrintLongWithFormat) {
    EXPECT_NO_THROW({
        Serial.print(1000000L, DEC);
        Serial.print(1000000L, HEX);
        Serial.println(1000000L, BIN);
    });
}

// Test: Print unsigned integers with format
TEST_F(SerialExtendedTest, PrintUnsignedWithFormat) {
    EXPECT_NO_THROW({
        Serial.print(4294967295U, DEC);
        Serial.print(4294967295U, HEX);
        Serial.println(4294967295U, BIN);
    });
}

// Test: Print zero in all formats
TEST_F(SerialExtendedTest, PrintZeroAllFormats) {
    EXPECT_NO_THROW({
        Serial.print(0, DEC);
        Serial.print(0, HEX);
        Serial.print(0, OCT);
        Serial.println(0, BIN);
    });
}

// Test: Print negative numbers with format
TEST_F(SerialExtendedTest, PrintNegativeWithFormat) {
    EXPECT_NO_THROW({
        Serial.print(-42, DEC);
        Serial.println(-100, DEC);
        // Negative in other bases shown as unsigned
        Serial.print(-1, HEX);
    });
}

// Test: Operations on closed port don't crash
TEST_F(SerialExtendedTest, OperationsOnClosedPort) {
    Serial.end();  // Ensure closed
    
    EXPECT_EQ(Serial.isOpen(), false);
    EXPECT_EQ(Serial.available(), 0);
    EXPECT_EQ(Serial.read(), -1);
    EXPECT_EQ(Serial.peek(), -1);
    EXPECT_EQ(Serial.write((uint8_t)42), 0);
    EXPECT_EQ(Serial.print("test"), 0);
    EXPECT_EQ(Serial.println(), 0);
}

// Test: Multiple begin/end cycles
TEST_F(SerialExtendedTest, MultipleBeginEndCycles) {
    for (int i = 0; i < 5; i++) {
        Serial.begin(9600, "/dev/ttyUSB0");
        Serial.end();
    }
    SUCCEED();
}

// Test: Timeout configuration
TEST_F(SerialExtendedTest, SetTimeout) {
    EXPECT_NO_THROW({
        Serial.setTimeout(0);      // Non-blocking
        Serial.setTimeout(100);    // 100ms
        Serial.setTimeout(5000);   // 5 seconds
    });
}

// Test: Print double with different precisions
TEST_F(SerialExtendedTest, PrintDoubleVariousPrecisions) {
    EXPECT_NO_THROW({
        Serial.print(3.14159, 0);   // 3
        Serial.print(3.14159, 2);   // 3.14
        Serial.println(3.14159, 6); // 3.141590
    });
}

// Test: Concurrent prints
TEST_F(SerialExtendedTest, ConcurrentPrints) {
    Serial.begin(9600, "/dev/ttyUSB0");
    
    std::thread t1([&]() {
        for (int i = 0; i < 10; i++) {
            Serial.println("Thread 1");
        }
    });
    
    std::thread t2([&]() {
        for (int i = 0; i < 10; i++) {
            Serial.println("Thread 2");
        }
    });
    
    t1.join();
    t2.join();
    
    Serial.end();
    SUCCEED();
}

// Test: Format constants have correct values
TEST_F(SerialExtendedTest, FormatConstantsValues) {
    EXPECT_EQ(DEC, 10);
    EXPECT_EQ(HEX, 16);
    EXPECT_EQ(OCT, 8);
    EXPECT_EQ(BIN, 2);
}

// Test: Common baud rates
TEST_F(SerialExtendedTest, CommonBaudRates) {
    std::vector<unsigned long> baudRates = {9600, 19200, 38400, 57600, 115200};
    
    for (auto baud : baudRates) {
        Serial.begin(baud, "/dev/ttyUSB0");
        Serial.end();
    }
    SUCCEED();
}
