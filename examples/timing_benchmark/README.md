# Timing Benchmark Example

Measure GPIO performance - toggle speeds, function call overhead, and timing accuracy.

## Features

- GPIO toggle speed measurement
- Function call overhead analysis
- `delay()` and `delayMicroseconds()` accuracy
- `millis()` and `micros()` performance
- Real-world performance data

## Building & Running

```bash
cd /path/to/PiPinPP/build
make example_timing_benchmark
./examples/example_timing_benchmark
```

## Expected Output

```
PiPinPP Timing Benchmark
========================

GPIO Toggle Speed:
  10,000 toggles: 45.23 ms
  Toggle rate: 221,089 Hz
  Theoretical maximum: ~12 MHz (libgpiod limitation)

Function Call Overhead:
  pinMode(): 2.3 µs average
  digitalWrite(): 1.8 µs average
  digitalRead(): 1.5 µs average

Timing Function Accuracy:
  delay(100): actual 100.02 ms (0.02% error)
  delayMicroseconds(1000): actual 1001.5 µs (0.15% error)
  millis() resolution: 1 ms
  micros() resolution: 1 µs
```

## Performance Comparison

| Platform | Toggle Rate | Notes |
|----------|-------------|-------|
| **PiPinPP (libgpiod)** | ~220 kHz | Software overhead |
| **WiringPi** | ~4-8 MHz | Direct register access |
| **pigpio** | ~100-200 kHz | Similar to libgpiod |
| **Arduino Uno** | ~8 MHz | Direct port manipulation |
| **Arduino (digitalWrite)** | ~100 kHz | Overhead similar to PiPinPP |

## Why The Speed Difference?

### PiPinPP / libgpiod
- ✅ Modern, kernel-maintained interface
- ✅ Safe, no direct memory access
- ✅ Works across kernel versions
- ❌ System call overhead (~5-10µs per operation)

### WiringPi (deprecated)
- ✅ Very fast (direct /dev/mem access)
- ❌ Unmaintained since 2019
- ❌ Unsafe, can crash system
- ❌ Breaks on new kernel versions

## When Speed Matters

**PiPinPP is fast enough for:**
- LED control, buttons, displays
- I2C, SPI, UART (hardware peripherals)
- Servo control (50 Hz)
- Most robotics applications
- Sensor reading (< 10 kHz)

**Consider hardware peripherals for:**
- High-speed protocols (>100 kHz custom protocols)
- Precise timing requirements (< 10µs jitter)
- Real-time signal generation
- Audio signals

## Hardware Setup

- **GPIO 18** → LED → 220Ω → GND (for visual verification)
- Or run without hardware (measures timing only)

## Benchmarking Tips

1. **Run as root for best accuracy**:
   ```bash
   sudo ./example_timing_benchmark
   ```

2. **Close other programs**:
   ```bash
   # Minimal interference from other processes
   ```

3. **Disable CPU throttling** (Pi 4/5):
   ```bash
   # In /boot/config.txt:
   force_turbo=1
   ```

4. **Run multiple times** - results vary slightly

## References

- [libgpiod Performance Analysis](https://lwn.net/Articles/532714/)
- [PiPinPP Architecture](../../docs/API_REFERENCE.md)
- [WiringPi Deprecation](http://wiringpi.com/wiringpi-deprecated/)

## License

Part of the PiPinPP project.
