# Button Input Example

Learn to read digital inputs by detecting button presses with internal pull-up resistors.

## Features

- Read button state with `digitalRead()`
- Use internal pull-up resistor (no external resistor needed!)
- Debouncing to eliminate false triggers
- Simple button press detection
- Arduino-compatible code

## Hardware Requirements

### Components
- 1x Pushbutton (normally open, momentary)
- 1x LED (optional, for visual feedback)
- 1x 220Ω resistor (for LED)
- Breadboard and jumper wires
- Raspberry Pi (any model)

### Wiring Diagram

```
Raspberry Pi              Button             LED (optional)
┌──────────┐              ┌───┐            ┌─────┐
│          │              │   │            │ ──> │
│ GPIO 17  ├──────────────┤ o │            │     │
│ (Pin 11) │              │ o ├───GND      │ ──> │
│          │              └───┘            └──┬──┘
│ GPIO 18  ├────[220Ω]───────────────────────┘
│ (Pin 12) │                                  
│  GND     ├──────────────────────────────GND
│  (Pin 6) │
└──────────┘
```

### Pin Connections
- **GPIO 17** (Physical Pin 11) → One side of button
- **GND** → Other side of button
- **GPIO 18** (Physical Pin 12) → LED anode → 220Ω resistor → LED cathode → GND

**Why No Pull-up Resistor?**
The code uses `INPUT_PULLUP` mode, which enables the Raspberry Pi's internal pull-up resistor (~50kΩ). This pulls the pin HIGH when the button is not pressed. Pressing the button connects the pin to GND, reading as LOW.

## Building

```bash
cd /path/to/PiPinPP/build
make example_button_input
```

## Running

```bash
cd build/examples
./example_button_input
```

## Expected Output

```
Button Input Example - Press button on GPIO 17
LED on GPIO 18 will mirror button state (LED on when button pressed)
Using internal pull-up resistor - no external resistor needed!

Button PRESSED!
Button PRESSED!
Button RELEASED!
Button PRESSED!
...
```

## Understanding the Code

### Pull-up Resistor Configuration
```cpp
pinMode(BUTTON_PIN, INPUT_PULLUP);  // Enable internal pull-up
```

This keeps the pin HIGH (reading 1) when the button is not pressed. Pressing the button connects the pin to GND, reading LOW (0).

### Reading Button State
```cpp
int buttonState = digitalRead(BUTTON_PIN);

if (buttonState == LOW) {
    // Button is PRESSED (connected to GND)
} else {
    // Button is RELEASED (pulled high by internal resistor)
}
```

### Debouncing
Physical buttons "bounce" - make multiple rapid contacts when pressed. The code uses a simple delay-based debounce:

```cpp
if (buttonState != lastButtonState) {
    delay(DEBOUNCE_DELAY);  // Wait for bouncing to settle
    buttonState = digitalRead(BUTTON_PIN);  // Read again
}
```

## Troubleshooting

### Button reading is inverted
This is normal! With `INPUT_PULLUP`:
- **Button not pressed** → reads HIGH (1)
- **Button pressed** → reads LOW (0)

This is why the code checks `if (buttonState == LOW)` for button pressed.

### Multiple triggers from one press
- Increase `DEBOUNCE_DELAY` (try 50-100ms)
- Check button quality (worn buttons bounce more)
- Consider using interrupts (see `button_interrupt` example)

### Button doesn't respond
- Verify button is normally-open type (not normally-closed)
- Check wiring connections
- Test button with multimeter (should show continuity when pressed)

### Permission errors
```bash
sudo usermod -a -G gpio $USER
newgrp gpio  # Or log out and back in
```

## Pull-up vs Pull-down

**INPUT_PULLUP (used in this example):**
```
3.3V ──[~50kΩ]── GPIO 17 ──[Button]── GND
```
- Not pressed: reads HIGH
- Pressed: reads LOW (connected to GND)
- **Advantage**: No external resistor needed

**INPUT_PULLDOWN (alternative):**
```
GND ──[~50kΩ]── GPIO 17 ──[Button]── 3.3V
```
- Not pressed: reads LOW
- Pressed: reads HIGH (connected to 3.3V)
- Less common, but more intuitive

## Extending This Example

### Count Button Presses
```cpp
int pressCount = 0;

if (buttonState == LOW && lastButtonState == HIGH) {
    pressCount++;
    std::cout << "Press count: " << pressCount << "\n";
}
```

### Toggle LED on Each Press
```cpp
if (buttonState == LOW && lastButtonState == HIGH) {
    ledState = !ledState;  // Flip state
    digitalWrite(LED_PIN, ledState);
}
```

### Long Press Detection
```cpp
if (buttonState == LOW) {
    pressTime++;
    if (pressTime > 3000) {  // 3 seconds
        std::cout << "Long press detected!\n";
    }
}
```

### Multiple Buttons
```cpp
const int BUTTON1 = 17;
const int BUTTON2 = 27;
const int BUTTON3 = 22;

pinMode(BUTTON1, INPUT_PULLUP);
pinMode(BUTTON2, INPUT_PULLUP);
pinMode(BUTTON3, INPUT_PULLUP);

// Read all buttons...
```

## Next Steps

After mastering this example, try:
- **button_interrupt** - Use interrupts instead of polling
- **led_fade** - Control LED brightness with button presses
- **thread_safety** - Safe multi-threaded button handling

## References

- [GPIO Pin Numbering Guide](../../docs/PIN_NUMBERING.md)
- [PiPinPP API Reference](../../docs/API_REFERENCE.md)
- [Button Debouncing Tutorial](../../docs/tutorials/)
- [Arduino digitalRead() reference](https://www.arduino.cc/reference/en/language/functions/digital-io/digitalread/)

## License

Part of the PiPinPP project - see LICENSE file for details.
