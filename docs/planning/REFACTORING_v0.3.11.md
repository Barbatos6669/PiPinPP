# PiPinPP v0.3.11 Refactoring Plan

**Date Created**: November 10, 2025  
**Target Release**: v0.3.11 (Code Quality & Documentation Fixes)  
**Type**: Patch Release (Non-Breaking Improvements)

---

## Overview

This document tracks code quality improvements, refactoring, and documentation fixes identified through comprehensive code review. These changes improve maintainability, consistency, and correctness without breaking the public API.

**Guiding Principles:**
- ✅ Non-breaking changes only (patch version)
- ✅ Improve code quality and maintainability
- ✅ Fix documentation inconsistencies
- ✅ Enhance performance where possible
- ✅ Maintain Arduino API compatibility

---

## 🔧 Code Refactoring Issues

### 1. Pin Constructor Duplication [HIGH PRIORITY]

**Issue**: `Pin(int, PinDirection)` and `Pin(int, PinMode)` constructors contain nearly identical GPIO setup code.

**Location**: `src/pin.cpp` lines ~50-100

**Current State**:
```cpp
// Constructor 1: PinDirection
Pin::Pin(int pinNumber, PinDirection direction) {
    // Duplicate GPIO setup code
}

// Constructor 2: PinMode (with pull resistors)
Pin::Pin(int pinNumber, PinMode mode) {
    // Nearly identical GPIO setup code
}
```

**Solution**:
```cpp
// Add private helper method
void Pin::initializeGpio(int pinNumber, gpiod_line_direction dir, 
                         gpiod_line_bias bias);

// Delegate from constructors
Pin::Pin(int pinNumber, PinDirection direction) 
    : pinNumber(pinNumber), direction(direction) {
    initializeGpio(pinNumber, convertDirection(direction), GPIOD_LINE_BIAS_AS_IS);
}

Pin::Pin(int pinNumber, PinMode mode) 
    : pinNumber(pinNumber), mode(mode) {
    auto [dir, bias] = convertMode(mode);
    initializeGpio(pinNumber, dir, bias);
}
```

**Benefits**:
- Single source of truth for GPIO initialization
- Easier to maintain and update
- Reduces risk of divergence between constructors

**Testing**: Existing tests should pass without changes

---

### 2. Remove std::cerr Direct Output [HIGH PRIORITY]

**Issue**: Library code writes warnings directly to `std::cerr` instead of using logging framework or exceptions.

**Locations**:
- `src/pin.cpp:validatePinNumber()` - Reserved pin warnings
- Possibly others (need to grep)

**Current State**:
```cpp
void Pin::validatePinNumber(int pinNumber) {
    if (pinNumber < 0 || pinNumber > 27) {
        throw InvalidPinError("Pin number must be 0-27, got " + std::to_string(pinNumber));
    }
    // Direct std::cerr output - BAD for library!
    if (pinNumber == 0 || pinNumber == 1 || pinNumber == 14 || pinNumber == 15) {
        std::cerr << "Warning: Pin " << pinNumber << " may be reserved..." << std::endl;
    }
}
```

**Solution Option 1** (Use logging framework):
```cpp
void Pin::validatePinNumber(int pinNumber) {
    if (pinNumber < 0 || pinNumber > 27) {
        throw InvalidPinError("Pin number must be 0-27, got " + std::to_string(pinNumber));
    }
    if (pinNumber == 0 || pinNumber == 1 || pinNumber == 14 || pinNumber == 15) {
        PIPINPP_LOG_WARNING("Pin " << pinNumber << " may be reserved (I2C/UART). "
                            "Use with caution.");
    }
}
```

**Solution Option 2** (Silent with documentation):
```cpp
// Remove warning entirely, document in API_REFERENCE.md
// Users should read docs about reserved pins
```

**Recommendation**: Use Option 1 (logging) so users can control verbosity via `PIPINPP_LOG_LEVEL`.

**Testing**: Add unit test to verify no std::cerr output during normal operation

---

### 3. Optimize Logging Macros [MEDIUM PRIORITY]

**Issue**: Logging macros use `std::endl` which flushes the buffer on every log, hurting performance.

**Location**: `include/log.hpp`

**Current State**:
```cpp
#define PIPINPP_LOG_ERROR(msg) \
    std::cerr << "[PIPINPP ERROR] " << msg << std::endl
```

**Solution**:
```cpp
#define PIPINPP_LOG_ERROR(msg) \
    std::cerr << "[PIPINPP ERROR] " << msg << '\n'
// Only flush on errors:
#define PIPINPP_LOG_ERROR(msg) \
    do { std::cerr << "[PIPINPP ERROR] " << msg << std::endl; } while(0)
```

**Recommendation**: Use `'\n'` for INFO/DEBUG/WARNING, keep `std::endl` (flush) for ERROR level only.

**Testing**: Performance benchmark before/after (timing_benchmark example)

---

### 4. Exception Type Consistency [HIGH PRIORITY]

**Issue**: `attachInterrupt()` throws `std::invalid_argument` while other Arduino compat functions throw custom exceptions.

**Location**: `src/interrupts.cpp`

**Current State**:
```cpp
void attachInterrupt(int pin, std::function<void()> callback, ArduinoInterruptMode mode) {
    if (mode != RISING && mode != FALLING && mode != CHANGE) {
        throw std::invalid_argument("Invalid interrupt mode");  // ❌ Wrong
    }
}
```

**Solution**:
```cpp
void attachInterrupt(int pin, std::function<void()> callback, ArduinoInterruptMode mode) {
    if (mode != RISING && mode != FALLING && mode != CHANGE) {
        throw InvalidPinError("Invalid interrupt mode. Use RISING, FALLING, or CHANGE");
    }
}
```

**Testing**: Update test expectations in `tests/gtest_interrupts.cpp` (if exists)

---

### 5. File Descriptor RAII Wrapper [MEDIUM PRIORITY]

**Issue**: File descriptors in `Wire` and `SPI` are manually managed. Risk of leaks on exceptions.

**Locations**:
- `src/Wire.cpp` - `int fd_`
- `src/SPI.cpp` - `int fd_`

**Current State**:
```cpp
class WireClass {
    int fd_ = -1;  // Manual management
public:
    ~WireClass() { end(); }
    void end() {
        if (fd_ >= 0) {
            close(fd_);
            fd_ = -1;
        }
    }
};
```

**Solution**: Create RAII wrapper
```cpp
// include/FileDescriptor.hpp (new file)
namespace pipinpp {
class FileDescriptor {
    int fd_ = -1;
public:
    FileDescriptor() = default;
    explicit FileDescriptor(int fd) : fd_(fd) {}
    ~FileDescriptor() { close(); }
    
    // Move semantics
    FileDescriptor(FileDescriptor&& other) : fd_(other.fd_) { other.fd_ = -1; }
    FileDescriptor& operator=(FileDescriptor&& other);
    
    // Delete copy
    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor& operator=(const FileDescriptor&) = delete;
    
    void reset(int new_fd = -1);
    void close();
    int get() const { return fd_; }
    bool isOpen() const { return fd_ >= 0; }
    explicit operator bool() const { return isOpen(); }
};
}

// Usage in Wire/SPI:
FileDescriptor fd_;  // Auto-cleanup!
```

**Benefits**:
- Exception-safe resource management
- Prevents double-close bugs
- Cleaner code

**Testing**: Existing tests should pass. Add exception safety test.

---

### 6. Review and Remove Dead Code [LOW PRIORITY]

**Issue**: Comments like `// Removed duplicate delay(...)` suggest cleanup was done, but verify no other dead code remains.

**Action Items**:
- [ ] Grep for `// TODO`, `// FIXME`, `// HACK`, `// XXX`
- [ ] Grep for commented-out function definitions
- [ ] Remove stale comments that refer to old code

**Command**:
```bash
grep -rn "// TODO\|// FIXME\|// HACK\|// XXX\|// Removed\|// Old" src/ include/
```

---

## 📚 Documentation Issues

### 7. Fix Broken PERMISSIONS.md Links [HIGH PRIORITY]

**Issue**: Multiple docs reference `PERMISSIONS.md` which doesn't exist.

**Locations**:
- `docs/FAQ.md` - "see PERMISSIONS.md for detailed setup"
- Possibly `docs/INSTALL.md`, `docs/TROUBLESHOOTING.md`

**Solution Option 1**: Create `docs/PERMISSIONS.md`
```markdown
# GPIO Permissions Setup

This guide explains how to set up GPIO permissions on Raspberry Pi...
[Move content from INSTALL.md section 3]
```

**Solution Option 2**: Remove all references
```markdown
# In FAQ.md:
-Alternatively, see PERMISSIONS.md for detailed setup.
+See the Installation Guide (INSTALL.md#permissions-setup) for detailed setup.
```

**Recommendation**: Option 2 (consolidate in INSTALL.md, update links)

**Action**:
```bash
grep -rn "PERMISSIONS.md" docs/
# Fix all references
```

---

### 8. Update CODE_STANDARDS.md [HIGH PRIORITY]

**Issue 1**: Guide shows `#define HIGH true` but code uses `constexpr bool HIGH = true`
**Issue 2**: Guide prescribes PascalCase headers but `log.hpp`, `exceptions.hpp` are lowercase

**Location**: `docs/CODE_STANDARDS.md`

**Fix 1**: Update constant example
```markdown
# BAD - Old style:
#define HIGH true
#define LOW false

# GOOD - Modern C++:
constexpr bool HIGH = true;
constexpr bool LOW = false;
```

**Fix 2**: Clarify header naming
```markdown
## File Naming

- **Header files**: PascalCase with `.hpp` extension
  - Example: `ArduinoCompat.hpp`, `Pin.hpp`, `Wire.hpp`
  - Exception: Utility headers may use lowercase: `log.hpp`, `exceptions.hpp`
```

---

### 9. Add INPUT_PULLDOWN to API Docs [MEDIUM PRIORITY]

**Issue**: `ArduinoPinMode` enum documentation is incomplete.

**Location**: `docs/API_REFERENCE.md`

**Current State**:
```cpp
enum ArduinoPinMode {
    INPUT = 0,
    OUTPUT = 1,
    INPUT_PULLUP = 2
};
```

**Fix**:
```cpp
enum ArduinoPinMode {
    INPUT = 0,           // Floating input (high impedance)
    OUTPUT = 1,          // Output mode
    INPUT_PULLUP = 2,    // Input with pull-up resistor
    INPUT_PULLDOWN = 3   // Input with pull-down resistor
};
```

---

### 10. Fix Exception Types in Documentation [HIGH PRIORITY]

**Issue**: Docs say functions throw `std::invalid_argument` but they actually throw custom exceptions.

**Locations**:
- `docs/API_REFERENCE.md` - pinMode(), digitalWrite(), etc.

**Current State**:
```markdown
### pinMode(int pin, int mode)
**Throws**: `std::invalid_argument` if pin is invalid
```

**Fix**:
```markdown
### pinMode(int pin, int mode)
**Throws**: 
- `InvalidPinError` if pin number is out of range (0-27)
- `GpioAccessError` if GPIO hardware access fails (permissions, etc.)
```

**Action**: Search/replace all exception documentation

---

### 11. Verify All Internal Links [MEDIUM PRIORITY]

**Issue**: Ensure all doc links work (e.g., to BUILD.md, TROUBLESHOOTING.md, examples)

**Action**:
```bash
# Extract all Markdown links (pattern matches [text](file.md) format)
grep -roh '\[.*\](.*\.md)' docs/ | sort -u

# Manually verify each exists
```

**Automated Check**: Consider adding to CI
```yaml
- name: Check broken links
  run: |
    npm install -g markdown-link-check
    find docs -name "*.md" -exec markdown-link-check {} \;
```

---

## 📁 File Structure Issues

### 12. Header File Naming Consistency [LOW PRIORITY]

**Issue**: `log.hpp`, `exceptions.hpp` are lowercase, others are PascalCase.

**Options**:

**Option A**: Rename to match style guide
```bash
git mv include/log.hpp include/Log.hpp
git mv include/exceptions.hpp include/Exceptions.hpp
# Update all #include statements
```

**Option B**: Update style guide to allow lowercase for utility headers

**Recommendation**: **Option B** (less churn, utility headers are common pattern)

---

### 13. Clean Up Planning Documents [LOW PRIORITY]

**Issue**: TODO.md, ROADMAP.md may have outdated items.

**Action**:
- [x] TODO.md - Already updated to v0.3.10
- [ ] Review `docs/planning/` for stale documents
- [ ] Archive completed milestones

---

## 🧪 Testing Plan

### Test Coverage for Refactoring

1. **Pin Constructor Refactoring**
   - [ ] All existing `gtest_pin.cpp` tests pass
   - [ ] Test both constructors initialize GPIO identically
   - [ ] Test exception handling works

2. **Logging Changes**
   - [ ] No `std::cerr` output in normal operation (unit test)
   - [ ] Reserved pin warnings appear in logs when enabled
   - [ ] Performance: logging macros don't impact disabled logs

3. **Exception Consistency**
   - [ ] `attachInterrupt()` with bad mode throws `InvalidPinError`
   - [ ] Update test expectations in interrupt tests

4. **RAII File Descriptors**
   - [ ] Wire/SPI cleanup properly on destruction
   - [ ] Exception during operation doesn't leak FDs
   - [ ] Move semantics work correctly

5. **Documentation**
   - [ ] All examples compile successfully
   - [ ] No broken internal links
   - [ ] All API descriptions match implementation

---

## 📋 Implementation Checklist

### Phase 1: Critical Code Fixes (v0.3.11-alpha)
- [ ] 1. Refactor Pin constructor duplication
- [ ] 2. Remove std::cerr from Pin::validatePinNumber
- [ ] 4. Fix attachInterrupt() exception type
- [ ] Run full test suite

### Phase 2: Documentation Fixes (v0.3.11-beta)
- [ ] 7. Fix PERMISSIONS.md broken links
- [ ] 8. Update CODE_STANDARDS.md
- [ ] 9. Add INPUT_PULLDOWN to API docs
- [ ] 10. Fix exception types in all documentation
- [ ] 11. Verify all internal links

### Phase 3: Optional Improvements (v0.3.11-rc)
- [ ] 3. Optimize logging macros (performance)
- [ ] 5. Create FileDescriptor RAII wrapper
- [ ] 6. Remove dead code
- [ ] 12. Decide on header file naming

### Phase 4: Release (v0.3.11)
- [ ] Update CHANGELOG.md
- [ ] Update version in all files
- [ ] Create release notes
- [ ] Tag and push v0.3.11

---

## ⚠️ Breaking Changes

**None planned.** This is a patch release focused on internal improvements and documentation fixes. All public APIs remain unchanged.

---

## 📊 Success Criteria

- ✅ All existing tests pass
- ✅ No new compiler warnings
- ✅ No broken documentation links
- ✅ Code coverage maintained or improved
- ✅ Performance not degraded (timing_benchmark)
- ✅ Examples compile and run correctly

---

## 🔗 Related Issues

- Issue #11 - Basic GPIO validation (found some of these issues)
- Issue #16 - Timing benchmark validation
- Future: Create GitHub issues for each refactoring task?

---

**Last Updated**: November 10, 2025  
**Status**: Planning Phase
