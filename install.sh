#!/bin/bash
# PiPinPP Quick Installation Script
# This script downloads, builds, and installs PiPinPP on your Raspberry Pi
#
# Usage:
#   curl -sSL https://raw.githubusercontent.com/Barbatos6669/PiPinPP/main/install.sh | sudo bash
#   
# Or download and run:
#   wget https://raw.githubusercontent.com/Barbatos6669/PiPinPP/main/install.sh
#   sudo bash install.sh

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
PIPINPP_VERSION="v0.3.11"
INSTALL_PREFIX="/usr/local"
BUILD_DIR="/tmp/pipinpp-build"
REPO_URL="https://github.com/Barbatos6669/PiPinPP.git"

# Print functions
print_info() {
    echo -e "${BLUE}ℹ${NC} $1"
}

print_success() {
    echo -e "${GREEN}✓${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}⚠${NC} $1"
}

print_error() {
    echo -e "${RED}✗${NC} $1"
}

print_header() {
    echo ""
    echo -e "${BLUE}═══════════════════════════════════════════════════════${NC}"
    echo -e "${BLUE}  $1${NC}"
    echo -e "${BLUE}═══════════════════════════════════════════════════════${NC}"
    echo ""
}

# Check if running as root
check_root() {
    if [ "$EUID" -ne 0 ]; then
        print_error "This script must be run as root (use sudo)"
        exit 1
    fi
}

# Detect system
detect_system() {
    print_info "Detecting system..."
    
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        OS=$NAME
        VER=$VERSION_ID
        print_success "Detected: $OS $VER"
    else
        print_warning "Cannot detect OS, assuming Debian-based system"
    fi
    
    # Check architecture
    ARCH=$(uname -m)
    print_info "Architecture: $ARCH"
    
    if [[ "$ARCH" != "aarch64" && "$ARCH" != "armv7l" && "$ARCH" != "x86_64" ]]; then
        print_warning "Unsupported architecture: $ARCH (PiPinPP is designed for ARM)"
    fi
}

# Install dependencies
install_dependencies() {
    print_header "Installing Dependencies"
    
    print_info "Updating package list..."
    apt-get update -qq
    
    print_info "Installing core build dependencies..."
    apt-get install -y -qq \
        build-essential \
        cmake \
        pkg-config \
        git \
        || {
            print_error "Failed to install core dependencies"
            exit 1
        }
    
    print_info "Attempting to install libgpiod from repositories..."
    # Try to install libgpiod, but don't fail if packages don't exist
    apt-get install -y -qq libgpiod-dev libgpiod2 gpiod 2>/dev/null || {
        print_warning "Could not install libgpiod from repositories (packages may not exist)"
    }
    
    print_success "Core dependencies installed"
    
    # Check libgpiod version
    GPIOD_VERSION=$(pkg-config --modversion libgpiod 2>/dev/null || echo "unknown")
    print_info "libgpiod version: $GPIOD_VERSION"
    
    # Check if we have libgpiod v2.x or need to build from source
    NEEDS_BUILD=false
    
    if [[ "$GPIOD_VERSION" == "unknown" ]]; then
        print_warning "libgpiod not found via pkg-config"
        NEEDS_BUILD=true
    elif [[ "$GPIOD_VERSION" < "2.0" ]]; then
        print_warning "libgpiod version < 2.0 detected (found $GPIOD_VERSION)"
        print_warning "PiPinPP requires libgpiod v2.x"
        NEEDS_BUILD=true
    else
        print_success "libgpiod v$GPIOD_VERSION is compatible"
    fi
    
    if [ "$NEEDS_BUILD" = true ]; then
        print_info "Attempting to install from backports..."
        
        # Try to find a newer version from backports
        if apt-get install -y -qq libgpiod-dev/testing 2>/dev/null; then
            print_success "Installed libgpiod from testing repository"
            GPIOD_VERSION=$(pkg-config --modversion libgpiod 2>/dev/null || echo "unknown")
            
            if [[ "$GPIOD_VERSION" < "2.0" ]]; then
                print_warning "Backports version still too old, building from source..."
                build_libgpiod_from_source
            fi
        elif apt-get install -y -qq libgpiod-dev/unstable 2>/dev/null; then
            print_success "Installed libgpiod from unstable repository"
            GPIOD_VERSION=$(pkg-config --modversion libgpiod 2>/dev/null || echo "unknown")
            
            if [[ "$GPIOD_VERSION" < "2.0" ]]; then
                print_warning "Backports version still too old, building from source..."
                build_libgpiod_from_source
            fi
        else
            print_warning "Cannot find libgpiod v2.x in repositories"
            print_info "Building libgpiod from source..."
            build_libgpiod_from_source
        fi
    fi
}

# Build libgpiod from source (fallback)
build_libgpiod_from_source() {
    print_info "Building libgpiod v2.2.1 from source..."
    
    print_info "Installing build dependencies for libgpiod..."
    # Install additional build deps - try common package names
    apt-get install -y -qq \
        autoconf \
        autoconf-archive \
        automake \
        libtool \
        python3 \
        python3-setuptools \
        libkmod-dev \
        libudev-dev \
        || true
    
    # Try to install kernel headers (package name varies by distro)
    apt-get install -y -qq linux-headers-generic 2>/dev/null || \
    apt-get install -y -qq linux-headers-$(uname -r) 2>/dev/null || \
    apt-get install -y -qq raspberrypi-kernel-headers 2>/dev/null || {
        print_warning "Could not install kernel headers (may not be needed)"
    }
    
    # Clone and build
    print_info "Cloning libgpiod v2.2.1 from kernel.org..."
    cd /tmp
    
    # Remove any existing build directory
    if [ -d "libgpiod-build" ]; then
        rm -rf libgpiod-build
    fi
    
    git clone --depth 1 --branch v2.2.1 \
        https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git \
        libgpiod-build || {
            print_error "Failed to clone libgpiod repository"
            exit 1
        }
    
    cd libgpiod-build
    
    print_info "Configuring libgpiod build..."
    ./autogen.sh --enable-tools=yes --prefix=/usr --disable-bindings-cxx --disable-bindings-python || {
        print_error "Failed to configure libgpiod"
        exit 1
    }
    
    print_info "Compiling libgpiod (this may take several minutes)..."
    make -j$(nproc) || {
        print_error "Failed to compile libgpiod"
        exit 1
    }
    
    print_info "Installing libgpiod..."
    make install || {
        print_error "Failed to install libgpiod"
        exit 1
    }
    
    ldconfig
    
    # Verify installation
    GPIOD_VERSION=$(pkg-config --modversion libgpiod 2>/dev/null || echo "unknown")
    if [[ "$GPIOD_VERSION" == "unknown" ]]; then
        print_error "libgpiod installation failed (pkg-config cannot find it)"
        exit 1
    fi
    
    print_success "libgpiod v$GPIOD_VERSION built and installed from source"
    
    # Cleanup
    cd /tmp
    rm -rf libgpiod-build
}

# Download PiPinPP
download_pipinpp() {
    print_header "Downloading PiPinPP $PIPINPP_VERSION"
    
    # Clean up any existing build directory
    if [ -d "$BUILD_DIR" ]; then
        print_info "Cleaning up previous build..."
        rm -rf "$BUILD_DIR"
    fi
    
    print_info "Cloning from GitHub..."
    git clone --depth 1 --branch "$PIPINPP_VERSION" "$REPO_URL" "$BUILD_DIR" || {
        print_error "Failed to download PiPinPP"
        exit 1
    }
    
    print_success "Downloaded PiPinPP $PIPINPP_VERSION"
}

# Build PiPinPP
build_pipinpp() {
    print_header "Building PiPinPP"
    
    cd "$BUILD_DIR"
    
    print_info "Configuring build..."
    cmake -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
        -DBUILD_TESTS=OFF \
        -DBUILD_EXAMPLES=ON \
        -DPIPINPP_ENABLE_LOGGING=OFF \
        || {
            print_error "CMake configuration failed"
            exit 1
        }
    
    print_info "Compiling (this may take a few minutes)..."
    cmake --build build -j$(nproc) || {
        print_error "Build failed"
        exit 1
    }
    
    print_success "Build completed"
}

# Install PiPinPP
install_pipinpp() {
    print_header "Installing PiPinPP"
    
    cd "$BUILD_DIR/build"
    
    print_info "Installing to $INSTALL_PREFIX..."
    cmake --install . || {
        print_error "Installation failed"
        exit 1
    }
    
    # Update library cache
    print_info "Updating library cache..."
    ldconfig
    
    print_success "Installation completed"
}

# Configure permissions
configure_permissions() {
    print_header "Configuring GPIO Permissions"
    
    # Check if gpio group exists
    if getent group gpio > /dev/null 2>&1; then
        print_success "gpio group exists"
    else
        print_info "Creating gpio group..."
        groupadd -r gpio
        print_success "gpio group created"
    fi
    
    # Add current user to gpio group (if not root)
    if [ -n "$SUDO_USER" ]; then
        print_info "Adding user '$SUDO_USER' to gpio group..."
        usermod -a -G gpio "$SUDO_USER"
        print_success "User added to gpio group"
        print_warning "User must log out and back in for group changes to take effect"
    fi
    
    # Create udev rules
    print_info "Creating udev rules for GPIO access..."
    cat > /etc/udev/rules.d/60-gpiochip.rules << 'EOF'
# GPIO chip character device permissions
# Allow members of the gpio group to access GPIO without sudo
SUBSYSTEM=="gpio", KERNEL=="gpiochip*", GROUP="gpio", MODE="0660"
EOF
    
    print_success "udev rules created"
    
    # Reload udev rules
    print_info "Reloading udev rules..."
    udevadm control --reload-rules
    udevadm trigger
    
    print_success "GPIO permissions configured"
}

# Verify installation
verify_installation() {
    print_header "Verifying Installation"
    
    # Check if library is installed
    if pkg-config --exists pipinpp; then
        VERSION_INSTALLED=$(pkg-config --modversion pipinpp)
        print_success "PiPinPP $VERSION_INSTALLED is installed"
    else
        print_error "pkg-config cannot find PiPinPP"
        return 1
    fi
    
    # Check if headers are present
    if [ -f "$INSTALL_PREFIX/include/pin.hpp" ]; then
        print_success "Headers are installed"
    else
        print_warning "Headers not found in $INSTALL_PREFIX/include/"
    fi
    
    # Check if library is present
    if [ -f "$INSTALL_PREFIX/lib/libpipinpp.a" ] || [ -f "$INSTALL_PREFIX/lib/libpipinpp.so" ]; then
        print_success "Library is installed"
    else
        print_warning "Library not found in $INSTALL_PREFIX/lib/"
    fi
    
    # Check if examples are present
    if [ -d "$INSTALL_PREFIX/share/pipinpp/examples" ]; then
        EXAMPLE_COUNT=$(ls -1 "$INSTALL_PREFIX/share/pipinpp/examples" 2>/dev/null | wc -l)
        print_success "Examples installed ($EXAMPLE_COUNT examples)"
    fi
    
    print_success "Installation verified"
}

# Cleanup
cleanup() {
    print_header "Cleaning Up"
    
    if [ -d "$BUILD_DIR" ]; then
        print_info "Removing build directory..."
        rm -rf "$BUILD_DIR"
        print_success "Cleanup completed"
    fi
}

# Print usage instructions
print_usage() {
    print_header "Installation Complete!"
    
    echo ""
    echo "PiPinPP has been installed successfully! 🎉"
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    echo "📚 Quick Start:"
    echo ""
    echo "  1. Create a new C++ file:"
    echo "     #include <pin.hpp>"
    echo "     int main() {"
    echo "         Pin led(17, PinDirection::OUTPUT);"
    echo "         led.write(true);"
    echo "         return 0;"
    echo "     }"
    echo ""
    echo "  2. Compile your program:"
    echo "     g++ main.cpp \$(pkg-config --cflags --libs pipinpp) -o main"
    echo ""
    echo "  3. Run it:"
    echo "     ./main"
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    echo "📖 Documentation:"
    echo "   https://github.com/Barbatos6669/PiPinPP"
    echo ""
    echo "💡 Examples:"
    echo "   $INSTALL_PREFIX/share/pipinpp/examples/"
    echo ""
    echo "🔧 CMake Usage:"
    echo "   find_package(PiPinPP REQUIRED)"
    echo "   target_link_libraries(your_app PiPinPP::pipinpp)"
    echo ""
    
    if [ -n "$SUDO_USER" ]; then
        echo "⚠️  IMPORTANT: User '$SUDO_USER' has been added to the 'gpio' group."
        echo "   You must log out and log back in for this to take effect!"
        echo "   Until then, you'll need to use 'sudo' to run GPIO programs."
        echo ""
    fi
    
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
}

# Main installation flow
main() {
    clear
    
    print_header "PiPinPP Installation Script"
    
    echo "This script will install PiPinPP $PIPINPP_VERSION"
    echo "Installation prefix: $INSTALL_PREFIX"
    echo ""
    
    # Run installation steps
    check_root
    detect_system
    install_dependencies
    download_pipinpp
    build_pipinpp
    install_pipinpp
    configure_permissions
    verify_installation
    cleanup
    print_usage
    
    print_success "Installation complete! Happy making! 🎉"
}

# Run main function
main "$@"
