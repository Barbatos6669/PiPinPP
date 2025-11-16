/**
 * @file main.cpp
 * @brief Hardware PWM Servo Control Example
 * @author Barbatos6669
 * @date 2025-11-16
 * 
 * Demonstrates precise servo control using hardware PWM on Raspberry Pi.
 * Hardware PWM provides jitter-free signals essential for smooth servo operation.
 * 
 * Hardware Setup:
 * - Servo signal wire → GPIO18 (Pin 12, PWM0)
 * - Servo VCC → 5V (Pin 2 or 4)
 * - Servo GND → GND (Pin 6, 9, 14, 20, 25, 30, 34, or 39)
 * 
 * Servo Timing:
 * - Standard servos: 50Hz (20ms period)
 * - Pulse width: 1ms (0°) to 2ms (180°)
 * - Center position: 1.5ms (90°)
 * 
 * @note Requires hardware PWM pin (GPIO12, 13, 18, or 19)
 * @note Software PWM has too much jitter for servo control
 */

#include "HardwarePWM.hpp"
#include "ArduinoCompat.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace pipinpp;
using namespace std;

// Servo control constants
constexpr uint32_t SERVO_FREQUENCY = 50;  // 50Hz = 20ms period
constexpr double MIN_PULSE_MS = 1.0;      // 1ms = 0°
constexpr double MAX_PULSE_MS = 2.0;      // 2ms = 180°
constexpr double CENTER_PULSE_MS = 1.5;   // 1.5ms = 90°

/**
 * @brief Convert servo angle to pulse width in milliseconds
 * @param angleDegrees Servo angle (0-180°)
 * @return Pulse width in milliseconds
 */
double angleToPulseWidth(double angleDegrees)
{
    // Clamp angle to valid range
    if (angleDegrees < 0.0) angleDegrees = 0.0;
    if (angleDegrees > 180.0) angleDegrees = 180.0;
    
    // Linear mapping: 0° → 1ms, 180° → 2ms
    return MIN_PULSE_MS + (angleDegrees / 180.0) * (MAX_PULSE_MS - MIN_PULSE_MS);
}

/**
 * @brief Convert pulse width to duty cycle percentage for 50Hz PWM
 * @param pulseWidthMs Pulse width in milliseconds
 * @return Duty cycle percentage (0-100)
 */
double pulseWidthToDutyCycle(double pulseWidthMs)
{
    // Period = 20ms at 50Hz
    constexpr double PERIOD_MS = 1000.0 / SERVO_FREQUENCY;
    return (pulseWidthMs / PERIOD_MS) * 100.0;
}

/**
 * @brief Move servo to specific angle
 * @param servo Hardware PWM object
 * @param angleDegrees Target angle (0-180°)
 */
void moveServoToAngle(HardwarePWM& servo, double angleDegrees)
{
    double pulseMs = angleToPulseWidth(angleDegrees);
    double dutyCycle = pulseWidthToDutyCycle(pulseMs);
    servo.setDutyCycle(dutyCycle);
}

/**
 * @brief Smooth servo sweep from start to end angle
 * @param servo Hardware PWM object
 * @param startAngle Starting angle
 * @param endAngle Ending angle
 * @param stepSize Angle increment per step
 * @param delayMs Delay between steps
 */
void sweepServo(HardwarePWM& servo, double startAngle, double endAngle, double stepSize, int delayMs)
{
    if (startAngle < endAngle) {
        for (double angle = startAngle; angle <= endAngle; angle += stepSize) {
            moveServoToAngle(servo, angle);
            delay(delayMs);
        }
    } else {
        for (double angle = startAngle; angle >= endAngle; angle -= stepSize) {
            moveServoToAngle(servo, angle);
            delay(delayMs);
        }
    }
}

int main()
{
    cout << "=== PiPinPP Hardware PWM Servo Control Example ===\n\n";
    
    // Check if GPIO18 supports hardware PWM
    int chip, channel;
    if (!HardwarePWM::gpioToPWM(18, chip, channel)) {
        cerr << "ERROR: GPIO18 doesn't support hardware PWM\n";
        return 1;
    }
    
    cout << "Hardware Setup:\n";
    cout << "  Servo Signal → GPIO18 (Pin 12)\n";
    cout << "  Servo VCC    → 5V (Pin 2 or 4)\n";
    cout << "  Servo GND    → GND (Pin 6, 9, 14, etc.)\n\n";
    
    cout << "Initializing hardware PWM on GPIO18 (chip=" << chip << ", channel=" << channel << ")...\n";
    
    HardwarePWM servo(chip, channel);
    
    // Initialize servo at center position (90°)
    double centerDutyCycle = pulseWidthToDutyCycle(CENTER_PULSE_MS);
    if (!servo.begin(SERVO_FREQUENCY, centerDutyCycle)) {
        cerr << "ERROR: Failed to initialize hardware PWM\n";
        cerr << "Make sure you have permissions to access /sys/class/pwm/\n";
        return 1;
    }
    
    cout << "✓ Hardware PWM initialized at " << SERVO_FREQUENCY << "Hz\n";
    cout << "✓ Servo at center position (90°)\n\n";
    
    delay(1000);
    
    // Test 1: Move to predefined positions
    cout << "--- Test 1: Predefined Positions ---\n";
    
    struct Position {
        double angle;
        const char* description;
    };
    
    Position positions[] = {
        {0, "Minimum (0°)"},
        {45, "Quarter (45°)"},
        {90, "Center (90°)"},
        {135, "Three-quarter (135°)"},
        {180, "Maximum (180°)"},
        {90, "Back to center (90°)"}
    };
    
    for (const auto& pos : positions) {
        cout << "Moving to " << pos.description << "...\n";
        moveServoToAngle(servo, pos.angle);
        
        double pulseMs = angleToPulseWidth(pos.angle);
        double dutyCycle = pulseWidthToDutyCycle(pulseMs);
        
        cout << "  Angle: " << fixed << setprecision(1) << pos.angle << "°\n";
        cout << "  Pulse width: " << setprecision(2) << pulseMs << "ms\n";
        cout << "  Duty cycle: " << setprecision(2) << dutyCycle << "%\n\n";
        
        delay(2000);
    }
    
    // Test 2: Smooth sweep
    cout << "--- Test 2: Smooth Sweep ---\n";
    cout << "Sweeping from 0° to 180° and back...\n\n";
    
    for (int i = 0; i < 2; i++) {
        cout << "Sweep right (0° → 180°)...\n";
        sweepServo(servo, 0, 180, 2.0, 20);
        
        delay(500);
        
        cout << "Sweep left (180° → 0°)...\n";
        sweepServo(servo, 180, 0, 2.0, 20);
        
        delay(500);
    }
    
    // Test 3: Precise positioning
    cout << "\n--- Test 3: Precise Positioning ---\n";
    cout << "Testing micro-adjustments (± 1°)...\n\n";
    
    double testAngles[] = {89.0, 90.0, 91.0, 90.0};
    for (double angle : testAngles) {
        moveServoToAngle(servo, angle);
        cout << "Position: " << fixed << setprecision(1) << angle << "°\n";
        delay(1000);
    }
    
    // Test 4: Speed control
    cout << "\n--- Test 4: Speed Control ---\n";
    
    cout << "Slow movement (1° steps, 50ms delay)...\n";
    sweepServo(servo, 90, 120, 1.0, 50);
    delay(500);
    sweepServo(servo, 120, 90, 1.0, 50);
    delay(500);
    
    cout << "Fast movement (5° steps, 10ms delay)...\n";
    sweepServo(servo, 90, 30, 5.0, 10);
    delay(500);
    sweepServo(servo, 30, 90, 5.0, 10);
    
    // Return to center and finish
    cout << "\n--- Finishing ---\n";
    cout << "Returning to center position (90°)...\n";
    moveServoToAngle(servo, 90);
    delay(1000);
    
    // Get final stats
    cout << "\nFinal PWM Status:\n";
    cout << "  Frequency: " << servo.getFrequency() << " Hz\n";
    cout << "  Duty Cycle: " << fixed << setprecision(2) << servo.getDutyCycle() << "%\n";
    cout << "  Period: " << servo.getPeriodNs() << " ns\n";
    cout << "  Pulse Width: " << servo.getDutyCycleNs() << " ns\n";
    
    servo.end();
    cout << "\n✓ Hardware PWM stopped\n";
    cout << "\n=== Servo Control Example Complete ===\n";
    
    cout << "\nNotes:\n";
    cout << "  • Hardware PWM provides jitter-free signals\n";
    cout << "  • Software PWM has too much jitter for servos\n";
    cout << "  • Use GPIO12, 13, 18, or 19 for hardware PWM\n";
    cout << "  • Servo VCC should be 5V from external supply for high-torque servos\n";
    
    return 0;
}
