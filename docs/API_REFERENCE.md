# PiPinPP API Reference

**Version**: 0.2.0  
**Date**: October 31, 2025

Complete API documentation for PiPinPP - A modern C++ GPIO library for Raspberry Pi with Arduino compatibility.

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Core Pin Class](#core-pin-class)
3. [Arduino Compatibility Layer](#arduino-compatibility-layer)
   - [Digital I/O Functions](#digital-io-functions)
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