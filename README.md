<div align="center">
  <img src="assets/logo_transparent.png" alt="PiPin++ Logo" width="300">
  
  # PiPin++
  
  **A modern C++ GPIO library for Raspberry Pi**  
  *Clean API, low-level control, and room to grow.*
</div>

[![CI](https://github.com/Barbatos6669/PiPinPP/actions/workflows/ci.yml/badge.svg)](https://github.com/Barbatos6669/PiPinPP/actions/workflows/ci.yml)
[![Docs](https://github.com/Barbatos6669/PiPinPP/actions/workflows/docs.yml/badge.svg)](https://github.com/Barbatos6669/PiPinPP/actions/workflows/docs.yml)
[![Coverage](https://img.shields.io/badge/coverage-73.5%25-brightgreen)](https://barbatos6669.github.io/PiPinPP/coverage/)
[![CodeQL](https://img.shields.io/badge/CodeQL-passing-brightgreen)](https://github.com/Barbatos6669/PiPinPP/security/code-scanning)
[![CodeFactor](https://www.codefactor.io/repository/github/Barbatos6669/PiPinPP/badge)](https://www.codefactor.io/repository/github/Barbatos6669/PiPinPP)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

[![Version](https://img.shields.io/badge/version-0.4.0-blue.svg)](https://github.com/Barbatos6669/PiPinPP/releases/tag/v0.4.0)
[![Platform](https://img.shields.io/badge/platform-Raspberry%20Pi-red.svg)](https://www.raspberrypi.org/)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![libgpiod](https://img.shields.io/badge/libgpiod-2.2.1-green.svg)](https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git)
[![Tests](https://img.shields.io/badge/tests-672%20passing-brightgreen)](https://github.com/Barbatos6669/PiPinPP)
[![Examples](https://img.shields.io/badge/examples-25+-blue)](examples/)
[![Tutorials](https://img.shields.io/badge/tutorials-7-success)](docs/tutorials/)
[![Wiki](https://img.shields.io/badge/wiki-comprehensive-informational)](https://github.com/Barbatos6669/PiPinPP/wiki)

🚀 **[Getting Started](docs/GETTING_STARTED.md)** | 📚 **[Tutorials](docs/tutorials/)** | 📖 **[Wiki](https://github.com/Barbatos6669/PiPinPP/wiki)** | 📘 **[API Reference](docs/API_REFERENCE.md)** | 🐛 **[Troubleshooting](docs/TROUBLESHOOTING.md)** | 🗺️ **[Roadmap](docs/ROADMAP.md)** | 🔒 **[Security](.github/SECURITY.md)**

---

> **⚡ Quick Install:** `curl -sSL https://raw.githubusercontent.com/Barbatos6669/PiPinPP/v0.4.0/install.sh | sudo bash`

> **🗺️ NEW: [2026 Roadmap](docs/ROADMAP.md)** - See our path to becoming the #1 GPIO library! **[Vote on features →](https://github.com/Barbatos6669/PiPinPP/issues/new?template=roadmap_feature.md)**

---

## Quick Links

- [Getting Started Guide](docs/GETTING_STARTED.md)
- [Build Instructions](docs/BUILD.md)
- [API Reference (Markdown)](docs/API_REFERENCE.md) · [Doxygen HTML](https://barbatos6669.github.io/PiPinPP/doxygen/index.html)
- [Examples Directory](examples/)
- [Tutorial Collection](docs/tutorials/README.md)
- [Python Bindings (`pypipinpp`)](bindings/python/README.md)
- [pipinpp CLI Usage](docs/CLI_USAGE.md)
- [Developer Onboarding](docs/DEVELOPER.md) · [Contributing](.github/CONTRIBUTING.md)
- [Platform Support Matrix](docs/PLATFORMS.md)

## Project Vision

**PiPin++** aims to make Raspberry Pi GPIO control easy and familiar for makers, especially those coming from Arduino. Our goal is a modern, lightweight, and extensible C++ library that feels instantly comfortable for Arduino users—removing the hassle of adapting to Pi-specific APIs.


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
  pinMode(17, OUTPUT);
    
  while (true) {
    digitalWrite(17, HIGH);
    delay(1000);
    digitalWrite(17, LOW);
    delay(1000);
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

## 📚 New to GPIO? Start Here!

**Complete beginner?** We've got you covered:

1. 🚀 **[Getting Started Guide](docs/GETTING_STARTED.md)** - Your first LED in 10 minutes
2. 📚 **[Tutorials](docs/tutorials/)** - Step-by-step projects (Traffic Light, Buttons, PWM)
3. 🐛 **[Troubleshooting](docs/TROUBLESHOOTING.md)** - Fix common issues
4. 📖 **[API Reference](docs/API_REFERENCE.md)** - Complete function documentation

Featured walkthroughs:
- [Controlling an LED](docs/tutorials/LED_BLINK.md)
- [Button Input](docs/tutorials/BUTTON_INPUT.md)
- [Interrupts 101](docs/tutorials/INTERRUPTS_101.md)
- [I2C Sensor Dashboard](docs/tutorials/I2C_SENSOR.md)
- [PWM Basics](docs/tutorials/PWM_BASICS.md)
- [Multi-Threading](docs/tutorials/MULTI_THREADING.md)

**Coming from Arduino?** Check out:
- [Arduino Migration Example](examples/arduino_migration/) - Side-by-side comparison
- [Arduino Style Example](examples/arduino_style/) - Familiar `setup()` and `loop()`
- [Tutorial Index](docs/tutorials/README.md) - Curated learning paths and hardware checklists

---

## Features

**v0.4.0 RELEASED!** 🎉 Test Coverage Milestone: 73.5% with 672 tests!

### 🏆 Production-Ready Quality
- ✅ **73.5% test coverage** (+13.7% from v0.3.13) with 672 comprehensive tests
- ✅ **94.8% function coverage** (202/213 functions fully tested)
- ✅ **CI passing** on ubuntu-22.04 and ubuntu-24.04 with strict warnings-as-errors
- ✅ **Thread-safe** validated across all APIs with dedicated concurrency tests
- ✅ **Exception-safe** with comprehensive error handling tests
- ✅ **Zero known bugs** in Arduino-compatible API surface

### Core GPIO (v0.1.0-v0.2.0)
- ✅ **Digital I/O**: Arduino-style pin control (`pinMode`, `digitalWrite`, `digitalRead`)
- ✅ **Pin abstraction**: Object-oriented pin management with RAII
- ✅ **Pull resistors**: Built-in `INPUT_PULLUP` and `INPUT_PULLDOWN` support
- ✅ **Arduino API**: Easy transition from Arduino sketches

### Advanced Features (v0.3.0-v0.3.12)
- ✅ **Timing functions**: `millis()`, `micros()`, `delay()`, `delayMicroseconds()` for precise timing
- ✅ **Interrupts**: Edge detection with callbacks (`attachInterrupt`, `detachInterrupt`)
- ✅ **Software PWM**: Thread-based PWM with `analogWrite()` (LED dimming, ~10-30% CPU per pin)
- ✅ **Hardware PWM**: Jitter-free PWM via `/sys/class/pwm` for servo control (GPIO12/13/18/19, zero CPU usage) (v0.3.13)
- ✅ **Pin queries**: Check pin state with `isOutput()`, `isInput()`, `getMode()`, `digitalToggle()`
- ✅ **Custom exceptions**: Type-safe error handling (`InvalidPinError`, `GpioAccessError`)
- ✅ **Math functions**: Arduino-inspired math functions (`sq()`, `constrain()`, `map()`, trigonometry constants)
- ✅ **Advanced I/O**: `pulseIn()`, `shiftOut()`, `shiftIn()`, `tone()`, `noTone()` (v0.3.6)
- ✅ **I2C/Wire**: Full Arduino Wire library compatibility for I2C communication (v0.3.6)
- ✅ **SPI**: Hardware SPI master with full Arduino compatibility (v0.3.7)
- ✅ **Serial/UART**: Arduino Serial API for communication with Arduino boards and sensors (v0.3.12)

### Quality & Testing (v0.3.0-v0.4.0)
- ✅ **GoogleTest framework**: 672 comprehensive automated tests (+260 since v0.3.13)
- ✅ **GitHub Actions CI/CD**: Multi-platform builds, automated testing, CodeQL security analysis
- ✅ **Modern CMake**: find_package() support, shared/static library options, pkg-config compatibility
- ✅ **25+ Examples with Full Documentation**: Every example includes comprehensive README with wiring diagrams, troubleshooting, and extension ideas
- ✅ **Performance profiler**: `examples/performance_profiler` logs drift/jitter metrics and GPIO throughput to CSV for reproducible tuning sessions
- ✅ **Clean project structure**: Organized documentation, no root clutter
- ✅ **Platform detection**: Automatic Raspberry Pi model detection and configuration

**v0.4.0 is production-ready with full Arduino API compatibility and extensive test coverage!**

## Test Coverage

[![Coverage](https://img.shields.io/badge/coverage-73.5%25-brightgreen)](https://barbatos6669.github.io/PiPinPP/coverage/index.html)

- **Overall:** 73.5% line coverage / 94.8% function coverage (gcov + lcov)
- **Hot spots:** Core `Pin`, Wire/SPI stacks, ArduinoCompat, math utilities
- **Next targets:** Software PWM edge cases (`pwm.cpp`), CLI integration tests, high-frequency interrupt fuzzing
- **How to view locally:** `cmake -S . -B build -DENABLE_COVERAGE=ON && cmake --build build --target coverage` → open `build/coverage_html/index.html`
- **Public docs:** https://barbatos6669.github.io/PiPinPP/coverage/

### Longer-Term Goals (v0.4.0+)
- [ ] **Multi-platform support**: Orange Pi and other ARM SBCs
- [ ] **Advanced features**: Analog input, GPIO monitoring tools
- [ ] **Development tools**: VS Code extensions and debugging utilities

---

## Performance Profiling Tools

Need real numbers for timing drift or GPIO throughput? Build the profiling example and log the
results for later analysis:

- **Delay drift** – Verifies `delay()` accuracy across multiple samples
- **Loop jitter** – Measures `delayMicroseconds()` while maintaining a 1 kHz loop
- **GPIO throughput** – Reports average cost of paired `digitalWrite()` calls (default GPIO 17)
- **CSV export** – Saves `performance_profile.csv` so you can plot histograms in Python/Excel

```bash
cd /path/to/PiPinPP/build
make example_performance_profiler
./examples/example_performance_profiler   # add sudo if your user is not in the gpio group
```

See `examples/performance_profiler/README.md` for tuning tips and sample output.

---

## Installation

### 🚀 One-Line Install (Recommended)

```bash
curl -sSL https://raw.githubusercontent.com/Barbatos6669/PiPinPP/v0.3.13/install.sh | sudo bash
```

This installs all dependencies, builds PiPinPP v0.3.13, and configures GPIO permissions automatically.

**Trust but verify?** Review the script first:
```bash
wget https://raw.githubusercontent.com/Barbatos6669/PiPinPP/v0.3.13/install.sh
less install.sh
sudo bash install.sh
```

### Manual Installation

```bash
sudo apt-get update
sudo apt-get install build-essential cmake pkg-config git ninja-build curl
# Install libgpiod 2.2.1 from source or run ./install.sh --libgpiod-only (see docs/PLATFORMS.md)
sudo apt-get install build-essential cmake pkg-config git libgpiod-dev

# 2. Build and install
cd PiPinPP
git checkout v0.4.0
./build.sh
cd build
sudo make install
sudo ldconfig
sudo ldconfig

# 3. Configure permissions
# Log out and back in for group changes to apply
sudo usermod -a -G gpio $USER
# Log out and back in for this to take effect
```

pkg-config --modversion pipinpp  # Should show: 0.4.0
```bash
pkg-config --modversion pipinpp  # Should show: 0.3.7
```

📖 **For detailed installation options and troubleshooting, see [INSTALL.md](docs/INSTALL.md)**

## Platform & Compatibility Matrix

- Supported boards: Raspberry Pi 5, 4, 3, Zero 2 W, CM4 (see [docs/PLATFORMS.md](docs/PLATFORMS.md) for notes)
- Experimental targets: Orange Pi 5, Jetson Nano, BeagleBone (roadmap v0.5+)

| PiPin++ Version | Tested libgpiod | Notes |
|-----------------|-----------------|-------|
| 0.4.0 (current) | 2.2.1 (preferred), 2.0.2 | Full feature set incl. platform detector + interrupts |
| 0.3.7           | 2.0.x            | Transitional release before platform detector |
| ≤0.3.0          | 1.6.3 (legacy)   | Deprecated – upgrade ASAP |

`install.sh` automatically builds libgpiod 2.2.1 on Raspberry Pi OS so you don't have to source
packages manually.

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

## pipinpp CLI Tool

Prefer quick experiments over editing C++? Build the CLI (`cmake --build build --target pipinpp_cli`) and run
commands such as:

```bash
pipinpp info
pipinpp mode 17 out
pipinpp write 17 1
pipinpp i2c scan
pipinpp benchmark
pipinpp monitor 23 5
pipinpp doctor
```

The CLI mirrors the Arduino-compatible API, making it great for smoke tests and lab automation. See
`docs/CLI_USAGE.md` for the full command matrix, installation tips, and environment overrides.

---

## 🗺️ Roadmap to "Super Champ" Status

**PiPinPP is on a mission to become the #1 GPIO library for Raspberry Pi!**

We have a comprehensive [2026 Roadmap](docs/ROADMAP.md) with exciting plans:

- **🏎️ Phase 1 (v0.4.0)**: Performance optimization - Reduce software PWM CPU usage by 70%
- **🌍 Phase 2 (v0.5.0)**: Multi-platform support - Orange Pi, BeagleBone, Jetson
- **🧰 Phase 3 (v0.6.0)**: Developer tools - CLI utilities, GPIO monitor, package managers
- **🤝 Phase 4 (v0.7.0)**: Ecosystem growth - Python bindings, plugin system
- **📚 Phase 5 (Ongoing)**: World-class documentation and tutorials

**Want to influence the roadmap?**
- 🗳️ [Vote on features](https://github.com/Barbatos6669/PiPinPP/issues?q=is%3Aissue+is%3Aopen+label%3Aroadmap)
- 💡 [Propose new ideas](https://github.com/Barbatos6669/PiPinPP/issues/new?template=roadmap_feature.md)
- 🖥️ [Report platform testing](https://github.com/Barbatos6669/PiPinPP/issues/new?template=platform_support.md)

**Read the full roadmap**: [docs/ROADMAP.md](docs/ROADMAP.md)

---

## Developer Onboarding

Ready to contribute? Start with:

- [Developer Guide](docs/DEVELOPER.md) – environment setup, formatting rules, coverage targets
- [.github/CONTRIBUTING.md](.github/CONTRIBUTING.md) – branching, PR etiquette, issue workflow
- [Test Matrix](docs/BUILD.md) & [TESTING.md](docs/TESTING.md) – build knobs, hardware guidance
- `scripts/run_examples.sh` – batch-build/execute examples (`--filter`, `--execute`, `--jobs`)

The TL;DR workflow: `./build.sh --clean --debug`, make your change, run `ctest`, update docs/examples,
then open a PR referencing the relevant issue.

---

## Contributing

**We're just getting started and welcome your ideas, feedback, and contributions!**

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
