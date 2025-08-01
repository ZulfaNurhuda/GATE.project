#include "test_helpers.h"
#include "gate/transpiler/NotalLexer.h"
#include "gate/transpiler/NotalParser.h"
#include "gate/transpiler/PascalCodeGenerator.h"
#include <cctype>
#include <regex>
#include <algorithm>

// Helper function to transpile NOTAL code to Pascal
std::string transpile(const std::string& notalCode) {
    gate::transpiler::NotalLexer lexer(notalCode);
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    gate::transpiler::NotalParser parser(tokens);
    std::shared_ptr<gate::ast::ProgramStmt> program = parser.parse();
    if (!program) {
        return "// Parsing failed";
    }
    gate::transpiler::PascalCodeGenerator generator;
    return generator.generate(program);
}

// Helper function to normalize whitespace and remove case sensitivity for comparison
std::string normalizeCode(const std::string& s) {
    std::string result = s;
    // Convert to lower case
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    // Remove newlines, tabs, and carriage returns
    result = std::regex_replace(result, std::regex("[\\n\\t\\r]"), " ");
    // Replace multiple spaces with a single space
    result = std::regex_replace(result, std::regex(" +"), " ");
    // Trim leading/trailing space
    result = std::regex_replace(result, std::regex("^ +| +$"), "");
    return result;
}

// Helper function to remove inconsistent whitespace for comparison
std::string cleanString(const std::string& s) {
    std::string result = s;
    // Remove newlines and tabs
    result = std::regex_replace(result, std::regex("[\\n\\t]"), "");
    // Replace multiple spaces with a single space
    result = std::regex_replace(result, std::regex(" +"), " ");
    // Remove spaces around parentheses
    result = std::regex_replace(result, std::regex(" *\\( *"), "(");
    result = std::regex_replace(result, std::regex(" *\\) *"), ")");
    return result;
}
