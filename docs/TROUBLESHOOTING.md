# Troubleshooting Guide

Having problems with PiPin++? This guide covers common issues and their solutions.

## 📋 Quick Diagnostics

Run these commands to check your system:

```bash
# Check PiPin++ installation
pkg-config --modversion pipinpp

# Check GPIO access
ls -la /dev/gpiochip*

# Check your groups
groups $USER

# Test GPIO detection
gpiodetect
```

---

## 🔴 Installation Issues

### "Command not found: pkg-config"

**Problem:** Build tools not installed

**Solution:**
```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config git
```

### "libgpiod not found"

**Problem:** GPIO library not installed

**Solution:**
```bash
sudo apt update
sudo apt install -y libgpiod-dev gpiod
```

**Verify:**
```bash
pkg-config --libs libgpiod
# Should output: -lgpiod
```

### "curl: command not found"

**Problem:** curl not installed

**Solution:**
```bash
sudo apt install curl
# Then retry installation
```

### Installation Script Fails

**Problem:** Network or permissions issue

**Solution: Manual installation**
```bash
git clone https://github.com/Barbatos6669/PiPinPP.git
cd PiPinPP
git checkout v0.3.7
./build.sh
cd build
sudo make install
sudo ldconfig
```

---

## 🔒 Permission Errors

### "Permission denied" accessing GPIO

```
Error: Failed to open /dev/gpiochip0: Permission denied
```

**Root cause:** Your user doesn't have GPIO permissions

**Solution 1: Add to gpio group (permanent)**
```bash
# Add yourself to gpio group
sudo usermod -a -G gpio $USER

# Verify it worked
groups $USER | grep gpio

# ⚠️ IMPORTANT: Log out and back in!
```

**Solution 2: Check udev rules**
```bash
# Check if udev rule exists
cat /etc/udev/rules.d/60-gpiochip.rules

# If it doesn't exist, create it:
echo 'SUBSYSTEM=="gpio", GROUP="gpio", MODE="0660"' | sudo tee /etc/udev/rules.d/60-gpiochip.rules

# Reload udev
sudo udevadm control --reload-rules
sudo udevadm trigger
```

**Solution 3: Temporary workaround (testing only)**
```bash
# Run with sudo (not recommended for production)
sudo ./your_program
```

### "Unable to open chip /dev/gpiochip0"

**Check if GPIO device exists:**
```bash
ls -la /dev/gpiochip*
```

**If missing:**
```bash
# Check if GPIO is enabled
sudo raspi-config
# → Interface Options → Enable GPIO

# Or edit config directly
sudo nano /boot/config.txt
# Add: dtparam=gpio=on
sudo reboot
```

---

## ⚡ Hardware Issues

### LED Doesn't Light Up

#### Check LED Direction
LEDs only work in one direction!
- **Long leg** = Positive (+) → Resistor → GPIO pin
- **Short leg** = Negative (-) → Ground

**Test:** Flip the LED around and try again

#### Check Resistor Value
```bash
# Typical resistor values for LEDs:
220Ω - 1kΩ → Good ✓
10kΩ+ → Too dim
No resistor → LED might burn out! ⚠️
```

**Resistor Color Codes:**
- 220Ω: Red-Red-Brown
- 330Ω: Orange-Orange-Brown
- 1kΩ: Brown-Black-Red

#### Test GPIO Pin Manually
```bash
# Export GPIO pin
echo 17 > /sys/class/gpio/export

# Set as output
echo out > /sys/class/gpio/gpio17/direction

# Turn on
echo 1 > /sys/class/gpio/gpio17/value

# Turn off
echo 0 > /sys/class/gpio/gpio17/value

# Cleanup
echo 17 > /sys/class/gpio/unexport
```

**Or use gpioset:**
```bash
gpioset gpiochip0 17=1  # Turn on
gpioset gpiochip0 17=0  # Turn off
```

#### Check Connections
```
Verify this circuit:
GPIO Pin → Resistor → LED (+) long leg → LED (-) short leg → Ground
```

### Button Not Working

#### Check Pull-up/Pull-down
Buttons need a pull-up or pull-down resistor!

**Software pull-up (easiest):**
```cpp
pinMode(18, INPUT_PULLUP);  // Built-in pull-up
// Button pressed = LOW
// Button released = HIGH
```

**External pull-up resistor:**
```
3.3V → 10kΩ resistor → GPIO pin → Button → Ground
```

#### Check Button Reading
```bash
# Read GPIO 18
gpioget gpiochip0 18

# Should be:
# 1 = button not pressed
# 0 = button pressed (if using pull-up)
```

#### Debouncing Issues
Buttons can "bounce" (multiple triggers)

**Software solution:**
```cpp
int lastState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;  // milliseconds

int buttonState = digitalRead(18);

if (buttonState != lastState) {
    lastDebounceTime = millis();
}

if ((millis() - lastDebounceTime) > debounceDelay) {
    // Button state is stable
}

lastState = buttonState;
```

---

## 💻 Compilation Errors

### "ArduinoCompat.hpp: No such file or directory"

**Problem:** Compiler can't find PiPin++ headers

**Check installation:**
```bash
ls /usr/local/include/ArduinoCompat.hpp
```

**If missing, reinstall:**
```bash
cd PiPinPP
./build.sh
cd build
sudo make install
```

**Fix compilation:**
```bash
# Make sure to use pkg-config
g++ main.cpp $(pkg-config --cflags --libs pipinpp) -o main

# Or specify include path manually
g++ -I/usr/local/include main.cpp -lpipinpp -lgpiod -o main
```

### "undefined reference to pinMode"

**Problem:** Not linking against PiPin++ library

**Solution:**
```bash
# Correct way (with pkg-config)
g++ main.cpp $(pkg-config --cflags --libs pipinpp) -o main

# Or link manually
g++ main.cpp -lpipinpp -lgpiod -o main
```

### "error: 'millis' was not declared"

**Problem:** Missing include

**Solution:**
```cpp
#include <ArduinoCompat.hpp>  // For Arduino-style functions
```

### C++ Standard Version Error

```
error: 'this_thread' is not a member of 'std'
```

**Problem:** Need C++11 or higher

**Solution:**
```bash
g++ -std=c++11 main.cpp $(pkg-config --cflags --libs pipinpp) -o main
```

---

## 🔧 Runtime Errors

### "Chip or line busy"

```
Error: Chip or line busy
```

**Problem:** GPIO pin already in use by another program

**Solution:**
```bash
# Find processes using GPIO
sudo lsof | grep gpiochip

# Kill the process
sudo killall your_program_name

# Or reboot to clean up
sudo reboot
```

### "Line offset out of range"

```
Error: Line offset 99 is out of range
```

**Problem:** Invalid GPIO pin number

**Common mistakes:**
- Using physical pin numbers instead of GPIO numbers
- Pin 11 ≠ GPIO 11 (it's GPIO 17!)

**Solution:** Check [Pin Numbering Guide](PIN_NUMBERING.md)

```cpp
// ❌ Wrong (physical pin number)
pinMode(11, OUTPUT);

// ✓ Correct (GPIO number)
pinMode(17, OUTPUT);
```

### Segmentation Fault

```
Segmentation fault (core dumped)
```

**Common causes:**
1. Accessing invalid pin number
2. Using pin after it's been released
3. Double-freeing a pin

**Debug with gdb:**
```bash
# Compile with debug symbols
g++ -g main.cpp $(pkg-config --cflags --libs pipinpp) -o main

# Run with debugger
gdb ./main
(gdb) run
(gdb) bt  # Backtrace when it crashes
```

---

## 🎯 Pin Number Confusion

### Understanding Pin Numbering

**There are 3 numbering systems!**

| Physical Pin | GPIO Number | BCM Number |
|--------------|-------------|------------|
| Pin 11 | GPIO 17 | BCM 17 |
| Pin 12 | GPIO 18 | BCM 18 |
| Pin 13 | GPIO 27 | BCM 27 |

**PiPin++ uses GPIO/BCM numbers** (same as Arduino numbering)

### Quick Reference

```cpp
// ✓ Use GPIO numbers in your code
pinMode(17, OUTPUT);    // GPIO 17 (Physical Pin 11)
pinMode(18, INPUT);     // GPIO 18 (Physical Pin 12)

// ❌ Don't use physical pin numbers
pinMode(11, OUTPUT);    // This is GPIO 11, not Pin 11!
```

[See full pinout →](PIN_NUMBERING.md)

---

## 🌡️ Performance Issues

### Program Using 100% CPU

**Problem:** Tight loop with no delay

**Bad code:**
```cpp
while (true) {
    int state = digitalRead(18);  // No delay!
}
```

**Good code:**
```cpp
while (true) {
    int state = digitalRead(18);
    delay(10);  // Small delay reduces CPU usage
}
```

### Slow Response to Button Presses

**Problem:** Using `delay()` in main loop

**Solution: Use millis() instead**
```cpp
unsigned long previousMillis = 0;
const long interval = 1000;

while (true) {
    unsigned long currentMillis = millis();
    
    // Check button immediately
    int buttonState = digitalRead(18);
    
    // Do timed action without blocking
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        // Do something every second
    }
}
```

---

## 🔄 Updating Issues

### "Already installed" but old version

```bash
pkg-config --modversion pipinpp
# Shows: 0.3.2 (but want 0.3.7)
```

**Solution: Force reinstall**
```bash
cd PiPinPP
git pull origin main
git checkout v0.3.7
./build.sh
cd build
sudo make install
sudo ldconfig

# Verify
pkg-config --modversion pipinpp
```

### Headers not updating

**Problem:** Old headers cached

**Solution: Clean install**
```bash
# Remove old installation
sudo rm -rf /usr/local/include/pipinpp
sudo rm -rf /usr/local/include/*.hpp
sudo rm -f /usr/local/lib/libpipinpp.*
sudo rm -f /usr/local/lib/pkgconfig/pipinpp.pc

# Reinstall
cd PiPinPP
git pull origin main
git checkout v0.3.7
./build.sh
cd build
sudo make install
sudo ldconfig
```

---

## 🔍 Advanced Debugging

### Enable Debug Output

```bash
# Compile with debug symbols and logging
g++ -g -DDEBUG main.cpp $(pkg-config --cflags --libs pipinpp) -o main

# Run with verbose output
./main
```

### Check Library Loading

```bash
# See what libraries your program uses
ldd ./main

# Should include:
#   libpipinpp.so => /usr/local/lib/libpipinpp.so
#   libgpiod.so.2 => /usr/lib/aarch64-linux-gnu/libgpiod.so.2
```

### Memory Leaks

```bash
# Check for memory leaks
valgrind --leak-check=full ./main
```

### GPIO State Check

```bash
# See current GPIO state
cat /sys/kernel/debug/gpio

# Requires root
sudo cat /sys/kernel/debug/gpio
```

---

## ❓ Still Stuck?

### Before Asking for Help

Gather this information:

```bash
# System info
uname -a
cat /etc/os-release

# PiPin++ version
pkg-config --modversion pipinpp

# GPIO status
gpiodetect
ls -la /dev/gpiochip*
groups $USER

# Compiler version
g++ --version

# Your code (simplified example)
```

### Get Help

- 💬 **Discord:** [Join PiPin++ Discord](https://discord.gg/wXeZP8Ev) - Quick community help
- 🐛 **GitHub Issues:** [Report a bug](https://github.com/Barbatos6669/PiPinPP/issues)
- 📖 **Documentation:** [Full API Reference](API_REFERENCE.md)

### Reporting Bugs

Include:
1. ✅ What you're trying to do
2. ✅ What you expected to happen
3. ✅ What actually happened (error messages)
4. ✅ Code example (simplified)
5. ✅ Hardware setup (circuit diagram or description)
6. ✅ System info (from commands above)

---

## 📚 Related Guides

- [Getting Started Guide](GETTING_STARTED.md) - Beginner tutorial
- [Pin Numbering Guide](PIN_NUMBERING.md) - Understanding GPIO pins
- [Permissions Guide](PERMISSIONS.md) - Setting up GPIO access
- [API Reference](API_REFERENCE.md) - Complete function documentation

---

*Last updated: November 5, 2025*
