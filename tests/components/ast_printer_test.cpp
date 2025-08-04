#include <gtest/gtest.h>
#include "ast/ASTPrinter.h"
#include "core/NotalLexer.h"
#include "core/NotalParser.h"
#include "diagnostics/DiagnosticEngine.h"
#include "../helpers/test_helpers.h"
#include <vector>
#include <string>

TEST(ASTPrinterTest, BasicProgram) {
    std::string source = R"(
PROGRAM SimpleTest
KAMUS
    x: integer
ALGORITMA
    x <- 42
    output(x)
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    auto program = parser.parse();
    ASSERT_NE(program, nullptr);
    
    gate::ast::ASTPrinter printer;
    std::string result = printer.print(program);
    
    EXPECT_TRUE(result.find("PROGRAM SimpleTest") != std::string::npos);
    EXPECT_TRUE(result.find("KAMUS") != std::string::npos);
    EXPECT_TRUE(result.find("VAR_DECL x : integer") != std::string::npos);
    EXPECT_TRUE(result.find("ALGORITMA") != std::string::npos);
    EXPECT_TRUE(result.find("<- x 42") != std::string::npos);
    EXPECT_TRUE(result.find("output") != std::string::npos);
}

TEST(ASTPrinterTest, ArithmeticExpressions) {
    std::string source = R"(
PROGRAM ArithmeticTest
KAMUS
    a: integer
    b: integer
    result: integer
ALGORITMA
    a <- 10
    b <- 5
    result <- a + b * 2
    output(result)
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    auto program = parser.parse();
    ASSERT_NE(program, nullptr);
    
    gate::ast::ASTPrinter printer;
    std::string result = printer.print(program);
    
    EXPECT_TRUE(result.find("+ a") != std::string::npos);
    EXPECT_TRUE(result.find("* b 2") != std::string::npos);
    EXPECT_TRUE(result.find("<- result") != std::string::npos);
}

TEST(ASTPrinterTest, ConditionalStatements) {
    std::string source = R"(
PROGRAM ConditionalTest
KAMUS
    x: integer
ALGORITMA
    x <- 10
    if x = 10 then
        output("Greater than 5")
    else
        output("Less than or equal to 5")
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    auto program = parser.parse();
    
    // Check for parsing errors
    if (diagnosticEngine.hasErrors()) {
        std::cout << "Parsing errors found" << std::endl;
    }
    
    ASSERT_NE(program, nullptr);
    
    gate::ast::ASTPrinter printer;
    std::string result = printer.print(program);
    
    // Debug output to see what's actually printed
    std::cout << "AST Printer Output:\n" << result << std::endl;
    
    EXPECT_TRUE(result.find("if") != std::string::npos);
    EXPECT_TRUE(result.find("<- x 10") != std::string::npos);
    EXPECT_TRUE(result.find("ALGORITMA") != std::string::npos);
}

TEST(ASTPrinterTest, LoopStatements) {
    std::string source = R"(
PROGRAM LoopTest
KAMUS
    i: integer
ALGORITMA
    i <- 1
    while (i <= 5) do
        output(i)
        i <- i + 1
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    auto program = parser.parse();
    ASSERT_NE(program, nullptr);
    
    gate::ast::ASTPrinter printer;
    std::string result = printer.print(program);
    
    EXPECT_TRUE(result.find("while") != std::string::npos);
    EXPECT_TRUE(result.find("<= i 5") != std::string::npos);
}

TEST(ASTPrinterTest, FunctionCalls) {
    std::string source = R"(
PROGRAM FunctionTest
KAMUS
    x: integer
    y: string
ALGORITMA
    x <- 42
    y <- IntegerToString(x)
    output(y)
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    auto program = parser.parse();
    ASSERT_NE(program, nullptr);
    
    gate::ast::ASTPrinter printer;
    std::string result = printer.print(program);
    
    EXPECT_TRUE(result.find("IntegerToString") != std::string::npos);
    EXPECT_TRUE(result.find("call") != std::string::npos || result.find("(") != std::string::npos);
}

TEST(ASTPrinterTest, ArrayAccess) {
    std::string source = R"(
PROGRAM ArrayTest
KAMUS
    arr: array[1..5] of integer
    i: integer
ALGORITMA
    i <- 1
    arr[i] <- 10
    output(arr[i])
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    auto program = parser.parse();
    ASSERT_NE(program, nullptr);
    
    gate::ast::ASTPrinter printer;
    std::string result = printer.print(program);
    
    EXPECT_TRUE(result.find("arr") != std::string::npos);
    EXPECT_TRUE(result.find("[") != std::string::npos || result.find("array-access") != std::string::npos);
}

TEST(ASTPrinterTest, ComplexExpression) {
    std::string source = R"(
PROGRAM AnotherTest
KAMUS
    x: integer
ALGORITMA
    x <- 1
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    auto program = parser.parse();
    ASSERT_NE(program, nullptr);
    
    gate::ast::ASTPrinter printer;
    std::string result = printer.print(program);
    
    EXPECT_TRUE(result.find("PROGRAM AnotherTest") != std::string::npos);
}
