# 74HC595 Shift Register Example

This example demonstrates controlling a 74HC595 8-bit shift register using both **hardware SPI** and **software bit-banging** (`shiftOut()`), with performance comparison.

## Features

- **Hardware SPI**: Fast, reliable communication using Raspberry Pi's SPI peripheral
- **Software shiftOut()**: Arduino-compatible bit-banging on any GPIO pins
- **Performance Comparison**: Measures speed difference between methods
- **Pattern Demonstrations**: Various LED patterns (walking bits, alternating, etc.)
- **Binary Counter**: Visual display of counting in binary

## Hardware Requirements

### Components
- 1x 74HC595 shift register (or compatible like SN74HC595)
- 8x LEDs (any color)
- 8x 220Ω resistors (for LED current limiting)
- Breadboard and jumper wires
- Raspberry Pi (any model with SPI support)

### Wiring Diagram

```
74HC595 Pin Layout:
                   ┌─────┴─────┐
        Q1  ← 1  15 → Q0
        Q2  ← 2  14 → DS/SER (Data Serial Input)
        Q3  ← 3  13 → OE (Output Enable, active LOW)
        Q4  ← 4  12 → STCP/RCK (Storage Register Clock/Latch)
        Q5  ← 5  11 → SHCP/SCK (Shift Register Clock)
        Q6  ← 6  10 → SRCLR (Master Reset, active LOW)
        Q7  ← 7   9 → Q7' (Serial Data Out - for cascading)
       GND    8      16   VCC
                   └───────────┘

Connections to Raspberry Pi:
============================

Hardware SPI Mode (faster, recommended):
-----------------------------------------
74HC595 Pin 14 (DS/SER)   → GPIO 10 (MOSI/SPI0_MOSI)
74HC595 Pin 11 (SHCP/SCK) → GPIO 11 (SCLK/SPI0_SCLK)
74HC595 Pin 12 (STCP/RCK) → GPIO 8  (CE0/SPI0_CE0) - used as latch
74HC595 Pin 13 (OE)       → GND (always enabled)
74HC595 Pin 10 (SRCLR)    → 3.3V (no reset)
74HC595 Pin 16 (VCC)      → 3.3V
74HC595 Pin 8  (GND)      → GND

Software Mode (any GPIO pins):
-------------------------------
74HC595 Pin 14 (DS/SER)   → GPIO 23 (SW_DATA_PIN)
74HC595 Pin 11 (SHCP/SCK) → GPIO 24 (SW_CLOCK_PIN)
74HC595 Pin 12 (STCP/RCK) → GPIO 25 (SW_LATCH_PIN)
74HC595 Pin 13 (OE)       → GND
74HC595 Pin 10 (SRCLR)    → 3.3V
74HC595 Pin 16 (VCC)      → 3.3V
74HC595 Pin 8  (GND)      → GND

LED Connections (for both modes):
----------------------------------
Q0-Q7 (Pins 15,1-7) → LEDs → 220Ω resistors → GND

Note: This example can drive both setups simultaneously if you
wire both SPI and software pins, showing real-time comparison!
```

## ASCII Wiring Diagram

```
Raspberry Pi                74HC595                LEDs (x8)
┌──────────┐              ┌───────────┐
│          │              │           │
│ GPIO 10  ├──────────────┤ DS (14)   │
│  (MOSI)  │              │           │
│          │              │           │      LED0
│ GPIO 11  ├──────────────┤ SHCP (11) │    ┌─────┐
│  (SCLK)  │              │           │    │ ──> │
│          │              │           ├────┤     ├─[220Ω]─GND
│ GPIO 8   ├──────────────┤ STCP (12) │    │ ──> │
│  (CE0)   │ (Latch)      │           │    └─────┘
│          │              │  Q0 (15)  ├─────┘
│  3.3V    ├──────────────┤ VCC (16)  │
│          │              │           │    (Q1-Q7 similar)
│  GND     ├──────────────┤ GND (8)   │
│          │         ┌────┤ OE (13)   │
│          │         │    │ SRCLR(10) ├────┐
└──────────┘         │    └───────────┘    │
                     │                     │
                    GND                  3.3V
```

## Building

From the build directory:
```bash
cd /path/to/PiPinPP/build
make example_spi_74hc595
```

Or rebuild everything:
```bash
./scripts/build.sh
```

## Running

### Enable SPI Interface (if not already enabled)
```bash
sudo raspi-config
# Navigate to: Interfacing Options → SPI → Enable

# Verify SPI is enabled:
ls -l /dev/spidev*
# Should show: /dev/spidev0.0 and /dev/spidev0.1
```

### Run the Example
```bash
cd build/examples
./example_spi_74hc595
```

## Expected Output

```
74HC595 Shift Register - SPI vs Software Comparison
===================================================

Initializing Hardware SPI...
  SPI Mode: 0 (CPOL=0, CPHA=0)
  Bit Order: MSB First
  Clock: 1000000 Hz
  Latch Pin: GPIO 8

Initializing Software Shift-Out...
  Data Pin: GPIO 23
  Clock Pin: GPIO 24
  Latch Pin: GPIO 25

Displaying common patterns...

  All OFF           : 0b00000000 (0x00)
  All ON            : 0b11111111 (0xff)
  Alternating 1     : 0b10101010 (0xaa)
  Alternating 2     : 0b01010101 (0x55)
  Walking 1s Left   : 0b00000001 (0x01)
  Walking 1s Right  : 0b10000000 (0x80)
  Center Out        : 0b00011000 (0x18)

Animating walking bits...

Performance Test: Counting 0-255 on outputs
---------------------------------------------
  Software (shiftOut):  89234 µs
  Hardware SPI:         31245 µs

  Speedup: 2.86x faster with hardware SPI

Binary counter demo (press Ctrl+C to stop)...
Watch the LEDs count in binary!
```

## What You'll See

1. **Pattern Demo**: Various LED patterns displayed sequentially
2. **Walking Bits**: Single LED "walking" across the array
3. **Performance Test**: Speed comparison printed to console
4. **Binary Counter**: LEDs continuously count 0-255 in binary

## Understanding the Output

- **Q0 (rightmost LED)**: Least Significant Bit (LSB)
- **Q7 (leftmost LED)**: Most Significant Bit (MSB)
- Example: Binary `0b00000101` = Decimal 5 → LEDs: `□□□□□■□■` (Q2 and Q0 lit)

## Performance Notes

Typical speedup with hardware SPI vs software:
- **Raspberry Pi 4**: 2.5-3x faster
- **Raspberry Pi 5**: 3-4x faster
- **Pi Zero**: 2-2.5x faster

Hardware SPI advantages:
- Consistent timing (no OS jitter)
- Frees CPU for other tasks
- Higher maximum speed (up to 125 MHz on Pi 5)

## Troubleshooting

### SPI not working
```bash
# Check if SPI is enabled
lsmod | grep spi
# Should show: spi_bcm2835

# Check permissions
ls -l /dev/spidev0.0
# Add user to spi group if needed:
sudo usermod -a -G spi $USER
# Log out and back in for group change to take effect
```

### LEDs not lighting
- Check power: VCC = 3.3V, GND connected
- Verify OE (pin 13) is connected to GND
- Verify SRCLR (pin 10) is connected to 3.3V
- Check LED polarity (long leg = anode/positive)
- Verify resistor values (220Ω recommended)

### Erratic behavior
- Add 0.1µF capacitor between VCC and GND (decoupling)
- Use shorter wires if on breadboard
- Reduce SPI clock speed: `SPI.setClock(500000);` (500 kHz)

## Extending This Example

### Cascading Multiple 74HC595s
Connect Q7' (pin 9) of first chip to DS (pin 14) of second chip:
```cpp
uint16_t data = 0xABCD;  // 16 bits for two chips
digitalWrite(LATCH_PIN, LOW);
SPI.transfer(highByte(data));  // Send high byte first
SPI.transfer(lowByte(data));   // Then low byte
digitalWrite(LATCH_PIN, HIGH);
```

### Controlling 7-Segment Displays
Wire Q0-Q7 to segments A-G plus DP:
```cpp
// Display digit '5' on common-cathode 7-segment display
const uint8_t DIGIT_5 = 0b01101101;  // Segments A,C,D,F,G
sendViaSPI(DIGIT_5);
```

## References

- [74HC595 Datasheet](https://www.ti.com/lit/ds/symlink/sn74hc595.pdf)
- [PiPinPP SPI Documentation](../../docs/API_REFERENCE.md#spi)
- [Raspberry Pi SPI Documentation](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#spi-hardware)

## License

Part of the PiPinPP project - see LICENSE file for details.
