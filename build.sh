#!/bin/bash
# Build script for PiPinPP

set -e

# Default values
BUILD_TYPE="Release"
BUILD_DIR="build"
CLEAN=false
INSTALL=false
TESTS_ONLY=false
EXAMPLES_ONLY=false
VERBOSE=false
BUILD_TESTS=true
BUILD_EXAMPLES=true

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
        --tests-only)
            TESTS_ONLY=true
            BUILD_EXAMPLES=false
            shift
            ;;
        --examples-only)
            EXAMPLES_ONLY=true
            BUILD_TESTS=false
            shift
            ;;
        --no-tests)
            BUILD_TESTS=false
            shift
            ;;
        --no-examples)
            BUILD_EXAMPLES=false
            shift
            ;;
        --verbose|-v)
            VERBOSE=true
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --debug          Build in debug mode (default: Release)"
            echo "  --clean          Clean build directory before building"
            echo "  --install        Install after building"
            echo "  --tests-only     Build only tests (skip examples)"
            echo "  --examples-only  Build only examples (skip tests)"
            echo "  --no-tests       Skip building tests"
            echo "  --no-examples    Skip building examples"
            echo "  --verbose, -v    Show detailed build output"
            echo "  --help, -h       Show this help message"
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
echo "==> Checking dependencies..."
if ! command -v pkg-config &> /dev/null; then
    echo "Error: pkg-config not found. Please install it:"
    echo "  sudo apt install pkg-config"
    exit 1
fi

if ! pkg-config --exists libgpiod; then
    echo "Error: libgpiod not found. Please install libgpiod development package:"
    echo "  For Ubuntu/Debian: sudo apt install libgpiod-dev"
    echo "  Or use the install script: ./install.sh"
    exit 1
fi

echo "✓ libgpiod $(pkg-config --modversion libgpiod) found"

# Clean if requested
if [[ "$CLEAN" == true ]]; then
    echo "==> Cleaning build directory..."
    if [[ "$VERBOSE" == true ]]; then
        rm -rfv "$BUILD_DIR"
    else
        rm -rf "$BUILD_DIR"
    fi
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
echo "==> Configuring build ($BUILD_TYPE)..."
CMAKE_OPTS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
CMAKE_OPTS="$CMAKE_OPTS -DBUILD_TESTS=$BUILD_TESTS"
CMAKE_OPTS="$CMAKE_OPTS -DBUILD_EXAMPLES=$BUILD_EXAMPLES"

if [[ "$VERBOSE" == true ]]; then
    CMAKE_OPTS="$CMAKE_OPTS -DCMAKE_VERBOSE_MAKEFILE=ON"
    echo "Build options: $CMAKE_OPTS"
fi

cmake $CMAKE_OPTS ..

# Build
JOBS=$(nproc)
if [[ "$VERBOSE" == true ]]; then
    echo "==> Building with $JOBS parallel jobs..."
    MAKEFLAGS="VERBOSE=1" make -j$JOBS
else
    echo "==> Building with $JOBS parallel jobs..."
    make -j$JOBS
fi

if [[ $? -eq 0 ]]; then
    echo "✓ Build completed successfully!"
else
    echo "✗ Build failed!"
    exit 1
fi

# Run tests
if [[ "$BUILD_TESTS" == true ]]; then
    echo "==> Running tests..."
    if [[ "$VERBOSE" == true ]]; then
        ctest --output-on-failure --verbose
    else
        ctest --output-on-failure
    fi
    
    if [[ $? -eq 0 ]]; then
        echo "✓ All tests passed!"
    else
        echo "✗ Some tests failed!"
        exit 1
    fi
else
    echo "==> Skipping tests (disabled)"
fi

# Install if requested
if [[ "$INSTALL" == true ]]; then
    echo "==> Installing..."
    if sudo make install; then
        echo "✓ Installation completed!"
        echo "Run 'ldconfig' if needed: sudo ldconfig"
    else
        echo "✗ Installation failed!"
        exit 1
    fi
fi

echo ""
echo "==> Build summary:"
echo "  Build type:    $BUILD_TYPE"
echo "  Tests built:   $BUILD_TESTS"
echo "  Examples built: $BUILD_EXAMPLES"
if [[ "$INSTALL" == true ]]; then
    echo "  Installed:     Yes"
fi
echo ""
echo "✓ Done!"