/**
 * @file pypipinpp.cpp
 * @brief Python bindings for PiPinPP using pybind11
 * @author Barbatos6669
 * @date 2025-11-17
 * 
 * Python bindings providing Arduino-compatible GPIO API for Raspberry Pi.
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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "ArduinoCompat.hpp"
#include "interrupts.hpp"  // For pipinpp::InterruptMode
#include "Wire.hpp"
#include "SPI.hpp"
#include "Serial.hpp"
#include "pwm.hpp"
#include "exceptions.hpp"
#include "platform.hpp"
#include <map>
#include <memory>

namespace py = pybind11;

// Global map to store Python callbacks for interrupts (keeps them alive)
static std::map<int, std::shared_ptr<py::function>> g_interrupt_callbacks;

// NOTE: Python interrupt support uses a workaround:
// The Arduino attachInterrupt() expects void(*)() but we need to capture pin number.
// Solution: Store callbacks in map above, and use low-level interrupt API directly
// by including interrupts.hpp which provides: attachInterrupt(int, std::function<void()>, InterruptMode, string)

PYBIND11_MODULE(pypipinpp, m) {
    m.doc() = "PiPinPP - Arduino-compatible GPIO library for Raspberry Pi (Python bindings)";
    
    // ========================================================================
    // EXCEPTIONS
    // ========================================================================
    
    py::register_exception<PinError>(m, "PinError");
    py::register_exception<InvalidPinError>(m, "InvalidPinError");
    py::register_exception<GpioAccessError>(m, "GpioAccessError");
    
    // ========================================================================
    // CONSTANTS - Pin Modes
    // ========================================================================
    
    m.attr("OUTPUT") = py::int_(static_cast<int>(OUTPUT));
    m.attr("INPUT") = py::int_(static_cast<int>(INPUT));
    m.attr("INPUT_PULLUP") = py::int_(static_cast<int>(INPUT_PULLUP));
    m.attr("INPUT_PULLDOWN") = py::int_(static_cast<int>(INPUT_PULLDOWN));
    
    // ========================================================================
    // CONSTANTS - Digital Values
    // ========================================================================
    
    m.attr("HIGH") = py::int_(HIGH);
    m.attr("LOW") = py::int_(LOW);
    
    // ========================================================================
    // CONSTANTS - Interrupt Modes
    // ========================================================================
    
    m.attr("RISING") = py::int_(RISING);
    m.attr("FALLING") = py::int_(FALLING);
    m.attr("CHANGE") = py::int_(CHANGE);
    
    // ========================================================================
    // CONSTANTS - Bit Order
    // ========================================================================
    
    m.attr("LSBFIRST") = py::int_(LSBFIRST);
    m.attr("MSBFIRST") = py::int_(MSBFIRST);
    
    // ========================================================================
    // CONSTANTS - SPI Modes
    // ========================================================================
    
    m.attr("SPI_MODE0") = SPI_MODE0;
    m.attr("SPI_MODE1") = SPI_MODE1;
    m.attr("SPI_MODE2") = SPI_MODE2;
    m.attr("SPI_MODE3") = SPI_MODE3;
    
    // ========================================================================
    // CONSTANTS - SPI Clock Dividers
    // ========================================================================
    
    m.attr("SPI_CLOCK_DIV2") = pipinpp::SPI_CLOCK_DIV2;
    m.attr("SPI_CLOCK_DIV4") = pipinpp::SPI_CLOCK_DIV4;
    m.attr("SPI_CLOCK_DIV8") = pipinpp::SPI_CLOCK_DIV8;
    m.attr("SPI_CLOCK_DIV16") = pipinpp::SPI_CLOCK_DIV16;
    m.attr("SPI_CLOCK_DIV32") = pipinpp::SPI_CLOCK_DIV32;
    m.attr("SPI_CLOCK_DIV64") = pipinpp::SPI_CLOCK_DIV64;
    m.attr("SPI_CLOCK_DIV128") = pipinpp::SPI_CLOCK_DIV128;
    
    // ========================================================================
    // CONSTANTS - Serial Print Formats
    // ========================================================================
    
    m.attr("DEC") = py::int_(static_cast<int>(pipinpp::DEC));
    m.attr("HEX") = py::int_(static_cast<int>(pipinpp::HEX));
    m.attr("OCT") = py::int_(static_cast<int>(pipinpp::OCT));
    m.attr("BIN") = py::int_(static_cast<int>(pipinpp::BIN));
    
    // ========================================================================
    // DIGITAL I/O FUNCTIONS
    // ========================================================================
    
    m.def("pinMode", &pinMode, 
          py::arg("pin"), py::arg("mode"),
          "Set the mode of a GPIO pin (OUTPUT, INPUT, INPUT_PULLUP, INPUT_PULLDOWN)");
    
    m.def("digitalWrite", &digitalWrite,
          py::arg("pin"), py::arg("value"),
          "Write a digital value (HIGH or LOW) to a GPIO pin");
    
    m.def("digitalRead", &digitalRead,
          py::arg("pin"),
          "Read the digital value from a GPIO pin (returns HIGH or LOW)");
    
    m.def("digitalToggle", &digitalToggle,
          py::arg("pin"),
          "Toggle the state of an output pin (HIGH->LOW or LOW->HIGH)");
    
    // ========================================================================
    // PIN STATE QUERY FUNCTIONS
    // ========================================================================
    
    m.def("isOutput", &isOutput,
          py::arg("pin"),
          "Check if a pin is configured as OUTPUT");
    
    m.def("isInput", &isInput,
          py::arg("pin"),
          "Check if a pin is configured as INPUT");
    
    m.def("getMode", &getMode,
          py::arg("pin"),
          "Get the current mode of a pin (returns mode constant)");
    
    // ========================================================================
    // ANALOG I/O FUNCTIONS (PWM)
    // ========================================================================
    
    m.def("analogWrite", &analogWrite,
          py::arg("pin"), py::arg("value"),
          "Write an analog value (PWM) to a pin (0-255)");
    
    // ========================================================================
    // TIMING FUNCTIONS
    // ========================================================================
    
    m.def("delay", &delay,
          py::arg("ms"),
          "Pause execution for specified milliseconds");
    
    m.def("delayMicroseconds", &delayMicroseconds,
          py::arg("us"),
          "Pause execution for specified microseconds");
    
    m.def("millis", &millis,
          "Get milliseconds since program start");
    
    m.def("micros", &micros,
          "Get microseconds since program start");
    
    // ========================================================================
    // INTERRUPT FUNCTIONS
    // ========================================================================
    
    m.def("attachInterrupt", 
          [](int pin, py::function callback, int mode) {
              // Store Python callback to keep it alive
              g_interrupt_callbacks[pin] = std::make_shared<py::function>(callback);
              
              // Convert mode constant to InterruptMode enum
              InterruptMode intMode;
              if (mode == RISING) {
                  intMode = InterruptMode::RISING;
              } else if (mode == FALLING) {
                  intMode = InterruptMode::FALLING;
              } else {
                  intMode = InterruptMode::CHANGE;
              }
              
              // Create callback wrapper that acquires GIL
              InterruptCallback cpp_callback = [pin]() {
                  auto it = g_interrupt_callbacks.find(pin);
                  if (it != g_interrupt_callbacks.end() && it->second) {
                      py::gil_scoped_acquire acquire;
                      (*it->second)();
                  }
              };
              
              // Call InterruptManager::getInstance().attachInterrupt() directly
              // This avoids the Arduino wrapper that expects void(*)()
              InterruptManager::getInstance().attachInterrupt(pin, cpp_callback, intMode, "gpiochip0");
          },
          py::arg("pin"), py::arg("callback"), py::arg("mode"),
          "Attach an interrupt handler to a pin (RISING, FALLING, or CHANGE)");
    
    m.def("detachInterrupt",
          [](int pin) {
              InterruptManager::getInstance().detachInterrupt(pin);
              g_interrupt_callbacks.erase(pin);  // Clean up Python callback
          },
          py::arg("pin"),
          "Detach the interrupt handler from a pin");
    
    // ========================================================================
    // ADVANCED I/O FUNCTIONS
    // ========================================================================
    
    m.def("tone",
          [](int pin, unsigned int frequency, unsigned long duration) {
              tone(pin, frequency, duration);
          },
          py::arg("pin"), py::arg("frequency"), py::arg("duration") = 0,
          "Generate a tone of specified frequency on a pin (duration=0 for continuous)");
    
    m.def("noTone", &noTone,
          py::arg("pin"),
          "Stop tone generation on a pin");
    
    m.def("pulseIn", &pulseIn,
          py::arg("pin"), py::arg("state"), py::arg("timeout") = 1000000,
          "Measure the duration of a pulse on a pin (in microseconds)");
    
    m.def("shiftOut", &shiftOut,
          py::arg("dataPin"), py::arg("clockPin"), py::arg("bitOrder"), py::arg("value"),
          "Shift out a byte of data one bit at a time (MSBFIRST or LSBFIRST)");
    
    m.def("shiftIn", &shiftIn,
          py::arg("dataPin"), py::arg("clockPin"), py::arg("bitOrder"),
          "Shift in a byte of data one bit at a time (returns uint8_t)");
    
    // ========================================================================
    // MATH & UTILITY FUNCTIONS
    // ========================================================================
    
    m.def("map", 
          py::overload_cast<long, long, long, long, long>(&map),
          py::arg("value"), py::arg("fromLow"), py::arg("fromHigh"), 
          py::arg("toLow"), py::arg("toHigh"),
          "Re-map a number from one range to another");
    
    m.def("constrain", 
          py::overload_cast<int, int, int>(&constrain),
          py::arg("value"), py::arg("min"), py::arg("max"),
          "Constrain a value to be within a range");
    
    m.def("constrain", 
          py::overload_cast<long, long, long>(&constrain),
          py::arg("value"), py::arg("min"), py::arg("max"),
          "Constrain a value to be within a range (long)");
    
    m.def("constrain", 
          py::overload_cast<float, float, float>(&constrain),
          py::arg("value"), py::arg("min"), py::arg("max"),
          "Constrain a value to be within a range (float)");
    
    // sq is a template function - provide explicit overloads
    m.def("sq", [](int x) { return sq(x); }, py::arg("x"), "Square a number (int)");
    m.def("sq", [](double x) { return sq(x); }, py::arg("x"), "Square a number (double)");
    
    m.def("random", 
          py::overload_cast<long>(&random),
          py::arg("max"),
          "Generate a random number from 0 to max-1");
    
    m.def("random", 
          py::overload_cast<long, long>(&random),
          py::arg("min"), py::arg("max"),
          "Generate a random number from min to max-1");
    
    m.def("randomSeed", &randomSeed,
          py::arg("seed"),
          "Initialize the random number generator with a seed");
    
    // ========================================================================
    // BIT MANIPULATION FUNCTIONS
    // ========================================================================
    
    m.def("bit", &bit,
          py::arg("n"),
          "Compute the value of the bit at position n (1 << n)");
    
    m.def("bitRead", &bitRead,
          py::arg("value"), py::arg("bit"),
          "Read a specific bit from a number");
    
    m.def("bitWrite", &bitWrite,
          py::arg("value"), py::arg("bit"), py::arg("bitvalue"),
          "Write a specific bit in a number");
    
    m.def("bitSet", &bitSet,
          py::arg("value"), py::arg("bit"),
          "Set a specific bit in a number to 1");
    
    m.def("bitClear", &bitClear,
          py::arg("value"), py::arg("bit"),
          "Clear a specific bit in a number to 0");
    
    m.def("lowByte", &lowByte,
          py::arg("value"),
          "Extract the low byte from a 16-bit value");
    
    m.def("highByte", &highByte,
          py::arg("value"),
          "Extract the high byte from a 16-bit value");
    
    // ========================================================================
    // WIRE (I2C) CLASS
    // ========================================================================
    
    py::class_<pipinpp::WireClass>(m, "WireClass")
        .def("begin", py::overload_cast<>(&pipinpp::WireClass::begin),
             "Initialize I2C with auto-detected bus")
        .def("begin", py::overload_cast<int>(&pipinpp::WireClass::begin),
             py::arg("busNumber"),
             "Initialize I2C with specific bus number")
        .def("end", &pipinpp::WireClass::end,
             "Close I2C interface")
        .def("setClock", &pipinpp::WireClass::setClock,
             py::arg("frequency"),
             "Set I2C clock frequency in Hz")
        .def("getClock", &pipinpp::WireClass::getClock,
             "Get current I2C clock frequency")
        .def("beginTransmission", &pipinpp::WireClass::beginTransmission,
             py::arg("address"),
             "Start I2C transmission to device address")
        .def("write", py::overload_cast<uint8_t>(&pipinpp::WireClass::write),
             py::arg("data"),
             "Write a single byte")
        .def("write", 
             [](pipinpp::WireClass& self, py::bytes data) {
                 std::string str = data;
                 return self.write(reinterpret_cast<const uint8_t*>(str.data()), str.size());
             },
             py::arg("data"),
             "Write multiple bytes")
        .def("endTransmission",
             py::overload_cast<bool>(&pipinpp::WireClass::endTransmission),
             py::arg("sendStop") = true,
             "End I2C transmission and return status code")
        .def("requestFrom",
             py::overload_cast<uint8_t, size_t, bool>(&pipinpp::WireClass::requestFrom),
             py::arg("address"), py::arg("quantity"), py::arg("sendStop") = true,
             "Request bytes from I2C device")
        .def("available", &pipinpp::WireClass::available,
             "Get number of bytes available for reading")
        .def("read", &pipinpp::WireClass::read,
             "Read a single byte")
        .def("scan", &pipinpp::WireClass::scan,
             "Scan for I2C devices (returns list of addresses)")
        .def("readRegister", &pipinpp::WireClass::readRegister,
             py::arg("deviceAddress"), py::arg("registerAddress"),
             "Read a single byte from a register")
        .def("writeRegister", &pipinpp::WireClass::writeRegister,
             py::arg("deviceAddress"), py::arg("registerAddress"), py::arg("data"),
             "Write a single byte to a register");
    
    // Global Wire instance
    m.attr("Wire") = &pipinpp::Wire;
    
    // ========================================================================
    // SPI CLASS
    // ========================================================================
    
    py::class_<pipinpp::SPIClass>(m, "SPIClass")
        .def("begin", py::overload_cast<>(&pipinpp::SPIClass::begin),
             "Initialize SPI with default bus and CS")
        .def("begin", py::overload_cast<int, int>(&pipinpp::SPIClass::begin),
             py::arg("bus"), py::arg("cs"),
             "Initialize SPI with specific bus and CS pin")
        .def("end", &pipinpp::SPIClass::end,
             "Close SPI interface")
        .def("setDataMode", &pipinpp::SPIClass::setDataMode,
             py::arg("mode"),
             "Set SPI mode (SPI_MODE0, SPI_MODE1, SPI_MODE2, SPI_MODE3)")
        .def("setBitOrder", &pipinpp::SPIClass::setBitOrder,
             py::arg("order"),
             "Set bit order (MSBFIRST or LSBFIRST)")
        .def("setClockDivider", &pipinpp::SPIClass::setClockDivider,
             py::arg("divider"),
             "Set clock divider (SPI_CLOCK_DIV2 to SPI_CLOCK_DIV128)")
        .def("setClock", &pipinpp::SPIClass::setClock,
             py::arg("frequency"),
             "Set SPI clock frequency in Hz")
        .def("getClock", &pipinpp::SPIClass::getClock,
             "Get current SPI clock frequency")
        .def("transfer", py::overload_cast<uint8_t>(&pipinpp::SPIClass::transfer),
             py::arg("data"),
             "Transfer a single byte (returns received byte)")
        .def("transfer", 
             [](pipinpp::SPIClass& self, py::bytes data) -> py::bytes {
                 std::string str = data;
                 std::vector<uint8_t> buffer(str.begin(), str.end());
                 self.transfer(buffer.data(), buffer.size());
                 return py::bytes(reinterpret_cast<char*>(buffer.data()), buffer.size());
             },
             py::arg("data"),
             "Transfer multiple bytes (returns received bytes)");
    
    // Global SPI instance
    m.attr("SPI") = &pipinpp::SPI;
    
    // ========================================================================
    // SERIAL (UART) CLASS
    // ========================================================================
    
    py::class_<pipinpp::SerialPort>(m, "SerialPort")
        .def("begin", &pipinpp::SerialPort::begin,
             py::arg("baudRate"), py::arg("device") = "/dev/ttyUSB0",
             "Open serial port with specified baud rate")
        .def("end", &pipinpp::SerialPort::end,
             "Close serial port")
        .def("isOpen", &pipinpp::SerialPort::isOpen,
             "Check if serial port is open")
        .def("available", &pipinpp::SerialPort::available,
             "Get number of bytes available for reading")
        .def("read", &pipinpp::SerialPort::read,
             "Read a single byte (returns -1 if none available)")
        .def("peek", &pipinpp::SerialPort::peek,
             "Peek at next byte without removing it")
        .def("write", py::overload_cast<uint8_t>(&pipinpp::SerialPort::write),
             py::arg("byte"),
             "Write a single byte")
        .def("write", 
             [](pipinpp::SerialPort& self, py::bytes data) {
                 std::string str = data;
                 return self.write(reinterpret_cast<const uint8_t*>(str.data()), str.size());
             },
             py::arg("data"),
             "Write multiple bytes")
        .def("print", py::overload_cast<const std::string&>(&pipinpp::SerialPort::print),
             py::arg("data"),
             "Print string without newline")
        .def("print", py::overload_cast<int>(&pipinpp::SerialPort::print),
             py::arg("num"),
             "Print integer without newline")
        .def("println", py::overload_cast<const std::string&>(&pipinpp::SerialPort::println),
             py::arg("data"),
             "Print string with newline")
        .def("println", py::overload_cast<int>(&pipinpp::SerialPort::println),
             py::arg("num"),
             "Print integer with newline")
        .def("readString", &pipinpp::SerialPort::readString,
             "Read all available data as string")
        .def("readStringUntil", &pipinpp::SerialPort::readStringUntil,
             py::arg("terminator"),
             "Read string until terminator character")
        .def("setTimeout", &pipinpp::SerialPort::setTimeout,
             py::arg("timeout"),
             "Set read timeout in milliseconds")
        .def("flush", &pipinpp::SerialPort::flush,
             "Flush output buffer");
    
    // Global Serial instance
    m.attr("Serial") = &Serial;
    
    // ========================================================================
    // PLATFORM DETECTION
    // ========================================================================
    
    py::enum_<pipinpp::Platform>(m, "Platform")
        .value("UNKNOWN", pipinpp::Platform::UNKNOWN)
        .value("RASPBERRY_PI_3", pipinpp::Platform::RASPBERRY_PI_3)
        .value("RASPBERRY_PI_4", pipinpp::Platform::RASPBERRY_PI_4)
        .value("RASPBERRY_PI_5", pipinpp::Platform::RASPBERRY_PI_5)
        .value("RASPBERRY_PI_CM4", pipinpp::Platform::RASPBERRY_PI_CM4)
        .value("RASPBERRY_PI_ZERO", pipinpp::Platform::RASPBERRY_PI_ZERO)
        .value("RASPBERRY_PI_ZERO2", pipinpp::Platform::RASPBERRY_PI_ZERO2)
        .value("ORANGE_PI", pipinpp::Platform::ORANGE_PI)
        .value("BEAGLEBONE", pipinpp::Platform::BEAGLEBONE)
        .value("JETSON_NANO", pipinpp::Platform::JETSON_NANO)
        .export_values();
    
    py::class_<pipinpp::PlatformInfo, std::unique_ptr<pipinpp::PlatformInfo, py::nodelete>>(m, "PlatformInfo")
        .def_static("instance", &pipinpp::PlatformInfo::instance,
                   py::return_value_policy::reference,
                   "Get the singleton PlatformInfo instance")
        .def("getPlatform", &pipinpp::PlatformInfo::getPlatform,
             "Get detected platform")
        .def("getPlatformName", &pipinpp::PlatformInfo::getPlatformName,
             "Get platform name as string")
        .def("getKernelVersion", &pipinpp::PlatformInfo::getKernelVersion,
             "Get Linux kernel version")
        .def("getLibgpiodVersion", &pipinpp::PlatformInfo::getLibgpiodVersion,
             "Get libgpiod version")
        .def("getDefaultGPIOChip", &pipinpp::PlatformInfo::getDefaultGPIOChip,
             "Get default GPIO chip path")
        .def("getDefaultI2CBus", &pipinpp::PlatformInfo::getDefaultI2CBus,
             "Get default I2C bus number")
        .def("printInfo", &pipinpp::PlatformInfo::printInfo,
             "Print platform information to console");
    
    // ========================================================================
    // VERSION INFO
    // ========================================================================
    
    m.attr("__version__") = "0.4.0";
    m.attr("__author__") = "Barbatos6669";
    m.attr("__license__") = "MIT";
}
