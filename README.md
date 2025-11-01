<div align="center">
  <img src="assets/logo_transparent.png" alt="PiPin++ Logo" width="300">
  
  # PiPin++
  
  **A modern C++ GPIO library for Raspberry Pi**  
  *Clean API, low-level control, and room to grow.*
</div>

![Version](https://img.shields.io/badge/version-0.2.0-blue) ![Roadmap Progress](https://img.shields.io/badge/roadmap-100%25%20v0.2.0-green) ![License](https://img.shields.io/badge/license-MIT-green)

📍 **[View our development roadmap →](ROADMAP.md)** | 🚀 **[Current version: 0.2.0](CHANGELOG.md)** | 📖 **[API Documentation →](docs/API_REFERENCE.md)**

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

### Next Milestones (v0.3.0)

- [ ] **Interrupt support**: Edge detection with callback functions (`attachInterrupt`)
- [ ] **PWM support**: Arduino-style `analogWrite()` for motor control and LED dimming
- [ ] **Timing functions**: `millis()` and `micros()` for precise timing
- [ ] **Testing framework**: Comprehensive unit tests and continuous integration
- [ ] **Performance optimization**: Fast GPIO operations and reduced overhead

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
