# Arduino Style Example

Pure Arduino code running on Raspberry Pi - demonstrates API compatibility.

## Features

- 100% Arduino-compatible syntax
- No platform-specific code
- `setup()` and `loop()` pattern
- Perfect for Arduino→Pi porting
- Shows API parity

## What This Demonstrates

This example intentionally uses **only** Arduino functions to prove you can run Arduino sketches on Raspberry Pi with minimal changes:

```cpp
void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(9600);  // (Serial not yet implemented)
}

void loop() {
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    delay(1000);
}
```

**No Raspberry Pi specific code needed!**

## Hardware Requirements

### Components
- 1x LED
- 1x 220Ω resistor
- Breadboard and jumper wires

### Wiring
Same as `basic_led` example:
- **GPIO 18** → 220Ω resistor → LED → GND

## Building

```bash
cd /path/to/PiPinPP/build
make example_arduino_style
```

## Running

```bash
cd build/examples
./example_arduino_style
```

## Expected Output

```
Starting Arduino-style program...
LED Blink - Arduino Compatible Code
Running setup()...
Pin 18 configured as OUTPUT.

Entering loop()...
LED ON
LED OFF
LED ON
LED OFF
...
```

## Arduino API Compatibility

### Implemented Functions

| Category | Functions |
|----------|-----------|
| **Digital I/O** | `pinMode()`, `digitalWrite()`, `digitalRead()` |
| **Analog I/O** | `analogWrite()` (PWM), `analogRead()` (planned) |
| **Time** | `delay()`, `delayMicroseconds()`, `millis()`, `micros()` |
| **Math** | `map()`, `constrain()`, `min()`, `max()`, `abs()`, `pow()`, `sqrt()` |
| **Trigonometry** | `sin()`, `cos()`, `tan()` (degree-based) |
| **Random** | `random()`, `randomSeed()` |
| **Bits/Bytes** | `bit()`, `bitRead()`, `bitWrite()`, `bitSet()`, `bitClear()`, `highByte()`, `lowByte()` |
| **Advanced I/O** | `shiftOut()`, `shiftIn()`, `pulseIn()`, `tone()`, `noTone()` |
| **Communication** | `Wire` (I2C), `SPI` (v0.3.7+) |
| **Interrupts** | `attachInterrupt()`, `detachInterrupt()` |

### Not Yet Implemented
- Serial/UART communication (`Serial.begin()`, `Serial.print()`, etc.)
- `analogRead()` for ADC
- Hardware PWM (`analogWriteResolution()`)
- EEPROM

## Porting Arduino Code to Raspberry Pi

### Step 1: Replace Arduino-specific hardware references
```cpp
// Arduino
#define LED_PIN 13  // Built-in LED

// Raspberry Pi
#define LED_PIN 18  // Choose any GPIO pin
```

### Step 2: Adjust voltage levels
```cpp
// Arduino uses 5V logic
// Raspberry Pi uses 3.3V logic
// Use voltage divider for 5V inputs!
```

### Step 3: Compile with PiPinPP
```cpp
#include "ArduinoCompat.hpp"  // Instead of Arduino.h

// Rest of Arduino code unchanged!
```

### Step 4: Test and adjust timing
```cpp
// Arduino runs at 16 MHz
// Raspberry Pi is MUCH faster
// May need to adjust delays in timing-critical code
```

## Common Porting Issues

### Issue: Pin numbers different
**Arduino**: Digital pins 0-13, analog A0-A5
**Raspberry Pi**: GPIO numbers 0-27 (BCM numbering)

**Solution**: Update pin definitions at top of sketch

### Issue: Voltage mismatch
**Arduino**: 5V tolerant
**Raspberry Pi**: 3.3V ONLY (5V will damage pins!)

**Solution**: Use level shifters or voltage dividers for 5V devices

### Issue: No built-in ADC
**Arduino**: `analogRead()` on A0-A5
**Raspberry Pi**: No built-in ADC

**Solution**: Use external ADC (ADS1115 via I2C)

### Issue: Timing differences
**Arduino**: Predictable timing (16 MHz)
**Raspberry Pi**: Multitasking OS causes jitter

**Solution**: Use hardware peripherals (SPI, I2C) instead of bit-banging

## Example: Porting a Simple Arduino Sketch

### Original Arduino Code:
```cpp
// Arduino sketch
const int LED = 13;
const int BUTTON = 2;

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(BUTTON) == LOW) {
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }
}
```

### Ported to Raspberry Pi:
```cpp
// Raspberry Pi with PiPinPP
#include "ArduinoCompat.hpp"

const int LED = 18;     // GPIO 18 instead of 13
const int BUTTON = 17;  // GPIO 17 instead of 2

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(BUTTON) == LOW) {
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }
}

int main() {
  setup();
  while (true) {
    loop();
  }
  return 0;
}
```

**Only 3 changes needed:**
1. Include `ArduinoCompat.hpp`
2. Update pin numbers
3. Add `main()` function

## Libraries Compatibility

### Already Working
- Wire (I2C)
- SPI
- Servo (software, future hardware)

### Planned
- Serial (UART)
- Ethernet/WiFi (use Linux networking)
- SD card (use standard filesystem)

### Not Applicable
- EEPROM → Use filesystem
- Watchdog → Use systemd
- Power management → Use Linux power management

## Next Steps

Try porting your own Arduino sketches! Start with:
1. Simple digital I/O sketches
2. Sensor reading projects
3. I2C device communication

See also:
- **arduino_migration** - Detailed migration guide
- **i2c_***  - I2C device examples (Arduino library compatible)

## References

- [Arduino Migration Guide](../../docs/GETTING_STARTED.md)
- [Arduino→PiPinPP API Mapping](../../docs/API_REFERENCE.md)
- [Pin Numbering Differences](../../docs/PIN_NUMBERING.md)

## License

Part of the PiPinPP project - see LICENSE file for details.
