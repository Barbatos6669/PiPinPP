/*
 * PiPin++ Platform Detection Module
 * 
 * Provides runtime detection of hardware platform, including:
 * - Raspberry Pi model detection (Pi 3, 4, 5, CM4, Zero, etc.)
 * - SBC detection (Orange Pi, BeagleBone, Jetson Nano)
 * - Kernel version and libgpiod compatibility
 * - Hardware capabilities (GPIO chips, I2C buses, PWM channels)
 * 
 * This module enables PiPinPP to automatically adapt to different
 * platforms without requiring compile-time configuration.
 * 
 * Author: Barbatos6669
 * Date: November 16, 2025
 * Status: Phase 2 (Platform Expansion)
 */

#ifndef PIPINPP_PLATFORM_HPP
#define PIPINPP_PLATFORM_HPP

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <stdexcept>

namespace pipinpp {

/**
 * @brief Enumeration of supported hardware platforms
 */
enum class Platform {
    UNKNOWN,
    RASPBERRY_PI_3,     // BCM2837, 1.2 GHz quad-core ARM Cortex-A53
    RASPBERRY_PI_4,     // BCM2711, 1.5 GHz quad-core ARM Cortex-A72
    RASPBERRY_PI_5,     // BCM2712, 2.4 GHz quad-core ARM Cortex-A76
    RASPBERRY_PI_CM4,   // Compute Module 4 (same as Pi 4)
    RASPBERRY_PI_ZERO,  // BCM2835, 1 GHz single-core ARM1176JZF-S
    RASPBERRY_PI_ZERO2, // BCM2837, 1 GHz quad-core ARM Cortex-A53
    ORANGE_PI,          // Allwinner H3/H5/H6
    BEAGLEBONE,         // TI AM335x
    JETSON_NANO         // NVIDIA Tegra X1
};

/**
 * @brief GPIO chip information
 */
struct GPIOChipInfo {
    std::string name;           // e.g., "gpiochip0", "gpiochip1"
    std::string label;          // e.g., "pinctrl-bcm2835", "gpio-tegra"
    int numLines;               // Total GPIO lines available
    bool available;             // Whether chip is accessible
};

/**
 * @brief I2C bus information
 */
struct I2CBusInfo {
    int busNumber;              // e.g., 1, 20
    std::string devicePath;     // e.g., "/dev/i2c-1"
    bool available;             // Whether bus is accessible
};

/**
 * @brief PWM channel information
 */
struct PWMChannelInfo {
    int chip;                   // PWM chip number
    int channel;                // PWM channel number
    int gpioPin;                // Associated GPIO pin (-1 if not mapped)
    std::string sysfsPath;      // e.g., "/sys/class/pwm/pwmchip0"
    bool available;             // Whether channel is accessible
};

/**
 * @brief Hardware capabilities of detected platform
 */
struct PlatformCapabilities {
    std::vector<GPIOChipInfo> gpioChips;
    std::vector<I2CBusInfo> i2cBuses;
    std::vector<PWMChannelInfo> pwmChannels;
    int totalGPIOPins;          // Total GPIO pins available
    bool hasDMASupport;         // Whether DMA GPIO is feasible
    uint32_t peripheralBase;    // Base address for DMA/hardware access
};

/**
 * @brief Main platform detection class
 * 
 * Singleton pattern - one instance per process.
 * Auto-detects platform on first access.
 * 
 * @example
 * ```cpp
 * auto& platform = PlatformInfo::instance();
 * 
 * if (platform.getPlatform() == Platform::RASPBERRY_PI_4) {
 *     std::cout << "Running on Raspberry Pi 4\n";
 * }
 * 
 * // Check I2C availability
 * auto i2cBuses = platform.getCapabilities().i2cBuses;
 * for (const auto& bus : i2cBuses) {
 *     std::cout << "I2C bus " << bus.busNumber << ": " 
 *               << (bus.available ? "available" : "not available") << "\n";
 * }
 * ```
 */
class PlatformInfo {
public:
    /**
     * @brief Get singleton instance
     * @return Reference to PlatformInfo singleton
     */
    static PlatformInfo& instance();
    
    /**
     * @brief Get detected platform
     * @return Platform enum value
     */
    Platform getPlatform() const { return platform_; }
    
    /**
     * @brief Get platform name as string
     * @return Human-readable platform name
     */
    std::string getPlatformName() const;
    
    /**
     * @brief Get platform capabilities
     * @return Struct with detected hardware capabilities
     */
    const PlatformCapabilities& getCapabilities() const { return capabilities_; }
    
    /**
     * @brief Check if platform is supported
     * @return true if platform is recognized and supported
     */
    bool isSupported() const { return platform_ != Platform::UNKNOWN; }
    
    /**
     * @brief Check if running on Raspberry Pi (any model)
     * @return true if any Raspberry Pi variant
     */
    bool isRaspberryPi() const;
    
    /**
     * @brief Get kernel version
     * @return Kernel version string (e.g., "6.1.21")
     */
    std::string getKernelVersion() const { return kernelVersion_; }
    
    /**
     * @brief Get libgpiod version
     * @return libgpiod version string (e.g., "2.2.1")
     */
    std::string getLibgpiodVersion() const { return libgpiodVersion_; }
    
    /**
     * @brief Get default GPIO chip name
     * @return Chip name (e.g., "gpiochip0")
     * @note Returns first available GPIO chip
     */
    std::string getDefaultGPIOChip() const;
    
    /**
     * @brief Get default I2C bus number
     * @return Bus number (e.g., 1 for Pi 4, 20 for Pi 5)
     * @note Returns first available I2C bus
     */
    int getDefaultI2CBus() const;
    
    /**
     * @brief Print platform information to stdout
     * 
     * Useful for debugging and diagnostics.
     * 
     * @example
     * ```
     * Platform: Raspberry Pi 4
     * Kernel: 6.1.21-v8+
     * libgpiod: 2.2.1
     * GPIO Chips: 1 (gpiochip0: 58 lines)
     * I2C Buses: 2 (/dev/i2c-1, /dev/i2c-20)
     * PWM Channels: 2 (GPIO 18, 19)
     * DMA Support: Available
     * ```
     */
    void printInfo() const;
    
    /**
     * @brief Force re-detection of platform
     * 
     * Normally not needed - detection happens once on first access.
     * Use this if hardware changes at runtime (hot-plug, etc.)
     */
    void refresh();
    
    // Delete copy/move constructors (singleton)
    PlatformInfo(const PlatformInfo&) = delete;
    PlatformInfo& operator=(const PlatformInfo&) = delete;
    PlatformInfo(PlatformInfo&&) = delete;
    PlatformInfo& operator=(PlatformInfo&&) = delete;
    
private:
    PlatformInfo();  // Private constructor (singleton)
    ~PlatformInfo() = default;
    
    // Detection methods
    void detectPlatform();
    void detectCapabilities();
    void detectGPIOChips();
    void detectI2CBuses();
    void detectPWMChannels();
    void detectKernelVersion();
    void detectLibgpiodVersion();
    void detectDMASupport();
    
    // Helper methods
    std::string readFile(const std::string& path) const;
    bool fileExists(const std::string& path) const;
    bool deviceExists(const std::string& path) const;
    Platform parseCPUInfo() const;
    Platform parseDeviceTree() const;
    uint32_t getPeripheralBase() const;
    
    // Member variables
    Platform platform_;
    PlatformCapabilities capabilities_;
    std::string kernelVersion_;
    std::string libgpiodVersion_;
    bool detected_;  // Whether detection has run
};

/**
 * @brief Exception thrown when platform detection fails
 */
class PlatformDetectionError : public std::runtime_error {
public:
    explicit PlatformDetectionError(const std::string& message)
        : std::runtime_error(message) {}
};

} // namespace pipinpp

#endif // PIPINPP_PLATFORM_HPP
