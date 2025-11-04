cmake_minimum_required(VERSION 3.10)

project(robot_stress_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if we're building as part of main project or standalone
if(NOT TARGET pipinpp)
    # Standalone build - find installed library
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PIPINPP REQUIRED pipinpp)
    
    add_executable(robot_stress_test main.cpp)
    target_include_directories(robot_stress_test PRIVATE ${PIPINPP_INCLUDE_DIRS})
    target_link_libraries(robot_stress_test ${PIPINPP_LIBRARIES})
else()
    # Building as part of main project
    add_executable(robot_stress_test main.cpp)
    target_link_libraries(robot_stress_test pipinpp)
endif()

# Find threads
find_package(Threads REQUIRED)
target_link_libraries(robot_stress_test Threads::Threads)

# Enable warnings
target_compile_options(robot_stress_test PRIVATE -Wall -Wextra -Wpedantic)

message(STATUS "Robot Stress Test configured successfully")
