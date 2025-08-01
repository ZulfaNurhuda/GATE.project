cmake_minimum_required(VERSION 3.16)
project(GATE VERSION 1.0.0 LANGUAGES CXX)

# Modern C++ standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(GATE_BUILD_TESTS "Build tests" ON)
option(GATE_BUILD_EXAMPLES "Build examples" ON)
option(GATE_ENABLE_WARNINGS "Enable compiler warnings" ON)

# Define source and include directories to keep things clean
set(GATE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(GATE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add vendor dependencies
add_subdirectory(vendor/cxxopts)

# Gather source files for the library (excluding main.cpp)
file(GLOB_RECURSE GATE_LIB_SOURCES
    "${GATE_SOURCE_DIR}/transpiler/*.cpp"
    "${GATE_SOURCE_DIR}/ast/*.cpp"
    "${GATE_SOURCE_DIR}/core/*.cpp"
    "${GATE_SOURCE_DIR}/utils/*.cpp"
)

# Main library
add_library(gate_lib STATIC
    ${GATE_LIB_SOURCES}
)

target_include_directories(gate_lib PUBLIC ${GATE_INCLUDE_DIR})

# Main executable
add_executable(gate ${GATE_SOURCE_DIR}/main.cpp)
target_link_libraries(gate PRIVATE gate_lib cxxopts::cxxopts)

# Testing
if(GATE_BUILD_TESTS)
    enable_testing()
    # Add googletest
    add_subdirectory(vendor/googletest)

    # Gather test sources
    file(GLOB_RECURSE TEST_SOURCES
        "tests/unit/*.cpp"
        "tests/helpers/*.cpp"
    )

    # Create the test executable
    add_executable(gate_tests
        ${TEST_SOURCES}
    )

    # Link test executable against gtest and our library
    target_link_libraries(gate_tests PRIVATE gate_lib gtest_main)

    # Add test to CTest
    add_test(NAME unit_tests COMMAND gate_tests)
endif()
