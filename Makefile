# Makefile for NOTAL to Pascal Transpiler

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
LDFLAGS =

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin
VENDOR_DIR = vendor
TESTS_DIR = tests

# Target executable
TARGET = $(BIN_DIR)/notal_transpiler

# Source files and object files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

# Include paths
INCLUDES = -I$(INCLUDE_DIR) -I$(VENDOR_DIR)/cxxopts/include

# GTest variables
GTEST_DIR = $(VENDOR_DIR)/googletest
GTEST_INCLUDE_DIR = $(GTEST_DIR)/googletest/include
GTEST_SRC_DIR = $(GTEST_DIR)/googletest/src
GTEST_BUILD_DIR = $(BUILD_DIR)/gtest
GTEST_LIB = $(GTEST_BUILD_DIR)/libgtest.a
TEST_SOURCES = $(wildcard $(TESTS_DIR)/*.cpp)
TEST_OBJECTS = $(patsubst $(TESTS_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(TEST_SOURCES))
TEST_EXECUTABLE = $(BIN_DIR)/run_tests

# Default target
all: $(TARGET)

# Rule to build the main transpiler
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Rule to build the test runner
tests: $(TEST_EXECUTABLE)
$(TEST_EXECUTABLE): $(OBJECTS) $(TEST_OBJECTS) $(GTEST_LIB) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -I$(GTEST_INCLUDE_DIR) -o $@ $(filter-out $(BUILD_DIR)/main.o,$(OBJECTS)) $(TEST_OBJECTS) $(GTEST_LIB) -pthread

# Rule to compile test source files
$(BUILD_DIR)/%.o: $(TESTS_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -I$(GTEST_INCLUDE_DIR) -c $< -o $@

# Rule to build gtest library
$(GTEST_LIB): | $(GTEST_BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(GTEST_INCLUDE_DIR) -I$(GTEST_DIR)/googletest -c $(GTEST_SRC_DIR)/gtest-all.cc -o $(GTEST_BUILD_DIR)/gtest-all.o
	ar -rv $@ $(GTEST_BUILD_DIR)/gtest-all.o

# Rule to run tests
run-tests: tests
	./$(TEST_EXECUTABLE)

# Create directories if they don't exist
$(BUILD_DIR) $(BIN_DIR) $(GTEST_BUILD_DIR):
	mkdir -p $@

# Phony targets
.PHONY: all tests run-tests clean

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

