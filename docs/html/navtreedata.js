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
      [ "Project Vision", "index.html#autotoc_md3", null ],
      [ "Why?", "index.html#autotoc_md4", null ],
      [ "Who's it for?", "index.html#autotoc_md5", null ],
      [ "Quick Start Example", "index.html#autotoc_md7", null ],
      [ "Features", "index.html#autotoc_md9", [
        [ "✅ Current Features (v0.3.2)", "index.html#autotoc_md10", null ],
        [ "Core GPIO (v0.1.0-v0.2.0)", "index.html#autotoc_md11", null ],
        [ "Advanced Features (v0.3.0-v0.3.2)", "index.html#autotoc_md12", null ],
        [ "Quality & Testing (v0.3.0+)", "index.html#autotoc_md13", null ],
        [ "Longer-Term Goals (v0.4.0+)", "index.html#autotoc_md14", null ]
      ] ],
      [ "Installation", "index.html#autotoc_md16", [
        [ "🚀 One-Line Install (Easiest!)", "index.html#autotoc_md17", null ],
        [ "Manual Installation", "index.html#autotoc_md18", null ],
        [ "🔄 Updating to a Newer Version", "index.html#autotoc_md19", null ],
        [ "Using in Your Projects", "index.html#autotoc_md20", null ]
      ] ],
      [ "Contributing", "index.html#autotoc_md22", null ],
      [ "💬 Community & Support", "index.html#autotoc_md23", null ],
      [ "🤝 Contributing", "index.html#autotoc_md24", null ],
      [ "License", "index.html#autotoc_md26", null ]
    ] ],
    [ "PiPinPP API Reference", "md_docs_2API__REFERENCE.html", [
      [ "Table of Contents", "md_docs_2API__REFERENCE.html#autotoc_md30", null ],
      [ "Quick Start", "md_docs_2API__REFERENCE.html#autotoc_md32", [
        [ "Installation", "md_docs_2API__REFERENCE.html#autotoc_md33", null ],
        [ "Basic Usage", "md_docs_2API__REFERENCE.html#autotoc_md34", null ]
      ] ],
      [ "Core Pin Class", "md_docs_2API__REFERENCE.html#autotoc_md36", [
        [ "Enums", "md_docs_2API__REFERENCE.html#autotoc_md37", [
          [ "<tt>PinDirection</tt>", "md_docs_2API__REFERENCE.html#autotoc_md38", null ],
          [ "<tt>PinMode</tt>", "md_docs_2API__REFERENCE.html#autotoc_md39", null ]
        ] ],
        [ "Constructors", "md_docs_2API__REFERENCE.html#autotoc_md40", [
          [ "<tt>Pin(int pin, PinDirection direction, const std::string& chipname = \"gpiochip0\")</tt>", "md_docs_2API__REFERENCE.html#autotoc_md41", null ],
          [ "<tt>Pin(int pin, PinMode mode, const std::string& chipname = \"gpiochip0\")</tt>", "md_docs_2API__REFERENCE.html#autotoc_md42", null ]
        ] ],
        [ "Methods", "md_docs_2API__REFERENCE.html#autotoc_md43", [
          [ "<tt>bool write(bool value)</tt>", "md_docs_2API__REFERENCE.html#autotoc_md44", null ],
          [ "<tt>int read()</tt>", "md_docs_2API__REFERENCE.html#autotoc_md45", null ]
        ] ]
      ] ],
      [ "Arduino Compatibility Layer", "md_docs_2API__REFERENCE.html#autotoc_md47", [
        [ "Constants", "md_docs_2API__REFERENCE.html#autotoc_md48", null ],
        [ "Digital I/O Functions", "md_docs_2API__REFERENCE.html#autotoc_md49", [
          [ "<tt>void pinMode(int pin, int mode)</tt>", "md_docs_2API__REFERENCE.html#autotoc_md50", null ],
          [ "<tt>void digitalWrite(int pin, bool value)</tt>", "md_docs_2API__REFERENCE.html#autotoc_md51", null ],
          [ "<tt>int digitalRead(int pin)</tt>", "md_docs_2API__REFERENCE.html#autotoc_md52", null ]
        ] ],
        [ "Interrupt Functions", "md_docs_2API__REFERENCE.html#autotoc_md53", [
          [ "<tt>void attachInterrupt(int pin, InterruptCallback callback, InterruptMode mode)</tt>", "md_docs_2API__REFERENCE.html#autotoc_md54", null ],
          [ "<tt>void detachInterrupt(int pin)</tt>", "md_docs_2API__REFERENCE.html#autotoc_md55", null ]
        ] ],
        [ "PWM Functions", "md_docs_2API__REFERENCE.html#autotoc_md56", [
          [ "<tt>void analogWrite(int pin, int value)</tt>", "md_docs_2API__REFERENCE.html#autotoc_md57", null ],
          [ "<tt>void stopPWM(int pin)</tt>", "md_docs_2API__REFERENCE.html#autotoc_md58", null ]
        ] ],
        [ "Timing Functions", "md_docs_2API__REFERENCE.html#autotoc_md59", [
          [ "<tt>unsigned long millis()</tt>", "md_docs_2API__REFERENCE.html#autotoc_md60", null ],
          [ "<tt>unsigned long micros()</tt>", "md_docs_2API__REFERENCE.html#autotoc_md61", null ],
          [ "<tt>void delay(unsigned long ms)</tt>", "md_docs_2API__REFERENCE.html#autotoc_md62", null ],
          [ "<tt>void delayMicroseconds(unsigned int us)</tt>", "md_docs_2API__REFERENCE.html#autotoc_md63", null ]
        ] ],
        [ "Pin State Query Functions", "md_docs_2API__REFERENCE.html#autotoc_md64", [
          [ "<tt>bool isOutput(int pin)</tt>", "md_docs_2API__REFERENCE.html#autotoc_md65", null ],
          [ "<tt>bool isInput(int pin)</tt>", "md_docs_2API__REFERENCE.html#autotoc_md66", null ],
          [ "<tt>ArduinoPinMode getMode(int pin)</tt>", "md_docs_2API__REFERENCE.html#autotoc_md67", null ],
          [ "<tt>void digitalToggle(int pin)</tt>", "md_docs_2API__REFERENCE.html#autotoc_md68", null ]
        ] ],
        [ "Math Functions", "md_docs_2API__REFERENCE.html#autotoc_md69", [
          [ "<tt>int constrain(int x, int min, int max)</tt>", "md_docs_2API__REFERENCE.html#autotoc_md70", null ],
          [ "<tt>long map(long x, long in_min, long in_max, long out_min, long out_max)</tt>", "md_docs_2API__REFERENCE.html#autotoc_md71", null ],
          [ "<tt>T sq(T x)</tt> (template)", "md_docs_2API__REFERENCE.html#autotoc_md72", null ]
        ] ],
        [ "Trigonometry Constants", "md_docs_2API__REFERENCE.html#autotoc_md73", [
          [ "<tt>DEG_TO_RAD</tt>", "md_docs_2API__REFERENCE.html#autotoc_md74", null ],
          [ "<tt>RAD_TO_DEG</tt>", "md_docs_2API__REFERENCE.html#autotoc_md75", null ]
        ] ]
      ] ],
      [ "Exception Handling", "md_docs_2API__REFERENCE.html#autotoc_md77", [
        [ "Exception Hierarchy", "md_docs_2API__REFERENCE.html#autotoc_md78", null ],
        [ "Exception Classes", "md_docs_2API__REFERENCE.html#autotoc_md79", [
          [ "<tt>PinError</tt>", "md_docs_2API__REFERENCE.html#autotoc_md80", null ],
          [ "<tt>InvalidPinError</tt>", "md_docs_2API__REFERENCE.html#autotoc_md81", null ],
          [ "<tt>GpioAccessError</tt>", "md_docs_2API__REFERENCE.html#autotoc_md82", null ]
        ] ],
        [ "Exception Handling Best Practices", "md_docs_2API__REFERENCE.html#autotoc_md83", [
          [ "1. Catch Specific Exceptions", "md_docs_2API__REFERENCE.html#autotoc_md84", null ],
          [ "2. Catch Base Exception for Generic Handling", "md_docs_2API__REFERENCE.html#autotoc_md85", null ],
          [ "3. Initialization Pattern", "md_docs_2API__REFERENCE.html#autotoc_md86", null ]
        ] ],
        [ "Common Error Causes and Solutions", "md_docs_2API__REFERENCE.html#autotoc_md87", null ]
      ] ],
      [ "Examples", "md_docs_2API__REFERENCE.html#autotoc_md89", [
        [ "Blink LED (Object-Oriented)", "md_docs_2API__REFERENCE.html#autotoc_md90", null ],
        [ "Button Input (Arduino-Style)", "md_docs_2API__REFERENCE.html#autotoc_md91", null ],
        [ "Advanced Pull Resistor Usage", "md_docs_2API__REFERENCE.html#autotoc_md92", null ],
        [ "Timing Functions Usage", "md_docs_2API__REFERENCE.html#autotoc_md93", null ],
        [ "Button Interrupt with Debouncing", "md_docs_2API__REFERENCE.html#autotoc_md94", null ],
        [ "LED Breathing Effect with PWM", "md_docs_2API__REFERENCE.html#autotoc_md95", null ],
        [ "Multiple PWM Outputs (RGB LED)", "md_docs_2API__REFERENCE.html#autotoc_md96", null ]
      ] ],
      [ "Error Handling", "md_docs_2API__REFERENCE.html#autotoc_md98", [
        [ "Exception Types", "md_docs_2API__REFERENCE.html#autotoc_md99", null ],
        [ "Error Handling Example", "md_docs_2API__REFERENCE.html#autotoc_md100", null ],
        [ "Common Error Solutions", "md_docs_2API__REFERENCE.html#autotoc_md101", null ]
      ] ],
      [ "Hardware Notes", "md_docs_2API__REFERENCE.html#autotoc_md103", [
        [ "Pin Numbering", "md_docs_2API__REFERENCE.html#autotoc_md104", null ],
        [ "Electrical Specifications", "md_docs_2API__REFERENCE.html#autotoc_md105", null ],
        [ "Pull Resistors", "md_docs_2API__REFERENCE.html#autotoc_md106", null ],
        [ "Hardware Setup Examples", "md_docs_2API__REFERENCE.html#autotoc_md107", [
          [ "LED Connection", "md_docs_2API__REFERENCE.html#autotoc_md108", null ],
          [ "Button Connection (with internal pull-up)", "md_docs_2API__REFERENCE.html#autotoc_md109", null ],
          [ "Button Connection (with external pull-up)", "md_docs_2API__REFERENCE.html#autotoc_md110", null ]
        ] ]
      ] ],
      [ "Dependencies", "md_docs_2API__REFERENCE.html#autotoc_md112", null ],
      [ "License", "md_docs_2API__REFERENCE.html#autotoc_md114", null ]
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
"structInterruptHandler.html#ad45fe58c1e6dcc207db87a19467aa6dc"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';