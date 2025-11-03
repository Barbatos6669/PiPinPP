/*
 * PiPin++ PWM LED Fade Example
 * 
 * Demonstrates PWM (Pulse Width Modulation) for smooth LED brightness control.
 * This example shows how to use analogWrite() to create fading effects.
 * 
 * Hardware Setup:
 * - Connect an LED with current-limiting resistor (220Ω-1kΩ) to GPIO 17
 * - LED anode (long leg) to GPIO 17
 * - LED cathode (short leg) to resistor, then to GND
 * 
 * Features Demonstrated:
 * - analogWrite() - Set PWM duty cycle for analog-like output
 * - Smooth LED fading (0-255 brightness range)
 * - Multiple fade patterns
 * - PWM frequency control
 * 
 * Wiring:
 * ```
 * GPIO 17 ----[ LED >|-- ]----[ 330Ω ]---- GND
 *         (anode) (cathode)  (resistor)
 * ```
 * 
 * Author: HobbyHacker / Barbatos6669
 * Date: November 3, 2025
 */

#include <ArduinoCompat.hpp>
#include <iostream>
#include <csignal>
#include <atomic>

// Global flag for graceful shutdown
std::atomic<bool> running(true);

/**
 * @brief Signal handler for graceful shutdown
 */
void signalHandler(int signal) {
    if (signal == SIGINT) {
        std::cout << "\nShutting down gracefully..." << std::endl;
        running = false;
    }
}

/**
 * @brief Fade LED from off to full brightness
 */
void fadeIn(int pin, int fadeTime = 2000) {
    std::cout << "  Fading in...";
    std::cout.flush();
    
    int steps = 256;
    int delayMs = fadeTime / steps;
    
    for (int brightness = 0; brightness <= 255 && running; brightness++) {
        analogWrite(pin, brightness);
        delay(delayMs);
    }
    
    std::cout << " Done!" << std::endl;
}

/**
 * @brief Fade LED from full brightness to off
 */
void fadeOut(int pin, int fadeTime = 2000) {
    std::cout << "  Fading out...";
    std::cout.flush();
    
    int steps = 256;
    int delayMs = fadeTime / steps;
    
    for (int brightness = 255; brightness >= 0 && running; brightness--) {
        analogWrite(pin, brightness);
        delay(delayMs);
    }
    
    std::cout << " Done!" << std::endl;
}

/**
 * @brief Pulse LED with breathing effect
 */
void breathe(int pin, int cycles = 3) {
    std::cout << "  Breathing pattern (" << cycles << " cycles)...";
    std::cout.flush();
    
    for (int i = 0; i < cycles && running; i++) {
        // Fade in
        for (int brightness = 0; brightness <= 255 && running; brightness++) {
            analogWrite(pin, brightness);
            delay(5);
        }
        
        // Fade out
        for (int brightness = 255; brightness >= 0 && running; brightness--) {
            analogWrite(pin, brightness);
            delay(5);
        }
    }
    
    std::cout << " Done!" << std::endl;
}

/**
 * @brief Demonstrate different brightness levels
 */
void demonstrateLevels(int pin) {
    std::cout << "  Demonstrating brightness levels:" << std::endl;
    
    int levels[] = {0, 32, 64, 128, 192, 255};
    const char* labels[] = {"Off", "12.5%", "25%", "50%", "75%", "100%"};
    
    for (int i = 0; i < 6 && running; i++) {
        std::cout << "    " << labels[i] << " (value=" << levels[i] << ")";
        std::cout.flush();
        analogWrite(pin, levels[i]);
        delay(1000);
        std::cout << " ✓" << std::endl;
    }
}

int main() {
    std::cout << "==========================================" << std::endl;
    std::cout << "   PiPin++ PWM LED Fade Example" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Hardware Setup:" << std::endl;
    std::cout << "  - LED on GPIO 17 with 330Ω resistor" << std::endl;
    std::cout << "  - LED anode (+) to GPIO 17" << std::endl;
    std::cout << "  - LED cathode (-) to resistor to GND" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PWM Configuration:" << std::endl;
    std::cout << "  - Frequency: 490Hz (default)" << std::endl;
    std::cout << "  - Resolution: 8-bit (0-255)" << std::endl;
    std::cout << "  - Mode: Software PWM" << std::endl;
    std::cout << std::endl;
    
    // Setup signal handler for Ctrl+C
    std::signal(SIGINT, signalHandler);
    
    const int LED_PIN = 17;
    
    try {
        // Configure GPIO 17 as output
        pinMode(LED_PIN, OUTPUT);
        std::cout << "✓ GPIO " << LED_PIN << " configured as OUTPUT" << std::endl;
        std::cout << std::endl;
        
        // Start with LED off
        analogWrite(LED_PIN, 0);
        
        std::cout << "=== Starting LED Fade Demonstrations ===" << std::endl;
        std::cout << "(Press Ctrl+C to exit)" << std::endl;
        std::cout << std::endl;
        
        int demoCount = 0;
        
        while (running) {
            demoCount++;
            std::cout << "Demo Cycle #" << demoCount << ":" << std::endl;
            
            // Demo 1: Simple fade in/out
            if (running) {
                std::cout << "\n1. Simple Fade:" << std::endl;
                fadeIn(LED_PIN);
                delay(500);
                fadeOut(LED_PIN);
                delay(1000);
            }
            
            // Demo 2: Breathing effect
            if (running) {
                std::cout << "\n2. Breathing Effect:" << std::endl;
                breathe(LED_PIN, 3);
                delay(1000);
            }
            
            // Demo 3: Discrete brightness levels
            if (running) {
                std::cout << "\n3. Brightness Levels:" << std::endl;
                demonstrateLevels(LED_PIN);
                delay(1000);
            }
            
            // Demo 4: Quick pulse
            if (running) {
                std::cout << "\n4. Quick Pulse:" << std::endl;
                std::cout << "  Pulsing...";
                std::cout.flush();
                for (int i = 0; i < 5 && running; i++) {
                    analogWrite(LED_PIN, 255);
                    delay(100);
                    analogWrite(LED_PIN, 0);
                    delay(100);
                }
                std::cout << " Done!" << std::endl;
                delay(1000);
            }
            
            if (running) {
                std::cout << "\n--- Cycle complete. Starting next cycle in 2 seconds ---\n" << std::endl;
                delay(2000);
            }
        }
        
        std::cout << std::endl;
        std::cout << "Cleaning up..." << std::endl;
        
        // Turn off LED
        analogWrite(LED_PIN, 0);
        std::cout << "✓ LED turned off" << std::endl;
        
        // Small delay to ensure PWM stops cleanly
        delay(100);
        
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
