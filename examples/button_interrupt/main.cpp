/*
 * PiPin++ Interrupt Example
 * 
 * Demonstrates GPIO interrupt handling (edge detection) with Arduino-inspired API.
 * This example shows how to detect button presses using interrupts instead of polling.
 * 
 * Hardware Setup:
 * - Connect a button between GPIO 18 and GND
 * - The internal pull-up resistor will be enabled
 * - Button press will trigger a FALLING edge interrupt
 * 
 * Features Demonstrated:
 * - attachInterrupt() - Register interrupt handler
 * - detachInterrupt() - Unregister interrupt handler
 * - Software debouncing in interrupt callback
 * - Non-blocking interrupt-driven input
 * 
 * Wiring:
 * ```
 * GPIO 18 ----[ Button ]---- GND
 * (Internal pull-up enabled)
 * ```
 * 
 * Author: HobbyHacker / Barbatos6669
 * Date: November 3, 2025
 */

#include <ArduinoCompat.hpp>
#include <iostream>
#include <atomic>
#include <csignal>

// Global variables for interrupt handler
std::atomic<unsigned long> buttonPressCount(0);
std::atomic<bool> running(true);

/**
 * @brief Interrupt service routine for button press
 * 
 * This function is called automatically when the button is pressed (FALLING edge).
 * 
 * @note Keep interrupt handlers fast and non-blocking
 * @note Use atomic variables for data shared between interrupt and main code
 */
void buttonInterruptHandler() {
    static unsigned long lastInterruptTime = 0;
    unsigned long interruptTime = millis();
    
    // Software debouncing: ignore if within 200ms of last press
    if (interruptTime - lastInterruptTime > 200) {
        buttonPressCount++;
        std::cout << "Button pressed! Count: " << buttonPressCount << std::endl;
        lastInterruptTime = interruptTime;
    }
}

/**
 * @brief Signal handler for graceful shutdown
 */
void signalHandler(int signal) {
    if (signal == SIGINT) {
        std::cout << "\nShutting down gracefully..." << std::endl;
        running = false;
    }
}

int main() {
    std::cout << "==========================================" << std::endl;
    std::cout << "   PiPin++ Interrupt Example" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Hardware Setup:" << std::endl;
    std::cout << "  - Button on GPIO 18 (connected to GND)" << std::endl;
    std::cout << "  - Pull-up resistor enabled" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Interrupt Configuration:" << std::endl;
    std::cout << "  - Trigger: FALLING edge (button press)" << std::endl;
    std::cout << "  - Debounce: 200ms software debouncing" << std::endl;
    std::cout << std::endl;
    
    // Setup signal handler for Ctrl+C
    std::signal(SIGINT, signalHandler);
    
    try {
        // Configure GPIO 18 as input with pull-up resistor
        pinMode(18, INPUT_PULLUP);
        std::cout << "✓ GPIO 18 configured as INPUT_PULLUP" << std::endl;
        
        // Attach interrupt to GPIO 18, trigger on FALLING edge (button press)
        attachInterrupt(18, buttonInterruptHandler, FALLING);
        std::cout << "✓ Interrupt attached to GPIO 18 (FALLING edge)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Ready! Press the button on GPIO 18..." << std::endl;
        std::cout << "(Press Ctrl+C to exit)" << std::endl;
        std::cout << std::endl;
        
        // Main loop - can do other work while interrupts handle button presses
        unsigned long lastReportTime = 0;
        while (running) {
            // Report status every 5 seconds
            unsigned long now = millis();
            if (now - lastReportTime >= 5000) {
                std::cout << "[" << now / 1000 << "s] Still running... "
                         << "Button presses: " << buttonPressCount << std::endl;
                lastReportTime = now;
            }
            
            // Sleep to reduce CPU usage (interrupt will still work)
            delay(100);
        }
        
        std::cout << std::endl;
        std::cout << "Final Statistics:" << std::endl;
        std::cout << "  Total button presses: " << buttonPressCount << std::endl;
        std::cout << std::endl;
        
        // Clean up: detach interrupt
        detachInterrupt(18);
        std::cout << "✓ Interrupt detached" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "Example completed successfully!" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    return 0;
}
