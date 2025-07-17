#include <gtest/gtest.h>
#include "notal_transpiler/Lexer.h"
#include "notal_transpiler/Parser.h"
#include "notal_transpiler/ASTPrinter.h"
#include <vector>
#include <string>
#include <regex>

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


TEST(ParserTest, BasicProgram) {
    std::string source = R"(
PROGRAM Test
KAMUS
    x: integer
ALGORITMA
    x <- 42
    output(x + 1)
)";

    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();

    notal::Parser parser(tokens);
    std::shared_ptr<notal::ast::ProgramStmt> program = parser.parse();

    ASSERT_NE(program, nullptr);

    notal::ASTPrinter printer;
    std::string result = printer.print(program);

    std::string expected = R"(
(PROGRAM Test
  (KAMUS
    (VAR_DECL x : integer)
  )
  (ALGORITMA (block
    (expr-stmt (<- x 42))
    (output (+ x 1))
  ))
)
)";

    EXPECT_EQ(cleanString(result), cleanString(expected));
}


