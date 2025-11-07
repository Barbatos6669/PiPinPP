# Installation Guide

This guide will help you install and build PiPinPP on your Raspberry Pi.

## Prerequisites
### Hardware Requirements
- Raspberry Pi (any model with GPIO pins)
- Raspberry Pi OS (Bullseye or later recommended)
- SD card with at least 8GB storage

### Software Dependencies

#### Essential Dependencies
```bash
sudo apt update
sudo apt install -y build-essential cmake git
```

#### GPIO Library (libgpiod)
```bash
sudo apt install -y libgpiod-dev gpiod
```

#### Optional: Documentation Tools
```bash
# For generating documentation (optional)
sudo apt install -y doxygen graphviz
```

## Quick Installation

### Method 1: Install Latest Release (v0.3.7)

1. **Clone the repository:**
   ```bash
   git clone https://github.com/Barbatos6669/PiPinPP.git
   cd PiPinPP
   ```

2. **Checkout the latest version (optional):**
   ```bash
   # For latest release
   git checkout v0.3.7
   
   # Or for development branch
   # git checkout main
   ```

3. **Build and install:**
   ```bash
   # Use the build script (recommended)
   ./build.sh
   
   # Install system-wide (requires sudo)
   cd build
   sudo make install
   ```

4. **Verify installation:**
   ```bash
   # Check with pkg-config
   pkg-config --modversion pipinpp
   # Should output: 0.3.7
   
   # Check installed files
   ls /usr/local/include/pipinpp/
   ls /usr/local/lib/libpipinpp.*
   ```

### Method 2: Build Without Installing

If you just want to build and test without system-wide installation:

1. **Clone and build:**
   ```bash
   git clone https://github.com/Barbatos6669/PiPinPP.git
   cd PiPinPP
   ./build.sh
   ```

2. **Run tests:**
   ```bash
   cd build
   ctest --output-on-failure
   # Or run specific examples
   ./examples/example_basic_led
   ```

## Manual Compilation

### Compile Your Own Project

If you want to use PiPinPP in your own project:

```bash
# Include the header and link against the library
g++ -std=c++11 -I/path/to/PiPinPP/include your_project.cpp -lgpiod -o your_project

# Or if you built the static library:
g++ -std=c++11 -I/path/to/PiPinPP/include your_project.cpp /path/to/PiPinPP/build/libPiPinPP.a -lgpiod -o your_project
```

### Include in CMake Project

Add this to your `CMakeLists.txt`:

```cmake
# Find libgpiod
find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)

# Include PiPinPP
include_directories(/path/to/PiPinPP/include)

# Link your executable
target_link_libraries(your_target ${GPIOD_LIBRARIES} /path/to/PiPinPP/build/libPiPinPP.a)
```

## Permissions

### GPIO Access Rights

To access GPIO pins without sudo, add your user to the gpio group:

```bash
sudo usermod -a -G gpio $USER
```

**Note:** You'll need to log out and log back in for group changes to take effect.

### Alternative: udev Rules

Create a udev rule for GPIO access:

```bash
# Create the rule file
sudo nano /etc/udev/rules.d/99-gpio.rules

# Add this line:
KERNEL=="gpiochip*", GROUP="gpio", MODE="0660"

# Reload udev rules
sudo udevadm control --reload-rules
sudo udevadm trigger
```

## Updating PiPinPP

If you already have PiPinPP installed and want to update to a newer version:

### Method 1: Quick Update (Recommended)

```bash
cd PiPinPP
git pull origin main
git checkout v0.3.7    # Replace with desired version
./build.sh
cd build
sudo make install
```

### Method 2: One-Line Reinstall

```bash
curl -sSL https://raw.githubusercontent.com/Barbatos6669/PiPinPP/v0.3.7/install.sh | sudo bash
```

This will automatically update to the specified version.

### Method 3: Fresh Install

If you prefer a clean installation:

```bash
# Remove old installation (optional)
sudo rm -rf /usr/local/include/pipinpp
sudo rm -f /usr/local/lib/libpipinpp.*
sudo rm -f /usr/local/lib/pkgconfig/pipinpp.pc

# Clone and install new version
cd ~
rm -rf PiPinPP
git clone https://github.com/Barbatos6669/PiPinPP.git
cd PiPinPP
git checkout v0.3.7    # Replace with desired version
./build.sh
cd build
sudo make install
```

### Verify Update

Check that the new version is installed:

```bash
pkg-config --modversion pipinpp
# Should show: 0.3.7 (or your target version)
```

### Important Notes

- **Backward Compatibility:** All minor/patch updates (v0.3.x) are 100% backward compatible
- **No Code Changes Needed:** Your existing projects will continue to work
- **Safe to Reinstall:** Installing over an existing version is safe - files are simply replaced
- **Check Release Notes:** Always review release notes for new features: [release_notes/](../release_notes/)

## Verification

### Test GPIO Access

```bash
# Check if GPIO chip is accessible
ls -la /dev/gpiochip*

# Should show:
# crw-rw---- 1 root gpio 254, 0 Oct 29 10:00 /dev/gpiochip0
```

### Test libgpiod Installation

```bash
# Test GPIO detection
gpiodetect

# Should show something like:
# gpiochip0 [pinctrl-bcm2835] (54 lines)
```

## Troubleshooting

Having installation issues? See the comprehensive [TROUBLESHOOTING.md](TROUBLESHOOTING.md) guide.

**Quick fixes for common installation problems:**

**"libgpiod not found":**
```bash
sudo apt install libgpiod-dev
```

**"Permission denied" after installation:**
```bash
sudo usermod -a -G gpio $USER
# Log out and back in
```

**Build fails:**
```bash
# Ensure all dependencies installed
sudo apt install build-essential cmake pkg-config git libgpiod-dev
```

## Development Setup

### For Contributing to PiPinPP

```bash
# Install additional development tools
sudo apt install -y clang-format cppcheck valgrind

# Clone with all branches
git clone --recurse-submodules https://github.com/Barbatos6669/PiPinPP.git
cd PiPinPP

# Set up development branch
git checkout -b feature/your-feature-name
```

## Next Steps

Once installed, check out:
- [GETTING_STARTED.md](GETTING_STARTED.md) - Your first GPIO program
- [examples/](../examples/) - Sample code and usage patterns
- [API_REFERENCE.md](API_REFERENCE.md) - Complete API reference
- [README.md](../README.md) - Project overview

## Getting Help

- **Issues:** [GitHub Issues](https://github.com/Barbatos6669/PiPinPP/issues)
- **Discord:** [Community Support](https://discord.gg/wXeZP8Ev)
- **Documentation:** [Full docs](https://barbatos6669.github.io/PiPinPP/)

---

**Platform Support:** Raspberry Pi OS (Bullseye+), Ubuntu 20.04+  
**Last Updated:** November 5, 2025