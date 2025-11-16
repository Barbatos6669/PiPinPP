# Platform Support Matrix

PiPinPP is designed to work across multiple ARM single-board computers (SBCs) using the libgpiod v2 interface. This document tracks platform compatibility, known issues, and testing status.

## Supported Platforms

### ✅ Fully Supported

| Platform | Models | Kernel | libgpiod | GPIO Chip | I2C Bus | Status |
|----------|--------|--------|----------|-----------|---------|--------|
| **Raspberry Pi 4** | 4B, 400, CM4 | 5.15+ | 2.2.1+ | gpiochip0 | /dev/i2c-1 | ✅ Tested |
| **Raspberry Pi 5** | 5B | 6.1+ | 2.2.1+ | gpiochip4 | /dev/i2c-20 | ✅ Tested |

### 🔄 In Testing

| Platform | Models | Kernel | libgpiod | GPIO Chip | I2C Bus | Status |
|----------|--------|--------|----------|-----------|---------|--------|
| **Raspberry Pi 3** | 3B, 3B+, 3A+ | 5.10+ | 2.2.1+ | gpiochip0 | /dev/i2c-1 | 🔄 CI Testing |
| **Raspberry Pi Zero** | Zero, Zero W, Zero 2W | 5.10+ | 2.2.1+ | gpiochip0 | /dev/i2c-1 | 🔄 Planned |
| **Orange Pi 5** | 5, 5B, 5 Plus | 5.10+ | 2.2.1+ | gpiochipX | /dev/i2c-X | 🔄 Research |
| **Jetson Nano** | B01 | 4.9+ | 2.0+ | gpiochip0 | /dev/i2c-X | 🔄 Research |

### ❓ Untested (Community Reports Welcome!)

| Platform | Models | Expected Compatibility | Notes |
|----------|--------|----------------------|-------|
| **BeagleBone Black** | Rev C | Should work | Kernel 5.10+, libgpiod 2.0+ |
| **Rock Pi 4** | A/B/C | Should work | Rockchip RK3399 |
| **ODROID-N2** | N2, N2+ | Should work | Amlogic S922X |
| **Banana Pi** | M2+, M5 | Should work | Allwinner H3/H6 |

## Compatibility Requirements

### Minimum Requirements
- **Kernel**: 5.10+ (for libgpiod v2 support)
- **libgpiod**: 2.0+ (2.2.1+ recommended)
- **Compiler**: GCC 7+ or Clang 6+ (C++17 support)
- **GPIO**: Character device interface (`/dev/gpiochip*`)

### Automatic Detection
PiPinPP automatically detects:
- Platform type (via `/proc/cpuinfo`, `/sys/firmware/devicetree/base/model`)
- Available GPIO chips (`/dev/gpiochip*`)
- I2C buses (`/dev/i2c-*`)
- Hardware PWM channels (`/sys/class/pwm/`)
- DMA support (via peripheral base addresses)

## Platform-Specific Details

### Raspberry Pi 4 (Tested ✅)
**Hardware:** BCM2711, 1.5GHz quad-core ARM Cortex-A72
**GPIO:** 28 GPIO pins (0-27), 5V tolerant inputs: NO
**Peripherals:**
- GPIO: `/dev/gpiochip0` (54 lines)
- I2C: `/dev/i2c-1` (pins 2/3), `/dev/i2c-0` (ID EEPROM, advanced)
- SPI: `/dev/spidev0.0`, `/dev/spidev0.1`
- Hardware PWM: PWM0 (GPIO12, 18), PWM1 (GPIO13, 19)
- UART: `/dev/ttyAMA0` (pins 14/15)

**Tested Features:**
- ✅ Digital I/O (pinMode, digitalWrite, digitalRead)
- ✅ Software PWM (analogWrite)
- ✅ Hardware PWM (HardwarePWM class)
- ✅ I2C (Wire interface)
- ✅ SPI (SPI interface)
- ✅ Serial (UART)
- ✅ Interrupts (attachInterrupt)
- ✅ Timing functions (millis, micros, delay)

**Known Issues:** None

### Raspberry Pi 5 (Tested ✅)
**Hardware:** BCM2712, 2.4GHz quad-core ARM Cortex-A76
**GPIO:** 28 GPIO pins (0-27), 5V tolerant inputs: NO
**Peripherals:**
- GPIO: `/dev/gpiochip4` (54 lines) **⚠️ Note: gpiochip4, not gpiochip0!**
- I2C: `/dev/i2c-20` (pins 2/3) **⚠️ Note: i2c-20, not i2c-1!**
- SPI: `/dev/spidev0.0`, `/dev/spidev0.1`
- Hardware PWM: `/sys/class/pwm/pwmchip2/` (GPIO12, 13, 18, 19)
- UART: `/dev/ttyAMA10` (pins 14/15) **⚠️ Note: ttyAMA10, not ttyAMA0!**

**Tested Features:**
- ✅ Digital I/O (auto-detects gpiochip4)
- ✅ Software PWM
- ✅ I2C (auto-detects /dev/i2c-20)
- ⚠️ Hardware PWM (different sysfs layout)
- ⚠️ Serial (different device path)
- ✅ Platform detection

**Known Issues:**
- Hardware PWM sysfs path differs from Pi 4 (`pwmchip2` vs `pwmchip0`)
- Serial UART device is `/dev/ttyAMA10` instead of `/dev/ttyAMA0`

**Auto-Detection:** PiPinPP automatically detects Pi 5 and uses correct devices

### Raspberry Pi 3 (In Testing 🔄)
**Hardware:** BCM2837, 1.2GHz quad-core ARM Cortex-A53
**GPIO:** 28 GPIO pins (0-27)
**Peripherals:**
- GPIO: `/dev/gpiochip0` (54 lines)
- I2C: `/dev/i2c-1` (pins 2/3)
- SPI: `/dev/spidev0.0`, `/dev/spidev0.1`
- Hardware PWM: PWM0 (GPIO12, 18), PWM1 (GPIO13, 19)
- UART: `/dev/ttyAMA0` (pins 14/15) or `/dev/ttyS0` (if Bluetooth enabled)

**Testing Status:** CI pipeline testing in progress

**Known Issues:** 
- Bluetooth may disable primary UART (/dev/ttyAMA0), need to use `/dev/ttyS0`

### Orange Pi 5 (Research Phase 🔄)
**Hardware:** Rockchip RK3588S, 2.4GHz octa-core ARM Cortex-A76/A55
**GPIO:** 40-pin header (variable GPIO chips)
**Expected Support:** Should work with libgpiod 2.0+

**Research Needed:**
- [ ] GPIO chip device names and line counts
- [ ] I2C bus numbering
- [ ] SPI device availability
- [ ] Hardware PWM support
- [ ] DMA peripheral base addresses

**Community Help Wanted:** If you have an Orange Pi 5, please test and report!

### NVIDIA Jetson Nano (Research Phase 🔄)
**Hardware:** Tegra X1, 1.43GHz quad-core ARM Cortex-A57
**GPIO:** 40-pin header
**Expected Support:** Partial (libgpiod 1.x on older kernels)

**Research Needed:**
- [ ] Kernel version and libgpiod availability
- [ ] GPIO chip configuration
- [ ] Jetson.GPIO compatibility layer
- [ ] Hardware-specific quirks

**Known Limitations:**
- Older Jetson images use libgpiod 1.x (incompatible)
- May require kernel upgrade to 5.10+ for libgpiod v2

## Testing Your Platform

### Quick Compatibility Check
```bash
# Check kernel version (need 5.10+)
uname -r

# Check libgpiod version (need 2.0+)
gpiodetect --version

# List available GPIO chips
gpiodetect

# List I2C buses
ls -l /dev/i2c-*

# Check SPI devices
ls -l /dev/spidev*
```

### Run PiPinPP Platform Detection
```bash
# Build and install PiPinPP
curl -sSL https://raw.githubusercontent.com/Barbatos6669/PiPinPP/main/install.sh | sudo bash

# Run platform info example
cd /usr/share/doc/pipinpp/examples/platform_detection
./platform_info
```

**Expected Output:**
```
PiPinPP Platform Detection
=========================
Platform: Raspberry Pi 4 Model B Rev 1.2
SoC: BCM2711
Kernel: 6.1.21-v8+
libgpiod: 2.2.1

GPIO Chips:
  - /dev/gpiochip0 (54 lines)

I2C Buses:
  - /dev/i2c-1 (default)

Hardware PWM:
  - 2 channels detected

DMA Support: Yes (BCM2711 peripherals)
```

### Report Compatibility
If you test on a new platform, please report:
1. Platform name and model
2. Kernel version (`uname -r`)
3. libgpiod version (`gpiodetect --version`)
4. GPIO chip info (`gpiodetect`)
5. Output of `platform_info` example
6. Which features work/don't work

**Submit via:** [GitHub Issue](https://github.com/Barbatos6669/PiPinPP/issues/new?template=platform_report.md)

## Kernel & libgpiod Compatibility

### libgpiod Version Matrix

| libgpiod | Kernel | ABI | PiPinPP Support |
|----------|--------|-----|-----------------|
| 2.2.1+ | 5.10+ | v2 | ✅ Recommended |
| 2.0-2.1 | 5.10+ | v2 | ✅ Supported |
| 1.6.x | 4.8+ | v1 | ❌ Incompatible |
| 1.4.x | 4.8+ | v1 | ❌ Incompatible |

**Important:** PiPinPP uses libgpiod v2 API exclusively. Older v1 API is incompatible.

### Raspberry Pi OS Versions

| OS Version | Release Date | Kernel | libgpiod | PiPinPP Support |
|------------|--------------|--------|----------|-----------------|
| Bookworm (12) | 2023-10-10 | 6.1+ | 2.2.1 | ✅ Full |
| Bullseye (11) | 2021-11-08 | 5.10+ | 1.6.3 | ⚠️ Upgrade needed |
| Buster (10) | 2019-06-20 | 4.19 | 1.2 | ❌ Too old |

**Recommendation:** Use Raspberry Pi OS Bookworm (Debian 12) for best compatibility.

### Upgrading libgpiod

If your system has libgpiod 1.x:
```bash
# Remove old version
sudo apt remove libgpiod-dev libgpiod2

# Install from source (automated in install.sh)
curl -sSL https://raw.githubusercontent.com/Barbatos6669/PiPinPP/main/install.sh | sudo bash
```

## Platform-Specific Quirks

### I2C Bus Numbering
- **Pi 4/3/Zero:** `/dev/i2c-1` (pins 2/3)
- **Pi 5:** `/dev/i2c-20` (pins 2/3)
- **Orange Pi:** Varies by model (often `/dev/i2c-0` or `/dev/i2c-3`)

**Solution:** Use `Wire.begin()` without arguments (auto-detects) or `Wire.begin(bus_number)`

### GPIO Chip Devices
- **Pi 4/3/Zero:** `/dev/gpiochip0`
- **Pi 5:** `/dev/gpiochip4`
- **Other platforms:** May have multiple chips (gpiochip0, gpiochip1, etc.)

**Solution:** PiPinPP auto-detects primary GPIO chip

### Hardware PWM Availability
- **Pi 4/3:** `/sys/class/pwm/pwmchip0/` (2 channels)
- **Pi 5:** `/sys/class/pwm/pwmchip2/` (4 channels)
- **Other platforms:** May not support hardware PWM

**Solution:** Use software PWM (analogWrite) as fallback

### UART Device Paths
- **Pi 4:** `/dev/ttyAMA0` (primary UART)
- **Pi 3 with Bluetooth:** `/dev/ttyS0` (mini UART)
- **Pi 5:** `/dev/ttyAMA10` (primary UART)

**Solution:** PiPinPP Serial class accepts custom device paths

## Troubleshooting

### "No such device" errors
```
Error: Failed to open GPIO chip '/dev/gpiochip0'
```
**Solutions:**
1. Check available chips: `ls -l /dev/gpiochip*`
2. Verify permissions: `sudo adduser $USER gpio`
3. Update device tree: Check `/boot/config.txt`

### "Function not implemented" errors
```
Error: Operation not supported
```
**Causes:**
- Kernel too old (need 5.10+)
- libgpiod v1 installed (need v2)

**Solution:** Upgrade OS or install libgpiod v2

### Auto-detection fails
If platform detection doesn't work:
```cpp
// Manual override
Platform::getInstance().setDefaultGPIOChip("/dev/gpiochip4");
Platform::getInstance().setDefaultI2CBus(20);
```

### Permission denied
```
Error: Permission denied accessing GPIO
```
**Solution:**
```bash
# Add user to gpio group
sudo usermod -aG gpio $USER

# Reboot (required for group changes)
sudo reboot
```

## Contributing Platform Support

Want to help expand platform support?

### Testing Checklist
- [ ] Run all 192 unit tests: `cd build && ctest`
- [ ] Test basic GPIO: Digital I/O, PWM
- [ ] Test I2C: Wire.scan(), read/write
- [ ] Test SPI: Transfer bytes
- [ ] Test Serial: UART communication
- [ ] Test interrupts: attachInterrupt callbacks
- [ ] Document any quirks or workarounds

### Submitting Results
1. Fork PiPinPP repository
2. Update this document (PLATFORM_SUPPORT.md)
3. Add platform-specific notes
4. Create pull request with test results
5. Include hardware photos (optional but appreciated!)

### Community Rewards
- Listed as contributor in README
- Platform badge on your GitHub profile
- Helping makers everywhere! 🎉

## Future Plans

### Short-term (v0.5.0)
- [ ] Complete testing on Pi 3, Pi Zero 2W
- [ ] Document Orange Pi 5 compatibility
- [ ] Add Jetson Nano compatibility layer
- [ ] Create platform support badges for README

### Long-term (v0.6.0+)
- [ ] Generic Linux GPIO abstraction layer
- [ ] Auto-generate device tree overlays
- [ ] Platform-specific optimization
- [ ] Cross-compilation support for all platforms

---

**Last Updated:** 2025-11-16  
**Tested Platforms:** 2 (Pi 4, Pi 5)  
**Next Target:** Pi 3, Orange Pi 5, Jetson Nano

**Help expand this list!** [Submit platform reports →](https://github.com/Barbatos6669/PiPinPP/issues/new?template=platform_report.md)
