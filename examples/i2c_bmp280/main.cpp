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
 * @file i2c_bmp280/main.cpp
 * @brief BMP280 barometric pressure and temperature sensor example
 * 
 * Demonstrates reading pressure and temperature from the BMP280 sensor
 * using the Wire library. The BMP280 is a high-precision environmental
 * sensor that measures barometric pressure and temperature.
 * 
 * Features demonstrated:
 * - Sensor identification and initialization
 * - Calibration data reading
 * - Compensated temperature calculation
 * - Compensated pressure calculation
 * - Altitude estimation from pressure
 * 
 * Hardware setup:
 * - BMP280 VCC  → Pi 3.3V
 * - BMP280 GND  → Pi GND
 * - BMP280 SDA  → Pi GPIO 2 (Pin 3)
 * - BMP280 SCL  → Pi GPIO 3 (Pin 5)
 * - BMP280 SDO  → GND (address 0x76) or VCC (address 0x77)
 * 
 * Applications:
 * - Weather station
 * - Altitude measurement
 * - Pressure monitoring
 * - Indoor/outdoor sensors
 * 
 * @author PiPinPP Team
 * @version 0.4.0
 * @date 2025-11-05
 */

#include <Wire.hpp>
#include <ArduinoCompat.hpp>
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

// BMP280 I2C addresses
const uint8_t BMP280_ADDR_LOW  = 0x76;  // SDO to GND
const uint8_t BMP280_ADDR_HIGH = 0x77;  // SDO to VCC

// BMP280 Registers
const uint8_t REG_CHIP_ID      = 0xD0;
const uint8_t REG_RESET        = 0xE0;
const uint8_t REG_STATUS       = 0xF3;
const uint8_t REG_CTRL_MEAS    = 0xF4;
const uint8_t REG_CONFIG       = 0xF5;
const uint8_t REG_PRESS_MSB    = 0xF7;
const uint8_t REG_CALIB_START  = 0x88;

// Expected chip ID
const uint8_t CHIP_ID_BMP280 = 0x58;
const uint8_t CHIP_ID_BME280 = 0x60;

// Sea level pressure (adjust for your location)
const float SEA_LEVEL_PRESSURE = 1013.25;  // hPa

// Calibration data
struct CalibrationData {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
};

CalibrationData calib;
uint8_t bmp280_addr = BMP280_ADDR_LOW;
int32_t t_fine;  // Used in compensation formulas

bool detectBMP280() {
    cout << "Scanning for BMP280...\n";
    
    // Try primary address
    if (Wire.exists(BMP280_ADDR_LOW)) {
        bmp280_addr = BMP280_ADDR_LOW;
        cout << "Found device at 0x" << hex << uppercase << (int)bmp280_addr << dec << "\n";
        return true;
    }
    
    // Try secondary address
    if (Wire.exists(BMP280_ADDR_HIGH)) {
        bmp280_addr = BMP280_ADDR_HIGH;
        cout << "Found device at 0x" << hex << uppercase << (int)bmp280_addr << dec << "\n";
        return true;
    }
    
    cerr << "ERROR: BMP280 not found at 0x76 or 0x77\n";
    return false;
}

bool readCalibrationData() {
    cout << "Reading calibration data...\n";
    
    uint8_t calib_data[24];
    int bytesRead = Wire.readRegisters(bmp280_addr, REG_CALIB_START, calib_data, 24);
    
    if (bytesRead != 24) {
        cerr << "ERROR: Failed to read calibration data\n";
        return false;
    }
    
    // Parse calibration data (little-endian)
    calib.dig_T1 = (calib_data[1] << 8) | calib_data[0];
    calib.dig_T2 = (calib_data[3] << 8) | calib_data[2];
    calib.dig_T3 = (calib_data[5] << 8) | calib_data[4];
    calib.dig_P1 = (calib_data[7] << 8) | calib_data[6];
    calib.dig_P2 = (calib_data[9] << 8) | calib_data[8];
    calib.dig_P3 = (calib_data[11] << 8) | calib_data[10];
    calib.dig_P4 = (calib_data[13] << 8) | calib_data[12];
    calib.dig_P5 = (calib_data[15] << 8) | calib_data[14];
    calib.dig_P6 = (calib_data[17] << 8) | calib_data[16];
    calib.dig_P7 = (calib_data[19] << 8) | calib_data[18];
    calib.dig_P8 = (calib_data[21] << 8) | calib_data[20];
    calib.dig_P9 = (calib_data[23] << 8) | calib_data[22];
    
    cout << "Calibration data loaded successfully\n";
    return true;
}

bool initBMP280() {
    cout << "Initializing BMP280...\n";
    
    // Verify chip ID
    int chip_id = Wire.readRegister(bmp280_addr, REG_CHIP_ID);
    if (chip_id < 0) {
        cerr << "ERROR: Failed to read chip ID\n";
        return false;
    }
    
    cout << "Chip ID: 0x" << hex << uppercase << setw(2) << setfill('0') << chip_id << dec << "\n";
    
    if (chip_id == CHIP_ID_BME280) {
        cout << "Note: Detected BME280 (includes humidity sensor)\n";
    } else if (chip_id != CHIP_ID_BMP280) {
        cerr << "WARNING: Unexpected chip ID (expected 0x58)\n";
    }
    
    // Read calibration data
    if (!readCalibrationData()) {
        return false;
    }
    
    // Configure sensor
    // ctrl_meas: oversampling x16 for temp and pressure, normal mode
    uint8_t ctrl_meas = (0b101 << 5) | (0b101 << 2) | 0b11;
    if (!Wire.writeRegister(bmp280_addr, REG_CTRL_MEAS, ctrl_meas)) {
        cerr << "ERROR: Failed to configure measurement control\n";
        return false;
    }
    
    // config: standby 500ms, filter off, SPI disabled
    uint8_t config = (0b100 << 5) | (0b000 << 2);
    if (!Wire.writeRegister(bmp280_addr, REG_CONFIG, config)) {
        cerr << "ERROR: Failed to configure device\n";
        return false;
    }
    
    delay(100);  // Wait for configuration
    
    cout << "BMP280 initialized successfully!\n\n";
    return true;
}

float compensateTemperature(int32_t adc_T) {
    // Formula from BMP280 datasheet
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)calib.dig_T1 << 1))) * ((int32_t)calib.dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)calib.dig_T1)) * ((adc_T >> 4) - ((int32_t)calib.dig_T1))) >> 12) * ((int32_t)calib.dig_T3)) >> 14;
    
    t_fine = var1 + var2;
    
    float T = (t_fine * 5 + 128) >> 8;
    return T / 100.0;
}

float compensatePressure(int32_t adc_P) {
    // Formula from BMP280 datasheet
    int64_t var1 = ((int64_t)t_fine) - 128000;
    int64_t var2 = var1 * var1 * (int64_t)calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)calib.dig_P3) >> 8) + ((var1 * (int64_t)calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)calib.dig_P1) >> 33;
    
    if (var1 == 0) {
        return 0;  // Avoid division by zero
    }
    
    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)calib.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)calib.dig_P7) << 4);
    
    return (float)p / 256.0;
}

bool readSensorData(float& temperature, float& pressure) {
    // Read all data registers (pressure + temperature)
    uint8_t data[6];
    int bytesRead = Wire.readRegisters(bmp280_addr, REG_PRESS_MSB, data, 6);
    
    if (bytesRead != 6) {
        cerr << "ERROR: Failed to read sensor data\n";
        return false;
    }
    
    // Parse raw ADC values (20-bit)
    int32_t adc_P = ((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | ((uint32_t)data[2] >> 4);
    int32_t adc_T = ((uint32_t)data[3] << 12) | ((uint32_t)data[4] << 4) | ((uint32_t)data[5] >> 4);
    
    // Apply compensation formulas
    temperature = compensateTemperature(adc_T);
    pressure = compensatePressure(adc_P);
    
    return true;
}

float calculateAltitude(float pressure) {
    // Barometric formula
    return 44330.0 * (1.0 - pow(pressure / SEA_LEVEL_PRESSURE / 100.0, 0.1903));
}

void displayWeatherData(float temperature, float pressure, float altitude) {
    cout << "\r";  // Carriage return for same-line update
    
    cout << "Temperature: " << fixed << setprecision(2) << setw(6) << temperature << "°C  |  ";
    cout << "Pressure: " << fixed << setprecision(2) << setw(8) << pressure / 100.0 << " hPa  |  ";
    cout << "Altitude: " << fixed << setprecision(1) << setw(7) << altitude << " m";
    
    cout << flush;
}

int main() {
    cout << "\n";
    cout << "╔══════════════════════════════════════════════════════════════╗\n";
    cout << "║        BMP280 Pressure & Temperature Sensor Demo            ║\n";
    cout << "║                                                              ║\n";
    cout << "║  High-precision barometric pressure measurement              ║\n";
    cout << "║  Version 0.4.0 - November 5, 2025                           ║\n";
    cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
    
    // Initialize I2C
    if (!Wire.begin()) {
        cerr << "ERROR: Failed to initialize I2C\n";
        return 1;
    }
    
    cout << "I2C initialized\n\n";
    
    // Detect and initialize BMP280
    if (!detectBMP280()) {
        cerr << "\nTroubleshooting:\n";
        cerr << "  1. Check wiring (SDA=GPIO2, SCL=GPIO3)\n";
        cerr << "  2. Verify power connections (3.3V, GND)\n";
        cerr << "  3. Run i2c_scanner to detect devices\n";
        return 1;
    }
    
    if (!initBMP280()) {
        return 1;
    }
    
    // Main loop
    cout << "Reading sensor data (Ctrl+C to stop)...\n\n";
    cout << "Legend:\n";
    cout << "  Temperature: Ambient temperature in Celsius\n";
    cout << "  Pressure:    Atmospheric pressure in hectopascals (hPa)\n";
    cout << "  Altitude:    Estimated altitude above sea level (meters)\n\n";
    cout << "Note: Altitude calculation uses standard sea level pressure (" 
         << SEA_LEVEL_PRESSURE << " hPa)\n";
    cout << "      Adjust SEA_LEVEL_PRESSURE in code for accurate altitude\n\n";
    
    int sampleCount = 0;
    float temp_sum = 0;
    float pressure_sum = 0;
    
    while (true) {
        float temperature, pressure;
        
        if (readSensorData(temperature, pressure)) {
            float altitude = calculateAltitude(pressure);
            
            displayWeatherData(temperature, pressure, altitude);
            
            temp_sum += temperature;
            pressure_sum += pressure;
            sampleCount++;
            
            // Every 50 samples (~25 seconds), show statistics
            if (sampleCount % 50 == 0) {
                float temp_avg = temp_sum / sampleCount;
                float pressure_avg = pressure_sum / sampleCount;
                
                cout << "\n\n";
                cout << "Statistics (last " << sampleCount << " samples):\n";
                cout << "  Average temperature: " << fixed << setprecision(2) << temp_avg << "°C\n";
                cout << "  Average pressure:    " << fixed << setprecision(2) << pressure_avg / 100.0 << " hPa\n";
                cout << "\nPress Ctrl+C to stop...\n\n";
                
                // Reset for next period
                temp_sum = 0;
                pressure_sum = 0;
                sampleCount = 0;
            }
        }
        
        delay(500);  // 2 Hz update rate
    }
    
    return 0;
}
