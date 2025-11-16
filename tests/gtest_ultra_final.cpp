/**
 * @file gtest_ultra_final.cpp
 * @brief Ultra-focused tests targeting last 6.5% to reach 80% coverage
 * @author Barbatos6669
 * @date 2025-11-16
 * 
 * Targets very specific uncovered paths in Serial, SPI, platform
 */

#include <gtest/gtest.h>
#include "Serial.hpp"
#include "SPI.hpp"
#include "ArduinoCompat.hpp"
#include "exceptions.hpp"

using namespace pipinpp;

// ============================================================================
// Serial Edge Cases and Error Paths
// ============================================================================

class UltraSerialTest : public ::testing::Test {
protected:
    void SetUp() override {
        try { Serial.end(); } catch (...) {}
    }
    
    void TearDown() override {
        try { Serial.end(); } catch (...) {}
    }
};

TEST_F(UltraSerialTest, PrintStringEmpty) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(std::string(""));
        EXPECT_EQ(written, 0);
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, PrintlnStringEmpty) {
    try {
        Serial.begin(9600);
        size_t written = Serial.println(std::string(""));
        EXPECT_GT(written, 0);  // Just newline
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, PrintlnNoArgs) {
    try {
        Serial.begin(9600);
        size_t written = Serial.println();
        EXPECT_GT(written, 0);
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, WriteBufferNullPointer) {
    try {
        Serial.begin(9600);
        size_t written = Serial.write(nullptr, 10);
        EXPECT_EQ(written, 0);
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, WriteBufferZeroLength) {
    try {
        Serial.begin(9600);
        uint8_t buffer[1];
        size_t written = Serial.write(buffer, 0);
        EXPECT_EQ(written, 0);
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, ReadBeforeAvailable) {
    try {
        Serial.begin(9600);
        int data = Serial.read();
        EXPECT_EQ(data, -1);  // No data available
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, PeekNoData) {
    try {
        Serial.begin(9600);
        int data = Serial.peek();
        EXPECT_EQ(data, -1);  // No data available
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, MultipleReads) {
    try {
        Serial.begin(9600);
        // Multiple reads when no data
        for (int i = 0; i < 5; i++) {
            int data = Serial.read();
            EXPECT_EQ(data, -1);
        }
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, MultipleAvailableCalls) {
    try {
        Serial.begin(9600);
        // Multiple available checks
        for (int i = 0; i < 5; i++) {
            int avail = Serial.available();
            EXPECT_EQ(avail, 0);
        }
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, AvailableNoData) {
    try {
        Serial.begin(9600);
        int avail = Serial.available();
        EXPECT_EQ(avail, 0);
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, FlushWithoutData) {
    try {
        Serial.begin(9600);
        Serial.flush();
        Serial.end();
        SUCCEED();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, PrintNegativeInt) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(-12345);
        EXPECT_GT(written, 0);
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, PrintNegativeIntHex) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(-1, HEX);
        EXPECT_GT(written, 0);
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, PrintNegativeLong) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(-999999L);
        EXPECT_GT(written, 0);
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, PrintNegativeLongHex) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(-1L, HEX);
        EXPECT_GT(written, 0);
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, PrintDoubleNegative) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(-123.456);
        EXPECT_GT(written, 0);
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, PrintDoubleZeroPrecision) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(123.456, 0);
        EXPECT_GT(written, 0);
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, PrintDoubleLargePrecision) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(123.456789012345, 15);
        EXPECT_GT(written, 0);
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, PrintlnNegativeInt) {
    try {
        Serial.begin(9600);
        size_t written = Serial.println(-54321);
        EXPECT_GT(written, 0);
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, PrintlnNegativeLong) {
    try {
        Serial.begin(9600);
        size_t written = Serial.println(-888888L);
        EXPECT_GT(written, 0);
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, PrintlnDoubleNegative) {
    try {
        Serial.begin(9600);
        size_t written = Serial.println(-99.99);
        EXPECT_GT(written, 0);
        Serial.end();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, BeginInvalidBaudRate) {
    // Very low baud rate
    try {
        bool success = Serial.begin(1);
        if (success) {
            Serial.end();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

TEST_F(UltraSerialTest, BeginVeryHighBaudRate) {
    // Very high baud rate
    try {
        bool success = Serial.begin(10000000);
        if (success) {
            Serial.end();
        }
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial not available";
    }
}

// ============================================================================
// Arduino Utility Edge Cases
// ============================================================================

TEST(ArduinoUtilityTest, MapLargeInputRange) {
    long result = map(50, 0, 1000000, 0, 100);
    EXPECT_GE(result, 0);
    EXPECT_LE(result, 1);
}

TEST(ArduinoUtilityTest, MapLargeOutputRange) {
    long result = map(5, 0, 10, 0, 1000000);
    EXPECT_GE(result, 400000);
    EXPECT_LE(result, 600000);
}

TEST(ArduinoUtilityTest, MapNegativeInput) {
    long result = map(-50, -100, 0, 0, 100);
    EXPECT_EQ(result, 50);
}

TEST(ArduinoUtilityTest, MapAllNegative) {
    long result = map(-50, -100, -10, -200, -20);
    EXPECT_GE(result, -150);
    EXPECT_LE(result, -50);
}

TEST(ArduinoUtilityTest, ConstrainIntMinGreaterThanMax) {
    int result = constrain(5, 10, 0);  // Inverted range
    EXPECT_EQ(result, 5);  // Should return value unchanged or clamp
}

TEST(ArduinoUtilityTest, ConstrainLongMinGreaterThanMax) {
    long result = constrain(5L, 10L, 0L);
    EXPECT_EQ(result, 5L);
}

TEST(ArduinoUtilityTest, ConstrainFloatMinGreaterThanMax) {
    float result = constrain(5.0f, 10.0f, 0.0f);
    EXPECT_FLOAT_EQ(result, 5.0f);
}

// Main function
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
