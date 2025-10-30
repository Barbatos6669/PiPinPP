#include "ArduinoCompat.hpp"
#include <unistd.h>
#include <iostream>

void setup() 
{
    // Set pin 17 as output (just like Arduino!)
    pinMode(17, OUTPUT);
}


void loop() 
{
    std::cout << "Blinking LED on GPIO pin 17..." << std::endl;
    
    // Set pin 17 as output (just like Arduino!)
    pinMode(17, OUTPUT);
    
    // Blink the LED
    while (true) {
        digitalWrite(17, HIGH);  // Turn LED on
        std::cout << "LED ON" << std::endl;
        sleep(1);
        
        digitalWrite(17, LOW);   // Turn LED off
        std::cout << "LED OFF" << std::endl;
        sleep(1);
    }
}

int main() 
{
    setup();
    loop();
    return 0;
}