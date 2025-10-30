#include "pin.hpp"
#include <stdexcept>
#include <gpiod.h>
#include <iostream> // Temporary for debugging

Pin::Pin(int pin, PinDirection direction, const std::string& chipname) 
: pinNumber(pin) 
{
    chip = gpiod_chip_open_by_name(chipname.c_str()); 

    // Error handling for chip opening
    if (!chip) 
    {
        throw std::runtime_error("Failed to open GPIO chip: " + chipname);
    }

    // Get the GPIO line
    line = gpiod_chip_get_line(chip, pinNumber);
    if (!line)
    {
        gpiod_chip_close(chip);
        throw std::runtime_error("Failed to get GPIO line: " + std::to_string(pinNumber));
    }

    int requestResult;

    // Request the line as input or output
    if (direction == PinDirection::OUTPUT) 
    {
        requestResult = gpiod_line_request_output(line, "PiPinPP", 0);
    } 
    else // INPUT
    {
        requestResult = gpiod_line_request_input(line, "PiPinPP");
    }

    // Error handling for line request
    if (requestResult < 0) 
    {
        gpiod_chip_close(chip); // Clean up chip on failure
        throw std::runtime_error("Failed to request GPIO line: " + std::to_string(pinNumber));
    }

    // Initialize GPIO pin here
    std::cout << "Initializing pin " << pinNumber << " as " 
              << (direction == PinDirection::OUTPUT ? "OUTPUT" : "INPUT") 
              << " on chip " << chipname << std::endl;
}

Pin::~Pin() 
{
    // Release the line
    if (line)
    {
        gpiod_line_release(line);
    }

    // Close the chip
    if (chip)
    {
        gpiod_chip_close(chip);
    }

    std::cout << "Releasing resources for pin " << pinNumber << std::endl;
}

bool Pin::write(bool value) 
{
    if (!line) 
    {
        return false; // Line not initialized
    }

    return gpiod_line_set_value(line, value ? 1 : 0) == 0;
}

int Pin::read() 
{
    if (!line) 
    {
        return -1; // Line not initialized
    }

    return gpiod_line_get_value(line);
}