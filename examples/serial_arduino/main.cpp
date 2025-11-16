/**
 * @file main.cpp
 * @brief Arduino-Pi Serial Communication Example
 * @author Barbatos6669
 * @date 2025-11-16
 * 
 * Demonstrates communicating with an Arduino over serial port.
 * Perfect for mecanum wheel robots where Arduino handles motors/sensors
 * and Raspberry Pi handles high-level logic and decision making.
 * 
 * Hardware Setup:
 * - Connect Arduino to Raspberry Pi via USB cable
 * - Arduino will appear as /dev/ttyUSB0 or /dev/ttyACM0
 * - Ensure user is in 'dialout' group: sudo usermod -a -G dialout $USER
 * 
 * Arduino Side (upload this to your Arduino):
 * ```
 * void setup() {
 *     Serial.begin(9600);
 *     pinMode(LED_BUILTIN, OUTPUT);
 * }
 * 
 * void loop() {
 *     if (Serial.available()) {
 *         String cmd = Serial.readStringUntil('\n');
 *         cmd.trim();
 *         
 *         if (cmd == "LED_ON") {
 *             digitalWrite(LED_BUILTIN, HIGH);
 *             Serial.println("OK:LED_ON");
 *         } else if (cmd == "LED_OFF") {
 *             digitalWrite(LED_BUILTIN, LOW);
 *             Serial.println("OK:LED_OFF");
 *         } else if (cmd == "STATUS") {
 *             Serial.println("READY");
 *         } else {
 *             Serial.println("ERROR:UNKNOWN_COMMAND");
 *         }
 *     }
 * }
 * ```
 */

#include "Serial.hpp"
#include "ArduinoCompat.hpp"
#include <iostream>
#include <string>

using namespace pipinpp;

int main() 
{
    std::cout << "=== PiPinPP Serial Communication Example ===\n";
    std::cout << "Demonstrating Arduino-Pi communication for mecanum robot\n\n";
    
    // Try common Arduino serial device paths
    std::string device = "/dev/ttyUSB0";  // FTDI, CH340 USB-to-serial adapters
    if (!Serial.begin(9600, device)) {
        device = "/dev/ttyACM0";  // Arduino Uno/Mega native USB
        if (!Serial.begin(9600, device)) {
            std::cerr << "ERROR: Could not open serial port\n";
            std::cerr << "Tried: /dev/ttyUSB0 and /dev/ttyACM0\n";
            std::cerr << "Check connections and permissions (dialout group)\n";
            return 1;
        }
    }
    
    std::cout << "✓ Connected to Arduino on " << device << " at 9600 baud\n\n";
    
    // Wait for Arduino to reset (some boards reset on serial connect)
    std::cout << "Waiting for Arduino to initialize...\n";
    delay(2000);
    
    // Clear any garbage from serial buffer
    while (Serial.available()) {
        Serial.read();
    }
    
    // Test 1: Query Arduino status
    std::cout << "\n--- Test 1: Query Status ---\n";
    Serial.println("STATUS");
    Serial.flush();
    
    delay(100);  // Give Arduino time to respond
    
    if (Serial.available()) {
        std::string response = Serial.readStringUntil('\n');
        std::cout << "Arduino response: " << response << "\n";
    } else {
        std::cout << "No response (check Arduino sketch uploaded)\n";
    }
    
    // Test 2: Send commands to Arduino
    std::cout << "\n--- Test 2: LED Control Commands ---\n";
    
    std::cout << "Turning LED ON...\n";
    Serial.println("LED_ON");
    Serial.flush();
    delay(100);
    if (Serial.available()) {
        std::cout << "  Arduino: " << Serial.readStringUntil('\n') << "\n";
    }
    
    delay(1000);
    
    std::cout << "Turning LED OFF...\n";
    Serial.println("LED_OFF");
    Serial.flush();
    delay(100);
    if (Serial.available()) {
        std::cout << "  Arduino: " << Serial.readStringUntil('\n') << "\n";
    }
    
    // Test 3: Rapid command sequence (simulating robot control)
    std::cout << "\n--- Test 3: Rapid Command Sequence ---\n";
    std::cout << "Simulating mecanum robot movement commands...\n";
    
    const char* commands[] = {
        "MOVE_FORWARD",
        "MOVE_BACKWARD", 
        "STRAFE_LEFT",
        "STRAFE_RIGHT",
        "ROTATE_CW",
        "ROTATE_CCW",
        "STOP"
    };
    
    for (const char* cmd : commands) {
        std::cout << "  Sending: " << cmd << "\n";
        Serial.println(cmd);
        Serial.flush();
        
        delay(50);  // Small delay for Arduino to process
        
        // Read response (if any)
        if (Serial.available()) {
            std::string response = Serial.readStringUntil('\n');
            std::cout << "    Response: " << response << "\n";
        }
        
        delay(200);  // Delay between commands
    }
    
    // Test 4: Sensor data request (simulate reading encoders/sensors)
    std::cout << "\n--- Test 4: Sensor Data Request ---\n";
    Serial.println("GET_ENCODERS");
    Serial.flush();
    delay(100);
    
    if (Serial.available()) {
        std::string data = Serial.readStringUntil('\n');
        std::cout << "Encoder data: " << data << "\n";
    } else {
        std::cout << "No encoder data (extend Arduino sketch for full implementation)\n";
    }
    
    // Test 5: Binary data transmission
    std::cout << "\n--- Test 5: Binary Data ---\n";
    uint8_t binaryData[] = {0x01, 0x02, 0x03, 0xFF, 0xAA};
    std::cout << "Sending 5 binary bytes...\n";
    Serial.write(binaryData, sizeof(binaryData));
    Serial.flush();
    
    delay(100);
    std::cout << "Bytes available: " << Serial.available() << "\n";
    
    // Cleanup
    Serial.end();
    std::cout << "\n✓ Serial port closed\n";
    std::cout << "\n=== Mecanum Robot Integration Notes ===\n";
    std::cout << "For your mecanum bot:\n";
    std::cout << "1. Arduino handles: Motor control, encoder reading, sensor polling\n";
    std::cout << "2. Raspberry Pi handles: Path planning, CV/AI, WiFi/network\n";
    std::cout << "3. Communication protocol: Simple text commands (easy to debug)\n";
    std::cout << "4. Response format: OK:<data> for success, ERROR:<msg> for errors\n";
    std::cout << "5. Recommended baud: 115200 for faster data transfer\n";
    std::cout << "6. Add CRC/checksum for critical commands (motor control)\n";
    
    return 0;
}
