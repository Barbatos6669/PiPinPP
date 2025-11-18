# PiPinPP API Reference

**Version**: 0.4.0  
**Date**: November 16, 2025

Complete API documentation for PiPinPP - A modern C++ GPIO library for Raspberry Pi with full Arduino-inspired API, interrupts, PWM, and professional tooling.

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Platform Detection (NEW v0.4.0)](#platform-detection)
   - [PlatformInfo Class](#platforminfo-class)
   - [Platform-Aware Code](#platform-aware-code)
3. [Core Pin Class](#core-pin-class)
4. [Arduino Compatibility Layer](#arduino-compatibility-layer)
   - [Digital I/O Functions](#digital-io-functions)
   - [Pin State Query Functions](#pin-state-query-functions)
   - [Interrupt Functions](#interrupt-functions)
   - [PWM Functions](#pwm-functions)
5. [Event-Driven PWM (v0.4.0)](#event-driven-pwm)
   - [EventPWM Class](#eventpwm-class)
   - [When to Use EventPWM](#when-to-use-eventpwm)
6. [Hardware PWM](#hardware-pwm)
   - [HardwarePWM Class](#hardwarepwm-class)
   - [Hardware PWM Examples](#hardware-pwm-examples)
7. [Timing Functions](#timing-functions)
   - [Math Functions](#math-functions)
   - [Trigonometry Constants](#trigonometry-constants)
   - [Advanced I/O Functions](#advanced-io-functions)
8. [Communication Protocols](#communication-protocols)
   - [Wire (I²C)](#wire-i2c)
   - [SPI](#spi)
   - [Serial (UART)](#serial-uart)
9. [Examples](#examples)
10. [Error Handling](#error-handling)
11. [Hardware Notes](#hardware-notes)

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

## Platform Detection

**New in v0.4.0**: Automatic hardware platform detection for cross-platform support.

### PlatformInfo Class

The `PlatformInfo` class provides runtime detection of the hardware platform and available capabilities. It uses a singleton pattern - one instance per process.

```cpp
#include "platform.hpp"

auto& platform = pipinpp::PlatformInfo::instance();
```

### Platform Enum

```cpp
enum class Platform {
    UNKNOWN,
    RASPBERRY_PI_3,      // BCM2837
    RASPBERRY_PI_4,      // BCM2711
    RASPBERRY_PI_5,      // BCM2712 with RP1 I/O
    RASPBERRY_PI_CM4,    // Compute Module 4
    RASPBERRY_PI_ZERO,   // BCM2835
    RASPBERRY_PI_ZERO2,  // BCM2837
    ORANGE_PI,           // Allwinner H3/H5/H6
    BEAGLEBONE,          // TI AM335x
    JETSON_NANO          // NVIDIA Tegra X1
};
```

### Core Methods

#### `getPlatform()`
Get detected platform.
```cpp
Platform detected = platform.getPlatform();
if (detected == Platform::RASPBERRY_PI_4) {
    std::cout << "Running on Pi 4\n";
}
```

#### `getPlatformName()`
Get human-readable platform name.
```cpp
std::string name = platform.getPlatformName();
// Returns: "Raspberry Pi 4", "Orange Pi", etc.
```

#### `isRaspberryPi()`
Check if running on any Raspberry Pi variant.
```cpp
if (platform.isRaspberryPi()) {
    // Platform-specific Raspberry Pi code
}
```

#### `isSupported()`
Check if platform is recognized.
```cpp
if (!platform.isSupported()) {
    std::cerr << "Warning: Unknown platform, using defaults\n";
}
```

### Capability Detection

#### `getCapabilities()`
Get detailed hardware capabilities.
```cpp
const auto& caps = platform.getCapabilities();

// GPIO chips
for (const auto& chip : caps.gpioChips) {
    std::cout << chip.name << ": " << chip.numLines << " lines\n";
}

// I2C buses
for (const auto& bus : caps.i2cBuses) {
    std::cout << bus.devicePath << " available: " << bus.available << "\n";
}

// PWM channels
for (const auto& pwm : caps.pwmChannels) {
    if (pwm.gpioPin >= 0) {
        std::cout << "PWM on GPIO " << pwm.gpioPin << "\n";
    }
}

// DMA support
if (caps.hasDMASupport) {
    std::cout << "DMA available at 0x" << std::hex << caps.peripheralBase << "\n";
}
```

#### `getDefaultGPIOChip()`
Get recommended GPIO chip name.
```cpp
std::string chip = platform.getDefaultGPIOChip();
// Returns: "gpiochip0" (most platforms)
```

#### `getDefaultI2CBus()`
Get recommended I2C bus number.
```cpp
int bus = platform.getDefaultI2CBus();
// Returns: 1 for Pi 3/4, 20 for Pi 5
Wire.begin(bus);  // Works across platforms!
```

### Version Detection

#### `getKernelVersion()`
Get Linux kernel version.
```cpp
std::string kernel = platform.getKernelVersion();
// Returns: "6.1.21-v8+", etc.
```

#### `getLibgpiodVersion()`
Get libgpiod version.
```cpp
std::string libgpiod = platform.getLibgpiodVersion();
// Returns: "2.2.1", etc.
```

### Diagnostic Output

#### `printInfo()`
Print comprehensive platform information.
```cpp
platform.printInfo();
```

Output:
```
╔════════════════════════════════════════════════════════════════╗
║               PiPin++ Platform Information                     ║
╚════════════════════════════════════════════════════════════════╝

Platform:      Raspberry Pi 4
Kernel:        6.1.21-v8+
libgpiod:      2.2.1

GPIO Chips:    1
  • gpiochip0 (pinctrl-bcm2711): 58 lines ✓

I2C Buses:     2
  • /dev/i2c-1 (bus 1) ✓
  • /dev/i2c-20 (bus 20) ✗

PWM Channels:  2
  • chip0/channel0 (GPIO 18) ✓
  • chip0/channel1 (GPIO 19) ✓

DMA Support:   Available ✓
  Peripheral Base: 0xfe000000

Total GPIO:    58 pins
```

### Platform-Aware Code

#### Cross-Platform I2C
```cpp
// Automatically uses correct I2C bus (1 for Pi 4, 20 for Pi 5)
auto& platform = PlatformInfo::instance();
Wire.begin(platform.getDefaultI2CBus());

// Read from I2C sensor (works on all platforms)
Wire.beginTransmission(0x76);  // BMP280 address
Wire.write(0xD0);              // ID register
Wire.endTransmission();
Wire.requestFrom(0x76, 1);
uint8_t chipId = Wire.read();
```

#### Feature Detection
```cpp
// Choose best PWM backend
auto& platform = PlatformInfo::instance();
const auto& caps = platform.getCapabilities();

std::unique_ptr<PWMBackend> pwm;

if (caps.hasDMASupport && geteuid() == 0) {
    // Use DMA (zero CPU, requires root)
    std::cout << "Using DMA PWM (0% CPU)\n";
    pwm = std::make_unique<DMAPWM>(18, 1000);
} else if (!caps.pwmChannels.empty()) {
    // Use hardware PWM
    std::cout << "Using Hardware PWM\n";
    pwm = std::make_unique<HardwarePWM>(18);
    pwm->begin(1000, 50.0);
} else {
    // Use software PWM (EventPWM)
    std::cout << "Using EventPWM (<5% CPU)\n";
    pwm = std::make_unique<EventPWM>(18, 1000);
}

pwm->setDutyCycle(50.0);
```

#### Platform-Specific Logic
```cpp
auto& platform = PlatformInfo::instance();

switch (platform.getPlatform()) {
    case Platform::RASPBERRY_PI_5:
        std::cout << "Pi 5 detected: Using RP1 I/O controller\n";
        std::cout << "Note: DMA GPIO not yet supported on Pi 5\n";
        break;
        
    case Platform::RASPBERRY_PI_4:
    case Platform::RASPBERRY_PI_CM4:
        std::cout << "Pi 4 detected: Full DMA support available\n";
        if (caps.hasDMASupport) {
            // Enable DMA features
        }
        break;
        
    case Platform::RASPBERRY_PI_3:
        std::cout << "Pi 3 detected: DMA at 0x3F000000\n";
        break;
        
    default:
        std::cout << "Generic platform: Using software-based I/O\n";
        break;
}
```

### Platform Support Matrix

| Platform | Detection | GPIO | I2C | PWM | DMA |
|----------|-----------|------|-----|-----|-----|
| **Raspberry Pi 5** | ✅ Full | ✅ | ✅ (bus 20) | ⚠️ Limited | ❌ (RP1) |
| **Raspberry Pi 4** | ✅ Full | ✅ | ✅ (bus 1) | ✅ | ✅ |
| **Raspberry Pi CM4** | ✅ Full | ✅ | ✅ | ✅ | ✅ |
| **Raspberry Pi 3** | ✅ Full | ✅ | ✅ | ✅ | ⚠️ Untested |
| **Raspberry Pi Zero 2** | ✅ Full | ✅ | ✅ | ✅ | ⚠️ Untested |
| **Raspberry Pi Zero** | ✅ Full | ✅ | ✅ | ✅ | ❌ |
| **Orange Pi** | ⚠️ Basic | ⚠️ | ⚠️ | ❌ | ❌ |
| **BeagleBone** | ⚠️ Basic | ⚠️ | ⚠️ | ❌ | ❌ |
| **Jetson Nano** | ⚠️ Basic | ⚠️ | ⚠️ | ❌ | ❌ |

**Legend:**
- ✅ Full support, tested
- ⚠️ Partial support or untested
- ❌ Not supported

### Example: Complete Platform Detection

See `examples/platform_detection/` for a comprehensive demonstration.

```cpp
#include "platform.hpp"
#include <iostream>

int main() {
    auto& platform = pipinpp::PlatformInfo::instance();
    
    // Print detailed platform info
    platform.printInfo();
    
    // Check capabilities
    if (!platform.isSupported()) {
        std::cerr << "Warning: Platform not recognized\n";
        return 1;
    }
    
    // Use platform-aware defaults
    std::cout << "Default GPIO chip: " << platform.getDefaultGPIOChip() << "\n";
    std::cout << "Default I2C bus: " << platform.getDefaultI2CBus() << "\n";
    
    // Feature availability
    const auto& caps = platform.getCapabilities();
    std::cout << "DMA available: " << (caps.hasDMASupport ? "Yes" : "No") << "\n";
    
    return 0;
}
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
- `InvalidPinError`: Invalid pin number
- `GpioAccessError`: GPIO access failure

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
- `InvalidPinError`: Invalid pin number
- `GpioAccessError`: GPIO access failure

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
constexpr bool HIGH = true;
constexpr bool LOW = false;

enum ArduinoPinMode {
    INPUT = 0,           // Floating input (high impedance)
    OUTPUT = 1,          // Output mode
    INPUT_PULLUP = 2,    // Input with internal pull-up resistor (~50kΩ to 3.3V)
    INPUT_PULLDOWN = 3   // Input with internal pull-down resistor (~50kΩ to GND)
};
```

### Digital I/O Functions

#### `void pinMode(int pin, int mode)`
Configure pin mode (Arduino-style).

**Parameters:**
- `pin`: GPIO pin number (0-27)
- `mode`: `INPUT`, `OUTPUT`, `INPUT_PULLUP`, or `INPUT_PULLDOWN`

**Example:**
```cpp
pinMode(17, OUTPUT);          // LED output
pinMode(18, INPUT_PULLUP);    // Button with pull-up (reads HIGH when unpressed)
pinMode(19, INPUT_PULLDOWN);  // Button with pull-down (reads LOW when unpressed)
pinMode(20, INPUT);           // Floating input (requires external resistor)
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
- **⚠️ CPU Usage**: Each active PWM pin runs a busy-loop thread consuming ~10-30% CPU per pin
  - Uses `std::this_thread::yield()` to share CPU, but still busy-waits for timing accuracy
  - For servo control or precise timing, use `HardwarePWM` class instead (zero CPU usage)
  - Software PWM is best for LED dimming and non-critical applications
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

---

## Event-Driven PWM

**NEW in v0.4.0** - The `EventPWM` class provides software PWM with **70-85% lower CPU usage** compared to `analogWrite()`. It uses a hybrid timing algorithm (clock_nanosleep + busy-wait) that reduces CPU consumption from 10-30% to <5% per pin while maintaining acceptable timing accuracy for LED control.

### EventPWM Class

#### `EventPWM(int pin, const std::string& chipname = "gpiochip0")`
Constructor for event-driven PWM on a GPIO pin.

**Parameters:**
- `pin`: GPIO pin number (0-27 for Raspberry Pi)
- `chipname`: GPIO chip name (default: "gpiochip0")

**Throws:**
- `InvalidPinError`: Invalid pin number
- `GpioAccessError`: GPIO access failure

**Example:**
```cpp
#include "event_pwm.hpp"

pipinpp::EventPWM led(17);  // Create EventPWM on GPIO17
```

#### `bool begin(double frequencyHz, double dutyCycle)`
Start EventPWM with specified frequency and duty cycle.

**Parameters:**
- `frequencyHz`: PWM frequency in Hz (50 - 10,000)
  - Recommended: 50-2000 Hz for LED control
  - Higher frequencies have proportionally higher CPU usage
- `dutyCycle`: Initial duty cycle percentage (0.0 - 100.0)
  - 0.0 = Always LOW (LED off)
  - 50.0 = 50% duty cycle (half brightness)
  - 100.0 = Always HIGH (LED fully on)

**Returns:**
- `true` on success
- `false` on failure

**Notes:**
- **CPU Usage**: <5% per pin (70-85% reduction vs `analogWrite()`)
- **Timing Accuracy**: <10 µs jitter (2x worse than busy-loop, but acceptable for LEDs)
- **Hybrid Algorithm**: Sleeps for (period - 100µs), then busy-waits final 100µs for precision
- Edge cases optimized: <0.1% duty = always off, >99.9% duty = always on (no PWM overhead)

**Example:**
```cpp
pipinpp::EventPWM led(17);
if (!led.begin(1000, 50.0)) {  // 1kHz, 50% duty
    std::cerr << "Failed to start PWM" << std::endl;
    return 1;
}
```

#### `void end()`
Stop EventPWM and release resources.

**Example:**
```cpp
led.end();  // Stop PWM, clean up thread
```

#### `void setDutyCycle(double dutyCycle)`
Change duty cycle while PWM is running.

**Parameters:**
- `dutyCycle`: New duty cycle percentage (0.0 - 100.0)

**Notes:**
- Thread-safe: can be called from any thread
- Change takes effect on next PWM cycle
- Use for smooth LED fading

**Example:**
```cpp
// Smooth fade from 0% to 100%
for (double brightness = 0.0; brightness <= 100.0; brightness += 0.5) {
    led.setDutyCycle(brightness);
    delay(10);
}
```

#### `void setDutyCycle8Bit(uint8_t value)`
Arduino-compatible duty cycle setting (0-255 scale).

**Parameters:**
- `value`: Duty cycle (0-255)
  - 0 = 0% duty (LED off)
  - 127 = ~50% duty (half brightness)
  - 255 = 100% duty (LED fully on)

**Example:**
```cpp
// Arduino-style LED fade
for (int brightness = 0; brightness <= 255; brightness++) {
    led.setDutyCycle8Bit(brightness);
    delay(10);
}
```

#### `void setFrequency(double frequencyHz)`
Change PWM frequency while running.

**Parameters:**
- `frequencyHz`: New frequency in Hz (50 - 10,000)

**Notes:**
- Frequency change takes effect on next cycle
- Duty cycle percentage is preserved
- Higher frequencies increase CPU usage slightly

**Example:**
```cpp
led.setFrequency(2000);  // Change to 2kHz
```

### When to Use EventPWM

**Choose EventPWM when:**
- ✅ Controlling multiple LEDs (3+ pins)
- ✅ Long-running applications where CPU usage matters
- ✅ Battery-powered projects (lower CPU = less power)
- ✅ LED dimming, fade effects, RGB color mixing
- ✅ Any application where <10µs jitter is acceptable

**Choose `analogWrite()` (busy-loop PWM) when:**
- ⚠️ Simple quick demos (1-2 pins, short duration)
- ⚠️ You need the absolute lowest jitter (<5µs)
- ⚠️ Legacy code compatibility

**Choose `HardwarePWM` when:**
- 🚀 Servo motor control (requires zero jitter)
- 🚀 Precise timing applications
- 🚀 High frequencies (>10 kHz)
- 🚀 Zero CPU usage required

### EventPWM Manager (Global Functions)

For Arduino-style usage without explicit object creation:

#### `void analogWriteEvent(int pin, int value)`
Start EventPWM using Arduino-compatible API.

**Parameters:**
- `pin`: GPIO pin number (0-27)
- `value`: Duty cycle (0-255)

**Notes:**
- Automatically manages EventPWM objects internally
- First call creates EventPWM, subsequent calls update duty cycle
- Default frequency: 490 Hz (matches Arduino)

**Example:**
```cpp
#include "event_pwm.hpp"

// Arduino-style: no object creation needed
pipinpp::analogWriteEvent(17, 128);  // 50% brightness
delay(5000);
pipinpp::analogWriteEvent(17, 0);    // Turn off
```

### Performance Comparison

| Implementation | CPU Usage/Pin | Jitter | Servo Control | Multi-Pin | Power Efficient |
|---------------|---------------|--------|---------------|-----------|-----------------|
| `analogWrite()` | 10-30% | <5µs | ❌ No | Limited (2-3 pins) | ❌ No |
| `EventPWM` | <5% | <10µs | ❌ No | ✅ Yes (10+ pins) | ✅ Yes |
| `HardwarePWM` | 0% | 0µs | ✅ Yes | Limited (2-4 pins) | ✅ Yes |

**Migration Example:**
```cpp
// OLD: High CPU usage
analogWrite(17, 128);  // ~10-30% CPU per pin

// NEW: Low CPU usage (recommended for v0.4.0+)
pipinpp::EventPWM led(17);
led.begin(490, 50.0);  // <5% CPU per pin
led.setDutyCycle8Bit(128);  // Arduino-compatible API
```

---

## Hardware PWM

For applications requiring jitter-free PWM (servo control, precise timing), use the `HardwarePWM` class which interfaces with the Raspberry Pi hardware PWM controller via Linux sysfs.

**🚀 Why Use Hardware PWM?**
- **Zero CPU usage** - Timing handled by hardware peripheral, not software threads
- **Perfect timing accuracy** - No jitter or timing drift (critical for servo motors)
- **High frequencies** - Supports up to 25 MHz (software PWM limited to ~10 kHz)
- **System load immune** - Unaffected by CPU load, context switches, or other processes

**⚠️ Software PWM (`analogWrite()`) Limitations:**
- Busy-loop threads consume ~10-30% CPU per pin
- Timing jitter: ~1-10 µs (unsuitable for servos)
- Frequency range: 50 Hz - 10 kHz practical limit
- Use software PWM only for LED dimming or non-critical applications

**Supported Pins:**
- GPIO12 (PWM0 channel 0)
- GPIO13 (PWM0 channel 1)
- GPIO18 (PWM0 channel 0)
- GPIO19 (PWM0 channel 1)

**Note:** Only one pin per PWM channel can be active at a time (e.g., GPIO12 and GPIO18 share PWM0 channel 0).

### HardwarePWM Class

#### `HardwarePWM(int chip, int channel)`
Constructor for hardware PWM control.

**Parameters:**
- `chip`: PWM chip number (0 or 1, typically 0 for Raspberry Pi)
- `channel`: PWM channel (0 or 1)

**Example:**
```cpp
#include "HardwarePWM.hpp"

// GPIO18 uses PWM chip 0, channel 0
HardwarePWM servo(0, 0);
```

**Helper Function:**
```cpp
// Get chip and channel from GPIO number
int chip, channel;
if (HardwarePWM::gpioToPWM(18, chip, channel)) {
    HardwarePWM pwm(chip, channel);
}
```

#### `bool begin(double frequencyHz, double dutyCycle = 0.0)`
Initialize hardware PWM with specified frequency.

**Parameters:**
- `frequencyHz`: Frequency in Hz (1 - 25,000,000)
- `dutyCycle`: Initial duty cycle percentage (0.0 - 100.0)

**Returns:**
- `true` on success
- `false` on failure (check logs with `PIPINPP_ENABLE_LOGGING=ON`)

**Example:**
```cpp
HardwarePWM servo(0, 0);
if (!servo.begin(50, 7.5)) {  // 50Hz, 7.5% duty (servo center)
    std::cerr << "Failed to initialize PWM" << std::endl;
    return 1;
}
```

#### `void end()`
Disable hardware PWM and release resources.

**Example:**
```cpp
servo.end();  // Clean shutdown
```

#### `bool setFrequency(double frequencyHz)`
Change PWM frequency while preserving duty cycle.

**Parameters:**
- `frequencyHz`: New frequency in Hz (1 - 25,000,000)

**Returns:**
- `true` on success
- `false` on failure

**Notes:**
- Automatically disables PWM, updates period, re-enables
- Duty cycle is recalculated to maintain the same pulse width
- Critical for servo control (requires 50Hz)

**Example:**
```cpp
servo.setFrequency(50);    // Standard servo frequency
servo.setFrequency(1000);  // 1kHz for LED control
```

#### `bool setDutyCycle(double dutyCycle)`
Set duty cycle as percentage.

**Parameters:**
- `dutyCycle`: Percentage (0.0 - 100.0)

**Returns:**
- `true` on success
- `false` on failure

**Example:**
```cpp
servo.setDutyCycle(5.0);   // Servo 0°
servo.setDutyCycle(7.5);   // Servo 90°
servo.setDutyCycle(10.0);  // Servo 180°
```

#### `bool setDutyCycle8Bit(uint8_t value)`
Set duty cycle using Arduino-style 0-255 range.

**Parameters:**
- `value`: Duty cycle (0 - 255)
  - `0` = 0% (always LOW)
  - `127` = 50%
  - `255` = 100% (always HIGH)

**Returns:**
- `true` on success
- `false` on failure

**Example:**
```cpp
HardwarePWM led(0, 0);
led.begin(1000);  // 1kHz

// Fade LED
for (int i = 0; i <= 255; i++) {
    led.setDutyCycle8Bit(i);
    delay(10);
}
```

#### `bool setPeriodNs(uint64_t periodNs)`
Set PWM period in nanoseconds (advanced).

**Parameters:**
- `periodNs`: Period in nanoseconds

**Returns:**
- `true` on success
- `false` on failure

**Example:**
```cpp
// 50Hz = 20ms = 20,000,000 ns
servo.setPeriodNs(20000000);
```

#### `bool setDutyCycleNs(uint64_t dutyCycleNs)`
Set duty cycle in nanoseconds (advanced).

**Parameters:**
- `dutyCycleNs`: Duty cycle in nanoseconds

**Returns:**
- `true` on success
- `false` on failure

**Example:**
```cpp
// 1.5ms pulse at 50Hz (servo center)
servo.setDutyCycleNs(1500000);
```

#### `bool setPolarity(PWMPolarity polarity)`
Set PWM output polarity.

**Parameters:**
- `polarity`: `PWMPolarity::NORMAL` or `PWMPolarity::INVERSED`

**Returns:**
- `true` on success
- `false` on failure

**Example:**
```cpp
servo.setPolarity(PWMPolarity::INVERSED);  // Invert output
```

#### `bool enable()` / `bool disable()`
Enable or disable PWM output.

**Returns:**
- `true` on success
- `false` on failure

**Example:**
```cpp
servo.enable();   // Start PWM
servo.disable();  // Stop PWM (output goes LOW)
```

#### Getters

```cpp
double getFrequency() const;      // Get current frequency (Hz)
double getDutyCycle() const;      // Get duty cycle (0.0-100.0)
uint64_t getPeriodNs() const;     // Get period (ns)
uint64_t getDutyCycleNs() const;  // Get duty cycle (ns)
bool isEnabled() const;            // Check if PWM is running
```

### Hardware PWM Examples

#### Servo Control
```cpp
#include "HardwarePWM.hpp"
#include "ArduinoCompat.hpp"

// Convert angle (0-180°) to pulse width (1-2ms)
double angleToPulseWidth(int angle) {
    return 1.0 + (angle / 180.0);  // 1ms at 0°, 2ms at 180°
}

// Convert pulse width to duty cycle at 50Hz
double pulseWidthToDutyCycle(double pulseMs) {
    return (pulseMs / 20.0) * 100.0;  // 20ms period
}

int main() {
    HardwarePWM servo(0, 0);  // GPIO18
    
    if (!servo.begin(50, 7.5)) {  // 50Hz, center position
        return 1;
    }
    
    // Move servo through positions
    for (int angle = 0; angle <= 180; angle += 10) {
        double pulseMs = angleToPulseWidth(angle);
        double duty = pulseWidthToDutyCycle(pulseMs);
        servo.setDutyCycle(duty);
        delay(100);
    }
    
    servo.end();
    return 0;
}
```

#### LED Fading with Hardware PWM
```cpp
#include "HardwarePWM.hpp"
#include "ArduinoCompat.hpp"
#include <cmath>

int main() {
    HardwarePWM led(0, 0);  // GPIO18
    
    if (!led.begin(1000)) {  // 1kHz frequency
        return 1;
    }
    
    // Breathing effect (sine wave)
    while (true) {
        for (int i = 0; i < 360; i++) {
            double radians = i * M_PI / 180.0;
            double brightness = (sin(radians) + 1.0) / 2.0;  // 0.0-1.0
            led.setDutyCycle(brightness * 100.0);
            delay(10);
        }
    }
    
    led.end();
    return 0;
}
```

**When to Use Hardware vs Software PWM:**
- **Hardware PWM**: Servos, motor control, precise timing, high frequencies (>1kHz)
- **Software PWM**: Multiple LED dimming, non-critical applications, pins without hardware PWM

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

#### Performance Profiling Example
Use `examples/performance_profiler/` to capture real-world drift and jitter data for the timing
APIs:

```bash
cd /path/to/PiPinPP/build
make example_performance_profiler
./examples/example_performance_profiler   # requires GPIO permissions or sudo
```

The program prints summary tables and writes `performance_profile.csv` (delay drift + loop jitter)
so you can graph the distribution in Python, Excel, or Grafana.

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
- **⚠️ Blocks execution** until pulse completes or timeout occurs
- **⚠️ CPU Usage**: Uses busy-waiting loop for microsecond timing accuracy
  - Consumes 100% of one CPU core while waiting for pulse
  - For non-blocking pulse measurement, consider using interrupts with timestamps
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
- **⚠️ CPU Usage**: Same busy-wait behavior as `pulseIn()` (100% CPU during measurement)

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
- **⚠️ CPU Usage**: Runs busy-loop thread consuming ~10-30% CPU while tone is active
  - Similar to `analogWrite()` - uses software PWM for square wave generation
  - For continuous tones or music, consider hardware PWM or external audio module
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

### Serial (UART)

**New in v0.3.12**: Arduino-compatible Serial/UART communication for Raspberry Pi.

#### Include
```cpp
#include "Serial.hpp"
using namespace pipinpp;
```

#### UART Pin Connections

**Raspberry Pi Hardware UART (GPIO 14/15):**
- TX: GPIO 14 (Physical Pin 8)
- RX: GPIO 15 (Physical Pin 10)
- Device: `/dev/ttyAMA0` (or `/dev/ttyS0` on older models)

**USB-to-Serial Adapters:**
- Device: `/dev/ttyUSB0`, `/dev/ttyUSB1`, etc.
- Common chips: FTDI FT232, CH340, CP2102

**Arduino via USB:**
- Device: `/dev/ttyACM0`, `/dev/ttyACM1`, etc.
- Arduino Uno, Mega, Leonardo (native USB)

#### Enable Hardware UART
```bash
# Disable console on serial (if using GPIO 14/15)
sudo raspi-config
# Navigate to: Interface Options → Serial Port
# Disable "Would you like a login shell accessible over serial?" → No
# Enable "Would you like the serial port hardware enabled?" → Yes

# Add user to dialout group (required for /dev/tty* access)
sudo usermod -a -G dialout $USER
# Log out and back in for group change to take effect

# Verify access
ls -l /dev/ttyUSB0  # or ttyACM0, ttyAMA0
```

#### Serial Functions

##### `Serial.begin(baudRate, device)`
Open serial port at specified baud rate.

**Parameters:**
- `baudRate`: Communication speed (9600, 19200, 38400, 57600, 115200, 230400, etc.)
- `device`: Serial device path (default: "/dev/ttyUSB0")

**Returns:** `true` on success, `false` on error

**Common Baud Rates:**
- **9600**: Standard Arduino default, sensors
- **19200**: Faster sensors, GPS modules
- **38400**: High-speed sensors
- **57600**: Fast Arduino communication
- **115200**: High-speed communication (most common)
- **230400**: Very high-speed (check cable quality)

**Example:**
```cpp
// Arduino via USB
if (!Serial.begin(9600, "/dev/ttyACM0")) {
    std::cerr << "Failed to open Arduino\n";
    return 1;
}

// FTDI USB adapter
if (!Serial.begin(115200, "/dev/ttyUSB0")) {
    std::cerr << "Failed to open USB-to-serial\n";
    return 1;
}

// Hardware UART (GPIO 14/15)
if (!Serial.begin(9600, "/dev/ttyAMA0")) {
    std::cerr << "Failed to open hardware UART\n";
    return 1;
}
```

##### `Serial.end()`
Close serial port and release resources.

**Example:**
```cpp
Serial.end();  // Always close when done
```

##### `Serial.isOpen()`
Check if serial port is open.

**Returns:** `true` if open, `false` otherwise

**Example:**
```cpp
if (!Serial.isOpen()) {
    Serial.begin(9600, "/dev/ttyUSB0");
}
```

##### `Serial.available()`
Get number of bytes available in receive buffer.

**Returns:** Number of bytes ready to read (0 if none)

**Example:**
```cpp
if (Serial.available() > 0) {
    int data = Serial.read();
}
```

##### `Serial.read()`
Read single byte from serial port.

**Returns:** Byte value (0-255), or -1 if no data or timeout

**Example:**
```cpp
int incomingByte = Serial.read();
if (incomingByte != -1) {
    std::cout << "Received: " << incomingByte << "\n";
}
```

##### `Serial.peek()`
Look at next byte without removing from buffer.

**Returns:** Byte value (0-255), or -1 if no data

**Example:**
```cpp
int nextByte = Serial.peek();
if (nextByte == 'S') {  // Start of command
    Serial.read();  // Now consume it
}
```

##### `Serial.write(byte)`
Send single byte.

**Parameters:**
- `byte`: Byte to send (0-255)

**Returns:** Number of bytes written (1 on success, 0 on failure)

**Example:**
```cpp
Serial.write(0x42);  // Send 'B'
Serial.write('\n');  // Send newline
```

##### `Serial.write(buffer, size)`
Send buffer of bytes.

**Parameters:**
- `buffer`: Pointer to data
- `size`: Number of bytes to send

**Returns:** Number of bytes actually written

**Example:**
```cpp
uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
Serial.write(data, 4);
```

##### `Serial.print(data)`
Print data without newline.

**Overloads:**
- `print(string)` - Print string
- `print(int)` - Print integer (decimal)
- `print(int, format)` - Print integer with format (DEC, HEX, OCT, BIN)
- `print(long)` - Print long integer
- `print(unsigned)` - Print unsigned integer
- `print(double, decimals)` - Print floating-point with precision

**Example:**
```cpp
Serial.print("Temperature: ");
Serial.print(23.5);
Serial.print(" C\n");

Serial.print(255, HEX);  // Prints "FF"
Serial.print(255, BIN);  // Prints "11111111"
```

##### `Serial.println(data)`
Print data with newline.

**Overloads:** Same as `print()` but adds `\n` at end

**Example:**
```cpp
Serial.println("Hello Arduino!");
Serial.println(42);
Serial.println(3.14159, 4);  // 4 decimal places: "3.1416"
```

##### `Serial.readString()`
Read all available data as string.

**Returns:** String with all available bytes

**Example:**
```cpp
delay(100);  // Wait for data
if (Serial.available()) {
    String response = Serial.readString();
    std::cout << "Received: " << response << "\n";
}
```

##### `Serial.readStringUntil(terminator)`
Read until terminator character.

**Parameters:**
- `terminator`: Stop character (e.g., '\n', '\r', ';')

**Returns:** String up to (but not including) terminator

**Example:**
```cpp
// Arduino sends "TEMP:25.3\n"
String response = Serial.readStringUntil('\n');
std::cout << "Got: " << response << "\n";  // "TEMP:25.3"
```

##### `Serial.setTimeout(milliseconds)`
Set read timeout.

**Parameters:**
- `milliseconds`: Timeout in ms (default: 1000)

**Example:**
```cpp
Serial.setTimeout(500);  // 0.5 second timeout
```

##### `Serial.flush()`
Wait until all outgoing data is transmitted.

**Example:**
```cpp
Serial.println("Critical command");
Serial.flush();  // Ensure it's sent before proceeding
```

#### Print Format Constants

```cpp
DEC  // Decimal (base 10) - default
HEX  // Hexadecimal (base 16)
OCT  // Octal (base 8)
BIN  // Binary (base 2)
```

#### Complete Serial Example

```cpp
#include "Serial.hpp"
#include "ArduinoCompat.hpp"
using namespace pipinpp;

int main() {
    // Open connection to Arduino at 9600 baud
    if (!Serial.begin(9600, "/dev/ttyACM0")) {
        std::cerr << "Failed to open Arduino\n";
        return 1;
    }
    
    std::cout << "Connected to Arduino. Sending commands...\n";
    
    // Send command every second
    for (int i = 0; i < 10; i++) {
        // Send command with parameters
        Serial.print("SET_LED:");
        Serial.println(i % 2);  // 0 or 1
        
        delay(100);  // Wait for Arduino to process
        
        // Read response
        if (Serial.available()) {
            String response = Serial.readStringUntil('\n');
            std::cout << "Arduino: " << response << "\n";
        }
        
        delay(900);  // Wait 1 second between commands
    }
    
    Serial.println("DONE");
    Serial.flush();
    Serial.end();
    
    return 0;
}
```

#### Arduino-to-Raspberry Pi Communication

**Arduino Code (upload this to Arduino):**
```cpp
void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        
        if (command.startsWith("SET_LED:")) {
            int state = command.substring(8).toInt();
            digitalWrite(LED_BUILTIN, state);
            Serial.println(state ? "LED ON" : "LED OFF");
        }
        else if (command == "DONE") {
            Serial.println("BYE");
        }
    }
}
```

**Raspberry Pi Code (PiPinPP):**
```cpp
#include "Serial.hpp"
using namespace pipinpp;

int main() {
    Serial.begin(9600, "/dev/ttyACM0");
    delay(2000);  // Wait for Arduino reset after serial connection
    
    // Send command
    Serial.println("SET_LED:1");
    
    // Wait for response
    delay(100);
    if (Serial.available()) {
        String response = Serial.readStringUntil('\n');
        std::cout << "Arduino says: " << response << "\n";  // "LED ON"
    }
    
    Serial.end();
}
```

#### Troubleshooting Serial

**Permission Denied:**
```bash
# Add user to dialout group
sudo usermod -a -G dialout $USER
# Log out and back in

# Verify
groups  # Should show "dialout"
```

**Device Not Found:**
```bash
# List available serial devices
ls -l /dev/tty*

# Check USB devices
dmesg | grep tty  # Look for "USB Serial" messages
lsusb  # Shows connected USB devices
```

**No Data Received:**
- Check baud rate matches on both sides
- Verify TX/RX connections (TX→RX, RX→TX)
- Add delay after `begin()` for Arduino to reset
- Check Arduino Serial Monitor isn't open (conflicts with /dev/ttyACM0)
- Try different USB cable (data vs power-only)

**Garbled Data:**
- Baud rate mismatch - verify both use same rate
- Check cable length (keep under 3m for high speeds)
- Lower baud rate (115200 → 57600 → 9600)

**Thread Safety:**
All Serial operations are thread-safe (protected by internal mutex). Multiple threads can safely call Serial methods concurrently.

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

PiPinPP uses custom exception types (all derived from `std::runtime_error`):

- `InvalidPinError`: Invalid pin numbers, invalid parameters
- `GpioAccessError`: Hardware access failures, GPIO errors

### Error Handling Example
```cpp
try {
    Pin led(17, PinDirection::OUTPUT);
    
    if (!led.write(true)) {
        std::cerr << "Failed to turn LED on" << std::endl;
    }
    
} catch (const InvalidPinError& e) {
    std::cerr << "Invalid pin configuration: " << e.what() << std::endl;
} catch (const GpioAccessError& e) {
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