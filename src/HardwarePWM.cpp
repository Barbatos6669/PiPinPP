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
 * @file HardwarePWM.cpp
 * @brief Implementation of hardware PWM via /sys/class/pwm
 * @author Barbatos6669
 * @date 2025-11-16
 */

#include "HardwarePWM.hpp"
#include "log.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <unistd.h>
#include <cerrno>
#include <cstring>

namespace pipinpp {

HardwarePWM::HardwarePWM(int chip, int channel)
    : chip_(chip)
    , channel_(channel)
    , exported_(false)
    , enabled_(false)
    , periodNs_(0)
    , dutyCycleNs_(0)
{
    basePath_ = "/sys/class/pwm/pwmchip" + std::to_string(chip_) + "/";
    pwmPath_ = basePath_ + "pwm" + std::to_string(channel_) + "/";
    
    PIPINPP_LOG_DEBUG("HardwarePWM created: chip=" << chip_ << ", channel=" << channel_);
}

HardwarePWM::~HardwarePWM()
{
    end();
}

bool HardwarePWM::begin(uint32_t frequencyHz, double initialDutyCycle)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (frequencyHz == 0) {
        PIPINPP_LOG_ERROR("Invalid frequency: " << frequencyHz << " Hz");
        return false;
    }
    
    // Export PWM channel
    if (!exported_ && !exportPWM()) {
        return false;
    }
    
    // Calculate period in nanoseconds
    uint64_t periodNs = 1000000000ULL / frequencyHz;
    
    // Set period first (must be set before duty cycle)
    if (!writeFile("period", std::to_string(periodNs))) {
        PIPINPP_LOG_ERROR("Failed to set PWM period");
        return false;
    }
    periodNs_ = periodNs;
    
    // Set initial duty cycle
    uint64_t dutyCycleNs = static_cast<uint64_t>((initialDutyCycle / 100.0) * periodNs);
    if (!writeFile("duty_cycle", std::to_string(dutyCycleNs))) {
        PIPINPP_LOG_ERROR("Failed to set PWM duty cycle");
        return false;
    }
    dutyCycleNs_ = dutyCycleNs;
    
    // Enable PWM
    if (!writeFile("enable", "1")) {
        PIPINPP_LOG_ERROR("Failed to enable PWM");
        return false;
    }
    enabled_ = true;
    
    PIPINPP_LOG_INFO("HardwarePWM started: " << frequencyHz << "Hz, " << initialDutyCycle << "% duty");
    return true;
}

void HardwarePWM::end()
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (enabled_) {
        writeFile("enable", "0");
        enabled_ = false;
    }
    
    if (exported_) {
        unexportPWM();
    }
    
    PIPINPP_LOG_DEBUG("HardwarePWM stopped");
}

bool HardwarePWM::isEnabled() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return enabled_;
}

bool HardwarePWM::setFrequency(uint32_t frequencyHz)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!exported_) {
        PIPINPP_LOG_ERROR("PWM not initialized - call begin() first");
        return false;
    }
    
    if (frequencyHz == 0) {
        PIPINPP_LOG_ERROR("Invalid frequency: " << frequencyHz << " Hz");
        return false;
    }
    
    // Calculate new period
    uint64_t newPeriodNs = 1000000000ULL / frequencyHz;
    
    // Preserve duty cycle percentage
    double dutyCyclePercent = (periodNs_ > 0) ? (100.0 * dutyCycleNs_ / periodNs_) : 0.0;
    uint64_t newDutyCycleNs = static_cast<uint64_t>((dutyCyclePercent / 100.0) * newPeriodNs);
    
    // Disable while changing period (required by some PWM hardware)
    bool wasEnabled = enabled_;
    if (wasEnabled) {
        writeFile("enable", "0");
    }
    
    // Set new period
    if (!writeFile("period", std::to_string(newPeriodNs))) {
        PIPINPP_LOG_ERROR("Failed to set PWM period");
        if (wasEnabled) {
            writeFile("enable", "1");
        }
        return false;
    }
    periodNs_ = newPeriodNs;
    
    // Update duty cycle to maintain percentage
    if (!writeFile("duty_cycle", std::to_string(newDutyCycleNs))) {
        PIPINPP_LOG_ERROR("Failed to set PWM duty cycle");
        if (wasEnabled) {
            writeFile("enable", "1");
        }
        return false;
    }
    dutyCycleNs_ = newDutyCycleNs;
    
    // Re-enable if it was enabled
    if (wasEnabled) {
        writeFile("enable", "1");
    }
    
    PIPINPP_LOG_DEBUG("PWM frequency set to " << frequencyHz << " Hz");
    return true;
}

bool HardwarePWM::setDutyCycle(double percent)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!exported_) {
        PIPINPP_LOG_ERROR("PWM not initialized - call begin() first");
        return false;
    }
    
    // Clamp to valid range
    if (percent < 0.0) percent = 0.0;
    if (percent > 100.0) percent = 100.0;
    
    uint64_t dutyCycleNs = static_cast<uint64_t>((percent / 100.0) * periodNs_);
    
    if (!writeFile("duty_cycle", std::to_string(dutyCycleNs))) {
        PIPINPP_LOG_ERROR("Failed to set duty cycle");
        return false;
    }
    dutyCycleNs_ = dutyCycleNs;
    
    PIPINPP_LOG_DEBUG("PWM duty cycle set to " << percent << "%");
    return true;
}

bool HardwarePWM::setDutyCycle8Bit(uint8_t value)
{
    double percent = (value / 255.0) * 100.0;
    return setDutyCycle(percent);
}

bool HardwarePWM::setPeriodNs(uint64_t nanoseconds)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!exported_) {
        PIPINPP_LOG_ERROR("PWM not initialized - call begin() first");
        return false;
    }
    
    // Disable while changing period
    bool wasEnabled = enabled_;
    if (wasEnabled) {
        writeFile("enable", "0");
    }
    
    if (!writeFile("period", std::to_string(nanoseconds))) {
        PIPINPP_LOG_ERROR("Failed to set period");
        if (wasEnabled) {
            writeFile("enable", "1");
        }
        return false;
    }
    periodNs_ = nanoseconds;
    
    // Ensure duty cycle doesn't exceed new period
    if (dutyCycleNs_ > periodNs_) {
        dutyCycleNs_ = periodNs_;
        writeFile("duty_cycle", std::to_string(dutyCycleNs_));
    }
    
    if (wasEnabled) {
        writeFile("enable", "1");
    }
    
    return true;
}

bool HardwarePWM::setDutyCycleNs(uint64_t nanoseconds)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!exported_) {
        PIPINPP_LOG_ERROR("PWM not initialized - call begin() first");
        return false;
    }
    
    if (nanoseconds > periodNs_) {
        PIPINPP_LOG_WARNING("Duty cycle (" << nanoseconds << "ns) exceeds period (" << periodNs_ << "ns), clamping");
        nanoseconds = periodNs_;
    }
    
    if (!writeFile("duty_cycle", std::to_string(nanoseconds))) {
        PIPINPP_LOG_ERROR("Failed to set duty cycle");
        return false;
    }
    dutyCycleNs_ = nanoseconds;
    
    return true;
}

bool HardwarePWM::setPolarity(PWMPolarity polarity)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!exported_) {
        PIPINPP_LOG_ERROR("PWM not initialized - call begin() first");
        return false;
    }
    
    // Must disable before changing polarity
    bool wasEnabled = enabled_;
    if (wasEnabled) {
        writeFile("enable", "0");
    }
    
    const char* polarityStr = (polarity == PWMPolarity::NORMAL) ? "normal" : "inversed";
    bool result = writeFile("polarity", polarityStr);
    
    if (wasEnabled) {
        writeFile("enable", "1");
    }
    
    if (result) {
        PIPINPP_LOG_DEBUG("PWM polarity set to " << polarityStr);
    }
    
    return result;
}

bool HardwarePWM::enable()
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!exported_) {
        PIPINPP_LOG_ERROR("PWM not initialized - call begin() first");
        return false;
    }
    
    if (!writeFile("enable", "1")) {
        PIPINPP_LOG_ERROR("Failed to enable PWM");
        return false;
    }
    enabled_ = true;
    
    return true;
}

bool HardwarePWM::disable()
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!exported_) {
        return true; // Already disabled
    }
    
    if (!writeFile("enable", "0")) {
        PIPINPP_LOG_ERROR("Failed to disable PWM");
        return false;
    }
    enabled_ = false;
    
    return true;
}

uint32_t HardwarePWM::getFrequency() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (periodNs_ == 0) {
        return 0;
    }
    
    return static_cast<uint32_t>(1000000000ULL / periodNs_);
}

double HardwarePWM::getDutyCycle() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (periodNs_ == 0) {
        return -1.0;
    }
    
    return (100.0 * dutyCycleNs_) / periodNs_;
}

uint64_t HardwarePWM::getPeriodNs() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return periodNs_;
}

uint64_t HardwarePWM::getDutyCycleNs() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return dutyCycleNs_;
}

bool HardwarePWM::gpioToPWM(int gpioPin, int& chip, int& channel)
{
    switch (gpioPin) {
        case 12:
        case 18:
            chip = 0;
            channel = 0;
            return true;
        case 13:
        case 19:
            chip = 0;
            channel = 1;
            return true;
        default:
            return false;
    }
}

// Private methods

bool HardwarePWM::exportPWM()
{
    // Check if already exported
    std::ifstream testFile(pwmPath_ + "period");
    if (testFile.good()) {
        PIPINPP_LOG_DEBUG("PWM already exported");
        exported_ = true;
        return true;
    }
    
    // Export the PWM channel
    std::string exportPath = basePath_ + "export";
    std::ofstream exportFile(exportPath);
    if (!exportFile) {
        PIPINPP_LOG_ERROR("Failed to open " << exportPath << " for export");
        return false;
    }
    
    exportFile << channel_;
    exportFile.close();
    
    // Wait for sysfs to create the directory
    for (int i = 0; i < 50; i++) {
        std::ifstream check(pwmPath_ + "period");
        if (check.good()) {
            exported_ = true;
            PIPINPP_LOG_DEBUG("PWM exported successfully");
            return true;
        }
        usleep(10000); // 10ms
    }
    
    PIPINPP_LOG_ERROR("PWM export timeout - " << pwmPath_ << " not created");
    return false;
}

bool HardwarePWM::unexportPWM()
{
    if (!exported_) {
        return true;
    }
    
    std::string unexportPath = basePath_ + "unexport";
    std::ofstream unexportFile(unexportPath);
    if (!unexportFile) {
        PIPINPP_LOG_WARNING("Failed to open " << unexportPath << " for unexport");
        return false;
    }
    
    unexportFile << channel_;
    unexportFile.close();
    
    exported_ = false;
    PIPINPP_LOG_DEBUG("PWM unexported");
    return true;
}

bool HardwarePWM::writeFile(const std::string& filename, const std::string& value)
{
    std::string fullPath = pwmPath_ + filename;
    std::ofstream file(fullPath);
    
    if (!file) {
        PIPINPP_LOG_ERROR("Failed to open " << fullPath << " for writing (errno=" << errno << ": " << std::strerror(errno) << ")");
        return false;
    }
    
    file << value;
    file.close();
    
    if (file.fail()) {
        PIPINPP_LOG_ERROR("Failed to write to " << fullPath);
        return false;
    }
    
    PIPINPP_LOG_DEBUG("Wrote '" << value << "' to " << filename);
    return true;
}

bool HardwarePWM::readFile(const std::string& filename, std::string& value) const
{
    std::string fullPath = pwmPath_ + filename;
    std::ifstream file(fullPath);
    
    if (!file) {
        PIPINPP_LOG_ERROR("Failed to open " << fullPath << " for reading");
        return false;
    }
    
    std::getline(file, value);
    file.close();
    
    return true;
}

} // namespace pipinpp
