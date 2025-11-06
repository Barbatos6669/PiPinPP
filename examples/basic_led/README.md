# Basic LED Blink Example

The "Hello World" of GPIO programming - blink an LED using Arduino-style functions.

## Features

- Simple LED blink using `digitalWrite()` and `delay()`
- Demonstrates basic GPIO output
- Arduino-compatible code that works on Raspberry Pi
- Perfect first example for beginners

## Hardware Requirements

### Components
- 1x LED (any color)
- 1x 220Ω resistor (for current limiting)
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
- **GPIO 18** (Physical Pin 12) → LED anode (long leg) → 220Ω resistor → LED cathode (short leg) → GND

**Important**: 
- GPIO pins are 3.3V (NOT 5V)
- Always use a current-limiting resistor with LEDs
- LED polarity matters: long leg = positive (anode), short leg = negative (cathode)

## Building

From the PiPinPP build directory:
```bash
cd /path/to/PiPinPP/build
make example_basic_led
```

## Running

```bash
cd build/examples
./example_basic_led
```

## Expected Output

```
Starting LED blink on GPIO 18
LED will blink every second.
Press Ctrl+C to stop.

LED ON
LED OFF
LED ON
LED OFF
...
```

The LED will blink on for 1 second, off for 1 second, continuously.

## Understanding the Code

Key concepts demonstrated:

```cpp
pinMode(LED_PIN, OUTPUT);        // Configure pin as output
digitalWrite(LED_PIN, HIGH);     // Turn LED on (3.3V)
delay(1000);                     // Wait 1000 milliseconds (1 second)
digitalWrite(LED_PIN, LOW);      // Turn LED off (0V)
```

## Troubleshooting

### LED doesn't light up
- Check LED polarity (long leg should be connected to GPIO via resistor)
- Verify resistor is 220Ω (red-red-brown bands)
- Check wiring connections
- Try a different LED (might be burned out)

### Permission errors
```bash
# Add user to gpio group
sudo usermod -a -G gpio $USER
# Log out and back in, or:
newgrp gpio
```

### GPIO in use
```bash
# Check if another process is using the pin
sudo lsof | grep gpiochip
# Kill conflicting process if needed
```

## Extending This Example

### Change Blink Speed
Modify the delay values:
```cpp
delay(100);  // Fast blink (0.1 seconds)
delay(2000); // Slow blink (2 seconds)
```

### Use Different Pin
Change the LED_PIN constant:
```cpp
const int LED_PIN = 17;  // Use GPIO 17 instead
```

### Multiple LEDs
Connect more LEDs to different pins:
```cpp
const int LED1 = 18;
const int LED2 = 23;
const int LED3 = 24;

pinMode(LED1, OUTPUT);
pinMode(LED2, OUTPUT);
pinMode(LED3, OUTPUT);

// Blink in sequence...
```

### SOS Pattern
```cpp
// S: dot-dot-dot
for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
}
delay(400);

// O: dash-dash-dash
for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(600);
    digitalWrite(LED_PIN, LOW);
    delay(200);
}
```

## Next Steps

After mastering this example, try:
- **button_input** - Read button presses
- **led_fade** - Smooth LED fading with PWM
- **button_interrupt** - React to button presses using interrupts

## References

- [GPIO Pin Numbering Guide](../../docs/PIN_NUMBERING.md)
- [PiPinPP API Reference](../../docs/API_REFERENCE.md)
- [Arduino digitalWrite() reference](https://www.arduino.cc/reference/en/language/functions/digital-io/digitalwrite/)

## License

Part of the PiPinPP project - see LICENSE file for details.
