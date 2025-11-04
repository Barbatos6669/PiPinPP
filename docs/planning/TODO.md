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

## 🚀 v0.4.0 - Communication Protocols & Complete Arduino API

**Target**: Q1 2026  
**Focus**: Full Arduino API compatibility + Communication protocols (I2C, SPI, UART) + Hardware PWM

### Arduino API Compatibility Status

#### ✅ Already Implemented (v0.3.0)
**Digital I/O**
- ✅ `pinMode(pin, mode)` - Set pin as INPUT, OUTPUT, INPUT_PULLUP
- ✅ `digitalWrite(pin, value)` - Write HIGH/LOW to pin
- ✅ `digitalRead(pin)` - Read pin state
- ✅ `HIGH` / `LOW` - Pin state constants
- ✅ `INPUT` / `OUTPUT` / `INPUT_PULLUP` - Pin mode constants

**Timing**
- ✅ `millis()` - Milliseconds since start
- ✅ `micros()` - Microseconds since start
- ✅ `delay(ms)` - Millisecond delay
- ✅ `delayMicroseconds(us)` - Microsecond delay

**Math**
- ✅ `constrain(x, min, max)` - Constrain value to range
- ✅ `map(x, in_min, in_max, out_min, out_max)` - Map value between ranges

**Interrupts**
- ✅ `attachInterrupt(pin, callback, mode)` - Attach interrupt handler
- ✅ `detachInterrupt(pin)` - Detach interrupt handler
- ✅ `RISING` / `FALLING` / `CHANGE` - Interrupt mode constants

**PWM**
- ✅ `analogWrite(pin, value)` - Software PWM output (0-255)

#### 📋 Planned for v0.4.0

**Extended Math** (Easy)
- `sq(x)`, `sqrt(x)`, `pow(base, exp)`, `max(a,b)`, `min(a,b)`
- `sin(rad)`, `cos(rad)`, `tan(rad)` with `DEG_TO_RAD`/`RAD_TO_DEG` helpers

**Random Numbers** (Easy)
- `random(max)`, `random(min, max)`, `randomSeed(seed)`

**Bits and Bytes** (Easy)
- `bit(n)`, `bitRead()`, `bitWrite()`, `bitSet()`, `bitClear()`
- `highByte()`, `lowByte()`

**Characters** (Easy)
- All standard character classification functions (isAlpha, isDigit, etc.)

**Advanced I/O** (Medium)
- `pulseIn()`, `pulseInLong()`, `shiftIn()`, `shiftOut()`
- `tone()`, `noTone()` for audio generation

**Pin Queries** (Easy)
- `isOutput()`, `isInput()`, `getMode()`, `digitalToggle()`

**Communication** (Medium/Hard)
- `Wire` (I2C) - Complete Wire library API
- `SPI` - Complete SPI library API
- `Serial` - Complete Serial/Stream API with multiple UARTs

**Hardware PWM** (Medium)
- Native hardware PWM for servo control
- Auto-detection and fallback to software PWM

**Analog I/O** (Future v0.5.0+)
- `analogRead()` - Requires external ADC (MCP3008, ADS1115)
- `analogReadResolution()`, `analogReference()`, `analogWriteResolution()`

#### ⚠️ Standard Library Conflict Avoidance

Functions that conflict with std library will be handled as:
1. **Use inline wrappers** - For math functions (abs, max, min, pow, sqrt, sin, cos, tan)
2. **Arduino namespace** - Already used for constrain, map (no conflicts)
3. **Character functions** - Overloaded to take char parameter like Arduino
4. **Documentation** - Clear guidance on using std:: vs Arduino functions

Example approach:
```cpp
// Option 1: Inline wrappers (recommended)
inline double sq(double x) { return x * x; }
inline double sqrt(double x) { return std::sqrt(x); }

// Option 2: Using declarations (user opt-in)
// Users can add: using std::abs; using std::max;

// Option 3: Document std:: usage
// Recommend: Use std::abs(), std::max(), std::min() directly
```

### Easy Tasks

- ✅ Pin state query functions [easy] - **COMPLETE (v0.3.1 - First update after v0.3.0 release)**
  - `bool isOutput(int pin)` - Check if pin is configured as output
  - `bool isInput(int pin)` - Check if pin is configured as input (any INPUT variant)
  - `ArduinoPinMode getMode(int pin)` - Get current pin mode
  - Added to ArduinoCompat layer with proper error handling
  - Thread-safe with mutex locks
  - Throws PinError if pin not initialized
  - Full support for INPUT_PULLDOWN mode

- ✅ Digital toggle function [easy] - **COMPLETE (v0.3.1 - First update after v0.3.0 release)**
  - `void digitalToggle(int pin)` - Toggle pin state (HIGH↔LOW)
  - Efficient implementation tracks lastValue internally (no read needed)
  - Thread-safe operation with mutex
  - Validates write() success and throws GpioAccessError on failure
  - Only works on OUTPUT pins (throws PinError for INPUT)

- ✅ Extended math functions [easy] - **COMPLETE (v0.3.2)**
  - `sq(x)` - Square a number (x²) - template function for Arduino compatibility
  - Note: sqrt(), pow(), max(), min() NOT provided to avoid std:: conflicts
  - Users should use std::sqrt(), std::pow(), std::max(), std::min() directly
  - Header-only inline implementation for optimal performance
  - Full Doxygen documentation with usage examples
  - Updated API_REFERENCE.md explaining why only sq() is provided
  - Math functions example demonstrating sq() with std library functions

- [ ] Trigonometry functions [easy]
  - `sin(rad)`, `cos(rad)`, `tan(rad)` - Use std::sin/cos/tan wrappers
  - Helper macros: `DEG_TO_RAD`, `RAD_TO_DEG`
  - Documentation showing Arduino compatibility

- [ ] Random number functions [easy]
  - `long random(max)` - Random number [0, max)
  - `long random(min, max)` - Random number [min, max)
  - `void randomSeed(seed)` - Seed random generator
  - Use std::random with Mersenne Twister

- [ ] Bits and bytes functions [easy]
  - `bit(n)` - Get bit value at position n (1 << n)
  - `bitRead(x, n)` - Read bit n from value x
  - `bitWrite(x, n, b)` - Write bit b to position n in x
  - `bitSet(x, n)` - Set bit n in x to 1
  - `bitClear(x, n)` - Clear bit n in x to 0
  - `highByte(x)` - Get high byte of word
  - `lowByte(x)` - Get low byte of word
  - All as inline functions or macros

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

- [ ] Character classification functions [easy]
  - Use std library wrappers for Arduino compatibility
  - `isAlpha()`, `isAlphaNumeric()`, `isAscii()`
  - `isControl()`, `isDigit()`, `isGraph()`
  - `isHexadecimalDigit()`, `isLowerCase()`, `isUpperCase()`
  - `isPrintable()`, `isPunct()`, `isSpace()`, `isWhitespace()`
  - All using std::isalpha, std::isdigit, etc. with char parameter

### Medium Tasks

- [ ] Advanced I/O functions [medium]
  - `pulseIn(pin, value, timeout)` - Measure pulse width
  - `pulseInLong(pin, value, timeout)` - Long pulse measurement
  - `shiftIn(dataPin, clockPin, bitOrder)` - Shift in byte (SPI-like)
  - `shiftOut(dataPin, clockPin, bitOrder, value)` - Shift out byte (SPI-like)
  - `tone(pin, frequency, duration)` - Generate audio tone (software PWM)
  - `noTone(pin)` - Stop tone generation
  - Unit tests for all functions

- [ ] Interrupt control functions [medium]
  - `interrupts()` - Enable global interrupts (no-op on Linux)
  - `noInterrupts()` - Disable global interrupts (no-op on Linux)
  - `digitalPinToInterrupt(pin)` - Convert pin to interrupt number (identity on Pi)
  - Documentation explaining difference from Arduino

- [ ] Analog input resolution [medium]
  - `analogReadResolution(bits)` - Set ADC resolution (for future ADC support)
  - `analogWriteResolution(bits)` - Set PWM resolution (current: 8-bit)
  - `analogReference(type)` - Set analog reference (for future ADC support)
  - Store settings for when ADC is implemented
  - Document that these are placeholders for v0.5.0+

- [ ] I2C/Wire support [medium]
  - I2C master implementation using Linux i2c-dev
  - Arduino Wire library compatible API:
    - `Wire.begin()` / `Wire.begin(address)` - Initialize as master/slave
    - `Wire.beginTransmission(address)` - Start transmission
    - `Wire.write(data)` / `Wire.write(buffer, len)` - Write data
    - `Wire.endTransmission()` / `Wire.endTransmission(stop)` - End transmission
    - `Wire.requestFrom(address, quantity)` - Request bytes from slave
    - `Wire.available()` - Check available bytes
    - `Wire.read()` - Read received byte
    - `Wire.setClock(frequency)` - Set I2C clock speed
    - `Wire.onReceive(handler)` - Register receive handler (slave mode)
    - `Wire.onRequest(handler)` - Register request handler (slave mode)
  - I2C bus scanning utility
  - Support for 7-bit and 10-bit addressing
  - Example: BME280 temperature/pressure/humidity sensor
  - Unit tests with I2C device simulation

- [ ] SPI support [medium]
  - SPI master implementation using Linux spidev
  - Arduino SPI library compatible API:
    - `SPI.begin()` - Initialize SPI interface
    - `SPI.end()` - Disable SPI interface
    - `SPI.beginTransaction(settings)` - Configure SPI for transaction
    - `SPI.endTransaction()` - Release SPI bus
    - `SPI.transfer(data)` - Transfer single byte
    - `SPI.transfer(buffer, len)` - Transfer multiple bytes
    - `SPI.transfer16(data)` - Transfer 16-bit word
    - `SPI.setBitOrder(order)` - MSBFIRST or LSBFIRST
    - `SPI.setDataMode(mode)` - SPI_MODE0, SPI_MODE1, SPI_MODE2, SPI_MODE3
    - `SPI.setClockDivider(divider)` - Set clock speed divider
    - `SPI.usingInterrupt(interrupt)` - Not needed on Linux
  - SPISettings class for transaction configuration
  - Multiple chip select support
  - Example: MCP3008 8-channel ADC
  - Example: NRF24L01 wireless module
  - Unit tests for SPI transactions

- [ ] UART/Serial support [medium]
  - Serial interface using Linux termios
  - Arduino Serial compatible API:
    - `Serial.begin(baudrate)` / `Serial.begin(baudrate, config)` - Initialize
    - `Serial.end()` - Disable serial
    - `Serial.available()` - Bytes available to read
    - `Serial.availableForWrite()` - Space available in write buffer
    - `Serial.read()` - Read single byte
    - `Serial.readBytes(buffer, length)` - Read multiple bytes
    - `Serial.readString()` - Read string until timeout
    - `Serial.readStringUntil(terminator)` - Read until character
    - `Serial.parseInt()` / `Serial.parseFloat()` - Parse numbers
    - `Serial.write(data)` / `Serial.write(buffer, len)` - Write data
    - `Serial.print(data)` / `Serial.print(data, format)` - Print data
    - `Serial.println(data)` - Print with newline
    - `Serial.flush()` - Wait for transmission complete
    - `Serial.peek()` - Peek at next byte without removing
    - `Serial.setTimeout(timeout)` - Set read timeout
    - `Serial.find(target)` / `Serial.findUntil(target, terminator)` - Search
  - Multiple UART instances (Serial, Serial1, Serial2)
  - Configurable: baud rates, data bits, parity, stop bits
  - Stream-based interface with printf-style formatting
  - Example: GPS NMEA parsing (Neo-6M module)
  - Example: Bluetooth serial bridge
  - Unit tests with virtual serial ports (pty)

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

**Communication Protocols:**
- ✅ I2C master support with complete Wire API
- ✅ SPI master support with complete SPI API  
- ✅ UART support with complete Serial/Stream API
- ✅ Hardware PWM support for servo control

**Arduino API Completion:**
- ✅ Extended math functions (sq, sqrt, pow, max, min, trig)
- ✅ Random number functions (random, randomSeed)
- ✅ Bits and bytes functions (bit ops, highByte, lowByte)
- ✅ Character classification functions
- ✅ Pin state query functions (isOutput, isInput, getMode)
- ✅ Digital toggle function
- ✅ Advanced I/O functions (pulseIn, shiftIn/Out, tone)

**Quality:**
- ✅ At least 3 protocol examples (I2C sensor + SPI device + Serial GPS)
- ✅ Unit tests for all new functions
- ✅ Complete Arduino API compatibility documentation
- ✅ Standard library conflict resolution documented

### Should Have (Desired)
- Performance benchmarking results
- Arduino function reference comparison table
- Migration guide from Arduino with code examples
- Namespace migration (if breaking changes acceptable)
- Debian package (.deb)

### Nice to Have (Optional)
- Advanced protocol features (slave mode, DMA)
- Additional examples (Bluetooth, radio modules, LCD displays)
- Arduino library compatibility testing guide
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
