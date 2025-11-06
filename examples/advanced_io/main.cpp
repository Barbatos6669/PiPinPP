/*
 * Copyright (c) 2025 HobbyHacker
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file advanced_io/main.cpp
 * @brief Demonstration of Advanced I/O functions: pulseIn, shiftOut/In, tone
 * 
 * This example demonstrates PiPin++ Advanced I/O functions that are commonly
 * used with Arduino. These functions enable interfacing with various sensors
 * and peripherals without requiring specialized libraries.
 * 
 * Demonstrations:
 * 1. pulseIn() - Measure pulse widths (ultrasonic sensor simulation)
 * 2. shiftOut() - Control shift register LEDs (74HC595)
 * 3. shiftIn() - Read shift register buttons (74HC165)
 * 4. tone()/noTone() - Generate audio frequencies (buzzer control)
 * 
 * @author PiPinPP Team
 * @version 0.3.7
 * @date 2025-11-06
 */

#include <ArduinoCompat.hpp>
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

/* ============================================================================
 * DEMONSTRATION 1: pulseIn() - Ultrasonic Distance Sensor
 * ============================================================================
 * 
 * pulseIn() measures how long a pin stays HIGH or LOW. This is essential for:
 * - Ultrasonic distance sensors (HC-SR04)
 * - Infrared receivers
 * - Frequency measurement
 * - PWM signal reading
 * 
 * Example: HC-SR04 ultrasonic sensor
 * 1. Send 10µs trigger pulse
 * 2. Measure echo pulse duration
 * 3. Calculate distance: duration * 0.034 / 2 (speed of sound)
 */

void demonstratePulseIn() {
    cout << "\n╔════════════════════════════════════════════════════════════╗\n";
    cout <<   "║  DEMONSTRATION 1: pulseIn() - Pulse Width Measurement      ║\n";
    cout <<   "╚════════════════════════════════════════════════════════════╝\n\n";
    
    cout << "Purpose: Measure pulse widths for sensors like HC-SR04 ultrasonic\n";
    cout << "Function: unsigned long pulseIn(pin, state, timeout)\n\n";
    
    // Simulate ultrasonic sensor timing
    cout << "Example: Ultrasonic Distance Sensor (HC-SR04)\n";
    cout << "------------------------------------------------\n\n";
    
    cout << "Typical usage pattern:\n";
    cout << "  const int TRIG_PIN = 17;\n";
    cout << "  const int ECHO_PIN = 27;\n\n";
    
    cout << "  // Send trigger pulse\n";
    cout << "  digitalWrite(TRIG_PIN, LOW);\n";
    cout << "  delayMicroseconds(2);\n";
    cout << "  digitalWrite(TRIG_PIN, HIGH);\n";
    cout << "  delayMicroseconds(10);\n";
    cout << "  digitalWrite(TRIG_PIN, LOW);\n\n";
    
    cout << "  // Measure echo pulse\n";
    cout << "  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);\n\n";
    
    cout << "  // Calculate distance\n";
    cout << "  float distance_cm = duration * 0.034 / 2;\n";
    cout << "  float distance_inch = duration * 0.0133 / 2;\n\n";
    
    // Simulate various distances
    vector<unsigned long> simulatedDurations = {
        145,   // ~2.5 cm
        580,   // ~10 cm
        1160,  // ~20 cm
        2320,  // ~40 cm
        5800,  // ~100 cm
        11600  // ~200 cm
    };
    
    cout << "Simulated sensor readings:\n";
    cout << "┌──────────────┬──────────────┬─────────────┐\n";
    cout << "│ Duration (µs)│ Distance (cm)│ Distance (in)│\n";
    cout << "├──────────────┼──────────────┼──────────────┤\n";
    
    for (auto duration : simulatedDurations) {
        float dist_cm = duration * 0.034 / 2.0;
        float dist_in = duration * 0.0133 / 2.0;
        
        cout << "│ " << setw(12) << duration 
             << " │ " << setw(12) << fixed << setprecision(1) << dist_cm
             << " │ " << setw(11) << fixed << setprecision(2) << dist_in << " │\n";
    }
    cout << "└──────────────┴──────────────┴──────────────┘\n\n";
    
    cout << "Key points:\n";
    cout << "  • Blocking function - waits for pulse\n";
    cout << "  • Timeout prevents infinite waiting\n";
    cout << "  • Returns 0 on timeout\n";
    cout << "  • Accuracy: ±10µs (sufficient for most sensors)\n";
    cout << "  • Range: HC-SR04 measures 2cm - 4m\n\n";
}

/* ============================================================================
 * DEMONSTRATION 2: shiftOut() - Shift Register Output
 * ============================================================================
 * 
 * shiftOut() sends data serially to shift registers like 74HC595.
 * This expands your GPIO outputs - control 8 LEDs with just 2-3 pins!
 * 
 * Common uses:
 * - LED arrays and 7-segment displays
 * - Multiplexing outputs
 * - Reducing pin count for large projects
 */

void demonstrateShiftOut() {
    cout << "\n╔════════════════════════════════════════════════════════════╗\n";
    cout <<   "║  DEMONSTRATION 2: shiftOut() - Shift Register Output       ║\n";
    cout <<   "╚════════════════════════════════════════════════════════════╝\n\n";
    
    cout << "Purpose: Control 8+ outputs with only 2-3 GPIO pins\n";
    cout << "Function: void shiftOut(dataPin, clockPin, bitOrder, value)\n\n";
    
    cout << "Hardware: 74HC595 Shift Register\n";
    cout << "  • Serial In Parallel Out (SIPO)\n";
    cout << "  • Cascade multiple chips for more outputs\n";
    cout << "  • Control 8 LEDs with 3 pins (data, clock, latch)\n\n";
    
    cout << "Circuit connection:\n";
    cout << "  Pi GPIO 17 (DATA)  → 74HC595 Pin 14 (DS - Serial Data)\n";
    cout << "  Pi GPIO 27 (CLOCK) → 74HC595 Pin 11 (SHCP - Shift Clock)\n";
    cout << "  Pi GPIO 22 (LATCH) → 74HC595 Pin 12 (STCP - Storage Clock)\n\n";
    
    cout << "Example code:\n";
    cout << "  const int DATA_PIN = 17;\n";
    cout << "  const int CLOCK_PIN = 27;\n";
    cout << "  const int LATCH_PIN = 22;\n\n";
    
    cout << "  pinMode(LATCH_PIN, OUTPUT);\n";
    cout << "  digitalWrite(LATCH_PIN, LOW);   // Begin transmission\n";
    cout << "  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, pattern);\n";
    cout << "  digitalWrite(LATCH_PIN, HIGH);  // Display output\n\n";
    
    // Demonstrate various LED patterns
    vector<pair<unsigned char, string>> patterns = {
        {0b00000000, "All OFF"},
        {0b11111111, "All ON"},
        {0b10101010, "Alternating 1"},
        {0b01010101, "Alternating 2"},
        {0b10000000, "LED 7 only (MSB)"},
        {0b00000001, "LED 0 only (LSB)"},
        {0b11110000, "Upper 4 LEDs"},
        {0b00001111, "Lower 4 LEDs"},
        {0b10000001, "Edge LEDs"},
        {0b01111110, "Middle 6 LEDs"}
    };
    
    cout << "LED Patterns (8-bit binary → visual representation):\n";
    cout << "┌──────────┬────────────────────┬────────────────────────────┐\n";
    cout << "│  Binary  │    Hex    │       LED Pattern (7→0)       │\n";
    cout << "├──────────┼────────────────────┼────────────────────────────┤\n";
    
    for (const auto& [pattern, description] : patterns) {
        cout << "│ ";
        // Print binary
        for (int i = 7; i >= 0; i--) {
            cout << ((pattern >> i) & 1);
        }
        cout << " │   0x" << hex << uppercase << setw(2) << setfill('0') 
             << (int)pattern << dec << setfill(' ') << "   │ ";
        
        // Visual representation
        for (int i = 7; i >= 0; i--) {
            cout << ((pattern >> i) & 1 ? "■" : "□") << " ";
        }
        cout << "│ " << description << "\n";
    }
    cout << "└──────────┴────────────────────┴────────────────────────────┘\n\n";
    
    cout << "Knight Rider effect (scanning LED):\n";
    cout << "  for (int i = 0; i < 8; i++) {\n";
    cout << "      shiftOut(DATA, CLOCK, MSBFIRST, 1 << i);\n";
    cout << "      delay(100);\n";
    cout << "  }\n\n";
    
    cout << "Key points:\n";
    cout << "  • MSBFIRST = Most Significant Bit First (bit 7 first)\n";
    cout << "  • LSBFIRST = Least Significant Bit First (bit 0 first)\n";
    cout << "  • Can cascade multiple chips for 16, 24+ outputs\n";
    cout << "  • Typical clock speed: 1-10 MHz (we use ~1MHz)\n\n";
}

/* ============================================================================
 * DEMONSTRATION 3: shiftIn() - Shift Register Input
 * ============================================================================
 * 
 * shiftIn() reads data from shift registers like 74HC165.
 * Expand your inputs - read 8 buttons with just 2-3 pins!
 */

void demonstrateShiftIn() {
    cout << "\n╔════════════════════════════════════════════════════════════╗\n";
    cout <<   "║  DEMONSTRATION 3: shiftIn() - Shift Register Input         ║\n";
    cout <<   "╚════════════════════════════════════════════════════════════╝\n\n";
    
    cout << "Purpose: Read 8+ inputs with only 2-3 GPIO pins\n";
    cout << "Function: unsigned char shiftIn(dataPin, clockPin, bitOrder)\n\n";
    
    cout << "Hardware: 74HC165 Shift Register\n";
    cout << "  • Parallel In Serial Out (PISO)\n";
    cout << "  • Read 8 buttons/switches with 3 pins\n";
    cout << "  • Cascade for more inputs\n\n";
    
    cout << "Circuit connection:\n";
    cout << "  Pi GPIO 17 (DATA)  → 74HC165 Pin 9 (QH - Serial Data Out)\n";
    cout << "  Pi GPIO 27 (CLOCK) → 74HC165 Pin 2 (CLK - Clock)\n";
    cout << "  Pi GPIO 22 (LOAD)  → 74HC165 Pin 1 (SH/LD - Load)\n\n";
    
    cout << "Example code:\n";
    cout << "  const int DATA_PIN = 17;\n";
    cout << "  const int CLOCK_PIN = 27;\n";
    cout << "  const int LOAD_PIN = 22;\n\n";
    
    cout << "  digitalWrite(LOAD_PIN, LOW);    // Load parallel data\n";
    cout << "  delayMicroseconds(5);\n";
    cout << "  digitalWrite(LOAD_PIN, HIGH);   // Ready to shift\n\n";
    
    cout << "  unsigned char buttons = shiftIn(DATA_PIN, CLOCK_PIN, MSBFIRST);\n\n";
    
    cout << "  // Check individual buttons\n";
    cout << "  if (buttons & (1 << 0)) { /* Button 0 pressed */ }\n";
    cout << "  if (buttons & (1 << 5)) { /* Button 5 pressed */ }\n\n";
    
    // Simulate button press combinations
    vector<pair<unsigned char, string>> buttonStates = {
        {0b00000000, "No buttons pressed"},
        {0b00000001, "Button 0 pressed"},
        {0b10000000, "Button 7 pressed"},
        {0b11111111, "All buttons pressed"},
        {0b00001111, "Buttons 0-3 pressed"},
        {0b10101010, "Even buttons pressed"},
        {0b00010001, "Buttons 0 and 4 pressed"},
        {0b11110000, "Buttons 4-7 pressed"}
    };
    
    cout << "Button State Examples:\n";
    cout << "┌──────────┬────────────────────────────────────────┐\n";
    cout << "│  Binary  │         Button Status (7→0)           │\n";
    cout << "├──────────┼────────────────────────────────────────┤\n";
    
    for (const auto& [state, description] : buttonStates) {
        cout << "│ ";
        // Print binary with button indicators
        for (int i = 7; i >= 0; i--) {
            cout << ((state >> i) & 1);
        }
        cout << " │ ";
        
        // Visual button representation
        for (int i = 7; i >= 0; i--) {
            if ((state >> i) & 1) {
                cout << "[" << i << "]";  // Pressed
            } else {
                cout << " " << i << " ";  // Released
            }
        }
        cout << " │\n";
        cout << "│          │ " << description << setw(39 - description.length()) << " │\n";
    }
    cout << "└──────────┴────────────────────────────────────────┘\n\n";
    
    cout << "Debouncing example:\n";
    cout << "  unsigned char lastState = 0;\n";
    cout << "  unsigned char currentState = shiftIn(DATA, CLOCK, MSBFIRST);\n";
    cout << "  unsigned char changed = currentState ^ lastState;\n";
    cout << "  unsigned char pressed = changed & currentState;\n\n";
    
    cout << "  for (int i = 0; i < 8; i++) {\n";
    cout << "      if (pressed & (1 << i)) {\n";
    cout << "          Serial.print(\"Button \");\n";
    cout << "          Serial.print(i);\n";
    cout << "          Serial.println(\" pressed!\");\n";
    cout << "      }\n";
    cout << "  }\n\n";
    
    cout << "Key points:\n";
    cout << "  • Returns 8-bit value (one bit per input)\n";
    cout << "  • Use bitwise operations to check individual inputs\n";
    cout << "  • Can chain multiple chips for 16, 24+ inputs\n";
    cout << "  • Perfect for button matrices and switch banks\n\n";
}

/* ============================================================================
 * DEMONSTRATION 4: tone() and noTone() - Audio Generation
 * ============================================================================
 * 
 * tone() generates square waves at specific frequencies.
 * Perfect for:
 * - Buzzers and alarms
 * - Musical notes
 * - Audio feedback
 * - Morse code
 */

void demonstrateTone() {
    cout << "\n╔════════════════════════════════════════════════════════════╗\n";
    cout <<   "║  DEMONSTRATION 4: tone()/noTone() - Audio Generation       ║\n";
    cout <<   "╚════════════════════════════════════════════════════════════╝\n\n";
    
    cout << "Purpose: Generate audio tones and simple music\n";
    cout << "Functions:\n";
    cout << "  void tone(pin, frequency, duration=0)\n";
    cout << "  void noTone(pin)\n\n";
    
    cout << "Hardware: Piezo Buzzer or Small Speaker\n";
    cout << "  Pi GPIO 17 → Buzzer (+) → Buzzer (-) → Ground\n\n";
    
    cout << "Musical Notes (Standard Frequencies):\n";
    cout << "┌──────┬──────────┬─────┬──────────┬─────┬──────────┐\n";
    cout << "│ Note │   Freq   │Note │   Freq   │Note │   Freq   │\n";
    cout << "├──────┼──────────┼─────┼──────────┼─────┼──────────┤\n";
    cout << "│  C4  │  262 Hz  │ E4  │  330 Hz  │ G4  │  392 Hz  │\n";
    cout << "│  C#4 │  277 Hz  │ F4  │  349 Hz  │ G#4 │  415 Hz  │\n";
    cout << "│  D4  │  294 Hz  │ F#4 │  370 Hz  │ A4  │  440 Hz  │\n";
    cout << "│  D#4 │  311 Hz  │ G4  │  392 Hz  │ A#4 │  466 Hz  │\n";
    cout << "│  E4  │  330 Hz  │ G#4 │  415 Hz  │ B4  │  494 Hz  │\n";
    cout << "└──────┴──────────┴─────┴──────────┴─────┴──────────┘\n\n";
    
    cout << "Example 1: Simple beep\n";
    cout << "  tone(17, 1000);     // 1kHz tone\n";
    cout << "  delay(500);         // Play for 500ms\n";
    cout << "  noTone(17);         // Stop\n\n";
    
    cout << "Example 2: Tone with duration (blocking)\n";
    cout << "  tone(17, 1000, 500);  // 1kHz for 500ms (automatic stop)\n\n";
    
    cout << "Example 3: Play \"Mary Had a Little Lamb\"\n";
    cout << "  int melody[] = {330, 294, 262, 294, 330, 330, 330}; // E D C D E E E\n";
    cout << "  int durations[] = {250, 250, 250, 250, 250, 250, 500};\n\n";
    
    cout << "  for (int i = 0; i < 7; i++) {\n";
    cout << "      tone(17, melody[i], durations[i]);\n";
    cout << "      delay(durations[i] * 1.3);  // Add gap between notes\n";
    cout << "  }\n\n";
    
    cout << "Example 4: Alarm sound\n";
    cout << "  for (int i = 0; i < 10; i++) {\n";
    cout << "      tone(17, 800, 100);   // High beep\n";
    cout << "      delay(150);\n";
    cout << "      tone(17, 400, 100);   // Low beep\n";
    cout << "      delay(150);\n";
    cout << "  }\n\n";
    
    cout << "Example 5: Frequency sweep (sci-fi sound)\n";
    cout << "  for (int freq = 200; freq <= 2000; freq += 50) {\n";
    cout << "      tone(17, freq, 50);\n";
    cout << "      delay(60);\n";
    cout << "  }\n";
    cout << "  noTone(17);\n\n";
    
    cout << "Common frequencies:\n";
    cout << "  • Door bell: 800-1000 Hz\n";
    cout << "  • Phone ring: 400-480 Hz\n";
    cout << "  • Alarm: 2-3 kHz\n";
    cout << "  • Middle C (C4): 262 Hz\n";
    cout << "  • A4 (tuning): 440 Hz\n";
    cout << "  • Human hearing: 20 Hz - 20 kHz\n\n";
    
    cout << "Key points:\n";
    cout << "  • Frequency range: 31 Hz - 65535 Hz\n";
    cout << "  • Uses PWM at 50% duty cycle\n";
    cout << "  • Duration parameter is optional (0 = continuous)\n";
    cout << "  • Always call noTone() to stop continuous tones\n";
    cout << "  • Only one tone per pin at a time\n\n";
}

/* ============================================================================
 * MAIN PROGRAM
 * ============================================================================
 */

int main() {
    cout << "\n";
    cout << "╔══════════════════════════════════════════════════════════════╗\n";
    cout << "║     PiPin++ Advanced I/O Functions Demonstration             ║\n";
    cout << "║                                                               ║\n";
    cout << "║  Exploring Arduino-inspired advanced I/O capabilities         ║\n";
    cout << "║  Version 0.3.7 - November 6, 2025                            ║\n";
    cout << "╚══════════════════════════════════════════════════════════════╝\n";
    
    cout << "\nThis demonstration shows how to use PiPin++'s advanced I/O\n";
    cout << "functions for interfacing with various sensors and peripherals.\n";
    cout << "\nNote: This is a code example demonstration.\n";
    cout << "      Hardware connections are shown but not required to run.\n";
    
    // Run all demonstrations
    demonstratePulseIn();
    demonstrateShiftOut();
    demonstrateShiftIn();
    demonstrateTone();
    
    // Summary
    cout << "\n╔══════════════════════════════════════════════════════════════╗\n";
    cout <<   "║                         SUMMARY                              ║\n";
    cout <<   "╚══════════════════════════════════════════════════════════════╝\n\n";
    
    cout << "New Advanced I/O Functions:\n\n";
    
    cout << "1. pulseIn(pin, state, timeout)\n";
    cout << "   • Measure pulse widths\n";
    cout << "   • Perfect for ultrasonic sensors, IR receivers\n";
    cout << "   • Returns duration in microseconds\n\n";
    
    cout << "2. shiftOut(dataPin, clockPin, bitOrder, value)\n";
    cout << "   • Send data to shift registers (74HC595)\n";
    cout << "   • Expand outputs: control 8+ LEDs with 2-3 pins\n";
    cout << "   • Support for MSB or LSB first\n\n";
    
    cout << "3. shiftIn(dataPin, clockPin, bitOrder)\n";
    cout << "   • Read data from shift registers (74HC165)\n";
    cout << "   • Expand inputs: read 8+ buttons with 2-3 pins\n";
    cout << "   • Returns 8-bit value\n\n";
    
    cout << "4. tone(pin, frequency, duration) / noTone(pin)\n";
    cout << "   • Generate audio tones (31Hz-65kHz)\n";
    cout << "   • Control buzzers, speakers\n";
    cout << "   • Play simple music\n\n";
    
    cout << "Benefits:\n";
    cout << "  ✓ Familiar Arduino API - easy migration\n";
    cout << "  ✓ Reduce pin usage with shift registers\n";
    cout << "  ✓ Interface with standard sensors\n";
    cout << "  ✓ No external libraries needed\n";
    cout << "  ✓ Comprehensive error handling\n\n";
    
    cout << "Real-world applications:\n";
    cout << "  • Robotics (distance sensors, motor control)\n";
    cout << "  • Home automation (buttons, indicators)\n";
    cout << "  • Musical instruments (tone generation)\n";
    cout << "  • Data acquisition (pulse measurement)\n";
    cout << "  • LED matrices and displays\n\n";
    
    cout << "For complete documentation and more examples:\n";
    cout << "  • docs/API_REFERENCE.md\n";
    cout << "  • examples/\n";
    cout << "  • https://github.com/Barbatos6669/PiPinPP\n\n";
    
    return 0;
}
