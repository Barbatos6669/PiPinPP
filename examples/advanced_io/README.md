# Advanced I/O Example

Demonstrates advanced Arduino-compatible I/O functions: `shiftOut()`, `shiftIn()`, `pulseIn()`, `tone()`, and `noTone()`.

## Features

- **shiftOut()** - Serial data output (e.g., 74HC595 shift registers)
- **shiftIn()** - Serial data input (e.g., 74HC165 shift registers)  
- **pulseIn()** - Measure pulse width (e.g., ultrasonic sensors, RC receivers)
- **tone()** / **noTone()** - Generate square wave tones
- All Arduino API compatible

## Building & Running

```bash
cd /path/to/PiPinPP/build
make example_advanced_io
./examples/example_advanced_io
```

## Hardware Examples

### 1. shiftOut() - Control 74HC595 Shift Register
```cpp
const int dataPin = 23;   // DS
const int clockPin = 24;  // SHCP
const int latchPin = 25;  // STCP

pinMode(dataPin, OUTPUT);
pinMode(clockPin, OUTPUT);
pinMode(latchPin, OUTPUT);

digitalWrite(latchPin, LOW);
shiftOut(dataPin, clockPin, MSBFIRST, 0b10101010);
digitalWrite(latchPin, HIGH);  // Latch data to outputs
```

**Wiring**: GPIO 23 → DS (pin 14), GPIO 24 → SHCP (pin 11), GPIO 25 → STCP (pin 12)

### 2. shiftIn() - Read 74HC165 Shift Register
```cpp
const int dataPin = 23;   // Q7
const int clockPin = 24;  // CP
const int loadPin = 25;   // PL

pinMode(dataPin, INPUT);
pinMode(clockPin, OUTPUT);
pinMode(loadPin, OUTPUT);

digitalWrite(loadPin, LOW);   // Load parallel data
digitalWrite(loadPin, HIGH);

uint8_t data = shiftIn(dataPin, clockPin, MSBFIRST);
```

**Use case**: Read 8 buttons with 3 wires

### 3. pulseIn() - Measure Ultrasonic Distance
```cpp
const int trigPin = 23;
const int echoPin = 24;

pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);

// Trigger pulse
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Measure echo
unsigned long duration = pulseIn(echoPin, HIGH, 30000);
float distance = duration * 0.034 / 2;  // Convert to cm

std::cout << "Distance: " << distance << " cm\n";
```

**Wiring**: HC-SR04 ultrasonic sensor

### 4. tone() - Buzzer/Speaker Control
```cpp
const int buzzerPin = 18;

pinMode(buzzerPin, OUTPUT);

tone(buzzerPin, 440);     // Play A4 note (440 Hz)
delay(1000);
tone(buzzerPin, 523);     // Play C5 note (523 Hz)
delay(1000);
noTone(buzzerPin);        // Stop
```

**Wiring**: Piezo buzzer between GPIO 18 and GND

## Function Reference

### shiftOut(dataPin, clockPin, bitOrder, value)
- **bitOrder**: `MSBFIRST` or `LSBFIRST`
- **Speed**: ~200-300 kHz (software bit-banging)
- **Use**: Control multiple outputs with few pins

### shiftIn(dataPin, clockPin, bitOrder)
- **Returns**: uint8_t (8 bits read)
- **Speed**: ~200-300 kHz
- **Use**: Read multiple inputs with few pins

### pulseIn(pin, state, timeout)
- **state**: `HIGH` or `LOW`
- **timeout**: Maximum wait time (µs), 0 = no timeout
- **Returns**: Pulse width in microseconds, 0 if timeout
- **Accuracy**: ±5-10 µs (software limitation)

### tone(pin, frequency)
- **Frequency**: 20 Hz to 20 kHz (audible range)
- **Uses**: Software PWM (blocking)
- **Limitation**: Ties up CPU for duration

## Performance Notes

| Function | Type | Speed | Blocking? |
|----------|------|-------|-----------|
| `shiftOut()` | Software | ~250 kHz | Yes |
| `shiftIn()` | Software | ~250 kHz | Yes |
| `pulseIn()` | Software | ±10 µs accuracy | Yes |
| `tone()` | Software PWM | Good | Yes |

**For better performance**: Use hardware SPI instead of shiftOut() (see `spi_74hc595` example)

## See Also

- **spi_74hc595** - Hardware SPI alternative to shiftOut()
- **button_input** - Basic digital I/O

## References

- [Arduino shiftOut() reference](https://www.arduino.cc/reference/en/language/functions/advanced-io/shiftout/)
- [74HC595 Shift Register Datasheet](https://www.ti.com/lit/ds/symlink/sn74hc595.pdf)
- [PiPinPP API Reference](../../docs/API_REFERENCE.md#advanced-io)

## License

Part of the PiPinPP project.
