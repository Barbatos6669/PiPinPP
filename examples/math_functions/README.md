# Math Functions Example

Arduino-compatible mathematical functions for calculations in embedded projects.

## Features

- `map()` - Linear interpolation/scaling
- `constrain()` - Limit values to range
- `min()`, `max()` - Find extremes
- `abs()` - Absolute value
- `pow()` - Exponentiation
- `sqrt()` - Square root
- Arduino API compatible

## Building & Running

```bash
cd /path/to/PiPinPP/build
make example_math_functions
./examples/example_math_functions
```

## Functions Demonstrated

### 1. `map(value, fromLow, fromHigh, toLow, toHigh)`
Scale a value from one range to another:

```cpp
int sensor = 512;  // ADC reading (0-1023)
int brightness = map(sensor, 0, 1023, 0, 255);
// Maps 512 → 127 (halfway point)

int temp = map(analogRead(SENSOR), 0, 1023, -40, 125);
// Convert ADC to temperature range
```

**Common Uses:**
- Scale sensor readings
- Convert between units
- Map joystick input to servo angle

### 2. `constrain(value, min, max)`
Limit a value to a specific range:

```cpp
int speed = constrain(userInput, 0, 255);
// Ensures speed is between 0-255

int angle = constrain(reading, -90, 90);
// Clamp angle to valid range
```

### 3. `min(a, b)` and `max(a, b)`
Find minimum or maximum of two values:

```cpp
int lowest = min(temp1, temp2);
int highest = max(speed1, speed2);

// Prevent overflow:
int sum = min(a + b, 255);
```

### 4. Mathematical Operations
```cpp
int absolute = abs(-42);        // → 42
float power = pow(2, 8);        // → 256.0
float root = sqrt(144);         // → 12.0
```

## Real-World Example: Sensor Smoothing

```cpp
const int NUM_READINGS = 10;
int readings[NUM_READINGS];
int index = 0;

void loop() {
    // Read sensor
    readings[index] = analogRead(SENSOR_PIN);
    index = (index + 1) % NUM_READINGS;
    
    // Calculate average
    int sum = 0;
    for (int i = 0; i < NUM_READINGS; i++) {
        sum += readings[i];
    }
    int average = sum / NUM_READINGS;
    
    // Map to output range and constrain
    int output = map(average, 0, 1023, 0, 255);
    output = constrain(output, 50, 200);  // Limit range
    
    analogWrite(LED_PIN, output);
}
```

## See Also

- **trig_functions** - Trigonometry functions
- **utility_functions** - Bit manipulation, random numbers
- **led_fade** - Uses `map()` for brightness control

## References

- [Arduino Math Reference](https://www.arduino.cc/reference/en/language/functions/math/)
- [PiPinPP API Reference](../../docs/API_REFERENCE.md#mathematical-functions)

## License

Part of the PiPinPP project.
