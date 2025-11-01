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

// Arduino-style constants
#define HIGH true
#define LOW false

// Arduino pin modes
enum ArduinoPinMode {
    INPUT = 0,
    OUTPUT = 1,
    INPUT_PULLUP = 2
};

// Arduino-style global functions for familiar API
/**
 * @brief Set pin mode (Arduino-style function)
 * 
 * @param pin GPIO pin number
 * @param mode INPUT (0) or OUTPUT (1)
 */
void pinMode(int pin, int mode);

/**
 * @brief Write digital value to pin (Arduino-style function)
 * 
 * @param pin GPIO pin number
 * @param value HIGH (true) or LOW (false)
 */
void digitalWrite(int pin, bool value);

/**
 * @brief Read digital value from pin (Arduino-style function)
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
/* ------------------------------------------------------------ */

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