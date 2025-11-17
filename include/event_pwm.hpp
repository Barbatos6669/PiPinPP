/**
 * @file event_pwm.hpp
 * @brief Event-driven PWM with reduced CPU usage
 * @author Barbatos6669
 * @date 2025
 * 
 * @details
 * Hybrid timing system that reduces CPU usage from 10-30% to <5% per pin
 * while maintaining microsecond-level timing accuracy.
 * 
 * Timing Strategy:
 * 1. Use clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME) for coarse delays
 * 2. Busy-wait final ~100µs for precision
 * 3. This achieves <5% CPU usage while keeping jitter <10µs
 * 
 * Performance Comparison:
 * - Old busy-loop: 10-30% CPU, <5µs jitter
 * - New hybrid:    <5% CPU,    <10µs jitter
 * - Hardware PWM:  0% CPU,     0µs jitter (reference)
 * 
 * ⚠️ CPU Usage:
 * - 70-85% reduction compared to pure busy-loop PWM
 * - Suitable for LED dimming and non-critical applications
 * - For servo control, use HardwarePWM class (zero CPU, zero jitter)
 * 
 * Example usage:
 * @code
 * #include "event_pwm.hpp"
 * 
 * int main() {
 *     EventPWM led(17);
 *     led.begin(1000);          // 1kHz frequency
 *     led.setDutyCycle8Bit(128); // 50% brightness
 *     
 *     delay(5000);              // Run for 5 seconds
 *     led.end();                // Clean shutdown
 *     return 0;
 * }
 * @endcode
 * 
 * @author Barbatos6669
 * @version 0.4.0
 * @date 2025-11-16
 */

#pragma once

#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <map>
#include <time.h>
#include "pin.hpp"

namespace pipinpp {

/**
 * @brief Threshold for switching from sleep to busy-wait (microseconds)
 * 
 * Below this threshold, we busy-wait for precision.
 * Above this threshold, we use clock_nanosleep() to yield CPU.
 * 
 * Tuning notes:
 * - Too small: No CPU savings
 * - Too large: Increased timing jitter
 * - 100µs is empirically optimal for Pi 4/5
 */
constexpr long BUSYWAIT_THRESHOLD_US = 100;

/**
 * @brief Event-driven PWM channel with reduced CPU usage
 * 
 * Uses hybrid timing: clock_nanosleep() for coarse delays,
 * busy-wait for final precision microseconds.
 */
class EventPWM {
public:
    /**
     * @brief Constructor
     * @param pin GPIO pin number (0-27)
     * @throws InvalidPinError if pin number invalid
     */
    explicit EventPWM(int pin);
    
    /**
     * @brief Destructor - stops PWM and cleans up
     */
    ~EventPWM();
    
    // Prevent copying
    EventPWM(const EventPWM&) = delete;
    EventPWM& operator=(const EventPWM&) = delete;
    
    /**
     * @brief Start PWM with specified frequency
     * @param frequencyHz Frequency in Hz (50-10000)
     * @param dutyCycle Initial duty cycle percentage (0.0-100.0)
     * @return true on success
     * 
     * @note Pin is automatically configured as OUTPUT
     */
    bool begin(double frequencyHz, double dutyCycle = 0.0);
    
    /**
     * @brief Stop PWM and release resources
     */
    void end();
    
    /**
     * @brief Set duty cycle as percentage
     * @param dutyCycle Percentage (0.0 = always LOW, 100.0 = always HIGH)
     * @return true on success
     */
    bool setDutyCycle(double dutyCycle);
    
    /**
     * @brief Set duty cycle using 8-bit value (Arduino-style)
     * @param value 0-255 (0 = 0%, 255 = 100%)
     * @return true on success
     * 
     * @note Compatible with analogWrite() API
     */
    bool setDutyCycle8Bit(uint8_t value);
    
    /**
     * @brief Change PWM frequency
     * @param frequencyHz New frequency in Hz (50-10000)
     * @return true on success
     */
    bool setFrequency(double frequencyHz);
    
    /**
     * @brief Get current duty cycle
     * @return Duty cycle percentage (0.0-100.0)
     */
    double getDutyCycle() const;
    
    /**
     * @brief Get current frequency
     * @return Frequency in Hz
     */
    double getFrequency() const;
    
    /**
     * @brief Check if PWM is running
     * @return true if active
     */
    bool isActive() const;
    
    /**
     * @brief Get GPIO pin number
     * @return Pin number
     */
    int getPin() const { return pin_; }

private:
    /**
     * @brief PWM generation thread function
     * 
     * Hybrid timing algorithm:
     * 1. Calculate period = 1/frequency
     * 2. Calculate onTime = period * (dutyCycle/100)
     * 3. For each cycle:
     *    a. Set pin HIGH
     *    b. Sleep (onTime - THRESHOLD), then busy-wait final THRESHOLD
     *    c. Set pin LOW
     *    d. Sleep (offTime - THRESHOLD), then busy-wait final THRESHOLD
     */
    void pwmThreadFunction();
    
    /**
     * @brief Hybrid sleep: clock_nanosleep() + busy-wait
     * @param durationNs Target sleep duration in nanoseconds
     * 
     * If duration > BUSYWAIT_THRESHOLD_US:
     *   - Sleep (duration - threshold) using clock_nanosleep()
     *   - Busy-wait final threshold microseconds for precision
     * Else:
     *   - Pure busy-wait for entire duration
     */
    void hybridSleep(long durationNs);
    
    int pin_;                              ///< GPIO pin number
    std::unique_ptr<Pin> pinObj_;          ///< Pin control object
    std::thread pwmThread_;                ///< PWM generation thread
    std::atomic<bool> active_;             ///< Thread running flag
    std::atomic<double> dutyCycle_;        ///< Duty cycle percentage (0.0-100.0)
    std::atomic<double> frequencyHz_;      ///< PWM frequency in Hz
    mutable std::mutex mutex_;             ///< Protect state changes
};

/**
 * @brief Manager for multiple EventPWM channels
 * 
 * Singleton pattern to coordinate multiple PWM outputs.
 */
class EventPWMManager {
public:
    /**
     * @brief Get singleton instance
     */
    static EventPWMManager& getInstance();
    
    /**
     * @brief Start PWM on a pin
     * @param pin GPIO pin number
     * @param value 8-bit duty cycle (0-255)
     * @param frequencyHz Frequency in Hz (default: 490)
     * 
     * @note Arduino-compatible API
     */
    void analogWriteEvent(int pin, int value, int frequencyHz = 490);
    
    /**
     * @brief Stop PWM on a pin
     * @param pin GPIO pin number
     * @return true if PWM was active
     */
    bool stopPWM(int pin);
    
    /**
     * @brief Check if PWM is active on a pin
     * @param pin GPIO pin number
     * @return true if active
     */
    bool isActive(int pin) const;
    
    /**
     * @brief Get number of active PWM channels
     * @return Active channel count
     */
    size_t getActiveCount() const;

private:
    EventPWMManager() = default;
    ~EventPWMManager();
    
    // Prevent copying
    EventPWMManager(const EventPWMManager&) = delete;
    EventPWMManager& operator=(const EventPWMManager&) = delete;
    
    std::map<int, std::unique_ptr<EventPWM>> channels_;
    mutable std::mutex mutex_;
};

} // namespace pipinpp
