# Testing & Code Coverage

**PiPin++** uses GoogleTest for unit testing and lcov for code coverage analysis.

## Quick Start

### Running Tests
```bash
# Build with tests (default)
./build.sh

# Run all tests
cd build && ctest --output-on-failure

# Run specific test suite
./build/gtest_pin
./build/gtest_wire
```

### Generating Coverage Reports
```bash
# Build with coverage instrumentation
cmake -DPIPINPP_ENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug -B build
cmake --build build

# Generate HTML coverage report
./coverage.sh

# View in browser
./coverage.sh --view

# Clean coverage data
./coverage.sh --clean
```

## Current Coverage Status

**Overall: 52.2% line coverage, 73.1% function coverage** (Target: ≥80%)

### Per-File Breakdown

| File | Line Coverage | Function Coverage | Status | Priority |
|------|--------------|-------------------|--------|----------|
| `src/pin.cpp` | 83.1% | 100% | ✅ Good | Low |
| `src/Wire.cpp` | 75.4% | 87.5% | ⚠️ Near target | Medium |
| `src/Serial.cpp` | 67.0% | 100% | ⚠️ Needs work | Medium |
| `src/pwm.cpp` | 62.4% | 71.4% | ⚠️ Needs work | Medium |
| `src/platform.cpp` | 61.4% | 95.5% | ⚠️ Needs work | High |
| `src/ArduinoCompat.cpp` | 55.2% | 44.4% | ❌ Insufficient | High |
| `src/SPI.cpp` | 50.4% | 80.0% | ❌ Insufficient | High |
| `src/HardwarePWM.cpp` | 31.3% | 59.1% | ❌ Needs hardware | High |
| `src/interrupts.cpp` | 0.0% | 0.0% | ❌ **No tests!** | **CRITICAL** |

## Test Organization

### Unit Tests (`tests/gtest_*.cpp`)
- **gtest_pin.cpp** - Pin class tests (36 tests)
- **gtest_exceptions.cpp** - Exception handling (10 tests)
- **gtest_timing.cpp** - Timing functions (8 tests)
- **gtest_wire.cpp** - I²C/Wire interface (25 tests)
- **gtest_spi.cpp** - SPI interface (22 tests)
- **gtest_advanced_io.cpp** - Advanced I/O (pulseIn, shiftOut, tone) (24 tests)
- **gtest_serial.cpp** - Serial/UART interface (40 tests)
- **gtest_hardware_pwm.cpp** - Hardware PWM (26 tests)
- **gtest_platform.cpp** - Platform detection (16 tests)

**Total: 192 tests** (179 run, 13 skipped)

### Integration Tests (`tests/test_*.cpp`)
- **test_thread_safety.cpp** - Concurrent access patterns
- **test_exceptions.cpp** - Exception propagation
- **test_timing.cpp** - Real-world timing accuracy

## Understanding Coverage Gaps

### Hardware-Dependent Code (Expected)
Some code requires physical hardware and is skipped in CI:
- **SPI transfers** - Requires `/dev/spidev0.X` and connected device
- **Serial loopback** - Needs physical RX→TX jumper
- **Hardware PWM** - Requires `/sys/class/pwm` support
- **Interrupt callbacks** - Needs GPIO edge events

**Solution:** Hardware-in-the-loop testing (self-hosted runner with test rig)

### Untested Code (Needs Attention)
Code that CAN be tested but isn't:
- **interrupts.cpp (0%)** - No mock tests at all!
  - Need tests for `attachInterrupt()`, `detachInterrupt()`
  - Mock GPIO edge events with test fixtures
  - Test callback registration and cleanup
  
- **ArduinoCompat.cpp (55.2%)** - Main API surface
  - More edge case tests for `analogWrite()`, `analogRead()`
  - Test pin registry cleanup and thread safety
  - Test global state consistency
  
- **platform.cpp (61.4%)** - Detection logic
  - Test on different Raspberry Pi models (mock `/proc/cpuinfo`)
  - Test fallback paths when hardware unavailable
  - Test peripheral address detection

## Improving Coverage

### 1. Add Tests for Interrupts (0% → 80%)
**Priority: CRITICAL** - This is completely untested!

```cpp
// tests/gtest_interrupts.cpp (NEW FILE)
TEST(InterruptTest, AttachInterruptSetsCallback) {
    // Mock GPIO edge events or use loopback with digitalWrite
}

TEST(InterruptTest, DetachInterruptRemovesCallback) {
    // Verify callback no longer fires
}

TEST(InterruptTest, MultipleInterruptsOnDifferentPins) {
    // Concurrent interrupt handling
}
```

### 2. Mock Hardware for SPI/Serial Tests
- Use loopback testing (connect MOSI→MISO, TX→RX)
- Create mock `/dev/spidev0.0` using `socat` or test fixtures
- Inject test data into serial buffers

### 3. Increase ArduinoCompat Edge Cases
- Test invalid pin numbers
- Test concurrent calls to `pinMode()` on same pin
- Test cleanup when pins go out of scope
- Test `analogWrite()` with various duty cycles

### 4. Platform Detection Mocking
```cpp
// Mock /proc/cpuinfo with different Pi models
TEST(PlatformTest, DetectsRaspberryPi5) {
    // Mock cpuinfo: "Model : Raspberry Pi 5 Model B Rev 1.0"
}
```

## Coverage Goals

### v0.4.x (Current)
- ✅ Coverage infrastructure complete (lcov, coverage.sh, CMake integration)
- 🎯 Baseline: 52.2% line coverage established
- 🎯 Target: ≥60% (focus on interrupts, ArduinoCompat)

### v0.5.0
- 🎯 Target: ≥70% (hardware mocking, SPI/Serial loopback)
- Add hardware-in-the-loop tests with self-hosted CI runner

### v0.6.0
- 🎯 Target: ≥80% (comprehensive edge cases, platform mocks)
- Integrate coverage badge with CI (auto-update on PRs)

## Coverage Tools

### lcov Commands
```bash
# Capture coverage data
lcov --directory . --capture --output-file coverage.info --ignore-errors inconsistent

# Filter system headers
lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info.cleaned

# Generate HTML report
genhtml coverage.info.cleaned --output-directory coverage_html

# View summary
lcov --list coverage.info.cleaned
```

### coverage.sh Script
Automated wrapper for coverage workflow:
```bash
./coverage.sh           # Generate report
./coverage.sh --view    # Generate + open in browser
./coverage.sh --clean   # Remove coverage data
./coverage.sh --help    # Show usage
```

## CI Integration

### GitHub Actions (Future)
```yaml
- name: Generate Coverage
  run: |
    cmake -DPIPINPP_ENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug -B build
    cmake --build build
    cd build && ctest --output-on-failure
    lcov --capture --directory . --output-file coverage.info --ignore-errors inconsistent
    lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info.cleaned

- name: Upload to Codecov
  uses: codecov/codecov-action@v3
  with:
    files: ./build/coverage.info.cleaned
    fail_ci_if_error: true
```

### Coverage Badge
Update badge in README when coverage changes:
```markdown
[![Coverage](https://img.shields.io/badge/coverage-52.2%25-orange)](build/coverage_html/index.html)
```

Colors:
- **Green (≥80%)**: `brightgreen`
- **Yellow (60-79%)**: `yellow`
- **Orange (40-59%)**: `orange`
- **Red (<40%)**: `red`

## Best Practices

### Writing Testable Code
1. **Dependency Injection**: Pass hardware interfaces as parameters
2. **Mock-Friendly**: Use virtual interfaces for hardware access
3. **RAII**: Resource cleanup in destructors (easier to test)
4. **Pure Functions**: Avoid global state when possible

### Test Structure
```cpp
TEST(TestSuite, TestName) {
    // Arrange - Set up test fixtures
    Pin testPin(17, PinDirection::OUTPUT);
    
    // Act - Execute the code under test
    testPin.write(true);
    
    // Assert - Verify expected behavior
    EXPECT_EQ(testPin.read(), true);
}
```

### Hardware-Dependent Tests
Use `GTEST_SKIP()` for tests that require physical hardware:
```cpp
TEST(SPITest, TransferReturnsByte) {
    if (!SPI.begin()) {
        GTEST_SKIP() << "SPI hardware not available";
    }
    // Test SPI transfer...
}
```

## Debugging Low Coverage

### Identify Uncovered Lines
1. Open `build/coverage_html/index.html` in browser
2. Click on file with low coverage
3. **Red lines** = Not executed
4. **Green lines** = Covered
5. **Orange lines** = Partially covered (branches)

### Common Causes
- **Error paths** not tested (exception handling)
- **Edge cases** missing (boundary conditions)
- **Hardware dependencies** skipped
- **Dead code** never executed (should be removed)

### Example: Improving pin.cpp from 83% → 90%
1. Check report: `/home/larry/Development/PiPinPP/build/coverage_html/pin.cpp.gcov.html`
2. Find red lines (uncovered code)
3. Write tests specifically for those lines
4. Re-run coverage: `./coverage.sh`
5. Verify improvement

## Contributing

When submitting PRs:
1. **Add tests** for new features
2. **Maintain coverage** - Don't decrease overall %
3. **Document test strategy** in PR description
4. **Run coverage locally** before pushing

Target: Every PR should maintain or improve coverage percentage.

## Resources

- [GoogleTest Documentation](https://google.github.io/googletest/)
- [lcov Manual](https://github.com/linux-test-project/lcov)
- [gcov Documentation](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html)
- [Code Coverage Best Practices](https://testing.googleblog.com/2020/08/code-coverage-best-practices.html)

---

**Last Updated:** 2026-01-27  
**Coverage Baseline:** 52.2% line, 73.1% function  
**Next Target:** 60% (focus on interrupts.cpp, ArduinoCompat.cpp)
