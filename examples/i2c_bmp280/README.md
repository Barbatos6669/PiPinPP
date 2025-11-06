# BMP280 Pressure/Temperature Sensor Example

Read atmospheric pressure and temperature from a Bosch BMP280 sensor via I2C.

## Features

- Read temperature (°C and °F)
- Read barometric pressure (hPa)
- Calculate approximate altitude
- Arduino Wire library compatible
- Polling-based reading

## Hardware Setup

**BMP280 Connections:**
- VCC → 3.3V
- GND → GND
- SDA → GPIO 2 (Pin 3)
- SCL → GPIO 3 (Pin 5)

**I2C Address**: 0x76 (or 0x77 with SDO pulled high)

## Enable I2C

```bash
sudo raspi-config
# Interface Options → I2C → Enable

# Verify:
sudo i2cdetect -y 1
# Should show 76 or 77
```

## Building & Running

```bash
cd /path/to/PiPinPP/build
make example_i2c_bmp280
./examples/example_i2c_bmp280
```

## Expected Output

```
BMP280 Sensor Reading Example
==============================

Initializing BMP280...
Chip ID: 0x58 (BMP280 detected)

Temperature: 22.4°C (72.3°F)
Pressure: 1013.2 hPa
Altitude: ~150.3 m

Temperature: 22.5°C (72.5°F)
Pressure: 1013.1 hPa
Altitude: ~151.1 m
...
```

## Calibration

The BMP280 has built-in calibration coefficients stored in registers 0x88-0xA1. The example reads these automatically for accurate measurements.

## Accuracy

- **Temperature**: ±1.0°C typical
- **Pressure**: ±1 hPa typical (±8 m altitude)
- **Resolution**: 0.01 hPa

## Common Uses

- Weather stations
- Altitude measurement (drones, rockets)
- Vertical speed indication
- Indoor/outdoor climate monitoring

## Troubleshooting

### Device not found
```bash
# Run scanner to confirm address:
./example_i2c_scanner
# Should show 0x76 or 0x77
```

### Incorrect readings
- Check for 3.3V power (not 5V!)
- Verify I2C pull-up resistors (usually on-board)
- Shield from direct airflow during reading

### Chip ID mismatch
- BMP280 returns 0x58
- BME280 returns 0x60 (has humidity sensor too)
- BMP180 (older) has different protocol

## See Also

- **i2c_mpu6050** - Read accelerometer/gyroscope
- **i2c_scanner** - Find I2C device addresses
- **i2c_ssd1306** - Display data on OLED

## References

- [BMP280 Datasheet](https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/bmp280/)
- [PiPinPP Wire API](../../docs/API_REFERENCE.md#wire-i2c)

## License

Part of the PiPinPP project.
