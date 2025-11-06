# Utility Functions Example

Bit manipulation, random numbers, and other Arduino-compatible utility functions.

## Features

- **Bit operations**: `bit()`, `bitRead()`, `bitWrite()`, `bitSet()`, `bitClear()`
- **Byte operations**: `highByte()`, `lowByte()`
- **Random**: `random()`, `randomSeed()`
- Arduino API compatible

## Building & Running

```bash
cd /path/to/PiPinPP/build
make example_utility_functions
./examples/example_utility_functions
```

## Bit Manipulation

### Reading and Setting Bits
```cpp
uint8_t value = 0b10110100;  // Binary notation

bool bit5 = bitRead(value, 5);  // Read bit 5 → true (1)
bitWrite(value, 2, 1);           // Set bit 2 to 1
bitSet(value, 0);                // Set bit 0 to 1
bitClear(value, 7);              // Clear bit 7 to 0

uint8_t mask = bit(3);           // Create mask: 0b00001000
```

### Common Uses

**Reading sensor flags:**
```cpp
uint8_t status = readSensorStatus();
bool dataReady = bitRead(status, 0);
bool overflow = bitRead(status, 7);
```

**Building register values:**
```cpp
uint8_t config = 0;
bitSet(config, 0);  // Enable bit
bitSet(config, 3);  // Set option
// config = 0b00001001
writeSensorConfig(config);
```

## Byte Operations

### Split 16-bit Values
```cpp
uint16_t value = 0xABCD;

uint8_t high = highByte(value);  // 0xAB
uint8_t low = lowByte(value);    // 0xCD

// Useful for:
Wire.write(highByte(address));  // Send high byte
Wire.write(lowByte(address));   // Send low byte
```

### Combine Bytes
```cpp
uint8_t high = Wire.read();
uint8_t low = Wire.read();

uint16_t value = (high << 8) | low;  // Reconstruct 16-bit value
```

## Random Numbers

### Generate Random Values
```cpp
randomSeed(millis());  // Seed with current time

int dice = random(1, 7);        // Random 1-6 (dice roll)
int led = random(0, 256);       // Random 0-255 (brightness)
int pin = random(18, 28);       // Random GPIO pin

// Random boolean:
bool coinFlip = random(2);      // 0 or 1
```

### Random Blink Pattern
```cpp
while (true) {
    digitalWrite(LED_PIN, HIGH);
    delay(random(100, 1000));    // Random on time
    
    digitalWrite(LED_PIN, LOW);
    delay(random(100, 1000));    // Random off time
}
```

## Real-World Examples

### Parse I2C Response
```cpp
// Read 16-bit temperature from sensor
Wire.requestFrom(address, 2);
uint8_t msb = Wire.read();
uint8_t lsb = Wire.read();

int16_t temp = (msb << 8) | lsb;

// Or use highByte/lowByte to send:
Wire.write(highByte(temp));
Wire.write(lowByte(temp));
```

### Build SPI Command
```cpp
uint8_t command = 0;
bitSet(command, 7);      // Read operation
bitSet(command, 6);      // Auto-increment
command |= (register & 0x3F);  // Register address

SPI.transfer(command);
```

### Random LED Effects
```cpp
for (int i = 0; i < 20; i++) {
    int randomLED = random(0, NUM_LEDS);
    digitalWrite(ledPins[randomLED], HIGH);
    delay(random(50, 200));
    digitalWrite(ledPins[randomLED], LOW);
}
```

## Bit Manipulation Quick Reference

| Function | Example | Result |
|----------|---------|--------|
| `bit(n)` | `bit(3)` | `0b00001000` (mask) |
| `bitRead(x, n)` | `bitRead(0b1010, 1)` | `1` |
| `bitWrite(x, n, b)` | `bitWrite(0b1010, 0, 1)` | `0b1011` |
| `bitSet(x, n)` | `bitSet(0b1010, 0)` | `0b1011` |
| `bitClear(x, n)` | `bitClear(0b1010, 3)` | `0b0010` |

## See Also

- **math_functions** - Mathematical operations
- **advanced_io** - `shiftOut()`, `shiftIn()` for serial data
- **i2c_***  - Examples using byte operations

## References

- [Arduino Bit Manipulation](https://www.arduino.cc/reference/en/language/functions/bits-and-bytes/)
- [Binary and Hexadecimal](https://learn.sparkfun.com/tutorials/binary)
- [PiPinPP API Reference](../../docs/API_REFERENCE.md#utility-functions)

## License

Part of the PiPinPP project.
