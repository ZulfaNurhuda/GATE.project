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

TEST(ParserTest, DependOnLiteral) {
    std::string source = R"(
PROGRAM TestDependOn
KAMUS
    op: character
ALGORITMA
    depend on (op)
        '+': output('Tambah')
        '-': output('Kurang')
        otherwise: output('Lainnya')
)";
    notal::Lexer lexer(source);
    notal::Parser parser(lexer.allTokens());
    auto program = parser.parse();

    ASSERT_NE(program, nullptr);
    auto algoritma = program->algoritma;
    ASSERT_EQ(algoritma->body->statements.size(), (size_t)1);

    auto dependOnStmt = std::dynamic_pointer_cast<notal::ast::DependOnStmt>(algoritma->body->statements[0]);
    ASSERT_NE(dependOnStmt, nullptr);
    ASSERT_NE(dependOnStmt->expression, nullptr);
    ASSERT_EQ(dependOnStmt->cases.size(), 2);
    ASSERT_NE(dependOnStmt->otherwiseBranch, nullptr);

    // Case '+'
    ASSERT_EQ(dependOnStmt->cases[0].conditions.size(), 1);
    auto cond1 = std::dynamic_pointer_cast<notal::ast::Literal>(dependOnStmt->cases[0].conditions[0]);
    ASSERT_NE(cond1, nullptr);
    ASSERT_EQ(std::any_cast<std::string>(cond1->value), "+");

    // Case '-'
    ASSERT_EQ(dependOnStmt->cases[1].conditions.size(), 1);
    auto cond2 = std::dynamic_pointer_cast<notal::ast::Literal>(dependOnStmt->cases[1].conditions[0]);
    ASSERT_NE(cond2, nullptr);
    ASSERT_EQ(std::any_cast<std::string>(cond2->value), "-");
}

TEST(ParserTest, DependOnComplex) {
    std::string source = R"(
PROGRAM TestDependOn
KAMUS
    nilai: integer
ALGORITMA
    depend on (true)
        nilai > 90: output('A')
        nilai > 80: output('B')
)";
    notal::Lexer lexer(source);
    notal::Parser parser(lexer.allTokens());
    auto program = parser.parse();

    ASSERT_NE(program, nullptr);
    auto algoritma = program->algoritma;
    ASSERT_EQ(algoritma->body->statements.size(), 1);

    auto dependOnStmt = std::dynamic_pointer_cast<notal::ast::DependOnStmt>(algoritma->body->statements[0]);
    ASSERT_NE(dependOnStmt, nullptr);
    ASSERT_NE(dependOnStmt->expression, nullptr);
    ASSERT_EQ(dependOnStmt->cases.size(), 2);
    ASSERT_EQ(dependOnStmt->otherwiseBranch, nullptr);

    // Case nilai > 90
    ASSERT_EQ(dependOnStmt->cases[0].conditions.size(), 1);
    auto cond1 = std::dynamic_pointer_cast<notal::ast::Binary>(dependOnStmt->cases[0].conditions[0]);
    ASSERT_NE(cond1, nullptr);
}

TEST(ParserTest, ConstantDeclaration) {
    std::string source = R"(
PROGRAM TestConstants
KAMUS
    constant PI: real = 3.14
    constant MAX: integer = 100
ALGORITMA
    output(PI)
)";
    notal::Lexer lexer(source);
    notal::Parser parser(lexer.allTokens());
    auto program = parser.parse();

    ASSERT_NE(program, nullptr);
    ASSERT_NE(program->kamus, nullptr);
    ASSERT_EQ(program->kamus->declarations.size(), 2);

    auto constDecl1 = std::dynamic_pointer_cast<notal::ast::ConstDeclStmt>(program->kamus->declarations[0]);
    ASSERT_NE(constDecl1, nullptr);
    EXPECT_EQ(constDecl1->name.lexeme, "PI");
    EXPECT_EQ(constDecl1->type.type, notal::TokenType::REAL);
    auto literal1 = std::dynamic_pointer_cast<notal::ast::Literal>(constDecl1->initializer);
    ASSERT_NE(literal1, nullptr);
    EXPECT_EQ(std::any_cast<double>(literal1->value), 3.14);

    auto constDecl2 = std::dynamic_pointer_cast<notal::ast::ConstDeclStmt>(program->kamus->declarations[1]);
    ASSERT_NE(constDecl2, nullptr);
    EXPECT_EQ(constDecl2->name.lexeme, "MAX");
    EXPECT_EQ(constDecl2->type.type, notal::TokenType::INTEGER);
    auto literal2 = std::dynamic_pointer_cast<notal::ast::Literal>(constDecl2->initializer);
    ASSERT_NE(literal2, nullptr);
    EXPECT_EQ(std::any_cast<int>(literal2->value), 100);
}

TEST(ParserTest, InputStatement) {
    std::string source = R"(
PROGRAM TestInput
KAMUS
    nama: string
ALGORITMA
    input(nama)
)";
    notal::Lexer lexer(source);
    notal::Parser parser(lexer.allTokens());
    auto program = parser.parse();

    ASSERT_NE(program, nullptr);
    ASSERT_NE(program->algoritma, nullptr);
    ASSERT_NE(program->algoritma->body, nullptr);
    ASSERT_EQ(program->algoritma->body->statements.size(), 1);

    auto inputStmt = std::dynamic_pointer_cast<notal::ast::InputStmt>(program->algoritma->body->statements[0]);
    ASSERT_NE(inputStmt, nullptr);
    EXPECT_EQ(inputStmt->variable->name.lexeme, "nama");
}
