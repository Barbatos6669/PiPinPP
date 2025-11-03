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
 * @file exception_handling/main.cpp
 * @brief Demonstrates proper exception handling with PiPinPP custom exceptions
 * 
 * This example shows:
 * - InvalidPinError for invalid pin numbers
 * - GpioAccessError for hardware access failures
 * - Exception hierarchy (catching as base PinError)
 * - Graceful error recovery
 * - Best practices for error handling
 * 
 * Hardware:
 * - No external hardware required
 * - Demonstrates error handling without GPIO access
 */

#include "pin.hpp"
#include "ArduinoCompat.hpp"
#include "exceptions.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

// Helper function to print section headers
void printSection(const std::string& title) {
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║ " << std::left << std::setw(54) << title << " ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n\n";
}

// Example 1: Handling invalid pin numbers
void example1_invalid_pin() {
    printSection("Example 1: Invalid Pin Number");
    
    std::cout << "Attempting to create pin with number 999...\n";
    
    try {
        Pin invalid_pin(999, PinDirection::OUTPUT);
        std::cout << "✗ No exception thrown (unexpected!)\n";
    }
    catch (const InvalidPinError& e) {
        std::cout << "✓ Caught InvalidPinError:\n";
        std::cout << "  Message: " << e.what() << "\n";
        std::cout << "  This is the correct exception type!\n";
    }
    catch (const std::exception& e) {
        std::cout << "⚠ Caught generic exception: " << e.what() << "\n";
    }
}

// Example 2: Handling negative pin numbers
void example2_negative_pin() {
    printSection("Example 2: Negative Pin Number");
    
    std::cout << "Attempting to create pin with number -5...\n";
    
    try {
        Pin negative_pin(-5, PinDirection::INPUT);
        std::cout << "✗ No exception thrown (unexpected!)\n";
    }
    catch (const InvalidPinError& e) {
        std::cout << "✓ Caught InvalidPinError:\n";
        std::cout << "  Message: " << e.what() << "\n";
    }
}

// Example 3: Handling invalid chip path
void example3_invalid_chip() {
    printSection("Example 3: Invalid GPIO Chip Path");
    
    std::cout << "Attempting to access non-existent chip '/dev/nonexistent'...\n";
    
    try {
        Pin pin(17, PinDirection::OUTPUT, "/dev/nonexistent");
        std::cout << "✗ No exception thrown (unexpected!)\n";
    }
    catch (const GpioAccessError& e) {
        std::cout << "✓ Caught GpioAccessError:\n";
        std::cout << "  Message: " << e.what() << "\n";
        std::cout << "  This indicates hardware access failure!\n";
    }
    catch (const std::exception& e) {
        std::cout << "⚠ Caught generic exception: " << e.what() << "\n";
    }
}

// Example 4: Exception hierarchy (catch as base class)
void example4_exception_hierarchy() {
    printSection("Example 4: Exception Hierarchy");
    
    std::cout << "Demonstrating that custom exceptions can be caught as PinError:\n\n";
    
    // Try invalid pin
    std::cout << "Test 1: Catching InvalidPinError as PinError...\n";
    try {
        Pin pin(999, PinDirection::OUTPUT);
    }
    catch (const PinError& e) {  // Note: catching base class
        std::cout << "✓ Successfully caught InvalidPinError as base PinError\n";
        std::cout << "  Message: " << e.what() << "\n\n";
    }
    
    // Try invalid chip
    std::cout << "Test 2: Catching GpioAccessError as PinError...\n";
    try {
        Pin pin(17, PinDirection::OUTPUT, "/dev/nonexistent");
    }
    catch (const PinError& e) {  // Note: catching base class
        std::cout << "✓ Successfully caught GpioAccessError as base PinError\n";
        std::cout << "  Message: " << e.what() << "\n";
    }
}

// Example 5: Arduino compatibility layer exceptions
void example5_arduino_exceptions() {
    printSection("Example 5: Arduino Function Exceptions");
    
    std::cout << "Testing Arduino compatibility functions:\n\n";
    
    // Test digitalWrite without pinMode
    std::cout << "Test 1: digitalWrite() without pinMode()...\n";
    try {
        digitalWrite(17, HIGH);
        std::cout << "✗ No exception thrown\n";
    }
    catch (const InvalidPinError& e) {
        std::cout << "✓ Caught InvalidPinError: " << e.what() << "\n\n";
    }
    
    // Test digitalRead without pinMode
    std::cout << "Test 2: digitalRead() without pinMode()...\n";
    try {
        int value = digitalRead(27);
        (void)value;  // Suppress unused warning
        std::cout << "✗ No exception thrown\n";
    }
    catch (const InvalidPinError& e) {
        std::cout << "✓ Caught InvalidPinError: " << e.what() << "\n\n";
    }
    
    // Test pinMode with invalid pin
    std::cout << "Test 3: pinMode() with invalid pin number...\n";
    try {
        pinMode(999, OUTPUT);
        std::cout << "✗ No exception thrown\n";
    }
    catch (const InvalidPinError& e) {
        std::cout << "✓ Caught InvalidPinError: " << e.what() << "\n";
    }
}

// Example 6: Practical error recovery
void example6_error_recovery() {
    printSection("Example 6: Graceful Error Recovery");
    
    std::cout << "Attempting to initialize GPIO with fallback options:\n\n";
    
    std::vector<int> pin_options = {17, 27, 22, 23};
    Pin* led = nullptr;
    
    for (int pin_num : pin_options) {
        std::cout << "Trying GPIO " << pin_num << "... ";
        try {
            led = new Pin(pin_num, PinDirection::OUTPUT);
            std::cout << "✓ Success!\n";
            break;
        }
        catch (const PinError& e) {
            std::cout << "✗ Failed: " << e.what() << "\n";
        }
    }
    
    if (led) {
        std::cout << "\nGPIO initialized successfully!\n";
        std::cout << "Blinking LED 3 times...\n";
        
        for (int i = 0; i < 3; i++) {
            led->write(HIGH);
            delay(200);
            led->write(LOW);
            delay(200);
        }
        
        delete led;
        std::cout << "Cleanup complete!\n";
    } else {
        std::cout << "\n⚠ Could not initialize any GPIO pin\n";
        std::cout << "This might indicate permission issues or missing hardware\n";
    }
}

// Example 7: Best practices
void example7_best_practices() {
    printSection("Example 7: Error Handling Best Practices");
    
    std::cout << "Best Practice 1: Specific exception handling\n";
    std::cout << "─────────────────────────────────────────────\n";
    try {
        Pin pin(999, PinDirection::OUTPUT);
    }
    catch (const InvalidPinError& e) {
        // Handle invalid pin specifically
        std::cout << "✓ Invalid pin - check your pin number\n";
    }
    catch (const GpioAccessError& e) {
        // Handle hardware issues specifically
        std::cout << "✓ Hardware access failed - check permissions\n";
    }
    catch (const PinError& e) {
        // Catch-all for any PinError
        std::cout << "✓ General GPIO error: " << e.what() << "\n";
    }
    catch (const std::exception& e) {
        // Ultimate fallback
        std::cout << "⚠ Unexpected error: " << e.what() << "\n";
    }
    
    std::cout << "\nBest Practice 2: RAII for automatic cleanup\n";
    std::cout << "─────────────────────────────────────────────\n";
    std::cout << "Pin objects use RAII - they clean up automatically!\n";
    std::cout << "No need to manually close GPIO handles.\n";
    
    std::cout << "\nBest Practice 3: Check error messages\n";
    std::cout << "─────────────────────────────────────────────\n";
    try {
        Pin pin(999, PinDirection::OUTPUT);
    }
    catch (const InvalidPinError& e) {
        std::cout << "Exception message contains useful info:\n";
        std::cout << "  \"" << e.what() << "\"\n";
        std::cout << "  ↑ Notice it includes the invalid pin number!\n";
    }
}

int main() 
{
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║         PiPinPP Exception Handling Examples           ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";

    example1_invalid_pin();
    example2_negative_pin();
    example3_invalid_chip();
    example4_exception_hierarchy();
    example5_arduino_exceptions();
    example6_error_recovery();
    example7_best_practices();

    // Final summary
    printSection("Summary: Exception Types");
    
    std::cout << "PinError (base class)\n";
    std::cout << "  ├─ InvalidPinError\n";
    std::cout << "  │    • Invalid pin numbers (negative, too high)\n";
    std::cout << "  │    • Using pins before pinMode()\n";
    std::cout << "  │    • Configuration errors\n";
    std::cout << "  │\n";
    std::cout << "  └─ GpioAccessError\n";
    std::cout << "       • Cannot open GPIO chip\n";
    std::cout << "       • Permission denied\n";
    std::cout << "       • Hardware not available\n";
    std::cout << "       • libgpiod failures\n\n";
    
    std::cout << "💡 Tips:\n";
    std::cout << "  • Always use try-catch with GPIO operations\n";
    std::cout << "  • Catch specific exceptions before general ones\n";
    std::cout << "  • Check exception messages for debugging info\n";
    std::cout << "  • RAII handles cleanup automatically\n\n";

    return 0;
}
