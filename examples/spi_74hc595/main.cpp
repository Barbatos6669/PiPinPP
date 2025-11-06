/**
 * @file main.cpp
 * @brief 74HC595 Shift Register Example - Hardware SPI vs Software shiftOut()
 * 
 * This example demonstrates controlling a 74HC595 shift register using both:
 * 1. Hardware SPI (faster, more reliable)
 * 2. Software bit-banging with shiftOut() (Arduino-style)
 * 
 * Performance comparison shows the speed advantage of hardware SPI.
 * 
 * Hardware Setup:
 * ===============
 * 74HC595 Shift Register:
 * - Pin 14 (DS/SER)   -> GPIO 10 (MOSI/SPI0_MOSI) or GPIO 23 (software data)
 * - Pin 11 (SHCP/SCK) -> GPIO 11 (SCLK/SPI0_SCLK) or GPIO 24 (software clock)
 * - Pin 12 (STCP/RCK) -> GPIO 8  (CE0/SPI0_CE0)   or GPIO 25 (software latch)
 * - Pin 13 (OE)       -> GND (output always enabled)
 * - Pin 10 (SRCLR)    -> 3.3V (no reset)
 * - Pin 16 (VCC)      -> 3.3V
 * - Pin 8  (GND)      -> GND
 * - Q0-Q7 (outputs)   -> LEDs with 220Ω resistors to GND
 * 
 * SPI vs Software Comparison:
 * ============================
 * Hardware SPI:
 * - Uses dedicated SPI peripheral
 * - Consistent timing, no CPU jitter
 * - Higher speed (up to 125 MHz on Pi 5)
 * - Frees CPU for other tasks
 * 
 * Software (shiftOut):
 * - Bit-banging on any GPIO pins
 * - Flexible pin selection
 * - Slower, subject to OS scheduling
 * - Uses more CPU cycles
 * 
 * @author PiPinPP Development Team
 * @version 0.3.7
 * @date 2025
 */

#include "ArduinoCompat.hpp"
#include "SPI.hpp"
#include "exceptions.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>

using namespace pipinpp;

// Hardware SPI pins (fixed on Raspberry Pi)
const int SPI_LATCH_PIN = 8;   // CE0 (GPIO 8) - used as latch for 74HC595

// Software bit-bang pins (can be any GPIO)
const int SW_DATA_PIN  = 23;   // DS/SER
const int SW_CLOCK_PIN = 24;   // SHCP/SCK
const int SW_LATCH_PIN = 25;   // STCP/RCK

/**
 * @brief Send byte to 74HC595 using hardware SPI
 * @param data Byte to send (controls Q0-Q7 outputs)
 */
void sendViaSPI(uint8_t data)
{
    digitalWrite(SPI_LATCH_PIN, LOW);   // Pull latch low to start
    SPI.transfer(data);                  // Send data via hardware SPI
    digitalWrite(SPI_LATCH_PIN, HIGH);  // Latch to update outputs
}

/**
 * @brief Send byte to 74HC595 using software shiftOut()
 * @param data Byte to send (controls Q0-Q7 outputs)
 */
void sendViaSoftware(uint8_t data)
{
    digitalWrite(SW_LATCH_PIN, LOW);                      // Pull latch low
    shiftOut(SW_DATA_PIN, SW_CLOCK_PIN, MSBFIRST, data); // Bit-bang data
    digitalWrite(SW_LATCH_PIN, HIGH);                     // Latch to update
}

/**
 * @brief Count up from 0-255 and measure time
 * @param useHardwareSPI true for SPI, false for software
 * @return Time taken in microseconds
 */
long long performanceTest(bool useHardwareSPI)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i <= 255; i++)
    {
        if (useHardwareSPI)
        {
            sendViaSPI(static_cast<uint8_t>(i));
        }
        else
        {
            sendViaSoftware(static_cast<uint8_t>(i));
        }
        delayMicroseconds(100);  // Small delay to see LEDs change
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

/**
 * @brief Display binary pattern on 74HC595 outputs
 * @param data Byte pattern to display
 * @param useHardwareSPI true for SPI, false for software
 */
void displayPattern(uint8_t data, bool useHardwareSPI)
{
    if (useHardwareSPI)
    {
        sendViaSPI(data);
    }
    else
    {
        sendViaSoftware(data);
    }
}

int main()
{
    std::cout << "\n74HC595 Shift Register - SPI vs Software Comparison\n";
    std::cout << "===================================================\n\n";
    
    try
    {
        // ===================================================================
        // Hardware SPI Setup
        // ===================================================================
        std::cout << "Initializing Hardware SPI...\n";
        
        if (!SPI.begin())
        {
            std::cerr << "ERROR: SPI hardware not available.\n";
            std::cerr << "Make sure /dev/spidev0.0 exists and is accessible.\n";
            std::cerr << "Run: ls -l /dev/spidev*\n\n";
            std::cerr << "Falling back to software mode only...\n\n";
        }
        else
        {
            // Configure SPI for 74HC595 (Mode 0, MSB first, 1 MHz)
            SPI.setDataMode(SPI_MODE0);
            SPI.setBitOrder(MSBFIRST);
            SPI.setClock(1000000);  // 1 MHz - safe for breadboard
            
            pinMode(SPI_LATCH_PIN, OUTPUT);
            digitalWrite(SPI_LATCH_PIN, HIGH);
            
            std::cout << "  SPI Mode: 0 (CPOL=0, CPHA=0)\n";
            std::cout << "  Bit Order: MSB First\n";
            std::cout << "  Clock: " << SPI.getClock() << " Hz\n";
            std::cout << "  Latch Pin: GPIO " << SPI_LATCH_PIN << "\n\n";
        }
        
        // ===================================================================
        // Software Bit-Bang Setup
        // ===================================================================
        std::cout << "Initializing Software Shift-Out...\n";
        
        pinMode(SW_DATA_PIN, OUTPUT);
        pinMode(SW_CLOCK_PIN, OUTPUT);
        pinMode(SW_LATCH_PIN, OUTPUT);
        digitalWrite(SW_LATCH_PIN, HIGH);
        
        std::cout << "  Data Pin: GPIO " << SW_DATA_PIN << "\n";
        std::cout << "  Clock Pin: GPIO " << SW_CLOCK_PIN << "\n";
        std::cout << "  Latch Pin: GPIO " << SW_LATCH_PIN << "\n\n";
        
        // ===================================================================
        // Demonstration: Common Patterns
        // ===================================================================
        std::cout << "Displaying common patterns...\n\n";
        
        struct Pattern
        {
            const char* name;
            uint8_t value;
            int delayMs;
        };
        
        Pattern patterns[] = {
            {"All OFF",          0b00000000, 500},
            {"All ON",           0b11111111, 500},
            {"Alternating 1",    0b10101010, 500},
            {"Alternating 2",    0b01010101, 500},
            {"Walking 1s Left",  0b00000001, 100},  // Will animate
            {"Walking 1s Right", 0b10000000, 100},  // Will animate
            {"Center Out",       0b00011000, 500},
        };
        
        for (const auto& pattern : patterns)
        {
            std::cout << "  " << std::setw(18) << std::left << pattern.name << ": ";
            std::cout << "0b";
            for (int bit = 7; bit >= 0; bit--)
            {
                std::cout << ((pattern.value >> bit) & 1);
            }
            std::cout << " (0x" << std::hex << std::setw(2) << std::setfill('0') 
                      << static_cast<int>(pattern.value) << std::dec << ")\n";
            
            // Show on hardware SPI if available
            if (SPI.isInitialized())
            {
                displayPattern(pattern.value, true);
                delay(pattern.delayMs);
            }
            
            // Always show on software (different pins)
            displayPattern(pattern.value, false);
            delay(pattern.delayMs);
        }
        
        // Animate walking 1s
        std::cout << "\nAnimating walking bits...\n";
        for (int i = 0; i < 8; i++)
        {
            uint8_t pattern = 1 << i;
            if (SPI.isInitialized()) sendViaSPI(pattern);
            sendViaSoftware(pattern);
            delay(100);
        }
        
        // ===================================================================
        // Performance Comparison
        // ===================================================================
        std::cout << "\nPerformance Test: Counting 0-255 on outputs\n";
        std::cout << "---------------------------------------------\n";
        
        long long softwareTime = performanceTest(false);
        std::cout << "  Software (shiftOut):  " << softwareTime << " µs\n";
        
        if (SPI.isInitialized())
        {
            long long hardwareTime = performanceTest(true);
            std::cout << "  Hardware SPI:         " << hardwareTime << " µs\n";
            
            double speedup = static_cast<double>(softwareTime) / hardwareTime;
            std::cout << "\n  Speedup: " << std::fixed << std::setprecision(2) 
                      << speedup << "x faster with hardware SPI\n";
        }
        
        // ===================================================================
        // Binary Counter Demo
        // ===================================================================
        std::cout << "\nBinary counter demo (press Ctrl+C to stop)...\n";
        std::cout << "Watch the LEDs count in binary!\n\n";
        
        uint8_t counter = 0;
        while (true)
        {
            if (SPI.isInitialized())
            {
                sendViaSPI(counter);
            }
            sendViaSoftware(counter);
            
            delay(200);
            counter++;
        }
    }
    catch (const PinError& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
