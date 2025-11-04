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

#pragma once
#include "pin.hpp"

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
 * @note This is software PWM and has timing jitter
 * @note Not suitable for servo control (use hardware PWM for servos)
 * @note Each PWM pin uses a dedicated thread
 * 
 * @warning Software PWM is CPU-intensive. Avoid using too many PWM outputs.
 * @warning PWM accuracy decreases under high system load
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