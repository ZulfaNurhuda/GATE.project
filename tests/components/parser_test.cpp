#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"
#include "core/NotalLexer.h"
#include "core/NotalParser.h"
#include "diagnostics/DiagnosticEngine.h"
#include "ast/ASTPrinter.h"
#include <vector>
#include <string>

TEST(ParserTest, BasicProgram) {
    std::string source = R"(
PROGRAM Test
KAMUS
    x: integer
ALGORITMA
    x <- 42
    output(x + 1)
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();

    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    std::shared_ptr<gate::ast::ProgramStmt> program = parser.parse();

    ASSERT_NE(program, nullptr);

    gate::ast::ASTPrinter printer;
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
    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    gate::transpiler::NotalParser parser(lexer.getAllTokens(), diagnosticEngine);
    auto program = parser.parse();

    ASSERT_NE(program, nullptr);
    auto algoritma = program->algoritma;
    ASSERT_EQ(static_cast<long long>(algoritma->body->statements.size()), 1LL);

    auto dependOnStmt = std::dynamic_pointer_cast<gate::ast::DependOnStmt>(algoritma->body->statements[0]);
    ASSERT_NE(dependOnStmt, nullptr);
    ASSERT_EQ(static_cast<long long>(dependOnStmt->expressions.size()), 1LL);
    ASSERT_EQ(static_cast<long long>(dependOnStmt->cases.size()), 2LL);
    ASSERT_NE(dependOnStmt->otherwiseBranch, nullptr);

    // Case '+'
    ASSERT_EQ(static_cast<long long>(dependOnStmt->cases[0].conditions.size()), 1LL);
    auto cond1 = std::dynamic_pointer_cast<gate::ast::Literal>(dependOnStmt->cases[0].conditions[0]);
    ASSERT_NE(cond1, nullptr);
    ASSERT_EQ(std::any_cast<std::string>(cond1->value), "+");

    // Case '-'
    ASSERT_EQ(static_cast<long long>(dependOnStmt->cases[1].conditions.size()), 1LL);
    auto cond2 = std::dynamic_pointer_cast<gate::ast::Literal>(dependOnStmt->cases[1].conditions[0]);
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
    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    gate::transpiler::NotalParser parser(lexer.getAllTokens(), diagnosticEngine);
    auto program = parser.parse();

    ASSERT_NE(program, nullptr);
    auto algoritma = program->algoritma;
    ASSERT_EQ(static_cast<long long>(algoritma->body->statements.size()), 1LL);

    auto dependOnStmt = std::dynamic_pointer_cast<gate::ast::DependOnStmt>(algoritma->body->statements[0]);
    ASSERT_NE(dependOnStmt, nullptr);
    ASSERT_EQ(static_cast<long long>(dependOnStmt->expressions.size()), 1LL);
    ASSERT_EQ(static_cast<long long>(dependOnStmt->cases.size()), 2LL);
    ASSERT_EQ(dependOnStmt->otherwiseBranch, nullptr);

    // Case nilai > 90
    ASSERT_EQ(static_cast<long long>(dependOnStmt->cases[0].conditions.size()), 1LL);
    auto cond1 = std::dynamic_pointer_cast<gate::ast::Binary>(dependOnStmt->cases[0].conditions[0]);
    ASSERT_NE(cond1, nullptr);
}

TEST(ParserTest, DependOnMultipleVariables) {
    std::string source = R"(
PROGRAM DependOnMultiple
KAMUS
    score: integer
    attendance: integer
ALGORITMA
    depend on (score, attendance)
        score >= 90 and attendance >= 80: output('Excellent')
        score >= 70 and attendance >= 60: output('Good')
        otherwise: output('Needs improvement')
)";
    std::string expected_pascal_code = R"(
program DependOnMultiple;

var
  score: integer;
  attendance: integer;

begin
  if ((score >= 90) and (attendance >= 80)) then
  begin
    writeln('Excellent');
  end
  else if ((score >= 70) and (attendance >= 60)) then
  begin
    writeln('Good');
  end
  else
  begin
    writeln('Needs improvement');
  end;
end.
)";
    std::string generated_code = transpile(source);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
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
    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    gate::transpiler::NotalParser parser(lexer.getAllTokens(), diagnosticEngine);
    auto program = parser.parse();

    ASSERT_NE(program, nullptr);
    ASSERT_NE(program->kamus, nullptr);
    ASSERT_EQ(static_cast<long long>(program->kamus->declarations.size()), 2LL);

    auto constDecl1 = std::dynamic_pointer_cast<gate::ast::ConstDeclStmt>(program->kamus->declarations[0]);
    ASSERT_NE(constDecl1, nullptr);
    EXPECT_EQ(constDecl1->name.lexeme, "PI");
    EXPECT_EQ(constDecl1->type.type, gate::core::TokenType::REAL);
    auto literal1 = std::dynamic_pointer_cast<gate::ast::Literal>(constDecl1->initializer);
    ASSERT_NE(literal1, nullptr);
    EXPECT_EQ(std::any_cast<double>(literal1->value), 3.14);

    auto constDecl2 = std::dynamic_pointer_cast<gate::ast::ConstDeclStmt>(program->kamus->declarations[1]);
    ASSERT_NE(constDecl2, nullptr);
    EXPECT_EQ(constDecl2->name.lexeme, "MAX");
    EXPECT_EQ(constDecl2->type.type, gate::core::TokenType::INTEGER);
    auto literal2 = std::dynamic_pointer_cast<gate::ast::Literal>(constDecl2->initializer);
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
    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    gate::transpiler::NotalParser parser(lexer.getAllTokens(), diagnosticEngine);
    auto program = parser.parse();

    ASSERT_NE(program, nullptr);
    ASSERT_NE(program->algoritma, nullptr);
    ASSERT_NE(program->algoritma->body, nullptr);
    ASSERT_EQ(static_cast<long long>(program->algoritma->body->statements.size()), 1LL);

    auto inputStmt = std::dynamic_pointer_cast<gate::ast::InputStmt>(program->algoritma->body->statements[0]);
    ASSERT_NE(inputStmt, nullptr);
    EXPECT_EQ(inputStmt->variable->name.lexeme, "nama");
}
