/*
 * Copyright (c) 2025 HobbyHacker
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file i2c_scanner/main.cpp
 * @brief I2C bus scanner - finds all connected I2C devices
 * 
 * This example demonstrates the Wire library by scanning the I2C bus
 * for connected devices. It's a useful diagnostic tool for:
 * - Verifying I2C hardware is working
 * - Finding device addresses
 * - Troubleshooting connections
 * 
 * Common I2C devices and their addresses:
 * - 0x20-0x27: MCP23017 I/O expander
 * - 0x48-0x4F: ADS1115 ADC, PCF8591 ADC
 * - 0x50-0x57: AT24C EEPROM
 * - 0x68: MPU6050/MPU9250 IMU, DS1307/DS3231 RTC
 * - 0x76-0x77: BMP280/BME280 pressure/temp/humidity
 * - 0x3C, 0x3D: SSD1306 OLED display
 * 
 * Hardware setup:
 * - Connect SDA to GPIO 2 (Pin 3)
 * - Connect SCL to GPIO 3 (Pin 5)
 * - Connect device GND to Pi GND
 * - Connect device VCC to Pi 3.3V (check device specs!)
 * - Use pull-up resistors (1.8kΩ-4.7kΩ) if not on board
 * 
 * @author PiPinPP Team
 * @version 0.4.0
 * @date 2025-11-05
 */

#include <Wire.hpp>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <unistd.h>

using namespace std;

// Known I2C device identification
map<uint8_t, string> knownDevices = {
    // I/O Expanders
    {0x20, "MCP23017 I/O Expander"},
    {0x21, "MCP23017 I/O Expander"},
    {0x22, "MCP23017 I/O Expander"},
    {0x23, "MCP23017 I/O Expander"},
    {0x24, "MCP23017 I/O Expander"},
    {0x25, "MCP23017 I/O Expander"},
    {0x26, "MCP23017 I/O Expander"},
    {0x27, "MCP23017 I/O Expander (or PCF8574 LCD)"},
    
    // ADCs
    {0x48, "ADS1115 ADC / PCF8591 ADC"},
    {0x49, "ADS1115 ADC"},
    {0x4A, "ADS1115 ADC"},
    {0x4B, "ADS1115 ADC"},
    
    // EEPROM
    {0x50, "AT24C EEPROM"},
    {0x51, "AT24C EEPROM"},
    {0x52, "AT24C EEPROM"},
    {0x53, "AT24C EEPROM / ADXL345 Accelerometer"},
    {0x54, "AT24C EEPROM"},
    {0x55, "AT24C EEPROM"},
    {0x56, "AT24C EEPROM"},
    {0x57, "AT24C EEPROM"},
    
    // Displays
    {0x3C, "SSD1306 OLED Display"},
    {0x3D, "SSD1306 OLED Display"},
    
    // Sensors
    {0x68, "MPU6050/MPU9250 IMU / DS1307/DS3231 RTC"},
    {0x69, "MPU6050/MPU9250 IMU"},
    {0x76, "BMP280/BME280 Pressure/Temp/Humidity"},
    {0x77, "BMP280/BME280 Pressure/Temp/Humidity"},
    
    // Light sensors
    {0x29, "TSL2561 Light Sensor / VL53L0X Distance"},
    {0x23, "BH1750 Light Sensor"},
    
    // Motor drivers
    {0x60, "PCA9685 PWM Driver"},
    {0x61, "PCA9685 PWM Driver"},
    {0x62, "PCA9685 PWM Driver"},
    {0x63, "PCA9685 PWM Driver"},
    {0x64, "PCA9685 PWM Driver"},
    {0x65, "PCA9685 PWM Driver"},
    {0x66, "PCA9685 PWM Driver"},
    {0x67, "PCA9685 PWM Driver"},
};

void printHeader() {
    cout << "\n";
    cout << "╔══════════════════════════════════════════════════════════════╗\n";
    cout << "║              PiPinPP I2C Bus Scanner                         ║\n";
    cout << "║                                                              ║\n";
    cout << "║  Scans I2C bus for connected devices                         ║\n";
    cout << "║  Version 0.4.0 - November 5, 2025                           ║\n";
    cout << "╚══════════════════════════════════════════════════════════════╝\n";
    cout << "\n";
}

void scanBus() {
    cout << "Scanning I2C bus...\n\n";
    
    // Use Wire library's scan function
    auto devices = Wire.scan();
    
    if (devices.empty()) {
        cout << "No I2C devices found!\n\n";
        cout << "Troubleshooting:\n";
        cout << "  1. Check if I2C is enabled: ls /dev/i2c*\n";
        cout << "  2. Verify wiring (SDA=GPIO2/Pin3, SCL=GPIO3/Pin5)\n";
        cout << "  3. Check pull-up resistors (1.8kΩ-4.7kΩ)\n";
        cout << "  4. Ensure device has power (3.3V or 5V as required)\n";
        cout << "  5. Try: sudo i2cdetect -y 1 (or 20/21 on Pi 5)\n";
        return;
    }
    
    // Display results
    cout << "Found " << devices.size() << " device(s):\n\n";
    cout << "┌─────────┬────────────────────────────────────────────────┐\n";
    cout << "│ Address │ Possible Device                                │\n";
    cout << "├─────────┼────────────────────────────────────────────────┤\n";
    
    for (uint8_t addr : devices) {
        cout << "│  0x" << hex << uppercase << setw(2) << setfill('0') 
             << static_cast<int>(addr) << "  │ ";
        
        // Look up known device
        auto it = knownDevices.find(addr);
        if (it != knownDevices.end()) {
            cout << left << setw(46) << setfill(' ') << it->second;
        } else {
            cout << left << setw(46) << setfill(' ') << "Unknown device";
        }
        cout << " │\n";
    }
    
    cout << "└─────────┴────────────────────────────────────────────────┘\n\n";
}

void detailedScan() {
    cout << "Detailed I2C address map:\n\n";
    cout << "    ";
    for (int col = 0; col < 16; col++) {
        cout << " " << hex << uppercase << col;
    }
    cout << "\n";
    cout << "    ────────────────────────────────────────────────\n";
    
    for (int row = 0; row < 8; row++) {
        cout << hex << uppercase << row << "x: ";
        
        for (int col = 0; col < 16; col++) {
            uint8_t addr = (row << 4) | col;
            
            // Skip reserved addresses
            if (addr < 0x03 || addr > 0x77) {
                cout << " --";
                continue;
            }
            
            // Check if device exists
            if (Wire.exists(addr)) {
                cout << " " << setw(2) << setfill('0') << static_cast<int>(addr);
            } else {
                cout << " ..";
            }
        }
        cout << "\n";
    }
    cout << "\n";
    cout << "Legend:\n";
    cout << "  XX = Device found at address 0xXX\n";
    cout << "  .. = No device\n";
    cout << "  -- = Reserved address\n\n";
}

void testDevice(uint8_t address) {
    cout << "\nTesting device at 0x" << hex << uppercase << setw(2) << setfill('0') 
         << static_cast<int>(address) << "...\n\n";
    
    // Try to read WHO_AM_I or ID register (common at 0x00 or 0x75)
    cout << "Reading common ID registers:\n";
    
    int reg00 = Wire.readRegister(address, 0x00);
    int reg75 = Wire.readRegister(address, 0x75);
    int regD0 = Wire.readRegister(address, 0xD0);
    
    cout << "  Reg 0x00: ";
    if (reg00 >= 0) {
        cout << "0x" << hex << uppercase << setw(2) << setfill('0') << reg00;
    } else {
        cout << "read error";
    }
    cout << "\n";
    
    cout << "  Reg 0x75: ";
    if (reg75 >= 0) {
        cout << "0x" << hex << uppercase << setw(2) << setfill('0') << reg75;
        if (reg75 == 0x68 || reg75 == 0x69) {
            cout << " (MPU6050 detected!)";
        }
    } else {
        cout << "read error";
    }
    cout << "\n";
    
    cout << "  Reg 0xD0: ";
    if (regD0 >= 0) {
        cout << "0x" << hex << uppercase << setw(2) << setfill('0') << regD0;
        if (regD0 == 0x58) {
            cout << " (BMP280 detected!)";
        } else if (regD0 == 0x60) {
            cout << " (BME280 detected!)";
        }
    } else {
        cout << "read error";
    }
    cout << "\n\n";
}

int main() {
    printHeader();
    
    // Initialize I2C
    cout << "Initializing I2C interface...\n";
    if (!Wire.begin()) {
        cerr << "ERROR: Failed to initialize I2C!\n";
        cerr << "\n";
        cerr << "Possible causes:\n";
        cerr << "  1. I2C not enabled in raspi-config\n";
        cerr << "  2. No I2C device found (/dev/i2c-*)\n";
        cerr << "  3. Permission denied (try adding user to i2c group)\n";
        cerr << "\n";
        cerr << "Enable I2C:\n";
        cerr << "  sudo raspi-config\n";
        cerr << "  → Interface Options → I2C → Yes\n";
        cerr << "\n";
        cerr << "Or manually:\n";
        cerr << "  sudo modprobe i2c-dev\n";
        cerr << "  ls /dev/i2c*\n";
        return 1;
    }
    
    cout << "I2C initialized successfully!\n\n";
    
    // Set to standard mode (100kHz)
    Wire.setClock(100000);
    cout << "Clock speed: " << (Wire.getClock() / 1000) << " kHz\n\n";
    
    // Perform basic scan
    scanBus();
    
    // Perform detailed scan
    detailedScan();
    
    // Test specific devices
    auto devices = Wire.scan();
    if (!devices.empty()) {
        cout << "Testing first device in detail...\n";
        testDevice(devices[0]);
    }
    
    // Usage examples
    cout << "╔══════════════════════════════════════════════════════════════╗\n";
    cout << "║                     Usage Examples                           ║\n";
    cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
    
    cout << "Reading from an I2C device:\n";
    cout << "────────────────────────────────────────\n";
    cout << "Wire.begin();\n";
    cout << "int value = Wire.readRegister(0x68, 0x75);  // Read WHO_AM_I\n";
    cout << "printf(\"WHO_AM_I: 0x%02X\\n\", value);\n\n";
    
    cout << "Writing to an I2C device:\n";
    cout << "────────────────────────────────────────\n";
    cout << "Wire.writeRegister(0x68, 0x6B, 0x00);  // Wake up MPU6050\n\n";
    
    cout << "Reading multiple bytes:\n";
    cout << "────────────────────────────────────────\n";
    cout << "Wire.beginTransmission(0x68);\n";
    cout << "Wire.write(0x3B);  // Accel X register\n";
    cout << "Wire.endTransmission(false);  // Repeated START\n\n";
    
    cout << "Wire.requestFrom(0x68, 6);  // Read 6 bytes\n";
    cout << "int16_t ax = (Wire.read() << 8) | Wire.read();\n";
    cout << "int16_t ay = (Wire.read() << 8) | Wire.read();\n";
    cout << "int16_t az = (Wire.read() << 8) | Wire.read();\n\n";
    
    cout << "Common devices to try:\n";
    cout << "────────────────────────────────────────\n";
    cout << "  • MPU6050 IMU (0x68 or 0x69)\n";
    cout << "  • BMP280/BME280 sensor (0x76 or 0x77)\n";
    cout << "  • SSD1306 OLED (0x3C or 0x3D)\n";
    cout << "  • DS1307/DS3231 RTC (0x68)\n";
    cout << "  • ADS1115 ADC (0x48-0x4B)\n\n";
    
    cout << "For more examples, see:\n";
    cout << "  • examples/i2c_mpu6050/       (6-axis IMU)\n";
    cout << "  • examples/i2c_bme280/        (Temp/Pressure/Humidity)\n";
    cout << "  • examples/i2c_oled/          (OLED display)\n\n";
    
    return 0;
}
