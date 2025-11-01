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

#include "ArduinoCompat.hpp"
#include <iostream>

void setup() 
{
    // Set pin 17 as output (just like Arduino!)
    pinMode(17, OUTPUT);
}


void loop() 
{
    std::cout << "Blinking LED on GPIO pin 17..." << std::endl;
    
    // Blink the LED
    while (true) {
        digitalWrite(17, HIGH);  // Turn LED on
        std::cout << "LED ON" << std::endl;
        delay(1000);             // Arduino-style delay!
        
        digitalWrite(17, LOW);   // Turn LED off
        std::cout << "LED OFF" << std::endl;
        delay(1000);             // Arduino-style delay!
    }
}

int main() 
{
    setup();
    loop();
    return 0;
}