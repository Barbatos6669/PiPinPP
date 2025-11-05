/*
 * Trigonometry Functions Example for PiPin++
 * 
 * This example demonstrates how to use trigonometric functions with Arduino-compatible
 * angle conversion constants (DEG_TO_RAD and RAD_TO_DEG).
 * 
 * Since sin(), cos(), and tan() exist in the C++ standard library (<cmath>), PiPin++
 * doesn't wrap them. Instead, use std::sin(), std::cos(), std::tan() with the provided
 * conversion constants for Arduino-style angle handling.
 * 
 * Hardware: No GPIO hardware required (demonstration only)
 * 
 * Compilation:
 *   g++ main.cpp $(pkg-config --cflags --libs pipinpp) -o trig_functions
 * 
 * Usage:
 *   ./trig_functions
 */

#include <ArduinoCompat.hpp>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

using namespace std;

/* ===========================================================================
 *                       BASIC ANGLE CONVERSIONS
 * ===========================================================================*/

/**
 * Demonstrate basic degree <-> radian conversions
 */
void demonstrateAngleConversions() {
    cout << "\n========================================" << endl;
    cout << "  ANGLE CONVERSIONS" << endl;
    cout << "========================================\n" << endl;
    
    // Common angles in degrees
    vector<double> angles = {0, 30, 45, 60, 90, 120, 135, 150, 180, 270, 360};
    
    cout << "Degrees -> Radians Conversion:" << endl;
    cout << fixed << setprecision(6);
    
    for (double deg : angles) {
        double rad = deg * DEG_TO_RAD;
        cout << "  " << setw(6) << deg << "° = " << setw(10) << rad << " rad" << endl;
    }
    
    cout << "\nRadians -> Degrees Conversion:" << endl;
    
    vector<double> radians = {0, M_PI/6, M_PI/4, M_PI/3, M_PI/2, M_PI, 2*M_PI};
    
    for (double rad : radians) {
        double deg = rad * RAD_TO_DEG;
        cout << "  " << setw(10) << rad << " rad = " << setw(6) << deg << "°" << endl;
    }
}

/* ===========================================================================
 *                    TRIGONOMETRIC CALCULATIONS
 * ===========================================================================*/

/**
 * Demonstrate trigonometric functions with degree inputs
 */
void demonstrateTrigFunctions() {
    cout << "\n========================================" << endl;
    cout << "  TRIGONOMETRIC FUNCTIONS" << endl;
    cout << "========================================\n" << endl;
    
    cout << "Common Angles (sin, cos, tan):" << endl;
    cout << fixed << setprecision(4);
    
    vector<double> angles = {0, 30, 45, 60, 90, 120, 135, 150, 180};
    
    cout << "┌─────────┬──────────┬──────────┬──────────┐" << endl;
    cout << "│ Degrees │   sin    │   cos    │   tan    │" << endl;
    cout << "├─────────┼──────────┼──────────┼──────────┤" << endl;
    
    for (double deg : angles) {
        double rad = deg * DEG_TO_RAD;
        double sinVal = std::sin(rad);
        double cosVal = std::cos(rad);
        double tanVal = std::tan(rad);
        
        cout << "│ " << setw(6) << deg << "° │ ";
        cout << setw(8) << sinVal << " │ ";
        cout << setw(8) << cosVal << " │ ";
        
        // Handle tan(90°) which is undefined
        if (deg == 90.0) {
            cout << "  ∞/undef │" << endl;
        } else {
            cout << setw(8) << tanVal << " │" << endl;
        }
    }
    
    cout << "└─────────┴──────────┴──────────┴──────────┘" << endl;
}

/* ===========================================================================
 *                    PRACTICAL APPLICATION: CIRCULAR MOTION
 * ===========================================================================*/

/**
 * Calculate position on a circle given angle and radius
 */
struct Point2D {
    double x;
    double y;
};

Point2D calculateCircularPosition(double angleDegrees, double radius) {
    double angleRadians = angleDegrees * DEG_TO_RAD;
    
    return {
        radius * std::cos(angleRadians),
        radius * std::sin(angleRadians)
    };
}

/**
 * Demonstrate circular motion calculations
 */
void demonstrateCircularMotion() {
    cout << "\n========================================" << endl;
    cout << "  CIRCULAR MOTION (radius = 10.0)" << endl;
    cout << "========================================\n" << endl;
    
    cout << fixed << setprecision(2);
    double radius = 10.0;
    
    cout << "Position on circle at different angles:" << endl;
    cout << "┌─────────┬──────────┬──────────┐" << endl;
    cout << "│ Angle   │    X     │    Y     │" << endl;
    cout << "├─────────┼──────────┼──────────┤" << endl;
    
    for (int angle = 0; angle <= 360; angle += 45) {
        Point2D pos = calculateCircularPosition(angle, radius);
        cout << "│ " << setw(4) << angle << "°  │ ";
        cout << setw(8) << pos.x << " │ ";
        cout << setw(8) << pos.y << " │" << endl;
    }
    
    cout << "└─────────┴──────────┴──────────┘" << endl;
}

/* ===========================================================================
 *                    PRACTICAL APPLICATION: RIGHT TRIANGLES
 * ===========================================================================*/

/**
 * Calculate sides of a right triangle given angle and hypotenuse
 */
struct Triangle {
    double opposite;   // Side opposite to angle
    double adjacent;   // Side adjacent to angle
    double hypotenuse; // Hypotenuse
};

Triangle calculateTriangle(double angleDegrees, double hypotenuse) {
    double angleRadians = angleDegrees * DEG_TO_RAD;
    
    return {
        hypotenuse * std::sin(angleRadians),  // opposite = h * sin(θ)
        hypotenuse * std::cos(angleRadians),  // adjacent = h * cos(θ)
        hypotenuse
    };
}

/**
 * Demonstrate right triangle calculations
 */
void demonstrateTriangleCalculations() {
    cout << "\n========================================" << endl;
    cout << "  RIGHT TRIANGLE (hypotenuse = 10.0)" << endl;
    cout << "========================================\n" << endl;
    
    cout << fixed << setprecision(2);
    double hypotenuse = 10.0;
    
    cout << "Calculate opposite and adjacent sides:" << endl;
    cout << "┌─────────┬──────────┬──────────┐" << endl;
    cout << "│ Angle   │ Opposite │ Adjacent │" << endl;
    cout << "├─────────┼──────────┼──────────┤" << endl;
    
    for (int angle = 0; angle <= 90; angle += 15) {
        Triangle tri = calculateTriangle(angle, hypotenuse);
        cout << "│ " << setw(4) << angle << "°  │ ";
        cout << setw(8) << tri.opposite << " │ ";
        cout << setw(8) << tri.adjacent << " │" << endl;
    }
    
    cout << "└─────────┴──────────┴──────────┘" << endl;
}

/* ===========================================================================
 *                    PRACTICAL APPLICATION: WAVE GENERATION
 * ===========================================================================*/

/**
 * Generate sine wave values for LED breathing effect or audio generation
 */
void demonstrateWaveGeneration() {
    cout << "\n========================================" << endl;
    cout << "  SINE WAVE GENERATION (0-255 range)" << endl;
    cout << "========================================\n" << endl;
    
    cout << "Useful for LED breathing effects or PWM patterns:" << endl;
    cout << "Value = 127.5 + 127.5 * sin(angle)\n" << endl;
    
    cout << fixed << setprecision(0);
    
    cout << "┌─────────┬─────────┬────────────────────────────┐" << endl;
    cout << "│ Degrees │  Value  │          Visual            │" << endl;
    cout << "├─────────┼─────────┼────────────────────────────┤" << endl;
    
    for (int angle = 0; angle <= 360; angle += 30) {
        double angleRadians = angle * DEG_TO_RAD;
        double sineValue = std::sin(angleRadians);
        
        // Map from [-1, 1] to [0, 255] for PWM
        int pwmValue = static_cast<int>(127.5 + 127.5 * sineValue);
        
        // Create visual bar
        int barLength = pwmValue / 8;  // Scale to fit width
        string bar(barLength, '#');
        
        cout << "│ " << setw(4) << angle << "°  │ ";
        cout << setw(5) << pwmValue << "  │ ";
        cout << bar << endl;
    }
    
    cout << "└─────────┴─────────┴────────────────────────────┘" << endl;
    
    cout << "\nExample code for LED breathing:" << endl;
    cout << "  for (int angle = 0; angle < 360; angle++) {" << endl;
    cout << "      double rad = angle * DEG_TO_RAD;" << endl;
    cout << "      int brightness = 127.5 + 127.5 * std::sin(rad);" << endl;
    cout << "      analogWrite(ledPin, brightness);" << endl;
    cout << "      delay(10);" << endl;
    cout << "  }" << endl;
}

/* ===========================================================================
 *                    PRACTICAL APPLICATION: SERVO ANGLES
 * ===========================================================================*/

/**
 * Convert servo angle to pulse width (typical servo: 1000-2000µs for 0-180°)
 */
int angleToPulseWidth(double angleDegrees) {
    // Typical servo: 1000µs = 0°, 2000µs = 180°
    // Linear mapping: pulseWidth = 1000 + (angle / 180) * 1000
    return static_cast<int>(1000 + (angleDegrees / 180.0) * 1000);
}

/**
 * Demonstrate servo angle calculations
 */
void demonstrateServoAngles() {
    cout << "\n========================================" << endl;
    cout << "  SERVO ANGLE TO PULSE WIDTH" << endl;
    cout << "========================================\n" << endl;
    
    cout << "Standard servo mapping (0-180° to 1000-2000µs):" << endl;
    cout << "┌─────────┬─────────────┬─────────────────┐" << endl;
    cout << "│  Angle  │ Pulse Width │   Description   │" << endl;
    cout << "├─────────┼─────────────┼─────────────────┤" << endl;
    
    struct ServoPosition {
        int angle;
        const char* description;
    };
    
    vector<ServoPosition> positions = {
        {0, "Full left"},
        {45, "45° left"},
        {90, "Center"},
        {135, "45° right"},
        {180, "Full right"}
    };
    
    for (const auto& pos : positions) {
        int pulseWidth = angleToPulseWidth(pos.angle);
        cout << "│ " << setw(4) << pos.angle << "°  │ ";
        cout << setw(8) << pulseWidth << "µs │ ";
        cout << pos.description << endl;
    }
    
    cout << "└─────────┴─────────────┴─────────────────┘" << endl;
    
    cout << "\nNote: Use hardware PWM (50Hz) for precise servo control." << endl;
    cout << "Software PWM may have jitter affecting servo positioning." << endl;
}

/* ===========================================================================
 *                    PRACTICAL APPLICATION: ANGLE CALCULATIONS
 * ===========================================================================*/

/**
 * Calculate angle from X and Y coordinates using atan2
 */
double calculateAngle(double x, double y) {
    double angleRadians = std::atan2(y, x);
    double angleDegrees = angleRadians * RAD_TO_DEG;
    
    // Normalize to 0-360° range
    if (angleDegrees < 0) {
        angleDegrees += 360;
    }
    
    return angleDegrees;
}

/**
 * Demonstrate angle calculations from coordinates
 */
void demonstrateAngleCalculations() {
    cout << "\n========================================" << endl;
    cout << "  CALCULATE ANGLE FROM COORDINATES" << endl;
    cout << "========================================\n" << endl;
    
    cout << "Using atan2(y, x) to find angle:" << endl;
    cout << fixed << setprecision(2);
    
    struct Coordinate {
        double x, y;
        const char* quadrant;
    };
    
    vector<Coordinate> coords = {
        {10.0, 0.0, "East (0°)"},
        {10.0, 10.0, "NE (45°)"},
        {0.0, 10.0, "North (90°)"},
        {-10.0, 10.0, "NW (135°)"},
        {-10.0, 0.0, "West (180°)"},
        {-10.0, -10.0, "SW (225°)"},
        {0.0, -10.0, "South (270°)"},
        {10.0, -10.0, "SE (315°)"}
    };
    
    cout << "┌──────────┬──────────┬─────────┬─────────────┐" << endl;
    cout << "│    X     │    Y     │  Angle  │  Direction  │" << endl;
    cout << "├──────────┼──────────┼─────────┼─────────────┤" << endl;
    
    for (const auto& coord : coords) {
        double angle = calculateAngle(coord.x, coord.y);
        cout << "│ " << setw(8) << coord.x << " │ ";
        cout << setw(8) << coord.y << " │ ";
        cout << setw(6) << angle << "° │ ";
        cout << coord.quadrant << endl;
    }
    
    cout << "└──────────┴──────────┴─────────┴─────────────┘" << endl;
}

/* ===========================================================================
 *                              MAIN PROGRAM
 * ===========================================================================*/

int main() {
    cout << "\n╔════════════════════════════════════════════════╗" << endl;
    cout << "║   PiPin++ Trigonometry Functions Example      ║" << endl;
    cout << "║   Demonstrating DEG_TO_RAD and RAD_TO_DEG     ║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
    
    // Run all demonstrations
    demonstrateAngleConversions();
    demonstrateTrigFunctions();
    demonstrateCircularMotion();
    demonstrateTriangleCalculations();
    demonstrateWaveGeneration();
    demonstrateServoAngles();
    demonstrateAngleCalculations();
    
    cout << "\n========================================" << endl;
    cout << "  SUMMARY" << endl;
    cout << "========================================\n" << endl;
    cout << "Arduino Compatibility:" << endl;
    cout << "  • Use DEG_TO_RAD and RAD_TO_DEG constants" << endl;
    cout << "  • Use std::sin(), std::cos(), std::tan() from <cmath>" << endl;
    cout << "  • Use std::atan2() for angle calculations" << endl;
    cout << "\nNo GPIO hardware needed for this example." << endl;
    cout << "All calculations are performed in software.\n" << endl;
    
    return 0;
}
