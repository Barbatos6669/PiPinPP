# PiPinPP API Reference

**Version**: 0.3.10  
**Date**: November 9, 2025

Complete API documentation for PiPinPP - A modern C++ GPIO library for Raspberry Pi with full Arduino-inspired API, interrupts, PWM, and professional tooling.

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
   - [Trigonometry Constants](#trigonometry-constants)
   - [Advanced I/O Functions](#advanced-io-functions)
4. [Communication Protocols](#communication-protocols)
   - [Wire (I²C)](#wire-i2c)
   - [SPI](#spi)
5. [Examples](#examples)
6. [Error Handling](#error-handling)
7. [Hardware Notes](#hardware-notes)

---

## Quick Start

**Installation:** See [INSTALL.md](INSTALL.md) for complete installation instructions.

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

The Arduino-inspired API layer provides familiar functions for Arduino users.

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
Generate PWM output on a GPIO pin (Arduino-inspired).

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
Calculate the square of a number (Arduino-specific function).

**Template Parameters:**
- `T`: Any numeric type (int, long, float, double, etc.)

**Parameters:**
- `x`: Value to square

**Returns:**
- Square of x (x²)

**Notes:**
- More efficient than `std::pow(x, 2)`
- Works with any numeric type
- This is the only math function provided by PiPinPP

**Example:**
```cpp
int a = sq(5);        // Returns 25
float b = sq(3.5f);   // Returns 12.25
double c = sq(-4.0);  // Returns 16.0
```

**For other math functions, use the standard library:**

- **Square root**: Use `std::sqrt()` from `<cmath>`
  ```cpp
  #include <cmath>
  double result = std::sqrt(16.0);  // Returns 4.0
  ```

- **Power**: Use `std::pow()` from `<cmath>`
  ```cpp
  #include <cmath>
  double result = std::pow(2.0, 3.0);  // Returns 8.0
  ```

- **Maximum**: Use `std::max()` from `<algorithm>`
  ```cpp
  #include <algorithm>
  int result = std::max(10, 20);  // Returns 20
  ```

- **Minimum**: Use `std::min()` from `<algorithm>`
  ```cpp
  #include <algorithm>
  int result = std::min(10, 20);  // Returns 10
  ```

**Why not provide wrappers?**  
Creating our own `sqrt()`, `pow()`, `max()`, `min()` functions causes naming conflicts 
when users write `using namespace std;` in their code. The standard library functions 
are already widely available and well-optimized.

### Trigonometry Constants

PiPinPP provides Arduino-inspired angle conversion constants for use with standard library trigonometric functions.

#### `DEG_TO_RAD`
Constant for converting degrees to radians.

**Value**: `0.017453292519943295769236907684886` (π/180)

**Usage:**
Multiply an angle in degrees by `DEG_TO_RAD` to convert to radians for use with `std::sin()`, `std::cos()`, `std::tan()`.

**Example:**
```cpp
#include <cmath>
#include <ArduinoCompat.hpp>

double angleDegrees = 45.0;
double angleRadians = angleDegrees * DEG_TO_RAD;
double sineValue = std::sin(angleRadians);  // sin(45°) ≈ 0.707
```

#### `RAD_TO_DEG`
Constant for converting radians to degrees.

**Value**: `57.295779513082320876798154814105` (180/π)

**Usage:**
Multiply an angle in radians by `RAD_TO_DEG` to convert to degrees.

**Example:**
```cpp
#include <cmath>
#include <ArduinoCompat.hpp>

double angleRadians = M_PI / 2;  // 90 degrees in radians
double angleDegrees = angleRadians * RAD_TO_DEG;  // Returns 90.0
```

**Trigonometric Functions:**

For trigonometric calculations, use the standard library functions from `<cmath>`:

- **Sine**: Use `std::sin(radians)` from `<cmath>`
  ```cpp
  #include <cmath>
  double result = std::sin(45.0 * DEG_TO_RAD);  // sin(45°) ≈ 0.707
  ```

- **Cosine**: Use `std::cos(radians)` from `<cmath>`
  ```cpp
  #include <cmath>
  double result = std::cos(60.0 * DEG_TO_RAD);  // cos(60°) = 0.5
  ```

- **Tangent**: Use `std::tan(radians)` from `<cmath>`
  ```cpp
  #include <cmath>
  double result = std::tan(45.0 * DEG_TO_RAD);  // tan(45°) ≈ 1.0
  ```

- **Arc Tangent (2-argument)**: Use `std::atan2(y, x)` for angle calculations
  ```cpp
  #include <cmath>
  double angle = std::atan2(10.0, 10.0) * RAD_TO_DEG;  // Returns 45.0°
  ```

**Practical Applications:**

**Circular Motion:**
```cpp
// Calculate position on a circle
double angleDeg = 30.0;
double radius = 10.0;
double x = radius * std::cos(angleDeg * DEG_TO_RAD);
double y = radius * std::sin(angleDeg * DEG_TO_RAD);
```

**LED Breathing Effect (Sine Wave):**
```cpp
// Generate smooth breathing pattern
for (int angle = 0; angle < 360; angle++) {
    double rad = angle * DEG_TO_RAD;
    int brightness = 127.5 + 127.5 * std::sin(rad);  // 0-255 range
    analogWrite(ledPin, brightness);
    delay(10);
}
```

**Servo Positioning:**
```cpp
// Calculate servo position using angles
double servoAngle = 90.0;  // Center position
int pulseWidth = 1500;     // Center pulse width (1.5ms)

// For angled movement calculations
double armLength = 10.0;   // cm
double height = armLength * std::sin(servoAngle * DEG_TO_RAD);
double reach = armLength * std::cos(servoAngle * DEG_TO_RAD);
```

**Why not provide sin(), cos(), tan() wrappers?**  
Like `sqrt()` and `pow()`, these functions exist in the C++ standard library (`<cmath>`). 
Creating wrappers causes naming conflicts when users write `using namespace std;`. 
The constants `DEG_TO_RAD` and `RAD_TO_DEG` provide Arduino-style angle handling without conflicts.

---

## Advanced I/O Functions

PiPinPP provides Arduino-compatible advanced input/output functions for pulse measurement, shift registers, and tone generation.

### `unsigned long pulseIn(int pin, bool state, unsigned long timeout)`

Measure the duration of a pulse on a pin (Arduino-compatible).

**Parameters:**
- `pin`: GPIO pin number to read
- `state`: Pulse level to measure (`HIGH` or `LOW`)
- `timeout`: Maximum time to wait for pulse in microseconds (default: 1000000 = 1 second)

**Returns:**
- Pulse duration in microseconds, or `0` if no pulse detected within timeout

**Behavior:**
1. Waits for pin to enter opposite state (preparation)
2. Waits for pin to enter desired state (pulse start)
3. Measures time until pin leaves desired state (pulse end)
4. Returns elapsed time in microseconds

**Example:**
```cpp
// Measure HIGH pulse duration (e.g., from ultrasonic sensor)
pinMode(echoPin, INPUT);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

unsigned long duration = pulseIn(echoPin, HIGH, 30000);  // 30ms timeout
if (duration > 0) {
    float distance = (duration / 2.0) * 0.0343;  // Convert to cm
    std::cout << "Distance: " << distance << " cm" << std::endl;
}
```

**Notes:**
- Requires pin to be configured with `pinMode(pin, INPUT)`
- Blocks execution until pulse completes or timeout occurs
- Resolution: ~1-2 microseconds
- Maximum reliable pulse: ~5-10 seconds (implementation dependent)

---

### `unsigned long pulseInLong(int pin, bool state, unsigned long timeout)`

Extended pulse measurement with better precision for longer pulses.

**Parameters:**
- `pin`: GPIO pin number to read
- `state`: Pulse level to measure (`HIGH` or `LOW`)
- `timeout`: Maximum time to wait in microseconds (default: 1000000)

**Returns:**
- Pulse duration in microseconds, or `0` if timeout

**Differences from `pulseIn()`:**
- Better accuracy for pulses > 1 second
- Uses 64-bit timer internally for overflow protection
- Slightly more CPU overhead

**Example:**
```cpp
// Measure long servo pulse (1-2ms typical, but can be longer)
unsigned long servoPulse = pulseInLong(servoPin, HIGH, 3000);
std::cout << "Servo pulse width: " << servoPulse << " μs" << std::endl;
```

---

###shiftOut(int dataPin, int clockPin, uint8_t bitOrder, uint8_t value)`

Shift out a byte to a shift register using software bit-banging (Arduino-compatible).

**Parameters:**
- `dataPin`: GPIO pin connected to shift register data input (DS/SER)
- `clockPin`: GPIO pin connected to shift register clock (SHCP/SRCLK)
- `bitOrder`: `MSBFIRST` or `LSBFIRST` (which bit to send first)
- `value`: 8-bit value to shift out

**Behavior:**
- Shifts out 8 bits, one per clock cycle
- Data is clocked on rising edge
- Blocking operation (~100-300 kHz depending on CPU)

**Example (74HC595 shift register):**
```cpp
const int dataPin = 17;   // DS
const int clockPin = 27;  // SHCP
const int latchPin = 22;  // STCP

pinMode(dataPin, OUTPUT);
pinMode(clockPin, OUTPUT);
pinMode(latchPin, OUTPUT);

// Send LED pattern to shift register
digitalWrite(latchPin, LOW);
shiftOut(dataPin, clockPin, MSBFIRST, 0b10101010);  // Alternating LEDs
digitalWrite(latchPin, HIGH);
```

**Performance:**
- Software implementation: ~200-300 kHz clock speed
- For higher speed (>1 MHz), use hardware `SPI` instead (see [SPI section](#spi))

**Multiple Registers (Daisy Chain):**
```cpp
digitalWrite(latchPin, LOW);
shiftOut(dataPin, clockPin, MSBFIRST, 0xFF);  // First register (far end)
shiftOut(dataPin, clockPin, MSBFIRST, 0x00);  // Second register (near end)
digitalWrite(latchPin, HIGH);
```

---

### `uint8_t shiftIn(int dataPin, int clockPin, uint8_t bitOrder)`

Shift in a byte from a device using software bit-banging (Arduino-compatible).

**Parameters:**
- `dataPin`: GPIO pin connected to device data output
- `clockPin`: GPIO pin connected to clock (generated by Pi)
- `bitOrder`: `MSBFIRST` or `LSBFIRST` (which bit to receive first)

**Returns:**
- 8-bit value read from device

**Example (74HC165 shift register - parallel to serial):**
```cpp
const int dataPin = 17;   // Q7/SER_OUT
const int clockPin = 27;  // CLK
const int loadPin = 22;   // SH/LD

pinMode(dataPin, INPUT);
pinMode(clockPin, OUTPUT);
pinMode(loadPin, OUTPUT);

// Load parallel inputs into shift register
digitalWrite(loadPin, LOW);
delayMicroseconds(5);
digitalWrite(loadPin, HIGH);

// Read 8 bits
uint8_t inputs = shiftIn(dataPin, clockPin, MSBFIRST);
std::cout << "Button states: " << std::bitset<8>(inputs) << std::endl;
```

---

### `void tone(int pin, unsigned int frequency, unsigned long duration)`

Generate a square wave tone on a pin (Arduino-compatible).

**Parameters:**
- `pin`: GPIO pin number for tone output
- `frequency`: Frequency in Hertz (31 Hz to 65535 Hz)
- `duration`: Duration in milliseconds (optional, 0 = continuous)

**Behavior:**
- Generates square wave using software PWM
- Blocks for specified duration if provided
- Non-blocking if duration is 0 (use `noTone()` to stop)

**Example (Simple beep):**
```cpp
pinMode(buzzerPin, OUTPUT);

// 1000 Hz tone for 500ms
tone(buzzerPin, 1000, 500);
delay(500);  // Wait for tone to finish

// Continuous tone (non-blocking)
tone(buzzerPin, 2000);
delay(1000);
noTone(buzzerPin);
```

**Musical Notes Example:**
```cpp
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523

// Play simple melody
int melody[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5};
int noteDurations[] = {250, 250, 250, 500};

for (int i = 0; i < 4; i++) {
    tone(buzzerPin, melody[i], noteDurations[i]);
    delay(noteDurations[i] * 1.3);  // 30% pause between notes
}
```

**Notes:**
- Software-generated tone, not hardware PWM
- Frequency accuracy: ±1-2% (depends on system load)
- Multiple simultaneous tones not supported (last call wins)
- For precise audio, consider hardware PWM or external audio hardware

---

### `void noTone(int pin)`

Stop tone generation on a pin (Arduino-compatible).

**Parameters:**
- `pin`: GPIO pin number to stop

**Example:**
```cpp
tone(buzzerPin, 1000);  // Start continuous tone
delay(2000);
noTone(buzzerPin);      // Stop it
```

---

### Bit Manipulation Functions

PiPinPP provides Arduino-compatible bit manipulation utilities for working with individual bits in bytes and integers.

#### `uint8_t bit(uint8_t n)`

Calculate the value of a specific bit position (returns 2^n).

**Parameters:**
- `n`: Bit position (0-7 for uint8_t)

**Returns:**
- Value with bit `n` set (1 << n)

**Example:**
```cpp
uint8_t mask = bit(3);  // Returns 0b00001000 (8)
```

---

#### `uint8_t bitRead(uint8_t value, uint8_t n)`

Read a specific bit from a value.

**Parameters:**
- `value`: Value to read from
- `n`: Bit position (0 = LSB)

**Returns:**
- `0` or `1`

**Example:**
```cpp
uint8_t data = 0b10110010;
uint8_t bit5 = bitRead(data, 5);  // Returns 1
```

---

#### `void bitWrite(uint8_t& value, uint8_t n, uint8_t bitValue)`

Write a specific bit in a value.

**Parameters:**
- `value`: Reference to value to modify
- `n`: Bit position
- `bitValue`: `0` or `1`

**Example:**
```cpp
uint8_t data = 0b00000000;
bitWrite(data, 3, 1);  // data becomes 0b00001000
```

---

#### `void bitSet(uint8_t& value, uint8_t n)`

Set a specific bit to 1.

**Parameters:**
- `value`: Reference to value to modify
- `n`: Bit position

**Example:**
```cpp
uint8_t flags = 0b00000000;
bitSet(flags, 7);  // flags becomes 0b10000000
```

---

#### `void bitClear(uint8_t& value, uint8_t n)`

Clear a specific bit to 0.

**Parameters:**
- `value`: Reference to value to modify
- `n`: Bit position

**Example:**
```cpp
uint8_t flags = 0b11111111;
bitClear(flags, 3);  // flags becomes 0b11110111
```

---

#### `uint8_t lowByte(uint16_t value)`

Extract the low byte from a 16-bit value.

**Parameters:**
- `value`: 16-bit value

**Returns:**
- Low 8 bits

**Example:**
```cpp
uint16_t data = 0x1234;
uint8_t low = lowByte(data);  // Returns 0x34
```

---

#### `uint8_t highByte(uint16_t value)`

Extract the high byte from a 16-bit value.

**Parameters:**
- `value`: 16-bit value

**Returns:**
- High 8 bits

**Example:**
```cpp
uint16_t data = 0x1234;
uint8_t high = highByte(data);  // Returns 0x12
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

## Communication Protocols

PiPinPP provides Arduino-compatible APIs for I²C and SPI communication.

---

### Wire (I²C)

Arduino-compatible I²C communication library for Raspberry Pi.

#### Include
```cpp
#include "Wire.hpp"
using namespace pipinpp;
```

#### I²C Pin Connections

**Raspberry Pi 0-4:**
- SDA: GPIO 2 (Physical Pin 3)
- SCL: GPIO 3 (Physical Pin 5)
- Bus: `/dev/i2c-1`

**Raspberry Pi 5:**
- SDA: GPIO 2 (Physical Pin 3)
- SCL: GPIO 3 (Physical Pin 5)
- Bus: `/dev/i2c-20` (for pin headers)

#### Enable I²C
```bash
sudo raspi-config
# Navigate to: Interface Options → I2C → Enable

# Verify:
ls -l /dev/i2c-*
```

#### Wire Functions

##### `Wire.begin()`
Initialize I²C interface with automatic bus detection.

**Returns:** `true` on success, `false` on error

**Example:**
```cpp
if (!Wire.begin()) {
    std::cerr << "I2C initialization failed" << std::endl;
}
```

##### `Wire.begin(int busNumber)`
Initialize I²C with specific bus number.

**Parameters:**
- `busNumber`: I²C bus number (1 for Pi 0-4, 20 for Pi 5)

**Example:**
```cpp
Wire.begin(1);  // Explicitly use /dev/i2c-1
```

##### `Wire.end()`
Close I²C interface.

##### `Wire.setClock(uint32_t frequency)`
Set I²C bus speed.

**Parameters:**
- `frequency`: Clock frequency in Hz (default: 100000 = 100 kHz)

**Common Speeds:**
- Standard: 100,000 Hz (100 kHz)
- Fast: 400,000 Hz (400 kHz)
- Fast Plus: 1,000,000 Hz (1 MHz)

**Example:**
```cpp
Wire.setClock(400000);  // 400 kHz fast mode
```

##### `Wire.getClock()`
Get current I²C clock speed.

**Returns:** Clock frequency in Hz

##### `Wire.beginTransmission(uint8_t address)`
Start I²C transmission to device.

**Parameters:**
- `address`: 7-bit I²C device address (0x08-0x77)

**Example:**
```cpp
Wire.beginTransmission(0x68);  // MPU6050 address
```

##### `Wire.write(uint8_t data)`
Write single byte to I²C device.

**Returns:** Number of bytes written (1 on success)

**Example:**
```cpp
Wire.write(0x6B);  // Write register address
Wire.write(0x00);  // Write register value
```

##### `Wire.write(const uint8_t* data, size_t length)`
Write multiple bytes.

**Parameters:**
- `data`: Pointer to data buffer
- `length`: Number of bytes to write

##### `Wire.endTransmission(bool sendStop = true)`
End I²C transmission.

**Parameters:**
- `sendStop`: Send I²C stop condition (default: true)

**Returns:** Status code
- `0`: Success
- `1`: Data too long
- `2`: NACK on address
- `3`: NACK on data
- `4`: Other error

**Example:**
```cpp
Wire.beginTransmission(0x68);
Wire.write(0x6B);
Wire.write(0x00);
uint8_t error = Wire.endTransmission();
if (error != 0) {
    std::cerr << "I2C error: " << (int)error << std::endl;
}
```

##### `Wire.requestFrom(uint8_t address, size_t length, bool sendStop = true)`
Request data from I²C device.

**Parameters:**
- `address`: 7-bit I²C device address
- `length`: Number of bytes to request
- `sendStop`: Send stop condition after reading

**Returns:** Number of bytes available to read

**Example:**
```cpp
Wire.requestFrom(0x68, 2);  // Request 2 bytes from device
uint8_t high = Wire.read();
uint8_t low = Wire.read();
```

##### `Wire.available()`
Check how many bytes are available to read.

**Returns:** Number of bytes in receive buffer

##### `Wire.read()`
Read one byte from receive buffer.

**Returns:** Next byte (uint8_t)

##### `Wire.peek()`
Peek at next byte without removing from buffer.

**Returns:** Next byte (uint8_t)

#### Helper Functions

##### `Wire.exists(uint8_t address)`
Check if device exists at address.

**Returns:** `true` if device responds, `false` otherwise

**Example:**
```cpp
if (Wire.exists(0x76)) {
    std::cout << "BMP280 sensor found" << std::endl;
}
```

##### `Wire.scan()`
Scan all I²C addresses for connected devices.

**Returns:** `std::vector<uint8_t>` of found addresses

**Example:**
```cpp
std::vector<uint8_t> devices = Wire.scan();
for (uint8_t addr : devices) {
    std::cout << "Device at 0x" << std::hex << (int)addr << std::endl;
}
```

##### `Wire.readRegister(uint8_t address, uint8_t reg, uint8_t& value)`
Read single register from device.

**Parameters:**
- `address`: Device I²C address
- `reg`: Register address
- `value`: Reference to store read value

**Returns:** `true` on success

##### `Wire.writeRegister(uint8_t address, uint8_t reg, uint8_t value)`
Write single register to device.

**Returns:** `true` on success

#### Complete I²C Example
```cpp
#include "Wire.hpp"

// Read temperature from MPU6050
Wire.begin();
Wire.setClock(400000);  // 400 kHz

// Read WHO_AM_I register (0x75)
Wire.beginTransmission(0x68);
Wire.write(0x75);
Wire.endTransmission(false);

Wire.requestFrom(0x68, 1);
uint8_t whoami = Wire.read();

std::cout << "WHO_AM_I: 0x" << std::hex << (int)whoami << std::endl;
```

---

### SPI

Arduino-compatible SPI master communication for Raspberry Pi.

#### Include
```cpp
#include "SPI.hpp"
using namespace pipinpp;
```

#### SPI Pin Connections (All Raspberry Pi Models)

**Hardware SPI0:**
- MOSI: GPIO 10 (Physical Pin 19)
- MISO: GPIO 9 (Physical Pin 21)
- SCLK: GPIO 11 (Physical Pin 23)
- CE0: GPIO 8 (Physical Pin 24)
- CE1: GPIO 7 (Physical Pin 26)

**Device:** `/dev/spidev0.0` or `/dev/spidev0.1`

#### Enable SPI
```bash
sudo raspi-config
# Navigate to: Interface Options → SPI → Enable

# Verify:
ls -l /dev/spidev*
```

#### SPI Constants

##### Modes
```cpp
SPI_MODE0  // CPOL=0, CPHA=0 (most common)
SPI_MODE1  // CPOL=0, CPHA=1
SPI_MODE2  // CPOL=1, CPHA=0
SPI_MODE3  // CPOL=1, CPHA=1
```

##### Bit Order
```cpp
MSBFIRST  // Most Significant Bit First (default, most devices)
LSBFIRST  // Least Significant Bit First
```

##### Clock Dividers
```cpp
SPI_CLOCK_DIV2    // 125 MHz
SPI_CLOCK_DIV4    // 62.5 MHz
SPI_CLOCK_DIV8    // 31.25 MHz
SPI_CLOCK_DIV16   // 15.625 MHz
SPI_CLOCK_DIV32   // 7.8125 MHz
SPI_CLOCK_DIV64   // 3.90625 MHz
SPI_CLOCK_DIV128  // 1.953125 MHz
```

#### SPI Functions

##### `SPI.begin()`
Initialize SPI with default settings (MODE0, MSBFIRST, 4 MHz).

**Returns:** `true` on success, `false` if hardware not available

**Example:**
```cpp
if (!SPI.begin()) {
    std::cerr << "SPI initialization failed" << std::endl;
}
```

##### `SPI.begin(int bus, int cs)`
Initialize specific SPI bus and chip select.

**Parameters:**
- `bus`: SPI bus number (0 or 1)
- `cs`: Chip select (0 or 1)

**Example:**
```cpp
SPI.begin(0, 0);  // /dev/spidev0.0
SPI.begin(0, 1);  // /dev/spidev0.1
```

##### `SPI.end()`
Close SPI interface.

##### `SPI.setDataMode(uint8_t mode)`
Set SPI mode (clock polarity and phase).

**Parameters:**
- `mode`: `SPI_MODE0`, `SPI_MODE1`, `SPI_MODE2`, or `SPI_MODE3`

**Example:**
```cpp
SPI.setDataMode(SPI_MODE0);  // Most common mode
```

##### `SPI.setBitOrder(uint8_t order)`
Set bit transmission order.

**Parameters:**
- `order`: `MSBFIRST` (default) or `LSBFIRST`

**Example:**
```cpp
SPI.setBitOrder(MSBFIRST);  // Send MSB first
```

##### `SPI.setClockDivider(uint8_t divider)`
Set clock speed using divider (Arduino compatibility).

**Parameters:**
- `divider`: `SPI_CLOCK_DIV2` through `SPI_CLOCK_DIV128`

**Example:**
```cpp
SPI.setClockDivider(SPI_CLOCK_DIV8);  // 31.25 MHz
```

##### `SPI.setClock(uint32_t speed)`
Set clock speed directly in Hz.

**Parameters:**
- `speed`: Clock frequency in Hz (up to 125 MHz)

**Example:**
```cpp
SPI.setClock(1000000);   // 1 MHz
SPI.setClock(10000000);  // 10 MHz
```

##### `SPI.getClock()`
Get current clock speed.

**Returns:** Clock frequency in Hz

##### `SPI.transfer(uint8_t data)`
Send and receive one byte (full-duplex).

**Parameters:**
- `data`: Byte to send

**Returns:** Byte received

**Example:**
```cpp
digitalWrite(CS_PIN, LOW);   // Select device
uint8_t response = SPI.transfer(0x42);
digitalWrite(CS_PIN, HIGH);  // Deselect device
```

##### `SPI.transfer(uint8_t* buffer, size_t length)`
Transfer multiple bytes (in-place).

**Parameters:**
- `buffer`: Data to send, will be overwritten with received data
- `length`: Number of bytes

**Example:**
```cpp
uint8_t data[] = {0x01, 0x02, 0x03};
digitalWrite(CS_PIN, LOW);
SPI.transfer(data, 3);  // data[] now contains received bytes
digitalWrite(CS_PIN, HIGH);
```

##### `SPI.transfer(const uint8_t* txBuffer, uint8_t* rxBuffer, size_t length)`
Transfer with separate TX and RX buffers.

**Parameters:**
- `txBuffer`: Data to send
- `rxBuffer`: Buffer for received data
- `length`: Number of bytes

##### `SPI.isInitialized()`
Check if SPI is initialized.

**Returns:** `true` if initialized

#### Complete SPI Example
```cpp
#include "ArduinoCompat.hpp"
#include "SPI.hpp"

const int CS_PIN = 8;  // Chip select

void setup() {
    // Initialize SPI
    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClock(1000000);  // 1 MHz
    
    // Setup chip select
    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);
}

void loop() {
    // Send command to SPI device
    digitalWrite(CS_PIN, LOW);
    
    SPI.transfer(0x40);  // Command byte
    uint8_t status = SPI.transfer(0x00);  // Read status
    
    digitalWrite(CS_PIN, HIGH);
    
    delay(100);
}
```

#### SPI vs shiftOut Performance

| Method | Speed | Use Case |
|--------|-------|----------|
| **Hardware SPI** | ~1-125 MHz | Fast, reliable, recommended |
| **shiftOut()** | ~200-300 kHz | Any GPIO pins, flexible |

See `examples/spi_74hc595/` for performance comparison demonstration.

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