# Frequently Asked Questions (FAQ)

Quick answers to common questions about PiPin++.

---

## 🚀 Getting Started

### Q: Do I need any prior programming experience?

**A:** Basic C/C++ knowledge helps, but our tutorials assume you're a complete beginner! If you've done any Arduino programming, you'll feel right at home.

**Start here:**
- [Getting Started Guide](GETTING_STARTED.md) - Step-by-step tutorial
- [examples/basic_led/](../examples/basic_led/) - Simple working code

---

### Q: What Raspberry Pi models are supported?

**A:** PiPin++ works on any Raspberry Pi with 40-pin GPIO header:
- ✅ Raspberry Pi 5 (recommended)
- ✅ Raspberry Pi 4
- ✅ Raspberry Pi 3 (B, B+, A+)
- ✅ Raspberry Pi 2
- ✅ Raspberry Pi Zero (W, 2W)
- ✅ Raspberry Pi 400

**Not supported:**
- ❌ Raspberry Pi 1 Model A/B (26-pin header)
- ❌ Raspberry Pi Pico (different architecture - use Arduino IDE)

---

### Q: Which OS should I use?

**A:** We recommend **Raspberry Pi OS** (formerly Raspbian):
- ✅ Raspberry Pi OS Bookworm (Debian 12) - Latest, best support
- ✅ Raspberry Pi OS Bullseye (Debian 11) - Stable
- ✅ Ubuntu 22.04+ for Raspberry Pi

**Requirements:**
- libgpiod 2.0+ (included in recent distributions)
- C++17 compatible compiler (GCC 7+)

---

### Q: How is PiPin++ different from other GPIO libraries?

**A:** Great question! Here's how we compare:

| Feature | PiPin++ | WiringPi | pigpio | lgpio |
|---------|---------|----------|--------|-------|
| **Arduino-inspired API** | ✅ Yes | ⚠️ Similar | ❌ No | ❌ No |
| **Modern C++17** | ✅ Yes | ❌ C-style | ❌ C | ❌ C |
| **Active Development** | ✅ Yes | ❌ Deprecated | ✅ Yes | ✅ Yes |
| **Thread-Safe** | ✅ Yes | ⚠️ Partial | ✅ Yes | ✅ Yes |
| **Uses libgpiod v2** | ✅ Yes | ❌ No | ❌ No | ✅ Yes |
| **Exception Handling** | ✅ Yes | ❌ No | ❌ No | ❌ No |
| **Beginner Friendly** | ✅ Very | ⚠️ Medium | ❌ Advanced | ❌ Advanced |

**PiPin++ advantages:**
- Familiar to Arduino users
- Modern, type-safe C++
- Comprehensive documentation
- Active community support

---

## 🔧 Installation & Setup

### Q: Do I need to compile from source?

**A:** No! Use our one-line installer:

```bash
curl -sSL https://raw.githubusercontent.com/Barbatos6669/PiPinPP/v0.3.7/install.sh | sudo bash
```

This installs everything automatically. If you prefer to compile manually, see [INSTALL.md](INSTALL.md).

---

### Q: Can I use PiPin++ without sudo?

**A:** Yes! After installation, add yourself to the `gpio` group:

```bash
sudo usermod -a -G gpio $USER
```

**Important:** Log out and back in for this to take effect!

Alternatively, see [PERMISSIONS.md](PERMISSIONS.md) for detailed setup.

---

### Q: How do I uninstall PiPin++?

**A:** To remove PiPin++:

```bash
# Remove installed files
sudo rm -rf /usr/local/include/pipinpp
sudo rm -rf /usr/local/include/*.hpp
sudo rm -f /usr/local/lib/libpipinpp.*
sudo rm -f /usr/local/lib/pkgconfig/pipinpp.pc
sudo ldconfig

# Remove source (optional)
rm -rf ~/PiPinPP
```

---

## 💻 Programming

### Q: Can I use PiPin++ with Python?

**A:** Not directly - PiPin++ is a C++ library. However:

**For Python users**, we recommend:
- **gpiozero** - Beginner-friendly Python GPIO
- **RPi.GPIO** - Traditional Python GPIO library

**Why use PiPin++?**
- Faster performance (compiled C++)
- Better for real-time applications
- Ideal if you're learning C++ or migrating from Arduino

---

### Q: How do I use PiPin++ in my own project?

**A:** After installation, it's easy!

**Method 1: pkg-config (recommended)**
```bash
g++ myproject.cpp $(pkg-config --cflags --libs pipinpp) -o myproject
```

**Method 2: CMake**
```cmake
find_package(PiPinPP REQUIRED)
target_link_libraries(myproject PiPinPP::pipinpp)
```

See [Build Guide](BUILD.md) for details.

---

### Q: Can I use setup() and loop() like Arduino?

**A:** Yes! Check out [arduino_style example](../examples/arduino_style/):

```cpp
#include <ArduinoCompat.hpp>

void setup() {
    pinMode(17, OUTPUT);
}

void loop() {
    digitalWrite(17, HIGH);
    delay(1000);
    digitalWrite(17, LOW);
    delay(1000);
}

int main() {
    setup();
    while (true) {
        loop();
    }
    return 0;
}
```

---

### Q: What's the difference between Pin class and Arduino-style functions?

**A:** Two ways to use PiPin++:

**Arduino-style (easy, familiar):**
```cpp
#include <ArduinoCompat.hpp>

pinMode(17, OUTPUT);
digitalWrite(17, HIGH);
```

**Object-oriented (powerful, flexible):**
```cpp
#include "pin.hpp"

Pin led(17, PinDirection::OUTPUT);
led.write(true);
```

Use whichever feels more comfortable! Many users start with Arduino-style and transition to object-oriented as projects grow.

---

## 🔌 Hardware

### Q: Why isn't my LED working?

**Common issues:**

1. **LED backwards?** 
   - Long leg = (+) positive
   - Short leg = (-) negative

2. **No resistor?** 
   - LEDs need 220Ω - 1kΩ resistor
   - Without it, LED may burn out

3. **Wrong pin number?**
   - Use GPIO numbers, not physical pins
   - GPIO 17 = Physical Pin 11

4. **Bad connection?**
   - Check all wires are secure
   - Test with multimeter if available

See [Troubleshooting Guide](TROUBLESHOOTING.md) for more help.

---

### Q: Which GPIO pins can I safely use?

**Safe for general use:**
- GPIO 2, 3, 4, 17, 27, 22, 10, 9, 11
- GPIO 14, 15 (but avoid if using serial)
- GPIO 23, 24, 25, 8, 7

**Avoid these pins:**
- GPIO 0, 1 (reserved for HAT EEPROM)
- GPIO 14, 15 (UART - if using serial console)
- GPIO 18, 19, 20, 21 (if using I2C/SPI)

See [Pin Numbering Guide](PIN_NUMBERING.md) for complete reference.

---

### Q: Can I control motors with PiPin++?

**A:** Small motors, yes! But:

**⚠️ Never connect motors directly to GPIO pins!**

Use a motor driver board:
- **L293D** - For small DC motors
- **L298N** - For larger DC motors  
- **TB6612FNG** - Modern, efficient
- **Servo controller** - For servo motors

**Why?** GPIO pins provide only 3.3V at tiny current. Motors need more power and can damage your Pi!

**Example with motor driver:**
```cpp
// Control motor through driver
pinMode(17, OUTPUT);  // Motor direction
pinMode(27, OUTPUT);  // Motor enable

digitalWrite(17, HIGH);  // Forward
digitalWrite(27, HIGH);  // Turn on
```

---

### Q: What voltage do GPIO pins output?

**A:** Raspberry Pi GPIO pins are **3.3V**, not 5V like Arduino!

**Important:**
- ✅ 3.3V output when HIGH
- ✅ 0V output when LOW  
- ⚠️ Input tolerance: 0-3.3V ONLY
- ❌ **Never connect 5V to GPIO pins!** (Will damage Pi)

**Using 5V devices:**
- Use a level shifter (3.3V ↔ 5V converter)
- Or use a voltage divider for input only
- Check if device is 3.3V compatible

---

## ⚡ Performance

### Q: How fast can I toggle GPIO pins?

**A:** PiPin++ can toggle pins at:
- ~1 MHz (1 million toggles/second) on Pi 4/5
- Limited by Linux scheduling, not the library

**For very high speed:**
- Consider using PWM hardware (up to 10 MHz)
- Or a dedicated microcontroller (Pico, Arduino)

---

### Q: Can I use PiPin++ in real-time systems?

**A:** Raspberry Pi OS is not a real-time OS, so timing is not guaranteed.

**Good enough for:**
- ✅ LED control
- ✅ Button reading
- ✅ Sensor polling
- ✅ Motor control
- ✅ Most hobby projects

**Not suitable for:**
- ❌ Microsecond-precise timing
- ❌ WS2812 LED strips (needs precise timing)
- ❌ Bit-banging protocols (use hardware SPI/I2C instead)

**For hard real-time:** Use a microcontroller (Arduino, Pico) alongside your Pi.

---

## 🐛 Troubleshooting

### Q: "Permission denied" error when running my program

**Solution:**

```bash
# Add yourself to gpio group
sudo usermod -a -G gpio $USER

# Log out and back in!
# Or run with sudo as temporary workaround:
sudo ./myprogram
```

See [Permissions Guide](PERMISSIONS.md) for complete setup.

---

### Q: "Failed to open /dev/gpiochip0"

**Check if GPIO is enabled:**

```bash
# Should show gpiochip0
ls /dev/gpiochip*

# If missing, enable GPIO:
sudo raspi-config
# → Interface Options → GPIO → Enable
```

---

### Q: My program compiles but doesn't run

**Common issues:**

1. **Missing sudo/permissions:**
   ```bash
   sudo ./myprogram
   ```

2. **Pin already in use:**
   ```bash
   # Find and kill other GPIO programs
   sudo killall myprogram
   ```

3. **Wrong pin number:**
   - Check you're using GPIO numbers, not physical pins

See [Troubleshooting Guide](TROUBLESHOOTING.md) for complete diagnostics.

---

## 📚 Learning

### Q: I'm completely new to programming. Where do I start?

**Learning path for absolute beginners:**

1. **Learn C++ basics** (1-2 weeks)
   - Variables, loops, functions
   - [learncpp.com](https://www.learncpp.com/) (free)
   
2. **Try our Getting Started guide** (1 hour)
   - [docs/GETTING_STARTED.md](GETTING_STARTED.md)
   
3. **Build tutorial projects** (1-2 weeks)
   - [docs/tutorials/](tutorials/) - Traffic Light, Buttons, etc.
   
4. **Study our examples** (ongoing)
   - [examples/](../examples/) - Real working code

**Don't give up!** Everyone starts as a beginner. Our Discord community is here to help!

---

### Q: Best resources for learning C++ and GPIO?

**C++ Learning:**
- [learncpp.com](https://www.learncpp.com/) - Free, comprehensive
- [cplusplus.com](http://www.cplusplus.com/) - Reference
- [cppreference.com](https://en.cppreference.com/) - Advanced reference

**GPIO & Electronics:**
- [Our tutorials](tutorials/) - Raspberry Pi specific
- [Adafruit Learn](https://learn.adafruit.com/) - Great electronics tutorials
- [SparkFun Tutorials](https://learn.sparkfun.com/) - Hardware guides

**Video Learning:**
- Search YouTube for "Raspberry Pi GPIO tutorials"
- Look for "C++ for beginners" courses

---

## 🤝 Community

### Q: How can I get help?

**Quick help:**
- 💬 [Discord Server](https://discord.gg/wXeZP8Ev) - Community chat, fastest response

**Bug reports:**
- 🐛 [GitHub Issues](https://github.com/Barbatos6669/PiPinPP/issues) - Report bugs

**General discussion:**
- 💭 [GitHub Discussions](https://github.com/Barbatos6669/PiPinPP/discussions) - Feature requests, ideas

**Before asking:**
1. Check [Troubleshooting Guide](TROUBLESHOOTING.md)
2. Search existing issues/discussions
3. Try to create a minimal example
4. Include error messages and system info

---

### Q: How can I contribute?

**We love contributions!**

**Easy ways to help:**
- 📝 Improve documentation
- 🐛 Report bugs with good details
- 💡 Suggest features
- ⭐ Star the repo on GitHub
- 📢 Share with friends

**Code contributions:**
- Fix bugs
- Add examples
- Write tutorials
- Improve tests

See [CONTRIBUTING.md](../.github/CONTRIBUTING.md) for guidelines.

---

### Q: Is PiPin++ production-ready?

**A:** Yes! Version 0.3.7+ is stable and production-ready for most applications.

**Current status:**
- ✅ Digital I/O - Fully tested
- ✅ Timing functions - Stable
- ✅ Interrupts - Production-ready
- ✅ PWM - Software PWM stable
- ✅ I2C/Wire - Full Arduino Wire library compatibility
- ⚠️ SPI/UART - Coming in v0.4.0

**Used in production?** Let us know! We'd love to feature your project.

---

## 📦 Future Features

### Q: When will I2C/SPI support be added?

**A:** Planned for v0.4.0 (Q1 2026)

**Current roadmap:**
- v0.3.7 - SPI support, comprehensive documentation (current)
- v0.4.0 - Hardware PWM, UART support
- v0.5.0 - Advanced peripherals, analog input
- v1.0.0 - Full feature completion

See [ROADMAP.md](planning/ROADMAP.md) for details.

---

### Q: Will you support other boards (Orange Pi, etc.)?

**A:** Yes! Planned for v0.5.0+

PiPin++ uses libgpiod, which supports many ARM SBCs. We'll add specific testing and documentation for popular boards.

---

### Q: Can I request a feature?

**A:** Absolutely! 

**Feature requests welcome:**
- Open a [GitHub Discussion](https://github.com/Barbatos6669/PiPinPP/discussions)
- Or ask on [Discord](https://discord.gg/wXeZP8Ev)
- Check [ROADMAP.md](planning/ROADMAP.md) to see if it's already planned

**We prioritize features by:**
1. Community votes/interest
2. Arduino API compatibility
3. Educational value
4. Implementation difficulty

---

## 🔒 Security

### Q: Is it safe to run GPIO code with sudo?

**A:** It works, but it's better to use proper permissions.

**Why avoid sudo:**
- Security risk (program runs as root)
- Bad practice for production
- Harder to test/debug

**Better approach:**
```bash
# One-time setup
sudo usermod -a -G gpio $USER
# Log out and back in

# Then run without sudo
./myprogram
```

See [PERMISSIONS.md](PERMISSIONS.md) for complete guide.

---

### Q: Can GPIO access damage my Raspberry Pi?

**A:** Physical damage is possible if you're not careful!

**Safe practices:**
- ✅ Use current-limiting resistors
- ✅ Never exceed 3.3V on input pins
- ✅ Don't draw more than 16mA per pin
- ✅ Use drivers for motors/high current loads

**Dangerous things:**
- ❌ Connecting 5V directly to GPIO pins
- ❌ Shorting pins to ground
- ❌ Drawing excessive current
- ❌ Connecting motors directly

**Good news:** PiPin++ can't damage your Pi by itself - only incorrect wiring can!

---

## 💡 Still Have Questions?

**Didn't find your answer?**

- 💬 Ask on [Discord](https://discord.gg/wXeZP8Ev)
- 🐛 Open a [GitHub Issue](https://github.com/Barbatos6669/PiPinPP/issues)
- 💭 Start a [Discussion](https://github.com/Barbatos6669/PiPinPP/discussions)
- 📖 Check [Full Documentation](https://barbatos6669.github.io/PiPinPP/)

We're here to help! 🚀

---

*Last updated: November 5, 2025*
