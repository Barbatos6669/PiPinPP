# Tutorial: Traffic Light Controller

Build a working traffic light system with PiPin++! 🚦

## What You'll Build

A traffic light that cycles through:
- 🔴 Red (5 seconds)
- 🟡 Yellow (2 seconds)  
- 🟢 Green (5 seconds)
- 🟡 Yellow (2 seconds)
- ↺ Repeat

## What You Need

**Hardware:**
- 1× Red LED
- 1× Yellow/Amber LED
- 1× Green LED
- 3× 220Ω resistors
- 4× Jumper wires
- 1× Breadboard (optional but helpful)

**Time:** 15 minutes

**Skill level:** Beginner (assumes you've done [Getting Started](../GETTING_STARTED.md))

---

## Step 1: Wire the Circuit

### Circuit Diagram

```
Raspberry Pi
┌──────────────┐
│              │
│   GPIO 17 ───┼──→ 220Ω ──→ Red LED ──→ GND
│   GPIO 27 ───┼──→ 220Ω ──→ Yellow LED ──→ GND  
│   GPIO 22 ───┼──→ 220Ω ──→ Green LED ──→ GND
│              │
└──────────────┘
```

### Wiring Steps

**⚠️ Power off your Pi while wiring!**

1. **Red LED:**
   - GPIO 17 (Pin 11) → 220Ω resistor → Red LED (+) → LED (-) → GND (Pin 14)

2. **Yellow LED:**
   - GPIO 27 (Pin 13) → 220Ω resistor → Yellow LED (+) → LED (-) → GND

3. **Green LED:**
   - GPIO 22 (Pin 15) → 220Ω resistor → Green LED (+) → LED (-) → GND

**Remember:** 
- Long leg = (+) to resistor
- Short leg = (-) to ground

### Using a Breadboard

If you have a breadboard, it's easier:

```
      Red    Yellow  Green
GPIO: 17     27      22
       ↓      ↓       ↓
    [220Ω] [220Ω] [220Ω]
       ↓      ↓       ↓
    [LED+] [LED+] [LED+]
       ↓      ↓       ↓
    [LED-] [LED-] [LED-]
       └──────┴───────┘
              ↓
            [GND]
```

---

## Step 2: Write the Code

Create a new file:

```bash
cd ~/pipinpp-projects
nano traffic_light.cpp
```

### Basic Version (Simple Timing)

```cpp
#include <ArduinoCompat.hpp>
#include <iostream>

// Pin definitions
const int RED_LED = 17;
const int YELLOW_LED = 27;
const int GREEN_LED = 22;

// Timing constants (in milliseconds)
const int RED_TIME = 5000;      // 5 seconds
const int YELLOW_TIME = 2000;   // 2 seconds
const int GREEN_TIME = 5000;    // 5 seconds

int main() {
    std::cout << "=== Traffic Light Controller ===" << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl << std::endl;
    
    // Initialize all LEDs as outputs
    pinMode(RED_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    
    // Turn all lights off initially
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    
    std::cout << "Traffic light started!" << std::endl << std::endl;
    
    // Main traffic light loop
    while (true) {
        // RED LIGHT
        std::cout << "🔴 RED - STOP" << std::endl;
        digitalWrite(RED_LED, HIGH);
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
        delay(RED_TIME);
        
        // YELLOW LIGHT (after red)
        std::cout << "🟡 YELLOW - PREPARE" << std::endl;
        digitalWrite(RED_LED, LOW);
        digitalWrite(YELLOW_LED, HIGH);
        digitalWrite(GREEN_LED, LOW);
        delay(YELLOW_TIME);
        
        // GREEN LIGHT
        std::cout << "🟢 GREEN - GO" << std::endl;
        digitalWrite(RED_LED, LOW);
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
        delay(GREEN_TIME);
        
        // YELLOW LIGHT (before red)
        std::cout << "🟡 YELLOW - SLOW DOWN" << std::endl;
        digitalWrite(RED_LED, LOW);
        digitalWrite(YELLOW_LED, HIGH);
        digitalWrite(GREEN_LED, LOW);
        delay(YELLOW_TIME);
    }
    
    return 0;
}
```

---

## Step 3: Compile and Run

```bash
# Compile
g++ traffic_light.cpp $(pkg-config --cflags --libs pipinpp) -o traffic_light

# Run
./traffic_light
```

### Success! 🎉

You should see:
- 🔴 Red LED on for 5 seconds
- 🟡 Yellow LED on for 2 seconds
- 🟢 Green LED on for 5 seconds
- 🟡 Yellow LED on for 2 seconds
- Repeat forever!

**To stop:** Press `Ctrl+C`

---

## Step 4: Add Features (Advanced)

### Feature 1: Countdown Timer

Show how many seconds remain:

```cpp
void countdownLight(int pin, const char* color, int seconds) {
    digitalWrite(pin, HIGH);
    
    for (int i = seconds; i > 0; i--) {
        std::cout << color << " - " << i << " seconds remaining" << std::endl;
        delay(1000);
    }
    
    digitalWrite(pin, LOW);
}

// In main loop:
digitalWrite(RED_LED, HIGH);
digitalWrite(YELLOW_LED, LOW);
digitalWrite(GREEN_LED, LOW);
countdownLight(RED_LED, "🔴 RED", RED_TIME / 1000);
```

### Feature 2: Pedestrian Button

Add a button to request crossing:

**Hardware:** Add button to GPIO 18

```cpp
const int BUTTON_PIN = 18;
bool pedestrianRequest = false;

void setup() {
    pinMode(RED_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void checkPedestrianButton() {
    if (digitalRead(BUTTON_PIN) == LOW) {
        pedestrianRequest = true;
        std::cout << "🚶 Pedestrian crossing requested!" << std::endl;
    }
}

// In main loop, check button during each light:
while (true) {
    // During RED light
    for (int i = 0; i < RED_TIME/100; i++) {
        checkPedestrianButton();
        delay(100);
    }
    
    // If button pressed, extend red light for pedestrians
    if (pedestrianRequest) {
        std::cout << "⏱️ Extending red light for pedestrians" << std::endl;
        delay(3000);  // Extra 3 seconds
        pedestrianRequest = false;
    }
    
    // Continue with yellow, green...
}
```

### Feature 3: Night Mode

Flash yellow at night (press a button to toggle):

```cpp
bool nightMode = false;

void nightModeFlash() {
    std::cout << "🌙 NIGHT MODE - Flashing Yellow" << std::endl;
    
    while (nightMode) {
        digitalWrite(YELLOW_LED, HIGH);
        delay(500);
        digitalWrite(YELLOW_LED, LOW);
        delay(500);
        
        // Check button to exit night mode
        if (digitalRead(BUTTON_PIN) == LOW) {
            nightMode = false;
            delay(200);  // Debounce
        }
    }
}
```

---

## Challenges

Try these modifications to level up your skills:

### 🌟 Easy Challenges

1. **Different Timings:** Make green light longer (10 seconds)
2. **Weekend Mode:** Shorter yellow lights on weekends
3. **Startup Sequence:** Flash all lights 3 times before starting

### 🌟🌟 Medium Challenges

4. **Two-Way Traffic:** Control two traffic lights (opposite directions)
5. **Walk Signal:** Add a 4th LED for pedestrian crossing indicator
6. **Emergency Override:** Button to force red light (for emergency vehicles)

### 🌟🌟🌟 Hard Challenges

7. **Intelligent Timing:** Use sensors to detect waiting traffic
8. **Rush Hour Mode:** Adjust timings based on time of day
9. **Multi-Intersection:** Coordinate multiple traffic lights

---

## Code Improvements

### Better Structure with Functions

```cpp
#include <ArduinoCompat.hpp>
#include <iostream>

const int RED = 17, YELLOW = 27, GREEN = 22;

void setLight(int red, int yellow, int green) {
    digitalWrite(RED, red);
    digitalWrite(YELLOW, yellow);
    digitalWrite(GREEN, green);
}

void showRed(int duration) {
    std::cout << "🔴 RED - STOP" << std::endl;
    setLight(HIGH, LOW, LOW);
    delay(duration);
}

void showYellow(int duration) {
    std::cout << "🟡 YELLOW" << std::endl;
    setLight(LOW, HIGH, LOW);
    delay(duration);
}

void showGreen(int duration) {
    std::cout << "🟢 GREEN - GO" << std::endl;
    setLight(LOW, LOW, HIGH);
    delay(duration);
}

int main() {
    pinMode(RED, OUTPUT);
    pinMode(YELLOW, OUTPUT);
    pinMode(GREEN, OUTPUT);
    
    while (true) {
        showRed(5000);
        showYellow(2000);
        showGreen(5000);
        showYellow(2000);
    }
    
    return 0;
}
```

---

## Troubleshooting

### LEDs not lighting in correct order

**Check your pin numbers:**
```bash
# Test each LED individually
gpioset gpiochip0 17=1  # Red
gpioset gpiochip0 27=1  # Yellow
gpioset gpiochip0 22=1  # Green
```

### Wrong colors?

Check your wiring - you might have swapped two LEDs!

### All LEDs dim or flickering?

- Check resistor values (should be 220Ω - 1kΩ)
- Make sure all ground connections are secure
- Verify power supply is adequate

---

## What You Learned

- ✅ Controlling multiple GPIOs simultaneously
- ✅ Using timing to create sequences
- ✅ Organizing code with functions
- ✅ State machines (traffic light states)
- ✅ Real-world application of GPIO

---

## Next Steps

**More Tutorials:** *(Coming in v0.3.8)*
- Multiple LED Patterns - Create cool light patterns
- Button-Controlled Patterns - Interactive LED control
- PWM LED Fading - Smooth LED transitions

**Example Code:**
- [examples/led_fade/](../../examples/led_fade/) - PWM examples
- [examples/button_interrupt/](../../examples/button_interrupt/) - Button handling

**Documentation:**
- [API Reference](../API_REFERENCE.md) - All available functions
- [Troubleshooting Guide](../TROUBLESHOOTING.md) - Common issues

---

## Share Your Project!

Built a traffic light? Share it on Discord! 🚦

- Take a photo or video
- Share your code modifications
- Help other beginners learn

[Join PiPin++ Discord →](https://discord.gg/wXeZP8Ev)

---

*Happy making!* 🎉

*Last updated: November 5, 2025*
