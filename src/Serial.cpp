/**
 * @file Serial.cpp
 * @brief Implementation of Arduino-compatible Serial communication
 * @author Barbatos6669
 * @date 2025-11-16
 * 
 * Copyright (c) 2025 HobbyHacker
 * SPDX-License-Identifier: MIT
 */

#include "Serial.hpp"
#include "log.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <cstring>
#include <sstream>
#include <iomanip>

namespace pipinpp {

SerialPort::SerialPort()
    : fd_(-1)
    , baudRate_(0)
    , timeout_(1000)  // Default 1 second timeout
    , device_("")
{
}

SerialPort::~SerialPort()
{
    end();
}

bool SerialPort::begin(unsigned long baudRate, const std::string& device)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Close existing connection if open
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
    
    // Open serial port
    fd_ = ::open(device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd_ < 0) {
        PIPINPP_LOG_ERROR("Failed to open serial port: " << device 
                         << " (errno: " << errno << " - " << std::strerror(errno) << ")");
        return false;
    }
    
    // Configure port settings
    if (!configurePort(baudRate)) {
        ::close(fd_);
        fd_ = -1;
        return false;
    }
    
    device_ = device;
    baudRate_ = baudRate;
    
    PIPINPP_LOG_INFO("Serial port opened: " << device << " at " << baudRate << " baud");
    return true;
}

void SerialPort::end()
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (fd_ >= 0) {
        ::close(fd_);
        PIPINPP_LOG_INFO("Serial port closed: " << device_);
        fd_ = -1;
        baudRate_ = 0;
        device_ = "";
    }
}

bool SerialPort::isOpen() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return fd_ >= 0;
}

int SerialPort::available()
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (fd_ < 0) {
        return 0;
    }
    
    int bytesAvailable = 0;
    if (::ioctl(fd_, FIONREAD, &bytesAvailable) < 0) {
        PIPINPP_LOG_ERROR("Failed to query available bytes (errno: " << errno << ")");
        return 0;
    }
    
    return bytesAvailable;
}

int SerialPort::read()
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (fd_ < 0) {
        return -1;
    }
    
    uint8_t byte;
    
    // Set timeout using select()
    fd_set readfds;
    struct timeval tv;
    FD_ZERO(&readfds);
    FD_SET(fd_, &readfds);
    tv.tv_sec = timeout_ / 1000;
    tv.tv_usec = (timeout_ % 1000) * 1000;
    
    int result = ::select(fd_ + 1, &readfds, nullptr, nullptr, &tv);
    if (result <= 0) {
        return -1;  // Timeout or error
    }
    
    ssize_t bytesRead = ::read(fd_, &byte, 1);
    if (bytesRead == 1) {
        return byte;
    }
    
    return -1;
}

int SerialPort::peek()
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (fd_ < 0) {
        return -1;
    }
    
    uint8_t byte;
    ssize_t bytesRead = ::recv(fd_, &byte, 1, MSG_PEEK);
    if (bytesRead == 1) {
        return byte;
    }
    
    return -1;
}

size_t SerialPort::write(uint8_t byte)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (fd_ < 0) {
        return 0;
    }
    
    ssize_t bytesWritten = ::write(fd_, &byte, 1);
    return (bytesWritten > 0) ? bytesWritten : 0;
}

size_t SerialPort::write(const uint8_t* buffer, size_t size)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (fd_ < 0 || buffer == nullptr || size == 0) {
        return 0;
    }
    
    ssize_t bytesWritten = ::write(fd_, buffer, size);
    return (bytesWritten > 0) ? bytesWritten : 0;
}

size_t SerialPort::write(const std::string& str)
{
    return write(reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
}

size_t SerialPort::print(const std::string& data)
{
    return write(data);
}

size_t SerialPort::print(int num)
{
    return write(std::to_string(num));
}

size_t SerialPort::print(double num, int decimals)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(decimals) << num;
    return write(oss.str());
}

size_t SerialPort::println(const std::string& data)
{
    return write(data + "\r\n");
}

size_t SerialPort::println(int num)
{
    return write(std::to_string(num) + "\r\n");
}

size_t SerialPort::println(double num, int decimals)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(decimals) << num;
    return write(oss.str() + "\r\n");
}

size_t SerialPort::println()
{
    return write("\r\n");
}

std::string SerialPort::readString()
{
    std::string result;
    
    while (true) {
        int byte = read();
        if (byte < 0) {
            break;  // Timeout or no more data
        }
        result += static_cast<char>(byte);
    }
    
    return result;
}

std::string SerialPort::readStringUntil(char terminator)
{
    std::string result;
    
    while (true) {
        int byte = read();
        if (byte < 0) {
            break;  // Timeout
        }
        if (static_cast<char>(byte) == terminator) {
            break;  // Found terminator
        }
        result += static_cast<char>(byte);
    }
    
    return result;
}

void SerialPort::setTimeout(unsigned long milliseconds)
{
    std::lock_guard<std::mutex> lock(mutex_);
    timeout_ = milliseconds;
}

void SerialPort::flush()
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (fd_ >= 0) {
        ::tcdrain(fd_);  // Wait for all output to be transmitted
    }
}

unsigned long SerialPort::getBaudRate() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return baudRate_;
}

bool SerialPort::configurePort(unsigned long baudRate)
{
    struct termios tty;
    
    // Get current settings
    if (::tcgetattr(fd_, &tty) != 0) {
        PIPINPP_LOG_ERROR("Failed to get terminal attributes (errno: " << errno << ")");
        return false;
    }
    
    // Get baud rate constant
    speed_t speed = getBaudRateConstant(baudRate);
    if (speed == 0) {
        PIPINPP_LOG_ERROR("Invalid baud rate: " << baudRate);
        return false;
    }
    
    // Set baud rate
    ::cfsetospeed(&tty, speed);
    ::cfsetispeed(&tty, speed);
    
    // Configure 8N1 (8 data bits, no parity, 1 stop bit)
    tty.c_cflag &= ~PARENB;        // No parity
    tty.c_cflag &= ~CSTOPB;        // 1 stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;            // 8 data bits
    tty.c_cflag &= ~CRTSCTS;       // No hardware flow control
    tty.c_cflag |= CREAD | CLOCAL; // Enable receiver, ignore modem control lines
    
    // Raw input mode
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    
    // Disable software flow control
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    
    // Raw output mode
    tty.c_oflag &= ~OPOST;
    
    // Set read timeout (VMIN=0, VTIME=0 for non-blocking with select())
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;
    
    // Apply settings
    if (::tcsetattr(fd_, TCSANOW, &tty) != 0) {
        PIPINPP_LOG_ERROR("Failed to set terminal attributes (errno: " << errno << ")");
        return false;
    }
    
    // Flush buffers
    ::tcflush(fd_, TCIOFLUSH);
    
    return true;
}

speed_t SerialPort::getBaudRateConstant(unsigned long baudRate) const
{
    switch (baudRate) {
        case 50:      return B50;
        case 75:      return B75;
        case 110:     return B110;
        case 134:     return B134;
        case 150:     return B150;
        case 200:     return B200;
        case 300:     return B300;
        case 600:     return B600;
        case 1200:    return B1200;
        case 1800:    return B1800;
        case 2400:    return B2400;
        case 4800:    return B4800;
        case 9600:    return B9600;
        case 19200:   return B19200;
        case 38400:   return B38400;
        case 57600:   return B57600;
        case 115200:  return B115200;
        case 230400:  return B230400;
        case 460800:  return B460800;
        case 500000:  return B500000;
        case 576000:  return B576000;
        case 921600:  return B921600;
        case 1000000: return B1000000;
        case 1152000: return B1152000;
        case 1500000: return B1500000;
        case 2000000: return B2000000;
        case 2500000: return B2500000;
        case 3000000: return B3000000;
        case 3500000: return B3500000;
        case 4000000: return B4000000;
        default:
            return 0;  // Invalid baud rate
    }
}

} // namespace pipinpp

// Global Serial instance
pipinpp::SerialPort Serial;
