# Tutorial: Using an I2C Sensor (BMP280)

Learn how to talk to I2C peripherals by reading temperature and pressure data from a Bosch BMP280.

## Overview
- **What you'll build:** Console dashboard that prints live temperature/pressure
- **Skills you'll learn:** Enabling the Pi's I2C bus, scanning for devices, running the Wire-compatible API
- **Difficulty:** ⭐⭐ Medium
- **Time:** ~20 minutes

## Hardware
- Raspberry Pi with 40-pin header
- BMP280 breakout (3.3 V version)
- Female-to-female jumper wires

### Wiring (3.3 V logic only)
```
BMP280 VIN  -> 3V3 (pin 1)
BMP280 GND  -> GND (pin 6)
BMP280 SCL  -> GPIO3 / SCL (pin 5)
BMP280 SDA  -> GPIO2 / SDA (pin 3)
```

## Steps

1. **Enable I2C** via `raspi-config` (Interface Options → I2C → Enable). Reboot if prompted.

2. **Build PiPin++ with examples**:
   ```bash
   ./build.sh
   cmake --build build --target example_i2c_bmp280
   ```

3. **Scan the bus** (optional sanity check):
   ```bash
   ./build/examples/example_i2c_scanner
   ```
   You should see address `0x76` or `0x77` depending on the sensor's strap pin.

4. **Run the BMP280 example**:
   ```bash
   ./build/examples/example_i2c_bmp280
   ```
   Expect a stream of temperature/pressure readings refreshed every second.

## Code Highlights
- Uses the Arduino-compatible `Wire` API (`Wire.begin`, `Wire.requestFrom`).
- Handles bus selection automatically; override via `Wire.setBus(n)` when needed.
- Demonstrates retry logic for sensors that are still powering up.

## Troubleshooting
- **No devices found:** double-check SDA/SCL wiring and confirm `i2c-bcm2708` is enabled via `lsmod`.
- **Read failures:** confirm voltage (use only 3.3 V BMP280) and verify pull-up resistors if using long wires.
- **Multiple sensors:** update the code to pass the alternate address (0x77) to avoid clashes.

## Next Steps
- Display the readings on an OLED using `examples/i2c_ssd1306/`.
- Log sensor data to CSV and visualize it with the new `examples/performance_profiler/` workflow.
