/**
 * @file Serial.hpp
 * @brief Arduino-compatible Serial (UART) communication for Raspberry Pi
 * @author Barbatos6669
 * @date 2025-11-16
 * 
 * Provides Arduino Serial API compatibility for communicating with external
 * devices like Arduino boards, sensors, and other UART peripherals.
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

#ifndef PIPINPP_SERIAL_HPP
#define PIPINPP_SERIAL_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <mutex>
#include <termios.h>

namespace pipinpp {

/**
 * @class SerialPort
 * @brief Arduino-compatible Serial communication class for Raspberry Pi
 * 
 * This class provides UART communication compatible with Arduino's Serial API.
 * It uses Linux termios for hardware serial ports (/dev/ttyUSB*, /dev/ttyACM*, /dev/ttyAMA*).
 * 
 * @note Thread-safe: All operations are protected by internal mutex
 * @note Hardware serial: Uses actual UART, not software bit-banging
 * 
 * Example Usage (communicating with Arduino):
 * @code
 * #include "Serial.hpp"
 * using namespace pipinpp;
 * 
 * int main() {
 *     // Open serial connection to Arduino at 9600 baud
 *     if (!Serial.begin(9600, "/dev/ttyUSB0")) {
 *         std::cerr << "Failed to open serial port\n";
 *         return 1;
 *     }
 *     
 *     // Send command to Arduino
 *     Serial.println("MOVE_FORWARD");
 *     
 *     // Wait for response
 *     delay(100);
 *     if (Serial.available()) {
 *         String response = Serial.readStringUntil('\n');
 *         std::cout << "Arduino says: " << response << "\n";
 *     }
 *     
 *     Serial.end();
 *     return 0;
 * }
 * @endcode
 */
class SerialPort 
{
public:
    /**
     * @brief Constructor
     */
    SerialPort();
    
    /**
     * @brief Destructor - automatically closes port if open
     */
    ~SerialPort();
    
    /**
     * @brief Open serial port with specified baud rate
     * @param baudRate Communication speed (9600, 19200, 38400, 57600, 115200, etc.)
     * @param device Serial device path (default: "/dev/ttyUSB0")
     * @return true if port opened successfully, false on error
     * 
     * Common devices:
     * - /dev/ttyUSB0 - USB-to-serial adapter (FTDI, CH340, etc.)
     * - /dev/ttyACM0 - Arduino Uno/Mega (native USB)
     * - /dev/ttyAMA0 - Raspberry Pi hardware UART (GPIO 14/15)
     * - /dev/ttyS0 - Raspberry Pi mini UART (less accurate, avoid if possible)
     * 
     * @note Multiple begin() calls are safe - will close existing connection first
     * @note User must be in 'dialout' group for /dev/tty* access
     */
    bool begin(unsigned long baudRate, const std::string& device = "/dev/ttyUSB0");
    
    /**
     * @brief Close serial port
     * @note Safe to call even if port not open
     */
    void end();
    
    /**
     * @brief Check if serial port is open and ready
     * @return true if port is open, false otherwise
     */
    bool isOpen() const;
    
    /**
     * @brief Get number of bytes available for reading
     * @return Number of bytes in receive buffer
     * @note Returns 0 if port not open or no data available
     */
    int available();
    
    /**
     * @brief Read single byte from serial port (blocking)
     * @return Byte value (0-255), or -1 if no data available or timeout
     * @note Respects timeout set by setTimeout()
     */
    int read();
    
    /**
     * @brief Peek at next byte without removing from buffer
     * @return Byte value (0-255), or -1 if no data available
     * @note Does not advance read position
     */
    int peek();
    
    /**
     * @brief Write single byte to serial port
     * @param byte Byte to send (0-255)
     * @return Number of bytes written (1 on success, 0 on failure)
     */
    size_t write(uint8_t byte);
    
    /**
     * @brief Write buffer of bytes to serial port
     * @param buffer Pointer to data buffer
     * @param size Number of bytes to send
     * @return Number of bytes actually written
     */
    size_t write(const uint8_t* buffer, size_t size);
    
    /**
     * @brief Write string to serial port
     * @param str String to send
     * @return Number of bytes written
     */
    size_t write(const std::string& str);
    
    /**
     * @brief Print data to serial port (no newline)
     * @param data String to print
     * @return Number of bytes written
     */
    size_t print(const std::string& data);
    
    /**
     * @brief Print integer to serial port (no newline)
     * @param num Integer to print
     * @return Number of bytes written
     */
    size_t print(int num);
    
    /**
     * @brief Print floating-point number to serial port (no newline)
     * @param num Number to print
     * @param decimals Number of decimal places (default: 2)
     * @return Number of bytes written
     */
    size_t print(double num, int decimals = 2);
    
    /**
     * @brief Print data to serial port with newline
     * @param data String to print
     * @return Number of bytes written (including newline)
     */
    size_t println(const std::string& data);
    
    /**
     * @brief Print integer to serial port with newline
     * @param num Integer to print
     * @return Number of bytes written
     */
    size_t println(int num);
    
    /**
     * @brief Print floating-point number to serial port with newline
     * @param num Number to print
     * @param decimals Number of decimal places (default: 2)
     * @return Number of bytes written
     */
    size_t println(double num, int decimals = 2);
    
    /**
     * @brief Print newline only
     * @return Number of bytes written (2: \r\n)
     */
    size_t println();
    
    /**
     * @brief Read all available data into string
     * @return String containing all received data
     * @note Reads until no more data available or timeout
     */
    std::string readString();
    
    /**
     * @brief Read data until specific character found
     * @param terminator Character to stop at (not included in result)
     * @return String containing data up to terminator
     * @note Commonly used with '\n' to read line-by-line from Arduino
     */
    std::string readStringUntil(char terminator);
    
    /**
     * @brief Set timeout for read operations
     * @param milliseconds Timeout in milliseconds (0 = non-blocking, default: 1000)
     * @note Affects read(), readString(), readStringUntil()
     */
    void setTimeout(unsigned long milliseconds);
    
    /**
     * @brief Flush transmit buffer (wait for all data to be sent)
     * @note Blocks until all pending data transmitted
     */
    void flush();
    
    /**
     * @brief Get current baud rate
     * @return Baud rate, or 0 if port not open
     */
    unsigned long getBaudRate() const;

private:
    int fd_;                          ///< File descriptor for serial port
    unsigned long baudRate_;          ///< Current baud rate
    unsigned long timeout_;           ///< Read timeout in milliseconds
    std::string device_;              ///< Device path (e.g., "/dev/ttyUSB0")
    mutable std::mutex mutex_;        ///< Mutex for thread safety
    
    /**
     * @brief Configure termios settings for serial port
     * @param baudRate Desired baud rate
     * @return true on success, false on failure
     */
    bool configurePort(unsigned long baudRate);
    
    /**
     * @brief Convert baud rate integer to termios speed constant
     * @param baudRate Baud rate (e.g., 9600, 115200)
     * @return termios speed_t constant, or 0 if invalid
     */
    speed_t getBaudRateConstant(unsigned long baudRate) const;
};

} // namespace pipinpp

// Global Serial instance (Arduino-compatible)
extern pipinpp::SerialPort Serial;

#endif // PIPINPP_SERIAL_HPP
