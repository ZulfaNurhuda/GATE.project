# GATE (Ganesha Algorithmic Transpiler Engine) CMake Configuration
#
# This file provides a modern CMake build system for the GATE project,
# replacing the need for the manual Makefile.

cmake_minimum_required(VERSION 3.16)
project(GATE VERSION 1.0.0 LANGUAGES CXX)

# --- Set C++ Standard ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Set Output Directories ---
# Place libraries in the build 'lib' directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# --- Project Options ---
option(GATE_BUILD_TESTS "Build the testing suite" ON)
option(GATE_ENABLE_WARNINGS "Enable strong compiler warnings" ON)

# --- Add Submodules/Dependencies ---
# cxxopts for command-line parsing
add_subdirectory(vendor/cxxopts)

# --- Source File Discovery ---
# Gather all source files for the core library (excluding main)
file(GLOB_RECURSE GATE_LIB_SOURCES
    "src/transpiler/*.cpp"
    "src/ast/*.cpp"
    "src/core/*.cpp"
    "src/utils/*.cpp"
)

# --- Core Library Target ---
# Define the main static library for the transpiler engine
add_library(gate_lib STATIC
    ${GATE_LIB_SOURCES}
)

# --- Include Directories for gate_lib ---
# The library itself needs access to its own headers
target_include_directories(gate_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# --- Compiler Warnings ---
if(GATE_ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(gate_lib PRIVATE /W4)
    else()
        target_compile_options(gate_lib PRIVATE -Wall -Wextra -pedantic -g)
    endif()
endif()

# --- Main Executable Target ---
# Define the main 'gate' command-line application
add_executable(gate src/main.cpp)
set_target_properties(gate PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/bin"
)

# --- Linking for Main Executable ---
# Link gate against the core library and dependencies
find_package(Threads REQUIRED)
target_link_libraries(gate PRIVATE
    gate_lib
    cxxopts::cxxopts
    Threads::Threads
)

# --- Testing ---
if(GATE_BUILD_TESTS)
    # Enable the CTest framework
    enable_testing()

    # Add GoogleTest from the vendor directory
    add_subdirectory(vendor/googletest)

    # Gather all test source files
    file(GLOB_RECURSE TEST_SOURCES
        "tests/unit/*.cpp"
        "tests/helpers/*.cpp"
    )

    # Define the test executable
    add_executable(gate_tests
        ${TEST_SOURCES}
    )
    set_target_properties(gate_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/bin"
)

    # Link the test executable against our library and GoogleTest
    target_link_libraries(gate_tests PRIVATE
        gate_lib
        gtest_main # Provided by add_subdirectory(googletest)
        Threads::Threads
    )

    # Add the test to CTest for execution via 'ctest' command
    add_test(NAME unit_tests COMMAND gate_tests)
endif()

# --- Final Message ---
message(STATUS "GATE project configured successfully.")
message(STATUS "  - To build:       cmake --build .")
message(STATUS "  - To run tests:   ctest (after building)")
message(STATUS "  - Binaries are in: ${PROJECT_SOURCE_DIR}/bin")