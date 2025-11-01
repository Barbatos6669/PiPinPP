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
 * @file arduino_migration/main.cpp
 * @brief Arduino to PiPinPP migration example with side-by-side comparison
 * 
 * This example shows how to migrate Arduino code to PiPinPP, demonstrating
 * both the similarities and differences. It includes commented Arduino code
 * alongside working PiPinPP code to help Arduino users understand the transition.
 * 
 * Hardware Setup:
 * - LED connected to GPIO pin 17 (positive leg + 220Ω resistor to GND)
 * - Button connected to GPIO pin 18 (one terminal to pin, other to GND)
 * - External 10kΩ pull-up resistor from pin 18 to 3.3V (until internal pull-ups implemented)
 * 
 * @author PiPinPP Team
 * @version 0.2.0
 * @date 2025-10-31
 */

#include "ArduinoCompat.hpp"
#include <iostream>

/*
 * ============================================================================
 * ARDUINO CODE (for reference - would work on Arduino)
 * ============================================================================
 * 
 * const int ledPin = 13;        // Arduino typically uses pin 13 for built-in LED
 * const int buttonPin = 2;      // Arduino digital pin 2
 * 
 * void setup() {
 *   Serial.begin(9600);
 *   pinMode(ledPin, OUTPUT);
 *   pinMode(buttonPin, INPUT_PULLUP);  // Built-in pull-up
 *   Serial.println("Arduino LED Button Example");
 * }
 * 
 * void loop() {
 *   int buttonState = digitalRead(buttonPin);
 *   
 *   if (buttonState == LOW) {  // Button pressed (pull-up logic)
 *     digitalWrite(ledPin, HIGH);
 *     Serial.println("Button pressed - LED ON");
 *   } else {
 *     digitalWrite(ledPin, LOW);
 *     Serial.println("Button released - LED OFF");
 *   }
 *   
 *   delay(100);
 * }
 * 
 * ============================================================================
 * PIPINPP CODE (actual working code below)
 * ============================================================================
 */

// Pin definitions (Raspberry Pi GPIO pins)
const int ledPin = 17;          // GPIO pin 17 for LED
const int buttonPin = 18;       // GPIO pin 18 for button

void setup() {
    // Initialize serial output (using std::cout instead of Serial)
    std::cout << "PiPinPP LED Button Example (Arduino Migration Demo)" << std::endl;
    std::cout << "====================================================" << std::endl;
    
    // Pin setup - same as Arduino!
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT);      // Note: INPUT_PULLUP not yet implemented
    
    std::cout << "Setup complete!" << std::endl;
    std::cout << "LED pin: GPIO " << ledPin << std::endl;
    std::cout << "Button pin: GPIO " << buttonPin << std::endl;
    std::cout << "Press button to control LED" << std::endl << std::endl;
}

void loop() {
    // Read button state - same as Arduino!
    int buttonState = digitalRead(buttonPin);
    
    // Control LED based on button state
    // Note: Logic may be inverted compared to Arduino due to pull-up differences
    if (buttonState == LOW) {       // Button pressed (connected to GND)
        digitalWrite(ledPin, HIGH); // LED on - same as Arduino!
        std::cout << "Button pressed - LED ON" << std::endl;
    } else {
        digitalWrite(ledPin, LOW);  // LED off - same as Arduino!
        std::cout << "Button released - LED OFF" << std::endl;
    }
    
    delay(100);  // Same as Arduino!
}

int main() {
    /*
     * Arduino Migration Notes:
     * ========================
     * 
     * SIMILARITIES (What stays the same):
     * - pinMode(), digitalWrite(), digitalRead() work identically
     * - HIGH, LOW, INPUT, OUTPUT constants are the same
     * - delay() function works the same way
     * - setup() and loop() pattern can be preserved
     * 
     * DIFFERENCES (What changes):
     * - Pin numbers: Arduino pins vs Raspberry Pi GPIO pins
     * - Serial.print() becomes std::cout (or you can add Serial compatibility)
     * - INPUT_PULLUP not yet implemented (use external pull-up resistor)
     * - main() function needed to call setup() and loop()
     * - Must run with sudo for GPIO access on Raspberry Pi
     * 
     * MIGRATION CHECKLIST:
     * □ Change pin numbers to Raspberry Pi GPIO pins
     * □ Replace Serial.print() with std::cout or implement Serial class
     * □ Add main() function to call setup() and loop()
     * □ Add external pull-up resistors where INPUT_PULLUP was used
     * □ Include "ArduinoCompat.hpp" instead of Arduino libraries
     * □ Compile with g++ instead of Arduino IDE
     * □ Run with sudo for GPIO permissions
     */
    
    std::cout << "=== ARDUINO TO PIPINPP MIGRATION DEMO ===" << std::endl;
    std::cout << "This example shows how Arduino code translates to PiPinPP" << std::endl;
    std::cout << "Check the source code comments for detailed migration notes" << std::endl << std::endl;
    
    try {
        setup();
        
        // Arduino loop() runs forever, so we simulate that
        while (true) {
            loop();
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << std::endl << "Migration troubleshooting:" << std::endl;
        std::cerr << "1. Run with sudo: sudo ./example_arduino_migration" << std::endl;
        std::cerr << "2. Check GPIO pin wiring matches code" << std::endl;
        std::cerr << "3. Verify libgpiod is installed" << std::endl;
        std::cerr << "4. Make sure no other processes are using GPIO pins" << std::endl;
        return 1;
    }
    
    return 0;
}