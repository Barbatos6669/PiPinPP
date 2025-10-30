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