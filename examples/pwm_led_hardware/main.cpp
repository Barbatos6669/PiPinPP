/**
 * @file main.cpp
 * @brief Hardware PWM LED Fade Example
 * @author Barbatos6669
 * @date 2025-11-16
 * 
 * Demonstrates smooth LED fading using hardware PWM on Raspberry Pi.
 * Hardware PWM provides flicker-free, jitter-free LED dimming compared
 * to software PWM.
 * 
 * Hardware Setup:
 * - LED anode (+) → 220Ω resistor → GPIO18 (Pin 12, PWM0)
 * - LED cathode (-) → GND (Pin 6, 9, 14, 20, 25, 30, 34, or 39)
 * 
 * @note Requires hardware PWM pin (GPIO12, 13, 18, or 19)
 * @note Hardware PWM is flicker-free even at high speeds
 */

#include "HardwarePWM.hpp"
#include "ArduinoCompat.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace pipinpp;
using namespace std;

/**
 * @brief Fade LED from one brightness to another
 * @param pwm Hardware PWM object
 * @param startBrightness Starting brightness (0-255)
 * @param endBrightness Ending brightness (0-255)
 * @param steps Number of steps in fade
 * @param delayMs Delay between steps in milliseconds
 */
void fadeLED(HardwarePWM& pwm, int startBrightness, int endBrightness, int steps, int delayMs)
{
    for (int i = 0; i <= steps; i++) {
        int brightness = startBrightness + (endBrightness - startBrightness) * i / steps;
        pwm.setDutyCycle8Bit(brightness);
        delay(delayMs);
    }
}

/**
 * @brief Smooth sine wave breathing effect
 * @param pwm Hardware PWM object
 * @param duration Duration in milliseconds
 * @param cycles Number of breathing cycles
 */
void breathingEffect(HardwarePWM& pwm, int duration, int cycles)
{
    int steps = 100;
    int delayMs = duration / steps;
    
    for (int cycle = 0; cycle < cycles; cycle++) {
        for (int i = 0; i <= steps; i++) {
            double angle = (i * M_PI) / steps;  // 0 to π
            int brightness = static_cast<int>(127.5 + 127.5 * sin(angle));
            pwm.setDutyCycle8Bit(brightness);
            delay(delayMs);
        }
    }
}

/**
 * @brief Pulse effect (quick flash)
 * @param pwm Hardware PWM object
 * @param pulseCount Number of pulses
 * @param pulseDuration Pulse duration in milliseconds
 */
void pulseEffect(HardwarePWM& pwm, int pulseCount, int pulseDuration)
{
    for (int i = 0; i < pulseCount; i++) {
        pwm.setDutyCycle8Bit(255);
        delay(pulseDuration);
        pwm.setDutyCycle8Bit(0);
        delay(pulseDuration);
    }
}

int main()
{
    cout << "=== PiPinPP Hardware PWM LED Fade Example ===\n\n";
    
    // Check GPIO18 for hardware PWM
    int chip, channel;
    if (!HardwarePWM::gpioToPWM(18, chip, channel)) {
        cerr << "ERROR: GPIO18 doesn't support hardware PWM\n";
        return 1;
    }
    
    cout << "Hardware Setup:\n";
    cout << "  LED+ → 220Ω resistor → GPIO18 (Pin 12)\n";
    cout << "  LED- → GND (Pin 6, 9, 14, etc.)\n\n";
    
    cout << "Initializing hardware PWM on GPIO18...\n";
    
    HardwarePWM led(chip, channel);
    
    // Initialize at 1kHz (imperceptible to human eye)
    if (!led.begin(1000, 0.0)) {
        cerr << "ERROR: Failed to initialize hardware PWM\n";
        cerr << "Make sure you have permissions to access /sys/class/pwm/\n";
        return 1;
    }
    
    cout << "✓ Hardware PWM initialized at 1kHz\n";
    cout << "✓ LED ready\n\n";
    
    // Test 1: Basic fade in/out
    cout << "--- Test 1: Basic Fade In/Out ---\n";
    cout << "Fading in (0% → 100%)...\n";
    fadeLED(led, 0, 255, 50, 20);
    
    cout << "Holding at full brightness...\n";
    delay(1000);
    
    cout << "Fading out (100% → 0%)...\n";
    fadeLED(led, 255, 0, 50, 20);
    delay(500);
    
    // Test 2: Different fade speeds
    cout << "\n--- Test 2: Fade Speed Comparison ---\n";
    
    cout << "Slow fade (3 seconds)...\n";
    fadeLED(led, 0, 255, 100, 30);
    fadeLED(led, 255, 0, 100, 30);
    delay(500);
    
    cout << "Fast fade (0.5 seconds)...\n";
    fadeLED(led, 0, 255, 25, 20);
    fadeLED(led, 255, 0, 25, 20);
    delay(500);
    
    cout << "Ultra-fast fade (0.1 seconds)...\n";
    fadeLED(led, 0, 255, 10, 10);
    fadeLED(led, 255, 0, 10, 10);
    delay(1000);
    
    // Test 3: Breathing effect
    cout << "\n--- Test 3: Breathing Effect (Sine Wave) ---\n";
    cout << "Smooth breathing for 10 seconds...\n";
    breathingEffect(led, 2500, 4);  // 4 cycles of 2.5s each
    
    led.setDutyCycle8Bit(0);
    delay(1000);
    
    // Test 4: Brightness levels
    cout << "\n--- Test 4: Discrete Brightness Levels ---\n";
    int brightnessLevels[] = {0, 32, 64, 96, 128, 160, 192, 224, 255};
    
    for (int brightness : brightnessLevels) {
        led.setDutyCycle8Bit(brightness);
        double percent = (brightness / 255.0) * 100.0;
        cout << "Brightness: " << brightness << "/255 (" << fixed << setprecision(1) << percent << "%)\n";
        delay(800);
    }
    
    led.setDutyCycle8Bit(0);
    delay(500);
    
    // Test 5: Pulse effect
    cout << "\n--- Test 5: Pulse Effect ---\n";
    cout << "Quick pulses...\n";
    pulseEffect(led, 5, 100);
    delay(1000);
    
    // Test 6: PWM frequency comparison
    cout << "\n--- Test 6: PWM Frequency Comparison ---\n";
    cout << "(All should be flicker-free with hardware PWM)\n\n";
    
    struct FreqTest {
        uint32_t frequency;
        const char* description;
    };
    
    FreqTest freqTests[] = {
        {100, "100Hz - Might be visible to some"},
        {500, "500Hz - Flicker-free"},
        {1000, "1kHz - Standard"},
        {5000, "5kHz - High frequency"},
        {10000, "10kHz - Very high frequency"}
    };
    
    for (const auto& test : freqTests) {
        cout << test.description << "...\n";
        led.end();
        led.begin(test.frequency, 0.0);
        
        // Fade at this frequency
        fadeLED(led, 0, 255, 20, 25);
        fadeLED(led, 255, 0, 20, 25);
        delay(500);
    }
    
    // Test 7: Demonstration of no flicker
    cout << "\n--- Test 7: Flicker Test (50% brightness) ---\n";
    cout << "Holding at 50% brightness for 5 seconds...\n";
    cout << "(Hardware PWM should show zero flicker)\n";
    
    led.end();
    led.begin(1000, 0.0);
    led.setDutyCycle8Bit(128);  // 50%
    delay(5000);
    
    // Final demonstration
    cout << "\n--- Final Demonstration ---\n";
    cout << "Smooth crossfade sequence...\n";
    
    for (int i = 0; i < 3; i++) {
        fadeLED(led, 0, 255, 50, 10);
        fadeLED(led, 255, 0, 50, 10);
    }
    
    // Get final stats
    cout << "\nFinal PWM Status:\n";
    cout << "  Frequency: " << led.getFrequency() << " Hz\n";
    cout << "  Duty Cycle: " << fixed << setprecision(2) << led.getDutyCycle() << "%\n";
    cout << "  Period: " << led.getPeriodNs() << " ns\n";
    cout << "  Pulse Width: " << led.getDutyCycleNs() << " ns\n";
    
    led.end();
    cout << "\n✓ Hardware PWM stopped\n";
    cout << "\n=== LED Fade Example Complete ===\n";
    
    cout << "\nHardware PWM Advantages:\n";
    cout << "  • Zero flicker at all brightness levels\n";
    cout << "  • No timing jitter\n";
    cout << "  • No CPU overhead\n";
    cout << "  • Precise frequency control\n";
    cout << "  • Suitable for high-speed switching\n";
    
    return 0;
}
