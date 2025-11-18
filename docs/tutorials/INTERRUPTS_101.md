# Tutorial: Interrupts 101 (Button + LED)

React instantly to user input by wiring a push button that triggers an interrupt-driven LED toggle.

## Overview
- **What you'll build:** Button on GPIO 23 that toggles an LED on GPIO 24 using `attachInterrupt`
- **Skills you'll learn:** Configuring pull-ups, registering callbacks, debouncing
- **Difficulty:** ⭐⭐ Medium
- **Time:** ~25 minutes

## Hardware
- Raspberry Pi
- 1× momentary push button
- 1× LED + 220 Ω resistor
- Jumper wires + breadboard

### Wiring
```
Button leg A -> GPIO23
Button leg B -> GND
LED anode -> GPIO24 -> 220 Ω resistor -> LED -> GND
```
(Use the Pi's internal pull-up to keep the button stable.)

## Steps

1. **Build the example**:
   ```bash
   ./build.sh
   cmake --build build --target example_button_interrupt
   ```

2. **Run it**:
   ```bash
   ./build/examples/example_button_interrupt
   ```
   The console shows interrupt events and the LED toggles immediately when the button is pressed.

3. **Explore the code:** open `examples/button_interrupt/main.cpp` and note:
   - `pinMode(buttonPin, INPUT_PULLUP)` enables the internal pull-up.
   - `attachInterrupt(buttonPin, onPress, FALLING)` registers a callback.
   - A light software debounce ensures bouncy buttons do not spam callbacks.

## Troubleshooting
- **Button fires constantly:** verify that the button leg is wired to GND and use the pull-up mode.
- **No response:** check that the interrupt pin is valid (0–27 on Raspberry Pi) and that your user has GPIO access.
- **Double triggers:** adjust the debounce delay inside the callback.

## Next Steps
- Adapt the logic to count button presses and display the count via the `pipinpp` CLI.
- Combine with the [LED Blink tutorial](LED_BLINK.md) for more complex patterns.
- Explore multi-pin interrupts by wiring additional buttons and using `CHANGE` mode.
