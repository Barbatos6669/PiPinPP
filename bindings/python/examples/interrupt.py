#!/usr/bin/env python3
"""
Interrupt Example
Use interrupts to detect button presses
"""

import pypipinpp as gpio
import signal
import sys

LED_PIN = 17
BUTTON_PIN = 18
press_count = 0

def button_handler():
    """Called when button is pressed"""
    global press_count
    press_count += 1
    print(f"Button pressed! Count: {press_count}")
    gpio.digitalToggle(LED_PIN)

def signal_handler(sig, frame):
    """Handle Ctrl+C"""
    print("\nCleaning up...")
    gpio.detachInterrupt(BUTTON_PIN)
    gpio.digitalWrite(LED_PIN, gpio.LOW)
    sys.exit(0)

def main():
    print("Interrupt Example")
    print("Press button to toggle LED and increment counter")
    print("Press Ctrl+C to exit")
    
    # Setup
    gpio.pinMode(LED_PIN, gpio.OUTPUT)
    gpio.pinMode(BUTTON_PIN, gpio.INPUT_PULLUP)
    
    # Attach interrupt on falling edge (button press)
    gpio.attachInterrupt(BUTTON_PIN, button_handler, gpio.FALLING)
    
    # Handle Ctrl+C
    signal.signal(signal.SIGINT, signal_handler)
    
    # Keep program running
    while True:
        gpio.delay(1000)

if __name__ == "__main__":
    main()
