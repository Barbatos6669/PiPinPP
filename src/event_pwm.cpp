/**
 * @file event_pwm.cpp
 * @brief Implementation of event-driven PWM with reduced CPU usage
 * @author Barbatos6669
 * @date 2025
 * 
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

#include "event_pwm.hpp"
#include "exceptions.hpp"
#include "log.hpp"
#include <algorithm>
#include <chrono>
#include <sstream>

namespace pipinpp {

// EventPWM Implementation

EventPWM::EventPWM(int pin) 
    : pin_(pin), active_(false), dutyCycle_(0.0), frequencyHz_(490.0) {
    
    // Validate pin number
    if (pin < 0 || pin > 27) {
        throw InvalidPinError("Invalid pin number: " + std::to_string(pin) + 
                            " (must be 0-27 for Raspberry Pi)");
    }
    
    PIPINPP_LOG_DEBUG("EventPWM created for pin " << pin);
}

EventPWM::~EventPWM() {
    end();
}

bool EventPWM::begin(double frequencyHz, double dutyCycle) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Validate parameters
    if (frequencyHz < 50.0 || frequencyHz > 10000.0) {
        PIPINPP_LOG_ERROR("Frequency out of range: " << frequencyHz << " Hz (must be 50-10000)");
        return false;
    }
    
    if (dutyCycle < 0.0 || dutyCycle > 100.0) {
        PIPINPP_LOG_ERROR("Duty cycle out of range: " << dutyCycle << "% (must be 0-100)");
        return false;
    }
    
    // If already running, update parameters
    if (active_) {
        frequencyHz_ = frequencyHz;
        dutyCycle_ = dutyCycle;
        PIPINPP_LOG_INFO("Updated EventPWM pin " << pin_ << ": " << frequencyHz << " Hz, " << dutyCycle << "%");
        return true;
    }
    
    // Create Pin object
    try {
        pinObj_ = std::make_unique<Pin>(pin_, PinDirection::OUTPUT);
    } catch (const std::exception& e) {
        PIPINPP_LOG_ERROR("Failed to configure pin " << pin_ << " for PWM: " << e.what());
        return false;
    }
    
    // Set parameters
    frequencyHz_ = frequencyHz;
    dutyCycle_ = dutyCycle;
    
    // Start PWM thread
    active_ = true;
    pwmThread_ = std::thread(&EventPWM::pwmThreadFunction, this);
    
    PIPINPP_LOG_INFO("Started EventPWM on pin " << pin_ << ": " << frequencyHz << " Hz, " << dutyCycle << "%");
    return true;
}

void EventPWM::end() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!active_) {
        return;
    }
    
    // Stop thread
    active_ = false;
    
    if (pwmThread_.joinable()) {
        pwmThread_.join();
    }
    
    // Set pin LOW
    if (pinObj_) {
        pinObj_->write(false);
        pinObj_.reset();
    }
    
    PIPINPP_LOG_INFO("Stopped EventPWM on pin " << pin_);
}

bool EventPWM::setDutyCycle(double dutyCycle) {
    if (dutyCycle < 0.0 || dutyCycle > 100.0) {
        PIPINPP_LOG_ERROR("Duty cycle out of range: " << dutyCycle << "%");
        return false;
    }
    
    dutyCycle_ = dutyCycle;
    PIPINPP_LOG_DEBUG("EventPWM pin " << pin_ << " duty cycle set to " << dutyCycle << "%");
    return true;
}

bool EventPWM::setDutyCycle8Bit(uint8_t value) {
    double dutyCycle = (static_cast<double>(value) / 255.0) * 100.0;
    return setDutyCycle(dutyCycle);
}

bool EventPWM::setFrequency(double frequencyHz) {
    if (frequencyHz < 50.0 || frequencyHz > 10000.0) {
        PIPINPP_LOG_ERROR("Frequency out of range: " << frequencyHz << " Hz");
        return false;
    }
    
    frequencyHz_ = frequencyHz;
    PIPINPP_LOG_DEBUG("EventPWM pin " << pin_ << " frequency set to " << frequencyHz << " Hz");
    return true;
}

double EventPWM::getDutyCycle() const {
    return dutyCycle_.load();
}

double EventPWM::getFrequency() const {
    return frequencyHz_.load();
}

bool EventPWM::isActive() const {
    return active_.load();
}

void EventPWM::pwmThreadFunction() {
    PIPINPP_LOG_DEBUG("EventPWM thread started for pin " << pin_);
    
    while (active_) {
        double freq = frequencyHz_.load();
        double duty = dutyCycle_.load();
        
        // Calculate period in nanoseconds
        long periodNs = static_cast<long>(1000000000.0 / freq);
        long onTimeNs = static_cast<long>((periodNs * duty) / 100.0);
        long offTimeNs = periodNs - onTimeNs;
        
        // Handle edge cases
        if (duty <= 0.1) {
            // Always off (< 0.1% duty)
            pinObj_->write(false);
            hybridSleep(periodNs);
            continue;
        } else if (duty >= 99.9) {
            // Always on (> 99.9% duty)
            pinObj_->write(true);
            hybridSleep(periodNs);
            continue;
        }
        
        // Get absolute time for cycle start
        struct timespec cycleStart;
        clock_gettime(CLOCK_MONOTONIC, &cycleStart);
        
        // HIGH period
        pinObj_->write(true);
        if (onTimeNs > 0) {
            hybridSleep(onTimeNs);
        }
        
        // LOW period
        pinObj_->write(false);
        if (offTimeNs > 0) {
            hybridSleep(offTimeNs);
        }
    }
    
    // Ensure pin is LOW when stopping
    if (pinObj_) {
        pinObj_->write(false);
    }
    
    PIPINPP_LOG_DEBUG("EventPWM thread stopped for pin " << pin_);
}

void EventPWM::hybridSleep(long durationNs) {
    const long BUSYWAIT_THRESHOLD_NS = BUSYWAIT_THRESHOLD_US * 1000;
    
    if (durationNs <= BUSYWAIT_THRESHOLD_NS) {
        // Duration is short - pure busy-wait for precision
        auto start = std::chrono::high_resolution_clock::now();
        auto target = start + std::chrono::nanoseconds(durationNs);
        
        while (std::chrono::high_resolution_clock::now() < target && active_) {
            std::this_thread::yield();
        }
        return;
    }
    
    // Duration is long - hybrid approach
    // 1. Sleep most of the duration using clock_nanosleep()
    long sleepNs = durationNs - BUSYWAIT_THRESHOLD_NS;
    
    struct timespec req;
    clock_gettime(CLOCK_MONOTONIC, &req);
    
    // Add sleep duration to current time
    req.tv_nsec += sleepNs;
    
    // Handle nanosecond overflow
    if (req.tv_nsec >= 1000000000) {
        req.tv_sec += req.tv_nsec / 1000000000;
        req.tv_nsec = req.tv_nsec % 1000000000;
    }
    
    // Sleep using absolute time (immune to system time changes)
    if (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &req, nullptr) != 0) {
        // Sleep interrupted or error - fall back to busy-wait
        PIPINPP_LOG_DEBUG("clock_nanosleep interrupted, using busy-wait");
    }
    
    // 2. Busy-wait the final threshold microseconds for precision
    auto busyStart = std::chrono::high_resolution_clock::now();
    auto busyTarget = busyStart + std::chrono::nanoseconds(BUSYWAIT_THRESHOLD_NS);
    
    while (std::chrono::high_resolution_clock::now() < busyTarget && active_) {
        std::this_thread::yield();
    }
}

// EventPWMManager Implementation

EventPWMManager& EventPWMManager::getInstance() {
    static EventPWMManager instance;
    return instance;
}

EventPWMManager::~EventPWMManager() {
    PIPINPP_LOG_DEBUG("EventPWMManager shutting down");
    std::lock_guard<std::mutex> lock(mutex_);
    channels_.clear(); // Destructors will stop all PWM
}

void EventPWMManager::analogWriteEvent(int pin, int value, int frequencyHz) {
    // Validate and clamp value
    value = std::clamp(value, 0, 255);
    frequencyHz = std::clamp(frequencyHz, 50, 10000);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if PWM already exists
    auto it = channels_.find(pin);
    if (it != channels_.end()) {
        // Update existing PWM
        it->second->setDutyCycle8Bit(static_cast<uint8_t>(value));
        it->second->setFrequency(static_cast<double>(frequencyHz));
        return;
    }
    
    // Create new EventPWM
    try {
        auto pwm = std::make_unique<EventPWM>(pin);
        double dutyCycle = (static_cast<double>(value) / 255.0) * 100.0;
        
        if (!pwm->begin(static_cast<double>(frequencyHz), dutyCycle)) {
            PIPINPP_LOG_ERROR("Failed to start EventPWM on pin " << pin);
            return;
        }
        
        channels_[pin] = std::move(pwm);
        
    } catch (const std::exception& e) {
        PIPINPP_LOG_ERROR("Failed to create EventPWM on pin " << pin << ": " << e.what());
    }
}

bool EventPWMManager::stopPWM(int pin) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = channels_.find(pin);
    if (it == channels_.end()) {
        return false;
    }
    
    channels_.erase(it);
    PIPINPP_LOG_INFO("Stopped EventPWM on pin " << pin);
    return true;
}

bool EventPWMManager::isActive(int pin) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return channels_.find(pin) != channels_.end();
}

size_t EventPWMManager::getActiveCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return channels_.size();
}

} // namespace pipinpp
