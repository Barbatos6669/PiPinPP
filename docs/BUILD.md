# Building PiPinPP

This document explains how to build PiPinPP using CMake.

## Prerequisites

- CMake 3.16 or later
- C++17 compatible compiler (GCC 7+ or Clang 5+)
- libgpiod development package

### Installing Prerequisites on Raspberry Pi OS

```bash
sudo apt update
sudo apt install cmake build-essential libgpiod-dev pkg-config
```

## Quick Build

Use the provided build script for the easiest build experience:

```bash
./build.sh
```

### Build Script Options

- `--debug`: Build in debug mode (default: Release)
- `--clean`: Clean build directory before building  
- `--install`: Install after building
- `--help`: Show help message

Examples:
```bash
./build.sh --debug          # Debug build
./build.sh --clean          # Clean and build
./build.sh --install        # Build and install
./build.sh --clean --debug  # Clean debug build
```

## Manual Build

If you prefer to build manually:

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
make -j$(nproc)

# Run tests
ctest --output-on-failure

# Install (optional)
sudo make install
```

## Build Options

The CMake configuration supports the following options:

- `BUILD_TESTS`: Build test executables (default: ON)
- `BUILD_EXAMPLES`: Build example programs (default: ON)
- `CMAKE_BUILD_TYPE`: Build type (Debug/Release, default: Release)
- `PIPINPP_ENABLE_LOGGING`: Enable debug logging output (default: OFF)
- `PIPINPP_LOG_LEVEL`: Logging level when enabled: 0=DEBUG, 1=INFO, 2=WARNING, 3=ERROR (default: 1)
- `PIPINPP_WARNINGS_AS_ERRORS`: Treat compiler warnings as errors (default: OFF)

### Examples with Custom Options

```bash
# Standard release build (no logging, warnings enabled)
cmake -DCMAKE_BUILD_TYPE=Release ..

# Debug build with logging enabled
cmake -DCMAKE_BUILD_TYPE=Debug -DPIPINPP_ENABLE_LOGGING=ON ..

# Debug build with detailed logging
cmake -DCMAKE_BUILD_TYPE=Debug -DPIPINPP_ENABLE_LOGGING=ON -DPIPINPP_LOG_LEVEL=0 ..

# Strict build (warnings treated as errors)
cmake -DPIPINPP_WARNINGS_AS_ERRORS=ON ..

# No examples, just library and tests
cmake -DBUILD_EXAMPLES=OFF ..
```

### Logging

By default, PiPinPP builds without any logging overhead. Enable logging for debugging:

```bash
cmake -DPIPINPP_ENABLE_LOGGING=ON ..
```

Log levels:
- **0 = DEBUG**: Detailed internal operations (e.g., resource cleanup)
- **1 = INFO**: Important operations (e.g., pin initialization) - **default**
- **2 = WARNING**: Potential issues
- **3 = ERROR**: Errors only

Example output with logging enabled:
```
[INFO] Initializing pin 17 as OUTPUT on chip gpiochip0
[INFO] pinMode: Set pin 17 to OUTPUT
[DEBUG] Releasing resources for pin 17
```

### Compiler Warnings

The library builds with `-Wall -Wextra -Wpedantic` enabled by default. For strict development:

```bash
cmake -DPIPINPP_WARNINGS_AS_ERRORS=ON ..
```

This treats all warnings as errors, ensuring code quality.

## Build Outputs

After building, you'll find:

- **Library**: `build/libpipinpp.a` - Static library
- **Tests**: `build/test_pipinpp` - Test executable
- **Examples**: `build/examples/` - Example programs:
  - `example_arduino_style` - Arduino-compatible LED blinking
  - `example_basic_led` - Object-oriented LED control 
  - `example_button_input` - Button reading with debouncing
  - `example_arduino_migration` - Arduino to PiPinPP migration guide

## Installation

To install the library system-wide:

```bash
sudo make install
```

This installs:
- Library to `/usr/local/lib/`
- Headers to `/usr/local/include/`
- pkg-config file to `/usr/local/lib/pkgconfig/`

## Using PiPinPP in Your Project

### With pkg-config

```bash
g++ -o myprogram myprogram.cpp $(pkg-config --cflags --libs pipinpp)
```

### With CMake

```cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(PIPINPP REQUIRED pipinpp)

target_link_libraries(your_target ${PIPINPP_LIBRARIES})
target_include_directories(your_target PRIVATE ${PIPINPP_INCLUDE_DIRS})
```

### Manual linking

```bash
g++ -o myprogram myprogram.cpp -lpipinpp -lgpiod
```