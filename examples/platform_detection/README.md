# Platform Detection Example

Demonstrates automatic hardware platform detection and capability discovery.

## What It Does

This example shows how PiPinPP automatically detects:
- **Platform**: Raspberry Pi model (3, 4, 5, Zero, CM4), Orange Pi, BeagleBone, Jetson Nano
- **GPIO Chips**: All available `/dev/gpiochip*` devices
- **I2C Buses**: All `/dev/i2c-*` buses (important: Pi 5 uses `/dev/i2c-20`)
- **PWM Channels**: Hardware PWM via `/sys/class/pwm`
- **DMA Support**: Whether DMA GPIO is feasible on this platform
- **Versions**: Kernel version and libgpiod version

## Why This Matters

**Problem**: Different Raspberry Pi models have different hardware:
- **Pi 3**: Peripheral base at `0x3F000000`
- **Pi 4**: Peripheral base at `0xFE000000`
- **Pi 5**: Uses RP1 I/O controller with 64-bit addressing, default I2C on bus 20

**Solution**: PiPinPP detects the platform at runtime and automatically uses the correct settings.

## Building

```bash
cd /home/larry/Development/PiPinPP
./build.sh
```

The example will be at: `build/examples/example_platform_detection`

## Running

```bash
# No sudo required for basic detection
./build/examples/example_platform_detection
```

## Example Output (Raspberry Pi 4)

```
╔════════════════════════════════════════════════════════════════╗
║          PiPin++ Platform Detection Demonstration              ║
╚════════════════════════════════════════════════════════════════╝

╔════════════════════════════════════════════════════════════════╗
║               PiPin++ Platform Information                     ║
╚════════════════════════════════════════════════════════════════╝

Platform:      Raspberry Pi 4
Kernel:        6.1.21-v8+
libgpiod:      2.2.1

GPIO Chips:    1
  • gpiochip0 (pinctrl-bcm2711): 58 lines ✓

I2C Buses:     2
  • /dev/i2c-1 (bus 1) ✓
  • /dev/i2c-20 (bus 20) ✗

PWM Channels:  2
  • chip0/channel0 (GPIO 18) ✓
  • chip0/channel1 (GPIO 19) ✓

DMA Support:   Available ✓
  Peripheral Base: 0xfe000000

Total GPIO:    58 pins

Recommended Defaults:
  GPIO Chip:     gpiochip0
  I2C Bus:       /dev/i2c-1

📝 Raspberry Pi 4 Notes:
   • Best platform for DMA GPIO (well-tested)
   • Hardware PWM on GPIO 18, 19 (PWM0, PWM1)
   • I2C buses: /dev/i2c-1 (default), /dev/i2c-20 (GPIO 6/7 via dtoverlay)

Example Code Snippets:
───────────────────────────────────────────────────────────────

// Check if running on Raspberry Pi
if (platform.isRaspberryPi()) {
    std::cout << "Running on " << platform.getPlatformName() << "\n";
}

// Use default I2C bus (auto-detects Pi 4 vs Pi 5)
int bus = platform.getDefaultI2CBus();
Wire.begin(bus);  // Works on Pi 4 (/dev/i2c-1) and Pi 5 (/dev/i2c-20)

// Choose PWM backend based on capabilities
if (platform.getCapabilities().hasDMASupport && geteuid() == 0) {
    // Use DMA PWM (zero CPU, requires root)
    auto pwm = DMAPWM(18, 1000);
} else {
    // Use EventPWM (<5% CPU, no root required)
    auto pwm = EventPWM(18, 1000);
}

───────────────────────────────────────────────────────────────

✅ Platform detection complete!
```

## Example Output (Raspberry Pi 5)

Key differences on Pi 5:
```
Platform:      Raspberry Pi 5
GPIO Chips:    1
  • gpiochip4 (rp1): 54 lines ✓
I2C Buses:     3
  • /dev/i2c-20 (bus 20) ✓  ← Default for Pi 5
  • /dev/i2c-1 (bus 1) ✗
  • /dev/i2c-10 (bus 10) ✗
DMA Support:   Not Available ✗
  (Pi 5 uses RP1 I/O controller - different architecture)

Recommended Defaults:
  I2C Bus:       /dev/i2c-20  ← Note: Different from Pi 4!

📝 Raspberry Pi 5 Notes:
   • Uses RP1 I/O controller (different architecture)
   • Default I2C bus is /dev/i2c-20 (not /dev/i2c-1)
   • DMA GPIO requires 64-bit addressing (experimental)
```

## How It Works

### Detection Strategy

1. **Primary**: Read `/proc/device-tree/model` (modern kernels)
   - Most reliable method
   - Directly reports "Raspberry Pi 4 Model B", etc.

2. **Fallback**: Parse `/proc/cpuinfo`
   - Look for SoC identifiers (BCM2711, BCM2712, etc.)
   - Works on older kernels

### Capability Discovery

- **GPIO Chips**: Enumerate `/dev/gpiochip*`, query with libgpiod
- **I2C Buses**: Check `/dev/i2c-*` existence and accessibility
- **PWM Channels**: Scan `/sys/class/pwm/pwmchip*`
- **DMA Support**: Check `/dev/mem` + peripheral base address mapping

### Platform-Specific Logic

```cpp
// Automatically handle Pi 4 vs Pi 5 differences
int bus = platform.getDefaultI2CBus();
// Returns 1 for Pi 4, returns 20 for Pi 5

Wire.begin(bus);  // Just works on both platforms
```

## Use Cases

### 1. Cross-Platform Code

Write code that works on Pi 3, 4, and 5 without changes:

```cpp
auto& platform = PlatformInfo::instance();

// Auto-detects correct I2C bus
Wire.begin(platform.getDefaultI2CBus());

// Auto-detects correct GPIO chip
Pin led(17, platform.getDefaultGPIOChip());
```

### 2. Feature Detection

Choose best PWM backend based on capabilities:

```cpp
if (platform.getCapabilities().hasDMASupport && geteuid() == 0) {
    // Use DMA (zero CPU)
    pwm = std::make_unique<DMAPWM>(18, 1000);
} else {
    // Use EventPWM (<5% CPU)
    pwm = std::make_unique<EventPWM>(18, 1000);
}
```

### 3. Diagnostics

Troubleshooting GPIO/I2C/PWM issues:

```bash
./platform_detection > diagnostics.txt

# Send diagnostics.txt for bug reports
```

## Platform Support Matrix

| Platform | GPIO | I2C | Hardware PWM | DMA GPIO | Status |
|----------|------|-----|--------------|----------|--------|
| **Raspberry Pi 5** | ✅ | ✅ (bus 20) | ⚠️ Limited | ❌ (RP1) | Partial |
| **Raspberry Pi 4** | ✅ | ✅ (bus 1) | ✅ | ✅ | **Full** |
| **Raspberry Pi CM4** | ✅ | ✅ (bus 1) | ✅ | ✅ | **Full** |
| **Raspberry Pi 3** | ✅ | ✅ (bus 1) | ✅ | ⚠️ Untested | Partial |
| **Raspberry Pi Zero 2** | ✅ | ✅ (bus 1) | ✅ | ⚠️ Untested | Partial |
| **Raspberry Pi Zero** | ✅ | ✅ (bus 1) | ✅ | ❌ (slow) | Partial |
| **Orange Pi** | ⚠️ | ⚠️ | ❌ | ❌ | Experimental |
| **BeagleBone** | ⚠️ | ⚠️ | ❌ | ❌ | Experimental |
| **Jetson Nano** | ⚠️ | ⚠️ | ❌ | ❌ | Experimental |

**Legend**:
- ✅ Full support, tested
- ⚠️ Partial support, untested, or limitations
- ❌ Not supported

## Next Steps

1. **DMA Phase 2**: Implement PWM timing using platform-detected peripheral base
2. **Multi-Platform Testing**: Test on Pi 3, 5, Orange Pi, Jetson
3. **CI Integration**: Add platform matrix testing to GitHub Actions
4. **Documentation**: Create platform-specific setup guides

## See Also

- [docs/planning/DMA_GPIO_DESIGN.md](../../docs/planning/DMA_GPIO_DESIGN.md) - DMA uses platform detection for peripheral addresses
- [examples/poc_dma_gpio/](../poc_dma_gpio/) - DMA PoC hardcoded for Pi 4 (will use platform detection in Phase 4)
- [src/Wire.cpp](../../src/Wire.cpp) - Already uses platform detection for I2C bus selection
