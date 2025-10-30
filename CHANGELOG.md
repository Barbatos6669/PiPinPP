# Changelog

All notable changes to PiPinPP will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Nothing yet

### Changed
- Nothing yet

### Deprecated
- Nothing yet

### Removed
- Nothing yet

### Fixed
- Nothing yet

### Security
- Nothing yet

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