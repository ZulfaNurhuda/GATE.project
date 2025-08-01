#ifndef GATE_TESTS_HELPERS_TEST_HELPERS_H
#define GATE_TESTS_HELPERS_TEST_HELPERS_H

#include <string>
#include <memory>
#include <vector>
#include <regex>
#include <algorithm>

namespace gate::core {
    struct Token;
}

namespace gate::ast {
    struct ProgramStmt;
}

// Helper function to transpile NOTAL code to Pascal
std::string transpile(const std::string& notalCode);

// Helper to remove extra whitespace and newlines for consistent comparison
std::string normalizeCode(const std::string& s);

// Helper function to remove inconsistent whitespace for comparison
std::string cleanString(const std::string& s);

#endif // GATE_TESTS_HELPERS_TEST_HELPERS_H
