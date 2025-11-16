cmake_minimum_required(VERSION 3.16)

# Project definition
project(PiPinPP
    VERSION 0.4.0
    DESCRIPTION "A modern C++ GPIO library for Raspberry Pi with an Arduino-inspired API"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Build options
option(BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)
option(BUILD_TESTS "Build test executables" ON)
option(BUILD_EXAMPLES "Build example programs" ON)
option(PIPINPP_ENABLE_LOGGING "Enable logging output for debugging" OFF)
option(PIPINPP_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

# Logging configuration
if(PIPINPP_ENABLE_LOGGING)
    add_compile_definitions(PIPINPP_ENABLE_LOGGING)
    if(DEFINED PIPINPP_LOG_LEVEL)
        add_compile_definitions(PIPINPP_LOG_LEVEL=${PIPINPP_LOG_LEVEL})
    endif()
    message(STATUS "Logging enabled (level: ${PIPINPP_LOG_LEVEL})")
endif()

# Compiler warnings (applied to all targets)
add_compile_options(
    -Wall          # Enable most warnings
    -Wextra        # Enable extra warnings
    -Wpedantic     # Warn about non-standard C++
)

# Optionally treat warnings as errors
if(PIPINPP_WARNINGS_AS_ERRORS)
    add_compile_options(-Werror)
    message(STATUS "Warnings treated as errors")
endif()

# Compiler-specific options
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(PIPINPP_SOURCES
    src/pin.cpp
    src/ArduinoCompat.cpp
    src/interrupts.cpp
    src/pwm.cpp
    src/event_pwm.cpp
    src/HardwarePWM.cpp
    src/Wire.cpp
    src/SPI.cpp
    src/Serial.cpp
)

# Create the library (STATIC or SHARED based on BUILD_SHARED_LIBS)
add_library(pipinpp ${PIPINPP_SOURCES})

# Create an alias for better namespacing
add_library(PiPinPP::pipinpp ALIAS pipinpp)

# Link libraries
target_link_libraries(pipinpp PUBLIC ${GPIOD_LIBRARIES})

# Include directories with proper generator expressions for build/install
target_include_directories(pipinpp PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${GPIOD_INCLUDE_DIRS}
)

# Set target properties
set_target_properties(pipinpp PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "include/pin.hpp;include/ArduinoCompat.hpp;include/log.hpp;include/exceptions.hpp;include/interrupts.hpp;include/pwm.hpp;include/event_pwm.hpp;include/HardwarePWM.hpp;include/Wire.hpp;include/SPI.hpp;include/Serial.hpp"
)

if(BUILD_TESTS)
    enable_testing()
    
    # Fetch GoogleTest
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    # Legacy test executable (keep for hardware-dependent testing)
    add_executable(test_pipinpp tests/test.cpp)
    target_link_libraries(test_pipinpp pipinpp)
    
    # GoogleTest-based tests
    add_executable(gtest_timing tests/gtest_timing.cpp)
    target_link_libraries(gtest_timing pipinpp GTest::gtest_main)
    
    add_executable(gtest_exceptions tests/gtest_exceptions.cpp)
    target_link_libraries(gtest_exceptions pipinpp GTest::gtest_main)
    
    add_executable(gtest_pin tests/gtest_pin.cpp)
    target_link_libraries(gtest_pin pipinpp GTest::gtest_main)
    
    add_executable(gtest_wire tests/gtest_wire.cpp)
    target_link_libraries(gtest_wire pipinpp GTest::gtest_main)
    
    add_executable(gtest_spi tests/gtest_spi.cpp)
    target_link_libraries(gtest_spi pipinpp GTest::gtest_main)
    
    add_executable(gtest_advanced_io tests/gtest_advanced_io.cpp)
    target_link_libraries(gtest_advanced_io pipinpp GTest::gtest_main)
    
    add_executable(gtest_serial tests/gtest_serial.cpp)
    target_link_libraries(gtest_serial pipinpp GTest::gtest_main)
    
    add_executable(gtest_hardware_pwm tests/gtest_hardware_pwm.cpp)
    target_link_libraries(gtest_hardware_pwm pipinpp GTest::gtest_main)
    
    # Add tests
    add_test(NAME basic_test COMMAND test_pipinpp)
    add_test(NAME gtest_timing COMMAND gtest_timing)
    add_test(NAME gtest_exceptions COMMAND gtest_exceptions)
    add_test(NAME gtest_pin COMMAND gtest_pin)
    add_test(NAME gtest_wire COMMAND gtest_wire)
    add_test(NAME gtest_spi COMMAND gtest_spi)
    add_test(NAME gtest_advanced_io COMMAND gtest_advanced_io)
    add_test(NAME gtest_serial COMMAND gtest_serial)
    add_test(NAME gtest_hardware_pwm COMMAND gtest_hardware_pwm)
    
    # Enable GoogleTest discovery (finds all TEST() macros automatically)
    include(GoogleTest)
    gtest_discover_tests(gtest_timing)
    gtest_discover_tests(gtest_exceptions)
    gtest_discover_tests(gtest_pin)
    gtest_discover_tests(gtest_wire)
    gtest_discover_tests(gtest_spi)
    gtest_discover_tests(gtest_advanced_io)
    gtest_discover_tests(gtest_serial)
    gtest_discover_tests(gtest_hardware_pwm)
endif()

if(BUILD_EXAMPLES)
    # Check if example directories contain CMakeLists.txt or source files
    file(GLOB EXAMPLE_DIRS "${CMAKE_SOURCE_DIR}/examples/*")
    
    foreach(EXAMPLE_DIR ${EXAMPLE_DIRS})
        if(IS_DIRECTORY ${EXAMPLE_DIR})
            get_filename_component(EXAMPLE_NAME ${EXAMPLE_DIR} NAME)
            
            # Look for main source files in each example directory
            file(GLOB EXAMPLE_SOURCES "${EXAMPLE_DIR}/*.cpp" "${EXAMPLE_DIR}/*.c")
            
            if(EXAMPLE_SOURCES)
                add_executable(example_${EXAMPLE_NAME} ${EXAMPLE_SOURCES})
                target_link_libraries(example_${EXAMPLE_NAME} pipinpp)
                
                # Set output directory for examples
                set_target_properties(example_${EXAMPLE_NAME} PROPERTIES
                    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
                )
            endif()
        endif()
    endforeach()
endif()

# Installation
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install library and export targets
install(TARGETS pipinpp
    EXPORT PiPinPPTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install header files
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)

# Export targets for use by other CMake projects
install(EXPORT PiPinPPTargets
    FILE PiPinPPTargets.cmake
    NAMESPACE PiPinPP::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PiPinPP
)

# Generate the config file that includes the exports
configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/PiPinPPConfig.cmake.in
    ${CMAKE_BINARY_DIR}/PiPinPPConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PiPinPP
)

# Generate the version file for the config file
write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/PiPinPPConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Install the config and version files
install(FILES
    ${CMAKE_BINARY_DIR}/PiPinPPConfig.cmake
    ${CMAKE_BINARY_DIR}/PiPinPPConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PiPinPP
)

# Create and install pkg-config file (for backwards compatibility)
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/pipinpp.pc.in
    ${CMAKE_BINARY_DIR}/pipinpp.pc
    @ONLY
)

install(FILES ${CMAKE_BINARY_DIR}/pipinpp.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# Print configuration summary
message(STATUS "")
message(STATUS "Configuration Summary:")
message(STATUS "  Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  Build examples: ${BUILD_EXAMPLES}")
message(STATUS "  Logging enabled: ${PIPINPP_ENABLE_LOGGING}")
message(STATUS "  Warnings as errors: ${PIPINPP_WARNINGS_AS_ERRORS}")
message(STATUS "  libgpiod found: ${GPIOD_FOUND}")
message(STATUS "")