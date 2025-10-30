#include <iostream>
#include "pin.hpp"

int main() 
{
    // test Pin class functionality
    try 
    {
        Pin led(17, PinDirection::OUTPUT);
        if (!led.write(true)) 
        {
            std::cerr << "Failed to set pin high" << std::endl;
            return 1;
        }
        if (!led.write(false)) 
        {
            std::cerr << "Failed to set pin low" << std::endl;
            return 1;
        }
        Pin button(18, PinDirection::INPUT);
        int state = button.read();
        if (state == -1) 
        {
            std::cerr << "Failed to read pin state" << std::endl;
            return 1;
        }
        std::cout << "Button state: " << state << std::endl;
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}