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
 * @file SPI.hpp
 * @brief Arduino-inspired SPI communication library using Linux spidev
 * @author Barbatos6669
 * @date 2025
 * 
 * Provides SPI master communication compatible with Arduino's SPI library API.
 * Uses Linux kernel spidev interface for hardware SPI access on Raspberry Pi.
 * 
 * Features:
 * - Arduino SPI API compatibility
 * - Hardware SPI (up to 125 MHz on Pi 4/5)
 * - Configurable mode (0-3), bit order, clock speed
 * - Full-duplex communication
 * - Thread-safe operations
 * 
 * @author PiPinPP Team
 * @version 0.3.7
 * @date 2025-11-05
 */

#ifndef PIPINPP_SPI_HPP
#define PIPINPP_SPI_HPP

#include <cstdint>
#include <mutex>
#include <string>

namespace pipinpp {

// Arduino SPI mode constants
constexpr uint8_t SPI_MODE0 = 0;  ///< CPOL=0, CPHA=0
constexpr uint8_t SPI_MODE1 = 1;  ///< CPOL=0, CPHA=1
constexpr uint8_t SPI_MODE2 = 2;  ///< CPOL=1, CPHA=0
constexpr uint8_t SPI_MODE3 = 3;  ///< CPOL=1, CPHA=1

// Note: Bit order constants defined in ArduinoCompat.hpp
// Arduino: MSBFIRST=1, LSBFIRST=0

// Arduino clock divider constants (for compatibility)
constexpr uint8_t SPI_CLOCK_DIV2 = 2;
constexpr uint8_t SPI_CLOCK_DIV4 = 4;
constexpr uint8_t SPI_CLOCK_DIV8 = 8;
constexpr uint8_t SPI_CLOCK_DIV16 = 16;
constexpr uint8_t SPI_CLOCK_DIV32 = 32;
constexpr uint8_t SPI_CLOCK_DIV64 = 64;
constexpr uint8_t SPI_CLOCK_DIV128 = 128;

/**
 * @class SPIClass
 * @brief Arduino-compatible SPI master communication class
 * 
 * Implements the Arduino SPI library API for hardware SPI communication on Raspberry Pi.
 * Uses Linux spidev driver for direct hardware access.
 * 
 * Standard usage pattern:
 * @code
 * SPI.begin();                     // Initialize SPI
 * SPI.setDataMode(SPI_MODE0);      // Set SPI mode
 * SPI.setClockDivider(SPI_CLOCK_DIV8);  // Set clock speed
 * 
 * digitalWrite(CS_PIN, LOW);        // Select device
 * uint8_t response = SPI.transfer(0x42);  // Send/receive data
 * digitalWrite(CS_PIN, HIGH);       // Deselect device
 * 
 * SPI.end();                        // Close SPI
 * @endcode
 */
class SPIClass {
public:
    /**
     * @brief Construct SPI object
     * 
     * Creates an SPI interface object. Call begin() to initialize.
     */
    SPIClass();
    
    /**
     * @brief Destroy SPI object
     * 
     * Closes SPI device if open.
     */
    ~SPIClass();
    
    // Prevent copying
    SPIClass(const SPIClass&) = delete;
    SPIClass& operator=(const SPIClass&) = delete;
    
    /* ------------------------------------------------------------ */
    /*                   INITIALIZATION                             */
    /* ------------------------------------------------------------ */
    
    /**
     * @brief Initialize SPI interface
     * 
     * Opens the default SPI device (SPI0.0 on Raspberry Pi).
     * Configures default settings: MODE0, MSBFIRST, 4 MHz.
     * 
     * @return true if successful, false on error
     * 
     * @note Raspberry Pi SPI pins:
     *       - MOSI: GPIO 10 (Pin 19)
     *       - MISO: GPIO 9 (Pin 21)
     *       - SCLK: GPIO 11 (Pin 23)
     *       - CE0: GPIO 8 (Pin 24)
     *       - CE1: GPIO 7 (Pin 26)
     * 
     * @example
     * SPI.begin();  // Initialize with defaults
     */
    bool begin();
    
    /**
     * @brief Initialize SPI with specific bus and chip select
     * 
     * Opens a specific SPI device.
     * 
     * @param bus SPI bus number (0 or 1)
     * @param cs Chip select number (0 or 1)
     * @return true if successful, false on error
     * 
     * @example
     * SPI.begin(0, 0);  // /dev/spidev0.0 (default)
     * SPI.begin(0, 1);  // /dev/spidev0.1 (CE1)
     */
    bool begin(int bus, int cs);
    
    /**
     * @brief Close SPI interface
     * 
     * Closes the SPI device file descriptor.
     */
    void end();
    
    /* ------------------------------------------------------------ */
    /*                   CONFIGURATION                              */
    /* ------------------------------------------------------------ */
    
    /**
     * @brief Set SPI data mode
     * 
     * Configures clock polarity (CPOL) and phase (CPHA).
     * 
     * @param mode SPI mode (0-3):
     *             - SPI_MODE0: CPOL=0, CPHA=0 (most common)
     *             - SPI_MODE1: CPOL=0, CPHA=1
     *             - SPI_MODE2: CPOL=1, CPHA=0
     *             - SPI_MODE3: CPOL=1, CPHA=1
     * 
     * @example
     * SPI.setDataMode(SPI_MODE0);  // Default mode
     */
    void setDataMode(uint8_t mode);
    
    /**
     * @brief Set bit order
     * 
     * Configures whether MSB or LSB is transmitted first.
     * 
     * @param bitOrder MSBFIRST (default) or LSBFIRST
     * 
     * @example
     * SPI.setBitOrder(MSBFIRST);  // Most devices use MSB first
     */
    void setBitOrder(uint8_t bitOrder);
    
    /**
     * @brief Set clock divider (Arduino compatibility)
     * 
     * Sets SPI clock speed as a divider of the base clock.
     * On Raspberry Pi, base clock is typically 250 MHz.
     * 
     * @param divider Clock divider (2, 4, 8, 16, 32, 64, 128)
     * 
     * @note Actual speeds:
     *       - DIV2:   125 MHz (may not be achievable)
     *       - DIV4:   62.5 MHz
     *       - DIV8:   31.25 MHz
     *       - DIV16:  15.625 MHz (default)
     *       - DIV32:  7.8125 MHz
     *       - DIV64:  3.90625 MHz
     *       - DIV128: 1.953125 MHz
     * 
     * @example
     * SPI.setClockDivider(SPI_CLOCK_DIV8);  // 31.25 MHz
     */
    void setClockDivider(uint8_t divider);
    
    /**
     * @brief Set clock speed in Hz
     * 
     * Directly sets SPI clock speed in Hertz.
     * 
     * @param speed Clock speed in Hz (max ~125 MHz on Pi 4/5)
     * 
     * @example
     * SPI.setClock(4000000);  // 4 MHz
     * SPI.setClock(8000000);  // 8 MHz
     */
    void setClock(uint32_t speed);
    
    /**
     * @brief Get current clock speed
     * 
     * @return Current clock speed in Hz
     */
    uint32_t getClock() const;
    
    /* ------------------------------------------------------------ */
    /*                   DATA TRANSFER                              */
    /* ------------------------------------------------------------ */
    
    /**
     * @brief Transfer a single byte (full-duplex)
     * 
     * Sends one byte and simultaneously receives one byte.
     * SPI is full-duplex, so data is exchanged in both directions.
     * 
     * @param data Byte to send
     * @return Byte received
     * 
     * @note Chip select (CS) must be managed manually with digitalWrite()
     * 
     * @example
     * digitalWrite(CS_PIN, LOW);          // Select device
     * uint8_t status = SPI.transfer(0x00);  // Send dummy byte, read status
     * digitalWrite(CS_PIN, HIGH);         // Deselect device
     */
    uint8_t transfer(uint8_t data);
    
    /**
     * @brief Transfer multiple bytes (full-duplex)
     * 
     * Sends multiple bytes and simultaneously receives the same number.
     * 
     * @param buffer Buffer containing data to send; will be overwritten with received data
     * @param length Number of bytes to transfer
     * 
     * @note Buffer is modified in-place (sent data replaced with received data)
     * 
     * @example
     * uint8_t data[] = {0x01, 0x02, 0x03};
     * digitalWrite(CS_PIN, LOW);
     * SPI.transfer(data, 3);  // Send and receive 3 bytes
     * digitalWrite(CS_PIN, HIGH);
     * // data[] now contains received bytes
     */
    void transfer(uint8_t* buffer, size_t length);
    
    /**
     * @brief Transfer with separate send and receive buffers
     * 
     * Sends data from txBuffer and receives into rxBuffer.
     * 
     * @param txBuffer Buffer containing data to send
     * @param rxBuffer Buffer to receive data into
     * @param length Number of bytes to transfer
     * 
     * @example
     * uint8_t tx[] = {0x01, 0x02, 0x03};
     * uint8_t rx[3];
     * SPI.transfer(tx, rx, 3);
     * // rx[] contains received data, tx[] unchanged
     */
    void transfer(const uint8_t* txBuffer, uint8_t* rxBuffer, size_t length);
    
    /* ------------------------------------------------------------ */
    /*                   HELPER FUNCTIONS                           */
    /* ------------------------------------------------------------ */
    
    /**
     * @brief Check if SPI is initialized
     * 
     * @return true if begin() has been called and succeeded
     */
    bool isInitialized() const;
    
private:
    int fd_;                    ///< SPI device file descriptor
    int busNumber_;             ///< SPI bus number
    int csNumber_;              ///< Chip select number
    uint8_t mode_;              ///< SPI mode (0-3)
    uint8_t bitOrder_;          ///< Bit order (MSBFIRST/LSBFIRST)
    uint32_t speed_;            ///< Clock speed in Hz
    uint8_t bitsPerWord_;       ///< Bits per word (always 8)
    mutable std::mutex mutex_;  ///< Thread safety mutex
    
    static constexpr uint32_t DEFAULT_SPEED = 4000000;  ///< Default 4 MHz
    static constexpr uint32_t BASE_CLOCK = 250000000;   ///< Pi base clock (250 MHz)
    
    /**
     * @brief Apply current configuration to device
     * 
     * Updates device with current mode, speed, bit order settings.
     * 
     * @return true if successful, false on error
     */
    bool applySettings();
};

// Global SPI instance (Arduino compatibility)
extern SPIClass SPI;

} // namespace pipinpp

// Allow usage without namespace (Arduino compatibility)
using pipinpp::SPI;
using pipinpp::SPIClass;
using pipinpp::SPI_MODE0;
using pipinpp::SPI_MODE1;
using pipinpp::SPI_MODE2;
using pipinpp::SPI_MODE3;
// Note: MSBFIRST and LSBFIRST defined in ArduinoCompat.hpp

#endif // PIPINPP_SPI_HPP
