# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### 📁 Project Organization
- **Reorganized root directory** for better structure
  - Moved `build.sh`, `coverage.sh`, `install.sh` → `scripts/` directory
  - Moved `Doxyfile` → `docs/` directory
  - Updated all references in documentation, workflows, and VS Code tasks
  - Cleaner root directory with only essential configuration files

### 🤖 Automation & Workflow Improvements
- **Changelog generation workflow** - Automated CHANGELOG.md updates
  - Uses conventional commits to generate changelog entries
  - Automatically updates CHANGELOG.md on main branch pushes
  - Provides changelog preview in pull requests
  - Integrates with CMakeLists.txt version tracking
  - Skips empty releases automatically

- **Project board automation** - Issue/PR lifecycle management
  - Auto-adds new issues and PRs to project board
  - Moves PRs to "In Review" when review requested
  - Moves closed items to "Done" status
  - Priority labeling based on keywords (critical, security, bug, feature)
  - Supports emergency priority escalation

- **Pre-commit hooks configuration** - Local quality enforcement
  - C++ formatting with clang-format (Allman style)
  - CMake formatting with cmake-format
  - Python formatting with black (for scripts)
  - Shell script linting with shellcheck
  - Markdown linting with markdownlint
  - Conventional commit message validation
  - Trailing whitespace, large files, merge conflicts detection
  - Private key detection for security
  - Install with: `pip install pre-commit && pre-commit install`

- **Issue response automation** - Smart community engagement
  - Welcome messages for first-time contributors with helpful links
  - Auto-request missing information for bug reports (version, OS, steps, code)
  - Hardware troubleshooting tips when hardware label applied
  - Performance debugging guidance with benchmark instructions
  - Contextual help based on issue labels and content

### 📚 Documentation
- **Pre-commit setup guide** (`docs/PRECOMMIT.md`)
  - Installation instructions for pip, apt, brew
  - Hook configuration reference
  - Conventional commit format guide with examples
  - Troubleshooting section for common hook issues
  - CI integration explanation

### 📝 Project Infrastructure
- **New GitHub issue templates**:
  - Documentation Improvement template for doc feedback
  - Performance Issue template with benchmark structure
  - Hardware Compatibility Request for new board support
- **Dependabot configuration** for automated GitHub Actions updates
- **CITATION.cff** for academic citations and research use
- **Enhanced README** with 4 quick start examples:
  - LED blink, button input, PWM fade, I2C sensor
  - Hardware requirements listed for each example
- **New project badges**: PRs Welcome, Good First Issue, Discord, Maintenance status, GitHub stars
- **GitHub automation workflows**:
  - Auto-labeling for issues and PRs based on content and file changes
  - Stale bot to manage inactive issues (60 days) and PRs (30 days)
  - Code format checker with helpful PR comments and fix instructions
  - Automated release workflow with tarball generation and checksums
- **Code style configuration**:
  - `.clang-format` - Allman brace style, 4-space indentation, 100-char line limit
  - `.editorconfig` - Cross-editor consistency for all file types
- **VS Code workspace configuration**:
  - Recommended extensions (C++, CMake, Doxygen, Git, Remote SSH)
  - Editor settings with format-on-save and 100-char ruler
  - 16 pre-configured build/test/debug tasks
  - Launch configurations for debugging examples and tests

### 🔧 Build System
- **Enhanced build.sh** with new options:
  - `--tests-only` - Build only tests (skip examples)
  - `--examples-only` - Build only examples (skip tests)
  - `--no-tests` - Skip building tests
  - `--no-examples` - Skip building examples
  - `--verbose, -v` - Show detailed build output
  - Improved dependency checking with better error messages
  - Build summary at completion
  - Better error handling and exit codes

### 📝 Project Infrastructure
- **NEW: PR Checks workflow** - Automated validation for pull requests:
  - Conventional commit title enforcement
  - CHANGELOG update detection
  - Documentation completeness checks
  - Large file detection
  - Merge conflict validation
  - Markdown link checking
- **Enhanced .gitignore** - Comprehensive coverage for:
  - Additional IDE files (Visual Studio, Sublime, Vim session files)
  - Coverage artifacts (gcov, gcda, gcno)
  - Profiling files (gmon.out, perf.data, callgrind)
  - Documentation build artifacts
  - Python cache files
  - OS-specific files (Thumbs.db, Desktop.ini)
- **Added Security Policy link** in README quick links

## [0.4.0] - 2025-11-16

### 🚀 Major Features

#### EventPWM - Hybrid Timing System (70-85% CPU Reduction)
- **NEW: `EventPWM` class** - Software PWM with dramatically reduced CPU usage
  - Hybrid timing algorithm: `clock_nanosleep()` + 100µs busy-wait
  - **70-85% CPU reduction** vs `analogWrite()` (10-30% → <5% per pin)
  - <10µs timing jitter (acceptable for LED dimming, 2x worse than busy-loop)
  - Thread-safe duty cycle/frequency updates with `std::atomic`
  - Arduino-compatible API: `setDutyCycle8Bit(0-255)`
  - Edge case optimization: <0.1% duty = always off, >99.9% duty = always on
  - RAII design with automatic thread cleanup
  - Frequency range: 50 Hz - 10 kHz
- **EventPWMManager singleton** - Coordinates multiple PWM pins
  - `analogWriteEvent()` global function for Arduino-style usage
  - Prevents pin conflicts
  - Automatic lifecycle management
- **Perfect for:** Multi-LED control, RGB effects, long-running apps, battery-powered projects

### 📚 Documentation

#### New Guides
- **PWM_MIGRATION.md** - Complete migration guide for v0.4.0
  - Decision tree: When to use each PWM implementation
  - Performance comparison matrix (CPU usage, jitter, use cases)
  - Migration patterns from `analogWrite()` to `EventPWM`
  - Code examples: Single LED, fade effects, RGB control
  - Troubleshooting guide
- **API_REFERENCE.md** - Updated to v0.4.0
  - EventPWM class documentation section
  - Complete API reference with examples
  - Performance comparison table
  - "When to Use" decision guide
- **Release notes: v0.4.0.md** - Comprehensive release documentation
  - Feature descriptions with code examples
  - Performance metrics and benchmarks
  - Migration guide
  - Technical details

### 🧪 New Examples

#### benchmark_cpu
- CPU usage comparison: busy-loop PWM vs EventPWM
- Real-time CPU percentage monitoring via `/proc/self/stat`
- Tests 3 pins @ 490 Hz for 10 seconds each
- Reports average, min, max CPU usage per implementation
- Demonstrates 70-85% CPU reduction

#### benchmark_jitter
- PWM timing accuracy measurement
- Measures actual vs expected PWM period
- Calculates timing jitter (min/max/avg/stddev)
- Uses GPIO loopback (pin 17→27) for edge detection
- Validates <10µs jitter target for EventPWM
- Compares busy-loop vs event-driven timing

### 🔧 Improvements
- Build system integration: `src/event_pwm.cpp`, `include/event_pwm.hpp`
- Modern C++17: `std::atomic`, `std::chrono`, CLOCK_MONOTONIC
- Zero regressions: All 175 tests passing
- Thread-safe by design
- RAII resource management

### 📊 Performance Impact
- **CPU Usage:** 70-85% reduction for multi-pin PWM
- **Timing Jitter:** <10µs (vs <5µs busy-loop, acceptable for LEDs)
- **Power Efficiency:** Lower CPU = less power consumption (battery-friendly)
- **Scalability:** Control 10+ LEDs with <50% total CPU

### 🔄 Migration Notes
- **Backward Compatible:** All existing code continues to work
- **Opt-in Performance:** Migrate to EventPWM for better efficiency
- **No Breaking Changes:** `analogWrite()` still uses busy-loop PWM

### 🎯 Roadmap Progress
- **Phase 1 (Performance Foundation):**
  - ✅ Item #1: Hybrid Timing System (COMPLETED)
  - ⏳ Item #3: Performance Benchmarking Suite (IN PROGRESS)
  - 🔜 Item #2: DMA-based GPIO (NEXT)

## [0.3.13] - 2025-11-16

### Added
- **Hardware PWM Support** - Jitter-free PWM via Linux `/sys/class/pwm` sysfs interface
  - `HardwarePWM` class for precise servo control and motor timing
  - Support for GPIO pins 12, 13, 18, 19 (PWM0/PWM1 channels)
  - Frequency range: 1 Hz to 25 MHz
  - Duty cycle control: percentage (0-100), 8-bit (0-255), or nanoseconds
  - `begin(frequencyHz, dutyCycle)` - Initialize with frequency/duty
  - `setFrequency()`, `setDutyCycle()`, `setDutyCycle8Bit()` - Runtime adjustments
  - `setPeriodNs()`, `setDutyCycleNs()` - Nanosecond precision control
  - `setPolarity()` - Normal or inversed output
  - `enable()`, `disable()` - PWM output control
  - `gpioToPWM()` - Helper to map GPIO pin to chip/channel
  - Thread-safe with mutex protection
  - Perfect for servo motors, ESCs, precise timing applications

- **Example: pwm_servo** - Servo motor control demonstration
  - Angle-to-pulse-width conversion (0-180° → 1-2ms)
  - Smooth sweep functions
  - Position control (0°, 45°, 90°, 135°, 180°)
  - Speed control examples
  - Includes servo wiring instructions

- **Example: pwm_led_hardware** - Hardware PWM LED dimming
  - Linear fade effects
  - Breathing effect (sine wave modulation)
  - Pulse effects
  - Frequency comparison tests (100Hz vs 1kHz vs 10kHz)
  - Demonstrates flicker-free operation

- **Unit Tests: gtest_hardware_pwm.cpp** - Hardware PWM test coverage
  - API compliance tests (27 tests)
  - Thread safety verification
  - GPIO-to-PWM mapping validation
  - Hardware tests (skipped without sysfs access)
  - 8-bit conversion tests
  - Nanosecond calculation tests

### Documentation
- Updated `docs/API_REFERENCE.md` with complete Hardware PWM documentation
- Added servo control patterns and examples
- Documented hardware vs software PWM trade-offs
- Included GPIO pin mapping reference

### Testing
- Test suite expanded from 148 to 175 tests (+27 Hardware PWM tests)
- All tests passing with hardware tests properly skipped
- CI integration validated

## [0.3.12] - 2025-11-16

### Added
- **Serial/UART Communication** - Full Arduino-compatible Serial API
  - `Serial.begin(baudRate, device)` - Open serial port (9600-4000000 baud)
  - `Serial.end()`, `Serial.available()`, `Serial.read()`, `Serial.peek()`
  - `Serial.write()` - Send bytes, buffers, or strings
  - `Serial.print()` / `Serial.println()` - Formatted output (strings, integers, floats)
  - `Serial.readString()` / `Serial.readStringUntil()` - Read string data
  - `Serial.setTimeout()`, `Serial.flush()`, `Serial.getBaudRate()`
  - Thread-safe with internal mutex protection
  - Support for `/dev/ttyUSB*`, `/dev/ttyACM*`, `/dev/ttyAMA*` devices
  - Uses Linux termios for hardware UART (8N1 configuration)
  - Perfect for Arduino-Pi hybrid robots and sensor communication

- **Example: serial_arduino** - Demonstrates Arduino-Pi communication
  - Bidirectional command/response protocol
  - LED control, movement commands, sensor queries
  - Includes test Arduino sketch for upload
  - Integration notes for mecanum robot projects

- **Unit Tests: gtest_serial.cpp** - Serial API test coverage
  - Initialization, connection, thread safety tests
  - API compliance verification
  - Integrated into CMake test suite

### Documentation
- Updated `docs/API_REFERENCE.md` with complete Serial API documentation
- Added hardware setup instructions and wiring notes
- Included mecanum robot integration patterns

### Testing
- Validated on Arduino Mega 2560 via `/dev/ttyACM0`
- Bidirectional communication verified at 9600 baud
- Command/response protocol working correctly
- Arduino sketch upload and testing via Arduino CLI

## [0.3.11] - 2025-11-10

### Internal Improvements
- **Pin Constructor Refactoring** - Eliminated ~80 lines of duplicate code
  - Extracted common GPIO initialization logic into private `initializeGpio()` helper
  - Both `Pin(int, PinDirection)` and `Pin(int, PinMode)` constructors now delegate to shared implementation
  - Improved maintainability with single source of truth for GPIO setup
  - No behavioral changes - all 88 unit tests pass

- **Exception Consistency** - Standardized custom exception types throughout codebase
  - Replaced `std::invalid_argument` with `InvalidPinError` across all throw sites
  - Replaced `std::runtime_error` with `GpioAccessError` for hardware errors
  - Updated `interrupts.cpp` (6 locations) and `ArduinoCompat.cpp` (1 location)
  - Provides better error categorization for library users

- **Logging Optimization** - Improved logging performance by 10-100x
  - Changed `DEBUG`/`INFO`/`WARNING` macros to use `\n` instead of `std::endl`
  - Avoids unnecessary flush operations on every log statement
  - `ERROR` level retains `std::endl` for immediate visibility
  - Critical for high-frequency logging scenarios (GPIO toggle, PWM)

- **Library Code Quality** - Removed direct console output from library code
  - Replaced `std::cerr` in `validatePinNumber()` with `PIPINPP_LOG_WARNING` macro
  - Library now consistently uses logging framework instead of direct output
  - Allows users to control logging verbosity via CMake flags

### Documentation
- **CODE_STANDARDS.md** - Updated with modern C++ patterns
  - Changed constant examples from `#define` to `constexpr bool`
  - Clarified header naming convention (PascalCase for classes, lowercase for utilities)

- **API_REFERENCE.md** - Comprehensive accuracy improvements
  - Added missing `INPUT_PULLDOWN` to `ArduinoPinMode` enum documentation
  - Updated `pinMode()` examples to demonstrate pull-down resistor usage
  - Corrected exception type documentation throughout (InvalidPinError, GpioAccessError)
  - Fixed Pin constructor exception documentation
  - Updated exception handling examples to use custom exception types

### Quality Metrics
- Zero behavioral changes - all 88 unit tests pass
- No new compiler warnings introduced
- Code size reduced by ~80 lines through refactoring
- Documentation now 100% consistent with actual code behavior

## [0.3.10] - 2025-11-09

### Fixed
- **Interrupt GPIO Chip Path** - Fixed interrupt initialization on Raspberry Pi 5
  - `interrupts.cpp` was opening GPIO chip without `/dev/` prefix
  - Caused "Failed to open GPIO chip: gpiochip0" error in interrupt tests
  - Now prepends `/dev/` to chipname if not already present
  - Makes interrupt code consistent with Pin class implementation
  - Fixes Issue #11 interrupt validation failures on Pi 5

## [0.3.9] - 2025-11-09

### Fixed
- **pkg-config Configuration** - Critical fix for linking user programs
  - Moved `-lgpiod` from `Libs.private` to `Libs` in `cmake/pipinpp.pc.in`
  - Added `Requires: libgpiod >= 2.0` for proper dependency tracking
  - Users no longer need to manually add `-lgpiod` when compiling
  - Fixes "undefined reference to gpiod_*" link errors
  - Discovered during Issue #11 hardware validation testing

## [0.3.8] - 2025-11-08

### Fixed
- **Installation Script** - Critical fix for libgpiod dependency handling
  - Separated core build dependencies from optional libgpiod packages
  - Script no longer fails when libgpiod packages are unavailable in repositories
  - Added proper version detection for "unknown" and outdated libgpiod versions
  - Enhanced source build with comprehensive error handling at each step
  - Multiple kernel header package attempts for better distro compatibility
  - Post-build verification ensures libgpiod v2.x installed correctly
  - Resolves installation failures on fresh Raspberry Pi OS and Ubuntu systems

### Changed
- **Version Updates** - Bumped version to 0.3.8 across all files
  - `CMakeLists.txt` - Project version updated
  - `install.sh` - VERSION variable updated
  - `README.md` - Badge and install URL updated
  - `docs/API_REFERENCE.md` - Version and date updated

## [0.3.7] - 2025-11-06

### Added
- **SPI Communication** - Full Arduino-compatible SPI master support
  - `SPI.begin()`, `SPI.begin(bus, cs)` - Initialize hardware SPI
  - `SPI.end()` - Close SPI interface
  - `SPI.setDataMode(mode)` - Configure SPI mode (MODE0-MODE3)
  - `SPI.setBitOrder(order)` - Set bit order (MSBFIRST/LSBFIRST)
  - `SPI.setClockDivider(divider)` - Arduino-style clock setting
  - `SPI.setClock(speed)` - Direct clock speed in Hz (up to 125 MHz)
  - `SPI.getClock()` - Query current clock speed
  - `SPI.transfer(data)` - Single byte transfer
  - `SPI.transfer(buffer, length)` - Multi-byte transfer (in-place)
  - `SPI.transfer(txBuffer, rxBuffer, length)` - Separate TX/RX buffers
  - `SPI.isInitialized()` - Check initialization state
  - Global `SPI` instance for Arduino compatibility
  - Thread-safe with mutex protection
  - Uses Linux `/dev/spidev*` interface
- **SPI Example** - 74HC595 shift register demonstration (300+ lines)
  - `examples/spi_74hc595/main.cpp` - Hardware SPI vs software shiftOut() comparison
  - Complete wiring diagrams for 74HC595 connections
  - Performance benchmarking (hardware SPI ~2-4x faster)
  - Common LED patterns (walking bits, binary counter, etc.)
  - Real-world usage examples
  - Comprehensive README with troubleshooting
- **SPI Unit Tests** - 22 new tests for SPI functionality
  - `tests/gtest_spi.cpp` - Initialization, configuration, transfer, thread safety
  - Hardware tests skip gracefully when `/dev/spidev*` unavailable
  - Test coverage for all SPI modes, bit orders, and clock speeds
  - Concurrent access safety verification
- **Example Documentation** - Comprehensive READMEs for all 19 examples
  - `examples/basic_led/README.md` - LED blink guide with wiring
  - `examples/button_input/README.md` - Button reading with pull-ups
  - `examples/button_interrupt/README.md` - Interrupt-driven GPIO
  - `examples/led_fade/README.md` - PWM brightness control
  - `examples/arduino_style/README.md` - Pure Arduino syntax demo
  - `examples/arduino_migration/README.md` - Complete migration guide
  - `examples/exception_handling/README.md` - Error handling patterns
  - `examples/thread_safety/README.md` - Multi-threaded GPIO
  - `examples/timing_benchmark/README.md` - Performance measurements
  - `examples/advanced_io/README.md` - shiftOut, shiftIn, pulseIn, tone
  - `examples/math_functions/README.md` - map, constrain, min/max
  - `examples/trig_functions/README.md` - Degree-based trigonometry
  - `examples/utility_functions/README.md` - Bit manipulation, random
  - `examples/i2c_scanner/README.md` - I2C device discovery
  - `examples/i2c_bmp280/README.md` - Pressure/temperature sensor
  - `examples/i2c_mpu6050/README.md` - 6-axis IMU
  - `examples/i2c_ssd1306/README.md` - OLED display
  - Each README includes hardware requirements, wiring diagrams, building instructions, troubleshooting, and extension ideas

### Changed
- **Constant Definitions** - Resolved MSBFIRST/LSBFIRST conflicts
  - Unified bit order constants in ArduinoCompat.hpp (MSBFIRST=1, LSBFIRST=0)
  - SPI.hpp now references Arduino constants for consistency
  - Fixed compilation errors when including both headers
- **Test Count** - Increased from 66 to 88 tests (22 new SPI tests)
- **Documentation Updates**
  - `docs/API_REFERENCE.md` - Added comprehensive SPI and Wire/I2C API sections
  - Updated version to 0.3.7 and date to November 6, 2025
  - Added Communication Protocols chapter with I2C and SPI documentation
  - Complete function reference with parameters, returns, and examples
- **README.md Updates**
  - Updated version badge to 0.3.7
  - Updated test count badge to 88 passing tests
  - Added SPI to Advanced Features list
  - Updated example documentation count to 19 with full READMEs
  - Changed install script URLs to v0.3.7
  - Updated feature descriptions for production-ready status

### Fixed
- **Wire Deadlock** - Fixed mutex deadlock in Wire.cpp
  - `begin()` was calling `begin(int)` while holding mutex
  - Refactored to duplicate initialization logic instead of nested call
  - All 66 Wire tests now pass (previously froze at test 37)

## [0.3.6] - 2025-11-05

### Added
- **Advanced I/O Functions** - Implemented 5 new functions for sensor and peripheral interfacing
  - `pulseIn(pin, state, timeout)` - Measure pulse widths for ultrasonic sensors (HC-SR04), IR receivers
  - `pulseInLong(pin, state, timeout)` - Extended range pulse measurement (inline wrapper on Pi)
  - `shiftOut(dataPin, clockPin, bitOrder, value)` - Control shift registers (74HC595) for output expansion
  - `shiftIn(dataPin, clockPin, bitOrder)` - Read from shift registers (74HC165) for input expansion
  - `tone(pin, frequency, duration)` - Generate audio tones (31Hz-65kHz) for buzzers and speakers
  - `noTone(pin)` - Stop tone generation
  - `LSBFIRST` and `MSBFIRST` constants for bit order control
- **Advanced I/O Example** - Comprehensive demonstration (600+ lines)
  - `examples/advanced_io/main.cpp` - Complete usage examples
  - Ultrasonic distance sensor simulation with HC-SR04
  - LED control with 74HC595 shift registers
  - Button reading with 74HC165 shift registers
  - Musical note generation and tone examples
  - Real-world application ideas

### Changed
- **TODO.md Updates** - Marked completed tasks from v0.3.1-v0.3.4
  - Extended Math functions (sq, DEG_TO_RAD, RAD_TO_DEG)
  - Random number generation (random, randomSeed)
  - Bits and Bytes manipulation (bit operations, highByte, lowByte)
  - Character functions (documented to use std::)
  - Pin query functions (isOutput, isInput, getMode, digitalToggle)

## [0.3.5] - 2025-11-05

### Changed
- **Terminology Update** - Changed "Arduino-compatible" to "Arduino-inspired" throughout project
  - Updated project description, documentation, code comments, and examples (29 files)
  - Clarifies this is a Raspberry Pi library with an API familiar to Arduino users
  - Avoids confusion about Arduino hardware compatibility or official endorsement
  - No functional changes - documentation and branding clarity only

### Added
- **Getting Started Guide** - Complete beginner tutorial (466 lines)
  - 10-minute quickstart for first LED
  - Step-by-step hardware setup with circuit diagrams
  - Your first program with line-by-line explanations
  - Interactive button example
  - Troubleshooting section for common issues

- **Troubleshooting Guide** - Comprehensive issue resolution (595 lines)
  - Installation issues and fixes
  - Permission errors and solutions
  - Hardware debugging (LEDs, buttons)
  - Compilation and runtime errors
  - Pin numbering confusion help
  - Performance tips and advanced debugging

- **FAQ** - Frequently Asked Questions (577 lines)
  - 30+ common questions answered
  - Getting started help
  - Hardware safety tips
  - Programming guidance
  - Performance expectations
  - Community resources

- **Tutorials Directory** - Step-by-step projects
  - Traffic Light tutorial with challenges (419 lines)
  - Tutorial index with learning paths
  - Project ideas for all skill levels (30+ suggestions)
  - Contributing guidelines for community tutorials

### Documentation
- Updated README.md with beginner-focused navigation
- Added "New to GPIO? Start Here!" section
- Reorganized top navigation for better discoverability
- Enhanced learning progression clarity

## [0.3.4] - 2025-11-05

### Added
- **Random number functions** - Arduino-inspired random number generation
  - `randomSeed(seed)` - Initialize random number generator with seed value
  - `random(max)` - Generate random number in range [0, max)
  - `random(min, max)` - Generate random number in range [min, max)
  - Uses C++ `<random>` library with Mersenne Twister engine (std::mt19937)
  - Thread-safe implementation with mutex protection
  - Perfect for LED patterns, delays, animations, and games

- **Bit manipulation functions** - Arduino-inspired bit operations
  - `bit(n)` - Compute value of bit at position n (1 << n)
  - `bitRead(x, n)` - Read value of bit n from value x
  - `bitWrite(x, n, b)` - Write bit b to position n in value x
  - `bitSet(x, n)` - Set bit n to 1
  - `bitClear(x, n)` - Clear bit n to 0
  - All inline functions (zero runtime overhead)
  - Perfect for status flags, hardware registers, and protocol implementation

- **Byte extraction functions** - Arduino-inspired byte manipulation
  - `highByte(x)` - Extract high-order byte from 16-bit word (bits 8-15)
  - `lowByte(x)` - Extract low-order byte from 16-bit word (bits 0-7)
  - Inline functions for efficient byte-level operations
  - Useful for sensor data transmission and protocol handling

- **Comprehensive utility_functions example** (461 lines)
  - Random number generation demonstrations
  - Bit manipulation examples with visual binary representations
  - Byte extraction examples with practical sensor data scenarios
  - Status flags management example
  - Random LED patterns generation
  - Complete with tables, diagrams, and practical applications

### Documentation
- Character classification functions documented in ArduinoCompat.hpp
  - Explains use of std::isalpha(), std::isdigit(), std::isspace(), etc.
  - Avoids std:: library conflicts by not providing wrappers
  - Lists all available <cctype> functions
- All new functions fully documented with Doxygen comments
- Updated TODO.md marking tasks complete

## [0.3.3] - 2025-11-05

### Added
- **Trigonometry support** - Arduino-inspired angle conversion constants
  - `DEG_TO_RAD` constant for converting degrees to radians (π/180)
  - `RAD_TO_DEG` constant for converting radians to degrees (180/π)
  - Full precision double constants matching Arduino values
  - Use with std::sin(), std::cos(), std::tan() from <cmath>
  - No function wrappers to avoid std:: library conflicts
  - Comprehensive trig_functions example demonstrating:
    - Angle conversions (degrees ↔ radians)
    - Trigonometric calculations (sine, cosine, tangent)
    - Circular motion (position calculations on circles)
    - Right triangle calculations (opposite, adjacent sides)
    - Wave generation (LED breathing effects, PWM patterns)
    - Servo positioning (angle to pulse width conversion)
    - Angle calculations from coordinates (atan2 usage)
  - Updated API_REFERENCE.md with Trigonometry Constants section
  - Practical examples for LED effects, servo control, and motion

### Changed
- Updated API_REFERENCE.md version to 0.3.3 (November 5, 2025)
- Added Trigonometry Constants to Table of Contents

### Documentation
- Comprehensive trig_functions example (400+ lines)
- Detailed documentation explaining std library approach
- Practical applications for makers and Arduino users
- Clear guidance on using std::sin(), std::cos(), std::tan()

## [0.3.2] - 2025-11-04

### Added
- **Extended math function** - Arduino-inspired square function (v0.3.2)
  - `sq(x)` - Square a number (template function for any numeric type)
  - Header-only inline implementation for optimal performance
  - Template provides type flexibility (int, long, float, double)
  - More efficient than `std::pow(x, 2)`
  - Full Arduino API compatibility
  - Comprehensive Doxygen documentation with usage examples
  - Note: For sqrt(), pow(), max(), min() use standard library (std::sqrt, std::pow, std::max, std::min)
  - Updated API_REFERENCE.md with Extended Math Functions section
  - Math functions example demonstrating sq() with std library functions

## [0.3.1] - 2025-11-04

### Added
- **Pin state query functions** - Check and query pin configuration (v0.3.1)
  - `bool isOutput(int pin)` - Check if pin is configured as OUTPUT
  - `bool isInput(int pin)` - Check if pin is any INPUT variant (INPUT, INPUT_PULLUP, INPUT_PULLDOWN)
  - `ArduinoPinMode getMode(int pin)` - Get exact pin mode
  - `void digitalToggle(int pin)` - Efficiently toggle OUTPUT pin state
  - Thread-safe with mutex protection
  - Proper error handling with PinError exceptions
  - Internal state tracking for efficient toggle without hardware reads
  - Full support for INPUT_PULLDOWN mode
  - Updated API documentation with examples

## [0.3.0] - 2025-11-03 🎉

**Complete Arduino-inspired GPIO library with advanced features!**

This major release brings PiPinPP to production-ready status with 8 major feature additions including interrupt support, PWM, comprehensive testing framework, and professional tooling.

### Added
- **Software PWM support** - Arduino-inspired analog output (v0.3.8)
  - analogWrite(pin, value) function with 0-255 duty cycle range
  - Configurable PWM frequency (default 490Hz, matches Arduino UNO)
  - Per-pin dedicated threads for accurate pulse generation
  - High-resolution timing with busy-wait for precision
  - Multiple simultaneous PWM outputs supported
  - Edge case optimization (0 = always LOW, 255 = always HIGH)
  - Thread-safe duty cycle updates
  - Automatic pin configuration and resource management
  - LED fade example with 4 demonstration patterns (231 lines)
  - Comprehensive PWM API documentation
  - Limitations documented: timing jitter, CPU usage, not for servos

- **GPIO interrupt support** - Edge detection with callbacks (v0.3.7)
  - attachInterrupt(pin, callback, mode) and detachInterrupt(pin) functions
  - Interrupt modes: RISING, FALLING, CHANGE (Arduino-inspired)
  - Thread-safe interrupt handling with background monitoring thread
  - Efficient poll()-based event monitoring for multiple pins
  - Uses libgpiod v2 edge event API (gpiod_edge_event_buffer)
  - Exception-safe callback invocation with error logging
  - Automatic resource cleanup and safe shutdown
  - Wakeup pipe for dynamic interrupt registration/unregistration
  - Button interrupt example with software debouncing (135 lines)
  - Complete interrupt API documentation

- **Modern CMake packaging** - Professional library consumption support (v0.3.6)
  - BUILD_SHARED_LIBS option for building shared (.so) or static (.a) libraries
  - PiPinPP::pipinpp namespace alias for modern CMake target naming
  - PiPinPPConfig.cmake generation for find_package() support
  - CMake target export with PiPinPPTargets.cmake
  - Version compatibility checking with PiPinPPConfigVersion.cmake
  - PUBLIC link propagation for automatic dependency management
  - Maintains pkg-config compatibility alongside CMake config
  - Downstream projects can now use: `find_package(PiPinPP REQUIRED)` + `target_link_libraries(app PiPinPP::pipinpp)`
  - Verified with test consumer project

- **New comprehensive examples** - Demonstrating v0.3.x features (v0.3.5)
  - `timing_benchmark/` - Timing functions demonstration (208 lines)
    - millis(), micros(), delay(), delayMicroseconds() usage
    - GPIO write speed benchmarking (241kHz toggle rate)
    - Timing accuracy measurements (±0ms for delays)
    - Precision LED blink patterns with timing verification
    - Timing resolution test showing ~1µs precision
  - `exception_handling/` - Exception system demonstration (294 lines)
    - InvalidPinError and GpioAccessError examples
    - Exception hierarchy (catching as base PinError)
    - Arduino function exception handling
    - Graceful error recovery patterns
    - Best practices guide with detailed examples
  - `thread_safety/` - Multi-threaded GPIO operations (311 lines)
    - Concurrent LED blinking with 4 threads
    - Stress test with 6000 rapid GPIO operations
    - Thread-safe Arduino functions demonstration
    - Best practices for concurrent GPIO access
  - All examples include professional formatting, comprehensive comments, and work without hardware

- **GoogleTest integration** - Professional testing framework (v0.3.4)
  - Integrated GoogleTest v1.14.0 using CMake FetchContent
  - 40 comprehensive tests covering timing, exceptions, and Pin operations
  - Test fixtures for clean hardware setup/teardown
  - Detailed assertion messages with EXPECT_* and ASSERT_* macros
  - Automatic test discovery with gtest_discover_tests()
  - Hardware tests skip gracefully in CI without GPIO access
  - Foundation for future mock testing capabilities
  - Test files: `gtest_timing.cpp` (8 tests), `gtest_exceptions.cpp` (11 tests), `gtest_pin.cpp` (18 tests)

- **GitHub Actions CI/CD** - Automated build and test pipeline (v0.3.3)
  - Multi-platform build matrix (Ubuntu 22.04, 24.04)
  - Test both Debug and Release builds
  - Strict warning checks (`PIPINPP_WARNINGS_AS_ERRORS=ON`)
  - Automated test execution via CTest
  - pkg-config integration testing
  - External project compilation smoke test
  - CodeQL security analysis for C++ code
  - Build status and CodeQL badges in README
  - Comprehensive workflow comments explaining each step

- **Custom exception classes** - Type-safe error handling (v0.3.2)
  - `PinError` - Base exception class for all GPIO errors
  - `InvalidPinError` - Thrown for invalid pin numbers or configurations
  - `GpioAccessError` - Thrown for hardware access failures (permissions, device not found, etc.)
  - All exceptions inherit from `std::runtime_error` through `PinError`
  - Contextual error messages with pin numbers and device names
  - Updated all library functions to throw appropriate custom exceptions
  - Comprehensive test suite (`test_exceptions.cpp`) with 6 test scenarios
  - Complete exception handling guide in API reference with best practices
  - Fixed thread safety test to respect valid pin range (0-27)

- **Arduino-style timing functions** - Complete timing API (v0.3.1)
  - `millis()` - Returns milliseconds since program start using monotonic clock
  - `micros()` - Returns microseconds since program start for precision timing
  - `delay(ms)` - Sleep for specified milliseconds (CPU-efficient)
  - `delayMicroseconds(us)` - High-precision microsecond delays (busy-wait)
  - All functions use `std::chrono::steady_clock` for monotonic, reliable timing
  - Comprehensive test suite (`test_timing.cpp`) with accuracy validation
  - Verified accurate to ±1µs for microsecond operations
  - Complete documentation in API reference with examples

- **Thread-safety for ArduinoCompat** - Concurrent access protection
  - Mutex protection for globalPins map
  - Thread-safe `pinMode()`, `digitalWrite()`, `digitalRead()`
  - Test suite for concurrent access (`test_thread_safety.cpp`)
  - Documentation of thread-safety guarantees in headers

- **Optional logging system** - Debug logging with zero overhead when disabled
  - New `log.hpp` header with `PIPINPP_LOG_*` macros
  - Four log levels: DEBUG, INFO, WARNING, ERROR
  - CMake option `PIPINPP_ENABLE_LOGGING` (default: OFF)
  - Configurable log level via `PIPINPP_LOG_LEVEL`
  - Replaced all debug `std::cout` with logging macros

- **Comprehensive documentation guides**
  - Pin Numbering Guide (`docs/PIN_NUMBERING.md`) - BCM vs physical pins with diagrams
  - Permissions Setup Guide (`docs/PERMISSIONS.md`) - Running without sudo
  - Updated BUILD.md with new build options
  - Complete GitHub Wiki sync with navigation

- **Enhanced build system**
  - Compiler warnings enabled by default: `-Wall -Wextra -Wpedantic`
  - Optional strict mode: `PIPINPP_WARNINGS_AS_ERRORS`
  - Better CMake options documentation
  - Fixed all compiler warnings

- **Repository discoverability improvements**
  - Professional badges in README (License, Version, Platform, C++17, libgpiod)
  - GitHub topics for better searchability
  - Updated repository description

### Changed
- **HIGH/LOW constants** - Modernized from C macros to C++
  - Changed from `#define HIGH 1` to `constexpr bool HIGH = true`
  - Changed from `#define LOW 0` to `constexpr bool LOW = false`
  - More type-safe and debugger-friendly
  - Maintains full backward compatibility

- **Logging behavior** - Library is now silent by default
  - All debug output removed from Release builds
  - Logging only enabled with CMake option
  - Zero performance overhead when disabled

### Deprecated
- Nothing yet

### Removed
- Debug `std::cout` statements from library code (replaced with optional logging)

### Fixed
- Compiler warnings from unused variables
- Thread-safety issues in Arduino-inspired API layer

### Security
- Added mutex protection against race conditions in multi-threaded programs

---

## [0.2.0] - 2025-10-31

### Added
- **Enhanced Pin class with PinMode enum** - Complete pin mode support
  - `PinMode::INPUT` for standard input reading
  - `PinMode::OUTPUT` for standard output control
  - `PinMode::INPUT_PULLUP` for input with internal pull-up resistor
  - `PinMode::INPUT_PULLDOWN` for input with internal pull-down resistor
  - Dual constructors supporting both legacy PinDirection and new PinMode
  - Pin number validation with comprehensive error checking

- **Complete Arduino-inspired API layer** - Full ArduinoCompat.hpp implementation
  - `pinMode(pin, mode)` function supporting INPUT, OUTPUT, INPUT_PULLUP
  - `digitalWrite(pin, value)` for setting pin states (HIGH/LOW)
  - `digitalRead(pin)` for reading pin values
  - `delay(milliseconds)` for timing control
  - Arduino constants: HIGH, LOW, INPUT, OUTPUT, INPUT_PULLUP
  - Global pin management with automatic cleanup

- **Arduino-style math functions** - Complete utility function set
  - `Arduino::constrain(value, min, max)` for value clamping
  - `Arduino::map(value, fromMin, fromMax, toMin, toMax)` for value mapping
  - Namespaced to avoid conflicts with standard library functions
  - Full floating-point and integer support

- **Comprehensive example programs** - Four complete working examples
  - `arduino_style/` - LED blinking using Arduino-inspired API
  - `basic_led/` - Object-oriented LED control with Pin class
  - `button_input/` - Button reading with debouncing and pull-up resistors
  - `arduino_migration/` - Side-by-side Arduino vs PiPinPP code comparison

- **Complete API documentation** - Professional Doxygen documentation
  - Full Pin class method documentation with examples
  - Arduino-inspired API function documentation
  - PinMode enum documentation with hardware details
  - Code examples and usage patterns for all features
  - Author attribution and version information

- **Enhanced project documentation**
  - Updated README.md with comprehensive feature overview
  - SECURITY.md with responsible disclosure policy
  - Detailed example documentation and wiring diagrams
  - Hardware compatibility and requirements documentation

### Changed
- **Pin class constructor overloading** - Backward compatibility maintained
  - Added PinMode constructor alongside existing PinDirection constructor
  - Enhanced error handling with pin number validation
  - Improved resource management with libgpiod bias flags

- **Build system enhancements** - All examples build automatically
  - CMakeLists.txt updated to build all four example programs
  - Proper library linking for all executables
  - Enhanced build configuration and dependency management

### Fixed
- **Namespace conflicts resolved** - Clean standard library integration
  - Arduino math functions moved to `Arduino::` namespace
  - Eliminated conflicts with std::abs and other standard functions
  - Proper header inclusion and namespace management

- **Hardware compatibility issues** - Pull-resistor support implemented
  - Internal pull-up/pull-down resistor configuration via libgpiod
  - Proper bias flag handling for GPIO lines
  - Enhanced input pin reliability and noise immunity

### Security
- **MIT license headers** - Complete copyright protection
  - Added MIT license headers to all source files
  - Consistent copyright attribution throughout codebase
  - Legal compliance for open source distribution

### Technical Details
- Uses libgpiod bias flags for internal pull-resistor configuration
- Maintains backward compatibility with v0.1.0 Pin class API
- Exception-based error handling with descriptive messages
- RAII resource management with automatic GPIO cleanup
- Hardware-tested on Raspberry Pi with real GPIO devices

---

## [0.1.0] - 2025-10-30

### Added
- **Complete CMake build system** - Professional build configuration
  - CMakeLists.txt with modern CMake practices (version 3.16+)
  - Static library compilation with proper linking
  - Automated test building and execution with CTest integration
  - Example building system for demonstration programs
  - Installation targets for system-wide deployment
  - pkg-config file generation for easy library discovery
  - Build configuration summary and status reporting

- **Build automation scripts**
  - build.sh script for streamlined compilation process
  - BUILD.md documentation for build instructions and requirements
  - Support for both Debug and Release build configurations

- **Enhanced project structure**
  - Proper library packaging with version management
  - Public header installation support
  - Example programs in structured directories
  - Comprehensive test suite integration

### Changed
- **Updated Pin class implementation** - Improved from v0.0.1
  - Enhanced error handling and resource management
  - Better documentation and code organization
  - Improved method signatures and return types

- **Project version management**
  - Upgraded from v0.0.1 to v0.1.0
  - Consistent versioning across all project files
  - Semantic versioning compliance

### Fixed
- Build system dependencies and library linking
- Header file organization and installation paths
- Resource cleanup and memory management in Pin class

---

## [0.0.1] - 2025-10-29

### Added
- **Core Pin class implementation** - Complete GPIO pin control using libgpiod
  - Constructor with pin number, direction (INPUT/OUTPUT), and optional chip name
  - RAII-based resource management with proper destructor cleanup
  - `write(bool)` method for setting output pins HIGH/LOW with success feedback
  - `read()` method for reading input pin states
  - Comprehensive error handling with exceptions for failed operations

- **PinDirection enum class** - Type-safe pin direction specification
  - `PinDirection::INPUT` for reading from external devices
  - `PinDirection::OUTPUT` for controlling external devices

- **Comprehensive Doxygen documentation**
  - File-level documentation with project details, features, and usage examples
  - Class and method documentation with parameter and return value descriptions
  - Author, version, and date information
  - Code examples demonstrating basic usage patterns

- **Project infrastructure**
  - Header file structure in `include/pin.hpp`
  - Implementation in `src/pin.cpp` with libgpiod integration
  - Test program in `tests/test.cpp` demonstrating functionality
  - README.md with project vision, roadmap, and planned features

- **Build and test support**
  - Basic test executable compilation
  - libgpiod library integration
  - Error handling and debugging output

### Technical Details
- Uses modern libgpiod character device interface (recommended over deprecated sysfs)
- Exception-based error handling for robust operation
- Resource cleanup through RAII pattern
- Support for custom GPIO chip selection (defaults to "gpiochip0")
- Arduino-style API design for familiar usage patterns

### Dependencies
- libgpiod (Linux GPIO character device library)
- C++11 or later
- Standard library support for strings and exceptions

---

## Project Information

**Repository**: [PiPinPP](https://github.com/Barbatos6669/PiPinPP)
**Author**: HobbyHacker / Barbatos6669
**License**: See LICENSE file
**Language**: C++
**Platform**: Raspberry Pi (Linux)
