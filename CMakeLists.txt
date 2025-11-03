cmake_minimum_required(VERSION 3.16)

# Project definition
project(PiPinPP
    VERSION 0.2.0
    DESCRIPTION "A modern C++ GPIO library for Raspberry Pi with Arduino compatibility"
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
)

# Create the library
add_library(pipinpp STATIC ${PIPINPP_SOURCES})

# Link libraries
target_link_libraries(pipinpp ${GPIOD_LIBRARIES})

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
    PUBLIC_HEADER "include/pin.hpp;include/ArduinoCompat.hpp;include/log.hpp;include/exceptions.hpp"
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
    
    # Add tests
    add_test(NAME basic_test COMMAND test_pipinpp)
    add_test(NAME gtest_timing COMMAND gtest_timing)
    add_test(NAME gtest_exceptions COMMAND gtest_exceptions)
    add_test(NAME gtest_pin COMMAND gtest_pin)
    
    # Enable GoogleTest discovery (finds all TEST() macros automatically)
    include(GoogleTest)
    gtest_discover_tests(gtest_timing)
    gtest_discover_tests(gtest_exceptions)
    gtest_discover_tests(gtest_pin)
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

# Install library
install(TARGETS pipinpp
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install header files
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)

# Create and install pkg-config file
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