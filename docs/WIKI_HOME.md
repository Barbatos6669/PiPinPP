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

| Page | Description |
|------|-------------|
| **Installation Guide** | Complete setup instructions |
| **Arduino Migration** | Switch from Arduino to Pi easily |
| **API Reference** | Complete function documentation |
| **Hardware Setup** | GPIO pinouts and wiring |
| **Examples & Tutorials** | Step-by-step projects |
| **Troubleshooting** | Common issues and solutions |

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

**Version:** 0.2.0 (Complete Arduino-inspired API with pull-resistor support!)  
**Platform:** Raspberry Pi (all models with 40-pin GPIO)  
**Language:** C++17  
**License:** MIT  

### ✅ Working Features:
- Digital output (LED control)
- Digital input (button reading)
- Arduino-style functions
- Object-oriented `Pin` class
- CMake build system

### 🚧 Coming Soon:
- PWM support (`analogWrite()`)
- Interrupt handling
- I2C/SPI communication
- Pull-up/pull-down resistors

See our [ROADMAP.md](../ROADMAP.md) for detailed development plans.

---

## 🤝 Get Involved

- **⭐ Star the repo** on [GitHub](https://github.com/Barbatos6669/PiPinPP)
- **🐛 Report issues** or request features
- **📖 Improve documentation** (this wiki!)
- **💻 Contribute code** - check our [CONTRIBUTING.md](../CONTRIBUTING.md)

---

## 📞 Need Help?

- **📖 Check the Troubleshooting page**
- **💬 Open an issue on GitHub**

---

**Happy making! 🎉**

*PiPin++: Making Raspberry Pi feel like home for Arduino makers.*
