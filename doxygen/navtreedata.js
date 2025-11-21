/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "PiPinPP", "index.html", [
    [ "PiPin++", "index.html#autotoc_md0", [
      [ "</blockquote>", "index.html#autotoc_md2", null ],
      [ "Quick Links", "index.html#autotoc_md3", null ],
      [ "Project Vision", "index.html#autotoc_md4", null ],
      [ "Who's it for?", "index.html#autotoc_md5", null ],
      [ "Quick Start Examples", "index.html#autotoc_md7", [
        [ "🔵 Blink an LED", "index.html#autotoc_md8", null ],
        [ "🔘 Read a Button", "index.html#autotoc_md9", null ],
        [ "💡 PWM LED Fade", "index.html#autotoc_md10", null ],
        [ "📡 I2C Sensor (BMP280)", "index.html#autotoc_md11", null ]
      ] ],
      [ "📚 New to GPIO? Start Here!", "index.html#autotoc_md13", null ],
      [ "Features", "index.html#autotoc_md15", [
        [ "🏆 Production-Ready Quality", "index.html#autotoc_md16", null ],
        [ "Core GPIO (v0.1.0-v0.2.0)", "index.html#autotoc_md17", null ],
        [ "Advanced Features (v0.3.0-v0.3.12)", "index.html#autotoc_md18", null ],
        [ "Quality & Testing (v0.3.0-v0.4.0)", "index.html#autotoc_md19", null ]
      ] ],
      [ "Test Coverage", "index.html#autotoc_md20", [
        [ "Longer-Term Goals (v0.4.0+)", "index.html#autotoc_md21", null ]
      ] ],
      [ "Performance Profiling Tools", "index.html#autotoc_md23", null ],
      [ "Installation", "index.html#autotoc_md25", [
        [ "🚀 One-Line Install (Recommended)", "index.html#autotoc_md26", null ],
        [ "Manual Installation", "index.html#autotoc_md27", null ]
      ] ],
      [ "Platform & Compatibility Matrix", "index.html#autotoc_md28", [
        [ "Using in Your Projects", "index.html#autotoc_md29", null ]
      ] ],
      [ "pipinpp CLI Tool", "index.html#autotoc_md31", null ],
      [ "🗺️ Roadmap to \"Super Champ\" Status", "index.html#autotoc_md33", null ],
      [ "Developer Onboarding", "index.html#autotoc_md35", null ],
      [ "Contributing", "index.html#autotoc_md37", null ],
      [ "💬 Community & Support", "index.html#autotoc_md38", null ],
      [ "🤝 Contributing", "index.html#autotoc_md39", null ],
      [ "License", "index.html#autotoc_md41", null ]
    ] ],
    [ "PiPinPP API Reference", "md_docs_API_REFERENCE.html", [
      [ "Table of Contents", "md_docs_API_REFERENCE.html#autotoc_md45", null ],
      [ "Quick Start", "md_docs_API_REFERENCE.html#autotoc_md47", [
        [ "Basic Usage", "md_docs_API_REFERENCE.html#autotoc_md48", null ]
      ] ],
      [ "Platform Detection", "md_docs_API_REFERENCE.html#autotoc_md50", [
        [ "PlatformInfo Class", "md_docs_API_REFERENCE.html#autotoc_md51", null ],
        [ "Platform Enum", "md_docs_API_REFERENCE.html#autotoc_md52", null ],
        [ "Core Methods", "md_docs_API_REFERENCE.html#autotoc_md53", [
          [ "<tt>getPlatform()</tt>", "md_docs_API_REFERENCE.html#autotoc_md54", null ],
          [ "<tt>getPlatformName()</tt>", "md_docs_API_REFERENCE.html#autotoc_md55", null ],
          [ "<tt>isRaspberryPi()</tt>", "md_docs_API_REFERENCE.html#autotoc_md56", null ],
          [ "<tt>isSupported()</tt>", "md_docs_API_REFERENCE.html#autotoc_md57", null ]
        ] ],
        [ "Capability Detection", "md_docs_API_REFERENCE.html#autotoc_md58", [
          [ "<tt>getCapabilities()</tt>", "md_docs_API_REFERENCE.html#autotoc_md59", null ],
          [ "<tt>getDefaultGPIOChip()</tt>", "md_docs_API_REFERENCE.html#autotoc_md60", null ],
          [ "<tt>getDefaultI2CBus()</tt>", "md_docs_API_REFERENCE.html#autotoc_md61", null ]
        ] ],
        [ "Version Detection", "md_docs_API_REFERENCE.html#autotoc_md62", [
          [ "<tt>getKernelVersion()</tt>", "md_docs_API_REFERENCE.html#autotoc_md63", null ],
          [ "<tt>getLibgpiodVersion()</tt>", "md_docs_API_REFERENCE.html#autotoc_md64", null ]
        ] ],
        [ "Diagnostic Output", "md_docs_API_REFERENCE.html#autotoc_md65", [
          [ "<tt>printInfo()</tt>", "md_docs_API_REFERENCE.html#autotoc_md66", null ]
        ] ],
        [ "Platform-Aware Code", "md_docs_API_REFERENCE.html#autotoc_md67", [
          [ "Cross-Platform I2C", "md_docs_API_REFERENCE.html#autotoc_md68", null ],
          [ "Feature Detection", "md_docs_API_REFERENCE.html#autotoc_md69", null ],
          [ "Platform-Specific Logic", "md_docs_API_REFERENCE.html#autotoc_md70", null ]
        ] ],
        [ "Platform Support Matrix", "md_docs_API_REFERENCE.html#autotoc_md71", null ],
        [ "Example: Complete Platform Detection", "md_docs_API_REFERENCE.html#autotoc_md72", null ]
      ] ],
      [ "Core Pin Class", "md_docs_API_REFERENCE.html#autotoc_md74", [
        [ "Enums", "md_docs_API_REFERENCE.html#autotoc_md75", [
          [ "<tt>PinDirection</tt>", "md_docs_API_REFERENCE.html#autotoc_md76", null ],
          [ "<tt>PinMode</tt>", "md_docs_API_REFERENCE.html#autotoc_md77", null ]
        ] ],
        [ "Constructors", "md_docs_API_REFERENCE.html#autotoc_md78", [
          [ "<tt>Pin(int pin, PinDirection direction, const std::string& chipname = \"gpiochip0\")</tt>", "md_docs_API_REFERENCE.html#autotoc_md79", null ],
          [ "<tt>Pin(int pin, PinMode mode, const std::string& chipname = \"gpiochip0\")</tt>", "md_docs_API_REFERENCE.html#autotoc_md80", null ]
        ] ],
        [ "Methods", "md_docs_API_REFERENCE.html#autotoc_md81", [
          [ "<tt>bool write(bool value)</tt>", "md_docs_API_REFERENCE.html#autotoc_md82", null ],
          [ "<tt>int read()</tt>", "md_docs_API_REFERENCE.html#autotoc_md83", null ]
        ] ]
      ] ],
      [ "Arduino Compatibility Layer", "md_docs_API_REFERENCE.html#autotoc_md85", [
        [ "Constants", "md_docs_API_REFERENCE.html#autotoc_md86", null ],
        [ "Digital I/O Functions", "md_docs_API_REFERENCE.html#autotoc_md87", [
          [ "<tt>void pinMode(int pin, int mode)</tt>", "md_docs_API_REFERENCE.html#autotoc_md88", null ],
          [ "<tt>void digitalWrite(int pin, bool value)</tt>", "md_docs_API_REFERENCE.html#autotoc_md89", null ],
          [ "<tt>int digitalRead(int pin)</tt>", "md_docs_API_REFERENCE.html#autotoc_md90", null ]
        ] ],
        [ "Interrupt Functions", "md_docs_API_REFERENCE.html#autotoc_md91", [
          [ "<tt>void attachInterrupt(int pin, InterruptCallback callback, InterruptMode mode)</tt>", "md_docs_API_REFERENCE.html#autotoc_md92", null ],
          [ "<tt>void detachInterrupt(int pin)</tt>", "md_docs_API_REFERENCE.html#autotoc_md93", null ]
        ] ],
        [ "PWM Functions", "md_docs_API_REFERENCE.html#autotoc_md94", [
          [ "<tt>void analogWrite(int pin, int value)</tt>", "md_docs_API_REFERENCE.html#autotoc_md95", null ],
          [ "<tt>void stopPWM(int pin)</tt>", "md_docs_API_REFERENCE.html#autotoc_md96", null ]
        ] ]
      ] ],
      [ "Event-Driven PWM", "md_docs_API_REFERENCE.html#autotoc_md98", [
        [ "EventPWM Class", "md_docs_API_REFERENCE.html#autotoc_md99", [
          [ "<tt>EventPWM(int pin, const std::string& chipname = \"gpiochip0\")</tt>", "md_docs_API_REFERENCE.html#autotoc_md100", null ],
          [ "<tt>bool begin(double frequencyHz, double dutyCycle)</tt>", "md_docs_API_REFERENCE.html#autotoc_md101", null ],
          [ "<tt>void end()</tt>", "md_docs_API_REFERENCE.html#autotoc_md102", null ],
          [ "<tt>void setDutyCycle(double dutyCycle)</tt>", "md_docs_API_REFERENCE.html#autotoc_md103", null ],
          [ "<tt>void setDutyCycle8Bit(uint8_t value)</tt>", "md_docs_API_REFERENCE.html#autotoc_md104", null ],
          [ "<tt>void setFrequency(double frequencyHz)</tt>", "md_docs_API_REFERENCE.html#autotoc_md105", null ]
        ] ],
        [ "When to Use EventPWM", "md_docs_API_REFERENCE.html#autotoc_md106", null ],
        [ "EventPWM Manager (Global Functions)", "md_docs_API_REFERENCE.html#autotoc_md107", [
          [ "<tt>void analogWriteEvent(int pin, int value)</tt>", "md_docs_API_REFERENCE.html#autotoc_md108", null ]
        ] ],
        [ "Performance Comparison", "md_docs_API_REFERENCE.html#autotoc_md109", null ]
      ] ],
      [ "Hardware PWM", "md_docs_API_REFERENCE.html#autotoc_md111", [
        [ "HardwarePWM Class", "md_docs_API_REFERENCE.html#autotoc_md112", [
          [ "<tt>HardwarePWM(int chip, int channel)</tt>", "md_docs_API_REFERENCE.html#autotoc_md113", null ],
          [ "<tt>bool begin(double frequencyHz, double dutyCycle = 0.0)</tt>", "md_docs_API_REFERENCE.html#autotoc_md114", null ],
          [ "<tt>void end()</tt>", "md_docs_API_REFERENCE.html#autotoc_md115", null ],
          [ "<tt>bool setFrequency(double frequencyHz)</tt>", "md_docs_API_REFERENCE.html#autotoc_md116", null ],
          [ "<tt>bool setDutyCycle(double dutyCycle)</tt>", "md_docs_API_REFERENCE.html#autotoc_md117", null ],
          [ "<tt>bool setDutyCycle8Bit(uint8_t value)</tt>", "md_docs_API_REFERENCE.html#autotoc_md118", null ],
          [ "<tt>bool setPeriodNs(uint64_t periodNs)</tt>", "md_docs_API_REFERENCE.html#autotoc_md119", null ],
          [ "<tt>bool setDutyCycleNs(uint64_t dutyCycleNs)</tt>", "md_docs_API_REFERENCE.html#autotoc_md120", null ],
          [ "<tt>bool setPolarity(PWMPolarity polarity)</tt>", "md_docs_API_REFERENCE.html#autotoc_md121", null ],
          [ "<tt>bool enable()</tt> / <tt>bool disable()</tt>", "md_docs_API_REFERENCE.html#autotoc_md122", null ],
          [ "Getters", "md_docs_API_REFERENCE.html#autotoc_md123", null ]
        ] ],
        [ "Hardware PWM Examples", "md_docs_API_REFERENCE.html#autotoc_md124", [
          [ "Servo Control", "md_docs_API_REFERENCE.html#autotoc_md125", null ],
          [ "LED Fading with Hardware PWM", "md_docs_API_REFERENCE.html#autotoc_md126", null ]
        ] ],
        [ "Timing Functions", "md_docs_API_REFERENCE.html#autotoc_md127", [
          [ "<tt>unsigned long millis()</tt>", "md_docs_API_REFERENCE.html#autotoc_md128", null ],
          [ "<tt>unsigned long micros()</tt>", "md_docs_API_REFERENCE.html#autotoc_md129", null ],
          [ "<tt>void delay(unsigned long ms)</tt>", "md_docs_API_REFERENCE.html#autotoc_md130", null ],
          [ "<tt>void delayMicroseconds(unsigned int us)</tt>", "md_docs_API_REFERENCE.html#autotoc_md131", null ],
          [ "Performance Profiling Example", "md_docs_API_REFERENCE.html#autotoc_md132", null ]
        ] ],
        [ "Pin State Query Functions", "md_docs_API_REFERENCE.html#autotoc_md133", [
          [ "<tt>bool isOutput(int pin)</tt>", "md_docs_API_REFERENCE.html#autotoc_md134", null ],
          [ "<tt>bool isInput(int pin)</tt>", "md_docs_API_REFERENCE.html#autotoc_md135", null ],
          [ "<tt>ArduinoPinMode getMode(int pin)</tt>", "md_docs_API_REFERENCE.html#autotoc_md136", null ],
          [ "<tt>void digitalToggle(int pin)</tt>", "md_docs_API_REFERENCE.html#autotoc_md137", null ]
        ] ],
        [ "Math Functions", "md_docs_API_REFERENCE.html#autotoc_md138", [
          [ "<tt>int constrain(int x, int min, int max)</tt>", "md_docs_API_REFERENCE.html#autotoc_md139", null ],
          [ "<tt>long map(long x, long in_min, long in_max, long out_min, long out_max)</tt>", "md_docs_API_REFERENCE.html#autotoc_md140", null ],
          [ "<tt>T sq(T x)</tt> (template)", "md_docs_API_REFERENCE.html#autotoc_md141", null ]
        ] ],
        [ "Trigonometry Constants", "md_docs_API_REFERENCE.html#autotoc_md142", [
          [ "<tt>DEG_TO_RAD</tt>", "md_docs_API_REFERENCE.html#autotoc_md143", null ],
          [ "<tt>RAD_TO_DEG</tt>", "md_docs_API_REFERENCE.html#autotoc_md144", null ]
        ] ]
      ] ],
      [ "Advanced I/O Functions", "md_docs_API_REFERENCE.html#autotoc_md146", [
        [ "<tt>unsigned long pulseIn(int pin, bool state, unsigned long timeout)</tt>", "md_docs_API_REFERENCE.html#autotoc_md147", null ],
        [ "<tt>unsigned long pulseInLong(int pin, bool state, unsigned long timeout)</tt>", "md_docs_API_REFERENCE.html#autotoc_md149", null ],
        [ "shiftOut(int dataPin, int clockPin, uint8_t bitOrder, uint8_t value)`", "md_docs_API_REFERENCE.html#autotoc_md151", null ],
        [ "<tt>uint8_t shiftIn(int dataPin, int clockPin, uint8_t bitOrder)</tt>", "md_docs_API_REFERENCE.html#autotoc_md153", null ],
        [ "<tt>void tone(int pin, unsigned int frequency, unsigned long duration)</tt>", "md_docs_API_REFERENCE.html#autotoc_md155", null ],
        [ "<tt>void noTone(int pin)</tt>", "md_docs_API_REFERENCE.html#autotoc_md157", null ],
        [ "Bit Manipulation Functions", "md_docs_API_REFERENCE.html#autotoc_md159", [
          [ "<tt>uint8_t bit(uint8_t n)</tt>", "md_docs_API_REFERENCE.html#autotoc_md160", null ],
          [ "<tt>uint8_t bitRead(uint8_t value, uint8_t n)</tt>", "md_docs_API_REFERENCE.html#autotoc_md162", null ],
          [ "<tt>void bitWrite(uint8_t& value, uint8_t n, uint8_t bitValue)</tt>", "md_docs_API_REFERENCE.html#autotoc_md164", null ],
          [ "<tt>void bitSet(uint8_t& value, uint8_t n)</tt>", "md_docs_API_REFERENCE.html#autotoc_md166", null ],
          [ "<tt>void bitClear(uint8_t& value, uint8_t n)</tt>", "md_docs_API_REFERENCE.html#autotoc_md168", null ],
          [ "<tt>uint8_t lowByte(uint16_t value)</tt>", "md_docs_API_REFERENCE.html#autotoc_md170", null ],
          [ "<tt>uint8_t highByte(uint16_t value)</tt>", "md_docs_API_REFERENCE.html#autotoc_md172", null ]
        ] ]
      ] ],
      [ "Exception Handling", "md_docs_API_REFERENCE.html#autotoc_md174", [
        [ "Exception Hierarchy", "md_docs_API_REFERENCE.html#autotoc_md175", null ],
        [ "Exception Classes", "md_docs_API_REFERENCE.html#autotoc_md176", [
          [ "<tt>PinError</tt>", "md_docs_API_REFERENCE.html#autotoc_md177", null ],
          [ "<tt>InvalidPinError</tt>", "md_docs_API_REFERENCE.html#autotoc_md178", null ],
          [ "<tt>GpioAccessError</tt>", "md_docs_API_REFERENCE.html#autotoc_md179", null ]
        ] ],
        [ "Exception Handling Best Practices", "md_docs_API_REFERENCE.html#autotoc_md180", [
          [ "1. Catch Specific Exceptions", "md_docs_API_REFERENCE.html#autotoc_md181", null ],
          [ "2. Catch Base Exception for Generic Handling", "md_docs_API_REFERENCE.html#autotoc_md182", null ],
          [ "3. Initialization Pattern", "md_docs_API_REFERENCE.html#autotoc_md183", null ]
        ] ],
        [ "Common Error Causes and Solutions", "md_docs_API_REFERENCE.html#autotoc_md184", null ]
      ] ],
      [ "Communication Protocols", "md_docs_API_REFERENCE.html#autotoc_md186", [
        [ "Wire (I²C)", "md_docs_API_REFERENCE.html#autotoc_md188", [
          [ "Include", "md_docs_API_REFERENCE.html#autotoc_md189", null ],
          [ "I²C Pin Connections", "md_docs_API_REFERENCE.html#autotoc_md190", null ],
          [ "Enable I²C", "md_docs_API_REFERENCE.html#autotoc_md191", null ],
          [ "Wire Functions", "md_docs_API_REFERENCE.html#autotoc_md192", [
            [ "<tt>Wire.begin()</tt>", "md_docs_API_REFERENCE.html#autotoc_md193", null ],
            [ "<tt>Wire.begin(int busNumber)</tt>", "md_docs_API_REFERENCE.html#autotoc_md194", null ],
            [ "<tt>Wire.end()</tt>", "md_docs_API_REFERENCE.html#autotoc_md195", null ],
            [ "<tt>Wire.setClock(uint32_t frequency)</tt>", "md_docs_API_REFERENCE.html#autotoc_md196", null ],
            [ "<tt>Wire.getClock()</tt>", "md_docs_API_REFERENCE.html#autotoc_md197", null ],
            [ "<tt>Wire.beginTransmission(uint8_t address)</tt>", "md_docs_API_REFERENCE.html#autotoc_md198", null ],
            [ "<tt>Wire.write(uint8_t data)</tt>", "md_docs_API_REFERENCE.html#autotoc_md199", null ],
            [ "<tt>Wire.write(const uint8_t* data, size_t length)</tt>", "md_docs_API_REFERENCE.html#autotoc_md200", null ],
            [ "<tt>Wire.endTransmission(bool sendStop = true)</tt>", "md_docs_API_REFERENCE.html#autotoc_md201", null ],
            [ "<tt>Wire.requestFrom(uint8_t address, size_t length, bool sendStop = true)</tt>", "md_docs_API_REFERENCE.html#autotoc_md202", null ],
            [ "<tt>Wire.available()</tt>", "md_docs_API_REFERENCE.html#autotoc_md203", null ],
            [ "<tt>Wire.read()</tt>", "md_docs_API_REFERENCE.html#autotoc_md204", null ],
            [ "<tt>Wire.peek()</tt>", "md_docs_API_REFERENCE.html#autotoc_md205", null ]
          ] ],
          [ "Helper Functions", "md_docs_API_REFERENCE.html#autotoc_md206", [
            [ "<tt>Wire.exists(uint8_t address)</tt>", "md_docs_API_REFERENCE.html#autotoc_md207", null ],
            [ "<tt>Wire.scan()</tt>", "md_docs_API_REFERENCE.html#autotoc_md208", null ],
            [ "<tt>Wire.readRegister(uint8_t address, uint8_t reg, uint8_t& value)</tt>", "md_docs_API_REFERENCE.html#autotoc_md209", null ],
            [ "<tt>Wire.writeRegister(uint8_t address, uint8_t reg, uint8_t value)</tt>", "md_docs_API_REFERENCE.html#autotoc_md210", null ]
          ] ],
          [ "Complete I²C Example", "md_docs_API_REFERENCE.html#autotoc_md211", null ]
        ] ],
        [ "SPI", "md_docs_API_REFERENCE.html#autotoc_md213", [
          [ "Include", "md_docs_API_REFERENCE.html#autotoc_md214", null ],
          [ "SPI Pin Connections (All Raspberry Pi Models)", "md_docs_API_REFERENCE.html#autotoc_md215", null ],
          [ "Enable SPI", "md_docs_API_REFERENCE.html#autotoc_md216", null ],
          [ "SPI Constants", "md_docs_API_REFERENCE.html#autotoc_md217", [
            [ "Modes", "md_docs_API_REFERENCE.html#autotoc_md218", null ],
            [ "Bit Order", "md_docs_API_REFERENCE.html#autotoc_md219", null ],
            [ "Clock Dividers", "md_docs_API_REFERENCE.html#autotoc_md220", null ]
          ] ],
          [ "SPI Functions", "md_docs_API_REFERENCE.html#autotoc_md221", [
            [ "<tt>SPI.begin()</tt>", "md_docs_API_REFERENCE.html#autotoc_md222", null ],
            [ "<tt>SPI.begin(int bus, int cs)</tt>", "md_docs_API_REFERENCE.html#autotoc_md223", null ],
            [ "<tt>SPI.end()</tt>", "md_docs_API_REFERENCE.html#autotoc_md224", null ],
            [ "<tt>SPI.setDataMode(uint8_t mode)</tt>", "md_docs_API_REFERENCE.html#autotoc_md225", null ],
            [ "<tt>SPI.setBitOrder(uint8_t order)</tt>", "md_docs_API_REFERENCE.html#autotoc_md226", null ],
            [ "<tt>SPI.setClockDivider(uint8_t divider)</tt>", "md_docs_API_REFERENCE.html#autotoc_md227", null ],
            [ "<tt>SPI.setClock(uint32_t speed)</tt>", "md_docs_API_REFERENCE.html#autotoc_md228", null ],
            [ "<tt>SPI.getClock()</tt>", "md_docs_API_REFERENCE.html#autotoc_md229", null ],
            [ "<tt>SPI.transfer(uint8_t data)</tt>", "md_docs_API_REFERENCE.html#autotoc_md230", null ],
            [ "<tt>SPI.transfer(uint8_t* buffer, size_t length)</tt>", "md_docs_API_REFERENCE.html#autotoc_md231", null ],
            [ "<tt>SPI.transfer(const uint8_t* txBuffer, uint8_t* rxBuffer, size_t length)</tt>", "md_docs_API_REFERENCE.html#autotoc_md232", null ],
            [ "<tt>SPI.isInitialized()</tt>", "md_docs_API_REFERENCE.html#autotoc_md233", null ]
          ] ],
          [ "Complete SPI Example", "md_docs_API_REFERENCE.html#autotoc_md234", null ],
          [ "SPI vs shiftOut Performance", "md_docs_API_REFERENCE.html#autotoc_md235", null ]
        ] ],
        [ "Serial (UART)", "md_docs_API_REFERENCE.html#autotoc_md237", [
          [ "Include", "md_docs_API_REFERENCE.html#autotoc_md238", null ],
          [ "UART Pin Connections", "md_docs_API_REFERENCE.html#autotoc_md239", null ],
          [ "Enable Hardware UART", "md_docs_API_REFERENCE.html#autotoc_md240", null ],
          [ "Serial Functions", "md_docs_API_REFERENCE.html#autotoc_md241", [
            [ "<tt>Serial.begin(baudRate, device)</tt>", "md_docs_API_REFERENCE.html#autotoc_md242", null ],
            [ "<tt>Serial.end()</tt>", "md_docs_API_REFERENCE.html#autotoc_md243", null ],
            [ "<tt>Serial.isOpen()</tt>", "md_docs_API_REFERENCE.html#autotoc_md244", null ],
            [ "<tt>Serial.available()</tt>", "md_docs_API_REFERENCE.html#autotoc_md245", null ],
            [ "<tt>Serial.read()</tt>", "md_docs_API_REFERENCE.html#autotoc_md246", null ],
            [ "<tt>Serial.peek()</tt>", "md_docs_API_REFERENCE.html#autotoc_md247", null ],
            [ "<tt>Serial.write(byte)</tt>", "md_docs_API_REFERENCE.html#autotoc_md248", null ],
            [ "<tt>Serial.write(buffer, size)</tt>", "md_docs_API_REFERENCE.html#autotoc_md249", null ],
            [ "<tt>Serial.print(data)</tt>", "md_docs_API_REFERENCE.html#autotoc_md250", null ],
            [ "<tt>Serial.println(data)</tt>", "md_docs_API_REFERENCE.html#autotoc_md251", null ],
            [ "<tt>Serial.readString()</tt>", "md_docs_API_REFERENCE.html#autotoc_md252", null ],
            [ "<tt>Serial.readStringUntil(terminator)</tt>", "md_docs_API_REFERENCE.html#autotoc_md253", null ],
            [ "<tt>Serial.setTimeout(milliseconds)</tt>", "md_docs_API_REFERENCE.html#autotoc_md254", null ],
            [ "<tt>Serial.flush()</tt>", "md_docs_API_REFERENCE.html#autotoc_md255", null ]
          ] ],
          [ "Print Format Constants", "md_docs_API_REFERENCE.html#autotoc_md256", null ],
          [ "Complete Serial Example", "md_docs_API_REFERENCE.html#autotoc_md257", null ],
          [ "Arduino-to-Raspberry Pi Communication", "md_docs_API_REFERENCE.html#autotoc_md258", null ],
          [ "Troubleshooting Serial", "md_docs_API_REFERENCE.html#autotoc_md259", null ]
        ] ]
      ] ],
      [ "Examples", "md_docs_API_REFERENCE.html#autotoc_md261", [
        [ "Blink LED (Object-Oriented)", "md_docs_API_REFERENCE.html#autotoc_md262", null ],
        [ "Button Input (Arduino-Style)", "md_docs_API_REFERENCE.html#autotoc_md263", null ],
        [ "Advanced Pull Resistor Usage", "md_docs_API_REFERENCE.html#autotoc_md264", null ],
        [ "Timing Functions Usage", "md_docs_API_REFERENCE.html#autotoc_md265", null ],
        [ "Button Interrupt with Debouncing", "md_docs_API_REFERENCE.html#autotoc_md266", null ],
        [ "LED Breathing Effect with PWM", "md_docs_API_REFERENCE.html#autotoc_md267", null ],
        [ "Multiple PWM Outputs (RGB LED)", "md_docs_API_REFERENCE.html#autotoc_md268", null ]
      ] ],
      [ "Error Handling", "md_docs_API_REFERENCE.html#autotoc_md270", [
        [ "Exception Types", "md_docs_API_REFERENCE.html#autotoc_md271", null ],
        [ "Error Handling Example", "md_docs_API_REFERENCE.html#autotoc_md272", null ],
        [ "Common Error Solutions", "md_docs_API_REFERENCE.html#autotoc_md273", null ]
      ] ],
      [ "Hardware Notes", "md_docs_API_REFERENCE.html#autotoc_md275", [
        [ "Pin Numbering", "md_docs_API_REFERENCE.html#autotoc_md276", null ],
        [ "Electrical Specifications", "md_docs_API_REFERENCE.html#autotoc_md277", null ],
        [ "Pull Resistors", "md_docs_API_REFERENCE.html#autotoc_md278", null ],
        [ "Hardware Setup Examples", "md_docs_API_REFERENCE.html#autotoc_md279", [
          [ "LED Connection", "md_docs_API_REFERENCE.html#autotoc_md280", null ],
          [ "Button Connection (with internal pull-up)", "md_docs_API_REFERENCE.html#autotoc_md281", null ],
          [ "Button Connection (with external pull-up)", "md_docs_API_REFERENCE.html#autotoc_md282", null ]
        ] ]
      ] ],
      [ "Dependencies", "md_docs_API_REFERENCE.html#autotoc_md284", null ],
      [ "License", "md_docs_API_REFERENCE.html#autotoc_md286", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ],
        [ "Enumerator", "namespacemembers_eval.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", null ],
        [ "Functions", "functions_func.html", null ],
        [ "Variables", "functions_vars.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ],
    [ "Examples", "examples.html", "examples" ]
  ] ]
];

var NAVTREEINDEX =
[
"ArduinoCompat_8cpp.html",
"classpipinpp_1_1SerialPort.html#a4d73dbf39c0273bbace096b3c23c22df",
"md_docs_API_REFERENCE.html#autotoc_md253"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';