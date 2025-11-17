#!/usr/bin/env python3
"""
PWM LED Fading Example
Smoothly fade an LED in and out using PWM
"""

import pypipinpp as gpio

LED_PIN = 17

def main():
    print("LED Fading Example")
    print("Press Ctrl+C to exit")
    
    # Setup
    gpio.pinMode(LED_PIN, gpio.OUTPUT)
    
    try:
        while True:
            # Fade in
            print("Fading in...")
            for brightness in range(256):
                gpio.analogWrite(LED_PIN, brightness)
                gpio.delay(5)
            
            # Fade out
            print("Fading out...")
            for brightness in range(255, -1, -1):
                gpio.analogWrite(LED_PIN, brightness)
                gpio.delay(5)
    
    except KeyboardInterrupt:
        print("\nExiting...")
        gpio.noAnalogWrite(LED_PIN)
        gpio.digitalWrite(LED_PIN, gpio.LOW)

if __name__ == "__main__":
    main()
