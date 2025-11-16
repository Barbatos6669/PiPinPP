/**
 * @file pipinpp.cpp
 * @brief PiPinPP Command-Line Interface Tool
 * 
 * A versatile CLI tool for GPIO manipulation, testing, and debugging.
 * Similar to WiringPi's 'gpio' command but designed for PiPinPP.
 * 
 * Usage:
 *   pipinpp info              - Show platform and hardware information
 *   pipinpp read <pin>        - Read pin value (returns 0 or 1)
 *   pipinpp write <pin> <val> - Write pin value (0/LOW or 1/HIGH)
 *   pipinpp mode <pin> <mode> - Set pin mode (in/out/up/down)
 *   pipinpp pwm <pin> <value> - Set PWM duty cycle (0-255)
 *   pipinpp toggle <pin>      - Toggle pin state
 *   pipinpp blink <pin> <ms>  - Blink pin at interval
 *   pipinpp i2c scan          - Scan I2C bus for devices
 *   pipinpp i2c read <addr> <reg> - Read I2C register
 *   pipinpp i2c write <addr> <reg> <val> - Write I2C register
 *   pipinpp spi test          - SPI loopback test
 *   pipinpp test              - Run all self-tests
 * 
 * @copyright Copyright (c) 2025 PiPinPP Project
 * @license MIT License
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdlib>
#include <csignal>
#include <unistd.h>

#include "ArduinoCompat.hpp"
#include "Wire.hpp"
#include "SPI.hpp"
#include "platform.hpp"

using namespace std;
using namespace pipinpp;

// Version information
const string VERSION = "0.4.0";
const string BUILD_DATE = __DATE__;

// Color codes for output
const string COLOR_RESET = "\033[0m";
const string COLOR_GREEN = "\033[32m";
const string COLOR_RED = "\033[31m";
const string COLOR_YELLOW = "\033[33m";
const string COLOR_BLUE = "\033[34m";
const string COLOR_CYAN = "\033[36m";
const string COLOR_BOLD = "\033[1m";

// Global flag for interrupt handling
volatile bool running = true;

void signal_handler(int signum) 
{
    running = false;
    cout << "\nInterrupted. Cleaning up...\n";
}

void print_banner() 
{
    cout << COLOR_BOLD << COLOR_CYAN
         << "╔═══════════════════════════════════════╗\n"
         << "║         PiPinPP CLI Tool v" << VERSION << "       ║\n"
         << "║   GPIO & Peripheral Control Utility   ║\n"
         << "╚═══════════════════════════════════════╝\n"
         << COLOR_RESET << endl;
}

void print_usage() 
{
    cout << "Usage: pipinpp <command> [arguments]\n\n";
    cout << COLOR_BOLD << "Information Commands:\n" << COLOR_RESET;
    cout << "  info                    Show platform and hardware details\n";
    cout << "  version                 Show version information\n";
    cout << "  help                    Show this help message\n\n";
    
    cout << COLOR_BOLD << "GPIO Commands:\n" << COLOR_RESET;
    cout << "  read <pin>              Read digital pin (0 or 1)\n";
    cout << "  write <pin> <value>     Write digital pin (0/LOW or 1/HIGH)\n";
    cout << "  mode <pin> <mode>       Set pin mode (in, out, up, down)\n";
    cout << "  toggle <pin>            Toggle pin state\n";
    cout << "  blink <pin> [ms]        Blink pin (default: 500ms)\n";
    cout << "  pwm <pin> <value>       Set PWM duty cycle (0-255)\n\n";
    
    cout << COLOR_BOLD << "I2C Commands:\n" << COLOR_RESET;
    cout << "  i2c scan                Scan I2C bus for devices\n";
    cout << "  i2c read <addr> <reg>   Read I2C register (hex)\n";
    cout << "  i2c write <addr> <reg> <val>  Write I2C register (hex)\n\n";
    
    cout << COLOR_BOLD << "SPI Commands:\n" << COLOR_RESET;
    cout << "  spi test                SPI loopback test\n";
    cout << "  spi send <byte>         Send SPI byte (hex)\n\n";
    
    cout << COLOR_BOLD << "Testing Commands:\n" << COLOR_RESET;
    cout << "  test                    Run all self-tests\n";
    cout << "  benchmark               GPIO speed benchmark\n\n";
    
    cout << COLOR_BOLD << "Examples:\n" << COLOR_RESET;
    cout << "  pipinpp mode 17 out     # Set GPIO17 as output\n";
    cout << "  pipinpp write 17 1      # Set GPIO17 HIGH\n";
    cout << "  pipinpp read 18         # Read GPIO18 state\n";
    cout << "  pipinpp blink 17 1000   # Blink GPIO17 at 1 second intervals\n";
    cout << "  pipinpp pwm 18 128      # Set GPIO18 PWM to 50% duty cycle\n";
    cout << "  pipinpp i2c scan        # Scan I2C bus for devices\n";
}

void cmd_info() 
{
    PlatformInfo& platform = PlatformInfo::instance();
    
    cout << COLOR_BOLD << "\nPlatform Information:\n" << COLOR_RESET;
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    platform.printInfo();
    
    cout << "\n" << COLOR_BOLD << "Recommended Defaults:\n" << COLOR_RESET;
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << "  GPIO Chip:     " << platform.getDefaultGPIOChip() << "\n";
    cout << "  I2C Bus:       /dev/i2c-" << platform.getDefaultI2CBus() << "\n";
    
    cout << endl;
}

void cmd_version() 
{
    cout << "PiPinPP CLI Tool v" << VERSION << "\n";
    cout << "Build Date: " << BUILD_DATE << "\n";
    cout << "PiPinPP Library Version: 0.4.0\n";
    cout << "Copyright (c) 2025 PiPinPP Project\n";
    cout << "License: MIT\n";
}

void cmd_read(int pin) 
{
    try {
        pinMode(pin, INPUT);
        int value = digitalRead(pin);
        cout << value << endl;
        exit(value);  // Exit with pin value
    } catch (const exception& e) {
        cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        exit(1);
    }
}

void cmd_write(int pin, int value) 
{
    try {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, value ? HIGH : LOW);
        cout << COLOR_GREEN << "GPIO" << pin << " = " << (value ? "HIGH" : "LOW") << COLOR_RESET << endl;
    } catch (const exception& e) {
        cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        exit(1);
    }
}

void cmd_mode(int pin, const string& mode) 
{
    try {
        if (mode == "in" || mode == "input") {
            pinMode(pin, INPUT);
            cout << COLOR_GREEN << "GPIO" << pin << " set to INPUT" << COLOR_RESET << endl;
        }
        else if (mode == "out" || mode == "output") {
            pinMode(pin, OUTPUT);
            cout << COLOR_GREEN << "GPIO" << pin << " set to OUTPUT" << COLOR_RESET << endl;
        }
        else if (mode == "up" || mode == "pullup") {
            pinMode(pin, INPUT_PULLUP);
            cout << COLOR_GREEN << "GPIO" << pin << " set to INPUT_PULLUP" << COLOR_RESET << endl;
        }
        else if (mode == "down" || mode == "pulldown") {
            pinMode(pin, INPUT_PULLDOWN);
            cout << COLOR_GREEN << "GPIO" << pin << " set to INPUT_PULLDOWN" << COLOR_RESET << endl;
        }
        else {
            cerr << COLOR_RED << "Error: Invalid mode '" << mode << "'" << COLOR_RESET << endl;
            cerr << "Valid modes: in, out, up, down\n";
            exit(1);
        }
    } catch (const exception& e) {
        cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        exit(1);
    }
}

void cmd_toggle(int pin) 
{
    try {
        pinMode(pin, OUTPUT);
        int current = digitalRead(pin);
        int newValue = !current;
        digitalWrite(pin, newValue ? HIGH : LOW);
        cout << COLOR_GREEN << "GPIO" << pin << " toggled: " 
             << current << " → " << newValue << COLOR_RESET << endl;
    } catch (const exception& e) {
        cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        exit(1);
    }
}

void cmd_blink(int pin, int interval_ms) 
{
    try {
        pinMode(pin, OUTPUT);
        signal(SIGINT, signal_handler);
        
        cout << "Blinking GPIO" << pin << " at " << interval_ms << "ms intervals\n";
        cout << "Press Ctrl+C to stop...\n";
        
        while (running) {
            digitalWrite(pin, HIGH);
            cout << "█" << flush;
            delay(interval_ms);
            
            digitalWrite(pin, LOW);
            cout << "░" << flush;
            delay(interval_ms);
        }
        
        digitalWrite(pin, LOW);
        cout << "\nStopped. GPIO" << pin << " set LOW.\n";
        
    } catch (const exception& e) {
        cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        exit(1);
    }
}

void cmd_pwm(int pin, int value) 
{
    if (value < 0 || value > 255) {
        cerr << COLOR_RED << "Error: PWM value must be 0-255" << COLOR_RESET << endl;
        exit(1);
    }
    
    try {
        analogWrite(pin, value);
        double percent = (value / 255.0) * 100.0;
        cout << COLOR_GREEN << "GPIO" << pin << " PWM = " << value 
             << " (" << fixed << setprecision(1) << percent << "%)" 
             << COLOR_RESET << endl;
    } catch (const exception& e) {
        cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        exit(1);
    }
}

void cmd_i2c_scan() 
{
    try {
        if (!Wire.begin()) {
            cerr << COLOR_RED << "Error: Failed to initialize I2C" << COLOR_RESET << endl;
            exit(1);
        }
        
        cout << "Scanning I2C bus...\n";
        cout << "     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n";
        cout << "00:         ";
        
        int count = 0;
        for (int addr = 3; addr < 0x78; addr++) {
            if (addr % 16 == 0) {
                cout << "\n" << hex << setw(2) << setfill('0') << (addr & 0xF0) << ": ";
            }
            
            if (Wire.exists(addr)) {
                cout << COLOR_GREEN << setw(2) << setfill('0') << hex << addr << COLOR_RESET << " ";
                count++;
            } else {
                cout << "-- ";
            }
        }
        
        cout << dec << "\n\n";
        if (count > 0) {
            cout << COLOR_GREEN << "Found " << count << " device(s)" << COLOR_RESET << endl;
        } else {
            cout << COLOR_YELLOW << "No I2C devices found" << COLOR_RESET << endl;
        }
        
        Wire.end();
        
    } catch (const exception& e) {
        cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        exit(1);
    }
}

void cmd_i2c_read(uint8_t addr, uint8_t reg) 
{
    try {
        if (!Wire.begin()) {
            cerr << COLOR_RED << "Error: Failed to initialize I2C" << COLOR_RESET << endl;
            exit(1);
        }
        
        uint8_t value = Wire.readRegister(addr, reg);
        cout << "0x" << hex << setw(2) << setfill('0') << (int)value << dec << endl;
        
        Wire.end();
        
    } catch (const exception& e) {
        cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        exit(1);
    }
}

void cmd_i2c_write(uint8_t addr, uint8_t reg, uint8_t value) 
{
    try {
        if (!Wire.begin()) {
            cerr << COLOR_RED << "Error: Failed to initialize I2C" << COLOR_RESET << endl;
            exit(1);
        }
        
        Wire.writeRegister(addr, reg, value);
        cout << COLOR_GREEN << "Wrote 0x" << hex << setw(2) << setfill('0') << (int)value 
             << " to register 0x" << setw(2) << (int)reg 
             << " on device 0x" << setw(2) << (int)addr << dec << COLOR_RESET << endl;
        
        Wire.end();
        
    } catch (const exception& e) {
        cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        exit(1);
    }
}

void cmd_benchmark() 
{
    const int TEST_PIN = 17;
    const int ITERATIONS = 100000;
    
    try {
        pinMode(TEST_PIN, OUTPUT);
        
        cout << "Running GPIO speed benchmark...\n";
        cout << "Pin: GPIO" << TEST_PIN << "\n";
        cout << "Iterations: " << ITERATIONS << "\n\n";
        
        // Benchmark digitalWrite
        unsigned long start = micros();
        for (int i = 0; i < ITERATIONS; i++) {
            digitalWrite(TEST_PIN, HIGH);
            digitalWrite(TEST_PIN, LOW);
        }
        unsigned long elapsed = micros() - start;
        
        double togglesPerSecond = (ITERATIONS * 2.0) / (elapsed / 1000000.0);
        double usPerToggle = elapsed / (ITERATIONS * 2.0);
        
        cout << COLOR_BOLD << "Results:\n" << COLOR_RESET;
        cout << "Total time: " << elapsed << " μs\n";
        cout << "Toggles per second: " << COLOR_GREEN << fixed << setprecision(0) << togglesPerSecond << COLOR_RESET << "\n";
        cout << "Time per toggle: " << COLOR_CYAN << fixed << setprecision(2) << usPerToggle << " μs" << COLOR_RESET << "\n";
        
    } catch (const exception& e) {
        cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        exit(1);
    }
}

int main(int argc, char* argv[]) 
{
    if (argc < 2) {
        print_banner();
        print_usage();
        return 1;
    }
    
    string command = argv[1];
    
    try {
        if (command == "help" || command == "--help" || command == "-h") {
            print_banner();
            print_usage();
        }
        else if (command == "version" || command == "--version" || command == "-v") {
            cmd_version();
        }
        else if (command == "info") {
            cmd_info();
        }
        else if (command == "read") {
            if (argc < 3) {
                cerr << "Usage: pipinpp read <pin>\n";
                return 1;
            }
            cmd_read(atoi(argv[2]));
        }
        else if (command == "write") {
            if (argc < 4) {
                cerr << "Usage: pipinpp write <pin> <value>\n";
                return 1;
            }
            int value = (string(argv[3]) == "HIGH" || string(argv[3]) == "high") ? 1 : atoi(argv[3]);
            cmd_write(atoi(argv[2]), value);
        }
        else if (command == "mode") {
            if (argc < 4) {
                cerr << "Usage: pipinpp mode <pin> <mode>\n";
                cerr << "Modes: in, out, up, down\n";
                return 1;
            }
            cmd_mode(atoi(argv[2]), argv[3]);
        }
        else if (command == "toggle") {
            if (argc < 3) {
                cerr << "Usage: pipinpp toggle <pin>\n";
                return 1;
            }
            cmd_toggle(atoi(argv[2]));
        }
        else if (command == "blink") {
            if (argc < 3) {
                cerr << "Usage: pipinpp blink <pin> [interval_ms]\n";
                return 1;
            }
            int interval = (argc >= 4) ? atoi(argv[3]) : 500;
            cmd_blink(atoi(argv[2]), interval);
        }
        else if (command == "pwm") {
            if (argc < 4) {
                cerr << "Usage: pipinpp pwm <pin> <value>\n";
                cerr << "Value: 0-255 (0=0%, 255=100%)\n";
                return 1;
            }
            cmd_pwm(atoi(argv[2]), atoi(argv[3]));
        }
        else if (command == "i2c") {
            if (argc < 3) {
                cerr << "Usage: pipinpp i2c <scan|read|write>\n";
                return 1;
            }
            string subcmd = argv[2];
            if (subcmd == "scan") {
                cmd_i2c_scan();
            }
            else if (subcmd == "read") {
                if (argc < 5) {
                    cerr << "Usage: pipinpp i2c read <addr> <reg>\n";
                    cerr << "Example: pipinpp i2c read 0x76 0xD0\n";
                    return 1;
                }
                uint8_t addr = (uint8_t)strtol(argv[3], nullptr, 16);
                uint8_t reg = (uint8_t)strtol(argv[4], nullptr, 16);
                cmd_i2c_read(addr, reg);
            }
            else if (subcmd == "write") {
                if (argc < 6) {
                    cerr << "Usage: pipinpp i2c write <addr> <reg> <value>\n";
                    cerr << "Example: pipinpp i2c write 0x76 0xF4 0x27\n";
                    return 1;
                }
                uint8_t addr = (uint8_t)strtol(argv[3], nullptr, 16);
                uint8_t reg = (uint8_t)strtol(argv[4], nullptr, 16);
                uint8_t value = (uint8_t)strtol(argv[5], nullptr, 16);
                cmd_i2c_write(addr, reg, value);
            }
            else {
                cerr << "Unknown i2c subcommand: " << subcmd << "\n";
                return 1;
            }
        }
        else if (command == "benchmark") {
            cmd_benchmark();
        }
        else {
            cerr << COLOR_RED << "Unknown command: " << command << COLOR_RESET << "\n\n";
            print_usage();
            return 1;
        }
        
    } catch (const exception& e) {
        cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << endl;
        return 1;
    }
    
    return 0;
}
