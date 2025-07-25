#include "test_helpers.h"
#include "notal_transpiler/Lexer.h"
#include "notal_transpiler/Parser.h"
#include "notal_transpiler/CodeGenerator.h"
#include <cctype>
#include <regex>
#include <algorithm>

// Helper function to transpile NOTAL code to Pascal
std::string transpile(const std::string& notal_code) {
    notal::Lexer lexer(notal_code);
    std::vector<notal::Token> tokens = lexer.allTokens();
    notal::Parser parser(tokens);
    std::shared_ptr<notal::ast::ProgramStmt> program = parser.parse();
    if (!program) {
        return "// Parsing failed";
    }
    notal::CodeGenerator generator;
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
