#include "ArduinoCompat.hpp"
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    std::cout << "=== Testing Arduino-style Functions ===" << std::endl;
    
    try {
        // Test Arduino-style pinMode and digitalWrite
        std::cout << "\n1. Testing pinMode() and digitalWrite():" << std::endl;
        pinMode(18, OUTPUT);
        
        std::cout << "Blinking LED on pin 18..." << std::endl;
        for (int i = 0; i < 5; i++) {
            digitalWrite(18, HIGH);
            std::cout << "  Pin 18 -> HIGH" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            digitalWrite(18, LOW);
            std::cout << "  Pin 18 -> LOW" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
        // Test Arduino-style digitalRead
        std::cout << "\n2. Testing pinMode() for input and digitalRead():" << std::endl;
        pinMode(17, INPUT);
        
        std::cout << "Reading pin 17 state..." << std::endl;
        for (int i = 0; i < 3; i++) {
            int value = digitalRead(17);
            std::cout << "  Pin 17 state: " << (value == 1 ? "HIGH" : value == 0 ? "LOW" : "ERROR") << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        
        std::cout << "\n=== Arduino-style functions test completed! ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}