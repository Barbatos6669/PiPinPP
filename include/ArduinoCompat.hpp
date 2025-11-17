/**
 * @file ArduinoCompat.hpp
 * @brief Arduino-compatible API for Raspberry Pi GPIO
 * @author Barbatos6669
 * @date 2025
 * 
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

#pragma once
#include "pin.hpp"
#include <cmath>

// Arduino-style constants (simple and familiar)
constexpr bool HIGH = true;
constexpr bool LOW = false;

// Arduino pin modes
enum ArduinoPinMode {
    INPUT = 0,
    OUTPUT = 1,
    INPUT_PULLUP = 2,
    INPUT_PULLDOWN = 3  // Added for Raspberry Pi compatibility
};

// Arduino-style global functions for familiar API
/**
 * @brief Set pin mode (Arduino-style function)
 * 
 * Thread-safe: Multiple threads can call this function concurrently.
 * 
 * @param pin GPIO pin number
 * @param mode INPUT (0) or OUTPUT (1)
 */
void pinMode(int pin, int mode);

/**
 * @brief Write digital value to pin (Arduino-style function)
 * 
 * Thread-safe: Multiple threads can call this function concurrently.
 * 
 * @param pin GPIO pin number
 * @param value HIGH (true) or LOW (false)
 */
void digitalWrite(int pin, bool value);

/**
 * @brief Read digital value from pin (Arduino-style function)
 * 
 * Thread-safe: Multiple threads can call this function concurrently.
 * 
 * @param pin GPIO pin number
 * @return int 1 for HIGH, 0 for LOW, -1 for error
 */
int digitalRead(int pin);

/**
 * @brief Delay execution for specified milliseconds (Arduino-style function)
 * 
 * @param ms Delay time in milliseconds
 */
void delay(unsigned long ms);

/* ------------------------------------------------------------ */
/*                        MATH FUNCTIONS                       */
/* ------------------------------------------------------------*/

// Arduino-specific math functions (functions not available in standard library)
/**
 * @brief Constrain a value between minimum and maximum (Arduino-specific function)
 * @param x Input value to constrain
 * @param min Minimum allowed value
 * @param max Maximum allowed value
 * @return int Value constrained between min and max
 */
int constrain(int x, int min, int max);

/**
 * @brief Constrain a value between minimum and maximum (Arduino-specific function)
 * @param x Input value to constrain
 * @param min Minimum allowed value
 * @param max Maximum allowed value
 * @return long Value constrained between min and max
 */
long constrain(long x, long min, long max);

/**
 * @brief Constrain a value between minimum and maximum (Arduino-specific function)
 * @param x Input value to constrain
 * @param min Minimum allowed value
 * @param max Maximum allowed value
 * @return float Value constrained between min and max
 */
float constrain(float x, float min, float max);

/**
 * @brief Map a value from one range to another (Arduino-specific function)
 * @param x Input value to map
 * @param in_min Minimum of input range
 * @param in_max Maximum of input range
 * @param out_min Minimum of output range
 * @param out_max Maximum of output range
 * @return long Mapped value
 */
long map(long x, long in_min, long in_max, long out_min, long out_max);

// Note: For abs() function, use std::abs() from <cmath> or <cstdlib> instead

/* ------------------------------------------------------------ */
/*                        TIMING FUNCTIONS                       */
/* ------------------------------------------------------------ */

/**
 * @brief Returns milliseconds since program start (Arduino-style function)
 * 
 * Uses monotonic clock that won't jump if system time changes.
 * Resets to 0 at program start. Overflows after ~49 days.
 * 
 * @return unsigned long Milliseconds elapsed since program started
 * 
 * @example
 * unsigned long startTime = millis();
 * // ... do work ...
 * unsigned long elapsed = millis() - startTime;
 */
unsigned long millis();

/**
 * @brief Returns microseconds since program start (Arduino-style function)
 * 
 * High-precision timing using monotonic clock.
 * Resets to 0 at program start. Overflows after ~71 minutes.
 * 
 * @return unsigned long Microseconds elapsed since program started
 * 
 * @example
 * unsigned long start = micros();
 * // ... time-critical code ...
 * unsigned long duration = micros() - start;
 */
unsigned long micros();

/**
 * @brief Delay execution in microseconds with high precision (Arduino-style function)
 * 
 * Uses busy-waiting for microsecond accuracy (consumes CPU).
 * Use delay() for millisecond delays (more efficient).
 * 
 * @param us Delay time in microseconds
 * 
 * @note Accurate to ±1-2 microseconds
 * @warning Uses busy-wait loop - consumes CPU during delay
 * 
 * @example
 * digitalWrite(17, HIGH);
 * delayMicroseconds(100);  // 100µs pulse
 * digitalWrite(17, LOW);
 */
void delayMicroseconds(unsigned int us);

/* ------------------------------------------------------------ */
/*                    INTERRUPT FUNCTIONS                       */
/* ------------------------------------------------------------ */

// Forward declare InterruptMode enum from interrupts.hpp
enum class InterruptMode;

// Arduino interrupt mode constants (use constexpr for type safety)
constexpr int RISING = 0;   ///< Trigger interrupt on rising edge (LOW to HIGH)
constexpr int FALLING = 1;  ///< Trigger interrupt on falling edge (HIGH to LOW)
constexpr int CHANGE = 2;   ///< Trigger interrupt on any edge (rising or falling)

/**
 * @brief Attach an interrupt handler to a GPIO pin (Arduino-style function)
 * 
 * Registers a callback function to be invoked when the specified edge condition
 * occurs on the GPIO pin. The callback runs in a separate thread and should be
 * fast and non-blocking.
 * 
 * Thread-safe: Multiple threads can call this function concurrently.
 * 
 * @param pin GPIO pin number (0-27 for Raspberry Pi)
 * @param callback Function to call when interrupt triggers (no parameters, void return)
 * @param mode Interrupt mode: RISING, FALLING, or CHANGE
 * 
 * @throws InvalidPinError if pin number is invalid
 * @throws GpioAccessError if unable to configure interrupt
 * @throws std::runtime_error if interrupt already attached to this pin
 * 
 * @note Callback should be fast and non-blocking
 * @note Only one interrupt can be attached per pin
 * @note Pin is automatically configured as INPUT with pull-down resistor
 * 
 * @example
 * void buttonPressed() {
 *     static unsigned long lastPress = 0;
 *     unsigned long now = millis();
 *     
 *     // Simple debouncing
 *     if (now - lastPress > 50) {
 *         Serial.println("Button pressed!");
 *         lastPress = now;
 *     }
 * }
 * 
 * void setup() {
 *     pinMode(18, INPUT_PULLUP);
 *     attachInterrupt(18, buttonPressed, FALLING);
 * }
 * 
 * @warning Interrupt callbacks run in a separate thread, ensure thread safety
 * @warning Do not call delay() or blocking functions inside interrupt callbacks
 */
void attachInterrupt(int pin, void (*callback)(), int mode);

/**
 * @brief Detach an interrupt handler from a GPIO pin (Arduino-style function)
 * 
 * Removes the interrupt handler previously attached with attachInterrupt().
 * Safe to call even if no interrupt is attached.
 * 
 * Thread-safe: Multiple threads can call this function concurrently.
 * 
 * @param pin GPIO pin number to detach interrupt from
 * 
 * @note Safe to call even if no interrupt is attached
 * @note Pin remains in INPUT mode after detaching
 * 
 * @example
 * void loop() {
 *     if (someCondition) {
 *         detachInterrupt(18);  // Stop listening for interrupts
 *     }
 * }
 */
void detachInterrupt(int pin);

/* ------------------------------------------------------------ */
/*                        PWM FUNCTIONS                         */
/* ------------------------------------------------------------ */

/**
 * @brief Write an analog value (PWM wave) to a pin (Arduino-style function)
 * 
 * Generates a PWM (Pulse Width Modulation) signal on the specified pin.
 * Can be used to control LED brightness, motor speed, or simulate analog output.
 * 
 * Thread-safe: Multiple threads can call this function concurrently.
 * 
 * @param pin GPIO pin number (0-27 for Raspberry Pi)
 * @param value Duty cycle value (0-255)
 *              - 0   = always LOW (0% duty cycle, LED off)
 *              - 127 = 50% duty cycle (LED half brightness)
 *              - 255 = always HIGH (100% duty cycle, LED full brightness)
 * 
 * @throws InvalidPinError if pin number is invalid
 * @throws GpioAccessError if unable to configure pin
 * 
 * @note Pin is automatically configured as OUTPUT
 * @note PWM frequency is 490Hz by default (matches Arduino UNO)
 * @note This is software PWM and has timing jitter (~1-10 µs)
 * @note Each PWM pin uses a dedicated busy-loop thread
 * 
 * @warning ⚠️ CPU USAGE: Each PWM pin consumes ~10-30% CPU in busy-wait loop
 * @warning Uses std::this_thread::yield() but still busy-waits for timing accuracy
 * @warning Multiple PWM pins can cause significant CPU load
 * @warning ❌ NOT suitable for servo control - use HardwarePWM class (zero CPU usage)
 * @warning Software PWM best for LED dimming only, not precision timing
 * 
 * @example
 * // Fade an LED from off to full brightness
 * void setup() {
 *     pinMode(17, OUTPUT);
 * }
 * 
 * void loop() {
 *     // Fade in
 *     for (int brightness = 0; brightness <= 255; brightness++) {
 *         analogWrite(17, brightness);
 *         delay(10);
 *     }
 *     
 *     // Fade out
 *     for (int brightness = 255; brightness >= 0; brightness--) {
 *         analogWrite(17, brightness);
 *         delay(10);
 *     }
 * }
 * 
 * @example
 * // Control motor speed
 * analogWrite(18, 0);    // Motor off
 * analogWrite(18, 128);  // Motor half speed
 * analogWrite(18, 255);  // Motor full speed
 */
void analogWrite(int pin, int value);

/* ------------------------------------------------------------ */
/*                    PIN STATE QUERY FUNCTIONS                 */
/* ------------------------------------------------------------ */

/**
 * @brief Check if a pin is configured as OUTPUT
 * 
 * Thread-safe: Multiple threads can call this function concurrently.
 * 
 * @param pin GPIO pin number
 * @return true if pin is OUTPUT
 * @return false if pin is not OUTPUT
 * 
 * @throws PinError if pin is not initialized (pinMode not called)
 */
bool isOutput(int pin);

/**
 * @brief Check if a pin is configured as INPUT
 * 
 * Thread-safe: Multiple threads can call this function concurrently.
 * 
 * @param pin GPIO pin number
 * @return true if pin is INPUT, INPUT_PULLUP, or INPUT_PULLDOWN
 * @return false if pin is not INPUT
 * 
 * @throws PinError if pin is not initialized (pinMode not called)
 */
bool isInput(int pin);

/**
 * @brief Get the current mode of a pin
 * 
 * Thread-safe: Multiple threads can call this function concurrently.
 * 
 * @param pin GPIO pin number
 * @return ArduinoPinMode Current mode of the pin (INPUT, OUTPUT, INPUT_PULLUP, INPUT_PULLDOWN)
 * 
 * @throws PinError if pin is not initialized (pinMode not called)
 */
ArduinoPinMode getMode(int pin);

/**
 * @brief Toggle the state of an OUTPUT pin (HIGH↔LOW)
 * 
 * Efficiently toggles pin state without reading current value.
 * Much faster than digitalRead() + digitalWrite().
 * 
 * Thread-safe: Multiple threads can call this function concurrently.
 * 
 * @param pin GPIO pin number
 * @throws PinError if pin not initialized or not OUTPUT
 * 
 * @note Example:
 * @code
 * pinMode(17, OUTPUT);
 * digitalWrite(17, HIGH);
 * digitalToggle(17);  // Now LOW
 * digitalToggle(17);  // Now HIGH
 * @endcode
 */
void digitalToggle(int pin);

/* ------------------------------------------------------------ */
/*                  EXTENDED MATH FUNCTIONS                     */
/* ------------------------------------------------------------ */

/**
 * @brief Calculate the square of a number (Arduino-inspired function)
 * 
 * Returns x * x. More efficient than using pow(x, 2).
 * Works with any numeric type (int, long, float, double).
 * 
 * @tparam T Numeric type (int, long, float, double, etc.)
 * @param x Value to square
 * @return T Square of x (x²)
 * 
 * @example
 * int a = sq(5);        // Returns 25
 * float b = sq(3.5f);   // Returns 12.25
 * double c = sq(-4.0);  // Returns 16.0
 */
template<typename T>
inline T sq(T x) {
    return x * x;
}

// Note: For sqrt(), pow(), max(), min() use the standard library functions:
// - #include <cmath>      for std::sqrt() and std::pow()
// - #include <algorithm>  for std::max() and std::min()
// These are already available in C++ and creating wrappers causes ambiguity
// when users write "using namespace std;" in their code.

/* ------------------------------------------------------------ */
/*                  TRIGONOMETRY CONSTANTS                      */
/* ------------------------------------------------------------ */

/**
 * @brief Convert degrees to radians (Arduino-inspired constant)
 * 
 * Multiply an angle in degrees by DEG_TO_RAD to convert to radians.
 * Use with std::sin(), std::cos(), std::tan() from <cmath>.
 * 
 * @example
 * double angleInDegrees = 45.0;
 * double angleInRadians = angleInDegrees * DEG_TO_RAD;
 * double sineValue = std::sin(angleInRadians);
 */
constexpr double DEG_TO_RAD = 0.017453292519943295769236907684886;

/**
 * @brief Convert radians to degrees (Arduino-inspired constant)
 * 
 * Multiply an angle in radians by RAD_TO_DEG to convert to degrees.
 * 
 * @example
 * double angleInRadians = 1.5708;
 * double angleInDegrees = angleInRadians * RAD_TO_DEG;  // ~90 degrees
 */
constexpr double RAD_TO_DEG = 57.295779513082320876798154814105;

// Note: For sin(), cos(), tan() use the standard library functions:
// - #include <cmath> for std::sin(), std::cos(), std::tan()
// Arduino code like sin(angle * DEG_TO_RAD) becomes std::sin(angle * DEG_TO_RAD)
// This avoids naming conflicts when users write "using namespace std;".

/* ------------------------------------------------------------ */
/*                    RANDOM FUNCTIONS                          */
/* ------------------------------------------------------------ */

/**
 * @brief Initialize the random number generator (Arduino-inspired)
 * 
 * Seeds the pseudo-random number generator with a value. Use different
 * seeds to get different random sequences. Common practice is to seed
 * with micros() or a reading from an analog pin.
 * 
 * @param seed The seed value (any unsigned long)
 * 
 * @example
 * randomSeed(micros());  // Seed with current time
 * long value = random(100);  // Get random number 0-99
 */
void randomSeed(unsigned long seed);

/**
 * @brief Generate random number in range [0, max) (Arduino-inspired)
 * 
 * Returns a pseudo-random number from 0 up to (but not including) max.
 * 
 * @param max Upper bound (exclusive) - must be positive
 * @return long Random number in range [0, max)
 * 
 * @example
 * long dice = random(6) + 1;  // Simulate dice roll (1-6)
 * long value = random(100);    // Random 0-99
 */
long random(long max);

/**
 * @brief Generate random number in range [min, max) (Arduino-inspired)
 * 
 * Returns a pseudo-random number from min up to (but not including) max.
 * 
 * @param min Lower bound (inclusive)
 * @param max Upper bound (exclusive)
 * @return long Random number in range [min, max)
 * 
 * @example
 * long temp = random(20, 30);  // Random temperature 20-29
 * long delay = random(100, 500);  // Random delay 100-499ms
 */
long random(long min, long max);

/* ------------------------------------------------------------ */
/*                  BITS AND BYTES FUNCTIONS                    */
/* ------------------------------------------------------------ */

/**
 * @brief Compute the value of a bit (Arduino-inspired)
 * 
 * Returns the value of the bit at position n (0-31).
 * Equivalent to: (1 << n)
 * 
 * @param n Bit position (0-31)
 * @return unsigned long Value with bit n set (2^n)
 * 
 * @example
 * unsigned long mask = bit(4);  // Returns 16 (binary: 00010000)
 * unsigned long flags = bit(0) | bit(3) | bit(7);  // Multiple bits
 */
inline unsigned long bit(unsigned int n) {
    return 1UL << n;
}

/**
 * @brief Read a bit from a value (Arduino-inspired)
 * 
 * Returns the value of the bit at position n in value x.
 * 
 * @param x Value to read from
 * @param n Bit position (0-31)
 * @return unsigned long 1 if bit is set, 0 if bit is clear
 * 
 * @example
 * unsigned long flags = 0b10110100;
 * bool isSet = bitRead(flags, 5);  // Returns 1 (bit 5 is set)
 * bool isClear = bitRead(flags, 0);  // Returns 0 (bit 0 is clear)
 */
inline unsigned long bitRead(unsigned long x, unsigned int n) {
    return (x >> n) & 1UL;
}

/**
 * @brief Write a bit in a value (Arduino-inspired)
 * 
 * Sets or clears the bit at position n in value x.
 * 
 * @param x Value to modify
 * @param n Bit position (0-31)
 * @param b Bit value (0 or 1)
 * 
 * @example
 * unsigned long flags = 0;
 * bitWrite(flags, 3, 1);  // Set bit 3: flags = 0b00001000
 * bitWrite(flags, 3, 0);  // Clear bit 3: flags = 0b00000000
 */
inline void bitWrite(unsigned long &x, unsigned int n, unsigned int b) {
    if (b) {
        x |= (1UL << n);   // Set bit
    } else {
        x &= ~(1UL << n);  // Clear bit
    }
}

/**
 * @brief Set a bit in a value (Arduino-inspired)
 * 
 * Sets the bit at position n in value x to 1.
 * 
 * @param x Value to modify
 * @param n Bit position (0-31)
 * 
 * @example
 * unsigned long flags = 0b00000000;
 * bitSet(flags, 2);  // flags = 0b00000100
 * bitSet(flags, 5);  // flags = 0b00100100
 */
inline void bitSet(unsigned long &x, unsigned int n) {
    x |= (1UL << n);
}

/**
 * @brief Clear a bit in a value (Arduino-inspired)
 * 
 * Sets the bit at position n in value x to 0.
 * 
 * @param x Value to modify
 * @param n Bit position (0-31)
 * 
 * @example
 * unsigned long flags = 0b11111111;
 * bitClear(flags, 2);  // flags = 0b11111011
 * bitClear(flags, 5);  // flags = 0b11011011
 */
inline void bitClear(unsigned long &x, unsigned int n) {
    x &= ~(1UL << n);
}

/**
 * @brief Get the high byte of a word (Arduino-inspired)
 * 
 * Extracts the high-order (leftmost) byte of a word (16-bit value).
 * 
 * @param x 16-bit word value
 * @return unsigned char High byte (bits 8-15)
 * 
 * @example
 * unsigned int value = 0xABCD;
 * unsigned char high = highByte(value);  // Returns 0xAB
 * unsigned char low = lowByte(value);    // Returns 0xCD
 */
inline unsigned char highByte(unsigned int x) {
    return (x >> 8) & 0xFF;
}

/**
 * @brief Get the low byte of a word (Arduino-inspired)
 * 
 * Extracts the low-order (rightmost) byte of a word (16-bit value).
 * 
 * @param x 16-bit word value
 * @return unsigned char Low byte (bits 0-7)
 * 
 * @example
 * unsigned int value = 0xABCD;
 * unsigned char low = lowByte(value);   // Returns 0xCD
 * unsigned char high = highByte(value); // Returns 0xAB
 */
inline unsigned char lowByte(unsigned int x) {
    return x & 0xFF;
}

/* ------------------------------------------------------------ */
/*                 CHARACTER FUNCTIONS                          */
/* ------------------------------------------------------------ */

// Note: Arduino's character classification functions (isAlpha, isDigit, 
// isSpace, etc.) are already available in C++ standard library <cctype>.
// Use std::isalpha(), std::isdigit(), std::isspace(), etc.
//
// We don't provide wrappers to avoid naming conflicts when users write
// "using namespace std;".
//
// Arduino code:    if (isAlpha(ch)) { ... }
// PiPinPP code:    if (std::isalpha(ch)) { ... }
//
// Available functions in <cctype>:
// - std::isalpha(ch)   - Check if alphabetic
// - std::isdigit(ch)   - Check if digit
// - std::isspace(ch)   - Check if whitespace
// - std::isalnum(ch)   - Check if alphanumeric
// - std::isupper(ch)   - Check if uppercase
// - std::islower(ch)   - Check if lowercase
// - std::isxdigit(ch)  - Check if hexadecimal digit
// - std::ispunct(ch)   - Check if punctuation
// - std::isprint(ch)   - Check if printable
// - std::iscntrl(ch)   - Check if control character
// - std::isgraph(ch)   - Check if graphical character

/* ------------------------------------------------------------ */
/*                    ADVANCED I/O FUNCTIONS                    */
/* ------------------------------------------------------------ */

/**
 * @brief Measure the length of a pulse on a pin (Arduino-inspired)
 * 
 * Reads a pulse (either HIGH or LOW) on a pin. Times how long the pin
 * remains in that state. Useful for reading sensors like ultrasonic
 * distance sensors, infrared receivers, or any pulse-based communication.
 * 
 * @param pin GPIO pin number to read
 * @param state Pulse state to measure (HIGH or LOW)
 * @param timeout Maximum time to wait in microseconds (default: 1000000 = 1 second)
 * @return unsigned long Pulse length in microseconds, or 0 if timeout
 * 
 * @note Pin is automatically configured as INPUT if not already set
 * @note This is a blocking function - will wait up to timeout microseconds
 * @note Accuracy depends on system load (typical accuracy ±10µs)
 * 
 * @warning ⚠️ CPU USAGE: Uses busy-waiting loop consuming 100% of one CPU core
 * @warning Blocks execution and consumes full CPU until pulse completes or timeout
 * @warning For non-blocking pulse measurement, consider using interrupts with timestamps
 * 
 * @example
 * // Read ultrasonic sensor (HC-SR04)
 * pinMode(TRIG_PIN, OUTPUT);
 * pinMode(ECHO_PIN, INPUT);
 * 
 * // Send trigger pulse
 * digitalWrite(TRIG_PIN, LOW);
 * delayMicroseconds(2);
 * digitalWrite(TRIG_PIN, HIGH);
 * delayMicroseconds(10);
 * digitalWrite(TRIG_PIN, LOW);
 * 
 * // Measure echo pulse
 * unsigned long duration = pulseIn(ECHO_PIN, HIGH);
 * float distance_cm = duration * 0.034 / 2;  // Speed of sound
 */
unsigned long pulseIn(int pin, bool state, unsigned long timeout = 1000000);

/**
 * @brief Measure the length of a pulse on a pin with extended range (Arduino-inspired)
 * 
 * Similar to pulseIn() but returns unsigned long for measuring longer pulses.
 * Use this for pulses longer than ~71 minutes.
 * 
 * @param pin GPIO pin number to read
 * @param state Pulse state to measure (HIGH or LOW)
 * @param timeout Maximum time to wait in microseconds (default: 1000000 = 1 second)
 * @return unsigned long Pulse length in microseconds, or 0 if timeout
 * 
 * @note Identical to pulseIn() on Raspberry Pi (both return unsigned long)
 * @note Provided for Arduino code compatibility
 */
inline unsigned long pulseInLong(int pin, bool state, unsigned long timeout = 1000000) {
    return pulseIn(pin, state, timeout);
}

/**
 * @brief Shift out a byte of data one bit at a time (Arduino-inspired)
 * 
 * Shifts out a byte of data using bit-banging. Commonly used with shift
 * registers (74HC595) to expand the number of outputs.
 * 
 * @param dataPin GPIO pin for serial data (must be configured as OUTPUT)
 * @param clockPin GPIO pin for clock signal (must be configured as OUTPUT)
 * @param bitOrder Bit order: MSBFIRST (most significant bit first) or LSBFIRST
 * @param value Byte value to shift out (0-255)
 * 
 * @throws PinError if pins are not configured as OUTPUT via pinMode()
 * @throws InvalidPinError if pin numbers are out of range (0-27)
 * 
 * @note Call pinMode(pin, OUTPUT) for both pins before using this function
 * @note Clock idles LOW, pulses HIGH to shift each bit
 * @note No separate latch pin - use digitalWrite() to control latch separately
 * 
 * @example
 * // Control 8 LEDs with 74HC595 shift register
 * const int DATA_PIN = 17;
 * const int CLOCK_PIN = 27;
 * const int LATCH_PIN = 22;
 * 
 * pinMode(LATCH_PIN, OUTPUT);
 * digitalWrite(LATCH_PIN, LOW);  // Begin transmission
 * shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0b10101010);  // Pattern
 * digitalWrite(LATCH_PIN, HIGH);  // Latch data to outputs
 */
void shiftOut(int dataPin, int clockPin, int bitOrder, unsigned char value);

/**
 * @brief Shift in a byte of data one bit at a time (Arduino-inspired)
 * 
 * Shifts in a byte of data using bit-banging. Commonly used with shift
 * registers (74HC165) to expand the number of inputs.
 * 
 * @param dataPin GPIO pin for serial data input (must be configured as INPUT)
 * @param clockPin GPIO pin for clock signal output (must be configured as OUTPUT)
 * @param bitOrder Bit order: MSBFIRST (most significant bit first) or LSBFIRST
 * @return unsigned char Byte value read (0-255)
 * 
 * @throws PinError if pins are not configured correctly via pinMode()
 * @throws InvalidPinError if pin numbers are out of range (0-27)
 * 
 * @note Call pinMode(dataPin, INPUT) and pinMode(clockPin, OUTPUT) before using this function
 * @note Clock idles LOW, pulses HIGH to shift each bit
 * 
 * @example
 * // Read 8 buttons with 74HC165 shift register
 * const int DATA_PIN = 17;
 * const int CLOCK_PIN = 27;
 * const int LOAD_PIN = 22;
 * 
 * pinMode(LOAD_PIN, OUTPUT);
 * digitalWrite(LOAD_PIN, LOW);   // Load parallel data
 * delayMicroseconds(5);
 * digitalWrite(LOAD_PIN, HIGH);   // Ready to shift
 * 
 * unsigned char buttons = shiftIn(DATA_PIN, CLOCK_PIN, MSBFIRST);
 * if (buttons & 0x01) { // Check button 0
 *     // Button pressed
 * }
 */
unsigned char shiftIn(int dataPin, int clockPin, int bitOrder);

// Bit order constants for shiftIn/shiftOut
constexpr int LSBFIRST = 0;  ///< Least Significant Bit First
constexpr int MSBFIRST = 1;  ///< Most Significant Bit First

/**
 * @brief Generate a square wave tone on a pin (Arduino-inspired)
 * 
 * Generates a square wave of the specified frequency on a pin. Useful for
 * making beeps, alarms, or simple music with a piezo buzzer or speaker.
 * 
 * @param pin GPIO pin number (must be configured as OUTPUT before calling)
 * @param frequency Frequency in Hertz (31Hz - 65535Hz)
 * @param duration Optional duration in milliseconds (0 = continuous)
 * 
 * @throws PinError if pin is not configured as OUTPUT via pinMode()
 * @throws InvalidPinError if pin number is out of range (0-27)
 * @throws std::invalid_argument if frequency is 0 or > 65535
 * 
 * @note Call pinMode(pin, OUTPUT) before using this function
 * @note Uses software PWM (50% duty cycle square wave)
 * @note Only one tone can play per pin at a time
 * @note Call noTone() to stop continuous tones
 * @note Frequency range: 31Hz to 65535Hz (human hearing: ~20Hz-20kHz)
 * 
 * @warning ⚠️ CPU USAGE: Runs busy-loop thread consuming ~10-30% CPU while tone plays
 * @warning Same as analogWrite() - uses software PWM for square wave generation
 * @warning For continuous tones or music, consider hardware PWM or external audio module
 * 
 * @example
 * // Simple beep
 * tone(17, 1000);      // 1kHz tone continuously
 * delay(500);          // Play for 500ms
 * noTone(17);          // Stop
 * 
 * @example
 * // Play melody
 * tone(17, 262);  // C4
 * delay(500);
 * tone(17, 294);  // D4
 * delay(500);
 * tone(17, 330);  // E4
 * delay(500);
 * noTone(17);
 * 
 * @example
 * // With duration parameter
 * tone(17, 1000, 500);  // 1kHz for 500ms (blocking)
 */
void tone(int pin, unsigned int frequency, unsigned long duration = 0);

/**
 * @brief Stop tone generation on a pin (Arduino-inspired)
 * 
 * Stops the tone (square wave) being generated by tone().
 * 
 * @param pin GPIO pin number
 * 
 * @note Pin remains in OUTPUT mode but goes LOW
 * @note Safe to call even if no tone is playing
 * 
 * @example
 * tone(17, 1000);   // Start tone
 * delay(2000);      // Play for 2 seconds
 * noTone(17);       // Stop tone
 */
void noTone(int pin);