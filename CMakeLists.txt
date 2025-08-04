# ==============================================================================
# GATE (Ganesha Algorithmic Transpiler Engine) CMake Configuration
# ==============================================================================
#
# This file provides a modern CMake build system for the GATE project,
# offering a cross-platform alternative to the manual Makefile.
#
# OVERVIEW:
#   GATE is a transpiler that converts Notal language to Pascal code.
#   This CMake configuration handles:
#   - Core library compilation (gate_lib)
#   - Main executable creation (gate)
#   - Unit testing framework integration (gate_tests)
#   - External dependency management (cxxopts, googletest)
#   - Cross-platform build support
#
# USAGE:
#   mkdir build && cd build
#   cmake ..
#   cmake --build .
#   ctest (for running tests)
#
# TARGETS:
#   gate       - Main transpiler executable
#   gate_lib   - Core static library
#   gate_tests - Unit test executable (if GATE_BUILD_TESTS=ON)
#
# OPTIONS:
#   GATE_BUILD_TESTS     - Enable/disable test compilation (default: ON)
#   GATE_ENABLE_WARNINGS - Enable strong compiler warnings (default: ON)
#
# DEPENDENCIES:
#   - cxxopts: Command-line argument parsing
#   - googletest: Unit testing framework (if tests enabled)
#   - Threads: Multi-threading support
#
# DIRECTORY STRUCTURE:
#   src/        - Source code files
#   include/    - Header files
#   tests/      - Unit test files
#   vendor/     - External dependencies (submodules)
#   bin/        - Output executables
#   build/lib/  - Output libraries
# ==============================================================================

cmake_minimum_required(VERSION 3.16)
project(GATE VERSION 1.0.0 LANGUAGES CXX)

# --- Set C++ Standard ---
# Configure C++17 as the minimum standard for modern language features
# CMAKE_CXX_STANDARD_REQUIRED ensures the build fails if C++17 is not available
# CMAKE_CXX_EXTENSIONS OFF prevents compiler-specific extensions for portability
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Set Output Directories ---
# Organize build artifacts in structured directories:
# - Static libraries (.a/.lib) go to build/lib/
# - Shared libraries (.so/.dll) go to build/lib/
# - Executables are placed in project root bin/ directory (configured per target)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# --- Project Options ---
# GATE_BUILD_TESTS: Controls whether unit tests are compiled and linked
#   - ON (default): Includes GoogleTest framework and builds gate_tests executable
#   - OFF: Skips test compilation for faster builds in production
# GATE_ENABLE_WARNINGS: Controls compiler warning levels
#   - ON (default): Enables -Wall -Wextra -pedantic for better code quality
#   - OFF: Uses minimal warnings for compatibility with legacy code
option(GATE_BUILD_TESTS "Build the testing suite" ON)
option(GATE_ENABLE_WARNINGS "Enable strong compiler warnings" ON)

# --- Add Submodules/Dependencies ---
# External dependencies are managed as Git submodules in vendor/ directory
# cxxopts: Modern C++ command-line argument parsing library
#   - Provides type-safe argument parsing with automatic help generation
#   - Used in main.cpp for handling transpiler command-line options
#   - Repository: https://github.com/jarro2783/cxxopts
add_subdirectory(vendor/cxxopts)

# --- Source File Discovery ---
# Automatically discover all source files for the core library
# Excludes main.cpp to allow separate executable compilation
# Source organization:
#   - src/core/: Core transpiler components (Lexer, Parser, Code Generator, Token, etc.)
#   - src/ast/: Abstract Syntax Tree nodes and visitors
#   - src/diagnostics/: Error handling and diagnostic reporting
file(GLOB_RECURSE GATE_LIB_SOURCES
    "src/core/*.cpp"
    "src/ast/*.cpp"
    "src/diagnostics/*.cpp"
)

# --- Core Library Target ---
# Define the main GATE library containing all core functionality

# Create the core library as a static library
# Benefits of static linking:
# - Simpler deployment (single executable)
# - Better optimization opportunities
# - No runtime library dependencies
# - Faster startup time (no dynamic loading)
# - Self-contained distribution
add_library(gate_lib STATIC
    ${GATE_LIB_SOURCES}
)

# Set include directories for the library
# PUBLIC: These directories are used both when building gate_lib
#         and when other targets link against gate_lib
# Propagates include paths to dependent targets automatically
# Include paths:
# - include/: Project headers (AST, Parser, CodeGenerator, etc.)
target_include_directories(gate_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# --- Compiler Warning Configuration ---
# Enable comprehensive warnings when GATE_ENABLE_WARNINGS is ON
# Helps catch potential bugs and enforce coding standards

# Enable compiler warnings for better code quality
# Conditional compilation based on compiler type:
# - MSVC: Use /W4 (high warning level)
# - GCC/Clang: Use comprehensive warning flags
if(GATE_ENABLE_WARNINGS)
    if(MSVC)
        # MSVC warning level 4 (comprehensive warnings)
        target_compile_options(gate_lib PRIVATE /W4)
    else()
        # GCC/Clang comprehensive warning set:
        # -Wall: Enable most common warnings
        # -Wextra: Enable extra warnings not covered by -Wall
        # -pedantic: Enforce strict ISO C++ compliance
        # -g: Include debug information
        target_compile_options(gate_lib PRIVATE -Wall -Wextra -pedantic -g)
    endif()
endif()

# --- Main Executable Target ---
# Create the primary GATE transpiler executable

# Create the main executable
# Links main.cpp with the core library to create final transpiler
# Entry point: main() function in src/main.cpp
add_executable(gate src/main.cpp)
set_target_properties(gate PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/bin"
)

# --- Linking for Main Executable ---
# Link the executable with the core library and dependencies
# PRIVATE: Dependencies are implementation details of the gate executable
# Automatically inherits include directories from gate_lib (PUBLIC)

# Enable threading support (required for some standard library features)
# Threading support needed for:
# - Standard library thread-safe operations
# - Potential future parallel processing features
# - GoogleTest framework (in testing)
find_package(Threads REQUIRED)
target_link_libraries(gate PRIVATE
    gate_lib
    cxxopts::cxxopts
    Threads::Threads
)

# --- Testing Configuration ---
# Comprehensive unit testing setup using GoogleTest framework
# Only enabled when GATE_BUILD_TESTS option is ON

# Testing
if(GATE_BUILD_TESTS)
    # Enable CTest for running tests
    # CTest provides:
    # - Test discovery and execution
    # - Test result reporting
    # - Integration with IDEs and CI/CD systems
    # Usage: cmake --build . --target test
    enable_testing()

    # Add GoogleTest
    # Builds GoogleTest framework from source (vendor/googletest/)
    # Provides gtest, gtest_main, gmock, and gmock_main targets
    # Source build ensures compatibility and version control
    add_subdirectory(vendor/googletest)

    # Collect test source files
    # Recursively finds all .cpp files in tests/ subdirectories
    # Automatically includes new test files without CMake modification
    # Test organization:
    #   - tests/components/: Tests for transpiler components (Lexer, Parser, CodeGen)
    #   - tests/features/: Tests for NOTAL language features (Arrays, Loops, Functions, etc.)
    #   - tests/helpers/: Shared test utilities and helper functions
    file(GLOB_RECURSE TEST_SOURCES
        "tests/components/*.cpp"
        "tests/features/*.cpp"
        "tests/helpers/*.cpp"
    )

    # Create test executable
    # Combines all test source files into single test runner
    # Each test file can contain multiple TEST() and TEST_F() cases
    add_executable(gate_tests
        ${TEST_SOURCES}
    )
    set_target_properties(gate_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/bin"
)

    # Link test executable with the core library and GoogleTest
    # Dependencies:
    # - gate_lib: Access to all project functionality for testing
    # - gtest_main: Provides main() function for test runner
    # - Threads::Threads: Required by GoogleTest for thread safety
    target_link_libraries(gate_tests PRIVATE
        gate_lib
        gtest_main # Provided by add_subdirectory(googletest)
        Threads::Threads
    )

    # Add the test to CTest
    # Registers gate_tests executable with CTest framework
    # Enables: ctest, make test, cmake --build . --target test
    # Test name "unit_tests" appears in test reports
    add_test(NAME unit_tests COMMAND gate_tests)
endif()

# --- Build Status and Usage Information ---
# Display comprehensive build configuration and usage instructions

# Status messages
# Provide clear feedback about build configuration
# Helps developers verify settings and troubleshoot issues
message(STATUS "GATE project configured successfully.")
message(STATUS "  - Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  - C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  - Build tests: ${GATE_BUILD_TESTS}")
message(STATUS "  - Enable warnings: ${GATE_ENABLE_WARNINGS}")
message(STATUS "  - Output directory: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

# Testing usage instructions
# Display when tests are enabled to guide developers
# Provides multiple ways to run tests for different workflows
if(GATE_BUILD_TESTS)
    message(STATUS "")
    message(STATUS "To run tests after building:")
    message(STATUS "  make test          # Build and run via Make")
    message(STATUS "  ctest              # Run via CTest framework")
    message(STATUS "  cmake --build . --target test  # Cross-platform")
    message(STATUS "  ./bin/gate_tests   # Direct execution")
endif()

# Final build instructions
message(STATUS "")
message(STATUS "To build the project:")
message(STATUS "  cmake --build .    # Cross-platform build")
message(STATUS "  make               # Unix-style build")
message(STATUS "")
message(STATUS "Binaries are in: ${PROJECT_SOURCE_DIR}/bin")