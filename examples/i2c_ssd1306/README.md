# SSD1306 OLED Display Example

Display text and graphics on a 128x64 SSD1306 OLED screen via I2C.

## Features

- Initialize SSD1306 display
- Clear screen
- Display text
- Draw pixels and shapes
- Demonstrate basic graphics
- Arduino Wire library compatible

## Hardware Setup

**SSD1306 OLED Connections:**
- VCC → 3.3V (or 5V if module has regulator)
- GND → GND
- SDA → GPIO 2 (Pin 3)
- SCL → GPIO 3 (Pin 5)

**I2C Address**: 0x3C (common) or 0x3D

**Display Resolution**: 128x64 pixels (monochrome)

## Enable I2C

```bash
sudo raspi-config
# Interface Options → I2C → Enable

# Verify:
sudo i2cdetect -y 1
# Should show 3C or 3D
```

## Building & Running

```bash
cd /path/to/PiPinPP/build
make example_i2c_ssd1306
./examples/example_i2c_ssd1306
```

## Expected Output

The OLED display will show:
```
PiPinPP
SSD1306 Demo
------------
Hello, World!
Counter: 123
```

Plus various graphics patterns demonstrating pixel control.

## Display Basics

### Memory Layout
- 128 columns × 64 rows = 8,192 pixels
- Organized as 8 pages of 128×8 pixels
- Each byte controls 8 vertical pixels

### Coordinate System
```
(0,0) ──────────────────────> (127,0)
  │                               
  │   Text and graphics here
  │
  ↓
(0,63)                      (127,63)
```

## Common Commands

### Initialize Display
```cpp
Wire.beginTransmission(0x3C);
Wire.write(0x00);  // Command mode
Wire.write(0xAF);  // Display ON
Wire.endTransmission();
```

### Clear Screen
```cpp
// Fill display buffer with zeros
for (int page = 0; page < 8; page++) {
    // Set page address
    // Write 128 bytes of 0x00
}
```

### Draw Pixel
```cpp
int x = 64, y = 32;
int page = y / 8;
int bit = y % 8;
buffer[page * 128 + x] |= (1 << bit);
```

## Display Patterns

The example demonstrates:
1. **Text rendering** - Character-by-character display
2. **Horizontal lines** - Fast page-aligned drawing
3. **Vertical lines** - Bit-manipulation for columns
4. **Checkerboard** - Alternating pattern
5. **Scrolling** - Hardware scroll commands

## Performance

- **I2C Speed**: Typically 400 kHz (fast mode)
- **Full Screen Update**: ~30-50 ms
- **Effective Frame Rate**: ~20-30 FPS
- **Best for**: Text, simple graphics, slow animations

## Troubleshooting

### Display shows nothing
1. Check I2C address (0x3C vs 0x3D)
   ```bash
   ./example_i2c_scanner
   ```
2. Verify power (3.3V or 5V)
3. Check contrast setting (may be too low)
4. Ensure display is enabled (command 0xAF)

### Garbled display
- Check I2C wiring (swap SDA/SCL?)
- Verify I2C speed (slow down if issues)
- Check for loose connections

### Some pixels missing
- Normal for cheap OLED modules
- Dead pixels cannot be fixed
- Consider using different display module

### Display dims over time
- OLEDs degrade with use (especially blue pixels)
- Typical lifespan: 10,000-50,000 hours
- Avoid leaving static images (causes burn-in)

## Extending This Example

### Add Graphics Library
Consider using existing libraries:
- Adafruit_SSD1306 (Arduino, portable)
- u8g2 (Universal graphics library)
- luma.oled (Python, for comparison)

### Display Sensor Data
```cpp
char buffer[32];
snprintf(buffer, sizeof(buffer), "Temp: %.1f C", temperature);
displayText(0, 0, buffer);
```

### Create Animations
```cpp
for (int x = 0; x < 128; x++) {
    clearScreen();
    drawCircle(x, 32, 10);
    updateDisplay();
    delay(10);
}
```

## Common Uses

- Sensor readout displays
- Status indicators
- Menu systems
- Oscilloscope displays
- Real-time data visualization
- Embedded user interfaces

## See Also

- **i2c_bmp280** - Display temperature/pressure data
- **i2c_mpu6050** - Show orientation data
- **i2c_scanner** - Find display address

## References

- [SSD1306 Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
- [Adafruit SSD1306 Library](https://github.com/adafruit/Adafruit_SSD1306)
- [PiPinPP Wire API](../../docs/API_REFERENCE.md#wire-i2c)

## License

Part of the PiPinPP project.
