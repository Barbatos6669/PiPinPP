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
 * @file Wire.cpp
 * @brief Implementation of Arduino-inspired I2C Wire library
 * @author Barbatos6669
 * @date 2025
 */

#include "Wire.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstring>
#include <stdexcept>
#include <sys/stat.h>

namespace pipinpp {

// Global Wire instance
WireClass Wire;

WireClass::WireClass()
    : fd_(-1)
    , busNumber_(-1)
    , clockFrequency_(100000)  // Default 100kHz
    , txAddress_(0)
    , rxIndex_(0)
{
    txBuffer_.reserve(BUFFER_SIZE);
    rxBuffer_.reserve(BUFFER_SIZE);
}

WireClass::~WireClass() {
    end();
}

/* ------------------------------------------------------------ */
/*                   INITIALIZATION                             */
/* ------------------------------------------------------------ */

bool WireClass::begin() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Auto-detect I2C bus
    int detectedBus = detectBusNumber();
    if (detectedBus < 0) {
        return false;
    }
    
    // Close existing connection if any
    if (fd_ >= 0) {
        close(fd_);
    }
    
    busNumber_ = detectedBus;
    
    // Open I2C device
    char filename[20];
    snprintf(filename, sizeof(filename), "/dev/i2c-%d", busNumber_);
    
    fd_ = open(filename, O_RDWR);
    if (fd_ < 0) {
        busNumber_ = -1;
        return false;
    }
    
    return true;
}

bool WireClass::begin(int busNumber) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (fd_ >= 0) {
        close(fd_);
    }
    
    busNumber_ = busNumber;
    
    // Open I2C device
    char filename[20];
    snprintf(filename, sizeof(filename), "/dev/i2c-%d", busNumber_);
    
    fd_ = open(filename, O_RDWR);
    if (fd_ < 0) {
        busNumber_ = -1;
        return false;
    }
    
    return true;
}

void WireClass::end() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (fd_ >= 0) {
        close(fd_);
        fd_ = -1;
    }
    
    busNumber_ = -1;
    txBuffer_.clear();
    rxBuffer_.clear();
    rxIndex_ = 0;
}

/* ------------------------------------------------------------ */
/*                   TRANSMISSION                               */
/* ------------------------------------------------------------ */

void WireClass::beginTransmission(uint8_t address) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    txAddress_ = address;
    txBuffer_.clear();
}

uint8_t WireClass::endTransmission(bool sendStop) {
    (void)sendStop;  // Parameter not used on Linux i2c-dev (always sends STOP)
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (fd_ < 0) {
        return 4;  // Other error (not initialized)
    }
    
    if (txBuffer_.empty()) {
        return 0;  // Success (nothing to send)
    }
    
    if (!setSlaveAddress(txAddress_)) {
        return 2;  // NACK on address
    }
    
    // Write data
    ssize_t written = ::write(fd_, txBuffer_.data(), txBuffer_.size());
    
    if (written < 0) {
        return 3;  // NACK on data
    }
    
    if (static_cast<size_t>(written) != txBuffer_.size()) {
        return 3;  // Incomplete write
    }
    
    txBuffer_.clear();
    return 0;  // Success
}

uint8_t WireClass::endTransmission() {
    return this->endTransmission(true);
}

/* ------------------------------------------------------------ */
/*                   WRITING DATA                               */
/* ------------------------------------------------------------ */

size_t WireClass::write(uint8_t data) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (txBuffer_.size() >= BUFFER_SIZE) {
        return 0;  // Buffer full
    }
    
    txBuffer_.push_back(data);
    return 1;
}

size_t WireClass::write(const uint8_t* data, size_t length) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    size_t available = BUFFER_SIZE - txBuffer_.size();
    size_t toWrite = (length < available) ? length : available;
    
    if (toWrite == 0) {
        return 0;
    }
    
    txBuffer_.insert(txBuffer_.end(), data, data + toWrite);
    return toWrite;
}

/* ------------------------------------------------------------ */
/*                   READING DATA                               */
/* ------------------------------------------------------------ */

size_t WireClass::requestFrom(uint8_t address, size_t quantity, bool sendStop) {
    (void)sendStop;  // Parameter not used on Linux i2c-dev (always sends STOP)
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (fd_ < 0) {
        return 0;
    }
    
    if (quantity == 0 || quantity > BUFFER_SIZE) {
        return 0;
    }
    
    if (!setSlaveAddress(address)) {
        return 0;
    }
    
    // Prepare receive buffer
    rxBuffer_.resize(quantity);
    rxIndex_ = 0;
    
    // Read data
    ssize_t bytesRead = ::read(fd_, rxBuffer_.data(), quantity);
    
    if (bytesRead < 0) {
        rxBuffer_.clear();
        return 0;
    }
    
    rxBuffer_.resize(bytesRead);
    return bytesRead;
}

size_t WireClass::requestFrom(uint8_t address, size_t quantity) {
    return this->requestFrom(address, quantity, true);
}

size_t WireClass::available() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (rxIndex_ >= rxBuffer_.size()) {
        return 0;
    }
    
    return rxBuffer_.size() - rxIndex_;
}

int WireClass::read() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (rxIndex_ >= rxBuffer_.size()) {
        return -1;
    }
    
    return rxBuffer_[rxIndex_++];
}

int WireClass::peek() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (rxIndex_ >= rxBuffer_.size()) {
        return -1;
    }
    
    return rxBuffer_[rxIndex_];
}

/* ------------------------------------------------------------ */
/*                   CONFIGURATION                              */
/* ------------------------------------------------------------ */

bool WireClass::setClock(uint32_t frequency) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Note: Linux i2c-dev doesn't provide a standard way to set clock speed
    // from userspace. This would need to be configured at device tree level
    // or kernel module parameters.
    //
    // We store the requested frequency for reference, but actual speed
    // is determined by hardware/kernel configuration.
    
    clockFrequency_ = frequency;
    return true;  // Always return success (best effort)
}

uint32_t WireClass::getClock() const {
    return clockFrequency_;
}

/* ------------------------------------------------------------ */
/*                   HELPER FUNCTIONS                           */
/* ------------------------------------------------------------ */

int WireClass::readRegister(uint8_t address, uint8_t reg) {
    beginTransmission(address);
    write(reg);
    
    if (this->endTransmission(false) != 0) {  // Repeated START
        return -1;
    }
    
    if (this->requestFrom(address, static_cast<size_t>(1), true) != 1) {
        return -1;
    }
    
    return read();
}

bool WireClass::writeRegister(uint8_t address, uint8_t reg, uint8_t value) {
    beginTransmission(address);
    write(reg);
    write(value);
    
    return this->endTransmission(true) == 0;
}

int WireClass::readRegisters(uint8_t address, uint8_t reg, uint8_t* buffer, size_t length) {
    if (buffer == nullptr || length == 0) {
        return -1;
    }
    
    beginTransmission(address);
    write(reg);
    
    if (this->endTransmission(false) != 0) {  // Repeated START
        return -1;
    }
    
    size_t received = this->requestFrom(address, length, true);
    
    for (size_t i = 0; i < received; i++) {
        int byte = read();
        if (byte < 0) {
            return i;  // Return how many we got
        }
        buffer[i] = static_cast<uint8_t>(byte);
    }
    
    return received;
}

std::vector<uint8_t> WireClass::scan() {
    std::vector<uint8_t> devices;
    
    // Scan valid 7-bit addresses (0x03 to 0x77)
    // 0x00-0x02 and 0x78-0x7F are reserved
    for (uint8_t addr = 0x03; addr <= 0x77; addr++) {
        if (exists(addr)) {
            devices.push_back(addr);
        }
    }
    
    return devices;
}

bool WireClass::exists(uint8_t address) {
    beginTransmission(address);
    uint8_t error = this->endTransmission(true);
    return (error == 0);
}

/* ------------------------------------------------------------ */
/*                   PRIVATE METHODS                            */
/* ------------------------------------------------------------ */

int WireClass::detectBusNumber() {
    // Try common I2C bus numbers
    // Pi 0-4: typically /dev/i2c-1
    // Pi 5: typically /dev/i2c-20 or /dev/i2c-21
    
    const int candidates[] = {1, 20, 21, 0, 2};
    
    for (int bus : candidates) {
        char filename[20];
        snprintf(filename, sizeof(filename), "/dev/i2c-%d", bus);
        
        struct stat st;
        if (stat(filename, &st) == 0) {
            return bus;
        }
    }
    
    return -1;  // No I2C device found
}

bool WireClass::setSlaveAddress(uint8_t address) {
    if (fd_ < 0) {
        return false;
    }
    
    if (ioctl(fd_, I2C_SLAVE, address) < 0) {
        return false;
    }
    
    return true;
}

} // namespace pipinpp
