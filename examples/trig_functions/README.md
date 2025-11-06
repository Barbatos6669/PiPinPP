# Trigonometry Functions Example

Arduino-style trigonometric functions using **degrees** (not radians).

## Features

- `sin(degrees)` - Sine function
- `cos(degrees)` - Cosine function
- `tan(degrees)` - Tangent function
- Degree-based (Arduino-compatible)
- Mathematical constants (`PI`, `HALF_PI`, `TWO_PI`)

## Building & Running

```bash
cd /path/to/PiPinPP/build
make example_trig_functions
./examples/example_trig_functions
```

## Arduino vs Standard C++

| Arduino (This Library) | Standard C++ |
|------------------------|--------------|
| `sin(90)` = 1.0 | `std::sin(M_PI/2)` = 1.0 |
| `cos(180)` = -1.0 | `std::cos(M_PI)` = -1.0 |
| **Degrees** | **Radians** |

Arduino functions use degrees because they're more intuitive for robotics and hardware control.

## Common Uses

### 1. Circular Motion
```cpp
for (int angle = 0; angle < 360; angle++) {
    float x = 100 * cos(angle);  // X position
    float y = 100 * sin(angle);  // Y position
    moveRobotTo(x, y);
    delay(10);
}
```

### 2. Servo Sweep
```cpp
int centerAngle = 90;
int amplitude = 30;

for (int t = 0; t < 360; t++) {
    int servoAngle = centerAngle + amplitude * sin(t);
    servo.write(servoAngle);
    delay(15);
}
// Smooth sine wave motion
```

### 3. Calculate Distances
```cpp
float distance = 100;  // cm
float angle = 45;      // degrees

float horizontal = distance * cos(angle);  // 70.7 cm
float vertical = distance * sin(angle);    // 70.7 cm
```

### 4. LED Brightness Wave
```cpp
for (int angle = 0; angle < 360; angle++) {
    float brightness = (sin(angle) + 1) * 127.5;  // 0-255
    analogWrite(LED_PIN, (int)brightness);
    delay(10);
}
```

## Mathematical Constants

```cpp
PI         // 3.14159265358979323846
HALF_PI    // π/2 = 1.5707963...
TWO_PI     // 2π = 6.2831853...
DEG_TO_RAD // π/180 (convert degrees to radians)
RAD_TO_DEG // 180/π (convert radians to degrees)
```

## Quick Reference

| Angle (°) | sin | cos | tan |
|-----------|-----|-----|-----|
| 0° | 0 | 1 | 0 |
| 30° | 0.5 | 0.866 | 0.577 |
| 45° | 0.707 | 0.707 | 1 |
| 60° | 0.866 | 0.5 | 1.732 |
| 90° | 1 | 0 | ∞ |
| 180° | 0 | -1 | 0 |
| 270° | -1 | 0 | ∞ |

## See Also

- **math_functions** - Basic math operations
- **utility_functions** - Other helper functions

## References

- [Arduino Reference](https://www.arduino.cc/reference/en/language/functions/trigonometry/)
- [Trigonometry Basics](https://www.mathsisfun.com/sine-cosine-tangent.html)

## License

Part of the PiPinPP project.
