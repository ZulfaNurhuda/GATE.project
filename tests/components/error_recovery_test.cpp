#include <gtest/gtest.h>
#include "core/ErrorRecovery.h"
#include "core/NotalLexer.h"
#include "core/NotalParser.h"
#include "diagnostics/DiagnosticEngine.h"
#include <vector>
#include <string>

TEST(ErrorRecoveryTest, PanicModeRecovery) {
    std::string source = R"(
PROGRAM ErrorTest
KAMUS
    x: integer
    y: integer
ALGORITMA
    x <- (10 + )
    y <- 20
    output(y)
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "error-test");
    gate::transpiler::NotalLexer lexer(source, "error-test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    auto ast = parser.parse();
    
    // Parser should recover and continue parsing after error
    EXPECT_TRUE(diagnosticEngine.hasErrors());
    EXPECT_NE(ast, nullptr); // Should still produce some AST
}

TEST(ErrorRecoveryTest, PhraseLevelRecovery) {
    std::string source = R"(
PROGRAM RecoveryTest
KAMUS
    x: integer
    y: integer
ALGORITMA
    x <- 10
    y <- x +
    output(x)
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "recovery-test");
    gate::transpiler::NotalLexer lexer(source, "recovery-test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    auto ast = parser.parse();
    
    // Should detect error but continue parsing
    EXPECT_TRUE(diagnosticEngine.hasErrors());
    EXPECT_NE(ast, nullptr);
}

TEST(ErrorRecoveryTest, MultipleSyntaxErrors) {
    std::string source = R"(
PROGRAM MultiErrorTest
KAMUS
    x: integer
    y: integer
    z: integer
ALGORITMA
    x <- (10 + )
    y <- * 5
    z <- 30
    output(z)
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "multi-error-test");
    gate::transpiler::NotalLexer lexer(source, "multi-error-test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    auto ast = parser.parse();
    
    // Should detect multiple errors but still produce AST
    EXPECT_TRUE(diagnosticEngine.hasErrors());
    EXPECT_GT(diagnosticEngine.getErrorCount(), 1);
    EXPECT_NE(ast, nullptr);
}

TEST(ErrorRecoveryTest, MissingTokenRecovery) {
    std::string source = R"(
PROGRAM MissingTokenTest
KAMUS
    x: integer
ALGORITMA
    x <- 10
    if x > 5
        output("Greater than 5")
    endif
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "missing-token-test");
    gate::transpiler::NotalLexer lexer(source, "missing-token-test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    auto ast = parser.parse();
    
    // Should handle missing 'then' keyword
    EXPECT_NE(ast, nullptr);
}

TEST(ErrorRecoveryTest, UnexpectedTokenRecovery) {
    std::string source = R"(
PROGRAM UnexpectedTokenTest
KAMUS
    x: integer
ALGORITMA
    x <- 10
    output(x) extra_token
    x <- 20
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "unexpected-token-test");
    gate::transpiler::NotalLexer lexer(source, "unexpected-token-test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    auto ast = parser.parse();
    
    // Should handle unexpected token and continue
    EXPECT_NE(ast, nullptr);
}

TEST(ErrorRecoveryTest, NestedErrorRecovery) {
    std::string source = R"(
PROGRAM NestedErrorTest
KAMUS
    x: integer
    y: integer
ALGORITMA
    if x > 0 then
        y <- (x + )
        output(y)
    endif
    x <- 10
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "nested-error-test");
    gate::transpiler::NotalLexer lexer(source, "nested-error-test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    auto ast = parser.parse();
    
    // Should recover from nested expression error
    EXPECT_TRUE(diagnosticEngine.hasErrors());
    EXPECT_NE(ast, nullptr);
}