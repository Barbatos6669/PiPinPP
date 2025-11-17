/*
 * PiPin++ Platform Detection Unit Tests
 * 
 * Tests for platform.hpp/cpp
 * 
 * Author: Barbatos6669
 * Date: November 16, 2025
 */

#include <gtest/gtest.h>
#include "platform.hpp"
#include <fstream>

using namespace pipinpp;

// Test fixture
class PlatformTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Platform is singleton, already initialized
    }
};

// Test singleton pattern
TEST_F(PlatformTest, SingletonInstance) {
    auto& instance1 = PlatformInfo::instance();
    auto& instance2 = PlatformInfo::instance();
    
    // Same instance
    EXPECT_EQ(&instance1, &instance2);
}

// Test platform detection (basic smoke test)
TEST_F(PlatformTest, DetectsPlatform) {
    auto& platform = PlatformInfo::instance();
    
    // Should detect some platform (or UNKNOWN if not recognized)
    Platform detected = platform.getPlatform();
    EXPECT_GE(static_cast<int>(detected), static_cast<int>(Platform::UNKNOWN));
    EXPECT_LE(static_cast<int>(detected), static_cast<int>(Platform::JETSON_NANO));
    
    // Platform name should not be empty
    std::string name = platform.getPlatformName();
    EXPECT_FALSE(name.empty());
}

// Test Raspberry Pi detection
TEST_F(PlatformTest, RaspberryPiDetection) {
    auto& platform = PlatformInfo::instance();
    
    bool isRPi = platform.isRaspberryPi();
    Platform detected = platform.getPlatform();
    
    // Consistency check
    if (detected == Platform::RASPBERRY_PI_3 ||
        detected == Platform::RASPBERRY_PI_4 ||
        detected == Platform::RASPBERRY_PI_5 ||
        detected == Platform::RASPBERRY_PI_CM4 ||
        detected == Platform::RASPBERRY_PI_ZERO ||
        detected == Platform::RASPBERRY_PI_ZERO2) {
        EXPECT_TRUE(isRPi);
    } else {
        EXPECT_FALSE(isRPi);
    }
}

// Test GPIO chip detection
TEST_F(PlatformTest, DetectsGPIOChips) {
    auto& platform = PlatformInfo::instance();
    const auto& caps = platform.getCapabilities();
    
    // Should detect at least one GPIO chip on supported platforms
    // (or none if running in container/VM)
    if (platform.isSupported()) {
        // On real hardware, expect at least gpiochip0
        if (!caps.gpioChips.empty()) {
            EXPECT_GE(caps.gpioChips.size(), 1);
            
            // First chip should have reasonable number of lines
            EXPECT_GT(caps.gpioChips[0].numLines, 0);
            EXPECT_LE(caps.gpioChips[0].numLines, 256);  // Sanity check
            
            // Chip name should be formatted correctly
            EXPECT_TRUE(caps.gpioChips[0].name.find("gpiochip") == 0);
        }
    }
}

// Test I2C bus detection
TEST_F(PlatformTest, DetectsI2CBuses) {
    auto& platform = PlatformInfo::instance();
    const auto& caps = platform.getCapabilities();
    
    // I2C buses may or may not be present depending on configuration
    for (const auto& bus : caps.i2cBuses) {
        // Bus number should be valid
        EXPECT_GE(bus.busNumber, 0);
        EXPECT_LE(bus.busNumber, 20);  // Pi 5 uses up to /dev/i2c-20
        
        // Device path should be formatted correctly
        std::string expectedPath = "/dev/i2c-" + std::to_string(bus.busNumber);
        EXPECT_EQ(bus.devicePath, expectedPath);
    }
}

// Test default GPIO chip
TEST_F(PlatformTest, DefaultGPIOChip) {
    auto& platform = PlatformInfo::instance();
    std::string defaultChip = platform.getDefaultGPIOChip();
    
    // Should return something
    EXPECT_FALSE(defaultChip.empty());
    
    // Should be formatted as "gpiochipN"
    EXPECT_TRUE(defaultChip.find("gpiochip") == 0);
}

// Test default I2C bus
TEST_F(PlatformTest, DefaultI2CBus) {
    auto& platform = PlatformInfo::instance();
    int defaultBus = platform.getDefaultI2CBus();
    
    // Should be reasonable bus number
    EXPECT_GE(defaultBus, 0);
    EXPECT_LE(defaultBus, 20);
    
    // Pi 5 should default to bus 20
    if (platform.getPlatform() == Platform::RASPBERRY_PI_5) {
        const auto& caps = platform.getCapabilities();
        bool hasBus20 = false;
        for (const auto& bus : caps.i2cBuses) {
            if (bus.busNumber == 20 && bus.available) {
                hasBus20 = true;
                break;
            }
        }
        
        if (hasBus20) {
            EXPECT_EQ(defaultBus, 20);
        }
    }
}

// Test kernel version detection
TEST_F(PlatformTest, DetectsKernelVersion) {
    auto& platform = PlatformInfo::instance();
    std::string kernelVersion = platform.getKernelVersion();
    
    // Should not be empty
    EXPECT_FALSE(kernelVersion.empty());
    
    // Should contain at least one digit (version number)
    bool hasDigit = false;
    for (char c : kernelVersion) {
        if (std::isdigit(c)) {
            hasDigit = true;
            break;
        }
    }
    EXPECT_TRUE(hasDigit);
}

// Test libgpiod version detection
TEST_F(PlatformTest, DetectsLibgpiodVersion) {
    auto& platform = PlatformInfo::instance();
    std::string libgpiodVersion = platform.getLibgpiodVersion();
    
    // Should not be empty
    EXPECT_FALSE(libgpiodVersion.empty());
    
    // Should contain version number (e.g., "2.2.1")
    bool hasDigit = false;
    for (char c : libgpiodVersion) {
        if (std::isdigit(c)) {
            hasDigit = true;
            break;
        }
    }
    EXPECT_TRUE(hasDigit);
}

// Test PWM channel detection
TEST_F(PlatformTest, DetectsPWMChannels) {
    auto& platform = PlatformInfo::instance();
    const auto& caps = platform.getCapabilities();
    
    // PWM channels may or may not be present
    for (const auto& pwm : caps.pwmChannels) {
        // Chip and channel numbers should be valid
        EXPECT_GE(pwm.chip, 0);
        EXPECT_GE(pwm.channel, 0);
        
        // GPIO pin should be -1 (unmapped) or valid pin number
        EXPECT_GE(pwm.gpioPin, -1);
        if (pwm.gpioPin >= 0) {
            EXPECT_LE(pwm.gpioPin, 27);  // Raspberry Pi has 0-27
        }
        
        // Sysfs path should contain "pwm"
        EXPECT_TRUE(pwm.sysfsPath.find("pwm") != std::string::npos);
    }
}

// Test DMA support detection
TEST_F(PlatformTest, DetectsDMASupport) {
    auto& platform = PlatformInfo::instance();
    const auto& caps = platform.getCapabilities();
    
    // DMA should only be available on Raspberry Pi
    if (caps.hasDMASupport) {
        EXPECT_TRUE(platform.isRaspberryPi());
        
        // Peripheral base should be non-zero
        EXPECT_GT(caps.peripheralBase, 0);
        
        // Should be one of the known Pi addresses
        bool validAddress = 
            caps.peripheralBase == 0x3F000000 ||  // Pi 3
            caps.peripheralBase == 0xFE000000 ||  // Pi 4
            (caps.peripheralBase & 0xFFFFFFFF) == (0x1F00000000ULL & 0xFFFFFFFF);  // Pi 5 (truncated)
        
        EXPECT_TRUE(validAddress);
    }
    
    // Non-RPi platforms should not have DMA support
    if (!platform.isRaspberryPi()) {
        EXPECT_FALSE(caps.hasDMASupport);
        EXPECT_EQ(caps.peripheralBase, 0);
    }
}

// Test total GPIO pin count
TEST_F(PlatformTest, TotalGPIOPins) {
    auto& platform = PlatformInfo::instance();
    const auto& caps = platform.getCapabilities();
    
    // Total pins should equal sum of all chip lines
    int expectedTotal = 0;
    for (const auto& chip : caps.gpioChips) {
        expectedTotal += chip.numLines;
    }
    
    EXPECT_EQ(caps.totalGPIOPins, expectedTotal);
}

// Test platform name consistency
TEST_F(PlatformTest, PlatformNameConsistency) {
    auto& platform = PlatformInfo::instance();
    
    std::string name = platform.getPlatformName();
    Platform detected = platform.getPlatform();
    
    // Name should match platform enum
    switch (detected) {
        case Platform::RASPBERRY_PI_3:
            EXPECT_EQ(name, "Raspberry Pi 3");
            break;
        case Platform::RASPBERRY_PI_4:
            EXPECT_EQ(name, "Raspberry Pi 4");
            break;
        case Platform::RASPBERRY_PI_5:
            EXPECT_EQ(name, "Raspberry Pi 5");
            break;
        case Platform::RASPBERRY_PI_CM4:
            EXPECT_EQ(name, "Raspberry Pi Compute Module 4");
            break;
        case Platform::RASPBERRY_PI_ZERO:
            EXPECT_EQ(name, "Raspberry Pi Zero");
            break;
        case Platform::RASPBERRY_PI_ZERO2:
            EXPECT_EQ(name, "Raspberry Pi Zero 2");
            break;
        case Platform::ORANGE_PI:
            EXPECT_EQ(name, "Orange Pi");
            break;
        case Platform::BEAGLEBONE:
            EXPECT_EQ(name, "BeagleBone");
            break;
        case Platform::JETSON_NANO:
            EXPECT_EQ(name, "NVIDIA Jetson Nano");
            break;
        case Platform::UNKNOWN:
            EXPECT_EQ(name, "Unknown Platform");
            break;
    }
}

// Test refresh functionality
TEST_F(PlatformTest, RefreshWorks) {
    auto& platform = PlatformInfo::instance();
    
    // Get initial state
    Platform before = platform.getPlatform();
    std::string nameBefore = platform.getPlatformName();
    
    // Refresh
    platform.refresh();
    
    // Should detect same platform (hardware didn't change)
    Platform after = platform.getPlatform();
    std::string nameAfter = platform.getPlatformName();
    
    EXPECT_EQ(before, after);
    EXPECT_EQ(nameBefore, nameAfter);
}

// Test printInfo (smoke test - just verify it doesn't crash)
TEST_F(PlatformTest, PrintInfoDoesNotCrash) {
    auto& platform = PlatformInfo::instance();
    
    // Redirect stdout to avoid cluttering test output
    std::streambuf* oldCout = std::cout.rdbuf();
    std::ostringstream capturedOutput;
    std::cout.rdbuf(capturedOutput.rdbuf());
    
    // Should not crash
    EXPECT_NO_THROW(platform.printInfo());
    
    // Restore stdout
    std::cout.rdbuf(oldCout);
    
    // Output should contain platform name
    std::string output = capturedOutput.str();
    EXPECT_TRUE(output.find(platform.getPlatformName()) != std::string::npos);
}

// Test isSupported
TEST_F(PlatformTest, SupportedPlatforms) {
    auto& platform = PlatformInfo::instance();
    
    bool supported = platform.isSupported();
    Platform detected = platform.getPlatform();
    
    // Consistency check
    if (detected == Platform::UNKNOWN) {
        EXPECT_FALSE(supported);
    } else {
        EXPECT_TRUE(supported);
    }
}

int main(int argc, char** argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
