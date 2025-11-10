# PiPin++ Code Standards

This document defines the coding standards and style guidelines for the PiPin++ project to ensure consistency, readability, and maintainability across the codebase.

---

## 🎯 **General Principles**

- **Clarity over cleverness** - Code should be self-documenting
- **Consistency** - Follow established patterns throughout the project
- **Modern C++** - Use C++17 features appropriately
- **Arduino familiarity** - Keep compatibility layer intuitive for Arduino users
- **RAII principles** - Proper resource management

---

## 📝 **Naming Conventions**

### **Classes**
```cpp
// PascalCase for class names
class Pin { };
class ArduinoCompat { };
class GpioController { };
```

### **Functions and Methods**
```cpp
// camelCase for functions and methods
void pinMode(int pin, int mode);
bool write(bool value);
int digitalRead(int pin);
void setupGpioChip();
```

### **Variables**
```cpp
// camelCase for variables
int pinNumber;
bool isInitialized;
std::string chipName;
PinDirection direction;
```

### **Constants and Macros**
```cpp
// SCREAMING_SNAKE_CASE for macros
// Use constexpr for type-safe constants (preferred over #define)
constexpr bool HIGH = true;
constexpr bool LOW = false;
const int DEFAULT_CHIP_NUMBER = 0;

// Macros only when preprocessor needed
#define PIPINPP_VERSION_MAJOR 0
#define PIPINPP_ENABLE_LOGGING
```

### **Enums**
```cpp
// PascalCase for enum types, SCREAMING_SNAKE_CASE for values
enum class PinDirection { 
    INPUT, 
    OUTPUT 
};

enum ArduinoPinMode {
    INPUT = 0,
    OUTPUT = 1
};
```

### **File Names**
- **Class headers**: `PascalCase.hpp` (e.g., `ArduinoCompat.hpp`, `Pin.hpp`, `Wire.hpp`)
- **Class source**: `PascalCase.cpp` (e.g., `ArduinoCompat.cpp`, `Pin.cpp`)
- **Utility headers**: `lowercase.hpp` (e.g., `log.hpp`, `exceptions.hpp`)
  - Rationale: Common C++ pattern, distinguishes infrastructure from main classes
- **Examples**: `snake_case` directories, `main.cpp` files

**Note**: Utility headers use lowercase to match standard library convention (`<algorithm>`, `<memory>`) and clearly distinguish infrastructure code from business logic classes.

---

## 🏗️ **Code Structure**

### **Header File Organization**
```cpp
/**
 * @file ClassName.hpp
 * @brief Brief description
 * @author HobbyHacker / Barbatos6669
 * @version x.x.x
 * @date YYYY-MM-DD
 */

#pragma once

// System includes first
#include <string>
#include <memory>

// Third-party includes
#include <gpiod.h>

// Project includes
#include "OtherProjectHeader.hpp"

// Forward declarations (if needed)
class SomeClass;

// Constants and macros
#define SOME_CONSTANT 42

// Type definitions
enum class SomeEnum { VALUE1, VALUE2 };

// Main class definition
class ClassName {
public:
    // Public types first
    using SomeType = int;
    
    // Constructors
    ClassName();
    explicit ClassName(int parameter);
    
    // Destructor
    ~ClassName();
    
    // Copy/move operations (if needed)
    ClassName(const ClassName& other) = delete;
    ClassName& operator=(const ClassName& other) = delete;
    
    // Public methods
    bool someMethod(int param);
    int getValue() const;
    
private:
    // Private methods
    void initializeResources();
    
    // Member variables
    int memberVariable;
    std::string memberString;
    SomeType* resource;
};

// Free functions (if applicable)
void globalFunction(int param);
```

### **Source File Organization**
```cpp
#include "ClassName.hpp"

// System includes
#include <iostream>
#include <stdexcept>

// Implementation details here
// Constructor definitions, method implementations, etc.
```

---

## 🎨 **Formatting Standards**

### **Indentation**
- **4 spaces** (no tabs)
- **Consistent nesting**

### **Braces**
```cpp
// Allman style (opening brace on new line)
class ClassName 
{
public:
    void method() 
    {
        if (condition) 
        {
            doSomething();
        }
        else 
        {
            doSomethingElse();
        }
    }
};
```

### **Line Length**
- **Maximum 100 characters** per line
- Break long lines at logical points

### **Spacing**
```cpp
// Spaces around operators
int result = a + b * c;
bool condition = (x == y) && (z != w);

// Space after control keywords
if (condition) { }
for (int i = 0; i < count; ++i) { }
while (running) { }

// No space between function name and parentheses
functionCall(parameter);
method();
```

---

## 📖 **Documentation Standards**

### **Doxygen Documentation**
```cpp
/**
 * @brief Brief description of the function
 * 
 * @details Longer description explaining the function's purpose,
 * behavior, and any important implementation details.
 * 
 * @param pin GPIO pin number (0-27 on Raspberry Pi)
 * @param mode Pin mode: INPUT (0) or OUTPUT (1)
 * @return true on success, false on failure
 * 
 * @throws std::runtime_error if GPIO chip cannot be opened
 * 
 * @example
 * @code
 * pinMode(17, OUTPUT);
 * digitalWrite(17, HIGH);
 * @endcode
 * 
 * @see digitalWrite(), digitalRead()
 * @since v0.2.0
 */
void pinMode(int pin, int mode);
```

### **Inline Comments**
```cpp
// Use single-line comments for brief explanations
int pinNumber = 17; // Default LED pin

/* Use multi-line comments for longer explanations
   that need to span multiple lines and provide
   detailed context about complex logic */
```

---

## ⚡ **Performance Guidelines**

### **Memory Management**
```cpp
// Prefer RAII and smart pointers
std::unique_ptr<Pin> pin = std::make_unique<Pin>(17, PinDirection::OUTPUT);

// Avoid raw pointers for ownership
// BAD: Pin* pin = new Pin(17, PinDirection::OUTPUT);
// GOOD: Use smart pointers or stack allocation
```

### **Exception Safety**
```cpp
// Use exceptions for error handling, not error codes
if (!chip) 
{
    throw std::runtime_error("Failed to open GPIO chip: " + chipName);
}

// Provide strong exception safety guarantee where possible
```

### **Const Correctness**
```cpp
// Mark methods const when they don't modify state
int getValue() const { return value; }

// Use const references for parameters when appropriate
void processPin(const Pin& pin);

// Prefer const variables when values don't change
const std::string defaultChip = "gpiochip0";
```

---

## 🔧 **Arduino Compatibility Guidelines**

### **Function Naming**
```cpp
// Match Arduino naming exactly for compatibility functions
void pinMode(int pin, int mode);        // NOT: setPinMode()
void digitalWrite(int pin, bool value); // NOT: writePin()
int digitalRead(int pin);               // NOT: readPin()
```

### **Constants**
```cpp
// Use Arduino-inspired constants with constexpr for type safety
constexpr bool HIGH = true;
constexpr bool LOW = false;
constexpr int INPUT = 0;
constexpr int OUTPUT = 1;
constexpr int INPUT_PULLUP = 2;
constexpr int INPUT_PULLDOWN = 3;

// constexpr provides compile-time type checking unlike #define
```

### **Parameter Types**
```cpp
// Use Arduino-inspired parameter types
void digitalWrite(int pin, bool value); // bool for HIGH/LOW
int digitalRead(int pin);               // int return (not bool)
```

---

## 🧪 **Testing Standards**

### **Test File Naming**
- Test files: `test_ClassName.cpp`
- Example files: `example_feature_name.cpp`

### **Test Structure**
```cpp
#include "TestFramework.hpp" // or catch2, gtest, etc.
#include "Pin.hpp"

TEST_CASE("Pin initialization") 
{
    SECTION("Valid pin number") 
    {
        Pin pin(17, PinDirection::OUTPUT);
        // Test assertions here
    }
    
    SECTION("Invalid pin number") 
    {
        REQUIRE_THROWS(Pin(-1, PinDirection::OUTPUT));
    }
}
```

---

## 📁 **File Organization**

### **Directory Structure**
```
include/          # Public headers (.hpp)
src/              # Implementation files (.cpp)
tests/            # Test files
examples/         # Example programs
  feature_name/   # One directory per example
    main.cpp      # Main example file
docs/             # Additional documentation
```

### **Include Guards**
```cpp
// Use #pragma once for header guards
#pragma once

// Avoid traditional include guards unless needed for compatibility
```

---

## 🔄 **Version Control Standards**

### **Commit Messages**
```
type: Brief description (50 chars max)

Longer explanation if needed (wrap at 72 chars).
- List specific changes
- Explain why, not just what
- Reference issues: Fixes #123

Examples:
feat: Add Arduino-style API compatibility layer
fix: Resolve GPIO cleanup issue in Pin destructor  
docs: Update README with installation instructions
test: Add unit tests for Pin class edge cases
```

### **Branch Naming**
- **Feature branches**: `feature/arduino-compat`
- **Bug fixes**: `fix/gpio-cleanup-issue`
- **Documentation**: `docs/api-reference`

---

## 🚀 **Build Standards**

### **CMake Conventions**
```cmake
# Use consistent variable naming
set(PIPINPP_SOURCES
    src/Pin.cpp
    src/ArduinoCompat.cpp
)

# Document complex build logic
# Enable Arduino-inspired API by default
option(BUILD_ARDUINO_COMPAT "Build Arduino-inspired API layer" ON)
```

### **Compiler Warnings**
```cmake
# Enable comprehensive warnings
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
```

---

## ✅ **Code Review Checklist**

Before submitting code, verify:

- [ ] **Naming** follows established conventions
- [ ] **Documentation** is complete and accurate
- [ ] **Error handling** is appropriate
- [ ] **Memory management** follows RAII principles
- [ ] **Tests** are included for new functionality
- [ ] **Arduino-inspired API** is maintained (if applicable)
- [ ] **Build** succeeds without warnings
- [ ] **Examples** are updated if needed

---

## 🛠️ **Tools and Automation**

### **Recommended Tools**
- **clang-format** - Automatic code formatting
- **clang-tidy** - Static analysis
- **Doxygen** - Documentation generation
- **Valgrind** - Memory leak detection

### **Editor Configuration** 
```json
// .vscode/settings.json example
{
    "editor.tabSize": 4,
    "editor.insertSpaces": true,
    "editor.rulers": [100],
    "C_Cpp.clang_format_style": "file"
}
```

---

*This coding standard is a living document. Update as the project evolves and new patterns emerge.*

**Version**: 1.0  
**Last Updated**: October 30, 2025  
**Next Review**: When v1.0.0 is released