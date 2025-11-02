# Pin Numbering Guide

Understanding GPIO pin numbering on Raspberry Pi is essential for using PiPinPP correctly.

## 🎯 TL;DR - Quick Reference

**PiPinPP uses BCM (Broadcom) pin numbering** - the same as most GPIO libraries.

```cpp
pinMode(17, OUTPUT);  // ✅ BCM GPIO 17 (Physical pin 11)
digitalWrite(17, HIGH);
```

---

## 📍 Pin Numbering Systems

Raspberry Pi has **three different numbering systems**. It's confusing, but this guide will clear it up!

### **1. BCM (Broadcom) Numbers** ⭐ **(What PiPinPP Uses)**

- **GPIO numbers from the Broadcom SoC chip**
- Range: **GPIO 0-27** on most Raspberry Pi models
- **Most common** in software libraries
- **Consistent across Pi models** (mostly)

**Example:**
```cpp
pinMode(17, OUTPUT);     // BCM GPIO 17
digitalWrite(27, HIGH);  // BCM GPIO 27
```

### **2. Physical (Board) Numbers**

- **Physical pin position** on the 40-pin header
- Range: **1-40** (top to bottom, left and right columns)
- **Changes meaning** across Pi models
- Used by some libraries (like WiringPi's "board" mode)

**Example:**
- Physical pin 11 = BCM GPIO 17
- Physical pin 13 = BCM GPIO 27

### **3. WiringPi Numbers** (Legacy)

- **Old numbering** from the WiringPi library
- **Not used** by PiPinPP
- Mentioned only for reference

---

## 🗺️ Pin Mapping: BCM ↔ Physical

### **Raspberry Pi 40-Pin Header (All Models with 40 pins)**

```
        3.3V  [ 1] [ 2]  5V
   BCM 2 (SDA) [ 3] [ 4]  5V
   BCM 3 (SCL) [ 5] [ 6]  GND
       BCM 4   [ 7] [ 8]  BCM 14 (TXD)
         GND   [ 9] [10]  BCM 15 (RXD)
      BCM 17   [11] [12]  BCM 18
      BCM 27   [13] [14]  GND
      BCM 22   [15] [16]  BCM 23
        3.3V   [17] [18]  BCM 24
      BCM 10   [19] [20]  GND
       BCM 9   [21] [22]  BCM 25
      BCM 11   [23] [24]  BCM 8
         GND   [25] [26]  BCM 7
   BCM 0 (ID)  [27] [28]  BCM 1 (ID)
       BCM 5   [29] [30]  GND
       BCM 6   [31] [32]  BCM 12
      BCM 13   [33] [34]  GND
      BCM 19   [35] [36]  BCM 16
      BCM 26   [37] [38]  BCM 20
         GND   [39] [40]  BCM 21
```

### **Quick Lookup Table**

| Physical Pin | BCM GPIO | Common Use | Notes |
|--------------|----------|------------|-------|
| 3, 5 | 2, 3 | I2C | SDA, SCL (often used for I2C) |
| 7 | 4 | - | General purpose |
| 8, 10 | 14, 15 | UART | TXD, RXD (serial communication) |
| 11 | 17 | - | **Popular for LED demos** |
| 12 | 18 | PWM | Hardware PWM capable |
| 13 | 27 | - | General purpose |
| 15, 16, 18 | 22, 23, 24 | - | General purpose |
| 19, 21, 23, 24 | 10, 9, 11, 8 | SPI | MOSI, MISO, SCLK, CE0 |
| 22 | 25 | - | General purpose |
| 26 | 7 | SPI | CE1 |
| 27, 28 | 0, 1 | I2C ID | Reserved for HAT ID EEPROM |
| 29, 31 | 5, 6 | - | General purpose |
| 32 | 12 | PWM | Hardware PWM capable |
| 33 | 13 | PWM | Hardware PWM capable |
| 35, 36 | 19, 16 | - | General purpose |
| 37, 38, 40 | 26, 20, 21 | - | General purpose |

---

## 💡 Which Pins Should I Use?

### **✅ Safe Pins for General GPIO** (Best for beginners)

These pins are safe to use and have no special functions:

```cpp
// Best pins for LED/button demos
pinMode(17, OUTPUT);  // Physical pin 11
pinMode(27, OUTPUT);  // Physical pin 13
pinMode(22, INPUT);   // Physical pin 15
pinMode(23, INPUT);   // Physical pin 16
pinMode(24, OUTPUT);  // Physical pin 18
pinMode(25, INPUT);   // Physical pin 22
```

**BCM GPIO: 17, 27, 22, 23, 24, 25, 5, 6, 12, 13, 16, 19, 20, 21, 26**

### **⚠️ Pins with Special Functions** (Use with caution)

| BCM GPIO | Special Function | Warning |
|----------|------------------|---------|
| 0, 1 | I2C ID EEPROM | **Don't use** - Reserved for HAT identification |
| 2, 3 | I2C (SDA, SCL) | Used if you have I2C devices connected |
| 4 | General/1-Wire | Safe, but check for conflicting uses |
| 7, 8, 9, 10, 11 | SPI | Used if you have SPI devices |
| 14, 15 | UART (Serial) | Used if you need console/serial access |
| 18, 12, 13, 19 | Hardware PWM | Can be used as GPIO, but has PWM capability |

---

## 🔧 How to Find Pin Numbers

### **Method 1: Use `pinout` command** (Easiest!)

On your Raspberry Pi terminal:
```bash
pinout
```

This shows a visual diagram of your specific Pi model with pin numbers!

### **Method 2: Physical counting**

1. Hold Pi with GPIO header at top
2. Count from top-left (pin 1) down the left column
3. Count from top-right (pin 2) down the right column

### **Method 3: Online Reference**

- [pinout.xyz](https://pinout.xyz/) - Interactive pinout diagram
- [Raspberry Pi Documentation](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html)

---

## 🛠️ Example Code with Comments

```cpp
#include "ArduinoCompat.hpp"

int main() {
    // BCM GPIO 17 = Physical pin 11
    pinMode(17, OUTPUT);
    
    // BCM GPIO 27 = Physical pin 13
    pinMode(27, OUTPUT);
    
    // BCM GPIO 22 = Physical pin 15 (input with pull-up resistor)
    pinMode(22, INPUT_PULLUP);
    
    // Blink LED on GPIO 17
    while(true) {
        digitalWrite(17, HIGH);
        delay(500);
        digitalWrite(17, LOW);
        delay(500);
        
        // Read button on GPIO 22
        int buttonState = digitalRead(22);
        if (buttonState == LOW) {  // Button pressed (pull-up = LOW when pressed)
            digitalWrite(27, HIGH);
        } else {
            digitalWrite(27, LOW);
        }
    }
    
    return 0;
}
```

---

## 🧪 Testing Pins

Before using a pin in your project, test it with this simple program:

```cpp
#include "ArduinoCompat.hpp"

int main() {
    int testPin = 17;  // Change this to test different pins
    
    pinMode(testPin, OUTPUT);
    
    // Blink 5 times
    for(int i = 0; i < 5; i++) {
        digitalWrite(testPin, HIGH);
        delay(500);
        digitalWrite(testPin, LOW);
        delay(500);
    }
    
    return 0;
}
```

Connect an LED (with resistor) to verify it works!

---

## 📚 Additional Resources

- **PiPinPP Examples**: See `examples/` folder for working code
- **Raspberry Pi GPIO Guide**: https://www.raspberrypi.com/documentation/computers/raspberry-pi.html
- **Interactive Pinout**: https://pinout.xyz/
- **GPIO Zero Documentation**: https://gpiozero.readthedocs.io/ (Python, but good reference)

---

## ❓ FAQ

**Q: Why does PiPinPP use BCM numbering instead of physical?**  
A: BCM numbers are standard across most GPIO libraries (Python GPIO, libgpiod, WiringPi BCM mode). It's more consistent and easier to share code.

**Q: Can I use WiringPi numbers with PiPinPP?**  
A: No, PiPinPP uses BCM numbers only. Convert WiringPi numbers to BCM using online references.

**Q: What happens if I use an invalid pin number?**  
A: PiPinPP will throw an exception with an error message. Valid range is 0-27 on most models.

**Q: Do pin numbers change between Pi models?**  
A: BCM numbers are mostly consistent. The 40-pin header (Pi 2, 3, 4, Zero) all have GPIO 0-27. Earlier models (Pi 1 Model A/B) had only 26 pins.

**Q: Can I damage my Pi by using the wrong pin?**  
A: Using the wrong GPIO pin won't damage your Pi, but avoid pins 1, 2, 4, 17 (power pins) and pins 6, 9, 14, 20, 25, 30, 34, 39 (ground pins). PiPinPP only controls GPIO pins, not power/ground.
