/*
 * PiPin++ Platform Detection Example
 * 
 * Demonstrates automatic hardware platform detection and capability discovery.
 * 
 * This example shows how PiPinPP can automatically detect:
 * - Raspberry Pi model (3, 4, 5, Zero, CM4)
 * - Available GPIO chips
 * - I2C buses
 * - PWM channels
 * - DMA support
 * - Kernel and libgpiod versions
 * 
 * Usage:
 *   ./platform_detection
 * 
 * No arguments needed - auto-detects everything.
 * 
 * Author: Barbatos6669
 * Date: November 16, 2025
 */

#include "platform.hpp"
#include <iostream>
#include <iomanip>

int main() 
{
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║          PiPin++ Platform Detection Demonstration              ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    
    try {
        // Get platform info singleton
        auto& platform = pipinpp::PlatformInfo::instance();
        
        // Print comprehensive platform information
        platform.printInfo();
        
        // Additional checks
        if (!platform.isSupported()) {
            std::cout << "⚠️  Warning: Platform not recognized. PiPinPP may have limited functionality.\n";
            std::cout << "   Defaulting to generic Linux GPIO behavior.\n";
            std::cout << "\n";
        }
        
        // Show recommended defaults
        std::cout << "Recommended Defaults:\n";
        std::cout << "  GPIO Chip:     " << platform.getDefaultGPIOChip() << "\n";
        std::cout << "  I2C Bus:       /dev/i2c-" << platform.getDefaultI2CBus() << "\n";
        std::cout << "\n";
        
        // Platform-specific notes
        if (platform.getPlatform() == pipinpp::Platform::RASPBERRY_PI_5) {
            std::cout << "📝 Raspberry Pi 5 Notes:\n";
            std::cout << "   • Uses RP1 I/O controller (different architecture)\n";
            std::cout << "   • Default I2C bus is /dev/i2c-20 (not /dev/i2c-1)\n";
            std::cout << "   • DMA GPIO requires 64-bit addressing (experimental)\n";
            std::cout << "\n";
        }
        
        if (platform.getPlatform() == pipinpp::Platform::RASPBERRY_PI_4 ||
            platform.getPlatform() == pipinpp::Platform::RASPBERRY_PI_CM4) {
            std::cout << "📝 Raspberry Pi 4 Notes:\n";
            std::cout << "   • Best platform for DMA GPIO (well-tested)\n";
            std::cout << "   • Hardware PWM on GPIO 18, 19 (PWM0, PWM1)\n";
            std::cout << "   • I2C buses: /dev/i2c-1 (default), /dev/i2c-20 (GPIO 6/7 via dtoverlay)\n";
            std::cout << "\n";
        }
        
        if (platform.getPlatform() == pipinpp::Platform::RASPBERRY_PI_3) {
            std::cout << "📝 Raspberry Pi 3 Notes:\n";
            std::cout << "   • Different peripheral base address (0x3F000000)\n";
            std::cout << "   • DMA GPIO supported but less tested than Pi 4\n";
            std::cout << "   • Hardware PWM on GPIO 18, 19\n";
            std::cout << "\n";
        }
        
        // Capability warnings
        const auto& caps = platform.getCapabilities();
        
        if (caps.gpioChips.empty()) {
            std::cout << "❌ No GPIO chips detected!\n";
            std::cout << "   PiPinPP requires libgpiod and /dev/gpiochip* devices.\n";
            std::cout << "   Check kernel config: CONFIG_GPIO_CDEV=y\n";
            std::cout << "\n";
        }
        
        if (caps.i2cBuses.empty()) {
            std::cout << "⚠️  No I2C buses detected.\n";
            std::cout << "   Enable I2C in raspi-config or device tree.\n";
            std::cout << "\n";
        }
        
        if (caps.pwmChannels.empty()) {
            std::cout << "⚠️  No hardware PWM detected.\n";
            std::cout << "   PiPinPP will use software PWM (EventPWM).\n";
            std::cout << "\n";
        }
        
        if (!caps.hasDMASupport) {
            std::cout << "⚠️  DMA GPIO not available.\n";
            std::cout << "   Reasons:\n";
            std::cout << "   • Platform not supported (only Raspberry Pi)\n";
            std::cout << "   • /dev/mem not accessible (requires root)\n";
            std::cout << "   • Unknown peripheral base address\n";
            std::cout << "   PiPinPP will use EventPWM instead (still <5% CPU).\n";
            std::cout << "\n";
        }
        
        // Usage examples
        std::cout << "Example Code Snippets:\n";
        std::cout << "───────────────────────────────────────────────────────────────\n";
        std::cout << "\n";
        
        std::cout << "// Check if running on Raspberry Pi\n";
        std::cout << "if (platform.isRaspberryPi()) {\n";
        std::cout << "    std::cout << \"Running on \" << platform.getPlatformName() << \"\\n\";\n";
        std::cout << "}\n";
        std::cout << "\n";
        
        std::cout << "// Use default I2C bus (auto-detects Pi 4 vs Pi 5)\n";
        std::cout << "int bus = platform.getDefaultI2CBus();\n";
        std::cout << "Wire.begin(bus);  // Works on Pi 4 (/dev/i2c-1) and Pi 5 (/dev/i2c-20)\n";
        std::cout << "\n";
        
        std::cout << "// Choose PWM backend based on capabilities\n";
        std::cout << "if (platform.getCapabilities().hasDMASupport && geteuid() == 0) {\n";
        std::cout << "    // Use DMA PWM (zero CPU, requires root)\n";
        std::cout << "    auto pwm = DMAPWM(18, 1000);\n";
        std::cout << "} else {\n";
        std::cout << "    // Use EventPWM (<5% CPU, no root required)\n";
        std::cout << "    auto pwm = EventPWM(18, 1000);\n";
        std::cout << "}\n";
        std::cout << "\n";
        
        std::cout << "───────────────────────────────────────────────────────────────\n";
        std::cout << "\n";
        
        // Success summary
        std::cout << "✅ Platform detection complete!\n";
        std::cout << "\n";
        
        return 0;
        
    } catch (const pipinpp::PlatformDetectionError& e) {
        std::cerr << "❌ Platform detection error: " << e.what() << "\n";
        return 1;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Unexpected error: " << e.what() << "\n";
        return 1;
    }
}
