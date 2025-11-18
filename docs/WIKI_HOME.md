# Welcome to PiPin++! 🚀

**A modern C++ GPIO library for Raspberry Pi that feels like home for Arduino makers.**

---

## 🎯 What is PiPin++?

PiPin++ bridges the gap between Arduino and Raspberry Pi development. If you've ever tried to migrate an Arduino project to Raspberry Pi and felt lost in unfamiliar GPIO libraries, PiPin++ is for you!

### Two Ways to Use PiPin++:

#### 🔧 Arduino-Style (Familiar & Easy)
```cpp
#include "ArduinoCompat.hpp"

void setup() {
    pinMode(17, OUTPUT);    // Just like Arduino!
}

void loop() {
    digitalWrite(17, HIGH); // Turn LED on
    delay(1000);
    digitalWrite(17, LOW);  // Turn LED off
    delay(1000);
}
```

#### ⚡ Object-Oriented (Powerful & Flexible)
```cpp
#include "pin.hpp"

Pin led(17, PinDirection::OUTPUT);
led.write(true);   // Turn LED on
led.write(false);  // Turn LED off
```

---

## 🚀 Quick Start

### 1. Install Dependencies
```bash
sudo apt update
sudo apt install libgpiod-dev cmake build-essential
```

### 2. Clone & Build
```bash
git clone https://github.com/Barbatos6669/PiPinPP.git
cd PiPinPP
mkdir build && cd build
cmake .. && make
```

### 3. Test with LED
```bash
# Wire LED: Pin 11 (GPIO17) → 220Ω resistor → LED → Ground
sudo ./examples/example_arduino_style
```

**Success!** Your LED should be blinking! 💡

---

## 📚 Documentation

### Getting Started
- **[Installation Guide](INSTALL.md)** - Complete setup instructions (one-line install script!)
- **[Getting Started Tutorial](GETTING_STARTED.md)** - Your first LED in 10 minutes
- **[Build Instructions](BUILD.md)** - Building from source
- **[Developer Guide](DEVELOPER.md)** - Contributing to PiPin++

### API & Reference
- **[API Reference](API_REFERENCE.md)** - Complete function documentation with examples
- **[CLI Usage](CLI_USAGE.md)** - `pipinpp` command-line tool reference
- **[Pin Numbering](PIN_NUMBERING.md)** - GPIO pin mapping and physical layout
- **[Platform Support](PLATFORMS.md)** - Tested boards, OS compatibility, containers

### Tutorials (Step-by-Step)
- **[LED Blink Tutorial](tutorials/LED_BLINK.md)** - Your first GPIO project (⭐ Easy)
- **[Button Input Tutorial](tutorials/BUTTON_INPUT.md)** - Reading buttons with pull-ups (⭐ Easy)
- **[Traffic Light Tutorial](tutorials/TRAFFIC_LIGHT.md)** - Control multiple LEDs (⭐ Easy)
- **[PWM Basics Tutorial](tutorials/PWM_BASICS.md)** - Fade LEDs smoothly (⭐⭐ Medium)
- **[Interrupts 101 Tutorial](tutorials/INTERRUPTS_101.md)** - Event-driven button handling (⭐⭐ Medium)
- **[I2C Sensor Tutorial](tutorials/I2C_SENSOR.md)** - Read BMP280 temperature/pressure (⭐⭐ Medium)
- **[Multi-Threading Tutorial](tutorials/MULTI_THREADING.md)** - Concurrent GPIO tasks (⭐⭐⭐ Hard)
- **[Tutorial Index](tutorials/README.md)** - Full catalog with learning paths

### Help & Troubleshooting
- **[Troubleshooting Guide](TROUBLESHOOTING.md)** - Common issues and solutions
- **[FAQ](FAQ.md)** - Frequently asked questions
- **[Code Standards](CODE_STANDARDS.md)** - Coding style and conventions

---

## 🎯 Perfect For:

- **🔄 Arduino Migrants** - Use familiar `pinMode()`, `digitalWrite()`, `digitalRead()`
- **🎓 Students & Educators** - Learn GPIO control with clear examples
- **🛠️ Makers & Hobbyists** - Quick prototyping with reliable code
- **💻 C++ Developers** - Modern, object-oriented GPIO library

---

## 🌟 Key Features

✅ **Arduino-Compatible API** - Copy/paste your Arduino code  
✅ **Modern C++17** - RAII, smart pointers, exception handling  
✅ **Professional Build System** - CMake, pkg-config, easy integration  
✅ **Comprehensive Documentation** - Examples, tutorials, API reference  
✅ **Hardware Tested** - Real GPIO control on Raspberry Pi  
✅ **Open Source** - MIT license, community-driven  

---

## 🔧 Current Status

**Version:** 0.4.0 (Production-ready with 73.5% test coverage!)  
**Platform:** Raspberry Pi (all models with 40-pin GPIO)  
**Language:** C++17  
**License:** MIT  

### ✅ Available Features:
- **Digital I/O** - LED control, button reading with pull resistors
- **Arduino API** - `pinMode`, `digitalWrite`, `digitalRead`, `delay`, `millis`
- **Object-Oriented** - Modern `Pin` class with RAII and exceptions
- **Timing** - High-precision `millis`, `micros`, `delay`, `delayMicroseconds`
- **Interrupts** - Edge detection with `attachInterrupt`/`detachInterrupt`
- **PWM** - Software PWM (`analogWrite`) and hardware PWM support
- **I²C/Wire** - Full Arduino Wire API with auto-detection (Pi 5 support)
- **SPI** - Hardware SPI communication
- **Serial/UART** - Arduino Serial API
- **Platform Detection** - Auto-detect Pi model, GPIO chip, I2C bus
- **CLI Tool** - `pipinpp` command for testing and diagnostics
- **Math & Utilities** - `map`, `constrain`, bit manipulation, trig functions
- **Performance Tools** - GPIO profiling, timing benchmarks, CSV logging

See our [Roadmap](ROADMAP.md) for future development plans.

---

## 🤝 Get Involved

- **⭐ Star the repo** on [GitHub](https://github.com/Barbatos6669/PiPinPP)
- **🐛 Report issues** or request features
- **📖 Improve documentation** (this wiki!)
- **💻 Contribute code** - check our [CONTRIBUTING.md](../.github/CONTRIBUTING.md)

---

## 📞 Need Help?

- **📖 Check the Troubleshooting page**
- **💬 Open an issue on GitHub**

---

**Happy making! 🎉**

*PiPin++: Making Raspberry Pi feel like home for Arduino makers.*
