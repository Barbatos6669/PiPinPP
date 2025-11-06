# MPU6050 IMU Sensor Example

Read 6-axis motion data (accelerometer + gyroscope) from MPU6050 via I2C.

## Features

- Read 3-axis accelerometer (±2g to ±16g)
- Read 3-axis gyroscope (±250°/s to ±2000°/s)
- Read temperature sensor
- Calculate tilt angles (roll, pitch)
- Arduino Wire library compatible

## Hardware Setup

**MPU6050 Connections:**
- VCC → 3.3V (or 5V if module has regulator)
- GND → GND
- SDA → GPIO 2 (Pin 3)
- SCL → GPIO 3 (Pin 5)
- (AD0 → GND for address 0x68)

**I2C Address**: 0x68 (default) or 0x69 (if AD0 pulled high)

## Enable I2C

```bash
sudo raspi-config
# Interface Options → I2C → Enable

# Verify:
sudo i2cdetect -y 1
# Should show 68 or 69
```

## Building & Running

```bash
cd /path/to/PiPinPP/build
make example_i2c_mpu6050
./examples/example_i2c_mpu6050
```

## Expected Output

```
MPU6050 6-Axis IMU Example
==========================

Initializing MPU6050...
WHO_AM_I: 0x68 (MPU6050 detected)

Accel (g):  X: 0.02  Y: -0.01  Z: 1.00
Gyro (°/s): X: 0.5   Y: -0.3   Z: 0.1
Temp: 28.3°C

Tilt: Roll: 1.2°  Pitch: -0.8°

Accel (g):  X: 0.03  Y: -0.02  Z: 0.99
Gyro (°/s): X: 0.6   Y: -0.2   Z: 0.2
...
```

## Understanding the Data

### Accelerometer
- Measures acceleration in g-forces (1g = 9.8 m/s²)
- At rest: X≈0, Y≈0, Z≈1.0 (gravity)
- Tilting changes X/Y/Z distribution

### Gyroscope
- Measures rotation rate in degrees/second
- At rest: X≈0, Y≈0, Z≈0
- Moving/rotating: Shows angular velocity

### Temperature
- Built-in temperature sensor
- Useful for temperature compensation

## Calculating Tilt Angles

```cpp
float roll = atan2(accelY, accelZ) * RAD_TO_DEG;
float pitch = atan2(-accelX, sqrt(accelY*accelY + accelZ*accelZ)) * RAD_TO_DEG;
```

**Note**: Accelerometer-only tilt is noisy. For stable orientation, combine with gyroscope using complementary filter or Kalman filter.

## Common Uses

- Robotics (balance, orientation)
- Drones (flight controller)
- Gesture recognition
- Motion detection
- Gaming controllers
- Activity tracking

## Calibration

MPU6050 has some drift. For best accuracy:
1. Read sensor while stationary
2. Calculate average offset
3. Subtract offset from future readings

```cpp
// Read 100 samples, average for offset
for (int i = 0; i < 100; i++) {
    accelX_offset += readAccelX();
    delay(10);
}
accelX_offset /= 100;
```

## Troubleshooting

### Device not found
```bash
./example_i2c_scanner  # Should show 0x68
```

### Readings are all zeros
- Check power (3.3V or 5V depending on module)
- Verify I2C wiring
- Ensure device is out of sleep mode (code handles this)

### Noisy readings
- Normal for cheap MPU6050 modules
- Use software filtering (moving average)
- Consider upgrade to MPU9250 or BMI160

### Drift over time
- Gyroscope naturally drifts
- Use sensor fusion (accelerometer + gyroscope)
- Implement Kalman filter for best results

## See Also

- **i2c_bmp280** - Add pressure/altitude sensing
- **i2c_scanner** - Find I2C devices
- **button_interrupt** - React to motion events

## References

- [MPU6050 Datasheet](https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/)
- [Understanding IMUs](https://learn.sparkfun.com/tutorials/accelerometer-basics)
- [PiPinPP Wire API](../../docs/API_REFERENCE.md#wire-i2c)

## License

Part of the PiPinPP project.
