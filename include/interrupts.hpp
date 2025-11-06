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
 * @file interrupts.hpp
 * @brief GPIO interrupt (edge detection) support for Raspberry Pi
 * @author Barbatos6669
 * @date 2025
 * 
 * @details
 * Provides Arduino-inspired interrupt handling using libgpiod v2 edge event API.
 * Allows registering callback functions to be invoked when GPIO pins change state.
 * 
 * Features:
 * - Rising, falling, and change (both) edge detection
 * - Thread-safe callback invocation
 * - Multiple interrupts on different pins simultaneously
 * - Efficient event monitoring with background thread
 * - Automatic cleanup on shutdown
 *
 * Example usage:
 * @code
 * #include "interrupts.hpp"
 * 
 * void buttonPressed() {
 *     std::cout << "Button pressed!" << std::endl;
 * }
 * 
 * int main() {
 *     // Attach interrupt to GPIO pin 18, trigger on falling edge
 *     attachInterrupt(18, buttonPressed, InterruptMode::FALLING);
 *     
 *     // Do other work...
 *     
 *     // Clean up when done
 *     detachInterrupt(18);
 *     return 0;
 * }
 * @endcode
 * 
 * @author HobbyHacker / Barbatos6669
 * @version 0.3.7
 * @date 2025-11-03
 */

#pragma once

#include <gpiod.h>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <map>
#include <string>

/**
 * @brief Interrupt trigger modes (Arduino-inspired)
 */
enum class InterruptMode {
    RISING,   ///< Trigger on rising edge (LOW to HIGH transition)
    FALLING,  ///< Trigger on falling edge (HIGH to LOW transition)
    CHANGE    ///< Trigger on any edge (both rising and falling)
};

/**
 * @brief Callback function signature for interrupts
 * 
 * User callbacks should be fast and non-blocking. Heavy processing
 * should be deferred to avoid blocking other interrupts.
 */
using InterruptCallback = std::function<void()>;

/**
 * @brief Internal structure for managing a single interrupt
 */
struct InterruptHandler {
    int pin;                              ///< GPIO pin number
    InterruptCallback callback;           ///< User callback function
    InterruptMode mode;                   ///< Edge detection mode
    gpiod_chip* chip;                     ///< GPIO chip handle
    gpiod_line_request* request;          ///< Line request for this pin
    gpiod_edge_event_buffer* event_buffer; ///< Buffer for reading edge events
    std::atomic<bool> active;             ///< Whether this handler is active
    
    InterruptHandler() 
        : pin(0), mode(InterruptMode::CHANGE), chip(nullptr), 
          request(nullptr), event_buffer(nullptr), active(false) {}
          
    ~InterruptHandler();
};

/**
 * @brief Manages GPIO interrupts with a background monitoring thread
 * 
 * This class is thread-safe and handles multiple interrupt sources.
 * It uses a single background thread to monitor all registered interrupts
 * and dispatches callbacks when edge events occur.
 * 
 * @note This is a singleton class - only one instance exists.
 */
class InterruptManager {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the global InterruptManager instance
     */
    static InterruptManager& getInstance();
    
    /**
     * @brief Attach an interrupt handler to a GPIO pin
     * 
     * @param pin GPIO pin number (0-27 for Raspberry Pi)
     * @param callback Function to call when interrupt triggers
     * @param mode Edge detection mode (RISING, FALLING, or CHANGE)
     * @param chipname GPIO chip name (default: "gpiochip0")
     * @throws InvalidPinError if pin number is invalid
     * @throws GpioAccessError if unable to configure interrupt
     * @throws std::runtime_error if interrupt already attached to this pin
     * 
     * @note Pin must not already have an interrupt attached
     * @note Callback should be fast and non-blocking
     */
    void attachInterrupt(int pin, InterruptCallback callback, InterruptMode mode, 
                        const std::string& chipname = "gpiochip0");
    
    /**
     * @brief Detach an interrupt handler from a GPIO pin
     * 
     * @param pin GPIO pin number to detach interrupt from
     * @return true if interrupt was detached, false if no interrupt was attached
     * 
     * @note Safe to call even if no interrupt is attached
     */
    bool detachInterrupt(int pin);
    
    /**
     * @brief Check if an interrupt is attached to a pin
     * 
     * @param pin GPIO pin number to check
     * @return true if an interrupt handler is attached, false otherwise
     */
    bool isAttached(int pin) const;
    
    /**
     * @brief Get the number of active interrupts
     * 
     * @return Number of pins with attached interrupt handlers
     */
    size_t getActiveCount() const;
    
    // Prevent copying
    InterruptManager(const InterruptManager&) = delete;
    InterruptManager& operator=(const InterruptManager&) = delete;
    
private:
    InterruptManager();
    ~InterruptManager();
    
    /**
     * @brief Background thread function that monitors all interrupt sources
     * 
     * Uses poll() to efficiently wait for edge events on multiple pins.
     * Dispatches callbacks when events occur.
     */
    void monitorThread();
    
    /**
     * @brief Start the monitoring thread if not already running
     */
    void startMonitoring();
    
    /**
     * @brief Stop the monitoring thread
     */
    void stopMonitoring();
    
    /**
     * @brief Convert InterruptMode to libgpiod edge type
     */
    static gpiod_line_edge modeToEdge(InterruptMode mode);
    
    std::map<int, std::unique_ptr<InterruptHandler>> handlers_; ///< Active interrupt handlers
    mutable std::mutex mutex_;                                   ///< Protects handlers_ map
    std::thread monitor_thread_;                                 ///< Background monitoring thread
    std::atomic<bool> running_;                                  ///< Whether monitoring thread is running
    std::atomic<bool> shutdown_requested_;                       ///< Shutdown flag
    int wakeup_pipe_[2];                                        ///< Pipe for waking up poll()
};
