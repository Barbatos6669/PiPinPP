/**
 * @file SPI.cpp
 * @brief Implementation of Arduino-inspired SPI communication library
 * @author Barbatos6669
 * @date 2025
 * 
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

#include "SPI.hpp"
#include "ArduinoCompat.hpp"  // For MSBFIRST/LSBFIRST constants
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <cstring>

namespace pipinpp {

// Global SPI instance
SPIClass SPI;

SPIClass::SPIClass()
    : fd_(-1)
    , busNumber_(0)
    , csNumber_(0)
    , mode_(SPI_MODE0)
    , bitOrder_(MSBFIRST)
    , speed_(DEFAULT_SPEED)
    , bitsPerWord_(8)
{
}

SPIClass::~SPIClass() {
    end();
}

bool SPIClass::begin() {
    return begin(0, 0);  // Default: /dev/spidev0.0
}

bool SPIClass::begin(int bus, int cs) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Close existing connection if any
    if (fd_ >= 0) {
        close(fd_);
        fd_ = -1;
    }
    
    busNumber_ = bus;
    csNumber_ = cs;
    
    // Open SPI device
    char device[20];
    snprintf(device, sizeof(device), "/dev/spidev%d.%d", bus, cs);
    
    fd_ = open(device, O_RDWR);
    if (fd_ < 0) {
        return false;
    }
    
    // Set default configuration
    mode_ = SPI_MODE0;
    bitOrder_ = MSBFIRST;
    speed_ = DEFAULT_SPEED;
    bitsPerWord_ = 8;
    
    return applySettings();
}

void SPIClass::end() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (fd_ >= 0) {
        close(fd_);
        fd_ = -1;
    }
}

void SPIClass::setDataMode(uint8_t mode) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (mode > 3) {
        return;  // Invalid mode
    }
    
    mode_ = mode;
    
    if (fd_ >= 0) {
        applySettings();
    }
}

void SPIClass::setBitOrder(uint8_t bitOrder) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    bitOrder_ = bitOrder;
    
    if (fd_ >= 0) {
        applySettings();
    }
}

void SPIClass::setClockDivider(uint8_t divider) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Calculate speed from divider
    // Base clock is 250 MHz on Raspberry Pi
    speed_ = BASE_CLOCK / divider;
    
    if (fd_ >= 0) {
        applySettings();
    }
}

void SPIClass::setClock(uint32_t speed) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    speed_ = speed;
    
    if (fd_ >= 0) {
        applySettings();
    }
}

uint32_t SPIClass::getClock() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return speed_;
}

uint8_t SPIClass::transfer(uint8_t data) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (fd_ < 0) {
        return 0;
    }
    
    uint8_t rx = 0;
    
    struct spi_ioc_transfer tr;
    memset(&tr, 0, sizeof(tr));
    tr.tx_buf = (unsigned long)&data;
    tr.rx_buf = (unsigned long)&rx;
    tr.len = 1;
    tr.speed_hz = speed_;
    tr.bits_per_word = bitsPerWord_;
    
    if (ioctl(fd_, SPI_IOC_MESSAGE(1), &tr) < 0) {
        return 0;
    }
    
    return rx;
}

void SPIClass::transfer(uint8_t* buffer, size_t length) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (fd_ < 0 || buffer == nullptr || length == 0) {
        return;
    }
    
    struct spi_ioc_transfer tr;
    memset(&tr, 0, sizeof(tr));
    tr.tx_buf = (unsigned long)buffer;
    tr.rx_buf = (unsigned long)buffer;
    tr.len = length;
    tr.speed_hz = speed_;
    tr.bits_per_word = bitsPerWord_;
    
    ioctl(fd_, SPI_IOC_MESSAGE(1), &tr);
}

void SPIClass::transfer(const uint8_t* txBuffer, uint8_t* rxBuffer, size_t length) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (fd_ < 0 || txBuffer == nullptr || rxBuffer == nullptr || length == 0) {
        return;
    }
    
    struct spi_ioc_transfer tr;
    memset(&tr, 0, sizeof(tr));
    tr.tx_buf = (unsigned long)txBuffer;
    tr.rx_buf = (unsigned long)rxBuffer;
    tr.len = length;
    tr.speed_hz = speed_;
    tr.bits_per_word = bitsPerWord_;
    
    ioctl(fd_, SPI_IOC_MESSAGE(1), &tr);
}

bool SPIClass::isInitialized() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return fd_ >= 0;
}

bool SPIClass::applySettings() {
    // Note: Mutex should already be locked by caller
    
    if (fd_ < 0) {
        return false;
    }
    
    // Set SPI mode
    uint8_t spiMode = mode_;
    if (bitOrder_ == LSBFIRST) {
        spiMode |= SPI_LSB_FIRST;
    }
    
    if (ioctl(fd_, SPI_IOC_WR_MODE, &spiMode) < 0) {
        return false;
    }
    
    // Set bits per word
    if (ioctl(fd_, SPI_IOC_WR_BITS_PER_WORD, &bitsPerWord_) < 0) {
        return false;
    }
    
    // Set max speed
    if (ioctl(fd_, SPI_IOC_WR_MAX_SPEED_HZ, &speed_) < 0) {
        return false;
    }
    
    return true;
}

} // namespace pipinpp
