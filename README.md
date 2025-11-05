<div align="center">
  <img src="assets/logo_transparent.png" alt="PiPin++ Logo" width="300">
  
  # PiPin++
  
  **A modern C++ GPIO library for Raspberry Pi**  
  *Clean API, low-level control, and room to grow.*
</div>

[![CI](https://github.com/Barbatos6669/PiPinPP/actions/workflows/ci.yml/badge.svg)](https://github.com/Barbatos6669/PiPinPP/actions/workflows/ci.yml)
[![CodeQL](https://img.shields.io/badge/CodeQL-passing-brightgreen)](https://github.com/Barbatos6669/PiPinPP/security/code-scanning)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/version-0.3.4-blue.svg)](https://github.com/Barbatos6669/PiPinPP/releases/tag/v0.3.4)
[![Platform](https://img.shields.io/badge/platform-Raspberry%20Pi-red.svg)](https://www.raspberrypi.org/)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![libgpiod](https://img.shields.io/badge/libgpiod-2.2.1-green.svg)](https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git)
[![Tests](https://img.shields.io/badge/tests-40%20passing-brightgreen)](https://github.com/Barbatos6669/PiPinPP)

📍 **[Development Roadmap](docs/planning/ROADMAP.md)** | 🚀 **[Version 0.3.4](CHANGELOG.md)** | 📖 **[API Reference](docs/API_REFERENCE.md)** | 📌 **[Pin Numbering Guide](docs/PIN_NUMBERING.md)** | 🔐 **[Permissions Setup](docs/PERMISSIONS.md)**

---

> **⚡ Quick Install:** `curl -sSL https://raw.githubusercontent.com/Barbatos6669/PiPinPP/v0.3.4/install.sh | sudo bash`

---

## Project Vision

**PiPin++** aims to make Raspberry Pi GPIO control easy and familiar for makers, especially those coming from Arduino. Our goal is a modern, lightweight, and extensible C++ library that feels instantly comfortable for Arduino users—removing the hassle of adapting to Pi-specific APIs.

## Why?

- **Arduino makers often struggle to migrate projects to Raspberry Pi**, facing steep learning curves and inconsistent libraries.
- **We want PiPin++ to bridge that gap** with a clean, intuitive API and modern C++ features.

## Who's it for?

- Arduino users wanting to try Raspberry Pi projects
- Makers, hackers, students, educators
- Anyone needing simple, reliable GPIO control in C++

---

## Quick Start Example

**Blink an LED in 10 lines of code:**

```cpp
#include <ArduinoCompat.hpp>

int main() {
    pinMode(17, OUTPUT);              // Set GPIO 17 as output
    
    while (true) {
        digitalWrite(17, HIGH);       // Turn LED on
        delay(1000);                  // Wait 1 second
        digitalWrite(17, LOW);        // Turn LED off
        delay(1000);                  // Wait 1 second
    }
}
```

**Compile and run:**
```bash
g++ blink.cpp $(pkg-config --cflags --libs pipinpp) -o blink
sudo ./blink
```

That's it! If you know Arduino, you already know PiPin++. 🚀

---

## Features

**v0.3.4 RELEASED!** 🎉 Complete Arduino-compatible GPIO library with utility functions!

### ✅ Current Features (v0.3.2)

- ✅ **Digital input/output**: Arduino-style pin control (`pinMode`, `digitalWrite`, `digitalRead`)
- ✅ **Pin abstraction**: Simple, object-oriented pin management with RAII
- ✅ **Pull-up/pull-down resistors**: Built-in `INPUT_PULLUP` and `INPUT_PULLDOWN` support
- ✅ **Arduino API migration**: Easy transition from Arduino sketches
- ✅ **Complete examples**: LED control, button input, migration guide
- ✅ **Comprehensive documentation**: Full API reference and hardware setup guides

### Core GPIO (v0.1.0-v0.2.0)
- ✅ **Digital I/O**: Arduino-style pin control (`pinMode`, `digitalWrite`, `digitalRead`)
- ✅ **Pin abstraction**: Object-oriented pin management with RAII
- ✅ **Pull resistors**: Built-in `INPUT_PULLUP` and `INPUT_PULLDOWN` support
- ✅ **Arduino API**: Easy transition from Arduino sketches

### Advanced Features (v0.3.0-v0.3.2)
- ✅ **Timing functions**: `millis()`, `micros()`, `delay()`, `delayMicroseconds()` for precise timing
- ✅ **Interrupts**: Edge detection with callbacks (`attachInterrupt`, `detachInterrupt`)
- ✅ **PWM**: Software PWM with `analogWrite()` (0-255 duty cycle, configurable frequency)
- ✅ **Pin queries**: Check pin state with `isOutput()`, `isInput()`, `getMode()`, `digitalToggle()`
- ✅ **Custom exceptions**: Type-safe error handling (`InvalidPinError`, `GpioAccessError`)
- ✅ **Math functions**: Arduino-compatible `sq()` function (v0.3.2)

### Quality & Testing (v0.3.0+)
- ✅ **GoogleTest framework**: 40 comprehensive automated tests
- ✅ **GitHub Actions CI/CD**: Multi-platform builds, automated testing, CodeQL security analysis
- ✅ **Modern CMake**: find_package() support, shared/static library options, pkg-config compatibility
- ✅ **10 Examples**: Comprehensive demonstrations from basic to advanced usage
- ✅ **Clean project structure**: Organized documentation, no root clutter (v0.3.2)

**v0.3.2 is production-ready with excellent Arduino compatibility!**

### Longer-Term Goals (v0.4.0+)

- [ ] **Communication protocols**: I2C, SPI, and UART support
- [ ] **Multi-platform support**: Orange Pi and other ARM SBCs
- [ ] **Advanced features**: Analog input, GPIO monitoring tools
- [ ] **Development tools**: VS Code extensions and debugging utilities

---

## Installation

### 🚀 One-Line Install (Easiest!)

Install PiPinPP v0.3.4 with a single command - just like pip!

```bash
curl -sSL https://raw.githubusercontent.com/Barbatos6669/PiPinPP/v0.3.4/install.sh | sudo bash
```

**What this does:**
- ✅ Installs all dependencies (build tools, libgpiod v2)
- ✅ Downloads and builds PiPinPP v0.3.4
- ✅ Installs to `/usr/local` (system-wide)
- ✅ Configures GPIO permissions (adds you to `gpio` group)
- ✅ Sets up udev rules for non-sudo access
- ✅ Verifies installation

**Trust but verify?** Download and inspect the script first:

```bash
wget https://raw.githubusercontent.com/Barbatos6669/PiPinPP/v0.3.4/install.sh
less install.sh  # Review the script
sudo bash install.sh
```

### Manual Installation

**1. Install dependencies:**
```bash
sudo apt-get update
sudo apt-get install build-essential cmake pkg-config git libgpiod-dev
```

**2. Build and install:**
```bash
git clone https://github.com/Barbatos6669/PiPinPP.git
cd PiPinPP
git checkout v0.3.4  # Install latest stable release
./build.sh           # Build with all tests
cd build
sudo make install    # Install to /usr/local
sudo ldconfig        # Update library cache
```

**3. Verify installation:**
```bash
pkg-config --modversion pipinpp  # Should show: 0.3.4
```

**4. Configure permissions (optional but recommended):**
```bash
sudo usermod -a -G gpio $USER
# Log out and back in for this to take effect
```

### 🔄 Updating to a Newer Version

Already have PiPinPP installed? Updating is just as easy!

**Option 1: One-Line Update (Recommended)**
```bash
curl -sSL https://raw.githubusercontent.com/Barbatos6669/PiPinPP/v0.3.4/install.sh | sudo bash
```
*Replace `v0.3.4` with the version you want (e.g., `v0.4.0`)*

**Option 2: Manual Update**
```bash
cd PiPinPP
git pull origin main
git checkout v0.3.4  # Replace with desired version
./build.sh
cd build
sudo make install
```

**Check your installed version:**
```bash
pkg-config --modversion pipinpp
```

> 💡 **All updates are backward compatible** - your existing code will continue to work!

### Using in Your Projects

**With pkg-config:**
```bash
g++ main.cpp $(pkg-config --cflags --libs pipinpp) -o main
```

**With CMake:**
```cmake
find_package(PiPinPP REQUIRED)
target_link_libraries(your_app PiPinPP::pipinpp)
```

---

## Contributing

**We're just getting started and welcome your ideas, feedback, and contributions!**

📋 **Check our [ROADMAP.md](docs/planning/ROADMAP.md) to see what we're working on next**

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

See [CONTRIBUTING.md](.github/CONTRIBUTING.md) for detailed contribution guidelines.

---

## License

MIT

---

**PiPin++**: Making Raspberry Pi feel like home for Arduino makers.
