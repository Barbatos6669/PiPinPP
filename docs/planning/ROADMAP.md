# PiPinPP Roadmap

This document tracks the development roadmap for PiPinPP, a modern C++ GPIO library for Raspberry Pi designed to be familiar for Arduino makers.

**Current Version**: 0.3.2 (Released November 4, 2025)  
**Next Target**: 0.4.0 (Communication Protocols & Hardware PWM)  
**Last Updated**: November 4, 2025  
**Progress**: v0.3.2 Complete ✅ (100%) | v0.4.0 Planning Phase

---

## 🎉 RELEASE HISTORY

### v0.3.0 - Released November 3, 2025 ✅

**Production-ready Arduino-inspired GPIO library with advanced features!**

Eight major features delivered:
- ✅ v0.3.1 - Timing functions (millis, micros, delay, delayMicroseconds)
- ✅ v0.3.2 - Custom exceptions (PinError hierarchy)
- ✅ v0.3.3 - GitHub Actions CI/CD (multi-platform, CodeQL)
- ✅ v0.3.4 - GoogleTest framework (40 automated tests)
- ✅ v0.3.5 - Professional examples (timing, exceptions, threads)
- ✅ v0.3.6 - Modern CMake packaging (find_package support)
- ✅ v0.3.7 - GPIO interrupts (attachInterrupt with callbacks)
- ✅ v0.3.8 - Software PWM (analogWrite 0-255)

**Statistics**: 4,779 lines of code | 40 tests (100% passing) | 9 examples | 0 warnings

### v0.2.0 - Released October 31, 2025 ✅

**Arduino API compatibility complete!**

### ✅ Core API Enhancement - **COMPLETED**
- ✅ **Arduino-style API Functions** 
  - ✅ Implement `pinMode(pin, mode)` wrapper function
  - ✅ Implement `digitalWrite(pin, value)` wrapper function  
  - ✅ Implement `digitalRead(pin)` wrapper function
  - ✅ Add `HIGH` and `LOW` constants for Arduino-inspired API
  - ✅ Add `delay(ms)` function for Arduino-inspired API

### ✅ Pin Management - **COMPLETED**
- ✅ **Pull-up/Pull-down Resistor Support**
  - ✅ Add `PinMode` enum (INPUT, OUTPUT, INPUT_PULLUP, INPUT_PULLDOWN)
  - ✅ Extend Pin constructor to accept pull resistor configuration
  - ✅ Update libgpiod integration for resistor settings

- ✅ **Pin State Management**
  - ✅ Add pin state validation and error checking (0-27 range)
  - ✅ Implement pin availability checking (prevent conflicts)
  - ✅ Add pin capability detection (input/output support per pin)

### ✅ Examples and Documentation - **COMPLETED**
- ✅ **Arduino-style Example** 
  - ✅ Complete `examples/arduino_style/` with setup()/loop() pattern
  - ✅ Hardware tested LED blink example
  - ✅ CMake integration for examples working

- ✅ **Complete All Example Programs**
  - ✅ Finish `examples/basic_led/` implementation
  - ✅ Finish `examples/button_input/` implementation  
  - ✅ Finish `examples/arduino_migration/` with side-by-side comparison

- ✅ **Enhanced Documentation** 
  - ✅ GitHub Wiki with comprehensive Home page
  - ✅ CODE_STANDARDS.md with formatting guidelines
  - ✅ Complete Doxygen documentation for Arduino-inspired API functions
  - ✅ Usage examples in header documentation

- ✅ **API Documentation**
  - ✅ Complete Doxygen documentation for all Pin class methods
  - ✅ Create comprehensive API reference documentation

---

## 🚀 v0.4.0 - Communication Protocols & Hardware PWM

**Target**: Q1 2026  
**Focus**: Add I2C, SPI, UART support + hardware PWM for servo control  
**Status**: Planning Phase

### Core Protocol Support
- [ ] **I2C Master Support**
  - [ ] Linux i2c-dev integration
  - [ ] Arduino Wire library compatible API
  - [ ] `Wire.begin()`, `Wire.write()`, `Wire.read()`, etc.
  - [ ] Bus scanning utility
  - [ ] Example: BME280 temperature/pressure sensor
  - [ ] Unit tests with I2C device simulation

- [ ] **SPI Master Support**
  - [ ] Linux spidev integration
  - [ ] Arduino SPI library compatible API
  - [ ] `SPI.begin()`, `SPI.transfer()`, `SPI.setDataMode()`, etc.
  - [ ] Multiple chip select support
  - [ ] Example: MCP3008 ADC or NRF24L01 radio
  - [ ] Unit tests for SPI transactions

- [ ] **UART/Serial Support**
  - [ ] Linux termios integration
  - [ ] Arduino Serial compatible API
  - [ ] `Serial.begin()`, `Serial.print()`, `Serial.read()`, etc.
  - [ ] Multiple UART support (Serial1, Serial2)
  - [ ] Example: GPS module communication
  - [ ] Unit tests with virtual serial ports

- [ ] **Hardware PWM Support**
  - [ ] Native hardware PWM via PWM chip (pwm-bcm2835)
  - [ ] Auto-detect hardware vs software PWM
  - [ ] Servo-compatible frequency and resolution
  - [ ] Example: Precise servo control
  - [ ] Fallback to software PWM for unsupported pins

### API Enhancements
- [ ] **Pin State Queries**
  - [ ] `isOutput()`, `isInput()`, `getMode()` functions
  - [ ] Pin capability detection
  - [ ] State validation utilities

- [ ] **Additional Functions**
  - [ ] `digitalToggle(pin)` - Toggle pin state
  - [ ] Bulk GPIO operations (future consideration)

### Quality & Performance
- [ ] **Performance Benchmarking**
  - [ ] GPIO toggle speed measurements
  - [ ] Interrupt latency measurements
  - [ ] PWM frequency accuracy tests
  - [ ] Protocol throughput measurements
  - [ ] Memory usage profiling

- [ ] **Namespace Migration** (Optional - Breaking Change)
  - [ ] Wrap core API in `pipinpp::` namespace
  - [ ] Keep Arduino layer in global scope
  - [ ] Migration guide for users

### Package Management
- [ ] **Debian Package Creation**
  - [ ] .deb package for easy installation
  - [ ] Package repository setup
  - [ ] apt-get installation support

---

## 🔮 v0.5.0+ - Future Features

### Advanced Protocol Features
- [ ] **Advanced I2C**
  - [ ] I2C slave mode support
  - [ ] SMBus protocol support
  - [ ] Clock stretching support
  - [ ] Multi-master support

- [ ] **Advanced SPI**
  - [ ] Full-duplex simultaneous read/write
  - [ ] SPI slave mode support
  - [ ] DMA transfers for high-speed data
  - [ ] Dual/Quad SPI support

- [ ] **Advanced Serial**
  - [ ] Hardware flow control (RTS/CTS)
  - [ ] RS-485 support
  - [ ] Serial port enumeration

### Additional Protocols
- [ ] **One-Wire Protocol**
  - [ ] DS18B20 temperature sensor support
  - [ ] Arduino OneWire library compatibility
  - [ ] Device enumeration

- [ ] **CAN Bus Support**
  - [ ] SocketCAN integration
  - [ ] CAN message filtering
  - [ ] Arduino-style CAN API

### Development Tools
- [ ] **VS Code Extension**
  - [ ] GPIO pin monitoring view
  - [ ] Real-time state visualization
  - [ ] Interactive pin configuration
  - [ ] Code snippets and templates

- [ ] **Web-Based Monitor**
  - [ ] REST API for GPIO control
  - [ ] WebSocket real-time updates
  - [ ] Browser-based dashboard
  - [ ] Mobile-responsive design

---

## 🔮 Long-term Goals (v1.0.0+)

### Platform & Compatibility
- [ ] **Multi-platform Support**
  - [ ] Orange Pi compatibility
  - [ ] Other ARM SBC support
  - [ ] Generic Linux GPIO support
  - [ ] Cross-platform abstraction layer

- [ ] **Development Environment Support**
  - [ ] VS Code extension for GPIO debugging/monitoring
  - [ ] CMake integration templates for new projects
  - [ ] Code completion and IntelliSense improvements
  - [ ] Real-time GPIO visualization

### Advanced Features
- [ ] **Analog Input**
  - [ ] ADC support via external chips (MCP3008, etc.)
  - [ ] `analogRead(pin)` Arduino-style function
  - [ ] Multi-channel ADC support

- [ ] **Performance Optimization**
  - [ ] Fast GPIO operations
  - [ ] Memory-mapped GPIO access
  - [ ] Batch operations for multiple pins
  - [ ] Zero-copy data transfer

---

## 📋 Project Infrastructure

### Documentation
- [ ] **User Guide**
  - [ ] Getting started tutorial
  - [ ] Arduino migration guide
  - [ ] Hardware setup instructions
  - [ ] Troubleshooting section

- [ ] **Developer Documentation**
  - [ ] Architecture overview
  - [ ] Contributing guidelines enhancement
  - [ ] Code style guide
  - [ ] Release process documentation

### Community & Outreach
- [ ] **Community Building**
  - [ ] Create project website/GitHub Pages
  - [ ] Arduino forum announcement
  - [ ] Raspberry Pi community outreach
  - [ ] YouTube tutorials/demos

- [ ] **Feedback Collection**
  - [ ] User survey for Arduino migrants
  - [ ] Feature request tracking system
  - [ ] Regular community feedback sessions

### Release Management
- [ ] **Release Automation**
  - [ ] Automated version bumping
  - [ ] Release notes generation
  - [ ] Tag and release workflow
  - [ ] Changelog automation

---

## 🐛 Known Issues & Technical Debt

### v0.3.0 Priority Issues
- [ ] **Remove Debug Output**
  - [ ] Make std::cout debug messages optional or remove for production
  - [ ] Implement proper logging system with levels
  - [ ] Add quiet mode for library usage

- [ ] **Performance Optimizations**
  - [ ] Remove global pin storage overhead in Arduino-inspired API layer
  - [ ] Optimize pin creation/destruction cycles
  - [ ] Add pin state caching for repeated reads

- [ ] **API Improvements**
  - [ ] Add digitalToggle() function for pin state toggling
  - [ ] Implement pin state queries (isOutput(), isInput(), etc.)
  - [ ] Add bulk pin operations (read/write multiple pins)

### Current Limitations (Known Issues)
- [ ] **Pin Class Improvements**
  - [ ] Add copy/move constructors and assignment operators
  - [ ] Implement proper RAII with exception safety
  - [ ] Add pin ownership tracking to prevent conflicts

- [ ] **Error Handling**
  - [ ] More descriptive error messages with troubleshooting hints
  - [ ] Error code standardization across library
  - [ ] Logging framework integration with different verbosity levels

### Code Quality
- [ ] **Static Analysis**
  - [ ] Set up clang-tidy integration in CI/CD
  - [ ] Address any static analysis warnings
  - [ ] Code formatting standardization (clang-format)

- [ ] **Memory Management**
  - [ ] Valgrind testing for memory leaks
  - [ ] RAII improvements throughout codebase
  - [ ] Smart pointer usage where appropriate

---

## 📊 Metrics & Success Criteria

### v0.2.0 Success Criteria ✅ **ACHIEVED!**
- ✅ All high-priority items completed
- ✅ 4 working example programs (basic_led, button_input, arduino_migration, arduino_style)
- ✅ Complete Arduino API compatibility with pull-up support
- ✅ Comprehensive documentation for all core features
- ✅ Hardware tested and validated on Raspberry Pi
- ✅ Pin validation and error handling implemented

### v0.3.0 Success Criteria ✅ **ACHIEVED!**
- ✅ Interrupt support with callback functions
- ✅ PWM support with analogWrite() function
- ✅ millis() and micros() timing functions
- ✅ Unit testing framework with >80% coverage (40 tests)
- ✅ Continuous integration pipeline with CodeQL
- ✅ 9 comprehensive examples
- ✅ Modern CMake packaging

### v0.4.0 Success Criteria
- [ ] I2C master support with Wire API
- [ ] SPI master support with SPI API
- [ ] UART support with Serial API
- [ ] Hardware PWM support
- [ ] At least 2 protocol examples
- [ ] Performance benchmarking results
- [ ] Debian package (.deb)

### v1.0.0 Success Criteria
- [ ] Full Arduino digital I/O API compatibility
- [ ] At least one communication protocol (I2C or SPI)
- [ ] Community adoption (100+ GitHub stars)
- [ ] Positive feedback from Arduino migration users
- [ ] Production-ready stability and performance

---

## 💡 Ideas for Future Consideration

- **Visual Programming Interface**: Block-based programming like Scratch for GPIO
- **Web Interface**: Browser-based GPIO control and monitoring
- **Mobile App**: Smartphone control of GPIO pins
- **Home Assistant Integration**: IoT platform compatibility
- **Cross-compilation Support**: Easy building for Pi from development machines
- **Python Bindings**: PyBind11 wrapper for Python developers transitioning from RPi.GPIO

---

## 📝 Notes

- Focus on Arduino user experience and familiar API patterns
- Maintain lightweight and efficient core library
- Prioritize documentation and examples for easy adoption
- Keep compatibility with standard Linux GPIO interfaces
- Regular community feedback integration

---

*This TODO list is a living document. Items may be reprioritized based on community feedback and project needs.*