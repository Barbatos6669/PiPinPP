# Getting Started with PiPin++

**Welcome!** This guide will take you from zero to blinking an LED in 10 minutes. 🚀

## 📚 What You'll Learn

By the end of this guide, you'll:
- ✅ Install PiPin++ on your Raspberry Pi
- ✅ Wire up your first LED circuit
- ✅ Write and run your first GPIO program
- ✅ Understand the basics of GPIO control

## 🎯 Who Is This For?

- **Complete beginners** to Raspberry Pi GPIO
- **Arduino users** wanting to try Raspberry Pi
- **Anyone** who wants to control LEDs, buttons, and sensors with code!

**No prior experience required!** We'll explain everything step by step.

---

## Step 1: Check Your Hardware

### What You Need

**Required:**
- 🥧 Raspberry Pi (any model with GPIO pins)
- 💻 Raspberry Pi OS installed (Bookworm or Bullseye)
- 💡 1× LED (any color)
- ⚡ 1× 220Ω resistor (red-red-brown bands)
- 🔌 2× jumper wires (female-to-male)
- 🖱️ Keyboard, mouse, and monitor (or SSH access)

**Optional but helpful:**
- 🍞 Breadboard (for easier wiring)
- 📦 Resistor kit
- 🔘 Push button
- 📏 Multimeter (for testing)

### What's a GPIO Pin?

**GPIO = General Purpose Input/Output**

Your Raspberry Pi has 40 pins on the side. Some are for power, some are for ground, and 26 are **GPIO pins** that you can control with code!

```
Pin Layout (Raspberry Pi 4/5):
    3.3V [ 1]  [ 2] 5V
  GPIO 2 [ 3]  [ 4] 5V
  GPIO 3 [ 5]  [ 6] GND  ← Ground (-)
  GPIO 4 [ 7]  [ 8] GPIO 14
     GND [ 9]  [10] GPIO 15
 GPIO 17 [11]  [12] GPIO 18  ← We'll use GPIO 17!
 GPIO 27 [13]  [14] GND  ← Ground (-)
   ...and more...
```

💡 **Important:** We use **GPIO numbers**, not physical pin numbers!
- Physical Pin 11 = GPIO 17 ✓
- We use GPIO 17 in our code!

[See full pinout diagram →](PIN_NUMBERING.md)

---

## Step 2: Install PiPin++

Open a terminal on your Raspberry Pi and run:

```bash
# One-line installation (easiest!)
curl -sSL https://raw.githubusercontent.com/Barbatos6669/PiPinPP/v0.3.7/install.sh | sudo bash
```

**What this does:**
- Installs required tools (CMake, compiler)
- Installs libgpiod (GPIO library)
- Builds and installs PiPin++
- Sets up permissions so you don't need `sudo`

⏱️ **Takes about 2-3 minutes**

### Verify Installation

```bash
pkg-config --modversion pipinpp
```

✅ Should show: `0.3.7`

### Set Up Permissions (Important!)

```bash
# Add yourself to the gpio group
sudo usermod -a -G gpio $USER

# Log out and back in for this to take effect!
```

💡 **Why?** Without this, you'll need to run programs with `sudo` every time.

---

## Step 3: Wire Your LED Circuit

### Understanding the LED

LEDs have two legs:
- **Long leg** = Positive (+) = Anode
- **Short leg** = Negative (-) = Cathode = Flat side

**LEDs need a resistor!** Without it, they'll burn out. 💥

### Wiring Diagram

```
 Raspberry Pi          LED Circuit
┌─────────────┐
│             │
│  GPIO 17 ───┼────→ 220Ω Resistor ───→ LED (+) Long leg
│  (Pin 11)   │                           │
│             │                          LED
│     GND  ───┼───────────────────────────┘
│  (Pin 14)   │                          (-) Short leg
│             │
└─────────────┘
```

### Step-by-Step Wiring

**⚠️ POWER OFF your Pi while wiring!**

1. **Connect resistor to GPIO 17:**
   - Plug one end of resistor into GPIO 17 (Physical Pin 11)
   
2. **Connect LED to resistor:**
   - Connect LED's **long leg** to the other end of resistor
   
3. **Connect LED to ground:**
   - Connect LED's **short leg** to any GND pin (e.g., Physical Pin 14)

### Check Your Wiring

- ✅ LED long leg connected through resistor to GPIO 17?
- ✅ LED short leg connected to GND?
- ✅ Resistor between GPIO pin and LED? (not between LED and GND)

**Common mistakes:**
- ❌ LED backwards (won't light up)
- ❌ No resistor (LED might burn out)
- ❌ Wrong GPIO pin

---

## Step 4: Your First Program

Let's write a simple program that blinks your LED!

### Create Your Project

```bash
# Create a folder for your projects
mkdir ~/pipinpp-projects
cd ~/pipinpp-projects

# Create your first program file
nano blink.cpp
```

### Copy This Code

```cpp
#include <ArduinoCompat.hpp>

int main() {
    // Set up GPIO 17 as an output
    pinMode(17, OUTPUT);
    
    // Blink 10 times
    for (int i = 0; i < 10; i++) {
        digitalWrite(17, HIGH);  // Turn LED on
        delay(500);              // Wait 500ms (0.5 seconds)
        digitalWrite(17, LOW);   // Turn LED off
        delay(500);              // Wait 500ms
    }
    
    return 0;
}
```

**What this does:**
- Line 1: Include PiPin++ library
- Line 4: Tell GPIO 17 it's an output (for controlling LED)
- Line 7-12: Blink loop - turns LED on, waits, turns off, waits
- `HIGH` = LED on (3.3V)
- `LOW` = LED off (0V)
- `delay(500)` = wait 500 milliseconds

### Save and Exit

- Press `Ctrl+O` to save
- Press `Enter` to confirm
- Press `Ctrl+X` to exit

---

## Step 5: Compile and Run

### Compile Your Code

```bash
g++ blink.cpp $(pkg-config --cflags --libs pipinpp) -o blink
```

**What this command does:**
- `g++` = C++ compiler
- `blink.cpp` = your code file
- `$(pkg-config ...)` = tells compiler where to find PiPin++
- `-o blink` = creates executable named "blink"

### Run It!

```bash
./blink
```

### Success! 🎉

Your LED should blink 10 times!

If it doesn't work, see [Troubleshooting](#troubleshooting) below.

---

## Step 6: Make It Interactive

Let's make the LED stay on as long as you hold a button!

### Add a Button to Your Circuit

```
 Raspberry Pi          Circuit
┌─────────────┐
│             │
│  GPIO 17 ───┼────→ 220Ω ───→ LED (+) ───→ GND
│             │
│  GPIO 18 ───┼────→ Button ───→ GND
│             │
└─────────────┘
```

**Wiring:**
1. One button terminal → GPIO 18 (Physical Pin 12)
2. Other button terminal → GND
3. Keep LED circuit from before

### Update Your Code

```cpp
#include <ArduinoCompat.hpp>
#include <iostream>

int main() {
    std::cout << "Button-controlled LED demo!" << std::endl;
    std::cout << "Press button to turn LED on" << std::endl;
    
    pinMode(17, OUTPUT);           // LED
    pinMode(18, INPUT_PULLUP);     // Button with pull-up
    
    // Run forever (press Ctrl+C to stop)
    while (true) {
        int buttonState = digitalRead(18);
        
        if (buttonState == LOW) {  // Button pressed
            digitalWrite(17, HIGH);  // LED on
            std::cout << "Button pressed - LED ON" << std::endl;
        } else {                     // Button released
            digitalWrite(17, LOW);   // LED off
        }
        
        delay(50);  // Small delay to reduce CPU usage
    }
    
    return 0;
}
```

### Compile and Run

```bash
g++ blink.cpp $(pkg-config --cflags --libs pipinpp) -o button_led
./button_led
```

Press the button and watch your LED turn on! 💡

---

## 🎓 Understanding the Code

### Arduino-Style Functions

If you know Arduino, you already know PiPin++! These work exactly the same:

| Function | Purpose | Example |
|----------|---------|---------|
| `pinMode(pin, mode)` | Set pin as input/output | `pinMode(17, OUTPUT)` |
| `digitalWrite(pin, value)` | Turn pin on/off | `digitalWrite(17, HIGH)` |
| `digitalRead(pin)` | Read pin state | `int val = digitalRead(18)` |
| `delay(ms)` | Wait in milliseconds | `delay(1000)` = 1 second |

### Pin Modes

```cpp
OUTPUT       // For LEDs, motors, etc. (sending signals out)
INPUT        // For buttons, sensors (reading signals in)
INPUT_PULLUP // Button with internal pull-up resistor
```

### Values

```cpp
HIGH = true = 1 = 3.3V = LED ON
LOW  = false = 0 = 0V = LED OFF
```

---

## 🐛 Troubleshooting

### LED Doesn't Light Up

**Check wiring:**
- ✅ LED in correct direction? (long leg to resistor)
- ✅ Resistor connected?
- ✅ Ground connection secure?

**Check code:**
```bash
# Did it compile without errors?
g++ blink.cpp $(pkg-config --cflags --libs pipinpp) -o blink

# Any error messages when running?
./blink
```

**Test LED manually:**
```bash
# Turn GPIO 17 on
gpioset gpiochip0 17=1

# Turn GPIO 17 off  
gpioset gpiochip0 17=0
```

### Permission Denied

```
Error: Failed to open /dev/gpiochip0: Permission denied
```

**Solution 1: Add yourself to gpio group**
```bash
sudo usermod -a -G gpio $USER
# Log out and back in!
```

**Solution 2: Run with sudo (temporary)**
```bash
sudo ./blink
```

### LED Very Dim or Flickering

**Problem:** Might be using wrong resistor value
- Too much resistance (10kΩ) = very dim
- Too little resistance (no resistor) = might damage LED

**Solution:** Use 220Ω to 1kΩ resistor (ideal range)

### Wrong Pin Numbers

**Remember:**
- 🎯 Use **GPIO numbers** in code, not physical pin numbers!
- Physical Pin 11 = GPIO 17
- [See full pinout →](PIN_NUMBERING.md)

### Compilation Errors

**"pipinpp not found":**
```bash
# Check if installed
pkg-config --modversion pipinpp

# If not found, reinstall
curl -sSL https://raw.githubusercontent.com/Barbatos6669/PiPinPP/v0.3.7/install.sh | sudo bash
```

**"ArduinoCompat.hpp not found":**
```bash
# Check installation
ls /usr/local/include/ArduinoCompat.hpp

# If missing, reinstall PiPin++
```

---

## 🎯 What's Next?

Congratulations! You've:
- ✅ Installed PiPin++
- ✅ Wired your first LED circuit
- ✅ Written and run GPIO code
- ✅ Controlled an LED with a button

### Next Steps

**Beginner Tutorials:**
1. [Multiple LEDs Tutorial](tutorials/MULTIPLE_LEDS.md) - Control several LEDs
2. [Traffic Light Tutorial](tutorials/TRAFFIC_LIGHT.md) - Build a traffic light
3. [Button Patterns Tutorial](tutorials/BUTTON_PATTERNS.md) - Create LED patterns

**Example Programs:**
- [examples/basic_led/](../examples/basic_led/) - More LED patterns
- [examples/button_input/](../examples/button_input/) - Button debouncing
- [examples/led_fade/](../examples/led_fade/) - Fade LEDs with PWM

**Advanced Features:**
- [PWM (Pulse Width Modulation)](API_REFERENCE.md#pwm) - Fade LEDs smoothly
- [Interrupts](API_REFERENCE.md#interrupts) - Respond to button presses instantly
- [Timing Functions](API_REFERENCE.md#timing) - Precise millisecond timing

**Documentation:**
- 📖 [Complete API Reference](API_REFERENCE.md) - All functions explained
- 📌 [Pin Numbering Guide](PIN_NUMBERING.md) - Full GPIO pinout
- 🔐 [Permissions Guide](PERMISSIONS.md) - GPIO access without sudo

---

## 💬 Need Help?

**Got stuck?**
- 💬 [Join our Discord](https://discord.gg/wXeZP8Ev) - Quick community help
- 🐛 [GitHub Issues](https://github.com/Barbatos6669/PiPinPP/issues) - Report bugs
- 📖 [Full Documentation](https://barbatos6669.github.io/PiPinPP/) - Complete reference

**Want to learn more?**
- Check out our [tutorials directory](tutorials/)
- Browse more [examples](../examples/)
- Read the [API Reference](API_REFERENCE.md)

---

## 🎉 You Did It!

You're now a GPIO programmer! 🚀

Share your projects with the community and help other beginners get started!

**Happy making!** 💡

---

*Last updated: November 5, 2025*
