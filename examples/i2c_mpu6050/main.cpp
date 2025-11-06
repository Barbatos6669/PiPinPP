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
 * @file i2c_mpu6050/main.cpp
 * @brief MPU6050 6-axis IMU sensor example
 * 
 * Demonstrates reading accelerometer and gyroscope data from the MPU6050
 * using the Wire library. The MPU6050 is a popular 6-DOF IMU that combines
 * a 3-axis accelerometer and 3-axis gyroscope.
 * 
 * Features demonstrated:
 * - I2C device initialization
 * - Register-based communication
 * - Reading multiple bytes (burst read)
 * - Data conversion (16-bit signed values)
 * - Continuous sensor reading
 * 
 * Hardware setup:
 * - MPU6050 VCC  → Pi 3.3V
 * - MPU6050 GND  → Pi GND
 * - MPU6050 SDA  → Pi GPIO 2 (Pin 3)
 * - MPU6050 SCL  → Pi GPIO 3 (Pin 5)
 * - MPU6050 AD0  → GND (address 0x68) or VCC (address 0x69)
 * 
 * Common issues:
 * - Wrong address: Try both 0x68 and 0x69
 * - Pull-ups needed: Most boards have built-in pull-ups
 * - Power: MPU6050 works at 3.3V
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

// MPU6050 I2C address (AD0=LOW)
const uint8_t MPU6050_ADDR = 0x68;

// MPU6050 Registers
const uint8_t REG_PWR_MGMT_1   = 0x6B;  // Power management
const uint8_t REG_ACCEL_XOUT_H = 0x3B;  // Accelerometer X high byte
const uint8_t REG_TEMP_OUT_H   = 0x41;  // Temperature high byte
const uint8_t REG_GYRO_XOUT_H  = 0x43;  // Gyroscope X high byte
const uint8_t REG_WHO_AM_I     = 0x75;  // Device ID

// Sensor scales
const float ACCEL_SCALE = 16384.0;  // ±2g range
const float GYRO_SCALE = 131.0;     // ±250°/s range

struct SensorData {
    // Accelerometer (g)
    float accel_x;
    float accel_y;
    float accel_z;
    
    // Gyroscope (°/s)
    float gyro_x;
    float gyro_y;
    float gyro_z;
    
    // Temperature (°C)
    float temperature;
};

bool initMPU6050() {
    cout << "Initializing MPU6050...\n";
    
    // Check WHO_AM_I register
    int whoami = Wire.readRegister(MPU6050_ADDR, REG_WHO_AM_I);
    if (whoami < 0) {
        cerr << "ERROR: Failed to read WHO_AM_I register\n";
        cerr << "Check connections and I2C address (try 0x69 if 0x68 fails)\n";
        return false;
    }
    
    cout << "WHO_AM_I: 0x" << hex << uppercase << setw(2) << setfill('0') << whoami << dec << "\n";
    
    if (whoami != 0x68 && whoami != 0x69) {
        cerr << "WARNING: Unexpected WHO_AM_I value (expected 0x68 or 0x69)\n";
    }
    
    // Wake up MPU6050 (clear sleep bit in power management register)
    if (!Wire.writeRegister(MPU6050_ADDR, REG_PWR_MGMT_1, 0x00)) {
        cerr << "ERROR: Failed to wake up MPU6050\n";
        return false;
    }
    
    delay(100);  // Wait for sensor to stabilize
    
    cout << "MPU6050 initialized successfully!\n\n";
    return true;
}

bool readSensorData(SensorData& data) {
    // Read all sensor data in one burst (14 bytes)
    // Accel X/Y/Z (6 bytes) + Temp (2 bytes) + Gyro X/Y/Z (6 bytes)
    uint8_t buffer[14];
    
    int bytesRead = Wire.readRegisters(MPU6050_ADDR, REG_ACCEL_XOUT_H, buffer, 14);
    
    if (bytesRead != 14) {
        cerr << "ERROR: Failed to read sensor data (got " << bytesRead << " bytes)\n";
        return false;
    }
    
    // Convert raw data to 16-bit signed values
    int16_t accel_x_raw = (buffer[0] << 8) | buffer[1];
    int16_t accel_y_raw = (buffer[2] << 8) | buffer[3];
    int16_t accel_z_raw = (buffer[4] << 8) | buffer[5];
    int16_t temp_raw    = (buffer[6] << 8) | buffer[7];
    int16_t gyro_x_raw  = (buffer[8] << 8) | buffer[9];
    int16_t gyro_y_raw  = (buffer[10] << 8) | buffer[11];
    int16_t gyro_z_raw  = (buffer[12] << 8) | buffer[13];
    
    // Convert to real units
    data.accel_x = accel_x_raw / ACCEL_SCALE;
    data.accel_y = accel_y_raw / ACCEL_SCALE;
    data.accel_z = accel_z_raw / ACCEL_SCALE;
    
    data.gyro_x = gyro_x_raw / GYRO_SCALE;
    data.gyro_y = gyro_y_raw / GYRO_SCALE;
    data.gyro_z = gyro_z_raw / GYRO_SCALE;
    
    // Temperature = (raw / 340.0) + 36.53
    data.temperature = (temp_raw / 340.0) + 36.53;
    
    return true;
}

void displaySensorData(const SensorData& data) {
    cout << "\r";  // Carriage return for same-line update
    
    cout << "Accel: ";
    cout << "X:" << fixed << setprecision(2) << setw(6) << data.accel_x << "g ";
    cout << "Y:" << fixed << setprecision(2) << setw(6) << data.accel_y << "g ";
    cout << "Z:" << fixed << setprecision(2) << setw(6) << data.accel_z << "g  |  ";
    
    cout << "Gyro: ";
    cout << "X:" << fixed << setprecision(1) << setw(7) << data.gyro_x << "°/s ";
    cout << "Y:" << fixed << setprecision(1) << setw(7) << data.gyro_y << "°/s ";
    cout << "Z:" << fixed << setprecision(1) << setw(7) << data.gyro_z << "°/s  |  ";
    
    cout << "Temp: " << fixed << setprecision(1) << data.temperature << "°C";
    
    cout << flush;
}

void calculateTiltAngles(const SensorData& data) {
    // Calculate tilt angles from accelerometer
    float roll = atan2(data.accel_y, data.accel_z) * RAD_TO_DEG;
    float pitch = atan2(-data.accel_x, sqrt(data.accel_y * data.accel_y + data.accel_z * data.accel_z)) * RAD_TO_DEG;
    
    cout << "\n\nTilt Angles:\n";
    cout << "  Roll:  " << fixed << setprecision(1) << setw(6) << roll << "°\n";
    cout << "  Pitch: " << fixed << setprecision(1) << setw(6) << pitch << "°\n";
}

void detectMotion(const SensorData& data) {
    // Calculate total acceleration magnitude
    float accel_mag = sqrt(data.accel_x * data.accel_x + 
                          data.accel_y * data.accel_y + 
                          data.accel_z * data.accel_z);
    
    // Calculate total rotation rate magnitude
    float gyro_mag = sqrt(data.gyro_x * data.gyro_x + 
                         data.gyro_y * data.gyro_y + 
                         data.gyro_z * data.gyro_z);
    
    cout << "\n\nMotion Detection:\n";
    cout << "  Acceleration magnitude: " << fixed << setprecision(2) << accel_mag << "g\n";
    cout << "  Rotation magnitude: " << fixed << setprecision(1) << gyro_mag << "°/s\n";
    
    if (gyro_mag > 50.0) {
        cout << "  Status: ROTATING FAST!\n";
    } else if (gyro_mag > 10.0) {
        cout << "  Status: Moving\n";
    } else {
        cout << "  Status: Still\n";
    }
}

int main() {
    cout << "\n";
    cout << "╔══════════════════════════════════════════════════════════════╗\n";
    cout << "║           MPU6050 IMU Sensor Demo (I2C)                     ║\n";
    cout << "║                                                              ║\n";
    cout << "║  6-axis accelerometer + gyroscope demonstration              ║\n";
    cout << "║  Version 0.4.0 - November 5, 2025                           ║\n";
    cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
    
    // Initialize I2C
    if (!Wire.begin()) {
        cerr << "ERROR: Failed to initialize I2C\n";
        return 1;
    }
    
    Wire.setClock(400000);  // 400kHz fast mode
    cout << "I2C initialized at 400 kHz\n\n";
    
    // Check if MPU6050 exists
    if (!Wire.exists(MPU6050_ADDR)) {
        cerr << "ERROR: MPU6050 not found at address 0x" << hex << (int)MPU6050_ADDR << dec << "\n";
        cerr << "\nTroubleshooting:\n";
        cerr << "  1. Check wiring (SDA=GPIO2, SCL=GPIO3)\n";
        cerr << "  2. Verify power connections (3.3V, GND)\n";
        cerr << "  3. Try address 0x69 if AD0 is HIGH\n";
        cerr << "  4. Run i2c_scanner example to detect devices\n";
        return 1;
    }
    
    // Initialize MPU6050
    if (!initMPU6050()) {
        return 1;
    }
    
    // Main loop
    cout << "Reading sensor data (Ctrl+C to stop)...\n\n";
    cout << "Legend:\n";
    cout << "  Accel: Acceleration in g (1g = 9.8 m/s²)\n";
    cout << "  Gyro:  Rotation rate in degrees/second\n";
    cout << "  Temp:  Temperature in Celsius\n\n";
    
    int sampleCount = 0;
    SensorData data;
    
    while (true) {
        if (readSensorData(data)) {
            displaySensorData(data);
            
            sampleCount++;
            
            // Every 50 samples (~5 seconds), show analysis
            if (sampleCount % 50 == 0) {
                calculateTiltAngles(data);
                detectMotion(data);
                cout << "\nPress Ctrl+C to stop...\n\n";
            }
        }
        
        delay(100);  // 10 Hz update rate
    }
    
    return 0;
}
