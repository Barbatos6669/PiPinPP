# Tutorial: Controlling an LED

Build confidence with PiPin++ by blinking a single LED in less than 10 minutes.

## Overview
- **What you'll build:** Classic LED blink on GPIO 17
- **Skills you'll learn:** Configuring pin modes, driving digital outputs, compiling/running examples
- **Difficulty:** ⭐ Easy
- **Time:** ~10 minutes

## Hardware
- Raspberry Pi with 40-pin header (Pi 4/3/Zero supported)
- 1× LED
- 1× 220 Ω resistor
- Breadboard + jumper wires

### Wiring
```
GPIO17 ──> resistor ──> LED anode (+)
LED cathode (-) ───────> GND
```

## Steps

1. **Enable GPIO permissions** (once per user):
   ```bash
   sudo usermod -a -G gpio "$USER"
   ```
   Log out/in so the group change takes effect.

2. **Build the example** (from the repo root):
   ```bash
   ./build.sh --debug
   cmake --build build --target example_basic_led
   ```

3. **Run the demo:**
   ```bash
   ./build/examples/example_basic_led
   ```
   The LED should blink at one-second intervals.

4. **Make it your own:** Edit `examples/basic_led/main.cpp` to change the pin number, interval, or add multiple LEDs.

## Key Concepts
- `pinMode(pin, OUTPUT)` configures GPIO as a digital output.
- `digitalWrite(pin, HIGH/LOW)` drives the pin high or low.
- `delay(ms)` keeps timing simple and readable.

## Troubleshooting
- **LED always off:** flip the LED orientation (long leg is anode) and confirm resistor wiring.
- **Permission denied:** rerun the program with `sudo` or ensure your user is in the `gpio` group.
- **Wrong pin:** verify the physical pin mapping via `docs/PIN_NUMBERING.md`.

## Next Steps
- Chain three LEDs together using the [Traffic Light](TRAFFIC_LIGHT.md) tutorial.
- Control brightness with PWM using the `examples/led_fade/` example.
