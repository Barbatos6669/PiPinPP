# Arduino Migration Example

Complete guide and example code for migrating Arduino sketches to Raspberry Pi with PiPinPP.

## What This Example Shows

A side-by-side comparison of Arduino code and its PiPinPP equivalent, demonstrating:
- Pin number translation
- Header file changes
- Voltage considerations
- Platform differences
- Best practices

## The Migration Process

### Step 1: Understand Pin Numbering

**Arduino Uno:**
- Digital pins: 0-13
- Analog pins: A0-A5 (maps to 14-19 internally)
- Built-in LED: Pin 13

**Raspberry Pi (BCM numbering):**
- GPIO pins: 0-27
- No built-in LED on header
- All pins are 3.3V (NOT 5V tolerant!)

**Translation Table:**

| Arduino Purpose | Arduino Pin | Raspberry Pi Pin |
|-----------------|-------------|------------------|
| LED | 13 | 18 (or any GPIO) |
| Button | 2 | 17 (or any GPIO) |
| PWM-capable | 3, 5, 6, 9, 10, 11 | Any GPIO (software PWM) |
| I2C SDA | A4 | GPIO 2 (Pin 3) |
| I2C SCL | A5 | GPIO 3 (Pin 5) |
| SPI MOSI | 11 | GPIO 10 (Pin 19) |
| SPI MISO | 12 | GPIO 9 (Pin 21) |
| SPI SCK | 13 | GPIO 11 (Pin 23) |

### Step 2: Change Headers

**Arduino:**
```cpp
#include <Arduino.h>
// Or no include needed in .ino files
```

**PiPinPP:**
```cpp
#include "ArduinoCompat.hpp"  // For Arduino functions
#include "exceptions.hpp"      // For error handling
```

### Step 3: Add main() Function

**Arduino** (.ino file):
```cpp
void setup() {
  // Initialization
}

void loop() {
  // Main code
}
// No main() needed - handled by Arduino
```

**PiPinPP** (C++ .cpp file):
```cpp
void setup() {
  // Initialization
}

void loop() {
  // Main code
}

int main() {
  try {
    setup();
    while (true) {
      loop();
    }
  } catch (const PinError& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
```

### Step 4: Update Pin Numbers

```cpp
// Arduino Uno
const int LED_PIN = 13;

// Raspberry Pi  
const int LED_PIN = 18;  // Choose appropriate GPIO
```

### Step 5: Handle Voltage Differences

**CRITICAL**: Raspberry Pi GPIO is **3.3V ONLY**!

| Component | Arduino (5V) | Raspberry Pi (3.3V) | Solution |
|-----------|--------------|---------------------|----------|
| **5V input** | Direct | ❌ **DAMAGE!** | Voltage divider or level shifter |
| **3.3V input** | Direct | ✓ Direct | No change needed |
| **5V device** | Direct | May not work | May need level shifter |
| **LED** | Use 220Ω | Use 220Ω | Same (but lower current) |

**Voltage Divider for 5V Input:**
```
5V Signal ──[10kΩ]── GPIO Pin
                  │
                [20kΩ]
                  │
                 GND
Output: 5V × 20/(10+20) = 3.33V  ✓ Safe
```

## Complete Migration Example

### Original Arduino Code
```cpp
// Arduino Uno sketch
const int LED_PIN = 13;
const int BUTTON_PIN = 2;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Button pressed");
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  delay(50);
}
```

### Migrated PiPinPP Code
```cpp
// Raspberry Pi with PiPinPP
#include "ArduinoCompat.hpp"
#include "exceptions.hpp"
#include <iostream>

const int LED_PIN = 18;     // GPIO 18 instead of pin 13
const int BUTTON_PIN = 17;  // GPIO 17 instead of pin 2

void setup() {
  // Serial not yet implemented - use std::cout
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    digitalWrite(LED_PIN, HIGH);
    std::cout << "Button pressed\n";  // Instead of Serial.println
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  delay(50);
}

int main() {
  try {
    setup();
    while (true) {
      loop();
    }
  } catch (const PinError& e) {
    std::cerr << "GPIO Error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
```

**Changes Made:**
1. ✅ Changed `#include` to PiPinPP headers
2. ✅ Updated pin numbers (13→18, 2→17)
3. ✅ Replaced `Serial.println` with `std::cout`
4. ✅ Added `main()` function with error handling
5. ✅ No other code changes needed!

## Common Issues and Solutions

### Issue: analogRead() Not Working
**Arduino**: Built-in ADC on pins A0-A5
**Solution**: Use external ADC (ADS1115 via I2C)

```cpp
// See i2c_*** examples for ADC usage
```

### Issue: Timing is Different
**Arduino**: 16 MHz, predictable timing
**Raspberry Pi**: ~1.5 GHz, Linux multitasking

**Solution**: Use hardware peripherals (SPI, I2C, PWM) for precise timing

### Issue: EEPROM Not Available
**Arduino**: Built-in EEPROM
**Raspberry Pi**: No EEPROM

**Solution**: Use filesystem or external EEPROM IC

### Issue: Watchdog Timer Needed
**Arduino**: Built-in watchdog
**Raspberry Pi**: Use systemd

```bash
# Create systemd service for auto-restart
```

## What's Supported

| Feature | Arduino | PiPinPP | Notes |
|---------|---------|---------|-------|
| Digital I/O | ✓ | ✓ | Pin numbers differ |
| PWM (analogWrite) | ✓ | ✓ | Software PWM |
| I2C (Wire) | ✓ | ✓ | Fully compatible |
| SPI | ✓ | ✓ | v0.3.7+ |
| Servo | ✓ | Planned | Software control available |
| Serial (UART) | ✓ | Planned | Use Linux serial for now |
| analogRead() | ✓ | Planned | Use external ADC |
| Interrupts | ✓ | ✓ | Fully compatible |
| Tone | ✓ | ✓ | Software implementation |

## Building & Running

```bash
cd /path/to/PiPinPP/build
make example_arduino_migration
./examples/example_arduino_migration
```

## Next Steps

1. Try migrating simple Arduino sketches
2. Start with digital I/O projects
3. Move to I2C sensors (see `i2c_*` examples)
4. Add SPI devices (see `spi_74hc595` example)

## References

- [PiPinPP Getting Started](../../docs/GETTING_STARTED.md)
- [Complete API Reference](../../docs/API_REFERENCE.md)
- [Pin Numbering Guide](../../docs/PIN_NUMBERING.md)
- [Arduino Reference](https://www.arduino.cc/reference/)

## License

Part of the PiPinPP project.
