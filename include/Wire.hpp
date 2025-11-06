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
 * @file Wire.hpp
 * @brief Arduino-inspired I2C communication library using Linux i2c-dev
 * @author Barbatos6669
 * @date 2025
 * 
 * Provides Wire library API compatible with Arduino for I2C master communication.
 * Uses Linux kernel i2c-dev interface for hardware I2C access.
 * 
 * Features:
 * - Arduino Wire API compatibility
 * - I2C master mode
 * - 7-bit and 10-bit addressing support
 * - Configurable clock speed
 * - Read/write operations
 * - Register-based access helpers
 * 
 * @author PiPinPP Team
 * @version 0.4.0
 * @date 2025-11-05
 */

#ifndef PIPINPP_WIRE_HPP
#define PIPINPP_WIRE_HPP

#include <cstdint>
#include <cstddef>
#include <vector>
#include <mutex>
#include <string>

namespace pipinpp {

/**
 * @class WireClass
 * @brief Arduino-compatible I2C master communication class
 * 
 * Implements the Arduino Wire library API for I2C communication on Raspberry Pi.
 * Uses Linux i2c-dev driver for hardware I2C access.
 * 
 * Standard usage pattern:
 * @code
 * Wire.begin();                          // Initialize as master
 * Wire.beginTransmission(0x68);          // Start transmission to device
 * Wire.write(0x00);                      // Write register address
 * Wire.write(0x42);                      // Write data
 * Wire.endTransmission();                // End transmission
 * 
 * Wire.requestFrom(0x68, 2);             // Request 2 bytes
 * if (Wire.available()) {
 *     uint8_t data = Wire.read();        // Read received data
 * }
 * @endcode
 */
class WireClass {
public:
    /**
     * @brief Construct Wire object
     * 
     * Creates an I2C interface object. Call begin() to initialize.
     */
    WireClass();
    
    /**
     * @brief Destroy Wire object
     * 
     * Closes I2C device if open.
     */
    ~WireClass();
    
    // Prevent copying
    WireClass(const WireClass&) = delete;
    WireClass& operator=(const WireClass&) = delete;
    
    /* ------------------------------------------------------------ */
    /*                   INITIALIZATION                             */
    /* ------------------------------------------------------------ */
    
    /**
     * @brief Initialize I2C interface as master
     * 
     * Opens the I2C device and initializes for master mode operation.
     * Default I2C bus is determined automatically (/dev/i2c-1 on most Pi models).
     * 
     * @return true if successful, false on error
     * 
     * @example
     * Wire.begin();  // Initialize I2C master
     */
    bool begin();
    
    /**
     * @brief Initialize I2C interface with specific bus number
     * 
     * Opens a specific I2C bus device.
     * 
     * @param busNumber I2C bus number (e.g., 1 for /dev/i2c-1)
     * @return true if successful, false on error
     * 
     * @example
     * Wire.begin(1);  // Use /dev/i2c-1
     * Wire.begin(20); // Use /dev/i2c-20 (Pi 5)
     */
    bool begin(int busNumber);
    
    /**
     * @brief Close I2C interface
     * 
     * Closes the I2C device file descriptor.
     */
    void end();
    
    /* ------------------------------------------------------------ */
    /*                   TRANSMISSION                               */
    /* ------------------------------------------------------------ */
    
    /**
     * @brief Begin transmission to I2C device
     * 
     * Starts a transmission to the specified I2C slave address.
     * Subsequent write() calls queue data to send.
     * Call endTransmission() to actually send the data.
     * 
     * @param address 7-bit I2C slave address (0x00-0x7F)
     * 
     * @example
     * Wire.beginTransmission(0x68);  // MPU6050 address
     * Wire.write(0x6B);              // Power management register
     * Wire.write(0x00);              // Wake up
     * Wire.endTransmission();
     */
    void beginTransmission(uint8_t address);
    
    /**
     * @brief End transmission to I2C device
     * 
     * Sends queued data to the I2C slave and releases the bus.
     * 
     * @param sendStop true to send STOP condition (default), false for repeated START
     * @return 0 = success
     *         1 = data too long for transmit buffer
     *         2 = NACK on transmit of address
     *         3 = NACK on transmit of data
     *         4 = other error
     * 
     * @example
     * Wire.beginTransmission(0x68);
     * Wire.write(0x75);  // WHO_AM_I register
     * uint8_t error = Wire.endTransmission();
     * if (error != 0) {
     *     // Handle error
     * }
     */
    uint8_t endTransmission(bool sendStop = true);
    
    /**
     * @brief End transmission (simplified)
     * 
     * Always sends STOP condition.
     * 
     * @return Error code (same as endTransmission(true))
     */
    uint8_t endTransmission();
    
    /* ------------------------------------------------------------ */
    /*                   WRITING DATA                               */
    /* ------------------------------------------------------------ */
    
    /**
     * @brief Write a single byte to I2C device
     * 
     * Queues a byte for transmission. Must be called between
     * beginTransmission() and endTransmission().
     * 
     * @param data Byte to write
     * @return 1 if successful, 0 if buffer full
     * 
     * @example
     * Wire.beginTransmission(0x68);
     * Wire.write(0x6B);  // Register address
     * Wire.write(0x00);  // Register value
     * Wire.endTransmission();
     */
    size_t write(uint8_t data);
    
    /**
     * @brief Write multiple bytes to I2C device
     * 
     * Queues multiple bytes for transmission.
     * 
     * @param data Pointer to data buffer
     * @param length Number of bytes to write
     * @return Number of bytes queued (0 if buffer full)
     * 
     * @example
     * uint8_t config[] = {0x6B, 0x00, 0x1C, 0x10};
     * Wire.beginTransmission(0x68);
     * Wire.write(config, sizeof(config));
     * Wire.endTransmission();
     */
    size_t write(const uint8_t* data, size_t length);
    
    /* ------------------------------------------------------------ */
    /*                   READING DATA                               */
    /* ------------------------------------------------------------ */
    
    /**
     * @brief Request bytes from I2C device
     * 
     * Requests data from an I2C slave device. Data becomes available
     * for read() after this call completes.
     * 
     * @param address 7-bit I2C slave address
     * @param quantity Number of bytes to request
     * @param sendStop true to send STOP after read (default)
     * @return Number of bytes actually received
     * 
     * @example
     * Wire.requestFrom(0x68, 6);  // Request 6 bytes from MPU6050
     * while (Wire.available()) {
     *     uint8_t data = Wire.read();
     *     // Process data
     * }
     */
    size_t requestFrom(uint8_t address, size_t quantity, bool sendStop = true);
    
    /**
     * @brief Request bytes from I2C device (simplified)
     * 
     * Always sends STOP after read.
     * 
     * @param address 7-bit I2C slave address
     * @param quantity Number of bytes to request
     * @return Number of bytes received
     */
    size_t requestFrom(uint8_t address, size_t quantity);
    
    /**
     * @brief Check available received bytes
     * 
     * Returns number of bytes available in receive buffer after requestFrom().
     * 
     * @return Number of bytes available to read
     * 
     * @example
     * Wire.requestFrom(0x68, 2);
     * if (Wire.available() >= 2) {
     *     uint8_t high = Wire.read();
     *     uint8_t low = Wire.read();
     * }
     */
    size_t available();
    
    /**
     * @brief Read a single byte from receive buffer
     * 
     * Reads one byte from the receive buffer (populated by requestFrom()).
     * 
     * @return Received byte, or -1 if no data available
     * 
     * @example
     * Wire.requestFrom(0x68, 1);
     * if (Wire.available()) {
     *     int whoami = Wire.read();
     * }
     */
    int read();
    
    /**
     * @brief Peek at next byte without removing it
     * 
     * Returns the next byte from receive buffer without removing it.
     * Subsequent read() or peek() will return the same byte.
     * 
     * @return Next byte, or -1 if no data available
     * 
     * @example
     * if (Wire.peek() == 0xFF) {
     *     // Special handling for 0xFF
     * }
     * uint8_t data = Wire.read();  // Same byte
     */
    int peek();
    
    /* ------------------------------------------------------------ */
    /*                   CONFIGURATION                              */
    /* ------------------------------------------------------------ */
    
    /**
     * @brief Set I2C clock speed
     * 
     * Sets the I2C bus clock frequency. Standard speeds:
     * - 100000 = 100 kHz (standard mode)
     * - 400000 = 400 kHz (fast mode)
     * - 1000000 = 1 MHz (fast mode plus)
     * - 3400000 = 3.4 MHz (high speed mode) - if supported
     * 
     * @param frequency Clock frequency in Hz
     * @return true if successful, false on error
     * 
     * @example
     * Wire.begin();
     * Wire.setClock(400000);  // 400 kHz fast mode
     */
    bool setClock(uint32_t frequency);
    
    /**
     * @brief Get current I2C clock speed
     * 
     * @return Current clock frequency in Hz
     */
    uint32_t getClock() const;
    
    /* ------------------------------------------------------------ */
    /*                   HELPER FUNCTIONS                           */
    /* ------------------------------------------------------------ */
    
    /**
     * @brief Read a single byte from device register
     * 
     * Helper function to read one byte from a specific register.
     * Equivalent to: beginTransmission() + write(reg) + endTransmission() + requestFrom()
     * 
     * @param address I2C device address
     * @param reg Register address
     * @return Register value, or -1 on error
     * 
     * @example
     * int whoami = Wire.readRegister(0x68, 0x75);  // Read WHO_AM_I
     */
    int readRegister(uint8_t address, uint8_t reg);
    
    /**
     * @brief Write a single byte to device register
     * 
     * Helper function to write one byte to a specific register.
     * 
     * @param address I2C device address
     * @param reg Register address
     * @param value Value to write
     * @return true if successful, false on error
     * 
     * @example
     * Wire.writeRegister(0x68, 0x6B, 0x00);  // Wake up MPU6050
     */
    bool writeRegister(uint8_t address, uint8_t reg, uint8_t value);
    
    /**
     * @brief Read multiple bytes from device registers
     * 
     * Helper function to read multiple bytes starting from a register.
     * 
     * @param address I2C device address
     * @param reg Starting register address
     * @param buffer Buffer to store read data
     * @param length Number of bytes to read
     * @return Number of bytes actually read, or -1 on error
     * 
     * @example
     * uint8_t accel[6];
     * Wire.readRegisters(0x68, 0x3B, accel, 6);  // Read accel X, Y, Z
     */
    int readRegisters(uint8_t address, uint8_t reg, uint8_t* buffer, size_t length);
    
    /**
     * @brief Scan I2C bus for devices
     * 
     * Scans all possible 7-bit addresses (0x03-0x77) and returns list of
     * addresses that respond. Useful for device discovery.
     * 
     * @return Vector of responding I2C addresses
     * 
     * @example
     * auto devices = Wire.scan();
     * for (uint8_t addr : devices) {
     *     printf("Found device at 0x%02X\n", addr);
     * }
     */
    std::vector<uint8_t> scan();
    
    /**
     * @brief Check if device exists at address
     * 
     * Tests if an I2C device responds at the given address.
     * 
     * @param address I2C address to test
     * @return true if device responds, false otherwise
     * 
     * @example
     * if (Wire.exists(0x68)) {
     *     // MPU6050 is connected
     * }
     */
    bool exists(uint8_t address);
    
private:
    int fd_;                           ///< I2C device file descriptor
    int busNumber_;                    ///< I2C bus number
    uint32_t clockFrequency_;          ///< Current clock frequency (Hz)
    uint8_t txAddress_;                ///< Current transmission address
    std::vector<uint8_t> txBuffer_;    ///< Transmit buffer
    std::vector<uint8_t> rxBuffer_;    ///< Receive buffer
    size_t rxIndex_;                   ///< Current read position in rxBuffer
    std::mutex mutex_;                 ///< Thread safety mutex
    
    static constexpr size_t BUFFER_SIZE = 256;  ///< Maximum buffer size
    
    /**
     * @brief Detect default I2C bus number
     * 
     * Auto-detects the primary I2C bus by checking which device exists.
     * Pi 0-4 typically use /dev/i2c-1, Pi 5 uses /dev/i2c-20 or /dev/i2c-21.
     * 
     * @return Detected bus number, or -1 if none found
     */
    int detectBusNumber();
    
    /**
     * @brief Set I2C slave address for next operation
     * 
     * @param address I2C slave address
     * @return true if successful, false on error
     */
    bool setSlaveAddress(uint8_t address);
};

// Global Wire instance (Arduino compatibility)
extern WireClass Wire;

} // namespace pipinpp

// Allow usage without namespace (Arduino compatibility)
using pipinpp::Wire;
using pipinpp::WireClass;

#endif // PIPINPP_WIRE_HPP
