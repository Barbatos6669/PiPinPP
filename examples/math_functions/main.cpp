/*
 * PiPinPP - Extended Math Functions Example
 * 
 * Demonstrates the Arduino-inspired math functions:
 * - sq()        - Square a number (PiPinPP function)
 * - std::sqrt() - Square root (from <cmath>)
 * - std::pow()  - Power function (from <cmath>)
 * - std::max()  - Maximum of two values (from <algorithm>)
 * - std::min()  - Minimum of two values (from <algorithm>)
 * 
 * Note: Only sq() is provided by PiPinPP. Other math functions
 * should use the standard library to avoid naming conflicts.
 */

#include "ArduinoCompat.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

void printSeparator() {
    std::cout << "\n" << std::string(60, '-') << "\n" << std::endl;
}

void testSquareFunction() {
    std::cout << "=== sq() - Square Function ===" << std::endl;
    std::cout << "Calculates x² efficiently (faster than pow(x, 2))" << std::endl;
    
    int a = sq(5);
    std::cout << "  sq(5) = " << a << std::endl;
    
    float b = sq(3.5f);
    std::cout << "  sq(3.5) = " << b << std::endl;
    
    double c = sq(-4.0);
    std::cout << "  sq(-4.0) = " << c << std::endl;
    
    long d = sq(100L);
    std::cout << "  sq(100) = " << d << std::endl;
}

void testSquareRoot() {
    std::cout << "\n=== std::sqrt() - Square Root Function ===" << std::endl;
    std::cout << "Returns the square root (√x) (from <cmath>)" << std::endl;
    
    double a = std::sqrt(16.0);
    std::cout << "  std::sqrt(16.0) = " << a << std::endl;
    
    double b = std::sqrt(2.0);
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "  std::sqrt(2.0) = " << b << std::endl;
    
    double c = std::sqrt(0.25);
    std::cout << "  std::sqrt(0.25) = " << c << std::endl;
    
    double d = std::sqrt(100.0);
    std::cout << "  std::sqrt(100.0) = " << d << std::endl;
}

void testPowerFunction() {
    std::cout << "\n=== std::pow() - Power Function ===" << std::endl;
    std::cout << "Calculates base^exponent (from <cmath>)" << std::endl;
    
    double a = std::pow(2.0, 3.0);
    std::cout << "  std::pow(2.0, 3.0) = " << a << " (2³)" << std::endl;
    
    double b = std::pow(10.0, -2.0);
    std::cout << "  std::pow(10.0, -2.0) = " << b << " (10⁻²)" << std::endl;
    
    double c = std::pow(4.0, 0.5);
    std::cout << "  std::pow(4.0, 0.5) = " << c << " (√4)" << std::endl;
    
    double d = std::pow(5.0, 0.0);
    std::cout << "  std::pow(5.0, 0.0) = " << d << " (anything⁰ = 1)" << std::endl;
}

void testMaxFunction() {
    std::cout << "\n=== std::max() - Maximum Function ===" << std::endl;
    std::cout << "Returns the larger of two values (from <algorithm>)" << std::endl;
    
    int a = std::max(10, 20);
    std::cout << "  std::max(10, 20) = " << a << std::endl;
    
    float b = std::max(3.5f, 2.1f);
    std::cout << "  std::max(3.5, 2.1) = " << b << std::endl;
    
    long c = std::max(-5L, -10L);
    std::cout << "  std::max(-5, -10) = " << c << std::endl;
    
    double d = std::max(0.0, -0.0);
    std::cout << "  std::max(0.0, -0.0) = " << d << std::endl;
}

void testMinFunction() {
    std::cout << "\n=== std::min() - Minimum Function ===" << std::endl;
    std::cout << "Returns the smaller of two values (from <algorithm>)" << std::endl;
    
    int a = std::min(10, 20);
    std::cout << "  std::min(10, 20) = " << a << std::endl;
    
    float b = std::min(3.5f, 2.1f);
    std::cout << "  std::min(3.5, 2.1) = " << b << std::endl;
    
    long c = std::min(-5L, -10L);
    std::cout << "  std::min(-5, -10) = " << c << std::endl;
    
    double d = std::min(0.0, -0.0);
    std::cout << "  std::min(0.0, -0.0) = " << d << std::endl;
}

void testPracticalExample() {
    std::cout << "\n=== Practical Example: Distance Calculation ===" << std::endl;
    std::cout << "Calculate distance between two points using math functions" << std::endl;
    
    // Point 1: (3, 4)
    // Point 2: (6, 8)
    double x1 = 3.0, y1 = 4.0;
    double x2 = 6.0, y2 = 8.0;
    
    // Distance formula: d = √[(x₂-x₁)² + (y₂-y₁)²]
    double dx = x2 - x1;
    double dy = y2 - y1;
    
    std::cout << "  Point 1: (" << x1 << ", " << y1 << ")" << std::endl;
    std::cout << "  Point 2: (" << x2 << ", " << y2 << ")" << std::endl;
    std::cout << "  dx = " << dx << ", dy = " << dy << std::endl;
    
    double distance = sqrt(sq(dx) + sq(dy));
    std::cout << "  Distance = sqrt(sq(" << dx << ") + sq(" << dy << "))" << std::endl;
    std::cout << "  Distance = " << distance << " units" << std::endl;
}

void testSensorMapping() {
    std::cout << "\n=== Practical Example: Sensor Value Constraining ===" << std::endl;
    std::cout << "Use min/max to constrain sensor readings to valid range" << std::endl;
    
    int rawSensor1 = 1500;  // Too high
    int rawSensor2 = -50;   // Too low
    int rawSensor3 = 512;   // Just right
    
    int minVal = 0;
    int maxVal = 1023;
    
    std::cout << "  Valid range: " << minVal << " - " << maxVal << std::endl;
    
    // Constrain using std::max(min, std::min(x, max))
    int constrained1 = std::min(std::max(rawSensor1, minVal), maxVal);
    int constrained2 = std::min(std::max(rawSensor2, minVal), maxVal);
    int constrained3 = std::min(std::max(rawSensor3, minVal), maxVal);
    
    std::cout << "  Raw: " << rawSensor1 << " → Constrained: " << constrained1 << std::endl;
    std::cout << "  Raw: " << rawSensor2 << " → Constrained: " << constrained2 << std::endl;
    std::cout << "  Raw: " << rawSensor3 << " → Constrained: " << constrained3 << std::endl;
}

void testPWMCalculation() {
    std::cout << "\n=== Practical Example: PWM Brightness Curve ===" << std::endl;
    std::cout << "Use sq() for exponential LED brightness (looks more natural)" << std::endl;
    
    std::cout << "\n  Linear brightness (looks dim at low values):" << std::endl;
    for (int percent = 0; percent <= 100; percent += 20) {
        int linearPWM = (percent * 255) / 100;
        std::cout << "    " << std::setw(3) << percent << "% → PWM " 
                  << std::setw(3) << linearPWM << std::endl;
    }
    
    std::cout << "\n  Exponential brightness (more natural perception):" << std::endl;
    for (int percent = 0; percent <= 100; percent += 20) {
        // Use square function for exponential curve
        double normalized = percent / 100.0;
        int expPWM = (int)(sq(normalized) * 255.0);
        std::cout << "    " << std::setw(3) << percent << "% → PWM " 
                  << std::setw(3) << expPWM << std::endl;
    }
}

int main() {
    std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║        PiPinPP Extended Math Functions Example           ║" << std::endl;
    std::cout << "║                    Version 0.3.2                          ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
    
    printSeparator();
    testSquareFunction();
    
    printSeparator();
    testSquareRoot();
    
    printSeparator();
    testPowerFunction();
    
    printSeparator();
    testMaxFunction();
    
    printSeparator();
    testMinFunction();
    
    printSeparator();
    testPracticalExample();
    
    printSeparator();
    testSensorMapping();
    
    printSeparator();
    testPWMCalculation();
    
    printSeparator();
    std::cout << "\n✓ All math functions tested successfully!" << std::endl;
    std::cout << "\nNote: PiPinPP provides sq() for Arduino-inspired API." << std::endl;
    std::cout << "Use std::sqrt(), std::pow(), std::max(), std::min() from" << std::endl;
    std::cout << "the standard library (<cmath> and <algorithm>)." << std::endl;
    std::cout << "\nFor more information, see docs/API_REFERENCE.md" << std::endl;
    
    return 0;
}
