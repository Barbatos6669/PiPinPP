#!/bin/bash
#
# PiPinPP Code Coverage Script
# 
# Generates code coverage reports using gcov/lcov.
# Requires: lcov, genhtml
#
# Usage:
#   ./coverage.sh           # Generate coverage report
#   ./coverage.sh --clean   # Clean coverage data
#   ./coverage.sh --view    # Generate and open report in browser
#

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Check if lcov is installed
check_dependencies() {
    if ! command -v lcov &> /dev/null; then
        echo -e "${RED}❌ Error: lcov not found${NC}"
        echo "Install with: sudo apt install lcov"
        exit 1
    fi
    
    if ! command -v genhtml &> /dev/null; then
        echo -e "${RED}❌ Error: genhtml not found${NC}"
        echo "Install with: sudo apt install lcov"
        exit 1
    fi
}

# Clean coverage data
clean_coverage() {
    echo -e "${BLUE}🧹 Cleaning coverage data...${NC}"
    
    if [ -d "${BUILD_DIR}" ]; then
        cd "${BUILD_DIR}"
        
        # Remove coverage files
        find . -name "*.gcda" -delete
        find . -name "*.gcno" -delete
        rm -f coverage.info coverage.info.cleaned
        rm -rf coverage_html
        
        echo -e "${GREEN}✅ Coverage data cleaned${NC}"
    else
        echo -e "${YELLOW}⚠️  Build directory not found${NC}"
    fi
}

# Generate coverage report
generate_coverage() {
    echo -e "${BLUE}📊 Generating code coverage report...${NC}"
    echo ""
    
    # Check if project is built with coverage enabled
    if [ ! -d "${BUILD_DIR}" ]; then
        echo -e "${RED}❌ Error: Build directory not found${NC}"
        echo "Build with coverage enabled first:"
        echo "  cmake -DPIPINPP_ENABLE_COVERAGE=ON -B build"
        echo "  cmake --build build"
        exit 1
    fi
    
    cd "${BUILD_DIR}"
    
    # Check if coverage instrumentation exists
    if ! find . -name "*.gcno" | grep -q .; then
        echo -e "${RED}❌ Error: No coverage instrumentation found${NC}"
        echo "Rebuild with coverage enabled:"
        echo "  cmake -DPIPINPP_ENABLE_COVERAGE=ON -B build"
        echo "  cmake --build build"
        exit 1
    fi
    
    # Run tests
    echo -e "${BLUE}🧪 Running tests to collect coverage data...${NC}"
    ctest --output-on-failure || true  # Continue even if some tests fail
    echo ""
    
    # Capture coverage data
    echo -e "${BLUE}📸 Capturing coverage data...${NC}"
    lcov --directory . --capture --output-file coverage.info --ignore-errors inconsistent --quiet
    
    # Filter out unwanted files
    echo -e "${BLUE}🔍 Filtering coverage data...${NC}"
    lcov --remove coverage.info \
        '/usr/*' \
        '*/tests/*' \
        '*/examples/*' \
        '*/build/*' \
        '*/_deps/*' \
        '*/googletest/*' \
        --output-file coverage.info.cleaned --quiet \
        --ignore-errors unused
    
    # Generate HTML report
    echo -e "${BLUE}📄 Generating HTML report...${NC}"
    genhtml coverage.info.cleaned \
        --output-directory coverage_html \
        --title "PiPinPP Code Coverage" \
        --legend \
        --quiet
    
    echo ""
    echo -e "${GREEN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${GREEN}✅ Coverage Report Generated${NC}"
    echo -e "${GREEN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
    
    # Print summary
    echo -e "${BLUE}Coverage Summary:${NC}"
    lcov --list coverage.info.cleaned | tail -n 3
    echo ""
    
    # Calculate coverage percentage
    COVERAGE=$(lcov --summary coverage.info.cleaned 2>&1 | grep lines | awk '{print $2}' | sed 's/%//')
    
    if (( $(echo "$COVERAGE >= 80" | bc -l) )); then
        echo -e "${GREEN}✅ Coverage: ${COVERAGE}% (Target: ≥80%)${NC}"
    elif (( $(echo "$COVERAGE >= 60" | bc -l) )); then
        echo -e "${YELLOW}⚠️  Coverage: ${COVERAGE}% (Target: ≥80%)${NC}"
    else
        echo -e "${RED}❌ Coverage: ${COVERAGE}% (Target: ≥80%)${NC}"
    fi
    
    echo ""
    echo -e "${BLUE}📂 Report location:${NC} ${BUILD_DIR}/coverage_html/index.html"
    echo ""
    
    # Suggest how to view
    echo -e "${BLUE}💡 To view the report:${NC}"
    echo "  • Open: ${BUILD_DIR}/coverage_html/index.html"
    echo "  • Or run: ./coverage.sh --view"
    echo ""
}

# Open coverage report in browser
view_coverage() {
    REPORT="${BUILD_DIR}/coverage_html/index.html"
    
    if [ ! -f "${REPORT}" ]; then
        echo -e "${YELLOW}⚠️  Coverage report not found. Generating...${NC}"
        generate_coverage
    fi
    
    echo -e "${BLUE}🌐 Opening coverage report in browser...${NC}"
    
    # Try different browsers
    if command -v xdg-open &> /dev/null; then
        xdg-open "${REPORT}"
    elif command -v firefox &> /dev/null; then
        firefox "${REPORT}" &
    elif command -v chromium-browser &> /dev/null; then
        chromium-browser "${REPORT}" &
    elif command -v google-chrome &> /dev/null; then
        google-chrome "${REPORT}" &
    else
        echo -e "${YELLOW}⚠️  No browser found. Open manually:${NC}"
        echo "  ${REPORT}"
    fi
}

# Main
main() {
    case "${1:-}" in
        --clean)
            check_dependencies
            clean_coverage
            ;;
        --view)
            check_dependencies
            view_coverage
            ;;
        --help|-h)
            echo "PiPinPP Code Coverage Script"
            echo ""
            echo "Usage:"
            echo "  ./coverage.sh           Generate coverage report"
            echo "  ./coverage.sh --clean   Clean coverage data"
            echo "  ./coverage.sh --view    Generate and open report in browser"
            echo "  ./coverage.sh --help    Show this help"
            echo ""
            echo "Prerequisites:"
            echo "  1. Build with coverage enabled:"
            echo "     cmake -DPIPINPP_ENABLE_COVERAGE=ON -B build"
            echo "     cmake --build build"
            echo ""
            echo "  2. Install lcov:"
            echo "     sudo apt install lcov"
            ;;
        "")
            check_dependencies
            generate_coverage
            ;;
        *)
            echo -e "${RED}❌ Unknown option: $1${NC}"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
}

main "$@"
