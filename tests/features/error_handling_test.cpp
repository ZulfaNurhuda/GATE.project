#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"
#include "core/NotalLexer.h"
#include "core/NotalParser.h"
#include "core/PascalCodeGenerator.h"
#include "diagnostics/DiagnosticEngine.h"
#include <vector>
#include <string>

TEST(ErrorHandlingTest, SyntaxErrorDetection) {
    std::string source = R"(
PROGRAM ErrorTest
KAMUS
    i: integer
ALGORITMA
    i <- (10 + )
    output(i)
)";

    // Test that syntax error is detected
    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "error-test");
    gate::transpiler::NotalLexer lexer(source, "error-test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    auto ast = parser.parse();
    
    // Check that error was detected
    EXPECT_TRUE(diagnosticEngine.hasErrors());
}

TEST(ErrorHandlingTest, IncompleteExpression) {
    std::string source = R"(
PROGRAM IncompleteExprTest
KAMUS
    result: integer
ALGORITMA
    result <- 5 *
    output(result)
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "incomplete-test");
    gate::transpiler::NotalLexer lexer(source, "incomplete-test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    auto ast = parser.parse();
    
    // Check that incomplete expression error is detected
    EXPECT_TRUE(diagnosticEngine.hasErrors());
}

TEST(ErrorHandlingTest, MissingDeclaration) {
    std::string source = R"(
PROGRAM MissingDeclTest
KAMUS
ALGORITMA
    undeclared_var <- 42
    output(undeclared_var)
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "missing-decl-test");
    gate::transpiler::NotalLexer lexer(source, "missing-decl-test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    auto ast = parser.parse();
    
    // Parser should complete but semantic analysis might catch this
    // For now, just ensure parsing doesn't crash
    EXPECT_TRUE(ast != nullptr);
}

TEST(ErrorHandlingTest, InvalidTokenSequence) {
    std::string source = R"(
PROGRAM InvalidTokenTest
KAMUS
    x: integer
ALGORITMA
    x <- <- 5
    output(x)
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "invalid-token-test");
    gate::transpiler::NotalLexer lexer(source, "invalid-token-test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    auto ast = parser.parse();
    
    // Check that invalid token sequence is detected
    EXPECT_TRUE(diagnosticEngine.hasErrors());
}