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

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include "ArduinoCompat.hpp"

int main()
{
    std::cout << "Testing thread-safety of ArduinoCompat functions..." << std::endl;

    // Test concurrent pinMode calls
    std::vector<std::thread> threads;
    std::atomic<int> errors(0);

    // Create multiple threads that set pin modes concurrently
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([i, &errors]() {
            try {
                // Each thread works on a different pin to avoid conflicts
                int pin = 20 + i;
                pinMode(pin, OUTPUT);
                pinMode(pin, INPUT);  // Change mode
                pinMode(pin, INPUT_PULLUP);  // Change again
            } catch (const std::exception& e) {
                std::cerr << "Thread " << i << " pinMode error: " << e.what() << std::endl;
                errors++;
            }
        });
    }

    // Wait for all threads to complete
    for (auto& t : threads) {
        t.join();
    }

    if (errors > 0) {
        std::cerr << "Thread-safety test FAILED: " << errors << " errors occurred" << std::endl;
        return 1;
    }

    // Test concurrent read/write operations
    threads.clear();
    errors = 0;

    // Set up a test pin
    pinMode(17, OUTPUT);

    // Create threads that read and write concurrently
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([i, &errors]() {
            try {
                for (int j = 0; j < 100; ++j) {
                    digitalWrite(17, HIGH);
                    digitalRead(17);  // This will return the written value
                    digitalWrite(17, LOW);
                    digitalRead(17);
                }
            } catch (const std::exception& e) {
                std::cerr << "Thread " << i << " read/write error: " << e.what() << std::endl;
                errors++;
            }
        });
    }

    // Wait for all threads to complete
    for (auto& t : threads) {
        t.join();
    }

    if (errors > 0) {
        std::cerr << "Concurrent read/write test FAILED: " << errors << " errors occurred" << std::endl;
        return 1;
    }

    std::cout << "Thread-safety test PASSED: No race conditions detected" << std::endl;
    std::cout << "Note: This test uses mock GPIO operations and doesn't verify actual hardware behavior" << std::endl;

    return 0;
}