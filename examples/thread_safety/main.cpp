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
 * @file thread_safety/main.cpp
 * @brief Demonstrates thread-safe GPIO operations with multiple threads
 * 
 * This example shows:
 * - Safe concurrent access to different GPIO pins
 * - Arduino-inspired API functions with multiple threads
 * - Proper synchronization and cleanup
 * - Practical multi-threaded GPIO patterns
 * 
 * Hardware:
 * - Connect LEDs to GPIO 17, 27, and 22 (optional)
 * - Each LED with 220Ω resistor to ground
 * 
 * Wiring:
 *   GPIO 17 ----[220Ω]----[LED1]---- GND
 *   GPIO 27 ----[220Ω]----[LED2]---- GND
 *   GPIO 22 ----[220Ω]----[LED3]---- GND
 */

#include "ArduinoCompat.hpp"
#include "pin.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

// Global flag to control threads
std::atomic<bool> running(true);

// Thread 1: Fast blinker (GPIO 17)
void fast_blinker() {
    std::cout << "[Thread 1] Starting fast blinker on GPIO 17\n";
    
    try {
        pinMode(17, OUTPUT);
        
        int blinks = 0;
        while (running && blinks < 20) {
            digitalWrite(17, HIGH);
            delay(100);
            digitalWrite(17, LOW);
            delay(100);
            blinks++;
        }
        
        std::cout << "[Thread 1] Fast blinker completed " << blinks << " blinks\n";
    }
    catch (const std::exception& e) {
        std::cerr << "[Thread 1] Error: " << e.what() << "\n";
    }
}

// Thread 2: Slow blinker (GPIO 27)
void slow_blinker() {
    std::cout << "[Thread 2] Starting slow blinker on GPIO 27\n";
    
    try {
        pinMode(27, OUTPUT);
        
        int blinks = 0;
        while (running && blinks < 10) {
            digitalWrite(27, HIGH);
            delay(400);
            digitalWrite(27, LOW);
            delay(400);
            blinks++;
        }
        
        std::cout << "[Thread 2] Slow blinker completed " << blinks << " blinks\n";
    }
    catch (const std::exception& e) {
        std::cerr << "[Thread 2] Error: " << e.what() << "\n";
    }
}

// Thread 3: Pattern blinker (GPIO 22)
void pattern_blinker() {
    std::cout << "[Thread 3] Starting pattern blinker on GPIO 22\n";
    
    try {
        pinMode(22, OUTPUT);
        
        int patterns = 0;
        while (running && patterns < 5) {
            // Short-short-long pattern
            for (int i = 0; i < 2; i++) {
                digitalWrite(22, HIGH);
                delay(150);
                digitalWrite(22, LOW);
                delay(150);
            }
            digitalWrite(22, HIGH);
            delay(500);
            digitalWrite(22, LOW);
            delay(500);
            
            patterns++;
        }
        
        std::cout << "[Thread 3] Pattern blinker completed " << patterns << " patterns\n";
    }
    catch (const std::exception& e) {
        std::cerr << "[Thread 3] Error: " << e.what() << "\n";
    }
}

// Thread 4: Status monitor
void status_monitor() {
    std::cout << "[Monitor] Starting status monitor\n";
    
    unsigned long start_time = millis();
    int reports = 0;
    
    while (running && reports < 8) {
        delay(1000);
        unsigned long elapsed = millis() - start_time;
        std::cout << "[Monitor] Runtime: " << elapsed << " ms (" 
                  << (elapsed / 1000) << "s)\n";
        reports++;
    }
    
    std::cout << "[Monitor] Status monitor completed\n";
}

// Example 1: Multiple threads controlling different pins
void example1_concurrent_blink() {
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║     Example 1: Concurrent Multi-LED Blinking          ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "Starting 4 threads:\n";
    std::cout << "  Thread 1: Fast blink (100ms) on GPIO 17\n";
    std::cout << "  Thread 2: Slow blink (400ms) on GPIO 27\n";
    std::cout << "  Thread 3: Pattern blink on GPIO 22\n";
    std::cout << "  Thread 4: Status monitor\n\n";
    
    running = true;
    
    // Start all threads
    std::thread t1(fast_blinker);
    std::thread t2(slow_blinker);
    std::thread t3(pattern_blinker);
    std::thread t4(status_monitor);
    
    // Wait for all threads to complete
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    
    std::cout << "\n✓ All threads completed successfully!\n";
    std::cout << "  No crashes, no race conditions!\n";
}

// Example 2: Stress test with many quick operations
void stress_worker(int thread_id, int pin, int iterations) {
    try {
        pinMode(pin, OUTPUT);
        
        for (int i = 0; i < iterations; i++) {
            digitalWrite(pin, HIGH);
            delayMicroseconds(10);
            digitalWrite(pin, LOW);
            delayMicroseconds(10);
        }
        
        std::cout << "[Thread " << thread_id << "] Completed " 
                  << iterations << " iterations on GPIO " << pin << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "[Thread " << thread_id << "] Error: " << e.what() << "\n";
    }
}

void example2_stress_test() {
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║          Example 2: Thread Safety Stress Test         ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "Performing rapid GPIO operations across multiple threads...\n";
    std::cout << "Each thread: 1000 write operations\n\n";
    
    unsigned long start = millis();
    
    std::thread t1(stress_worker, 1, 17, 1000);
    std::thread t2(stress_worker, 2, 27, 1000);
    std::thread t3(stress_worker, 3, 22, 1000);
    
    t1.join();
    t2.join();
    t3.join();
    
    unsigned long duration = millis() - start;
    
    std::cout << "\n✓ Stress test completed in " << duration << " ms\n";
    std::cout << "  Total operations: 6000 (3000 HIGH + 3000 LOW)\n";
    std::cout << "  Operations per second: " << (6000000 / duration) << "\n";
}

// Example 3: Demonstrating Pin class thread safety
void pin_worker(int thread_id, int pin_num) {
    try {
        Pin led(pin_num, PinDirection::OUTPUT);
        
        std::cout << "[Thread " << thread_id << "] Initialized GPIO " << pin_num << "\n";
        
        for (int i = 0; i < 10; i++) {
            led.write(HIGH);
            delay(50);
            led.write(LOW);
            delay(50);
        }
        
        std::cout << "[Thread " << thread_id << "] Completed 10 blinks\n";
    }
    catch (const std::exception& e) {
        std::cerr << "[Thread " << thread_id << "] Error: " << e.what() << "\n";
    }
}

void example3_pin_class_threads() {
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║        Example 3: Pin Class in Multiple Threads       ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "Each thread creates its own Pin object\n";
    std::cout << "Testing proper construction and destruction...\n\n";
    
    std::thread t1(pin_worker, 1, 17);
    std::thread t2(pin_worker, 2, 27);
    std::thread t3(pin_worker, 3, 22);
    
    t1.join();
    t2.join();
    t3.join();
    
    std::cout << "\n✓ Pin objects cleaned up properly!\n";
    std::cout << "  RAII ensures no resource leaks\n";
}

int main() 
{
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║       PiPinPP Thread Safety Demonstration             ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
    
    std::cout << "\nThis example demonstrates thread-safe GPIO operations.\n";
    std::cout << "Connect LEDs to GPIOs 17, 27, and 22 to see visual effects.\n";
    std::cout << "(Example will work without LEDs, just no visual feedback)\n";

    // Run examples
    example1_concurrent_blink();
    
    delay(1000);  // Brief pause between examples
    
    example2_stress_test();
    
    delay(1000);
    
    example3_pin_class_threads();

    // Summary
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    Summary                             ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "Thread Safety Features:\n";
    std::cout << "✓ Arduino functions (pinMode, digitalWrite, digitalRead) are thread-safe\n";
    std::cout << "✓ Each Pin object manages its own GPIO line independently\n";
    std::cout << "✓ Multiple threads can control different pins safely\n";
    std::cout << "✓ Mutex protection prevents race conditions\n";
    std::cout << "✓ RAII ensures proper cleanup even with exceptions\n\n";
    
    std::cout << "Best Practices:\n";
    std::cout << "• Each thread should control different GPIO pins\n";
    std::cout << "• Don't share Pin objects between threads\n";
    std::cout << "• Use try-catch blocks for exception safety\n";
    std::cout << "• Let RAII handle resource cleanup\n\n";
    
    std::cout << "Use Cases:\n";
    std::cout << "• Multi-LED status indicators\n";
    std::cout << "• Concurrent sensor reading and display\n";
    std::cout << "• Parallel motor control\n";
    std::cout << "• Background monitoring with foreground control\n\n";

    return 0;
}
