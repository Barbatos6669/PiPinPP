/*
 * PiPinPP Utility Functions Example
 * 
 * This example demonstrates the Arduino-inspired utility functions:
 * - Random number generation (random, randomSeed)
 * - Bit manipulation (bit, bitRead, bitWrite, bitSet, bitClear)
 * - Byte extraction (highByte, lowByte)
 * 
 * Hardware: No hardware required! This runs without GPIO.
 * 
 * Compile:
 *   g++ main.cpp $(pkg-config --cflags --libs pipinpp) -o utility_functions
 * 
 * Run:
 *   ./utility_functions
 */

#include <ArduinoCompat.hpp>
#include <iostream>
#include <iomanip>
#include <vector>

// Helper function to print binary representation
void printBinary(unsigned long value, int bits = 8) {
    for (int i = bits - 1; i >= 0; i--) {
        std::cout << ((value >> i) & 1);
        if (i % 4 == 0 && i != 0) std::cout << " ";  // Space every 4 bits
    }
}

// Helper function to print a separator line
void printSeparator() {
    std::cout << "\n" << std::string(70, '=') << "\n\n";
}

/* ============================================================================
 * DEMONSTRATION 1: Random Number Generation
 * ============================================================================ */

void demonstrateRandomNumbers() {
    std::cout << "DEMONSTRATION 1: Random Number Generation\n";
    std::cout << std::string(70, '-') << "\n\n";
    
    // Seed with current time (in real applications, use micros())
    randomSeed(micros());
    std::cout << "✓ Seeded random generator with micros()\n\n";
    
    // Generate random numbers with single parameter (0 to max-1)
    std::cout << "Random numbers in range [0, 10):\n";
    for (int i = 0; i < 10; i++) {
        std::cout << "  " << random(10);
        if (i < 9) std::cout << ", ";
    }
    std::cout << "\n\n";
    
    // Generate random numbers with two parameters (min to max-1)
    std::cout << "Random numbers in range [20, 30):\n";
    for (int i = 0; i < 10; i++) {
        std::cout << "  " << random(20, 30);
        if (i < 9) std::cout << ", ";
    }
    std::cout << "\n\n";
    
    // Simulate dice rolls
    std::cout << "Simulate 20 dice rolls (1-6):\n";
    std::vector<int> counts(7, 0);  // Count occurrences of each number
    for (int i = 0; i < 20; i++) {
        int roll = random(1, 7);  // Random number 1-6
        counts[roll]++;
        std::cout << roll << " ";
    }
    std::cout << "\n\nDistribution:\n";
    for (int i = 1; i <= 6; i++) {
        std::cout << "  " << i << ": ";
        for (int j = 0; j < counts[i]; j++) {
            std::cout << "█";
        }
        std::cout << " (" << counts[i] << ")\n";
    }
    std::cout << "\n";
    
    // Random delays example (useful for LED effects)
    std::cout << "Random delay values for LED effects:\n";
    for (int i = 0; i < 5; i++) {
        long delayMs = random(100, 500);
        std::cout << "  Delay #" << (i+1) << ": " << delayMs << "ms\n";
    }
}

/* ============================================================================
 * DEMONSTRATION 2: Bit Manipulation
 * ============================================================================ */

void demonstrateBitManipulation() {
    std::cout << "\n\nDEMONSTRATION 2: Bit Manipulation\n";
    std::cout << std::string(70, '-') << "\n\n";
    
    // Computing bit values
    std::cout << "Computing bit values with bit(n):\n";
    std::cout << "┌──────┬─────────┬────────────┐\n";
    std::cout << "│  n   │  bit(n) │   Binary   │\n";
    std::cout << "├──────┼─────────┼────────────┤\n";
    for (int i = 0; i <= 7; i++) {
        unsigned long value = bit(i);
        std::cout << "│  " << i << "   │  " << std::setw(3) << value << "    │  ";
        printBinary(value, 8);
        std::cout << "  │\n";
    }
    std::cout << "└──────┴─────────┴────────────┘\n\n";
    
    // Reading bits from a value
    std::cout << "Reading individual bits with bitRead():\n";
    unsigned long flags = 0b10110100;  // Binary: 10110100 (180 decimal)
    std::cout << "Value: 0b";
    printBinary(flags, 8);
    std::cout << " (decimal: " << flags << ")\n\n";
    
    std::cout << "Bit positions:\n";
    for (int i = 7; i >= 0; i--) {
        std::cout << "  Bit " << i << ": " << bitRead(flags, i);
        if (bitRead(flags, i)) {
            std::cout << " (SET)";
        } else {
            std::cout << " (CLEAR)";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
    
    // Setting and clearing bits
    std::cout << "Modifying bits with bitSet() and bitClear():\n";
    unsigned long value = 0b00000000;
    std::cout << "Initial:        ";
    printBinary(value, 8);
    std::cout << " (" << value << ")\n";
    
    bitSet(value, 0);
    std::cout << "bitSet(0):      ";
    printBinary(value, 8);
    std::cout << " (" << value << ")\n";
    
    bitSet(value, 3);
    std::cout << "bitSet(3):      ";
    printBinary(value, 8);
    std::cout << " (" << value << ")\n";
    
    bitSet(value, 7);
    std::cout << "bitSet(7):      ";
    printBinary(value, 8);
    std::cout << " (" << value << ")\n";
    
    bitClear(value, 3);
    std::cout << "bitClear(3):    ";
    printBinary(value, 8);
    std::cout << " (" << value << ")\n\n";
    
    // Using bitWrite
    std::cout << "Writing specific bits with bitWrite():\n";
    value = 0b00000000;
    std::cout << "Initial:        ";
    printBinary(value, 8);
    std::cout << " (" << value << ")\n";
    
    bitWrite(value, 2, 1);
    std::cout << "bitWrite(2, 1): ";
    printBinary(value, 8);
    std::cout << " (" << value << ")\n";
    
    bitWrite(value, 5, 1);
    std::cout << "bitWrite(5, 1): ";
    printBinary(value, 8);
    std::cout << " (" << value << ")\n";
    
    bitWrite(value, 2, 0);
    std::cout << "bitWrite(2, 0): ";
    printBinary(value, 8);
    std::cout << " (" << value << ")\n";
}

/* ============================================================================
 * DEMONSTRATION 3: Byte Extraction
 * ============================================================================ */

void demonstrateByteExtraction() {
    std::cout << "\n\nDEMONSTRATION 3: Byte Extraction (High/Low Byte)\n";
    std::cout << std::string(70, '-') << "\n\n";
    
    std::cout << "Extracting high and low bytes from 16-bit values:\n\n";
    
    // Example values
    struct Example {
        unsigned int value;
        const char* description;
    };
    
    std::vector<Example> examples = {
        {0x1234, "Hex value 0x1234"},
        {0xABCD, "Hex value 0xABCD"},
        {0xFF00, "All high bits"},
        {0x00FF, "All low bits"},
        {12345, "Decimal 12345"},
        {65535, "Max 16-bit (0xFFFF)"}
    };
    
    std::cout << "┌─────────┬──────────────────┬────────────┬────────────┐\n";
    std::cout << "│  Value  │   Description    │  High Byte │  Low Byte  │\n";
    std::cout << "├─────────┼──────────────────┼────────────┼────────────┤\n";
    
    for (const auto& ex : examples) {
        unsigned char high = highByte(ex.value);
        unsigned char low = lowByte(ex.value);
        
        std::cout << "│ " << std::hex << std::setw(5) << std::setfill('0') 
                  << ex.value << " │ " << std::setw(16) << std::setfill(' ')
                  << ex.description << " │   0x" << std::hex << std::setw(2) 
                  << std::setfill('0') << (int)high << "     │   0x" 
                  << std::setw(2) << (int)low << "     │\n";
    }
    std::cout << "└─────────┴──────────────────┴────────────┴────────────┘\n";
    std::cout << std::dec << std::setfill(' ');  // Reset formatting
    
    std::cout << "\n\nPractical example: Sensor data transmission\n";
    std::cout << "┌──────────────────────────────────────────┐\n";
    
    // Simulate reading a 16-bit temperature sensor value
    unsigned int sensorValue = 2567;  // e.g., 25.67°C * 100
    std::cout << "│ Sensor reading: " << sensorValue << " (25.67°C)     │\n";
    std::cout << "│                                          │\n";
    std::cout << "│ Splitting for transmission:             │\n";
    
    unsigned char highData = highByte(sensorValue);
    unsigned char lowData = lowByte(sensorValue);
    
    std::cout << "│   High byte: 0x" << std::hex << std::setw(2) << std::setfill('0')
              << (int)highData << " (decimal: " << std::dec << (int)highData << ")      │\n";
    std::cout << "│   Low byte:  0x" << std::hex << std::setw(2) << std::setfill('0')
              << (int)lowData << " (decimal: " << std::dec << (int)lowData << ")      │\n";
    std::cout << "│                                          │\n";
    std::cout << "│ Reconstructing:                          │\n";
    
    unsigned int reconstructed = ((unsigned int)highData << 8) | lowData;
    std::cout << "│   Value: " << reconstructed << " ✓ Match!               │\n";
    std::cout << "└──────────────────────────────────────────┘\n";
}

/* ============================================================================
 * DEMONSTRATION 4: Practical Applications
 * ============================================================================ */

void demonstratePracticalApplications() {
    std::cout << "\n\nDEMONSTRATION 4: Practical Applications\n";
    std::cout << std::string(70, '-') << "\n\n";
    
    // Application 1: Status flags management
    std::cout << "Application 1: Managing device status flags\n";
    std::cout << "─────────────────────────────────────────────\n\n";
    
    unsigned long deviceStatus = 0;
    
    std::cout << "Device status bits:\n";
    std::cout << "  Bit 0: Power on/off\n";
    std::cout << "  Bit 1: Data ready\n";
    std::cout << "  Bit 2: Error flag\n";
    std::cout << "  Bit 3: Calibrated\n\n";
    
    std::cout << "Initial status: ";
    printBinary(deviceStatus, 8);
    std::cout << "\n\n";
    
    // Turn power on
    std::cout << "1. Turn power ON:\n";
    bitSet(deviceStatus, 0);
    std::cout << "   Status: ";
    printBinary(deviceStatus, 8);
    std::cout << "\n\n";
    
    // Set calibrated flag
    std::cout << "2. Device calibrated:\n";
    bitSet(deviceStatus, 3);
    std::cout << "   Status: ";
    printBinary(deviceStatus, 8);
    std::cout << "\n\n";
    
    // Data ready
    std::cout << "3. Data ready:\n";
    bitSet(deviceStatus, 1);
    std::cout << "   Status: ";
    printBinary(deviceStatus, 8);
    std::cout << "\n\n";
    
    // Check status
    std::cout << "4. Checking device status:\n";
    std::cout << "   Power is " << (bitRead(deviceStatus, 0) ? "ON" : "OFF") << "\n";
    std::cout << "   Data is " << (bitRead(deviceStatus, 1) ? "READY" : "NOT READY") << "\n";
    std::cout << "   Error is " << (bitRead(deviceStatus, 2) ? "PRESENT" : "CLEAR") << "\n";
    std::cout << "   Device is " << (bitRead(deviceStatus, 3) ? "CALIBRATED" : "NOT CALIBRATED") << "\n\n";
    
    // Application 2: Random LED patterns
    std::cout << "\nApplication 2: Random LED blink patterns\n";
    std::cout << "─────────────────────────────────────────────\n\n";
    
    randomSeed(micros());
    std::cout << "Generating 5 random LED patterns:\n\n";
    
    for (int pattern = 0; pattern < 5; pattern++) {
        std::cout << "Pattern " << (pattern + 1) << ": ";
        
        // Random number of blinks
        int blinks = random(3, 8);
        
        // Random delay between blinks
        int delayMs = random(100, 300);
        
        std::cout << blinks << " blinks, " << delayMs << "ms delay\n";
        std::cout << "           ";
        
        for (int i = 0; i < blinks; i++) {
            std::cout << "█";
            if (i < blinks - 1) {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
    
    std::cout << "\nThese random patterns create interesting visual effects!\n";
}

/* ============================================================================
 * MAIN FUNCTION
 * ============================================================================ */

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                  PiPinPP Utility Functions Demo                    ║\n";
    std::cout << "║                                                                    ║\n";
    std::cout << "║  Demonstrates Arduino-inspired utility functions:               ║\n";
    std::cout << "║    • Random number generation                                      ║\n";
    std::cout << "║    • Bit manipulation                                              ║\n";
    std::cout << "║    • Byte extraction                                               ║\n";
    std::cout << "║    • Practical applications                                        ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════╝\n";
    
    printSeparator();
    
    try {
        // Run all demonstrations
        demonstrateRandomNumbers();
        printSeparator();
        
        demonstrateBitManipulation();
        printSeparator();
        
        demonstrateByteExtraction();
        printSeparator();
        
        demonstratePracticalApplications();
        printSeparator();
        
        std::cout << "✓ All demonstrations completed successfully!\n\n";
        std::cout << "Key Takeaways:\n";
        std::cout << "  1. Use randomSeed(micros()) to initialize random generator\n";
        std::cout << "  2. Bit functions are inline (zero overhead)\n";
        std::cout << "  3. High/low byte extraction useful for data transmission\n";
        std::cout << "  4. All functions are Arduino-inspired\n\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
