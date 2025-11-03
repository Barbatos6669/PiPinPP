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

#include "pwm.hpp"
#include "exceptions.hpp"
#include "log.hpp"
#include <algorithm>

// PWMChannel destructor
PWMChannel::~PWMChannel() {
    active = false;
    if (pwmThread.joinable()) {
        pwmThread.join();
    }
}

// PWMManager implementation

PWMManager& PWMManager::getInstance() {
    static PWMManager instance;
    return instance;
}

PWMManager::PWMManager() {
    PIPINPP_LOG_DEBUG("PWMManager initialized");
}

PWMManager::~PWMManager() {
    PIPINPP_LOG_DEBUG("PWMManager shutting down");
    
    // Stop all PWM channels
    std::lock_guard<std::mutex> lock(mutex_);
    channels_.clear(); // Destructors will stop threads
}

void PWMManager::startPWM(int pin, int dutyCycle, int frequency) {
    // Validate pin number
    if (pin < 0 || pin > 27) {
        throw InvalidPinError("Invalid pin number: " + std::to_string(pin) + 
                            " (must be 0-27 for Raspberry Pi)");
    }
    
    // Validate and clamp values
    dutyCycle = validateDutyCycle(dutyCycle);
    frequency = validateFrequency(frequency);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if PWM already exists on this pin
    auto it = channels_.find(pin);
    if (it != channels_.end()) {
        // Update duty cycle only
        it->second->dutyCycle = dutyCycle;
        PIPINPP_LOG_INFO("Updated PWM duty cycle on pin " << pin << " to " << dutyCycle);
        return;
    }
    
    // Create new PWM channel
    auto channel = std::make_unique<PWMChannel>(pin);
    channel->dutyCycle = dutyCycle;
    channel->frequency = frequency;
    
    // Create Pin object for GPIO control
    try {
        channel->pinObj = std::make_unique<Pin>(pin, PinDirection::OUTPUT);
    } catch (const std::exception& e) {
        throw GpioAccessError("Failed to configure pin " + std::to_string(pin) + 
                            " for PWM: " + e.what());
    }
    
    // Start PWM thread
    channel->active = true;
    PWMChannel* channelPtr = channel.get();
    channel->pwmThread = std::thread(&PWMManager::pwmThreadFunction, channelPtr);
    
    // Add to channels map
    channels_[pin] = std::move(channel);
    
    PIPINPP_LOG_INFO("Started PWM on pin " << pin << " (duty=" << dutyCycle 
                     << ", freq=" << frequency << "Hz)");
}

bool PWMManager::setDutyCycle(int pin, int dutyCycle) {
    dutyCycle = validateDutyCycle(dutyCycle);
    
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = channels_.find(pin);
    if (it == channels_.end()) {
        return false; // PWM not active on this pin
    }
    
    it->second->dutyCycle = dutyCycle;
    PIPINPP_LOG_DEBUG("Updated PWM duty cycle on pin " << pin << " to " << dutyCycle);
    return true;
}

bool PWMManager::stopPWM(int pin) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = channels_.find(pin);
    if (it == channels_.end()) {
        return false; // PWM not active
    }
    
    // Remove channel (destructor will stop thread and clean up)
    channels_.erase(it);
    
    PIPINPP_LOG_INFO("Stopped PWM on pin " << pin);
    return true;
}

bool PWMManager::isActive(int pin) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return channels_.find(pin) != channels_.end();
}

size_t PWMManager::getActiveCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return channels_.size();
}

int PWMManager::getDutyCycle(int pin) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = channels_.find(pin);
    if (it == channels_.end()) {
        return -1;
    }
    return it->second->dutyCycle;
}

int PWMManager::getFrequency(int pin) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = channels_.find(pin);
    if (it == channels_.end()) {
        return -1;
    }
    return it->second->frequency;
}

void PWMManager::pwmThreadFunction(PWMChannel* channel) {
    PIPINPP_LOG_DEBUG("PWM thread started for pin " << channel->pin);
    
    while (channel->active) {
        int duty = channel->dutyCycle;
        int freq = channel->frequency;
        
        // Calculate period and timing
        double periodUs = 1000000.0 / freq; // Period in microseconds
        double onTimeUs = (periodUs * duty) / 255.0;
        
        // Handle edge cases
        if (duty == 0) {
            // Always off
            channel->pinObj->write(false);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        } else if (duty == 255) {
            // Always on
            channel->pinObj->write(true);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        // Generate PWM pulse
        auto cycleStart = std::chrono::high_resolution_clock::now();
        
        // HIGH period
        channel->pinObj->write(true);
        auto onTarget = cycleStart + std::chrono::nanoseconds(static_cast<long>(onTimeUs * 1000));
        while (std::chrono::high_resolution_clock::now() < onTarget && channel->active) {
            // Busy-wait for precision (necessary for software PWM)
            std::this_thread::yield();
        }
        
        // LOW period
        channel->pinObj->write(false);
        auto offTarget = cycleStart + std::chrono::nanoseconds(static_cast<long>(periodUs * 1000));
        while (std::chrono::high_resolution_clock::now() < offTarget && channel->active) {
            // Busy-wait for precision
            std::this_thread::yield();
        }
    }
    
    // Ensure pin is LOW when stopping
    if (channel->pinObj) {
        channel->pinObj->write(false);
    }
    
    PIPINPP_LOG_DEBUG("PWM thread stopped for pin " << channel->pin);
}

int PWMManager::validateDutyCycle(int dutyCycle) {
    return std::clamp(dutyCycle, 0, 255);
}

int PWMManager::validateFrequency(int frequency) {
    return std::clamp(frequency, MIN_PWM_FREQUENCY, MAX_PWM_FREQUENCY);
}
