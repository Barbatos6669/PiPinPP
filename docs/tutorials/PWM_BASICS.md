# Tutorial: PWM Basics (LED Fade)

Use PiPin++ software PWM to create smooth LED fades and understand duty cycle concepts.

## Overview
- **Project:** Breathing LED using `analogWrite`
- **Skills:** PWM fundamentals, duty cycle tuning, timing loops
- **Difficulty:** ⭐⭐ Medium
- **Time:** ~20 minutes

## Hardware
- Raspberry Pi (any 40-pin model)
- 1× LED + 220 Ω resistor
- Jumper wires + breadboard

### Wiring
```
GPIO18 (or any PWM-capable pin) -> resistor -> LED -> GND
```
GPIO18 is a hardware PWM pin, but this tutorial intentionally uses the software PWM API so it works on every pin.

## Steps

1. **Build the example:**
   ```bash
   ./scripts/build.sh --debug
   cmake --build build --target example_led_fade
   ```

2. **Run it:**
   ```bash
   ./build/examples/example_led_fade
   ```
   The LED should fade in/out smoothly.

3. **Experiment:** Open `examples/led_fade/main.cpp` and tweak:
   - `analogWrite(pin, value)` range (0-255)
   - Step size or delay to change fade speed
   - Pin numbers to drive multiple LEDs

## Concepts
- **Duty cycle:** Fraction of time spent HIGH per cycle; PiPin++ maps 0-255 → 0-100%.
- **Software PWM:** Implemented via background threads; suitable for LEDs, not servos.
- **Hardware PWM:** For precise timing (see `docs/API_REFERENCE.md#hardware-pwm`).

## Troubleshooting
- **Flicker or jitter:** Ensure your Pi isn’t overloaded; software PWM needs CPU.
- **Permission errors:** Run under a user in the `gpio` group or use `sudo`.
- **Slow fades:** Reduce the delay between writes or increase step size.

## Next Steps
- Mix multiple channels to build an RGB LED controller.
- Switch to `HardwarePWM` for servos or frequency-critical loads.
- Combine with sensor input to react to ambient light.
