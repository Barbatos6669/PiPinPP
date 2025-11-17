#!/usr/bin/env python3
"""
Button Input Example
Read a button and control an LED
"""

import pypipinpp as gpio

LED_PIN = 17
BUTTON_PIN = 18

def main():
    print("Button Control Example")
    print("Press button to light LED")
    print("Press Ctrl+C to exit")
    
    # Setup
    gpio.pinMode(LED_PIN, gpio.OUTPUT)
    gpio.pinMode(BUTTON_PIN, gpio.INPUT_PULLUP)
    
    try:
        while True:
            # Button pressed = LOW (pull-up resistor)
            if gpio.digitalRead(BUTTON_PIN) == gpio.LOW:
                gpio.digitalWrite(LED_PIN, gpio.HIGH)
                print("Button pressed - LED ON")
            else:
                gpio.digitalWrite(LED_PIN, gpio.LOW)
            
            gpio.delay(10)  # Small delay for debouncing
    
    except KeyboardInterrupt:
        print("\nExiting...")
        gpio.digitalWrite(LED_PIN, gpio.LOW)

if __name__ == "__main__":
    main()
