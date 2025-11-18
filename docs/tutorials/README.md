# PiPin++ Tutorials

Step-by-step project tutorials for learning GPIO control with PiPin++!

## 📚 Tutorial Index

### Beginner Tutorials

Start here if you're new to Raspberry Pi GPIO!

| Tutorial | What You'll Learn | Time | Difficulty |
|----------|-------------------|------|------------|
| **[Getting Started](../GETTING_STARTED.md)** | Install PiPin++, blink your first LED | 10 min | ⭐ Easy |
| **[Controlling an LED](LED_BLINK.md)** | Wire + blink a single LED with PiPin++ | 10 min | ⭐ Easy |
| **[Button Input](BUTTON_INPUT.md)** | Read push buttons with pull-ups | 10 min | ⭐ Easy |
| **[Traffic Light](TRAFFIC_LIGHT.md)** | Control multiple LEDs, create sequences | 15 min | ⭐ Easy |

### Intermediate Tutorials

Ready for more complex projects?

| Tutorial | What You'll Learn | Time | Difficulty |
|----------|-------------------|------|------------|
| **[PWM Basics](PWM_BASICS.md)** | Fade LEDs smoothly with PWM | 20 min | ⭐⭐ Medium |
| **[Interrupts 101](INTERRUPTS_101.md)** | Use attachInterrupt for buttons | 25 min | ⭐⭐ Medium |
| **[I2C Sensor](I2C_SENSOR.md)** | Read sensors via Wire/BMP280 | 20 min | ⭐⭐ Medium |
| **Coming Soon: SPI Devices** | Control high-speed peripherals | 30 min | ⭐⭐ Medium |

### Advanced Tutorials

Push your skills to the limit!

| Tutorial | What You'll Learn | Time | Difficulty |
|----------|-------------------|------|------------|
| **[Multi-Threading](MULTI_THREADING.md)** | Coordinate multiple GPIO tasks with threads | 30 min | ⭐⭐⭐ Hard |
| **I2C Communication** *(coming soon)* | Talk to sensors and displays | 30 min | ⭐⭐⭐ Hard |
| **Custom Protocols** *(coming soon)* | Create your own communication methods | 40 min | ⭐⭐⭐ Hard |

---

## 🎯 Learning Paths

### Path 1: Arduino Migration

Coming from Arduino? Follow this path:

1. **[Getting Started](../GETTING_STARTED.md)** - Install and basic setup
2. **[Arduino Migration Example](../../examples/arduino_migration/)** - See the similarities
3. **[Arduino Style Example](../../examples/arduino_style/)** - Familiar `setup()` and `loop()`
4. **[Traffic Light](TRAFFIC_LIGHT.md)** - Multi-LED control

### Path 2: Complete Beginner

Never programmed hardware before? Start here:

1. **[Getting Started](../GETTING_STARTED.md)** - Your first LED
2. **[Traffic Light](TRAFFIC_LIGHT.md)** - Multiple LEDs
3. **Button Input** *(coming soon)* - Reading sensors
4. **PWM Basics** *(coming soon)* - Analog-like control

### Path 3: GPIO Pro

Want to master everything?

1. Complete all Beginner tutorials
2. Complete all Intermediate tutorials  
3. Study [API Reference](../API_REFERENCE.md)
4. Build a complex project (ideas below)

---

## 🛠️ What You'll Need

### Essential Hardware

For most tutorials you'll need:
- 🥧 Raspberry Pi (any model with GPIO)
- 💡 LEDs (red, yellow, green)
- ⚡ 220Ω resistors
- 🔌 Jumper wires (male-to-female)
- 🔘 Push buttons

### Recommended Kit

Consider getting a beginner electronics kit with:
- Variety of LEDs (colors and sizes)
- Resistor assortment (100Ω - 10kΩ)
- Breadboard
- Jumper wire set
- Push buttons
- Potentiometers
- Basic sensors (temperature, light, motion)

**Budget:** $15-30 for a good starter kit

### Software Setup

Follow [Installation Guide](../INSTALL.md) to set up PiPin++

---

## 💡 Project Ideas

After completing tutorials, try these projects:

### Beginner Projects

- 🎲 **Electronic Dice** - Roll with button press
- 🎵 **Light Organ** - LEDs react to music (with sound sensor)
- ⏰ **Digital Timer** - Countdown with LED display
- 🌡️ **Temperature Monitor** - Show temp with LED colors
- 🚨 **Alarm System** - Motion detector with buzzer

### Intermediate Projects

- 🤖 **Line-Following Robot** - Follow black line on floor
- 🎮 **Game Controller** - Custom controller for PC games
- 📊 **Weather Station** - Display temp, humidity, pressure
- 💡 **Smart Lights** - Voice or app-controlled LEDs
- 🔐 **Keypad Lock** - Secure access with code

### Advanced Projects

- 🏠 **Home Automation** - Control lights, fans, appliances
- 🚗 **RC Car** - Build and control your own car
- 🎹 **MIDI Controller** - Play music with buttons
- 📷 **Camera Trigger** - Time-lapse or motion-activated photos
- 🤝 **Multi-Pi Network** - Communicate between multiple Pis

---

## 📖 Tutorial Format

Each tutorial includes:

### 📋 Header
- What you'll build
- What you'll learn
- Time estimate
- Difficulty level
- Parts list

### 🔧 Step-by-Step Guide
- Clear instructions
- Circuit diagrams
- Code with explanations
- Expected output

### 💡 Challenges
- Easy modifications
- Medium enhancements
- Hard extensions

### 🐛 Troubleshooting
- Common issues
- Debug tips
- Testing procedures

### 🎓 Learning Outcomes
- Skills practiced
- Concepts explained
- Next steps

---

## 🤝 Contributing Tutorials

Want to write a tutorial? We'd love your help!

**Good tutorial topics:**
- Real-world projects you've built
- Common beginner questions
- Arduino project migrations
- Cool sensor integrations
- Creative LED patterns

**Tutorial Guidelines:**
- Clear, step-by-step instructions
- Tested code that works
- Circuit diagrams (text art or images)
- Difficulty rating
- Parts list with prices
- Troubleshooting section

See [CONTRIBUTING.md](../../.github/CONTRIBUTING.md) for details.

---

## 📚 Additional Resources

### Documentation
- [API Reference](../API_REFERENCE.md) - Complete function list
- [Pin Numbering](../PIN_NUMBERING.md) - GPIO pinout guide
- [Troubleshooting](../TROUBLESHOOTING.md) - Fix common issues

### Examples
- [Basic Examples](../../examples/) - Simple working code
- [Advanced Examples](../../examples/) - Complex projects

### Community
- 💬 [Discord](https://discord.gg/wXeZP8Ev) - Ask questions, share projects
- 🐛 [GitHub Issues](https://github.com/Barbatos6669/PiPinPP/issues) - Report bugs
- 📖 [GitHub Discussions](https://github.com/Barbatos6669/PiPinPP/discussions) - General discussion

---

## ✨ Tutorial Requests

Want a tutorial on something specific? Let us know!

**Popular requests:**
- [ ] PWM LED Fading
- [ ] Button Debouncing
- [ ] Interrupt Handling
- [ ] I2C OLED Display
- [ ] SPI LED Matrix
- [ ] Servo Motor Control
- [ ] Ultrasonic Distance Sensor
- [ ] Temperature & Humidity Sensor

Vote or add requests in [GitHub Discussions](https://github.com/Barbatos6669/PiPinPP/discussions)!

---

## 🎉 Share Your Success!

Completed a tutorial? We want to see!

- Share photos/videos on Discord
- Post your code modifications
- Help other beginners
- Inspire the community

**Happy learning!** 🚀

---

*Last updated: November 5, 2025*
