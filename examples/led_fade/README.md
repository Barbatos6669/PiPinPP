# LED Fade Example

Smoothly fade an LED brightness using software PWM (Pulse Width Modulation).

## Features

- Software PWM for LED brightness control
- Smooth fading animations
- `analogWrite()` Arduino compatibility
- Demonstrates duty cycle concepts
- Visual feedback of PWM in action

## Hardware Requirements

### Components
- 1x LED (any color)
- 1x 220Ω resistor
- Breadboard and jumper wires
- Raspberry Pi (any model)

### Wiring Diagram

```
Raspberry Pi                    LED
┌──────────┐                  ┌─────┐
│          │                  │ ──> │ (LED)
│ GPIO 18  ├──────[220Ω]─────┤     │
│  (Pin 12)│                  │ ──> │
│          │                  └──┬──┘
│  GND     ├─────────────────────┘
│  (Pin 6) │
└──────────┘
```

### Pin Connection
- **GPIO 18** (Physical Pin 12) → 220Ω resistor → LED anode (long leg)
- LED cathode (short leg) → GND

## Building

```bash
cd /path/to/PiPinPP/build
make example_led_fade
```

## Running

```bash
cd build/examples
./example_led_fade
```

## Expected Output

```
LED Fade Example using Software PWM
LED on GPIO 18 will smoothly fade in and out.
Press Ctrl+C to stop.

Fading up... (brightness: 0)
Fading up... (brightness: 51)
Fading up... (brightness: 102)
Fading up... (brightness: 153)
Fading up... (brightness: 204)
Fading up... (brightness: 255)
Fading down... (brightness: 204)
Fading down... (brightness: 153)
...
```

You'll see the LED gradually brighten, then dim, in a smooth breathing pattern.

## Understanding PWM

### What is PWM?
Pulse Width Modulation rapidly switches the LED on/off to control perceived brightness:

```
0% Duty Cycle (OFF):
___________________________________________________

25% Duty Cycle (DIM):
█▁▁▁█▁▁▁█▁▁▁█▁▁▁█▁▁▁█▁▁▁█▁▁▁█▁▁▁█▁▁▁█▁▁▁

50% Duty Cycle (MEDIUM):
██▁▁██▁▁██▁▁██▁▁██▁▁██▁▁██▁▁██▁▁██▁▁██▁▁

75% Duty Cycle (BRIGHT):
███▁███▁███▁███▁███▁███▁███▁███▁███▁███▁

100% Duty Cycle (FULL):
███████████████████████████████████████████████
```

The LED switches faster than your eye can see, creating the illusion of varying brightness.

### The Code
```cpp
pinMode(LED_PIN, OUTPUT);

// Fade in
for (int brightness = 0; brightness <= 255; brightness += 5) {
    analogWrite(LED_PIN, brightness);  // Set PWM duty cycle
    delay(30);                         // Smooth transition
}

// Fade out
for (int brightness = 255; brightness >= 0; brightness -= 5) {
    analogWrite(LED_PIN, brightness);
    delay(30);
}
```

`analogWrite()` accepts values 0-255:
- **0** = LED off (0% duty cycle)
- **127** = LED at 50% brightness
- **255** = LED fully on (100% duty cycle)

## Software PWM Limitations

**Important**: This example uses software PWM, which has limitations:

| Aspect | Software PWM | Hardware PWM |
|--------|--------------|--------------|
| **Accuracy** | ±1-5ms jitter | Precise timing |
| **CPU Usage** | Moderate | Near zero |
| **Frequency** | ~100-500 Hz | Up to 125 MHz |
| **Pins** | Any GPIO | Limited pins |
| **Smoothness** | Visible flicker possible | Perfectly smooth |

**For production use**: Consider hardware PWM (see v0.3.7+ roadmap).

## Troubleshooting

### LED flickers instead of smooth fading
- **Expected with software PWM** on loaded systems
- Try running example alone (close other programs)
- Increase priority: `sudo nice -n -20 ./example_led_fade`
- Use hardware PWM for critical applications

### Brightness doesn't change much
- Some LED colors (blue, white) have narrow visible range
- Try red or amber LEDs for best effect
- Check resistor value (220Ω recommended)

### LED is too bright even at low values
- Use larger resistor (470Ω or 1kΩ)
- Reduce maximum brightness in code:
  ```cpp
  for (int brightness = 0; brightness <= 100; brightness += 5)
  ```

### CPU usage high
- Software PWM runs background thread
- Normal for software-based approach
- Hardware PWM eliminates this (future feature)

## Extending This Example

### Change Fade Speed
```cpp
delay(10);   // Faster fade (10ms between steps)
delay(100);  // Slower fade (100ms between steps)
```

### Breathing Effect
```cpp
while (true) {
    // Fade in (fast)
    for (int b = 0; b <= 255; b += 10) {
        analogWrite(LED_PIN, b);
        delay(20);
    }
    
    // Hold bright
    delay(500);
    
    // Fade out (slow)
    for (int b = 255; b >= 0; b -= 3) {
        analogWrite(LED_PIN, b);
        delay(30);
    }
    
    // Hold dark
    delay(1000);
}
```

### Multiple LEDs (RGB)
```cpp
const int RED_PIN = 18;
const int GREEN_PIN = 23;
const int BLUE_PIN = 24;

analogWrite(RED_PIN, 255);    // Full red
analogWrite(GREEN_PIN, 127);  // Half green
analogWrite(BLUE_PIN, 0);     // No blue
// Result: Orange color
```

### Map to Sensor Value
```cpp
int sensorValue = analogRead(SENSOR_PIN);  // 0-1023
int brightness = map(sensorValue, 0, 1023, 0, 255);
analogWrite(LED_PIN, brightness);
```

## Next Steps

After mastering this example, try:
- **button_input** - Control fade with buttons
- **timing_benchmark** - Measure PWM timing accuracy
- **advanced_io** - Use `pulseIn()` for input signals

## References

- [PiPinPP PWM Documentation](../../docs/API_REFERENCE.md#pwm-functions)
- [PWM Tutorial (Coming Soon)](../../docs/tutorials/)
- [Arduino analogWrite() reference](https://www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/)
- [PWM Explained](https://learn.sparkfun.com/tutorials/pulse-width-modulation)

## License

Part of the PiPinPP project - see LICENSE file for details.
