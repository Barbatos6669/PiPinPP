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
 * @file i2c_ssd1306/main.cpp
 * @brief SSD1306 OLED display example (128x64)
 * 
 * Demonstrates controlling a SSD1306 OLED display using the Wire library.
 * The SSD1306 is a popular monochrome OLED driver chip commonly used in
 * 128x64 and 128x32 displays.
 * 
 * Features demonstrated:
 * - Display initialization
 * - Basic drawing (pixels, lines, rectangles)
 * - Text rendering (simple font)
 * - Full framebuffer control
 * - Animation and scrolling
 * 
 * Hardware setup:
 * - SSD1306 VCC  → Pi 3.3V
 * - SSD1306 GND  → Pi GND
 * - SSD1306 SDA  → Pi GPIO 2 (Pin 3)
 * - SSD1306 SCL  → Pi GPIO 3 (Pin 5)
 * 
 * Common I2C addresses:
 * - 0x3C (most common)
 * - 0x3D (alternate)
 * 
 * Display sizes:
 * - 128x64 pixels (most common)
 * - 128x32 pixels (smaller variant)
 * 
 * @author PiPinPP Team
 * @version 0.4.0
 * @date 2025-11-05
 */

#include <Wire.hpp>
#include <ArduinoCompat.hpp>
#include <iostream>
#include <vector>
#include <cstring>
#include <cmath>

using namespace std;

// SSD1306 I2C addresses
const uint8_t SSD1306_ADDR = 0x3C;

// Display dimensions
const int DISPLAY_WIDTH = 128;
const int DISPLAY_HEIGHT = 64;
const int BUFFER_SIZE = (DISPLAY_WIDTH * DISPLAY_HEIGHT) / 8;

// SSD1306 Commands
const uint8_t CMD_DISPLAY_OFF        = 0xAE;
const uint8_t CMD_DISPLAY_ON         = 0xAF;
const uint8_t CMD_SET_CONTRAST       = 0x81;
const uint8_t CMD_NORMAL_DISPLAY     = 0xA6;
const uint8_t CMD_INVERSE_DISPLAY    = 0xA7;
const uint8_t CMD_SET_MEMORY_MODE    = 0x20;
const uint8_t CMD_SET_COLUMN_ADDR    = 0x21;
const uint8_t CMD_SET_PAGE_ADDR      = 0x22;
const uint8_t CMD_SET_START_LINE     = 0x40;
const uint8_t CMD_SET_SEGMENT_REMAP  = 0xA1;
const uint8_t CMD_SET_MULTIPLEX      = 0xA8;
const uint8_t CMD_SET_COM_SCAN_DEC   = 0xC8;
const uint8_t CMD_SET_DISPLAY_OFFSET = 0xD3;
const uint8_t CMD_SET_COM_PINS       = 0xDA;
const uint8_t CMD_SET_CLOCK_DIV      = 0xD5;
const uint8_t CMD_SET_PRECHARGE      = 0xD9;
const uint8_t CMD_SET_VCOM_DESELECT  = 0xDB;
const uint8_t CMD_CHARGE_PUMP        = 0x8D;

// Control bytes
const uint8_t CONTROL_CMD_STREAM  = 0x00;
const uint8_t CONTROL_DATA_STREAM = 0x40;

class SSD1306 {
private:
    uint8_t address_;
    vector<uint8_t> buffer_;
    
    bool sendCommand(uint8_t cmd) {
        Wire.beginTransmission(address_);
        Wire.write(CONTROL_CMD_STREAM);
        Wire.write(cmd);
        return (Wire.endTransmission(true) == 0);
    }
    
    bool sendCommands(const uint8_t* cmds, size_t count) {
        Wire.beginTransmission(address_);
        Wire.write(CONTROL_CMD_STREAM);
        for (size_t i = 0; i < count; i++) {
            Wire.write(cmds[i]);
        }
        return (Wire.endTransmission(true) == 0);
    }

public:
    SSD1306(uint8_t addr = SSD1306_ADDR) : address_(addr) {
        buffer_.resize(BUFFER_SIZE, 0);
    }
    
    bool begin() {
        // Initialization sequence for 128x64 display
        uint8_t init_sequence[] = {
            CMD_DISPLAY_OFF,
            CMD_SET_CLOCK_DIV, 0x80,
            CMD_SET_MULTIPLEX, 0x3F,
            CMD_SET_DISPLAY_OFFSET, 0x00,
            CMD_SET_START_LINE | 0x00,
            CMD_CHARGE_PUMP, 0x14,
            CMD_SET_MEMORY_MODE, 0x00,
            CMD_SET_SEGMENT_REMAP,
            CMD_SET_COM_SCAN_DEC,
            CMD_SET_COM_PINS, 0x12,
            CMD_SET_CONTRAST, 0xCF,
            CMD_SET_PRECHARGE, 0xF1,
            CMD_SET_VCOM_DESELECT, 0x40,
            CMD_NORMAL_DISPLAY,
            CMD_DISPLAY_ON
        };
        
        return sendCommands(init_sequence, sizeof(init_sequence));
    }
    
    void clear() {
        memset(buffer_.data(), 0, BUFFER_SIZE);
    }
    
    void fill() {
        memset(buffer_.data(), 0xFF, BUFFER_SIZE);
    }
    
    void setPixel(int x, int y, bool on = true) {
        if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT) {
            return;
        }
        
        int byte_index = x + (y / 8) * DISPLAY_WIDTH;
        int bit_index = y % 8;
        
        if (on) {
            buffer_[byte_index] |= (1 << bit_index);
        } else {
            buffer_[byte_index] &= ~(1 << bit_index);
        }
    }
    
    void drawLine(int x0, int y0, int x1, int y1) {
        // Bresenham's line algorithm
        int dx = abs(x1 - x0);
        int dy = abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;
        
        while (true) {
            setPixel(x0, y0);
            
            if (x0 == x1 && y0 == y1) break;
            
            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y0 += sy;
            }
        }
    }
    
    void drawRect(int x, int y, int w, int h, bool fill = false) {
        if (fill) {
            for (int i = 0; i < h; i++) {
                for (int j = 0; j < w; j++) {
                    setPixel(x + j, y + i);
                }
            }
        } else {
            drawLine(x, y, x + w - 1, y);
            drawLine(x + w - 1, y, x + w - 1, y + h - 1);
            drawLine(x + w - 1, y + h - 1, x, y + h - 1);
            drawLine(x, y + h - 1, x, y);
        }
    }
    
    void drawCircle(int x0, int y0, int radius) {
        int x = radius;
        int y = 0;
        int err = 0;
        
        while (x >= y) {
            setPixel(x0 + x, y0 + y);
            setPixel(x0 + y, y0 + x);
            setPixel(x0 - y, y0 + x);
            setPixel(x0 - x, y0 + y);
            setPixel(x0 - x, y0 - y);
            setPixel(x0 - y, y0 - x);
            setPixel(x0 + y, y0 - x);
            setPixel(x0 + x, y0 - y);
            
            y += 1;
            if (err <= 0) {
                err += 2 * y + 1;
            }
            if (err > 0) {
                x -= 1;
                err -= 2 * x + 1;
            }
        }
    }
    
    void drawChar(int x, int y, char c) {
        // Simple 5x8 font (partial implementation)
        const uint8_t font[][5] = {
            {0x7E, 0x11, 0x11, 0x11, 0x7E},  // A
            {0x7F, 0x49, 0x49, 0x49, 0x36},  // B
            {0x3E, 0x41, 0x41, 0x41, 0x22},  // C
            {0x00, 0x00, 0x00, 0x00, 0x00},  // Space (simplified)
        };
        
        int index = -1;
        if (c >= 'A' && c <= 'C') index = c - 'A';
        else if (c == ' ') index = 3;
        
        if (index >= 0) {
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 8; j++) {
                    if (font[index][i] & (1 << j)) {
                        setPixel(x + i, y + j);
                    }
                }
            }
        }
    }
    
    void drawText(int x, int y, const char* text) {
        int pos = x;
        while (*text) {
            drawChar(pos, y, *text);
            pos += 6;  // 5 pixels + 1 spacing
            text++;
        }
    }
    
    bool display() {
        // Set column and page addresses
        uint8_t addr_commands[] = {
            CMD_SET_COLUMN_ADDR, 0, 127,
            CMD_SET_PAGE_ADDR, 0, 7
        };
        
        if (!sendCommands(addr_commands, sizeof(addr_commands))) {
            return false;
        }
        
        // Send framebuffer data in chunks
        const int CHUNK_SIZE = 16;
        for (int i = 0; i < BUFFER_SIZE; i += CHUNK_SIZE) {
            int chunk_len = min(CHUNK_SIZE, BUFFER_SIZE - i);
            
            Wire.beginTransmission(address_);
            Wire.write(CONTROL_DATA_STREAM);
            Wire.write(&buffer_[i], chunk_len);
            
            if (Wire.endTransmission(true) != 0) {
                return false;
            }
        }
        
        return true;
    }
    
    void setContrast(uint8_t contrast) {
        uint8_t cmds[] = {CMD_SET_CONTRAST, contrast};
        sendCommands(cmds, 2);
    }
    
    void invert(bool inv) {
        sendCommand(inv ? CMD_INVERSE_DISPLAY : CMD_NORMAL_DISPLAY);
    }
};

void demoBootScreen(SSD1306& display) {
    cout << "Boot screen animation...\n";
    
    display.clear();
    
    // Draw title
    display.drawText(20, 10, "PIPINPP");
    display.display();
    delay(1000);
    
    // Draw version
    display.drawText(30, 30, "V0.4.0");
    display.display();
    delay(1000);
    
    // Draw progress bar
    for (int i = 0; i <= 100; i += 5) {
        int width = (i * 100) / 100;
        display.drawRect(14, 50, 102, 10);
        display.drawRect(15, 51, width, 8, true);
        display.display();
        delay(50);
    }
    
    delay(500);
}

void demoShapes(SSD1306& display) {
    cout << "Shapes demonstration...\n";
    
    // Rectangles
    display.clear();
    for (int i = 0; i < 5; i++) {
        display.drawRect(10 + i * 5, 10 + i * 5, 60 - i * 10, 40 - i * 10);
    }
    display.display();
    delay(2000);
    
    // Circles
    display.clear();
    for (int i = 0; i < 4; i++) {
        display.drawCircle(32 + i * 32, 32, 15);
    }
    display.display();
    delay(2000);
    
    // Lines
    display.clear();
    for (int i = 0; i < DISPLAY_WIDTH; i += 8) {
        display.drawLine(0, 0, i, DISPLAY_HEIGHT - 1);
        display.drawLine(DISPLAY_WIDTH - 1, 0, i, DISPLAY_HEIGHT - 1);
    }
    display.display();
    delay(2000);
}

void demoAnimation(SSD1306& display) {
    cout << "Animation demonstration...\n";
    
    // Bouncing ball
    int x = 64, y = 32;
    int vx = 2, vy = 2;
    int radius = 5;
    
    for (int frame = 0; frame < 200; frame++) {
        display.clear();
        
        // Update position
        x += vx;
        y += vy;
        
        // Bounce off edges
        if (x - radius < 0 || x + radius >= DISPLAY_WIDTH) vx = -vx;
        if (y - radius < 0 || y + radius >= DISPLAY_HEIGHT) vy = -vy;
        
        // Draw ball
        display.drawCircle(x, y, radius);
        
        // Draw borders
        display.drawRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
        
        display.display();
        delay(20);
    }
}

void demoScrollingText(SSD1306& display) {
    cout << "Scrolling text demonstration...\n";
    
    const char* message = "  RASPBERRY PI GPIO  ";
    int scroll_pos = DISPLAY_WIDTH;
    
    for (int i = 0; i < 300; i++) {
        display.clear();
        
        display.drawText(scroll_pos, 28, message);
        
        display.display();
        
        scroll_pos -= 2;
        if (scroll_pos < -200) {
            scroll_pos = DISPLAY_WIDTH;
        }
        
        delay(20);
    }
}

int main() {
    cout << "\n";
    cout << "╔══════════════════════════════════════════════════════════════╗\n";
    cout << "║            SSD1306 OLED Display Demo (I2C)                   ║\n";
    cout << "║                                                              ║\n";
    cout << "║  128x64 monochrome OLED graphics demonstration              ║\n";
    cout << "║  Version 0.4.0 - November 5, 2025                           ║\n";
    cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
    
    // Initialize I2C
    if (!Wire.begin()) {
        cerr << "ERROR: Failed to initialize I2C\n";
        return 1;
    }
    
    Wire.setClock(400000);  // 400kHz fast mode
    cout << "I2C initialized at 400 kHz\n\n";
    
    // Check if display exists
    if (!Wire.exists(SSD1306_ADDR)) {
        cerr << "ERROR: SSD1306 not found at address 0x" << hex << (int)SSD1306_ADDR << dec << "\n";
        cerr << "\nTroubleshooting:\n";
        cerr << "  1. Check wiring (SDA=GPIO2, SCL=GPIO3)\n";
        cerr << "  2. Verify power connections (3.3V, GND)\n";
        cerr << "  3. Try address 0x3D if 0x3C doesn't work\n";
        cerr << "  4. Run i2c_scanner to detect devices\n";
        return 1;
    }
    
    // Initialize display
    SSD1306 display;
    
    cout << "Initializing SSD1306 display...\n";
    if (!display.begin()) {
        cerr << "ERROR: Failed to initialize display\n";
        return 1;
    }
    
    cout << "Display initialized successfully!\n\n";
    
    // Run demonstrations
    demoBootScreen(display);
    demoShapes(display);
    demoAnimation(display);
    demoScrollingText(display);
    
    // Final message
    display.clear();
    display.drawText(25, 20, "DEMO");
    display.drawText(15, 35, "COMPLETE");
    display.display();
    
    cout << "\nDemo complete!\n";
    cout << "Display will show final message.\n";
    
    return 0;
}
