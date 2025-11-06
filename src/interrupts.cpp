/**
 * @file interrupts.cpp
 * @brief Implementation of GPIO interrupt (edge detection) support
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

#include "interrupts.hpp"
#include "exceptions.hpp"
#include "log.hpp"
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <algorithm>
#include <cstring>  // for strerror

// InterruptHandler destructor
InterruptHandler::~InterruptHandler() {
    active = false;
    
    if (event_buffer) {
        gpiod_edge_event_buffer_free(event_buffer);
        event_buffer = nullptr;
    }
    
    if (request) {
        gpiod_line_request_release(request);
        request = nullptr;
    }
    
    if (chip) {
        gpiod_chip_close(chip);
        chip = nullptr;
    }
}

// InterruptManager implementation

InterruptManager& InterruptManager::getInstance() {
    static InterruptManager instance;
    return instance;
}

InterruptManager::InterruptManager() 
    : running_(false), shutdown_requested_(false) {
    // Create wakeup pipe for signaling the monitor thread
    if (pipe(wakeup_pipe_) == -1) {
        throw std::runtime_error("Failed to create wakeup pipe for interrupt monitoring");
    }
    PIPINPP_LOG_DEBUG("InterruptManager initialized");
}

InterruptManager::~InterruptManager() {
    PIPINPP_LOG_DEBUG("InterruptManager shutting down");
    stopMonitoring();
    
    // Clean up all handlers
    std::lock_guard<std::mutex> lock(mutex_);
    handlers_.clear();
    
    // Close wakeup pipe
    close(wakeup_pipe_[0]);
    close(wakeup_pipe_[1]);
}

void InterruptManager::attachInterrupt(int pin, InterruptCallback callback, 
                                       InterruptMode mode, const std::string& chipname) {
    // Validate pin number (0-27 for Raspberry Pi)
    if (pin < 0 || pin > 27) {
        throw InvalidPinError("Invalid pin number: " + std::to_string(pin) + 
                            " (must be 0-27 for Raspberry Pi)");
    }
    
    if (!callback) {
        throw std::invalid_argument("Interrupt callback cannot be null");
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if interrupt already attached
    if (handlers_.find(pin) != handlers_.end()) {
        throw std::runtime_error("Interrupt already attached to pin " + std::to_string(pin));
    }
    
    // Create new handler
    auto handler = std::make_unique<InterruptHandler>();
    handler->pin = pin;
    handler->callback = callback;
    handler->mode = mode;
    
    // Open GPIO chip
    handler->chip = gpiod_chip_open(chipname.c_str());
    if (!handler->chip) {
        throw GpioAccessError("Failed to open GPIO chip: " + chipname);
    }
    
    // Configure line settings for edge detection
    gpiod_line_settings* settings = gpiod_line_settings_new();
    if (!settings) {
        gpiod_chip_close(handler->chip);
        throw std::runtime_error("Failed to create line settings");
    }
    
    // Set as input with edge detection
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_edge_detection(settings, modeToEdge(mode));
    
    // Create line config
    gpiod_line_config* line_cfg = gpiod_line_config_new();
    if (!line_cfg) {
        gpiod_line_settings_free(settings);
        gpiod_chip_close(handler->chip);
        throw std::runtime_error("Failed to create line config");
    }
    
    unsigned int pin_offset = static_cast<unsigned int>(pin);
    gpiod_line_config_add_line_settings(line_cfg, &pin_offset, 1, settings);
    
    // Create request config
    gpiod_request_config* req_cfg = gpiod_request_config_new();
    if (!req_cfg) {
        gpiod_line_config_free(line_cfg);
        gpiod_line_settings_free(settings);
        gpiod_chip_close(handler->chip);
        throw std::runtime_error("Failed to create request config");
    }
    
    gpiod_request_config_set_consumer(req_cfg, "PiPinPP-Interrupt");
    gpiod_request_config_set_event_buffer_size(req_cfg, 16);
    
    // Request the line
    handler->request = gpiod_chip_request_lines(handler->chip, req_cfg, line_cfg);
    
    // Clean up config objects
    gpiod_request_config_free(req_cfg);
    gpiod_line_config_free(line_cfg);
    gpiod_line_settings_free(settings);
    
    if (!handler->request) {
        gpiod_chip_close(handler->chip);
        throw GpioAccessError("Failed to request line for pin " + std::to_string(pin));
    }
    
    // Create event buffer
    handler->event_buffer = gpiod_edge_event_buffer_new(16);
    if (!handler->event_buffer) {
        gpiod_line_request_release(handler->request);
        gpiod_chip_close(handler->chip);
        throw std::runtime_error("Failed to create edge event buffer");
    }
    
    handler->active = true;
    
    // Add to handlers map
    handlers_[pin] = std::move(handler);
    
    PIPINPP_LOG_INFO("Interrupt attached to pin " << pin);
    
    // Start monitoring thread if not already running
    if (!running_) {
        startMonitoring();
    } else {
        // Wake up the monitor thread to reconfigure poll
        char wake = '1';
        ssize_t result = write(wakeup_pipe_[1], &wake, 1);
        (void)result; // Intentionally ignore - wakeup is best-effort
    }
}

bool InterruptManager::detachInterrupt(int pin) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = handlers_.find(pin);
    if (it == handlers_.end()) {
        return false; // No interrupt attached
    }
    
    // Remove handler (destructor will clean up resources)
    handlers_.erase(it);
    
    PIPINPP_LOG_INFO("Interrupt detached from pin " << pin);
    
    // Wake up the monitor thread to reconfigure poll
    if (running_) {
        char wake = '1';
        ssize_t result = write(wakeup_pipe_[1], &wake, 1);
        (void)result; // Intentionally ignore - wakeup is best-effort
    }
    
    return true;
}

bool InterruptManager::isAttached(int pin) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return handlers_.find(pin) != handlers_.end();
}

size_t InterruptManager::getActiveCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return handlers_.size();
}

void InterruptManager::startMonitoring() {
    if (running_) {
        return; // Already running
    }
    
    shutdown_requested_ = false;
    running_ = true;
    monitor_thread_ = std::thread(&InterruptManager::monitorThread, this);
    
    PIPINPP_LOG_DEBUG("Interrupt monitoring thread started");
}

void InterruptManager::stopMonitoring() {
    if (!running_) {
        return; // Not running
    }
    
    PIPINPP_LOG_DEBUG("Stopping interrupt monitoring thread");
    
    shutdown_requested_ = true;
    running_ = false;
    
    // Wake up the monitor thread
    char wake = '1';
    ssize_t result = write(wakeup_pipe_[1], &wake, 1);
    (void)result; // Intentionally ignore - wakeup is best-effort
    
    // Wait for thread to finish
    if (monitor_thread_.joinable()) {
        monitor_thread_.join();
    }
    
    PIPINPP_LOG_DEBUG("Interrupt monitoring thread stopped");
}

void InterruptManager::monitorThread() {
    PIPINPP_LOG_DEBUG("Monitor thread running");
    
    while (!shutdown_requested_) {
        std::vector<pollfd> poll_fds;
        std::vector<int> pin_map; // Maps poll_fds index to pin number
        
        {
            std::lock_guard<std::mutex> lock(mutex_);
            
            // Add wakeup pipe
            pollfd wakeup_fd;
            wakeup_fd.fd = wakeup_pipe_[0];
            wakeup_fd.events = POLLIN;
            wakeup_fd.revents = 0;
            poll_fds.push_back(wakeup_fd);
            pin_map.push_back(-1); // -1 indicates wakeup pipe
            
            // Add all active interrupt handlers
            for (auto& pair : handlers_) {
                if (pair.second->active) {
                    pollfd pfd;
                    pfd.fd = gpiod_line_request_get_fd(pair.second->request);
                    pfd.events = POLLIN;
                    pfd.revents = 0;
                    poll_fds.push_back(pfd);
                    pin_map.push_back(pair.first);
                }
            }
        }
        
        if (poll_fds.size() == 1) {
            // Only wakeup pipe, no active interrupts
            // Wait for new interrupts to be attached
            poll(poll_fds.data(), poll_fds.size(), -1);
            
            // Clear wakeup pipe
            char buf[16];
            ssize_t bytes_read = read(wakeup_pipe_[0], buf, sizeof(buf));
            (void)bytes_read; // Intentionally ignore - just clearing the pipe
            continue;
        }
        
        // Wait for events (timeout 100ms to check shutdown flag periodically)
        int ret = poll(poll_fds.data(), poll_fds.size(), 100);
        
        if (ret < 0) {
            if (errno == EINTR) {
                continue; // Interrupted by signal, retry
            }
            PIPINPP_LOG_ERROR("poll() failed: " << strerror(errno));
            break;
        }
        
        if (ret == 0) {
            continue; // Timeout, check shutdown flag
        }
        
        // Check wakeup pipe first
        if (poll_fds[0].revents & POLLIN) {
            char buf[16];
            ssize_t bytes_read = read(wakeup_pipe_[0], buf, sizeof(buf));
            (void)bytes_read; // Intentionally ignore - just clearing the pipe
            // Reconfigure poll on next iteration
            continue;
        }
        
        // Process events on interrupt pins
        for (size_t i = 1; i < poll_fds.size(); ++i) {
            if (poll_fds[i].revents & POLLIN) {
                int pin = pin_map[i];
                
                std::unique_lock<std::mutex> lock(mutex_);
                auto it = handlers_.find(pin);
                if (it == handlers_.end() || !it->second->active) {
                    continue; // Handler was removed
                }
                
                auto& handler = it->second;
                lock.unlock();
                
                // Read edge events
                int num_events = gpiod_line_request_read_edge_events(
                    handler->request, handler->event_buffer, 
                    gpiod_edge_event_buffer_get_capacity(handler->event_buffer)
                );
                
                if (num_events > 0) {
                    // Process all events
                    for (int j = 0; j < num_events; ++j) {
                        gpiod_edge_event* event = gpiod_edge_event_buffer_get_event(
                            handler->event_buffer, j
                        );
                        
                        if (event) {
                            // Invoke callback
                            try {
                                handler->callback();
                            } catch (const std::exception& e) {
                                PIPINPP_LOG_ERROR("Exception in interrupt callback for pin " 
                                         << pin << ": " << e.what());
                            } catch (...) {
                                PIPINPP_LOG_ERROR("Unknown exception in interrupt callback for pin " << pin);
                            }
                        }
                    }
                }
            }
        }
    }
    
    PIPINPP_LOG_DEBUG("Monitor thread exiting");
}

gpiod_line_edge InterruptManager::modeToEdge(InterruptMode mode) {
    switch (mode) {
        case InterruptMode::RISING:
            return GPIOD_LINE_EDGE_RISING;
        case InterruptMode::FALLING:
            return GPIOD_LINE_EDGE_FALLING;
        case InterruptMode::CHANGE:
            return GPIOD_LINE_EDGE_BOTH;
        default:
            return GPIOD_LINE_EDGE_BOTH;
    }
}
