# I2C Scanner Example

Scan the I2C bus to detect connected devices - essential for I2C troubleshooting.

## Features

- Scan all 127 I2C addresses
- Detect connected devices
- Display device addresses in hex format
- Identify common I2C devices
- Essential debugging tool

## Hardware Requirements

### Components
- Any I2C device(s) to scan
- No additional components needed (uses built-in I2C pins)

### I2C Pin Connections (Raspberry Pi)

**All Raspberry Pi Models (except Pi 5):**
- **SDA** (GPIO 2, Pin 3) → Device SDA
- **SCL** (GPIO 3, Pin 5) → Device SCL
- **3.3V** → Device VCC
- **GND** → Device GND

**Raspberry Pi 5:**
- **SDA** (GPIO 2, Pin 3) → Device SDA (I2C on pins, bus 1)
- **SCL** (GPIO 3, Pin 5) → Device SCL (I2C on pins, bus 1)
- Uses `/dev/i2c-20` for pin headers

**Note**: Most I2C devices have built-in pull-up resistors. If needed, add external 4.7kΩ resistors from SDA/SCL to 3.3V.

## Enable I2C

```bash
sudo raspi-config
# Navigate to: Interface Options → I2C → Enable

# Verify I2C is enabled:
ls -l /dev/i2c-*
# Should show: /dev/i2c-1 (or /dev/i2c-20 on Pi 5)

# Check for I2C devices (requires i2c-tools):
sudo apt-get install i2c-tools
sudo i2cdetect -y 1  # Use 20 for Pi 5
```

## Building

```bash
cd /path/to/PiPinPP/build
make example_i2c_scanner
```

## Running

```bash
cd build/examples
./example_i2c_scanner
```

## Expected Output

```
I2C Scanner - Scanning for devices...

Scanning I2C bus...
     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
00:          -- -- -- -- -- -- -- -- -- -- -- -- -- 
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
70: -- -- -- -- -- -- 76 --                         

Found 1 device(s):
  0x76 - Possible: BMP280, BME280, BME680, MS5607

Scan complete!
```

## Interpreting Results

### Common I2C Device Addresses

| Address | Device | Description |
|---------|--------|-------------|
| `0x27` | PCF8574 | LCD I2C backpack |
| `0x3C` | SSD1306 | 128x64 OLED display |
| `0x3D` | SSD1306 | Alternate address |
| `0x48` | ADS1115 | 16-bit ADC |
| `0x68` | MPU6050, DS1307 | IMU or RTC |
| `0x69` | MPU6050 | Alternate address |
| `0x76` | BMP280, BME280 | Pressure/temp sensor |
| `0x77` | BMP280, BME280 | Alternate address |

### Address Ranges
- `0x00-0x07`: Reserved
- `0x08-0x77`: Valid device addresses
- `0x78-0x7F`: Reserved

## Troubleshooting

### No devices found
1. Check wiring:
   ```bash
   # Verify connections with multimeter
   # SDA should be on GPIO 2 (Pin 3)
   # SCL should be on GPIO 3 (Pin 5)
   ```

2. Check I2C is enabled:
   ```bash
   lsmod | grep i2c
   # Should show: i2c_bcm2835 or similar
   ```

3. Check device power:
   ```bash
   # Measure voltage between VCC and GND
   # Should be ~3.3V
   ```

4. Try command-line tool:
   ```bash
   sudo i2cdetect -y 1  # Compare with scanner results
   ```

### Scanner detects wrong address
- Some devices have configurable addresses (check jumpers/solder bridges)
- Verify device datasheet for correct address
- Check for address conflicts (multiple devices same address)

### "Permission denied" error
```bash
# Add user to i2c group:
sudo usermod -a -G i2c $USER
# Log out and back in, or:
newgrp i2c
```

### Bus appears hung
```bash
# Reset I2C bus:
sudo rmmod i2c_bcm2835
sudo modprobe i2c_bcm2835

# Or reboot:
sudo reboot
```

## How It Works

The scanner tries to communicate with each possible I2C address (0x08 to 0x77):

```cpp
Wire.begin();

for (uint8_t address = 0x08; address < 0x78; address++) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();
    
    if (error == 0) {
        // Device found at this address!
        std::cout << "0x" << std::hex << address << std::endl;
    }
}
```

## Using This Tool

### Before Connecting New Device
1. Run scanner with NO devices connected
2. Note any existing addresses
3. Connect new device
4. Run scanner again
5. New address = your device's address

### Verifying Device Address
1. Check device datasheet for expected address
2. Run scanner
3. Confirm address matches
4. If different, check A0/A1/A2 jumpers

### Debugging Communication Issues
1. Scanner finds device → Wiring OK, address correct
2. Scanner doesn't find device → Check power, wiring, I2C enable
3. Device found but sketch doesn't work → Check initialization sequence

## Next Steps

After finding device addresses:
- **i2c_bmp280** - Read BMP280 pressure/temperature sensor
- **i2c_mpu6050** - Read MPU6050 accelerometer/gyroscope  
- **i2c_ssd1306** - Display graphics on OLED screen

## References

- [PiPinPP Wire (I2C) API](../../docs/API_REFERENCE.md#wire-i2c)
- [I2C Specification](https://www.nxp.com/docs/en/user-guide/UM10204.pdf)
- [Raspberry Pi I2C](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#i2c)
- [Arduino Wire reference](https://www.arduino.cc/reference/en/language/functions/communication/wire/)

## License

Part of the PiPinPP project - see LICENSE file for details.
