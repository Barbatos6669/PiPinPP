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
 * @file pwm.hpp
 * @brief Software PWM (Pulse Width Modulation) support for Raspberry Pi
 * 
 * @details
 * Provides Arduino-compatible PWM functionality using software-based pulse generation.
 * Allows controlling LED brightness, motor speed, and other analog-like outputs.
 * 
 * Features:
 * - Arduino-compatible analogWrite() function
 * - Configurable PWM frequency (default 490Hz like Arduino)
 * - 8-bit duty cycle resolution (0-255)
 * - Multiple independent PWM outputs
 * - Per-pin dedicated threads for accurate timing
 * - Smooth transitions and glitch-free operation
 *
 * Limitations:
 * - Software PWM has timing jitter (not suitable for precise applications)
 * - CPU usage increases with number of active PWM pins
 * - Frequency and accuracy affected by system load
 * - Not suitable for servo control (use hardware PWM for servos)
 *
 * Example usage:
 * @code
 * #include "pwm.hpp"
 * 
 * int main() {
 *     // Fade LED from off to full brightness
 *     pinMode(17, OUTPUT);
 *     for (int brightness = 0; brightness <= 255; brightness++) {
 *         analogWrite(17, brightness);
 *         delay(10);
 *     }
 *     return 0;
 * }
 * @endcode
 * 
 * @author HobbyHacker / Barbatos6669
 * @version 0.3.8
 * @date 2025-11-03
 */

#pragma once

#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <map>
#include <chrono>
#include "pin.hpp"

/**
 * @brief Default PWM frequency in Hz (matches Arduino UNO default)
 */
constexpr int DEFAULT_PWM_FREQUENCY = 490;

/**
 * @brief Minimum PWM frequency in Hz
 */
constexpr int MIN_PWM_FREQUENCY = 1;

/**
 * @brief Maximum PWM frequency in Hz (limited by software precision)
 */
constexpr int MAX_PWM_FREQUENCY = 5000;

/**
 * @brief Internal structure for managing a single PWM output
 */
struct PWMChannel {
    int pin;                                ///< GPIO pin number
    std::unique_ptr<Pin> pinObj;           ///< Pin object for GPIO control
    std::atomic<int> dutyCycle;            ///< Current duty cycle (0-255)
    std::atomic<int> frequency;            ///< PWM frequency in Hz
    std::atomic<bool> active;              ///< Whether PWM is active
    std::thread pwmThread;                 ///< Dedicated PWM generation thread
    
    PWMChannel(int p) 
        : pin(p), dutyCycle(0), frequency(DEFAULT_PWM_FREQUENCY), active(false) {}
    
    ~PWMChannel();
};

/**
 * @brief Manages software PWM generation on multiple GPIO pins
 * 
 * This class is thread-safe and handles multiple PWM outputs simultaneously.
 * Each PWM pin has its own dedicated thread for accurate pulse generation.
 * 
 * @note This is a singleton class - only one instance exists.
 * 
 * @warning Software PWM has inherent timing jitter and is not suitable for
 *          applications requiring precise timing (e.g., servo control).
 *          For servos, use hardware PWM instead.
 */
class PWMManager {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the global PWMManager instance
     */
    static PWMManager& getInstance();
    
    /**
     * @brief Start PWM output on a GPIO pin
     * 
     * @param pin GPIO pin number (0-27 for Raspberry Pi)
     * @param dutyCycle Duty cycle value (0-255, where 0=always LOW, 255=always HIGH)
     * @param frequency PWM frequency in Hz (default 490Hz)
     * @throws InvalidPinError if pin number is invalid
     * @throws GpioAccessError if unable to configure pin
     * 
     * @note If PWM is already active on this pin, duty cycle is updated
     * @note Frequency can only be changed by stopping and restarting PWM
     * @note Pin is automatically configured as OUTPUT
     */
    void startPWM(int pin, int dutyCycle, int frequency = DEFAULT_PWM_FREQUENCY);
    
    /**
     * @brief Update the duty cycle of an active PWM pin
     * 
     * @param pin GPIO pin number
     * @param dutyCycle New duty cycle value (0-255)
     * @return true if duty cycle was updated, false if PWM not active on this pin
     * 
     * @note This is a fast operation that doesn't restart the PWM thread
     * @note Changes take effect on the next PWM cycle
     */
    bool setDutyCycle(int pin, int dutyCycle);
    
    /**
     * @brief Stop PWM output on a GPIO pin
     * 
     * @param pin GPIO pin number to stop PWM on
     * @return true if PWM was stopped, false if PWM was not active
     * 
     * @note Pin is left in OUTPUT mode, set to LOW
     * @note Safe to call even if PWM is not active
     */
    bool stopPWM(int pin);
    
    /**
     * @brief Check if PWM is active on a pin
     * 
     * @param pin GPIO pin number to check
     * @return true if PWM is active, false otherwise
     */
    bool isActive(int pin) const;
    
    /**
     * @brief Get the number of active PWM outputs
     * 
     * @return Number of pins with active PWM
     */
    size_t getActiveCount() const;
    
    /**
     * @brief Get the current duty cycle of a PWM pin
     * 
     * @param pin GPIO pin number
     * @return Duty cycle (0-255), or -1 if PWM not active on this pin
     */
    int getDutyCycle(int pin) const;
    
    /**
     * @brief Get the current frequency of a PWM pin
     * 
     * @param pin GPIO pin number
     * @return Frequency in Hz, or -1 if PWM not active on this pin
     */
    int getFrequency(int pin) const;
    
    // Prevent copying
    PWMManager(const PWMManager&) = delete;
    PWMManager& operator=(const PWMManager&) = delete;
    
private:
    PWMManager();
    ~PWMManager();
    
    /**
     * @brief PWM generation thread function
     * 
     * Generates PWM pulses by rapidly toggling the pin state.
     * Uses high-resolution timing for accurate pulse width.
     * 
     * @param channel Pointer to the PWM channel to manage
     */
    static void pwmThreadFunction(PWMChannel* channel);
    
    /**
     * @brief Validate duty cycle value
     * @param dutyCycle Duty cycle to validate (0-255)
     * @return Clamped duty cycle value
     */
    static int validateDutyCycle(int dutyCycle);
    
    /**
     * @brief Validate frequency value
     * @param frequency Frequency to validate
     * @return Clamped frequency value
     */
    static int validateFrequency(int frequency);
    
    std::map<int, std::unique_ptr<PWMChannel>> channels_; ///< Active PWM channels
    mutable std::mutex mutex_;                             ///< Protects channels_ map
};
