# ==============================================================================
# GATE (Ganesha Algorithmic Transpiler Engine) Makefile
#
# This is a standalone Makefile for building the GATE project.
# It does not depend on CMake.
#
# Targets:
#   all       - Build the main 'gate' executable (default).
#   test      - Build and run the 'gate_tests' executable.
#   clean     - Remove all build artifacts.
#   help      - Display this help message.
# ==============================================================================

# --- Variables ---

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
LDFLAGS = -pthread

# Directories
SRC_DIR = src
INCLUDE_DIR = include
VENDOR_DIR = vendor
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = bin

# Test directories
TEST_SRC_DIR = tests
TEST_BUILD_DIR = $(BUILD_DIR)/test
TEST_OBJ_DIR = $(TEST_BUILD_DIR)/obj

# Include paths for our project
INC_PATHS = -I$(INCLUDE_DIR) \
            -I$(VENDOR_DIR)/cxxopts/include

# Include paths for GoogleTest compilation
GTEST_INC_PATHS = -I$(VENDOR_DIR)/googletest/googletest/include \
                  -I$(VENDOR_DIR)/googletest/googlemock/include \
                  -I$(VENDOR_DIR)/googletest/googletest \
                  -I$(VENDOR_DIR)/googletest/googlemock

# --- Source Files ---

# Find all .cpp files in the library source directories
GATE_LIB_SRCS = $(wildcard $(SRC_DIR)/transpiler/*.cpp) \
                $(wildcard $(SRC_DIR)/ast/*.cpp) \
                $(wildcard $(SRC_DIR)/core/*.cpp) \
                $(wildcard $(SRC_DIR)/utils/*.cpp)

# Main application source
GATE_MAIN_SRC = $(SRC_DIR)/main.cpp

# Test source files
# test_main.cpp is removed because we use the main from gtest_main.cc
TEST_SRCS = $(wildcard $(TEST_SRC_DIR)/unit/*.cpp) \
            $(wildcard $(TEST_SRC_DIR)/helpers/*.cpp)

# GoogleTest source files (we compile it ourselves)
GTEST_SRCS = $(VENDOR_DIR)/googletest/googletest/src/gtest-all.cc \
             $(VENDOR_DIR)/googletest/googlemock/src/gmock-all.cc \
             $(VENDOR_DIR)/googletest/googletest/src/gtest_main.cc

# --- Object Files ---

# Generate object file paths from source file paths
GATE_LIB_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(GATE_LIB_SRCS))
GATE_MAIN_OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(GATE_MAIN_SRC))
TEST_OBJS = $(patsubst $(TEST_SRC_DIR)/%.cpp, $(TEST_OBJ_DIR)/%.o, $(TEST_SRCS))
GTEST_OBJS = $(patsubst $(VENDOR_DIR)/googletest/%.cc, $(TEST_OBJ_DIR)/gtest/%.o, $(GTEST_SRCS))

# --- Targets ---

# Executable targets
TARGET = $(BIN_DIR)/gate
TEST_TARGET = $(BIN_DIR)/gate_tests

# Phony targets (targets that don't represent files)
.PHONY: all test clean help

# Default target
all: $(TARGET)

# Build the main executable
$(TARGET): $(GATE_LIB_OBJS) $(GATE_MAIN_OBJ)
	@echo "Linking main executable: $@"
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Build complete. Executable at: $@"

# Build and run tests
test: $(TEST_TARGET)
	@echo "Running tests..."
	@./$(TEST_TARGET)

# Build the test executable
$(TEST_TARGET): $(TEST_OBJS) $(GATE_LIB_OBJS) $(GTEST_OBJS)
	@echo "Linking test executable: $@"
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Test build complete. Executable at: $@"

# Rule to compile library source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC_PATHS) -c $< -o $@

# Rule to compile test source files into object files
$(TEST_OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.cpp
	@echo "Compiling test file $<"
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC_PATHS) $(GTEST_INC_PATHS) -c $< -o $@

# Rule to compile googletest source files
$(TEST_OBJ_DIR)/gtest/%.o: $(VENDOR_DIR)/googletest/%.cc
	@echo "Compiling gtest source $<"
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(GTEST_INC_PATHS) -c $< -o $@

# Clean up build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

# Help target
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
