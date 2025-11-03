<div align="center">
  <img src="assets/logo_transparent.png" alt="PiPin++ Logo" width="300">
  
  # PiPin++
  
  **A modern C++ GPIO library for Raspberry Pi**  
  *Clean API, low-level control, and room to grow.*
</div>

[![CI](https://github.com/Barbatos6669/PiPinPP/actions/workflows/ci.yml/badge.svg)](https://github.com/Barbatos6669/PiPinPP/actions/workflows/ci.yml)
[![CodeQL](https://img.shields.io/badge/CodeQL-passing-brightgreen)](https://github.com/Barbatos6669/PiPinPP/security/code-scanning)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/version-0.3.0--dev-blue.svg)](https://github.com/Barbatos6669/PiPinPP)
[![Platform](https://img.shields.io/badge/platform-Raspberry%20Pi-red.svg)](https://www.raspberrypi.org/)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![libgpiod](https://img.shields.io/badge/libgpiod-1.6%2B-green.svg)](https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git)
[![Roadmap](https://img.shields.io/badge/roadmap-100%25%20v0.2.0-brightgreen)](ROADMAP.md)

📍 **[Development Roadmap](ROADMAP.md)** | 🚀 **[Version 0.2.0](CHANGELOG.md)** | 📖 **[API Reference](docs/API_REFERENCE.md)** | 📌 **[Pin Numbering Guide](docs/PIN_NUMBERING.md)** | 🔐 **[Permissions Setup](docs/PERMISSIONS.md)**

---

## Project Vision

**PiPin++** aims to make Raspberry Pi GPIO control easy and familiar for makers, especially those coming from Arduino. Our goal is a modern, lightweight, and extensible C++ library that feels instantly comfortable for Arduino users—removing the hassle of adapting to Pi-specific APIs.

## Why?

- **Arduino makers often struggle to migrate projects to Raspberry Pi**, facing steep learning curves and inconsistent libraries.
- **We want PiPin++ to bridge that gap** with a clean, intuitive API and modern C++ features.

## Who’s it for?

- Arduino users wanting to try Raspberry Pi projects
- Makers, hackers, students, educators
- Anyone needing simple, reliable GPIO control in C++

---

## Roadmap

**v0.2.0 COMPLETE!** ✅ All major Arduino compatibility features implemented!

### ✅ Completed Features (v0.2.0)

- ✅ **Digital input/output**: Arduino-style pin control (`pinMode`, `digitalWrite`, `digitalRead`)
- ✅ **Pin abstraction**: Simple, object-oriented pin management with RAII
- ✅ **Pull-up/pull-down resistors**: Built-in `INPUT_PULLUP` and `INPUT_PULLDOWN` support
- ✅ **Arduino API migration**: Easy transition from Arduino sketches
- ✅ **Complete examples**: LED control, button input, migration guide
- ✅ **Comprehensive documentation**: Full API reference and hardware setup guides

### ✅ v0.3.0 COMPLETE! 🎉

- ✅ **Timing functions**: `millis()`, `micros()`, `delay()`, and `delayMicroseconds()` for precise timing
- ✅ **Custom exceptions**: Type-safe error handling with `InvalidPinError` and `GpioAccessError`
- ✅ **GitHub Actions CI/CD**: Automated builds, tests, and security scanning on every push
- ✅ **GoogleTest framework**: 40 comprehensive tests with professional reporting
- ✅ **New examples**: 5 comprehensive examples (timing, exceptions, threads, interrupts, PWM)
- ✅ **Modern CMake packaging**: find_package() support, namespace alias, and version checking
- ✅ **Interrupt support**: Edge detection with callback functions (`attachInterrupt`, `detachInterrupt`)
- ✅ **PWM support**: Arduino-style `analogWrite()` for motor control and LED dimming

**All planned v0.3.0 features complete and ready for release!**

### Longer-Term Goals (v0.4.0+)

- [ ] **Communication protocols**: I2C, SPI, and UART support
- [ ] **Multi-platform support**: Orange Pi and other ARM SBCs
- [ ] **Advanced features**: Analog input, GPIO monitoring tools
- [ ] **Development tools**: VS Code extensions and debugging utilities

---

## Contributing

**We're just getting started and welcome your ideas, feedback, and contributions!**

📋 **Check our [ROADMAP.md](ROADMAP.md) to see what we're working on next**

## 💬 Community & Support

**Join our Discord community for real-time support and discussions:**
- 🚀 **Quick help** with setup, coding issues, and troubleshooting  
- 💡 **Feature discussions** and Arduino migration questions
- 🔧 **Hardware projects** and GPIO usage examples
- 📢 **Development updates** and release announcements

> **[Join PiPin++ Discord Server](https://discord.gg/wXeZP8Ev)**

For formal bug reports and feature requests, please use GitHub Issues.

## 🤝 Contributing

- If you're an Arduino user, tell us which features or API patterns you'd love to see.
- Open an issue for suggestions, questions, or bug reports.
- Pull requests and documentation help are always welcome!
- Look for `good first issue` labels for beginner-friendly tasks.

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed contribution guidelines.

---

## License

MIT

---

**PiPin++**: Making Raspberry Pi feel like home for Arduino makers.
