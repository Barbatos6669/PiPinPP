# PWM Migration Guide

**Version:** 0.4.0  
**Date:** November 16, 2025

This guide helps you choose the right PWM implementation for your project and migrate from older PiPinPP versions.

---

## Table of Contents

1. [Quick Decision Tree](#quick-decision-tree)
2. [PWM Implementation Comparison](#pwm-implementation-comparison)
3. [Migration from v0.3.x to v0.4.0](#migration-from-v03x-to-v04)
4. [Performance Optimization Guide](#performance-optimization-guide)
5. [Common Use Cases](#common-use-cases)
6. [Troubleshooting](#troubleshooting)

---

## Quick Decision Tree

```
┌─────────────────────────────────────────┐
│  What are you controlling?              │
└─────────────┬───────────────────────────┘
              │
       ┌──────┴──────┐
       │             │
   ┌───▼───┐    ┌────▼─────┐
   │  LEDs │    │ Servos/  │
   │       │    │ Motors   │
   └───┬───┘    └────┬─────┘
       │             │
       │             └──► Use HardwarePWM
       │                  (Zero jitter required)
       │
   ┌───▼──────────────────┐
   │ How many LED pins?   │
   └───┬──────────────────┘
       │
   ┌───┴───┐
   │  1-2  │──► analogWrite() is fine
   │ pins  │    (Quick & simple)
   └───────┘
       │
   ┌───▼───┐
   │  3+   │──► Use EventPWM
   │ pins  │    (70-85% CPU savings)
   └───┬───┘
       │
   ┌───▼──────────────────┐
   │ Long-running app?    │
   │ (>1 hour)            │
   └───┬──────────────────┘
       │
   ┌───▼───┐
   │  YES  │──► Use EventPWM
   │       │    (Power efficient)
   └───┬───┘
       │
   ┌───▼───┐
   │  NO   │──► analogWrite() is OK
   │       │    (Simple demos)
   └───────┘
```

---

## PWM Implementation Comparison

### Feature Matrix

| Feature | analogWrite() | EventPWM | HardwarePWM |
|---------|---------------|----------|-------------|
| **CPU Usage** | 10-30% per pin | <5% per pin | 0% (hardware) |
| **Timing Jitter** | <5 µs | <10 µs | 0 µs (perfect) |
| **Available Pins** | Any GPIO (0-27) | Any GPIO (0-27) | GPIO 12,13,18,19 only |
| **Max Frequency** | ~10 kHz | ~10 kHz | 25 MHz |
| **Multi-Pin Support** | Limited (2-3) | Excellent (10+) | Limited (2-4 channels) |
| **Power Efficiency** | Poor (busy-loop) | Good | Excellent |
| **Servo Control** | ❌ Too much jitter | ❌ Too much jitter | ✅ Perfect |
| **LED Dimming** | ✅ Works | ✅ Recommended | ✅ Works (overkill) |
| **Arduino Compatible** | ✅ Yes | ✅ Yes (8-bit API) | ⚠️ Different API |
| **Thread-Safe** | ✅ Yes | ✅ Yes | ✅ Yes |

### Performance Metrics (Raspberry Pi 4)

**Test Setup:** 3 LEDs @ 490 Hz, 50% duty cycle, 10 second measurement

| Implementation | Avg CPU/Pin | Total CPU (3 pins) | Avg Jitter | Max Jitter |
|---------------|-------------|---------------------|------------|------------|
| `analogWrite()` | 10-15% | 30-45% | 2-4 µs | 8 µs |
| `EventPWM` | 2-3% | 6-9% | 5-8 µs | 15 µs |
| `HardwarePWM` | 0% | 0% | 0 µs | 0 µs |

**Key Insight:** EventPWM reduces CPU usage by 70-85% with only 2x increase in jitter (still acceptable for LEDs).

---

## Migration from v0.3.x to v0.4.0

### No Changes Required (Backward Compatible)

All existing code continues to work:

```cpp
// v0.3.x code - still works in v0.4.0
#include "ArduinoCompat.hpp"

int main() {
    analogWrite(17, 128);  // Still uses busy-loop PWM
    delay(5000);
    return 0;
}
```

### Recommended Migration (Opt-in Performance)

For better performance, migrate to EventPWM:

#### Pattern 1: Single Pin Arduino-Style

**Before (v0.3.x):**
```cpp
#include "ArduinoCompat.hpp"

int main() {
    analogWrite(17, 128);  // 50% brightness
    delay(5000);
    stopPWM(17);
    return 0;
}
```

**After (v0.4.0):**
```cpp
#include "event_pwm.hpp"

int main() {
    pipinpp::EventPWM led(17);
    led.begin(490, 50.0);      // 490Hz = Arduino default
    delay(5000);
    led.end();
    return 0;
}
```

#### Pattern 2: LED Fade Effect

**Before (v0.3.x):**
```cpp
#include "pwm.hpp"

int main() {
    // Fade up
    for (int brightness = 0; brightness <= 255; brightness++) {
        analogWrite(17, brightness);
        delay(10);
    }
    
    // Fade down
    for (int brightness = 255; brightness >= 0; brightness--) {
        analogWrite(17, brightness);
        delay(10);
    }
    
    return 0;
}
```

**After (v0.4.0) - Option A (Percentage):**
```cpp
#include "event_pwm.hpp"

int main() {
    pipinpp::EventPWM led(17);
    led.begin(490, 0.0);  // Start at 0%
    
    // Fade up (0% to 100%)
    for (double brightness = 0.0; brightness <= 100.0; brightness += 0.5) {
        led.setDutyCycle(brightness);
        delay(10);
    }
    
    // Fade down (100% to 0%)
    for (double brightness = 100.0; brightness >= 0.0; brightness -= 0.5) {
        led.setDutyCycle(brightness);
        delay(10);
    }
    
    led.end();
    return 0;
}
```

**After (v0.4.0) - Option B (8-bit Arduino-Compatible):**
```cpp
#include "event_pwm.hpp"

int main() {
    pipinpp::EventPWM led(17);
    led.begin(490, 0.0);
    
    // Fade up (0-255)
    for (uint8_t brightness = 0; brightness < 255; brightness++) {
        led.setDutyCycle8Bit(brightness);
        delay(10);
    }
    
    // Fade down (255-0)
    for (int brightness = 255; brightness >= 0; brightness--) {
        led.setDutyCycle8Bit(brightness);
        delay(10);
    }
    
    led.end();
    return 0;
}
```

#### Pattern 3: Multiple LEDs (RGB, Multi-Channel)

**Before (v0.3.x):**
```cpp
#include "ArduinoCompat.hpp"

int main() {
    // RGB LED control - HIGH CPU USAGE (30-45%)
    analogWrite(17, 255);  // Red full
    analogWrite(18, 128);  // Green half
    analogWrite(19, 64);   // Blue quarter
    
    delay(10000);
    
    // Clean up
    stopPWM(17);
    stopPWM(18);
    stopPWM(19);
    
    return 0;
}
```

**After (v0.4.0):**
```cpp
#include "event_pwm.hpp"

int main() {
    // RGB LED control - LOW CPU USAGE (<10%)
    pipinpp::EventPWM red(17);
    pipinpp::EventPWM green(18);
    pipinpp::EventPWM blue(19);
    
    red.begin(490, 100.0);   // Red full
    green.begin(490, 50.0);  // Green half
    blue.begin(490, 25.0);   // Blue quarter
    
    delay(10000);
    
    // Clean up (optional - destructor handles it)
    red.end();
    green.end();
    blue.end();
    
    return 0;
}
```

**Alternative: Arduino-style global functions:**
```cpp
#include "event_pwm.hpp"

int main() {
    // No object creation needed
    pipinpp::analogWriteEvent(17, 255);  // Red
    pipinpp::analogWriteEvent(18, 128);  // Green
    pipinpp::analogWriteEvent(19, 64);   // Blue
    
    delay(10000);
    
    // Clean up
    pipinpp::analogWriteEvent(17, 0);
    pipinpp::analogWriteEvent(18, 0);
    pipinpp::analogWriteEvent(19, 0);
    
    return 0;
}
```

---

## Performance Optimization Guide

### When to Use Each Implementation

#### Use `analogWrite()` (Busy-Loop PWM)

**Good for:**
- ✅ Quick demos and prototypes (< 5 minutes runtime)
- ✅ Single LED applications
- ✅ When you need the lowest jitter (<5 µs)
- ✅ Porting Arduino code with minimal changes

**Avoid when:**
- ❌ Running for extended periods (>1 hour)
- ❌ Controlling 3+ LEDs simultaneously
- ❌ Battery-powered applications
- ❌ CPU-intensive applications (video processing, etc.)

**Example:**
```cpp
// Simple blink demo - analogWrite() is fine
#include "ArduinoCompat.hpp"

int main() {
    for (int i = 0; i < 10; i++) {
        analogWrite(17, 255);
        delay(500);
        analogWrite(17, 0);
        delay(500);
    }
    return 0;
}
```

#### Use `EventPWM` (Hybrid Timing PWM)

**Good for:**
- ✅ Multi-LED control (3+ pins)
- ✅ Long-running applications
- ✅ Battery-powered projects
- ✅ LED dimming, fade effects
- ✅ RGB LED control
- ✅ When CPU usage matters

**Avoid when:**
- ⚠️ Controlling servos (use HardwarePWM)
- ⚠️ Need <5 µs jitter (use busy-loop)
- ⚠️ Quick throw-away demos (analogWrite simpler)

**Example:**
```cpp
// RGB LED controller - EventPWM recommended
#include "event_pwm.hpp"

int main() {
    pipinpp::EventPWM red(17), green(18), blue(19);
    
    red.begin(490, 0.0);
    green.begin(490, 0.0);
    blue.begin(490, 0.0);
    
    // Smooth color transitions
    while (true) {
        // Fade red up, green down
        for (double i = 0; i <= 100; i += 0.5) {
            red.setDutyCycle(i);
            green.setDutyCycle(100 - i);
            delay(10);
        }
        
        // Continue color wheel...
    }
    
    return 0;
}
```

#### Use `HardwarePWM`

**Good for:**
- ✅ Servo motor control (critical!)
- ✅ Precise timing applications
- ✅ High frequencies (>10 kHz)
- ✅ Zero CPU usage required
- ✅ Professional/production applications

**Limitations:**
- ⚠️ Only 4 pins available (GPIO 12, 13, 18, 19)
- ⚠️ Shared channels (GPIO12 and GPIO18 can't both be used)

**Example:**
```cpp
// Servo control - MUST use HardwarePWM
#include "HardwarePWM.hpp"

int main() {
    int chip, channel;
    if (!HardwarePWM::gpioToPWM(18, chip, channel)) {
        std::cerr << "GPIO18 doesn't support hardware PWM\n";
        return 1;
    }
    
    HardwarePWM servo(chip, channel);
    
    // Standard servo: 50Hz, 1-2ms pulse (5-10% duty)
    servo.begin(50, 7.5);  // Center position
    delay(1000);
    
    servo.setDutyCycle(5.0);   // Min position
    delay(1000);
    
    servo.setDutyCycle(10.0);  // Max position
    delay(1000);
    
    servo.end();
    return 0;
}
```

---

## Common Use Cases

### LED Strip Control (Many LEDs)

**Problem:** Controlling 10 LED channels with `analogWrite()` uses 100-300% CPU!

**Solution:** Use EventPWM

```cpp
#include "event_pwm.hpp"
#include <vector>

int main() {
    // 10-channel LED controller
    std::vector<int> ledPins = {17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
    std::vector<pipinpp::EventPWM> leds;
    
    // Initialize all channels
    for (int pin : ledPins) {
        leds.emplace_back(pin);
        leds.back().begin(490, 0.0);
    }
    
    // Smooth wave effect
    while (true) {
        for (size_t i = 0; i < leds.size(); i++) {
            double phase = (i * 36.0);  // 36° per LED
            double brightness = 50.0 + 50.0 * sin(phase * 3.14159 / 180.0);
            leds[i].setDutyCycle(brightness);
        }
        delay(50);
    }
    
    return 0;
}
```

**Result:** CPU usage <50% (vs 300% with analogWrite)

### Battery-Powered LED Indicators

**Problem:** Busy-loop PWM drains battery due to high CPU usage

**Solution:** EventPWM for power efficiency

```cpp
#include "event_pwm.hpp"

int main() {
    pipinpp::EventPWM statusLed(17);
    statusLed.begin(2, 50.0);  // Slow 2Hz blink at 50%
    
    // LED runs in background with minimal CPU
    // Main code can do other work
    while (true) {
        // Your application logic here
        delay(1000);
    }
    
    return 0;
}
```

### Breathing LED Effect

**Problem:** Smooth breathing requires many PWM updates

**Solution:** EventPWM with fine-grained duty cycle control

```cpp
#include "event_pwm.hpp"
#include <cmath>

int main() {
    pipinpp::EventPWM led(17);
    led.begin(490, 0.0);
    
    double phase = 0.0;
    
    while (true) {
        // Sine wave brightness
        double brightness = 50.0 + 50.0 * sin(phase);
        led.setDutyCycle(brightness);
        
        phase += 0.05;
        if (phase > 2 * 3.14159) phase = 0;
        
        delay(20);  // 50 updates/sec
    }
    
    return 0;
}
```

### Robot Control (Mixed PWM Types)

**Problem:** Motors need hardware PWM, status LEDs need software PWM

**Solution:** Combine HardwarePWM + EventPWM

```cpp
#include "HardwarePWM.hpp"
#include "event_pwm.hpp"

int main() {
    // Motors: Hardware PWM (precise speed control)
    int chip, channel;
    HardwarePWM::gpioToPWM(18, chip, channel);
    HardwarePWM motorLeft(chip, channel);
    motorLeft.begin(1000, 50.0);  // 1kHz motor drive
    
    // Status LEDs: EventPWM (efficient)
    pipinpp::EventPWM redLed(17);
    pipinpp::EventPWM greenLed(19);
    
    redLed.begin(490, 0.0);
    greenLed.begin(490, 100.0);  // Green = ready
    
    // Control loop
    while (true) {
        // Motor control logic...
        motorLeft.setDutyCycle(75.0);  // Speed up
        
        // LED indicators...
        redLed.setDutyCycle(0.0);     // Error off
        greenLed.setDutyCycle(100.0);  // Running
        
        delay(100);
    }
    
    return 0;
}
```

---

## Troubleshooting

### "Why is my LED flickering?"

**Cause:** Jitter in EventPWM or system load

**Solutions:**
1. Check if <10 µs jitter is acceptable for your LED
2. Try increasing frequency: `led.begin(2000, 50.0)`  // 2kHz
3. For critical applications, use HardwarePWM
4. Ensure no other CPU-intensive processes running

### "CPU usage is still high with EventPWM"

**Check:**
1. Are you using very high frequencies? (>5kHz reduces efficiency)
2. Did you migrate from `analogWrite()` or still using both?
3. Run benchmark: `sudo ./build/examples/example_benchmark_cpu`

**Solution:**
```cpp
// BAD: Still using old implementation
analogWrite(17, 128);  // High CPU

// GOOD: Using new implementation
pipinpp::EventPWM led(17);
led.begin(490, 50.0);  // Low CPU
```

### "How do I know which PWM I'm using?"

**Check your includes:**
```cpp
#include "pwm.hpp"          // analogWrite() - busy-loop
#include "event_pwm.hpp"    // EventPWM - hybrid timing
#include "HardwarePWM.hpp"  // HardwarePWM - zero CPU
```

### "Can I mix implementations?"

**Yes! They're independent:**
```cpp
#include "ArduinoCompat.hpp"
#include "event_pwm.hpp"
#include "HardwarePWM.hpp"

int main() {
    analogWrite(17, 128);              // Pin 17: busy-loop
    
    pipinpp::EventPWM led(19);         // Pin 19: event-driven
    led.begin(490, 50.0);
    
    HardwarePWM servo(0, 0);           // Pin 18: hardware
    servo.begin(50, 7.5);
    
    // All three run simultaneously
    delay(10000);
    
    return 0;
}
```

**Warning:** Don't use multiple implementations on the **same pin** - undefined behavior.

---

## Summary

**Default Choice for v0.4.0+:** Use `EventPWM` for all LED control

**Why?**
- 70-85% CPU reduction
- Acceptable jitter for LEDs (<10 µs)
- Scales to 10+ pins
- Power efficient
- Arduino-compatible API

**When to Deviate:**
- **Servos:** Use `HardwarePWM` (zero jitter required)
- **Quick Demos:** `analogWrite()` is simpler
- **Ultra-Low Jitter:** `analogWrite()` if <5 µs needed

**Migration Strategy:**
1. Keep existing code working (backward compatible)
2. Migrate long-running/multi-LED apps to EventPWM first (biggest gains)
3. Leave simple demos as analogWrite() (not worth the effort)
4. Always use HardwarePWM for servos

---

**Questions?** See [API_REFERENCE.md](API_REFERENCE.md) or file an issue on GitHub.
