# Tutorial: Reading a Button (Digital Input)

Learn how to read button presses reliably using PiPin++ digital inputs and pull resistors.

## Overview
- **Project:** Console prints whenever a button is pressed
- **Skills:** `pinMode` with pull-ups, `digitalRead`, debouncing basics
- **Difficulty:** ⭐ Easy
- **Time:** ~10 minutes

## Hardware
- Raspberry Pi
- Momentary push button
- Jumper wires + breadboard

### Wiring
```
Button leg A -> GPIO23
Button leg B -> GND
```
We'll use the Pi's internal pull-up so no resistor is required.

## Steps

1. **Build the example:**
   ```bash
   ./scripts/build.sh
   cmake --build build --target example_button_input
   ```

2. **Run it:**
   ```bash
   ./build/examples/example_button_input
   ```
   Press the button to see “Pressed!” output in the terminal.

3. **Inspect the code:**
   - `pinMode(buttonPin, INPUT_PULLUP)` keeps the line HIGH when idle.
   - `digitalRead` returns LOW when the button closes to ground.
   - Simple `delay(20)` debounce prevents multiple triggers.

## Troubleshooting
- **Constantly triggered?** Make sure the button leg goes to ground, not 3V3.
- **No response?** Confirm you used a valid GPIO number (see `docs/PIN_NUMBERING.md`).
- **Bouncy output?** Increase the debounce delay or add software filtering.

## Next Steps
- Combine with `LED_BLINK` to control LED toggling via button.
- Use `attachInterrupt` (see `INTERRUPTS_101.md`) for instant responses.
- Count presses and send the value over Serial/I2C to another device.
