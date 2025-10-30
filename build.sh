#!/bin/bash
# Build script for PiPinPP

set -e

# Default values
BUILD_TYPE="Release"
BUILD_DIR="build"
CLEAN=false
INSTALL=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --install)
            INSTALL=true
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --debug     Build in debug mode (default: Release)"
            echo "  --clean     Clean build directory before building"
            echo "  --install   Install after building"
            echo "  --help      Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Check for required dependencies
echo "Checking dependencies..."
if ! pkg-config --exists libgpiod; then
    echo "Error: libgpiod not found. Please install libgpiod development package:"
    echo "  sudo apt install libgpiod-dev"
    exit 1
fi

# Clean if requested
if [[ "$CLEAN" == true ]]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
echo "Configuring build ($BUILD_TYPE)..."
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ..

# Build
echo "Building..."
make -j$(nproc)

echo "Build completed successfully!"

# Run tests
echo "Running tests..."
ctest --output-on-failure

# Install if requested
if [[ "$INSTALL" == true ]]; then
    echo "Installing..."
    sudo make install
    echo "Installation completed!"
fi

echo "Done!"