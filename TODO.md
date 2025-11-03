# PiPinPP Todo List

This checklist tracks the planned work for PiPinPP development.

**Current Version**: v0.3.0 (Released November 3, 2025)  
**Next Target**: v0.4.0 (Communication Protocols & Hardware PWM)

Legend: [easy] quick win · [medium] moderate · [hard] larger feature

---

## 🎉 v0.3.0 - COMPLETE! (Released November 3, 2025)

## Easy

- ✅ Thread-safety for ArduinoCompat [easy]
  - Protect globalPins map with a mutex
  - Document thread-safety notes
  - Add minimal concurrent access test

- ✅ Tidy HIGH/LOW constants [easy]
    - Replaced #define macros with constexpr bool for type safety
    - Kept simple global scope for Arduino compatibility
    - No namespace needed - HIGH/LOW don't conflict with std library
    - Updated TODO.md to mark task as complete"

- ✅ Logging + build options [easy]
  - Minimal logging macros behind `PIPINPP_ENABLE_LOGGING` and `PIPINPP_LOG_LEVEL`
  - Remove `std::cout` from library code
  - Add `-Wall -Wextra -Wpedantic` and optional `PIPINPP_WARNINGS_AS_ERRORS`

- ✅ Repo discoverability [easy]
  - Set GitHub topics: raspberry-pi, gpio, cpp, arduino, libgpiod, embedded
  - Add README badges: build, release, license
  - GitHub Pages configured (awaiting v0.3.0 merge to main)

- ✅ Docs and wiki updates [easy]
  - Pin numbering guide (BCM vs physical) - See docs/API_REFERENCE.md
  - Permissions guide (gpio group / udev) - See INSTALL.md
  - TOC anchors added and docs synced to wiki structure

## Medium

- ✅ Timing functions [medium]
  - Implemented `millis()`, `micros()`, `delay()`, and `delayMicroseconds()`
  - Using `std::chrono::steady_clock` for monotonic timing
  - Comprehensive unit tests for accuracy, monotonicity and bounds
  - All functions tested and verified accurate to ±1µs
  - Completed in v0.3.1

- ✅ Custom exceptions [medium]
  - `PinError`, `InvalidPinError`, `GpioAccessError`
  - Replace generic throws and improve error messages
  - Document error model
  - Completed in v0.3.2

- ✅ GitHub Actions CI [medium]
  - Build Debug/Release, run tests
  - pkg-config smoke test
  - Optional CodeQL; add badges
  - Completed in v0.3.3

- ✅ Unit tests (GoogleTest) [medium]
  - Integrated GoogleTest v1.14.0 framework
  - 40 comprehensive tests (timing, exceptions, Pin operations)
  - Automatic test discovery and professional reporting
  - Hardware tests skip gracefully in CI environments
  - Completed in v0.3.4

- ✅ New examples [medium]
  - `timing_benchmark` - Timing functions and GPIO speed (208 lines)
  - `exception_handling` - Error handling patterns (294 lines)
  - `thread_safety` - Multi-threaded GPIO operations (311 lines)
  - Professional formatting with comprehensive comments
  - Completed in v0.3.5

- ✅ CMake packaging polish [medium]
  - BUILD_SHARED_LIBS option for shared/static library selection
  - PiPinPP::pipinpp namespace alias for modern CMake
  - PiPinPPConfig.cmake and PiPinPPTargets.cmake generation
  - Version compatibility checking with PiPinPPConfigVersion.cmake
  - Verified both CMake find_package() and pkg-config work
  - Completed in v0.3.6

## Hard

- ✅ Implement interrupts API [hard]
  - `attachInterrupt(pin, mode, callback)` and `detachInterrupt(pin)`
  - Support RISING/FALLING/CHANGE via libgpiod events
  - Internal polling/epoll thread with safe shutdown
  - Example + documentation
  - Completed in v0.3.7

- ✅ Add PWM (analogWrite) [hard]
  - Software PWM backend (configurable frequency/duty); jitter documented
  - LED fade example with multiple demonstration patterns
  - Roadmap to hardware PWM backend (future enhancement)
  - Completed in v0.3.8

- [ ] Namespace core API [hard] - **DEFERRED TO v0.4.0**
  - Wrap Pin, enums, helpers in `pipinpp::`
  - Provide transitional `using` declarations to avoid breaking users
  - Update headers and docs
  - **Rationale**: Breaking change best done in separate release; current global scope matches Arduino API; low priority with no conflicts reported

---

## 🚀 v0.4.0 - Communication Protocols & Hardware PWM

**Target**: Q1 2026  
**Focus**: Add communication protocol support (I2C, SPI, UART) and hardware PWM for servo control

### Easy Tasks

- [ ] Pin state query functions [easy]
  - `bool isOutput(int pin)` - Check if pin is configured as output
  - `bool isInput(int pin)` - Check if pin is configured as input
  - `PinMode getMode(int pin)` - Get current pin mode
  - Add to ArduinoCompat layer with proper error handling
  - Unit tests for all query functions

- [ ] Digital toggle function [easy]
  - `void digitalToggle(int pin)` - Toggle pin state (HIGH↔LOW)
  - Efficient implementation without read-modify-write
  - Thread-safe operation
  - Example demonstrating toggle for LED blinking

- [ ] Analog input example [easy]
  - Example showing MCP3008 ADC via SPI (once SPI implemented)
  - Arduino-style analogRead() wrapper
  - Voltage calculation and scaling
  - Multi-channel reading demonstration

- [ ] Servo control example [easy]
  - Example using hardware PWM (once implemented)
  - Servo library-compatible API
  - Angle to pulse width conversion
  - Multiple servo control demonstration

### Medium Tasks

- [ ] I2C support [medium]
  - I2C master implementation using Linux i2c-dev
  - Arduino Wire library compatible API
  - `Wire.begin()`, `Wire.beginTransmission()`, `Wire.write()`, `Wire.read()`
  - `Wire.endTransmission()`, `Wire.requestFrom()`
  - I2C bus scanning utility
  - Support for 7-bit and 10-bit addressing
  - Example: Read temperature from common I2C sensors (BME280, DHT12)
  - Unit tests with I2C device simulation

- [ ] SPI support [medium]
  - SPI master implementation using Linux spidev
  - Arduino SPI library compatible API
  - `SPI.begin()`, `SPI.transfer()`, `SPI.setBitOrder()`, `SPI.setDataMode()`
  - `SPI.setClockDivider()`, `SPI.end()`
  - Multiple chip select support
  - Configurable SPI modes (0-3)
  - Example: Control MCP3008 ADC or NRF24L01 radio
  - Unit tests for SPI transactions

- [ ] UART/Serial support [medium]
  - Serial interface using Linux termios
  - Arduino Serial compatible API
  - `Serial.begin(baudrate)`, `Serial.print()`, `Serial.println()`
  - `Serial.read()`, `Serial.available()`, `Serial.write()`
  - Multiple UART support (Serial1, Serial2)
  - Configurable baud rates, parity, stop bits
  - Example: GPS module communication, Arduino serial bridge
  - Unit tests with virtual serial ports

- [ ] Hardware PWM support [medium]
  - Native hardware PWM via PWM chip (pwm-bcm2835)
  - Access via Linux PWM sysfs interface
  - `analogWrite()` auto-detect: hardware PWM > software PWM
  - Servo-compatible frequency and resolution
  - PWM pins: GPIO 12, 13, 18, 19 (hardware channels)
  - Example: Precise servo control with hardware PWM
  - Fallback to software PWM for unsupported pins

- [ ] Performance benchmarking [medium]
  - GPIO toggle speed measurements
  - Interrupt latency measurements
  - PWM frequency accuracy tests
  - I2C/SPI/UART throughput tests
  - Memory usage profiling
  - Performance comparison documentation
  - Optimization recommendations

### Hard Tasks

- [ ] Namespace core API [hard]
  - Wrap Pin, enums, timing, interrupts, PWM in `pipinpp::` namespace
  - Keep Arduino compatibility layer in global scope
  - Provide `using pipinpp::Pin;` transitional declarations
  - Update all headers with namespace wrapping
  - Update examples to show both namespaced and global usage
  - Comprehensive migration guide for users
  - **BREAKING CHANGE** - major version consideration

- [ ] Advanced I2C features [hard]
  - I2C slave mode support
  - SMBus protocol support
  - Clock stretching support
  - Multi-master support
  - DMA transfers for large data
  - I2C debugging tools

- [ ] Advanced SPI features [hard]
  - Full-duplex simultaneous read/write
  - SPI slave mode support
  - DMA transfers for high-speed data
  - Dual/Quad SPI support
  - Loopback testing mode

---

## 🔮 v0.5.0+ - Future Features

### Platform Expansion
- [ ] Orange Pi support [hard]
  - Test on Orange Pi hardware
  - Adjust GPIO chip detection
  - Document platform-specific differences
  
- [ ] Generic ARM SBC support [hard]
  - Abstract GPIO chip discovery
  - Device tree parsing
  - Platform detection utilities

### Advanced Features
- [ ] Analog input via external ADC [medium]
  - MCP3008 SPI ADC support
  - ADS1115 I2C ADC support
  - `analogRead(channel)` API
  - Reference voltage configuration

- [ ] One-Wire protocol [medium]
  - DS18B20 temperature sensor support
  - Arduino OneWire library compatibility
  - Device enumeration and addressing

- [ ] CAN bus support [hard]
  - SocketCAN integration
  - CAN message filtering
  - Arduino-style CAN API

### Development Tools
- [ ] VS Code extension [hard]
  - GPIO pin monitoring view
  - Real-time state visualization
  - Interactive pin configuration
  - Code snippets and templates

- [ ] Web-based GPIO monitor [medium]
  - REST API for GPIO control
  - WebSocket real-time updates
  - Browser-based dashboard
  - Mobile-responsive design

### Quality & Performance
- [ ] Memory-mapped GPIO [hard]
  - Direct register access for speed
  - Bypass libgpiod for performance-critical paths
  - Maintain libgpiod as default safe option
  - Benchmark and document speed improvements

- [ ] Batch GPIO operations [medium]
  - Read/write multiple pins atomically
  - Port-based operations
  - Reduce system call overhead

---

## 📊 v0.4.0 Success Criteria

### Must Have (Required for Release)
- ✅ I2C master support with Wire API
- ✅ SPI master support with SPI API
- ✅ UART support with Serial API
- ✅ Hardware PWM support for servo control
- ✅ At least 2 protocol examples (I2C sensor + SPI device)
- ✅ Unit tests for all new features
- ✅ Documentation updates in API_REFERENCE.md

### Should Have (Desired)
- Pin state query functions
- Digital toggle function
- Performance benchmarking results
- Namespace migration (if breaking changes acceptable)

### Nice to Have (Optional)
- Advanced protocol features (slave mode, DMA)
- Additional examples (GPS, radio modules)
- VS Code snippets for common patterns

---

## 🎯 Development Phases for v0.4.0

### Phase 1: Foundation (Week 1-2)
1. Pin state query functions
2. Digital toggle function
3. Project structure for protocol support
4. Research libgpiod, i2c-dev, spidev APIs

### Phase 2: I2C Implementation (Week 3-4)
1. I2C master basic implementation
2. Wire API compatibility layer
3. I2C example with BME280 sensor
4. Unit tests for I2C

### Phase 3: SPI Implementation (Week 5-6)
1. SPI master basic implementation
2. SPI API compatibility layer
3. SPI example with MCP3008 ADC
4. Unit tests for SPI

### Phase 4: UART Implementation (Week 7-8)
1. Serial interface basic implementation
2. Serial API compatibility layer
3. Serial example with GPS module
4. Unit tests for UART

### Phase 5: Hardware PWM (Week 9-10)
1. Hardware PWM via sysfs
2. Auto-detect hardware vs software PWM
3. Servo control example
4. Performance comparison documentation

### Phase 6: Polish & Release (Week 11-12)
1. Comprehensive testing on hardware
2. Documentation updates
3. Performance benchmarking
4. Release candidate testing
5. v0.4.0 release

---

## 📝 Notes for v0.4.0

- Focus on Arduino-compatible APIs for all protocols
- Maintain backward compatibility with v0.3.0
- Consider namespace migration carefully (breaking change)
- Prioritize hardware testing for all protocols
- Document performance characteristics
- Provide migration examples from Arduino

---

*Last Updated: November 3, 2025 - v0.3.0 Released, Planning v0.4.0*
