# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.3.2] - 2025-11-04

### Added
- **Extended math functions** - Arduino-compatible mathematical operations (v0.3.2)
  - `sq(x)` - Square a number (template function for any numeric type)
  - `sqrt(x)` - Square root function (std::sqrt wrapper)
  - `pow(base, exponent)` - Power function (std::pow wrapper)
  - `max(a, b)` - Maximum of two values (template for any comparable type)
  - `min(a, b)` - Minimum of two values (template for any comparable type)
  - Header-only inline implementations for optimal performance
  - Template functions provide type flexibility (int, long, float, double)
  - Full Arduino API compatibility
  - Comprehensive Doxygen documentation with usage examples
  - Updated API_REFERENCE.md with Extended Math Functions section

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

**Complete Arduino-compatible GPIO library with advanced features!**

This major release brings PiPinPP to production-ready status with 8 major feature additions including interrupt support, PWM, comprehensive testing framework, and professional tooling.

### Added
- **Software PWM support** - Arduino-compatible analog output (v0.3.8)
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
  - Interrupt modes: RISING, FALLING, CHANGE (Arduino-compatible)
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
- Thread-safety issues in Arduino compatibility layer

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

- **Complete Arduino compatibility layer** - Full ArduinoCompat.hpp implementation
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
  - `arduino_style/` - LED blinking using Arduino-compatible API
  - `basic_led/` - Object-oriented LED control with Pin class
  - `button_input/` - Button reading with debouncing and pull-up resistors
  - `arduino_migration/` - Side-by-side Arduino vs PiPinPP code comparison

- **Complete API documentation** - Professional Doxygen documentation
  - Full Pin class method documentation with examples
  - Arduino compatibility function documentation
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