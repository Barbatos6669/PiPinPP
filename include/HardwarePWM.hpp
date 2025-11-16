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
 * @file HardwarePWM.hpp
 * @brief Hardware PWM support via /sys/class/pwm interface
 * @author Barbatos6669
 * @date 2025-11-16
 * 
 * Provides hardware PWM control for Raspberry Pi using Linux sysfs interface.
 * Hardware PWM is jitter-free and ideal for servo control, motor speed control,
 * and precise timing applications.
 * 
 * Raspberry Pi PWM Pins:
 * - GPIO12 (PWM0) - /sys/class/pwm/pwmchip0/pwm0
 * - GPIO13 (PWM1) - /sys/class/pwm/pwmchip0/pwm1
 * - GPIO18 (PWM0 alt) - Same channel as GPIO12
 * - GPIO19 (PWM1 alt) - Same channel as GPIO13
 * 
 * Features:
 * - Jitter-free hardware PWM
 * - Frequency range: 1Hz - 25MHz (limited by hardware)
 * - Nanosecond precision
 * - Multiple polarity modes
 * - Thread-safe operations
 * - Auto-export/unexport management
 * 
 * Example Usage:
 * @code
 * #include "HardwarePWM.hpp"
 * using namespace pipinpp;
 * 
 * int main() {
 *     // Control servo on GPIO18 (PWM0)
 *     HardwarePWM servo(0, 0);  // chip=0, channel=0
 *     
 *     // 50Hz (20ms period) for servo control
 *     servo.begin(50);
 *     
 *     // Sweep servo from 0° to 180°
 *     servo.setDutyCycle(5);   // ~1ms pulse (0°)
 *     delay(1000);
 *     servo.setDutyCycle(7.5); // ~1.5ms pulse (90°)
 *     delay(1000);
 *     servo.setDutyCycle(10);  // ~2ms pulse (180°)
 *     
 *     servo.end();
 *     return 0;
 * }
 * @endcode
 * 
 * @version 0.3.13
 */

#ifndef PIPINPP_HARDWARE_PWM_HPP
#define PIPINPP_HARDWARE_PWM_HPP

#include <cstdint>
#include <string>
#include <mutex>

namespace pipinpp {

/**
 * @brief PWM polarity modes
 */
enum class PWMPolarity {
    NORMAL,   ///< Active-high PWM signal
    INVERSED  ///< Active-low PWM signal (inverted)
};

/**
 * @class HardwarePWM
 * @brief Hardware PWM control via Linux sysfs interface
 * 
 * This class provides jitter-free hardware PWM using the Linux kernel's
 * /sys/class/pwm interface. Ideal for servo control, motor speed control,
 * and applications requiring precise timing.
 * 
 * @note Thread-safe: All operations are protected by internal mutex
 * @note Auto-export: PWM channel is automatically exported/unexported
 * @note Pin conflicts: Using hardware PWM on a pin disables other functions
 * 
 * Pin Mapping (Raspberry Pi):
 * - GPIO12 → PWM0 (chip=0, channel=0)
 * - GPIO13 → PWM1 (chip=0, channel=1)
 * - GPIO18 → PWM0 (chip=0, channel=0) - same as GPIO12
 * - GPIO19 → PWM1 (chip=0, channel=1) - same as GPIO13
 */
class HardwarePWM {
public:
    /**
     * @brief Constructor
     * @param chip PWM chip number (usually 0 for Raspberry Pi)
     * @param channel PWM channel number (0 or 1)
     */
    HardwarePWM(int chip = 0, int channel = 0);
    
    /**
     * @brief Destructor - automatically disables and unexports PWM
     */
    ~HardwarePWM();
    
    /**
     * @brief Initialize hardware PWM with frequency
     * @param frequencyHz PWM frequency in Hz (1-25000000)
     * @param initialDutyCycle Initial duty cycle percentage (0.0-100.0)
     * @return true on success, false on error
     * 
     * @note Exports the PWM channel if not already exported
     * @note Enables the PWM output
     * 
     * Example:
     * @code
     * HardwarePWM servo(0, 0);
     * servo.begin(50);  // 50Hz for servo control
     * @endcode
     */
    bool begin(uint32_t frequencyHz, double initialDutyCycle = 0.0);
    
    /**
     * @brief Disable and unexport PWM channel
     * 
     * @note Safe to call multiple times
     */
    void end();
    
    /**
     * @brief Check if PWM is currently enabled
     * @return true if PWM is enabled, false otherwise
     */
    bool isEnabled() const;
    
    /**
     * @brief Set PWM frequency in Hz
     * @param frequencyHz Frequency in Hz (1-25000000)
     * @return true on success, false on error
     * 
     * @note Changes take effect immediately
     * @note Duty cycle percentage is preserved
     * 
     * Example:
     * @code
     * pwm.setFrequency(1000);  // 1kHz PWM
     * @endcode
     */
    bool setFrequency(uint32_t frequencyHz);
    
    /**
     * @brief Set duty cycle as percentage (0.0-100.0)
     * @param percent Duty cycle percentage (0.0 = always LOW, 100.0 = always HIGH)
     * @return true on success, false on error
     * 
     * Example:
     * @code
     * pwm.setDutyCycle(50.0);  // 50% duty cycle
     * pwm.setDutyCycle(7.5);   // 7.5% for servo center position
     * @endcode
     */
    bool setDutyCycle(double percent);
    
    /**
     * @brief Set duty cycle as 8-bit value (Arduino-compatible)
     * @param value Duty cycle value (0-255)
     * @return true on success, false on error
     * 
     * @note Converts to percentage: 0 → 0%, 255 → 100%
     * 
     * Example:
     * @code
     * pwm.setDutyCycle8Bit(128);  // 50% duty cycle
     * pwm.setDutyCycle8Bit(255);  // 100% duty cycle
     * @endcode
     */
    bool setDutyCycle8Bit(uint8_t value);
    
    /**
     * @brief Set PWM period in nanoseconds (direct control)
     * @param nanoseconds Period in nanoseconds
     * @return true on success, false on error
     * 
     * @note For advanced users requiring precise period control
     * @note Frequency = 1,000,000,000 / period_ns
     */
    bool setPeriodNs(uint64_t nanoseconds);
    
    /**
     * @brief Set duty cycle in nanoseconds (direct control)
     * @param nanoseconds Duty cycle in nanoseconds
     * @return true on success, false on error
     * 
     * @note Must be less than or equal to period
     * @note For advanced users requiring precise pulse width control
     */
    bool setDutyCycleNs(uint64_t nanoseconds);
    
    /**
     * @brief Set PWM polarity
     * @param polarity Normal or Inversed
     * @return true on success, false on error
     * 
     * @note Normal = active-high, Inversed = active-low
     */
    bool setPolarity(PWMPolarity polarity);
    
    /**
     * @brief Enable PWM output
     * @return true on success, false on error
     * 
     * @note Must call begin() first
     */
    bool enable();
    
    /**
     * @brief Disable PWM output
     * @return true on success, false on error
     * 
     * @note Pin goes to LOW (or HIGH if inverted polarity)
     */
    bool disable();
    
    /**
     * @brief Get current frequency in Hz
     * @return Frequency in Hz, or 0 if not initialized
     */
    uint32_t getFrequency() const;
    
    /**
     * @brief Get current duty cycle percentage
     * @return Duty cycle percentage (0.0-100.0), or -1.0 on error
     */
    double getDutyCycle() const;
    
    /**
     * @brief Get current period in nanoseconds
     * @return Period in nanoseconds, or 0 on error
     */
    uint64_t getPeriodNs() const;
    
    /**
     * @brief Get current duty cycle in nanoseconds
     * @return Duty cycle in nanoseconds, or 0 on error
     */
    uint64_t getDutyCycleNs() const;
    
    /**
     * @brief Helper: Convert GPIO pin number to PWM chip/channel
     * @param gpioPin GPIO pin number (12, 13, 18, or 19)
     * @param[out] chip PWM chip number
     * @param[out] channel PWM channel number
     * @return true if pin supports hardware PWM, false otherwise
     * 
     * Example:
     * @code
     * int chip, channel;
     * if (HardwarePWM::gpioToPWM(18, chip, channel)) {
     *     HardwarePWM pwm(chip, channel);
     *     pwm.begin(1000);
     * }
     * @endcode
     */
    static bool gpioToPWM(int gpioPin, int& chip, int& channel);

private:
    int chip_;                      ///< PWM chip number
    int channel_;                   ///< PWM channel number
    bool exported_;                 ///< Whether PWM is currently exported
    bool enabled_;                  ///< Whether PWM is currently enabled
    uint64_t periodNs_;             ///< Current period in nanoseconds
    uint64_t dutyCycleNs_;          ///< Current duty cycle in nanoseconds
    mutable std::mutex mutex_;      ///< Mutex for thread safety
    
    std::string basePath_;          ///< Base path: /sys/class/pwm/pwmchipN/
    std::string pwmPath_;           ///< PWM path: /sys/class/pwm/pwmchipN/pwmM/
    
    /**
     * @brief Export PWM channel to sysfs
     * @return true on success, false on error
     */
    bool exportPWM();
    
    /**
     * @brief Unexport PWM channel from sysfs
     * @return true on success, false on error
     */
    bool unexportPWM();
    
    /**
     * @brief Write value to sysfs file
     * @param filename Relative filename in PWM path
     * @param value Value to write
     * @return true on success, false on error
     */
    bool writeFile(const std::string& filename, const std::string& value);
    
    /**
     * @brief Read value from sysfs file
     * @param filename Relative filename in PWM path
     * @param[out] value Read value
     * @return true on success, false on error
     */
    bool readFile(const std::string& filename, std::string& value) const;
};

} // namespace pipinpp

#endif // PIPINPP_HARDWARE_PWM_HPP
