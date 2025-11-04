# PiPinPP API Reference

**Version**: 0.3.0  
**Date**: November 3, 2025

Complete API documentation for PiPinPP - A modern C++ GPIO library for Raspberry Pi with full Arduino compatibility, interrupts, PWM, and professional tooling.

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Core Pin Class](#core-pin-class)
3. [Arduino Compatibility Layer](#arduino-compatibility-layer)
   - [Digital I/O Functions](#digital-io-functions)
   - [Pin State Query Functions](#pin-state-query-functions)
   - [Interrupt Functions](#interrupt-functions)
   - [PWM Functions](#pwm-functions)
   - [Timing Functions](#timing-functions)
   - [Math Functions](#math-functions)
4. [Examples](#examples)
5. [Error Handling](#error-handling)
6. [Hardware Notes](#hardware-notes)

---

## Quick Start

### Installation
```bash
git clone https://github.com/YourUsername/PiPinPP.git
cd PiPinPP
mkdir build && cd build
cmake ..
make
sudo make install
```

### Basic Usage
```cpp
#include "pin.hpp"

// Object-oriented approach
Pin led(17, PinDirection::OUTPUT);
led.write(true);   // LED on
led.write(false);  // LED off

// Arduino-style approach  
#include "ArduinoCompat.hpp"
pinMode(17, OUTPUT);
digitalWrite(17, HIGH);
```

---

## Core Pin Class

The `Pin` class provides object-oriented GPIO control with modern C++ features.

### Enums

#### `PinDirection`
```cpp
enum class PinDirection { 
    INPUT,     // Input without pull resistor
    OUTPUT     // Output mode
};
```

#### `PinMode`
```cpp
enum class PinMode { 
    INPUT,              // Input without pull resistor
    OUTPUT,             // Output mode
    INPUT_PULLUP,       // Input with internal pull-up resistor
    INPUT_PULLDOWN      // Input with internal pull-down resistor
};
```

### Constructors

#### `Pin(int pin, PinDirection direction, const std::string& chipname = "gpiochip0")`
Create a pin with basic direction setting.

**Parameters:**
- `pin`: GPIO pin number (0-27 for Raspberry Pi)
- `direction`: `PinDirection::INPUT` or `PinDirection::OUTPUT`
- `chipname`: GPIO chip name (default: "gpiochip0")

**Throws:**
- `std::invalid_argument`: Invalid pin number
- `std::runtime_error`: GPIO access failure

**Example:**
```cpp
Pin led(17, PinDirection::OUTPUT);
Pin button(18, PinDirection::INPUT);
```

#### `Pin(int pin, PinMode mode, const std::string& chipname = "gpiochip0")`
Create a pin with advanced mode including pull resistors.

**Parameters:**
- `pin`: GPIO pin number (0-27 for Raspberry Pi)
- `mode`: One of `PinMode` values
- `chipname`: GPIO chip name (default: "gpiochip0")

**Throws:**
- `std::invalid_argument`: Invalid pin number
- `std::runtime_error`: GPIO access failure

**Example:**
```cpp
Pin button(18, PinMode::INPUT_PULLUP);    // Built-in pull-up
Pin sensor(19, PinMode::INPUT_PULLDOWN);  // Built-in pull-down
```

### Methods

#### `bool write(bool value)`
Set output pin state.

**Parameters:**
- `value`: `true` for HIGH (3.3V), `false` for LOW (0V)

**Returns:**
- `true`: Success
- `false`: Failure (pin not configured as output, hardware error)

**Example:**
```cpp
Pin led(17, PinDirection::OUTPUT);
led.write(true);   // Turn on
led.write(false);  // Turn off
```

#### `int read()`
Read input pin state.

**Returns:**
- `1`: Pin is HIGH (3.3V)
- `0`: Pin is LOW (0V)  
- `-1`: Error (pin not initialized, hardware failure)

**Example:**
```cpp
Pin button(18, PinMode::INPUT_PULLUP);
int state = button.read();
if (state == 0) {
    std::cout << "Button pressed!" << std::endl;
}
```

---

## Arduino Compatibility Layer

The Arduino compatibility layer provides familiar functions for Arduino users.

### Constants

```cpp
#define HIGH true
#define LOW false

enum ArduinoPinMode {
    INPUT = 0,
    OUTPUT = 1,
    INPUT_PULLUP = 2
};
```

### Digital I/O Functions

#### `void pinMode(int pin, int mode)`
Configure pin mode (Arduino-style).

**Parameters:**
- `pin`: GPIO pin number (0-27)
- `mode`: `INPUT`, `OUTPUT`, or `INPUT_PULLUP`

**Example:**
```cpp
pinMode(17, OUTPUT);        // LED output
pinMode(18, INPUT_PULLUP);  // Button with pull-up
```

#### `void digitalWrite(int pin, bool value)`
Write to output pin (Arduino-style).

**Parameters:**
- `pin`: GPIO pin number
- `value`: `HIGH` (true) or `LOW` (false)

**Example:**
```cpp
digitalWrite(17, HIGH);  // Turn LED on
digitalWrite(17, LOW);   // Turn LED off
```

#### `int digitalRead(int pin)`
Read from input pin (Arduino-style).

**Parameters:**
- `pin`: GPIO pin number

**Returns:**
- `1`: HIGH state
- `0`: LOW state
- `-1`: Error

**Example:**
```cpp
int buttonState = digitalRead(18);
if (buttonState == LOW) {
    // Button pressed (with pull-up)
}
```

### Interrupt Functions

#### `void attachInterrupt(int pin, InterruptCallback callback, InterruptMode mode)`
Attach an interrupt handler to a GPIO pin for edge detection.

**Parameters:**
- `pin`: GPIO pin number (0-27)
- `callback`: Function to call when interrupt triggers (signature: `void callback()`)
- `mode`: Interrupt trigger mode:
  - `InterruptMode::RISING` - Trigger on LOW to HIGH transition
  - `InterruptMode::FALLING` - Trigger on HIGH to LOW transition
  - `InterruptMode::CHANGE` - Trigger on any state change (both edges)

**Notes:**
- Uses libgpiod v2 edge event API for hardware edge detection
- Callbacks are invoked in a background monitoring thread
- Multiple interrupts can be attached to different pins simultaneously
- Pin is automatically configured for input with edge detection
- Thread-safe: callbacks are protected by mutex

**Example:**
```cpp
#include "interrupts.hpp"
#include <atomic>

std::atomic<int> buttonPresses{0};

void buttonPressed() {
    buttonPresses++;
    std::cout << "Button pressed! Count: " << buttonPresses << std::endl;
}

int main() {
    // Attach interrupt to GPIO 18, trigger on falling edge (button press)
    attachInterrupt(18, buttonPressed, InterruptMode::FALLING);
    
    // Program continues, interrupts handled in background
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}
```

#### `void detachInterrupt(int pin)`
Remove interrupt handler from a GPIO pin.

**Parameters:**
- `pin`: GPIO pin number to detach

**Notes:**
- Stops monitoring the specified pin for edge events
- Safe to call even if no interrupt is attached
- Automatically called for all pins on program exit

**Example:**
```cpp
attachInterrupt(18, buttonCallback, InterruptMode::FALLING);
// ... later ...
detachInterrupt(18);  // Stop monitoring pin 18
```

### PWM Functions

#### `void analogWrite(int pin, int value)`
Generate PWM output on a GPIO pin (Arduino-compatible).

**Parameters:**
- `pin`: GPIO pin number (0-27)
- `value`: Duty cycle (0-255)
  - `0` = Always LOW (0% duty cycle, LED off)
  - `127` = 50% duty cycle (half brightness)
  - `255` = Always HIGH (100% duty cycle, LED fully on)

**Notes:**
- Software PWM implementation using dedicated per-pin threads
- Default frequency: 490Hz (matches Arduino UNO)
- Pin is automatically configured as OUTPUT
- Multiple pins can have independent PWM outputs
- Timing has jitter (not suitable for precise applications like servos)
- CPU usage increases with number of active PWM pins
- Edge cases optimized: 0 and 255 avoid PWM overhead

**Example:**
```cpp
#include "pwm.hpp"

int main() {
    // Fade LED from off to full brightness
    for (int brightness = 0; brightness <= 255; brightness++) {
        analogWrite(17, brightness);
        delay(10);  // Smooth transition
    }
    
    // Fade back to off
    for (int brightness = 255; brightness >= 0; brightness--) {
        analogWrite(17, brightness);
        delay(10);
    }
    
    return 0;
}
```

#### `void stopPWM(int pin)`
Stop PWM output on a pin and clean up resources.

**Parameters:**
- `pin`: GPIO pin number

**Notes:**
- Stops the PWM thread for the specified pin
- Pin remains in its last state (HIGH or LOW)
- Automatically called on program exit for all active PWM pins
- Safe to call multiple times

**Example:**
```cpp
analogWrite(17, 128);  // 50% brightness
delay(5000);           // Run for 5 seconds
stopPWM(17);           // Stop PWM, free resources
```

### Timing Functions

#### `unsigned long millis()`
Returns milliseconds since program start.

**Returns:**
- Milliseconds elapsed since program started

**Notes:**
- Uses monotonic clock (won't jump if system time changes)
- Resets to 0 at program start
- Overflows after ~49 days

**Example:**
```cpp
unsigned long startTime = millis();
// ... do something ...
unsigned long elapsed = millis() - startTime;
std::cout << "Operation took " << elapsed << " ms" << std::endl;
```

#### `unsigned long micros()`
Returns microseconds since program start.

**Returns:**
- Microseconds elapsed since program started

**Notes:**
- Uses monotonic clock for precision timing
- Resets to 0 at program start
- Overflows after ~71 minutes

**Example:**
```cpp
unsigned long start = micros();
// ... time-critical code ...
unsigned long duration = micros() - start;
std::cout << "Execution time: " << duration << " µs" << std::endl;
```

#### `void delay(unsigned long ms)`
Delay execution in milliseconds (Arduino-style).

**Parameters:**
- `ms`: Delay time in milliseconds

**Notes:**
- Yields CPU during delay (efficient)
- Not suitable for microsecond precision

**Example:**
```cpp
digitalWrite(17, HIGH);
delay(1000);              // Wait 1 second
digitalWrite(17, LOW);
```

#### `void delayMicroseconds(unsigned int us)`
Delay execution in microseconds with high precision.

**Parameters:**
- `us`: Delay time in microseconds

**Notes:**
- Uses busy-waiting for precision (consumes CPU)
- Accurate to within 1-2 microseconds
- Use `delay()` for millisecond delays (more efficient)

**Example:**
```cpp
digitalWrite(17, HIGH);
delayMicroseconds(100);   // Wait 100 microseconds
digitalWrite(17, LOW);
```

### Pin State Query Functions

#### `bool isOutput(int pin)`
Check if a pin is configured as OUTPUT.

**Parameters:**
- `pin`: GPIO pin number

**Returns:**
- `true` if pin is OUTPUT
- `false` if pin is INPUT, INPUT_PULLUP, or INPUT_PULLDOWN

**Throws:**
- `PinError`: If pin not initialized (pinMode not called)

**Example:**
```cpp
pinMode(17, OUTPUT);
if (isOutput(17)) {
    digitalToggle(17);  // Safe to toggle
}
```

#### `bool isInput(int pin)`
Check if a pin is configured as any INPUT variant.

**Parameters:**
- `pin`: GPIO pin number

**Returns:**
- `true` if pin is INPUT, INPUT_PULLUP, or INPUT_PULLDOWN
- `false` if pin is OUTPUT

**Throws:**
- `PinError`: If pin not initialized (pinMode not called)

**Example:**
```cpp
pinMode(18, INPUT_PULLUP);
if (isInput(18)) {
    int state = digitalRead(18);
}
```

#### `ArduinoPinMode getMode(int pin)`
Get the current mode of a pin.

**Parameters:**
- `pin`: GPIO pin number

**Returns:**
- `ArduinoPinMode` enum value:
  - `INPUT` (0)
  - `OUTPUT` (1)
  - `INPUT_PULLUP` (2)
  - `INPUT_PULLDOWN` (3)

**Throws:**
- `PinError`: If pin not initialized (pinMode not called)

**Example:**
```cpp
pinMode(17, OUTPUT);
ArduinoPinMode mode = getMode(17);
if (mode == OUTPUT) {
    digitalWrite(17, HIGH);
}
```

#### `void digitalToggle(int pin)`
Toggle an OUTPUT pin between HIGH and LOW states.

**Parameters:**
- `pin`: GPIO pin number

**Throws:**
- `PinError`: If pin not initialized or not in OUTPUT mode
- `GpioAccessError`: If write to GPIO fails

**Notes:**
- Much more efficient than `digitalRead()` + `digitalWrite()`
- Internally tracks last written value, no hardware read needed
- Thread-safe with mutex protection
- Only works on OUTPUT pins

**Example:**
```cpp
pinMode(17, OUTPUT);
digitalWrite(17, LOW);   // Start LOW
digitalToggle(17);       // Now HIGH
digitalToggle(17);       // Now LOW

// LED blink using toggle
while (true) {
    digitalToggle(17);
    delay(500);
}
```

### Math Functions

#### `int constrain(int x, int min, int max)`
Constrain value between limits.

**Parameters:**
- `x`: Input value
- `min`: Minimum limit
- `max`: Maximum limit

**Returns:**
- Constrained value

**Example:**
```cpp
int safe_value = constrain(sensor_reading, 0, 100);
```

#### `long map(long x, long in_min, long in_max, long out_min, long out_max)`
Map value from one range to another.

**Parameters:**
- `x`: Input value
- `in_min`, `in_max`: Input range
- `out_min`, `out_max`: Output range

**Returns:**
- Mapped value

**Example:**
```cpp
// Map 0-1023 sensor reading to 0-255 PWM value
int pwm_val = map(sensor_val, 0, 1023, 0, 255);
```

#### `T sq(T x)` (template)
Calculate the square of a number.

**Template Parameters:**
- `T`: Any numeric type (int, long, float, double, etc.)

**Parameters:**
- `x`: Value to square

**Returns:**
- Square of x (x²)

**Notes:**
- More efficient than `pow(x, 2)`
- Works with any numeric type

**Example:**
```cpp
int a = sq(5);        // Returns 25
float b = sq(3.5f);   // Returns 12.25
double c = sq(-4.0);  // Returns 16.0
```

#### `double sqrt(double x)`
Calculate the square root of a number.

**Parameters:**
- `x`: Value to take square root of (must be non-negative)

**Returns:**
- Square root of x (√x)

**Notes:**
- Returns NaN if x is negative
- Wrapper for `std::sqrt()`

**Example:**
```cpp
double a = sqrt(16.0);   // Returns 4.0
double b = sqrt(2.0);    // Returns 1.414213...
double c = sqrt(0.25);   // Returns 0.5
```

#### `double pow(double base, double exponent)`
Calculate base raised to the power of exponent.

**Parameters:**
- `base`: Base value
- `exponent`: Power to raise base to

**Returns:**
- Result of base^exponent

**Notes:**
- Wrapper for `std::pow()`

**Example:**
```cpp
double a = pow(2.0, 3.0);    // Returns 8.0 (2³)
double b = pow(10.0, -2.0);  // Returns 0.01 (10⁻²)
double c = pow(4.0, 0.5);    // Returns 2.0 (√4)
```

#### `T max(T a, T b)` (template)
Return the maximum of two values.

**Template Parameters:**
- `T`: Any comparable type (int, long, float, double, etc.)

**Parameters:**
- `a`: First value
- `b`: Second value

**Returns:**
- The larger of a and b

**Example:**
```cpp
int a = max(10, 20);      // Returns 20
float b = max(3.5f, 2.1f); // Returns 3.5
long c = max(-5L, -10L);  // Returns -5
```

#### `T min(T a, T b)` (template)
Return the minimum of two values.

**Template Parameters:**
- `T`: Any comparable type (int, long, float, double, etc.)

**Parameters:**
- `a`: First value
- `b`: Second value

**Returns:**
- The smaller of a and b

**Example:**
```cpp
int a = min(10, 20);      // Returns 10
float b = min(3.5f, 2.1f); // Returns 2.1
long c = min(-5L, -10L);  // Returns -10
```

---

## Exception Handling

PiPinPP provides a hierarchy of custom exceptions for better error handling and debugging. All exceptions inherit from `std::runtime_error` through the `PinError` base class.

### Exception Hierarchy

```
std::runtime_error
    └── PinError (base exception for all GPIO errors)
        ├── InvalidPinError (invalid pin numbers or configurations)
        └── GpioAccessError (hardware access failures)
```

### Exception Classes

#### `PinError`
Base class for all PiPinPP exceptions. Catch this to handle any library error generically.

**Constructor:**
```cpp
explicit PinError(const std::string& message)
```

**Example:**
```cpp
try {
    Pin led(17, PinDirection::OUTPUT);
} catch (const PinError& e) {
    std::cerr << "GPIO error: " << e.what() << std::endl;
}
```

#### `InvalidPinError`
Thrown when an invalid pin number or configuration is used.

**Constructors:**
```cpp
explicit InvalidPinError(const std::string& message)
InvalidPinError(int pinNumber, const std::string& reason)
```

**When thrown:**
- Pin number exceeds valid range (e.g., pin 999 on Raspberry Pi with 58 lines)
- Pin not initialized before digitalWrite/digitalRead
- Invalid pin configuration

**Examples:**
```cpp
// Example 1: Invalid pin number
try {
    Pin invalid(999, PinDirection::OUTPUT);
} catch (const InvalidPinError& e) {
    // e.what() might be: "Invalid pin 999: Pin number exceeds available GPIO lines (58)"
    std::cerr << e.what() << std::endl;
}

// Example 2: Arduino function without pinMode
try {
    digitalWrite(17, HIGH);  // No pinMode() called first
} catch (const InvalidPinError& e) {
    // e.what() might be: "Invalid pin 17: Pin not initialized. Call pinMode() first."
    std::cerr << e.what() << std::endl;
}
```

#### `GpioAccessError`
Thrown when hardware access fails (chip not found, permission denied, etc.).

**Constructors:**
```cpp
explicit GpioAccessError(const std::string& message)
GpioAccessError(const std::string& device, const std::string& reason)
```

**When thrown:**
- Cannot open GPIO chip device
- Cannot create GPIO line settings/config
- Cannot request GPIO lines
- Write/read operations fail

**Examples:**
```cpp
// Example 1: Invalid device path
try {
    Pin led(17, PinDirection::OUTPUT, "/dev/nonexistent");
} catch (const GpioAccessError& e) {
    // e.what() might be: "GPIO access error on /dev/nonexistent: Failed to open chip"
    std::cerr << e.what() << std::endl;
}

// Example 2: Permission denied
try {
    Pin led(17, PinDirection::OUTPUT);
} catch (const GpioAccessError& e) {
    // If user not in 'gpio' group
    std::cerr << "Check permissions: " << e.what() << std::endl;
}
```

### Exception Handling Best Practices

#### 1. Catch Specific Exceptions
```cpp
try {
    pinMode(17, OUTPUT);
    digitalWrite(17, HIGH);
} catch (const InvalidPinError& e) {
    // Handle configuration errors
    std::cerr << "Pin configuration error: " << e.what() << std::endl;
} catch (const GpioAccessError& e) {
    // Handle hardware access errors
    std::cerr << "Hardware access error: " << e.what() << std::endl;
}
```

#### 2. Catch Base Exception for Generic Handling
```cpp
try {
    Pin led(17, PinDirection::OUTPUT);
    led.write(true);
} catch (const PinError& e) {
    // Handle any PiPinPP error
    std::cerr << "GPIO operation failed: " << e.what() << std::endl;
    return 1;
}
```

#### 3. Initialization Pattern
```cpp
std::unique_ptr<Pin> led;
try {
    led = std::make_unique<Pin>(17, PinDirection::OUTPUT);
} catch (const InvalidPinError& e) {
    std::cerr << "Invalid pin configuration: " << e.what() << std::endl;
    return 1;
} catch (const GpioAccessError& e) {
    std::cerr << "Cannot access GPIO hardware: " << e.what() << std::endl;
    std::cerr << "Check that you're in the 'gpio' group" << std::endl;
    return 1;
}
```

### Common Error Causes and Solutions

| Exception | Common Cause | Solution |
|-----------|--------------|----------|
| `InvalidPinError` | Pin number too high | Check your board's GPIO pin range (0-27 for most RPi models) |
| `InvalidPinError` | Arduino function without pinMode | Always call `pinMode()` before `digitalWrite()`/`digitalRead()` |
| `GpioAccessError` | Permission denied | Add user to 'gpio' group: `sudo usermod -a -G gpio $USER` |
| `GpioAccessError` | Device not found | Verify `/dev/gpiochip0` exists, check kernel module loaded |
| `GpioAccessError` | Line already in use | Another process is using the GPIO pin |

---

## Examples

### Blink LED (Object-Oriented)
```cpp
#include "pin.hpp"
#include <thread>
#include <chrono>

int main() {
    Pin led(17, PinDirection::OUTPUT);
    
    for (int i = 0; i < 10; ++i) {
        led.write(true);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        led.write(false);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    return 0;
}
```

### Button Input (Arduino-Style)
```cpp
#include "ArduinoCompat.hpp"

void setup() {
    pinMode(17, OUTPUT);        // LED
    pinMode(18, INPUT_PULLUP);  // Button
}

void loop() {
    if (digitalRead(18) == LOW) {   // Button pressed
        digitalWrite(17, HIGH);     // LED on
    } else {
        digitalWrite(17, LOW);      // LED off
    }
    delay(50);  // Debounce delay
}

int main() {
    setup();
    while (true) {
        loop();
    }
    return 0;
}
```

### Advanced Pull Resistor Usage
```cpp
#include "pin.hpp"

int main() {
    // Button with internal pull-up (no external resistor needed)
    Pin button(18, PinMode::INPUT_PULLUP);
    
    // Sensor with pull-down (for active-high signals)
    Pin sensor(19, PinMode::INPUT_PULLDOWN);
    
    while (true) {
        if (button.read() == 0) {      // Button pressed (LOW)
            std::cout << "Button pressed!" << std::endl;
        }
        
        if (sensor.read() == 1) {      // Sensor active (HIGH)
            std::cout << "Sensor triggered!" << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}
```

### Timing Functions Usage
```cpp
#include "ArduinoCompat.hpp"
#include <iostream>

int main() {
    pinMode(17, OUTPUT);
    
    std::cout << "Starting timing demo..." << std::endl;
    unsigned long startTime = millis();
    
    for (int i = 0; i < 5; i++) {
        // Blink LED with precise timing
        digitalWrite(17, HIGH);
        delay(500);              // 500ms on
        digitalWrite(17, LOW);
        delay(500);              // 500ms off
        
        std::cout << "Blink " << (i + 1) 
                  << " at " << millis() << " ms" << std::endl;
    }
    
    unsigned long totalTime = millis() - startTime;
    std::cout << "Total time: " << totalTime << " ms" << std::endl;
    
    // Microsecond precision for fast signals
    unsigned long pulseStart = micros();
    digitalWrite(17, HIGH);
    delayMicroseconds(100);     // 100µs pulse
    digitalWrite(17, LOW);
    unsigned long pulseWidth = micros() - pulseStart;
    
    std::cout << "Pulse width: " << pulseWidth << " µs" << std::endl;
    
    return 0;
}
```

### Button Interrupt with Debouncing
```cpp
#include "interrupts.hpp"
#include "ArduinoCompat.hpp"
#include <atomic>
#include <csignal>

std::atomic<bool> running{true};
std::atomic<int> pressCount{0};
unsigned long lastPress = 0;
const unsigned long DEBOUNCE_MS = 200;

void buttonInterrupt() {
    unsigned long now = millis();
    
    // Software debouncing
    if (now - lastPress > DEBOUNCE_MS) {
        pressCount++;
        lastPress = now;
        std::cout << "Button pressed! Total: " << pressCount << std::endl;
    }
}

void signalHandler(int signal) {
    running = false;
}

int main() {
    signal(SIGINT, signalHandler);
    
    // GPIO 18 with pull-up resistor, button to ground
    pinMode(18, INPUT_PULLUP);
    
    // Attach interrupt on falling edge (button press)
    attachInterrupt(18, buttonInterrupt, InterruptMode::FALLING);
    
    std::cout << "Press button on GPIO 18 (Ctrl+C to exit)" << std::endl;
    
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    detachInterrupt(18);
    std::cout << "Total button presses: " << pressCount << std::endl;
    
    return 0;
}
```

### LED Breathing Effect with PWM
```cpp
#include "pwm.hpp"
#include "ArduinoCompat.hpp"
#include <cmath>

int main() {
    const int LED_PIN = 17;
    
    std::cout << "LED Breathing Effect Demo" << std::endl;
    
    // Breathing pattern: smooth sine wave
    for (int cycle = 0; cycle < 5; cycle++) {
        for (int i = 0; i < 360; i++) {
            // Calculate brightness using sine wave (0-255)
            double radians = i * M_PI / 180.0;
            int brightness = (int)((sin(radians) + 1.0) * 127.5);
            
            analogWrite(LED_PIN, brightness);
            delayMicroseconds(5000);  // 5ms per step = ~1.8s per cycle
        }
    }
    
    // Fade out
    for (int brightness = 255; brightness >= 0; brightness--) {
        analogWrite(LED_PIN, brightness);
        delay(5);
    }
    
    stopPWM(LED_PIN);
    std::cout << "Demo complete!" << std::endl;
    
    return 0;
}
```

### Multiple PWM Outputs (RGB LED)
```cpp
#include "pwm.hpp"
#include "ArduinoCompat.hpp"

void setColor(int red, int green, int blue) {
    analogWrite(17, red);
    analogWrite(27, green);
    analogWrite(22, blue);
}

int main() {
    std::cout << "RGB LED Color Demo" << std::endl;
    
    // Red
    setColor(255, 0, 0);
    delay(1000);
    
    // Green
    setColor(0, 255, 0);
    delay(1000);
    
    // Blue
    setColor(0, 0, 255);
    delay(1000);
    
    // Yellow (red + green)
    setColor(255, 255, 0);
    delay(1000);
    
    // Magenta (red + blue)
    setColor(255, 0, 255);
    delay(1000);
    
    // Cyan (green + blue)
    setColor(0, 255, 255);
    delay(1000);
    
    // White (all colors)
    setColor(255, 255, 255);
    delay(1000);
    
    // Smooth rainbow cycle
    for (int hue = 0; hue < 360; hue += 2) {
        // HSV to RGB conversion for smooth color transitions
        float h = hue / 60.0f;
        float x = (1 - fabs(fmod(h, 2) - 1));
        
        int r, g, b;
        if (h < 1) { r = 255; g = x * 255; b = 0; }
        else if (h < 2) { r = x * 255; g = 255; b = 0; }
        else if (h < 3) { r = 0; g = 255; b = x * 255; }
        else if (h < 4) { r = 0; g = x * 255; b = 255; }
        else if (h < 5) { r = x * 255; g = 0; b = 255; }
        else { r = 255; g = 0; b = x * 255; }
        
        setColor(r, g, b);
        delay(20);
    }
    
    // Off
    setColor(0, 0, 0);
    
    return 0;
}
```

---

## Error Handling

### Exception Types

PiPinPP uses standard C++ exceptions:

- `std::invalid_argument`: Invalid pin numbers, invalid parameters
- `std::runtime_error`: Hardware access failures, GPIO errors

### Error Handling Example
```cpp
try {
    Pin led(17, PinDirection::OUTPUT);
    
    if (!led.write(true)) {
        std::cerr << "Failed to turn LED on" << std::endl;
    }
    
} catch (const std::invalid_argument& e) {
    std::cerr << "Invalid pin configuration: " << e.what() << std::endl;
} catch (const std::runtime_error& e) {
    std::cerr << "Hardware error: " << e.what() << std::endl;
    std::cerr << "Make sure to run with sudo" << std::endl;
}
```

### Common Error Solutions

1. **Permission Denied**: Run with `sudo`
2. **Invalid Pin Number**: Use pins 0-27 for Raspberry Pi
3. **Pin Already in Use**: Make sure no other process is using the pin
4. **Hardware Failure**: Check wiring and connections

---

## Hardware Notes

### Pin Numbering
PiPinPP uses BCM GPIO pin numbering (not physical pin numbers):

- **Valid Range**: 0-27
- **Special Pins**: 
  - 0,1: I2C (SDA, SCL)
  - 14,15: UART (TXD, RXD)
  - 2,3: I2C (SDA, SCL) on newer models

### Electrical Specifications
- **Logic Levels**: 3.3V (HIGH), 0V (LOW)
- **Current Limits**: ~16mA per pin
- **Input Voltage**: 0-3.3V (NOT 5V tolerant)

### Pull Resistors
- **Internal Pull-up/Pull-down**: ~50kΩ
- **External Resistors**: 10kΩ typical for buttons
- **Floating Inputs**: Unreliable without pull resistors

### Hardware Setup Examples

#### LED Connection
```
GPIO Pin 17 ----[220Ω]----[LED+]----[LED-]----GND
```

#### Button Connection (with internal pull-up)
```
GPIO Pin 18 ----[Button]----GND
(No external resistor needed with INPUT_PULLUP)
```

#### Button Connection (with external pull-up)
```
3.3V ----[10kΩ]----GPIO Pin 18 ----[Button]----GND
```

---

## Dependencies

- **libgpiod**: Linux GPIO access library
- **C++17**: Modern C++ standard
- **CMake**: Build system (3.16+)
- **Raspberry Pi OS**: Or compatible Linux distribution

---

## License

MIT License - See LICENSE file for details.

---

*For more examples and tutorials, visit the [PiPinPP GitHub repository](https://github.com/YourUsername/PiPinPP).*