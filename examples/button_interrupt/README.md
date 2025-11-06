# Button Interrupt Example

Learn interrupt-driven GPIO programming - react instantly to button presses without polling.

## Features

- Hardware interrupts with `attachInterrupt()`
- Non-blocking button detection
- Instant response (no delay/polling)
- Interrupt Service Routine (ISR) patterns
- Proper debouncing for interrupts
- Arduino-compatible interrupt API

## Hardware Requirements

### Components
- 1x Pushbutton (normally open)
- 1x LED (for visual feedback)
- 1x 220Ω resistor (for LED)
- Breadboard and jumper wires
- Raspberry Pi (any model)

### Wiring Diagram

```
Raspberry Pi              Button             LED
┌──────────┐              ┌───┐            ┌─────┐
│          │              │   │            │ ──> │
│ GPIO 17  ├──────────────┤ o │            │     │
│ (Pin 11) │              │ o ├───GND      │ ──> │
│          │              └───┘            └──┬──┘
│ GPIO 18  ├────[220Ω]───────────────────────┘
│ (Pin 12) │                                  
│  GND     ├──────────────────────────────GND
│  (Pin 6) │
└──────────┘
```

### Pin Connections
- **GPIO 17** (Pin 11) → Button → GND (uses internal pull-up)
- **GPIO 18** (Pin 12) → 220Ω resistor → LED → GND

## Building

```bash
cd /path/to/PiPinPP/build
make example_button_interrupt
```

## Running

```bash
cd build/examples
./example_button_interrupt
```

## Expected Output

```
Button Interrupt Example
Waiting for button press on GPIO 17...
Main loop can do other work while waiting for button!

[Interrupt] Button pressed! Count: 1
[Interrupt] Button pressed! Count: 2
[Interrupt] Button pressed! Count: 3
Main loop iteration: 100
Main loop iteration: 200
[Interrupt] Button pressed! Count: 4
...
```

## Interrupts vs Polling

### Polling (traditional method):
```cpp
while (true) {
    if (digitalRead(BUTTON_PIN) == LOW) {
        // Handle button press
    }
    // Can't do other work efficiently
    delay(10);  // Must delay to avoid excessive CPU usage
}
```
**Problems**: Wastes CPU, delayed response, blocks other tasks

### Interrupts (this example):
```cpp
void buttonPressed() {
    // This runs IMMEDIATELY when button pressed
    pressCount++;
}

attachInterrupt(BUTTON_PIN, buttonPressed, FALLING);

while (true) {
    // Main loop free to do other work!
    doComplexCalculations();
    updateDisplay();
    // Button presses handled instantly in background
}
```
**Benefits**: Instant response, no polling overhead, non-blocking

## Understanding Interrupt Modes

```cpp
attachInterrupt(pin, function, mode);
```

| Mode | Triggers When | Use Case |
|------|---------------|----------|
| `RISING` | LOW → HIGH | Button release (with pull-down) |
| `FALLING` | HIGH → LOW | Button press (with pull-up) |
| `CHANGE` | Any transition | Detect press OR release |

With `INPUT_PULLUP` (this example): Use `FALLING` for button presses.

## ISR Best Practices

**DO:**
- ✅ Keep ISR short and fast
- ✅ Set flags, update counters
- ✅ Use `volatile` for shared variables
- ✅ Disable interrupts briefly if needed

**DON'T:**
- ❌ Call `delay()` or blocking functions
- ❌ Print to serial/console (slow!)
- ❌ Do complex calculations
- ❌ Access non-thread-safe resources

### Proper Pattern:
```cpp
volatile bool buttonFlag = false;

void buttonISR() {
    buttonFlag = true;  // Just set a flag
}

void loop() {
    if (buttonFlag) {
        buttonFlag = false;
        // Handle button press here (not in ISR)
        handleButtonPress();
    }
}
```

## Interrupt Debouncing

Physical buttons bounce, causing multiple rapid interrupts. This example uses time-based debouncing:

```cpp
void buttonISR() {
    unsigned long now = millis();
    if (now - lastInterruptTime > DEBOUNCE_MS) {
        // Genuine button press
        pressCount++;
        lastInterruptTime = now;
    }
    // Ignore bounces within debounce window
}
```

## Troubleshooting

### Multiple counts per press
- Increase `DEBOUNCE_MS` (try 200-300ms)
- Check button quality
- Add hardware debouncing (0.1µF capacitor across button)

### Interrupts not firing
- Verify correct interrupt mode (FALLING for pull-up)
- Check button wiring
- Ensure `attachInterrupt()` was called
- Check permissions: `groups` should show "gpio"

### Program crashes in ISR
- Remove `Serial.println()` or similar slow calls from ISR
- Check for non-volatile shared variables
- Avoid accessing objects that aren't thread-safe

### Spurious interrupts
- Add pull-up/pull-down resistor (or use `INPUT_PULLUP`)
- Shield wires from electrical noise
- Add small capacitor (100nF) for hardware debouncing

## Extending This Example

### Detect Long Press
```cpp
volatile unsigned long pressStartTime = 0;

void buttonPressed() {
    pressStartTime = millis();
}

void buttonReleased() {
    unsigned long pressDuration = millis() - pressStartTime;
    if (pressDuration > 3000) {
        Serial.println("Long press!");
    }
}

attachInterrupt(BUTTON_PIN, buttonPressed, FALLING);
attachInterrupt(BUTTON_PIN, buttonReleased, RISING);
```

### Multiple Buttons
```cpp
void button1ISR() { button1Pressed = true; }
void button2ISR() { button2Pressed = true; }
void button3ISR() { button3Pressed = true; }

attachInterrupt(BUTTON1_PIN, button1ISR, FALLING);
attachInterrupt(BUTTON2_PIN, button2ISR, FALLING);
attachInterrupt(BUTTON3_PIN, button3ISR, FALLING);
```

### Rotary Encoder
```cpp
void encoderISR() {
    int stateA = digitalRead(ENC_A);
    int stateB = digitalRead(ENC_B);
    
    if (stateA != stateB) {
        encoderPosition++;  // Clockwise
    } else {
        encoderPosition--;  // Counter-clockwise
    }
}

attachInterrupt(ENC_A, encoderISR, CHANGE);
```

## Next Steps

After mastering this example, try:
- **thread_safety** - Safe interrupt + thread interaction
- **timing_benchmark** - Measure interrupt latency
- **advanced_io** - Combine with `pulseIn()` for encoders

## References

- [PiPinPP Interrupt API](../../docs/API_REFERENCE.md#interrupt-functions)
- [Interrupt Tutorial (Coming Soon)](../../docs/tutorials/)
- [Arduino attachInterrupt() reference](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/)
- [Debouncing Guide](http://www.ganssle.com/debouncing.htm)

## License

Part of the PiPinPP project - see LICENSE file for details.
