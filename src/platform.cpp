/*
 * PiPin++ Platform Detection Implementation
 * 
 * Author: Barbatos6669
 * Date: November 16, 2025
 */

#include "platform.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <sys/utsname.h>
#include <unistd.h>
#include <gpiod.h>
#include <dirent.h>

namespace pipinpp {

// Singleton instance
PlatformInfo& PlatformInfo::instance() 
{
    static PlatformInfo instance;
    return instance;
}

// Constructor
PlatformInfo::PlatformInfo()
    : platform_(Platform::UNKNOWN)
    , capabilities_{}
    , kernelVersion_("")
    , libgpiodVersion_("")
    , detected_(false)
{
    detectPlatform();
    detectCapabilities();
    detected_ = true;
}

// Platform name mapping
std::string PlatformInfo::getPlatformName() const 
{
    switch (platform_) {
        case Platform::RASPBERRY_PI_3:     return "Raspberry Pi 3";
        case Platform::RASPBERRY_PI_4:     return "Raspberry Pi 4";
        case Platform::RASPBERRY_PI_5:     return "Raspberry Pi 5";
        case Platform::RASPBERRY_PI_CM4:   return "Raspberry Pi Compute Module 4";
        case Platform::RASPBERRY_PI_ZERO:  return "Raspberry Pi Zero";
        case Platform::RASPBERRY_PI_ZERO2: return "Raspberry Pi Zero 2";
        case Platform::ORANGE_PI:          return "Orange Pi";
        case Platform::BEAGLEBONE:         return "BeagleBone";
        case Platform::JETSON_NANO:        return "NVIDIA Jetson Nano";
        case Platform::UNKNOWN:
        default:                           return "Unknown Platform";
    }
}

// Check if Raspberry Pi
bool PlatformInfo::isRaspberryPi() const 
{
    return platform_ == Platform::RASPBERRY_PI_3 ||
           platform_ == Platform::RASPBERRY_PI_4 ||
           platform_ == Platform::RASPBERRY_PI_5 ||
           platform_ == Platform::RASPBERRY_PI_CM4 ||
           platform_ == Platform::RASPBERRY_PI_ZERO ||
           platform_ == Platform::RASPBERRY_PI_ZERO2;
}

// Get default GPIO chip
std::string PlatformInfo::getDefaultGPIOChip() const 
{
    if (capabilities_.gpioChips.empty()) {
        return "gpiochip0";  // Fallback default
    }
    
    // Return first available chip
    for (const auto& chip : capabilities_.gpioChips) {
        if (chip.available) {
            return chip.name;
        }
    }
    
    return capabilities_.gpioChips[0].name;
}

// Get default I2C bus
int PlatformInfo::getDefaultI2CBus() const 
{
    if (capabilities_.i2cBuses.empty()) {
        return 1;  // Fallback default
    }
    
    // Raspberry Pi 5 uses /dev/i2c-20 by default
    if (platform_ == Platform::RASPBERRY_PI_5) {
        for (const auto& bus : capabilities_.i2cBuses) {
            if (bus.busNumber == 20 && bus.available) {
                return 20;
            }
        }
    }
    
    // Return first available bus
    for (const auto& bus : capabilities_.i2cBuses) {
        if (bus.available) {
            return bus.busNumber;
        }
    }
    
    return capabilities_.i2cBuses[0].busNumber;
}

// Print platform information
void PlatformInfo::printInfo() const 
{
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║               PiPin++ Platform Information                     ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "Platform:      " << getPlatformName() << "\n";
    std::cout << "Kernel:        " << kernelVersion_ << "\n";
    std::cout << "libgpiod:      " << libgpiodVersion_ << "\n";
    std::cout << "\n";
    
    // GPIO Chips
    std::cout << "GPIO Chips:    " << capabilities_.gpioChips.size() << "\n";
    for (const auto& chip : capabilities_.gpioChips) {
        std::cout << "  • " << chip.name 
                  << " (" << chip.label << "): "
                  << chip.numLines << " lines "
                  << (chip.available ? "✓" : "✗") << "\n";
    }
    std::cout << "\n";
    
    // I2C Buses
    std::cout << "I2C Buses:     " << capabilities_.i2cBuses.size() << "\n";
    for (const auto& bus : capabilities_.i2cBuses) {
        std::cout << "  • " << bus.devicePath 
                  << " (bus " << bus.busNumber << ") "
                  << (bus.available ? "✓" : "✗") << "\n";
    }
    std::cout << "\n";
    
    // PWM Channels
    std::cout << "PWM Channels:  " << capabilities_.pwmChannels.size() << "\n";
    for (const auto& pwm : capabilities_.pwmChannels) {
        std::cout << "  • chip" << pwm.chip 
                  << "/channel" << pwm.channel;
        if (pwm.gpioPin >= 0) {
            std::cout << " (GPIO " << pwm.gpioPin << ")";
        }
        std::cout << " " << (pwm.available ? "✓" : "✗") << "\n";
    }
    std::cout << "\n";
    
    // DMA Support
    std::cout << "DMA Support:   " << (capabilities_.hasDMASupport ? "Available ✓" : "Not Available ✗") << "\n";
    if (capabilities_.hasDMASupport) {
        std::cout << "  Peripheral Base: 0x" << std::hex << capabilities_.peripheralBase << std::dec << "\n";
    }
    std::cout << "\n";
    
    // Total pins
    std::cout << "Total GPIO:    " << capabilities_.totalGPIOPins << " pins\n";
    std::cout << "\n";
}

// Force refresh
void PlatformInfo::refresh() 
{
    platform_ = Platform::UNKNOWN;
    capabilities_ = PlatformCapabilities{};
    kernelVersion_.clear();
    libgpiodVersion_.clear();
    
    detectPlatform();
    detectCapabilities();
    detected_ = true;
}

// Detect platform
void PlatformInfo::detectPlatform() 
{
    // Try /proc/device-tree/model first (Raspberry Pi, modern kernels)
    platform_ = parseDeviceTree();
    
    // Fallback to /proc/cpuinfo
    if (platform_ == Platform::UNKNOWN) {
        platform_ = parseCPUInfo();
    }
    
    detectKernelVersion();
    detectLibgpiodVersion();
}

// Detect all capabilities
void PlatformInfo::detectCapabilities() 
{
    detectGPIOChips();
    detectI2CBuses();
    detectPWMChannels();
    detectDMASupport();
    
    // Count total GPIO pins
    capabilities_.totalGPIOPins = 0;
    for (const auto& chip : capabilities_.gpioChips) {
        capabilities_.totalGPIOPins += chip.numLines;
    }
}

// Detect GPIO chips
void PlatformInfo::detectGPIOChips() 
{
    capabilities_.gpioChips.clear();
    
    // Enumerate all gpiochip devices
    for (int i = 0; i < 16; i++) {  // Check up to gpiochip15
        std::string chipName = "gpiochip" + std::to_string(i);
        std::string chipPath = "/dev/" + chipName;
        
        if (!deviceExists(chipPath)) {
            continue;
        }
        
        // Open chip to get info (libgpiod v2 API)
        struct gpiod_chip* chip = gpiod_chip_open(chipPath.c_str());
        if (!chip) {
            continue;
        }
        
        // Get chip info
        struct gpiod_chip_info* chip_info = gpiod_chip_get_info(chip);
        if (!chip_info) {
            gpiod_chip_close(chip);
            continue;
        }
        
        GPIOChipInfo info;
        info.name = chipName;
        info.label = gpiod_chip_info_get_label(chip_info);
        info.numLines = gpiod_chip_info_get_num_lines(chip_info);
        info.available = true;
        
        capabilities_.gpioChips.push_back(info);
        
        gpiod_chip_info_free(chip_info);
        gpiod_chip_close(chip);
    }
}

// Detect I2C buses
void PlatformInfo::detectI2CBuses() 
{
    capabilities_.i2cBuses.clear();
    
    // Check common I2C buses (0-20, covers Pi 5's /dev/i2c-20)
    for (int i = 0; i <= 20; i++) {
        std::string devicePath = "/dev/i2c-" + std::to_string(i);
        
        if (deviceExists(devicePath)) {
            I2CBusInfo info;
            info.busNumber = i;
            info.devicePath = devicePath;
            info.available = (access(devicePath.c_str(), R_OK | W_OK) == 0);
            
            capabilities_.i2cBuses.push_back(info);
        }
    }
}

// Detect PWM channels
void PlatformInfo::detectPWMChannels() 
{
    capabilities_.pwmChannels.clear();
    
    // Check sysfs PWM chips
    DIR* dir = opendir("/sys/class/pwm");
    if (!dir) {
        return;  // No PWM support or permission denied
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        
        // Look for pwmchip* entries
        if (name.find("pwmchip") == 0) {
            int chipNum = std::stoi(name.substr(7));  // Extract number
            
            std::string sysfsPath = "/sys/class/pwm/" + name;
            std::string npwmPath = sysfsPath + "/npwm";
            
            // Read number of PWM channels
            std::string npwmStr = readFile(npwmPath);
            if (npwmStr.empty()) {
                continue;
            }
            
            int numChannels = std::stoi(npwmStr);
            
            // Create info for each channel
            for (int ch = 0; ch < numChannels; ch++) {
                PWMChannelInfo info;
                info.chip = chipNum;
                info.channel = ch;
                info.gpioPin = -1;  // TODO: Map to GPIO pin number
                info.sysfsPath = sysfsPath;
                info.available = (access(sysfsPath.c_str(), R_OK | W_OK) == 0);
                
                capabilities_.pwmChannels.push_back(info);
            }
        }
    }
    
    closedir(dir);
    
    // Map PWM channels to GPIO pins (Raspberry Pi specific)
    if (isRaspberryPi()) {
        // Pi 4/5: PWM0 = GPIO18, PWM1 = GPIO19 (common mapping)
        for (auto& pwm : capabilities_.pwmChannels) {
            if (pwm.chip == 0 && pwm.channel == 0) {
                pwm.gpioPin = 18;
            } else if (pwm.chip == 0 && pwm.channel == 1) {
                pwm.gpioPin = 19;
            }
        }
    }
}

// Detect kernel version
void PlatformInfo::detectKernelVersion() 
{
    struct utsname buffer;
    if (uname(&buffer) == 0) {
        kernelVersion_ = buffer.release;
    } else {
        kernelVersion_ = "Unknown";
    }
}

// Detect libgpiod version
void PlatformInfo::detectLibgpiodVersion() 
{
    libgpiodVersion_ = gpiod_api_version();
}

// Detect DMA support
void PlatformInfo::detectDMASupport() 
{
    capabilities_.hasDMASupport = false;
    capabilities_.peripheralBase = 0;
    
    // Only Raspberry Pi supports DMA GPIO (for now)
    if (!isRaspberryPi()) {
        return;
    }
    
    // Check if /dev/mem exists and is readable
    if (!fileExists("/dev/mem")) {
        return;
    }
    
    // Check if running as root (required for /dev/mem)
    if (geteuid() != 0) {
        // Not root - DMA might work but can't verify
        capabilities_.peripheralBase = getPeripheralBase();
        capabilities_.hasDMASupport = (capabilities_.peripheralBase != 0);
        return;
    }
    
    // Get peripheral base address
    capabilities_.peripheralBase = getPeripheralBase();
    capabilities_.hasDMASupport = (capabilities_.peripheralBase != 0);
}

// Read file contents
std::string PlatformInfo::readFile(const std::string& path) const 
{
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Check if file exists
bool PlatformInfo::fileExists(const std::string& path) const 
{
    return access(path.c_str(), F_OK) == 0;
}

// Check if device exists
bool PlatformInfo::deviceExists(const std::string& path) const 
{
    return fileExists(path);
}

// Parse /proc/cpuinfo
Platform PlatformInfo::parseCPUInfo() const 
{
    std::string cpuinfo = readFile("/proc/cpuinfo");
    if (cpuinfo.empty()) {
        return Platform::UNKNOWN;
    }
    
    // Look for Raspberry Pi indicators
    if (cpuinfo.find("BCM2835") != std::string::npos) {
        return Platform::RASPBERRY_PI_ZERO;
    }
    if (cpuinfo.find("BCM2836") != std::string::npos) {
        return Platform::RASPBERRY_PI_3;  // Pi 2 v1.2+ shares BCM2836
    }
    if (cpuinfo.find("BCM2837") != std::string::npos) {
        // Could be Pi 3 or Zero 2
        if (cpuinfo.find("Raspberry Pi Zero 2") != std::string::npos) {
            return Platform::RASPBERRY_PI_ZERO2;
        }
        return Platform::RASPBERRY_PI_3;
    }
    if (cpuinfo.find("BCM2711") != std::string::npos) {
        // Could be Pi 4 or CM4
        if (cpuinfo.find("Compute Module 4") != std::string::npos) {
            return Platform::RASPBERRY_PI_CM4;
        }
        return Platform::RASPBERRY_PI_4;
    }
    if (cpuinfo.find("BCM2712") != std::string::npos) {
        return Platform::RASPBERRY_PI_5;
    }
    
    // Orange Pi indicators
    if (cpuinfo.find("Allwinner") != std::string::npos ||
        cpuinfo.find("sun") != std::string::npos) {  // Allwinner SoCs often have "sun" prefix
        return Platform::ORANGE_PI;
    }
    
    // BeagleBone indicators
    if (cpuinfo.find("AM33XX") != std::string::npos ||
        cpuinfo.find("AM335") != std::string::npos) {
        return Platform::BEAGLEBONE;
    }
    
    // Jetson Nano indicators
    if (cpuinfo.find("Tegra") != std::string::npos) {
        return Platform::JETSON_NANO;
    }
    
    return Platform::UNKNOWN;
}

// Parse /proc/device-tree/model
Platform PlatformInfo::parseDeviceTree() const 
{
    std::string model = readFile("/proc/device-tree/model");
    if (model.empty()) {
        return Platform::UNKNOWN;
    }
    
    // Remove null terminators and whitespace
    model.erase(std::remove(model.begin(), model.end(), '\0'), model.end());
    model.erase(std::remove(model.begin(), model.end(), '\n'), model.end());
    
    // Raspberry Pi detection
    if (model.find("Raspberry Pi 5") != std::string::npos) {
        return Platform::RASPBERRY_PI_5;
    }
    if (model.find("Raspberry Pi 4") != std::string::npos) {
        return Platform::RASPBERRY_PI_4;
    }
    if (model.find("Raspberry Pi Compute Module 4") != std::string::npos) {
        return Platform::RASPBERRY_PI_CM4;
    }
    if (model.find("Raspberry Pi 3") != std::string::npos) {
        return Platform::RASPBERRY_PI_3;
    }
    if (model.find("Raspberry Pi Zero 2") != std::string::npos) {
        return Platform::RASPBERRY_PI_ZERO2;
    }
    if (model.find("Raspberry Pi Zero") != std::string::npos) {
        return Platform::RASPBERRY_PI_ZERO;
    }
    
    // Orange Pi detection
    if (model.find("Orange Pi") != std::string::npos) {
        return Platform::ORANGE_PI;
    }
    
    // BeagleBone detection
    if (model.find("BeagleBone") != std::string::npos ||
        model.find("TI AM335") != std::string::npos) {
        return Platform::BEAGLEBONE;
    }
    
    // Jetson Nano detection
    if (model.find("Jetson") != std::string::npos) {
        return Platform::JETSON_NANO;
    }
    
    return Platform::UNKNOWN;
}

// Get peripheral base address
uint32_t PlatformInfo::getPeripheralBase() const 
{
    switch (platform_) {
        case Platform::RASPBERRY_PI_ZERO:
        case Platform::RASPBERRY_PI_3:
        case Platform::RASPBERRY_PI_ZERO2:
            return 0x3F000000;  // BCM2835/2837
            
        case Platform::RASPBERRY_PI_4:
        case Platform::RASPBERRY_PI_CM4:
            return 0xFE000000;  // BCM2711
            
        case Platform::RASPBERRY_PI_5:
            // Pi 5 uses 64-bit addressing with RP1 I/O controller
            // This is a simplified view - actual Pi 5 requires different approach
            return 0x1F00000000ULL & 0xFFFFFFFF;  // Truncated for 32-bit compatibility
            
        default:
            return 0;  // Unknown platform
    }
}

} // namespace pipinpp
