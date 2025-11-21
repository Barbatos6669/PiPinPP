# PiPinPP - AI Coding Agent Instructions

## Project Overview

**PiPinPP** is a modern C++ GPIO library for Raspberry Pi that provides an Arduino-compatible API over libgpiod v2. The core mission: make Arduino makers instantly productive on Raspberry Pi without learning new APIs.

**Key Architectural Principle**: Two-layer design
- **Low-level**: Object-oriented `Pin` class (RAII, exceptions, modern C++)
- **High-level**: Arduino-compatible global functions (`pinMode`, `digitalWrite`, etc.)

## Essential Build Knowledge

### Critical Build Commands
```bash
# Standard development build
./scripts/build.sh --clean --debug

# Release build with tests
./scripts/build.sh

# Build with logging (for debugging GPIO operations)
cmake -DPIPINPP_ENABLE_LOGGING=ON -DPIPINPP_LOG_LEVEL=0 -B build
```

**Important**: libgpiod v2.2.1+ is required (not the v1.x in Ubuntu repos). CI builds it from source; install script handles this automatically.

### CMake Options (see CMakeLists.txt)
- `PIPINPP_ENABLE_LOGGING=ON` - Essential for debugging GPIO issues (default: OFF)
- `PIPINPP_LOG_LEVEL=0` - DEBUG (0), INFO (1), WARNING (2), ERROR (3)
- `PIPINPP_WARNINGS_AS_ERRORS=ON` - Strict build mode (used in CI)
- `BUILD_TESTS=ON` - GoogleTest unit tests (default: ON)
- `BUILD_EXAMPLES=ON` - Example programs (default: ON)

### Testing Strategy
- **Unit tests** (`tests/gtest_*.cpp`) - Can run without hardware, use `GTEST_SKIP()` for hardware-dependent tests
- **Hardware tests** - Require `/dev/gpiochip0`, typically skip in CI
- Run tests: `cd build && ctest --output-on-failure`

## Code Architecture & Patterns

### 1. Arduino Compatibility Layer (ArduinoCompat.hpp/cpp)
**Critical Pattern**: Thread-safe pin registry using `std::map<int, std::unique_ptr<Pin>>` with mutex
```cpp
// DON'T create new Pin objects - reuse from registry
static std::map<int, std::unique_ptr<Pin>> pinRegistry;
static std::mutex pinMutex;
```
**Why**: Multiple `pinMode()` calls on same pin must return same Pin object for thread safety.

### 2. Pin Class Design (pin.hpp/cpp)
- Uses libgpiod v2 API (`gpiod_chip`, `gpiod_line_request`)
- RAII: Constructor opens chip/line, destructor releases resources
- Two constructors: `PinDirection` (basic) vs `PinMode` (with pull resistors)
- Pin validation: 0-27 for Raspberry Pi, warnings for reserved pins (0,1,14,15)

### 3. Exception Hierarchy (exceptions.hpp)
```
PinError (base)
├── InvalidPinError - Invalid pin numbers
└── GpioAccessError - Permission/hardware errors
```
**Always** throw exceptions for errors (never return error codes).

### 4. Interrupts (interrupts.hpp/cpp)
- Uses libgpiod v2 edge event monitoring in background thread
- One interrupt manager per pin (singleton-per-pin pattern)
- Callbacks run in separate thread - must be fast, non-blocking

### 5. Wire/I2C (Wire.hpp/cpp)
- **Namespace**: `pipinpp::Wire` with global instance
- Uses Linux i2c-dev (`/dev/i2c-N`) not libgpiod
- Thread-safe with internal mutex
- Auto-detects I2C bus: Pi 0-4 use `/dev/i2c-1`, Pi 5 uses `/dev/i2c-20`

## Project-Specific Conventions

### Coding Style (CODE_STANDARDS.md)
- **Braces**: Allman style (opening brace on new line)
```cpp
void function()
{
    if (condition)
    {
        doWork();
    }
}
```
- **Indentation**: 4 spaces (never tabs)
- **Line length**: 100 characters max
- **Naming**:
  - Classes: `PascalCase`
  - Functions/variables: `camelCase`
  - Constants: `SCREAMING_SNAKE_CASE`
  - Arduino functions: Match exactly (`digitalWrite` not `writePin`)

### Documentation Requirements
- Doxygen comments for all public APIs
- Include `@throws`, `@note`, `@warning`, `@example` tags
- Examples must compile and demonstrate real-world usage

### File Organization
```
include/       - Public headers ONLY (.hpp)
src/           - Implementation (.cpp)
examples/      - One directory per example with main.cpp
tests/         - gtest_*.cpp for unit tests, test_*.cpp for integration
docs/          - Comprehensive markdown docs
```

## Common Workflows

### Adding New Arduino-Compatible Function
1. Declare in `ArduinoCompat.hpp` (with full Doxygen)
2. Implement in `ArduinoCompat.cpp` using pin registry pattern
3. Add unit test in appropriate `tests/gtest_*.cpp`
4. Add example in `examples/` directory
5. Update `docs/API_REFERENCE.md`

### Adding New Feature to Pin Class
1. Update `Pin` class in `pin.hpp`
2. Implement in `src/pin.cpp`
3. Add tests in `tests/gtest_pin.cpp`
4. Consider if Arduino wrapper needed in `ArduinoCompat.hpp`

### Working with Examples
- Each example is self-contained in `examples/feature_name/`
- Must include hardware setup comments
- Should handle exceptions and print helpful error messages
- Build all examples with `BUILD_EXAMPLES=ON`

## Hardware & Platform Knowledge

### GPIO Pin Constraints
- Valid pins: 0-27 on Raspberry Pi
- Reserved pins: 0,1 (I2C), 14,15 (UART) - warn but allow
- Voltage: 3.3V logic (NOT 5V tolerant)

### libgpiod API Migration
- **Old (v1)**: `gpiod_line`, `gpiod_line_request_input/output`
- **New (v2)**: `gpiod_line_request`, `gpiod_line_config` (this project uses v2)

### Permission Requirements
- GPIO access requires membership in `gpio` group
- `/dev/gpiochip*` devices must be accessible
- `scripts/install.sh` creates udev rules for non-root access

## Testing in CI (.github/workflows/ci.yml)

**Critical**: CI has no `/dev/gpiochip0`, so:
1. Build libgpiod v2.2.1 from source (Ubuntu repos only have v1)
2. Skip hardware-dependent tests: `ctest -E "basic_test|thread_safety_test"`
3. Test installation with pkg-config integration
4. Optional self-hosted runner for hardware tests (currently disabled)

## Common Pitfalls

1. **Don't mix v1 and v2 libgpiod APIs** - This project uses v2 exclusively
2. **Thread safety**: Always use mutex when accessing `pinRegistry`
3. **Mutex deadlocks**: Don't call mutex-locked methods from within other mutex-locked methods (see Wire.cpp fix)
4. **Resource cleanup**: Pin destructor must safely release GPIO lines
5. **Interrupt callbacks**: Must be fast and non-blocking (no `delay()`)
6. **PWM limitations**: Software PWM has jitter, warn users about limitations
7. **I2C bus detection**: Different Pi models use different `/dev/i2c-N` numbers

## Key Files to Reference

- `ArduinoCompat.hpp` - Complete Arduino API surface
- `docs/CODE_STANDARDS.md` - Strict style guide
- `docs/API_REFERENCE.md` - Comprehensive API documentation
- `CMakeLists.txt` - Build configuration and options
- `.github/workflows/ci.yml` - CI build process (heavily commented)

## Version & Release Process

**Current Development**: v0.3.8 (unreleased, in development)
**Last Released**: v0.3.7 (November 6, 2025)

- Major versions: Breaking API changes
- Minor versions: New features (backward compatible)
- Patch versions: Bug fixes
- Release notes in `release_notes/vX.Y.Z.md`

## Development Roadmap & Priorities

### v0.3.7 - Peripheral Expansion (Current)
**Priority Focus**: Communication protocols and hardware interfaces

1. **✅ I²C/Wire Interface** (COMPLETED)
   - Full Arduino Wire API compatibility
   - Helper functions: `readRegister()`, `writeRegister()`, `scan()`
   - Examples: BMP280, MPU6050, SSD1306, I2C scanner

2. **SPI Interface** (PLANNED)
   - Create `SPI` class using `/dev/spidev0.*`
   - Configurable mode, bit order, clock speed
   - Example: 74HC595 shift register (hardware vs software comparison)

3. **Hardware PWM Wrapper** (PLANNED)
   - Interface with `/sys/class/pwm` or libgpiod PWM extension
   - Frequency + duty cycle settings
   - Example: Smooth LED fade or servo control with stable frequency

4. **Utility Functions** (PARTIAL)
   - ✅ `map()`, `constrain()` - Linear mapping and value limiting
   - ✅ Bit manipulation: `bit()`, `bitRead()`, `bitWrite()`, `bitSet()`, `bitClear()`
   - ✅ Byte operations: `highByte()`, `lowByte()`
   - Consider: `analogWriteResolution()`, `analogReadResolution()` (stubs for future)

### v0.3.8 - Documentation & Tutorials (NEXT)
**Priority Focus**: Complete documentation and learning resources

1. **Tutorial Completion**
   - PWM tutorial with wiring diagrams
   - Interrupts tutorial with debouncing patterns
   - Serial/I2C/SPI communication tutorials
   - Advanced GPIO patterns (state machines, event-driven design)

2. **Versioned Documentation**
   - Auto-generate with Doxygen
   - Publish via GitHub Pages (pipinpp.github.io/docs)
   - Version per release: `/v0.3.6/`, `/v0.3.7/`, etc.

3. **Arduino Porting Guide**
   - Table: Arduino Function → PiPin++ Equivalent
   - Example conversions with explanations
   - Common migration patterns

4. **Performance Benchmarks**
   - Toggle speeds, timing accuracy measurements
   - Comparisons to WiringPi/pigpio
   - `benchmark.cpp` example

### v0.3.9 - Testing & Packaging (FUTURE)
**Priority Focus**: Production-ready infrastructure

1. **Integration Tests**
   - Hardware-in-the-loop: output→input loopback tests
   - I²C transaction simulation/mocking
   - SPI loopback testing

2. **CI Improvements**
   - Cache build dependencies
   - Release build job (package artifacts)
   - Performance regression testing

3. **Debian Packaging**
   - CMake CPack configuration
   - `.deb` package generation
   - Include `/usr/share/doc/pipinpp` with docs

### v0.4.0 - Arduino Parity (MILESTONE)
**Goal**: Feature-complete Arduino-compatible GPIO library

- Full hardware I/O suite (GPIO, PWM, I²C, SPI, UART)
- Refined API stability
- Complete documentation
- Production-ready for robotics/IoT projects
- Consider submission to vcpkg/ConanCenter

### When Implementing New Features

**For v0.3.7 (Current Sprint)**:
- Focus on SPI and Hardware PWM
- I²C is complete, only fix bugs
- Add utility functions as needed
- All new features need: examples, tests, documentation

**Design Principles**:
- Arduino API compatibility first
- Thread-safe by default
- RAII resource management
- Clear error messages for makers
- Examples must show real-world hardware usage

## When Making Changes

1. **Run tests locally**: `./scripts/build.sh && cd build && ctest --output-on-failure`
2. **Check logging output**: Build with `PIPINPP_ENABLE_LOGGING=ON` for GPIO debugging
3. **Verify examples compile**: `BUILD_EXAMPLES=ON` in CMake
4. **Update API docs**: Keep `docs/API_REFERENCE.md` synchronized
5. **Consider Arduino users**: Will this change confuse Arduino developers?

## Documentation Update Checklist

When adding new functionality, **update ALL of these files** (version numbers must match):

### Version Numbers (Current Development: 0.3.7)
- [ ] `CMakeLists.txt` - `project(PiPinPP VERSION 0.3.7 ...)`
- [ ] `README.md` - Badge version, install script URLs, all references
- [ ] `docs/API_REFERENCE.md` - Version at top of file
- [ ] `scripts/install.sh` - VERSION variable
- [ ] Create `release_notes/vX.Y.Z.md` for the new version

### API Documentation
- [ ] `docs/API_REFERENCE.md` - Add new functions with examples
- [ ] Header files (`include/*.hpp`) - Complete Doxygen comments
- [ ] `README.md` - Update feature list if adding major functionality
- [ ] `CHANGELOG.md` - Document all user-facing changes

### Examples & Tests
- [ ] Add example in `examples/feature_name/` with main.cpp
- [ ] Add tests in `tests/gtest_*.cpp` (or new test file)
- [ ] Update example READMEs if affected

### User-Facing Docs
- [ ] `docs/GETTING_STARTED.md` - If affects beginners
- [ ] `docs/TROUBLESHOOTING.md` - If new failure modes exist
- [ ] `docs/FAQ.md` - If commonly asked about

**Search for old version numbers**: `grep -r "0.3.6" docs/ README.md` to find stale references

## Version Update Routine (BEFORE RELEASE)

When preparing to release a new version, follow this routine to prevent version drift:

### 1. Update Core Version Files
```bash
# Update CMakeLists.txt
# Change: project(PiPinPP VERSION 0.3.7 ...)

# Update scripts/install.sh
# Change: VERSION="v0.3.7"

# Update docs/API_REFERENCE.md header
# Change: **Version**: 0.3.7
# Change: **Date**: [Current Date]
```

### 2. Update All Documentation References
Run this search to find all version references:
```bash
grep -r "0\.3\.6" docs/ README.md examples/ scripts/install.sh CMakeLists.txt --include="*.md" --include="*.cpp" --include="*.sh" | grep -v "CHANGELOG" | grep -v "release_notes"
```

Update each file found:
- [ ] `README.md` - Badge, install URLs, feature sections (Current Features, Advanced Features, Quality & Testing)
- [ ] `docs/API_REFERENCE.md` - Version header and date
- [ ] `docs/GETTING_STARTED.md` - Install URLs, expected version output
- [ ] `docs/INSTALL.md` - All example commands and expected outputs
- [ ] `docs/FAQ.md` - Install URLs, production-ready version, roadmap
- [ ] `docs/TROUBLESHOOTING.md` - Example git checkout commands
- [ ] `docs/planning/TODO.md` - Current Version header
- [ ] `examples/*/main.cpp` - Version display strings in output
- [ ] `examples/*/README.md` - Version references in test results

### 3. Update Features List in README.md
Ensure README.md reflects ALL features in the new version:
- Update "Current Features" section version number
- Update "Advanced Features" section with new features
- Update test count if changed
- Update example count if changed
- Update production-ready statement

### 4. Create Release Notes
```bash
# Create release notes file
touch release_notes/v0.3.7.md

# Document:
# - New features added
# - Bug fixes
# - Breaking changes (if any)
# - Migration guide (if needed)
```

### 5. Update CHANGELOG.md
Add new version entry at the top with:
- Date of release
- New features
- Bug fixes
- Documentation updates

### 6. Verify Version Consistency
```bash
# Check all versions match
echo "CMakeLists.txt:" && grep "VERSION 0.3" CMakeLists.txt | head -1
echo "README.md badge:" && grep "badge/version" README.md
echo "scripts/install.sh:" && grep "^VERSION=" scripts/install.sh
echo "API_REFERENCE.md:" && head -5 docs/API_REFERENCE.md | grep "Version:"
```

### 7. Final Check Before Commit
- [ ] All tests pass: `cd build && ctest --output-on-failure`
- [ ] Examples compile: `BUILD_EXAMPLES=ON` verified
- [ ] Documentation builds without errors
- [ ] No references to old version remain (except in CHANGELOG/release_notes)
- [ ] Git status shows only intended changes

### 8. After Release
- [ ] Update this file (.github/copilot-instructions.md) to show new development version
- [ ] Example: "Current Development: v0.3.8" and "Last Released: v0.3.7"

## Community & Contributions

### Contributor Guidelines
- See `CONTRIBUTING.md` and `CODE_STANDARDS.md` for full details
- All PRs need: code, tests, examples, documentation
- Follow Allman brace style, Arduino naming conventions
- Include hardware setup instructions in examples

### Growing the Ecosystem
- **Show & Tell**: GitHub Discussions for user projects
- **Outreach**: Share demos on r/raspberry_pi, Hackster.io
- **Package Managers**: Target vcpkg/ConanCenter at v0.4.0
- **Integration**: Ensure `find_package(PiPinPP)` works cleanly

---

**Philosophy**: Prefer clarity over cleverness. Arduino developers are learning C++ - make code readable and errors helpful.
