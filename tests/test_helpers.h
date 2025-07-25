#ifndef NOTAL_TRANSPILER_TEST_HELPERS_H
#define NOTAL_TRANSPILER_TEST_HELPERS_H

#include <string>
#include <memory>
#include <vector>

namespace notal {
    class Token;
    namespace ast {
        class ProgramStmt;
    }
}

// Helper function to transpile NOTAL code to Pascal
std::string transpile(const std::string& notal_code);

#include <regex>
#include <algorithm>

// Helper to remove extra whitespace and newlines for consistent comparison
std::string normalizeCode(const std::string& s);

// Helper function to remove inconsistent whitespace for comparison
std::string cleanString(const std::string& s);

#endif // NOTAL_TRANSPILER_TEST_HELPERS_H
