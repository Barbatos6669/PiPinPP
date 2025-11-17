#!/usr/bin/env python3
"""
Blink LED Example
Classic "Hello World" of GPIO - blink an LED on GPIO 17
"""

import pypipinpp as gpio

LED_PIN = 17

def main():
    print("Blinking LED on GPIO 17")
    print("Press Ctrl+C to exit")
    
    # Setup
    gpio.pinMode(LED_PIN, gpio.OUTPUT)
    
    try:
        while True:
            gpio.digitalWrite(LED_PIN, gpio.HIGH)
            print("LED ON")
            gpio.delay(1000)
            
            gpio.digitalWrite(LED_PIN, gpio.LOW)
            print("LED OFF")
            gpio.delay(1000)
    
    except KeyboardInterrupt:
        print("\nExiting...")
        gpio.digitalWrite(LED_PIN, gpio.LOW)

if __name__ == "__main__":
    main()
