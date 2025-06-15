# Makefile for the NOTAL to C Transpiler (GATE)

# Compiler and flags
CXX = g++
# Adjust CXXFLAGS: use -Iinclude for transpiler's headers. Add -std=c++17.
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS = # Add any linker specific flags here if needed in future
DEBUG_FLAGS = -g

# Directories
SRC_DIR = src
# HEADER_DIR is used in CXXFLAGS via -Iinclude
BUILD_DIR = build

# Target executable name
TARGET = gate

# Automatically find all .cpp files in SRC_DIR
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Define object files based on SOURCES, placing them in BUILD_DIR
# e.g., src/main.cpp -> build/main.o
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

# Default target: build the executable
all: $(BUILD_DIR) $(TARGET)

# Rule to create the build directory if it doesn't exist
$(BUILD_DIR):
	@echo "Creating build directory: $(BUILD_DIR)..."
	@mkdir -p $(BUILD_DIR)

# Linking rule: link all object files into the target executable
$(TARGET): $(OBJECTS)
	@echo "Linking to $(TARGET)..."
	@$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "$(TARGET) build complete!"

# Compilation rule: compile .cpp source files from SRC_DIR to .o object files in BUILD_DIR
# Depends on the source file and potentially any header in the include directory.
# A more precise per-header dependency is complex for typical Makefiles without extra tools.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(wildcard include/*.hpp) Makefile
	@echo "Compiling $< to $@..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Target for building with debugging symbols
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: clean all

# Target to run the program with a sample input
# This provides a basic test run. Users can change the arguments.
run: all
	@echo "Running $(TARGET) with example..."
	@./$(TARGET) input/example.notal -o output/example.c
	@echo "Transpilation example complete. Check output/example.c."
	@echo "To compile and run the C output (example):"
	@echo "gcc -Wall -Wextra output/example.c adt/*.c -o output/example_c_exec -lm && ./output/example_c_exec"


# Target to clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(TARGET)
	@echo "Clean complete!"

# Phony targets (targets that are not actual files)
.PHONY: all debug run clean
