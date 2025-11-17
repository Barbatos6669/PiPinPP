#!/usr/bin/env python3
"""
I2C Scanner Example
Scan for I2C devices on the bus
"""

from pypipinpp import Wire

def main():
    print("I2C Scanner")
    print("-" * 40)
    
    # Initialize I2C
    if not Wire.begin():
        print("ERROR: Failed to initialize I2C")
        return
    
    print("Scanning I2C bus...")
    devices = Wire.scan()
    
    if devices:
        print(f"\nFound {len(devices)} device(s):")
        for addr in devices:
            print(f"  0x{addr:02X} ({addr})")
    else:
        print("\nNo I2C devices found")
    
    Wire.end()

if __name__ == "__main__":
    main()
