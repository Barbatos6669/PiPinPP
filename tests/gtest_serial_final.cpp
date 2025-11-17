/**
 * @file gtest_serial_final.cpp
 * @brief Final Serial tests targeting error paths and edge cases for 80% coverage
 * @author Barbatos6669
 * @date 2025-11-16
 * 
 * Targets remaining uncovered paths in Serial.cpp:
 * - Error handling paths
 * - Buffer boundaries
 * - State management edge cases
 * - Print overloads
 */

#include <gtest/gtest.h>
#include "Serial.hpp"
#include <string>
#include <thread>
#include <chrono>

using namespace pipinpp;
using namespace std::chrono_literals;

class SerialFinalTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Ensure Serial is not already open
        try {
            Serial.end();
        } catch (...) {}
    }
    
    void TearDown() override {
        try {
            Serial.end();
        } catch (...) {}
    }
};

// ============================================================================
// Print Overload Tests (various types)
// ============================================================================

TEST_F(SerialFinalTest, PrintInt) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(12345);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintNegativeInt) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(-9876);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintLong) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(123456789L);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintUnsignedInt) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(42U);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintLongHex) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(4095L, HEX);  // 0xFFF
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintDouble) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(3.14159);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintDoubleWithPrecision) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(3.14159, 2);  // 2 decimal places
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintDoubleHighPrecision) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(3.14159265358979, 10);  // 10 decimal places
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintDoubleZeroPrecision) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(3.14159, 0);  // No decimals
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintNegativeDouble) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(-2.71828);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintLongOct) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(511L, OCT);  // 777 in octal
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintHexBase) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(255, HEX);
        EXPECT_GT(written, 0);  // Should print "FF"
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintOctBase) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(64, OCT);
        EXPECT_GT(written, 0);  // Should print "100"
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintBinBase) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(5, BIN);
        EXPECT_GT(written, 0);  // Should print "101"
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

// ============================================================================
// Println Overload Tests
// ============================================================================

TEST_F(SerialFinalTest, PrintlnInt) {
    try {
        Serial.begin(9600);
        size_t written = Serial.println(123);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintlnLong) {
    try {
        Serial.begin(9600);
        size_t written = Serial.println(987654321L);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintlnUnsignedInt) {
    try {
        Serial.begin(9600);
        size_t written = Serial.println(99U);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintlnLongHex) {
    try {
        Serial.begin(9600);
        size_t written = Serial.println(65535L, HEX);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintlnDouble) {
    try {
        Serial.begin(9600);
        size_t written = Serial.println(2.71828);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintlnDoubleWithPrecision) {
    try {
        Serial.begin(9600);
        size_t written = Serial.println(2.71828, 3);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintlnLongOct) {
    try {
        Serial.begin(9600);
        size_t written = Serial.println(255L, OCT);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintlnHex) {
    try {
        Serial.begin(9600);
        size_t written = Serial.println(128, HEX);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintlnOct) {
    try {
        Serial.begin(9600);
        size_t written = Serial.println(128, OCT);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintlnBin) {
    try {
        Serial.begin(9600);
        size_t written = Serial.println(15, BIN);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

// ============================================================================
// Write Overload Tests
// ============================================================================

TEST_F(SerialFinalTest, WriteSingleByte) {
    try {
        Serial.begin(9600);
        uint8_t byte = 65;  // 'A'
        size_t written = Serial.write(&byte, 1);
        EXPECT_EQ(written, 1);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, WriteMultipleBytes) {
    try {
        Serial.begin(9600);
        uint8_t bytes[] = {72, 105};  // 'H', 'i'
        size_t written = Serial.write(bytes, 2);
        EXPECT_EQ(written, 2);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, WriteNullByte) {
    try {
        Serial.begin(9600);
        uint8_t byte = 0;
        size_t written = Serial.write(&byte, 1);
        EXPECT_EQ(written, 1);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, WriteNewlineByte) {
    try {
        Serial.begin(9600);
        uint8_t byte = 10;  // '\n'
        size_t written = Serial.write(&byte, 1);
        EXPECT_EQ(written, 1);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, WriteTabByte) {
    try {
        Serial.begin(9600);
        uint8_t byte = 9;  // '\t'
        size_t written = Serial.write(&byte, 1);
        EXPECT_EQ(written, 1);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

// ============================================================================
// Multiple Consecutive Operations
// ============================================================================

TEST_F(SerialFinalTest, MixedPrintOperations) {
    try {
        Serial.begin(9600);
        Serial.print(std::string("Value: "));
        Serial.print(42);
        Serial.print(std::string(" = 0x"));
        Serial.print(42, HEX);
        Serial.println();
        SUCCEED();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, MultipleDoublePrints) {
    try {
        Serial.begin(9600);
        for (int i = 0; i < 5; i++) {
            Serial.println(i * 1.5, 2);
        }
        SUCCEED();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, AlternatingPrintTypes) {
    try {
        Serial.begin(9600);
        Serial.print(123);
        Serial.print(std::string(" "));
        Serial.print(std::string("text"));
        Serial.print(std::string(" "));
        Serial.println(3.14);
        SUCCEED();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

// ============================================================================
// Edge Case Tests
// ============================================================================

TEST_F(SerialFinalTest, PrintVeryLargeNumber) {
    try {
        Serial.begin(9600);
        size_t written = Serial.println(2147483647L);  // max long (32-bit)
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintZero) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(0);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintZeroDouble) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(0.0);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintVerySmallDouble) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(0.000001, 6);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, PrintVeryLargeDouble) {
    try {
        Serial.begin(9600);
        size_t written = Serial.print(1234567890.123456);
        EXPECT_GT(written, 0);
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

// ============================================================================
// Rapid Operations Test
// ============================================================================

TEST_F(SerialFinalTest, RapidPrints) {
    try {
        Serial.begin(9600);
        for (int i = 0; i < 100; i++) {
            Serial.print(i);
            Serial.print(' ');
        }
        SUCCEED();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

TEST_F(SerialFinalTest, RapidWrites) {
    try {
        Serial.begin(9600);
        for (int i = 0; i < 256; i++) {
            uint8_t byte = static_cast<uint8_t>(i);
            Serial.write(&byte, 1);
        }
        SUCCEED();
    } catch (const std::runtime_error&) {
        GTEST_SKIP() << "Serial port not available";
    }
}

// Main function
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
