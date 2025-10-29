#include <iostream>
#include "pin.hpp"

int main() {
    Pin pin(17); // Create a Pin object for GPIO 17
    pin.setHigh();
    std::cout << "Pin 17 state: " << pin.read() << std::endl;
    pin.setLow();
    std::cout << "Pin 17 state: " << pin.read() << std::endl;
    return 0;
}