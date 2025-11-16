---
name: Platform Compatibility Report
about: Report testing results from a new ARM SBC platform
title: '[Platform] Platform Name - Compatibility Report'
labels: platform, testing
assignees: ''
---

## Platform Information

**Platform Name:** (e.g., Orange Pi 5, Jetson Nano)
**Model:** (e.g., Orange Pi 5 Plus, Jetson Nano B01)
**SoC:** (e.g., Rockchip RK3588S, NVIDIA Tegra X1)
**Architecture:** (e.g., ARM64, ARM32)

## System Details

**OS:** (e.g., Armbian 23.08, Ubuntu 20.04)
**Kernel Version:** (output of `uname -r`)
```
# Paste output here
```

**libgpiod Version:** (output of `gpiodetect --version`)
```
# Paste output here
```

**GCC Version:** (output of `gcc --version`)
```
# Paste output here
```

## Hardware Detection

**GPIO Chips:** (output of `gpiodetect`)
```
# Paste output here
```

**I2C Buses:** (output of `ls -l /dev/i2c-*`)
```
# Paste output here
```

**SPI Devices:** (output of `ls -l /dev/spidev*`)
```
# Paste output here
```

**Hardware PWM:** (output of `ls -l /sys/class/pwm/`)
```
# Paste output here
```

## PiPinPP Installation

**Install Method:**
- [ ] Automated install script (`curl -sSL ... | sudo bash`)
- [ ] Manual build from source
- [ ] From package manager

**Installation Successful:**
- [ ] Yes
- [ ] No (describe issues)

**Installation Notes:**
```
# Any issues or special steps required
```

## Platform Detection Test

**Output of platform_info example:**
```bash
cd /usr/share/doc/pipinpp/examples/platform_detection
./platform_info
```

```
# Paste full output here
```

## Feature Testing

Test each feature and check the box if it works:

### Basic GPIO
- [ ] pinMode(pin, OUTPUT)
- [ ] digitalWrite(pin, HIGH/LOW)
- [ ] pinMode(pin, INPUT)
- [ ] digitalRead(pin)
- [ ] INPUT_PULLUP mode
- [ ] INPUT_PULLDOWN mode

**Notes:**
```
# Any issues or observations
```

### Software PWM
- [ ] analogWrite(pin, 128) works
- [ ] PWM frequency can be changed
- [ ] Multiple pins with PWM simultaneously

**Notes:**
```
# PWM performance, jitter, CPU usage observations
```

### Hardware PWM (if available)
- [ ] HardwarePWM class constructor
- [ ] setFrequency() works
- [ ] setDutyCycle() works
- [ ] Measured output with oscilloscope/logic analyzer

**Notes:**
```
# Available PWM channels, sysfs paths
```

### I2C (Wire)
- [ ] Wire.begin() succeeds
- [ ] Wire.scan() detects devices
- [ ] Wire.beginTransmission() + Wire.write() + Wire.endTransmission()
- [ ] Wire.requestFrom() + Wire.read()

**Tested with device:** (e.g., BMP280, MPU6050, or "no device available")

**Notes:**
```
# I2C bus number, clock speed, any issues
```

### SPI
- [ ] SPI.begin() succeeds
- [ ] SPI.transfer(byte) works
- [ ] SPI.transfer(buffer, size) works
- [ ] Clock speed can be configured

**Tested with device:** (e.g., 74HC595, MFRC522, or "loopback test")

**Notes:**
```
# SPI bus/CS pin, clock speed, any issues
```

### Serial (UART)
- [ ] Serial.begin(baudrate) succeeds
- [ ] Serial.write() works
- [ ] Serial.read() works
- [ ] Tested with loopback (TX→RX)

**Device path:** (e.g., /dev/ttyAMA0, /dev/ttyUSB0)

**Notes:**
```
# UART device, baudrate, any issues
```

### Interrupts
- [ ] attachInterrupt(pin, callback, mode) works
- [ ] Interrupt fires on GPIO edge
- [ ] detachInterrupt(pin) works
- [ ] Multiple interrupts on different pins

**Test method:** (e.g., button press, GPIO loopback)

**Notes:**
```
# Edge modes tested, latency observations
```

### Timing Functions
- [ ] millis() increments correctly
- [ ] micros() increments correctly
- [ ] delay(ms) accurate within ±10ms
- [ ] delayMicroseconds(us) accurate

**Notes:**
```
# Timing accuracy measurements
```

## Unit Tests

**Test Results:** (output of `cd build && ctest --output-on-failure`)
```
# Total tests run:
# Tests passed:
# Tests failed:
# Tests skipped:
```

**Failed Tests (if any):**
```
# List failed test names and errors
```

## Known Issues

Describe any platform-specific issues, workarounds, or quirks:

```
# Example:
# - GPIO chip is /dev/gpiochip1, not gpiochip0 (needs manual override)
# - I2C bus numbering differs from Raspberry Pi
# - Hardware PWM not available
```

## Performance Notes

- **GPIO toggle speed:** (if measured)
- **Software PWM CPU usage:** (if measured)
- **I2C transaction speed:** (if measured)

## Additional Information

Any other observations, photos of hardware, oscilloscope captures, etc.

## Recommendation

Based on testing, I recommend this platform be marked as:
- [ ] ✅ Fully Supported (all features work)
- [ ] ⚠️ Partially Supported (most features work with noted exceptions)
- [ ] ❌ Incompatible (major issues, not recommended)

---

**Thank you for helping expand PiPinPP platform support!** 🎉

Your testing helps makers worldwide use PiPinPP on more hardware.
