#include "ArduinoCompat.hpp"
#include <unordered_map>
#include <memory>
#include <iostream>
#include <chrono>
#include <thread>

// Global storage for Arduino-style pins
static std::unordered_map<int, std::unique_ptr<Pin>> globalPins;

void pinMode(int pin, int mode) 
{
    try {
        PinDirection direction = (mode == OUTPUT) ? PinDirection::OUTPUT : PinDirection::INPUT;
        
        // Remove existing pin if it exists
        globalPins.erase(pin);
        
        // Create new pin with specified mode
        globalPins[pin] = std::make_unique<Pin>(pin, direction);
        
        std::cout << "pinMode: Set pin " << pin << " to " 
                  << (mode == OUTPUT ? "OUTPUT" : "INPUT") << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "pinMode error for pin " << pin << ": " << e.what() << std::endl;
    }
}

void digitalWrite(int pin, bool value) 
{
    auto it = globalPins.find(pin);
    if (it != globalPins.end()) {
        bool success = it->second->write(value);
        if (!success) {
            std::cerr << "digitalWrite failed for pin " << pin << std::endl;
        }
    } else {
        std::cerr << "digitalWrite: Pin " << pin << " not initialized. Call pinMode() first." << std::endl;
    }
}

int digitalRead(int pin) 
{
    auto it = globalPins.find(pin);
    if (it != globalPins.end()) {
        return it->second->read();
    } else {
        std::cerr << "digitalRead: Pin " << pin << " not initialized. Call pinMode() first." << std::endl;
        return -1;
    }
}

void delay(unsigned long ms) 
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}