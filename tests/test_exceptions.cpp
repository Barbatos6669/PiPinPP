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

#include "pin.hpp"
#include "ArduinoCompat.hpp"
#include "exceptions.hpp"
#include <iostream>
#include <string>

int main() 
{
    int testsPassed = 0;
    int testsFailed = 0;

    std::cout << "Running exception handling tests...\n\n";

    // Test 1: InvalidPinError with out-of-range pin number
    std::cout << "Test 1: InvalidPinError with pin 999..." << std::endl;
    try {
        Pin invalidPin(999, PinDirection::OUTPUT);
        std::cout << "  FAILED: Expected InvalidPinError but no exception was thrown\n";
        testsFailed++;
    }
    catch (const InvalidPinError& e) {
        std::cout << "  PASSED: Caught InvalidPinError: " << e.what() << "\n";
        testsPassed++;
    }
    catch (const std::exception& e) {
        std::cout << "  FAILED: Caught wrong exception type: " << e.what() << "\n";
        testsFailed++;
    }

    // Test 2: GpioAccessError with invalid chip path
    std::cout << "\nTest 2: GpioAccessError with invalid chip path..." << std::endl;
    try {
        Pin invalidChip(17, PinDirection::OUTPUT, "/dev/nonexistent");
        std::cout << "  FAILED: Expected GpioAccessError but no exception was thrown\n";
        testsFailed++;
    }
    catch (const GpioAccessError& e) {
        std::cout << "  PASSED: Caught GpioAccessError: " << e.what() << "\n";
        testsPassed++;
    }
    catch (const std::exception& e) {
        std::cout << "  FAILED: Caught wrong exception type: " << e.what() << "\n";
        testsFailed++;
    }

    // Test 3: Arduino digitalWrite without pinMode
    std::cout << "\nTest 3: Arduino digitalWrite without pinMode..." << std::endl;
    try {
        digitalWrite(17, HIGH);
        std::cout << "  FAILED: Expected InvalidPinError but no exception was thrown\n";
        testsFailed++;
    }
    catch (const InvalidPinError& e) {
        std::cout << "  PASSED: Caught InvalidPinError: " << e.what() << "\n";
        testsPassed++;
    }
    catch (const std::exception& e) {
        std::cout << "  FAILED: Caught wrong exception type: " << e.what() << "\n";
        testsFailed++;
    }

    // Test 4: Arduino digitalRead without pinMode
    std::cout << "\nTest 4: Arduino digitalRead without pinMode..." << std::endl;
    try {
        int value = digitalRead(27);
        (void)value; // Suppress unused variable warning
        std::cout << "  FAILED: Expected InvalidPinError but no exception was thrown\n";
        testsFailed++;
    }
    catch (const InvalidPinError& e) {
        std::cout << "  PASSED: Caught InvalidPinError: " << e.what() << "\n";
        testsPassed++;
    }
    catch (const std::exception& e) {
        std::cout << "  FAILED: Caught wrong exception type: " << e.what() << "\n";
        testsFailed++;
    }

    // Test 5: Exception hierarchy - catch as base PinError
    std::cout << "\nTest 5: Exception hierarchy - catch InvalidPinError as PinError..." << std::endl;
    try {
        Pin invalidPin(999, PinDirection::OUTPUT);
        std::cout << "  FAILED: Expected exception but none was thrown\n";
        testsFailed++;
    }
    catch (const PinError& e) {
        std::cout << "  PASSED: Caught as base PinError: " << e.what() << "\n";
        testsPassed++;
    }
    catch (const std::exception& e) {
        std::cout << "  FAILED: Caught wrong exception type: " << e.what() << "\n";
        testsFailed++;
    }

    // Test 6: Exception hierarchy - catch GpioAccessError as PinError
    std::cout << "\nTest 6: Exception hierarchy - catch GpioAccessError as PinError..." << std::endl;
    try {
        Pin invalidChip(17, PinDirection::OUTPUT, "/dev/nonexistent");
        std::cout << "  FAILED: Expected exception but none was thrown\n";
        testsFailed++;
    }
    catch (const PinError& e) {
        std::cout << "  PASSED: Caught as base PinError: " << e.what() << "\n";
        testsPassed++;
    }
    catch (const std::exception& e) {
        std::cout << "  FAILED: Caught wrong exception type: " << e.what() << "\n";
        testsFailed++;
    }

    // Summary
    std::cout << "\n===========================================\n";
    std::cout << "Exception Test Results:\n";
    std::cout << "  Tests passed: " << testsPassed << "\n";
    std::cout << "  Tests failed: " << testsFailed << "\n";
    std::cout << "===========================================\n";

    return (testsFailed == 0) ? 0 : 1;
}
