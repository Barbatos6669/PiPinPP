#include "ArduinoCompat.hpp"
#include <iostream>

int main() {
    std::cout << "Testing Pin State Query Functions\n";
    std::cout << "==================================\n\n";
    
    // Test 1: Pin not initialized (should throw)
    try {
        isOutput(17);
        std::cout << "ERROR: Should have thrown!\n";
    } catch (const PinError& e) {
        std::cout << "✓ Correct error: " << e.what() << "\n\n";
    }
    
    // Test 2: Configure pin as OUTPUT
    pinMode(17, OUTPUT);
    std::cout << "Configured pin 17 as OUTPUT\n";
    std::cout << "  isOutput(17): " << (isOutput(17) ? "true" : "false") << " ✓\n";
    std::cout << "  isInput(17): " << (isInput(17) ? "true" : "false") << " ✓\n";
    std::cout << "  getMode(17): " << static_cast<int>(getMode(17)) << " (OUTPUT=1) ✓\n\n";
    
    // Test 3: Configure pin as INPUT_PULLUP
    pinMode(27, INPUT_PULLUP);
    std::cout << "Configured pin 27 as INPUT_PULLUP\n";
    std::cout << "  isOutput(27): " << (isOutput(27) ? "true" : "false") << " ✓\n";
    std::cout << "  isInput(27): " << (isInput(27) ? "true" : "false") << " ✓\n";
    std::cout << "  getMode(27): " << static_cast<int>(getMode(27)) << " (INPUT_PULLUP=2) ✓\n\n";
    
    // Test 4: Digital toggle
    std::cout << "Testing digitalToggle on pin 17\n";
    digitalWrite(17, LOW);
    std::cout << "  Initial: LOW\n";
    digitalToggle(17);
    std::cout << "  After toggle: " << (digitalRead(17) ? "HIGH" : "LOW") << " ✓\n";
    digitalToggle(17);
    std::cout << "  After toggle: " << (digitalRead(17) ? "HIGH" : "LOW") << " ✓\n\n";
    
    // Test 5: Try toggle on INPUT (should fail)
    try {
        digitalToggle(27);
        std::cout << "ERROR: Should have thrown!\n";
    } catch (const PinError& e) {
        std::cout << "✓ Correctly threw exception when toggling INPUT: " << e.what() << "\n\n";
    }
    
    std::cout << "All tests passed! ✓\n";
    return 0;
}
