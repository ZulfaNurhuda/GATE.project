# ==============================================================================
# GATE (Ganesha Algorithmic Transpiler Engine) Makefile
# ==============================================================================
#
# This is a standalone Makefile for building the GATE project without CMake.
# Provides a traditional Unix-style build system for developers who prefer
# direct control over compilation flags and dependencies.
#
# OVERVIEW:
#   GATE is a transpiler that converts Notal language to Pascal code.
#   This Makefile handles:
#   - Core library compilation (gate_lib objects)
#   - Main executable creation (gate)
#   - Unit testing with GoogleTest integration (gate_tests)
#   - Dependency management for external libraries
#   - Cross-platform compatibility (Linux, macOS, Windows with MinGW)
#
# USAGE:
#   make           - Build main executable (default target)
#   make all       - Same as above
#   make test      - Build and run unit tests
#   make clean     - Remove all build artifacts
#   make help      - Display help information
#
# TARGETS:
#   all       - Build the main 'gate' executable (default)
#   test      - Build and run the 'gate_tests' executable
#   clean     - Remove all build artifacts (build/ and bin/ directories)
#   help      - Display this help message
#
# REQUIREMENTS:
#   - g++ compiler with C++17 support
#   - Git submodules initialized (cxxopts, googletest)
#   - POSIX-compatible shell (for Windows: MinGW/MSYS2)
#
# DIRECTORY STRUCTURE:
#   src/        - Source code files
#   include/    - Header files
#   tests/      - Unit test files
#   vendor/     - External dependencies (Git submodules)
#   build/      - Intermediate build files
#   bin/        - Output executables
# ==============================================================================

# --- Variables ---
# Build configuration variables that can be overridden from command line
# Example: make CXX=clang++ CXXFLAGS="-std=c++17 -O3"

# Compiler and flags
# CXX: C++ compiler (default: g++, can use clang++, etc.)
# CXXFLAGS: Compilation flags
#   -std=c++17: Enable C++17 standard features
#   -Wall -Wextra: Enable comprehensive warnings
#   -g: Include debug information for debugging
# LDFLAGS: Linker flags
#   -pthread: Enable POSIX threads support (required by GoogleTest)
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
LDFLAGS = -pthread

# Directories
# Project structure organization
# SRC_DIR: Main source code location
# INCLUDE_DIR: Header files location
# VENDOR_DIR: External dependencies (Git submodules)
# BUILD_DIR: Intermediate build artifacts
# OBJ_DIR: Compiled object files for main library
# BIN_DIR: Final executable output
SRC_DIR = src
INCLUDE_DIR = include
VENDOR_DIR = vendor
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = bin

# Test directories
# TEST_SRC_DIR: Unit test source files
# TEST_BUILD_DIR: Test-specific build artifacts
# TEST_OBJ_DIR: Compiled test object files
TEST_SRC_DIR = tests
TEST_BUILD_DIR = $(BUILD_DIR)/test
TEST_OBJ_DIR = $(TEST_BUILD_DIR)/obj

# Include paths for our project
# INC_PATHS: Header search paths for main compilation
#   - include/: Project header files (gate/*.h)
#   - vendor/cxxopts/include/: Command-line parsing library headers
INC_PATHS = -I$(INCLUDE_DIR) \
            -I$(VENDOR_DIR)/cxxopts/include

# Include paths for GoogleTest compilation
# GTEST_INC_PATHS: Additional headers needed for test compilation
#   - googletest/include/: Public GoogleTest API headers
#   - googlemock/include/: Public GoogleMock API headers
#   - googletest/: Internal GoogleTest implementation headers
#   - googlemock/: Internal GoogleMock implementation headers
GTEST_INC_PATHS = -I$(VENDOR_DIR)/googletest/googletest/include \
                  -I$(VENDOR_DIR)/googletest/googlemock/include \
                  -I$(VENDOR_DIR)/googletest/googletest \
                  -I$(VENDOR_DIR)/googletest/googlemock

# --- Source Files ---
# Automatic source file discovery using wildcard patterns
# This approach automatically includes new .cpp files without manual updates

# Find all .cpp files in the library source directories
# GATE_LIB_SRCS: Core library source files (excludes main.cpp)
#   - transpiler/: NotalLexer.cpp, NotalParser.cpp, PascalCodeGenerator.cpp
#   - ast/: ASTPrinter.cpp and other AST-related implementations
#   - core/: Token.cpp and other core language constructs
#   - utils/: ErrorHandler and utility class implementations
GATE_LIB_SRCS = $(wildcard $(SRC_DIR)/transpiler/*.cpp) \
                $(wildcard $(SRC_DIR)/ast/*.cpp) \
                $(wildcard $(SRC_DIR)/core/*.cpp) \
                $(wildcard $(SRC_DIR)/utils/*.cpp) \
                $(wildcard $(SRC_DIR)/diagnostics/*.cpp)

# Main application source
# GATE_MAIN_SRC: Entry point for the transpiler executable
# Contains command-line interface and main program logic
GATE_MAIN_SRC = $(SRC_DIR)/main.cpp

# Test source files
# TEST_SRCS: Unit test implementations
#   - unit/: Individual test files for different components
#   - helpers/: Test utility functions and common test infrastructure
# Note: test_main.cpp is excluded because we use gtest_main.cc
TEST_SRCS = $(wildcard $(TEST_SRC_DIR)/unit/*.cpp) \
            $(wildcard $(TEST_SRC_DIR)/helpers/*.cpp)

# GoogleTest source files (compiled from source for better control)
# GTEST_SRCS: GoogleTest framework implementation
#   - gtest-all.cc: Complete GoogleTest implementation
#   - gmock-all.cc: Complete GoogleMock implementation
#   - gtest_main.cc: Default main() function for test executables
GTEST_SRCS = $(VENDOR_DIR)/googletest/googletest/src/gtest-all.cc \
             $(VENDOR_DIR)/googletest/googlemock/src/gmock-all.cc \
             $(VENDOR_DIR)/googletest/googletest/src/gtest_main.cc

# --- Object Files ---
# Transform source file paths to corresponding object file paths
# Uses patsubst function to maintain directory structure in build/

# Generate object file paths from source file paths
# GATE_LIB_OBJS: Object files for core library
#   Example: src/transpiler/NotalLexer.cpp -> build/obj/transpiler/NotalLexer.o
GATE_LIB_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(GATE_LIB_SRCS))

# GATE_MAIN_OBJ: Object file for main executable
#   Example: src/main.cpp -> build/obj/main.o
GATE_MAIN_OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(GATE_MAIN_SRC))

# TEST_OBJS: Object files for unit tests
#   Example: tests/unit/lexer_test.cpp -> build/test/obj/unit/lexer_test.o
TEST_OBJS = $(patsubst $(TEST_SRC_DIR)/%.cpp, $(TEST_OBJ_DIR)/%.o, $(TEST_SRCS))

# GTEST_OBJS: Object files for GoogleTest framework
#   Example: vendor/googletest/googletest/src/gtest-all.cc -> build/test/obj/gtest/googletest/src/gtest-all.o
GTEST_OBJS = $(patsubst $(VENDOR_DIR)/googletest/%.cc, $(TEST_OBJ_DIR)/gtest/%.o, $(GTEST_SRCS))

# --- Targets ---
# Define build targets and their dependencies
# This section contains the main build rules and target definitions

# Executable targets
# TARGET: Main transpiler executable path
# TEST_TARGET: Unit test executable path
TARGET = $(BIN_DIR)/gate
TEST_TARGET = $(BIN_DIR)/gate_tests

# Phony targets (targets that don't represent files)
# .PHONY prevents make from looking for files with these names
# Essential for targets like 'clean', 'help', 'test' that don't create files
.PHONY: all test clean help

# Default target
# 'all' is the default target when running 'make' without arguments
# Builds the main executable by depending on $(TARGET)
all: $(TARGET)

# Build the main executable
# $(TARGET): Links all library objects and main.o into final executable
# Dependencies: All core library objects + main object file
# Process:
#   1. Create output directory if it doesn't exist
#   2. Link all object files with C++ linker
#   3. Apply linker flags (pthread support)
#   4. Output final executable to bin/gate
$(TARGET): $(GATE_LIB_OBJS) $(GATE_MAIN_OBJ)
	@echo "Linking main executable: $@"
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Build complete. Executable at: $@"

# Build and run tests
# test: Convenience target that builds and immediately runs unit tests
# Dependencies: Test executable must be built first
# Process:
#   1. Build test executable (triggers $(TEST_TARGET) rule)
#   2. Execute the test binary to run all unit tests
#   3. GoogleTest framework handles test discovery and reporting
test: $(TEST_TARGET)
	@echo "Running tests..."
	@./$(TEST_TARGET)

# Build the test executable
# $(TEST_TARGET): Links test objects, library objects, and GoogleTest into test executable
# Dependencies: Test objects + Core library objects + GoogleTest framework objects
# Process:
#   1. Create output directory if needed
#   2. Link all test-related object files
#   3. Include GoogleTest framework (gtest_main provides main() function)
#   4. Apply threading support for GoogleTest
$(TEST_TARGET): $(TEST_OBJS) $(GATE_LIB_OBJS) $(GTEST_OBJS)
	@echo "Linking test executable: $@"
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Test build complete. Executable at: $@"

# --- Compilation Rules ---
# Pattern rules for compiling different types of source files
# These rules define how to transform .cpp/.cc files into .o object files

# Rule to compile library source files into object files
# Pattern: src/*/%.cpp -> build/obj/*/%.o
# Process:
#   1. Create directory structure in build/obj/ matching src/
#   2. Compile with project include paths
#   3. Generate object file with debug information
# Includes: Project headers and cxxopts headers
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC_PATHS) -c $< -o $@

# Rule to compile test source files into object files
# Pattern: tests/*/%.cpp -> build/test/obj/*/%.o
# Process:
#   1. Create directory structure in build/test/obj/ matching tests/
#   2. Compile with both project and GoogleTest include paths
#   3. Enable access to both main project and test framework headers
# Includes: Project headers + GoogleTest/GoogleMock headers
$(TEST_OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.cpp
	@echo "Compiling test file $<"
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC_PATHS) $(GTEST_INC_PATHS) -c $< -o $@

# Rule to compile GoogleTest source files
# Pattern: vendor/googletest/*/%.cc -> build/test/obj/gtest/*/%.o
# Process:
#   1. Create GoogleTest-specific directory structure
#   2. Compile GoogleTest framework source files
#   3. Use only GoogleTest include paths (no project headers needed)
# Note: Compiles GoogleTest from source for better control and compatibility
$(TEST_OBJ_DIR)/gtest/%.o: $(VENDOR_DIR)/googletest/%.cc
	@echo "Compiling gtest source $<"
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(GTEST_INC_PATHS) -c $< -o $@

# --- Utility Targets ---
# Maintenance and helper targets for development workflow

# Clean up build artifacts
# clean: Removes all generated files to ensure fresh builds
# Process:
#   1. Remove entire build/ directory (object files, intermediate files)
#   2. Remove entire bin/ directory (executables)
#   3. Force complete rebuild on next make invocation
# Usage: make clean && make (for clean rebuild)
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

# Help target
# help: Displays usage information and available targets
# Process:
#   1. Show Makefile title and usage syntax
#   2. List all available targets with descriptions
#   3. Provide guidance for common development tasks
# Usage: make help (when unsure about available options)
help:
	@echo "GATE Makefile"
	@echo "------------------"
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all       Build the 'gate' executable (default)."
	@echo "  test      Build and run unit tests."
	@echo "  clean     Remove all build artifacts (in build/ and bin/)."
	@echo "  help      Show this help message."
	@echo ""
	@echo "Examples:"
	@echo "  make              # Build main executable"
	@echo "  make test         # Build and run tests"
	@echo "  make clean all    # Clean rebuild"
	@echo "  make CXX=clang++  # Use different compiler"
