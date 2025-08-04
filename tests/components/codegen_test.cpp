#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"
#include "core/NotalLexer.h"
#include "core/NotalParser.h"
#include "diagnostics/DiagnosticEngine.h"
#include "core/PascalCodeGenerator.h"
#include <vector>
#include <string>

TEST(CodeGenTest, BasicProgram) {
    std::string source = R"(
PROGRAM HelloWorld
KAMUS
    msg: string
ALGORITMA
    msg <- 'Hello, World!'
    output(msg)
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();

    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    std::shared_ptr<gate::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    gate::transpiler::PascalCodeGenerator generator;
    std::string result = generator.generate(program);

    std::string expected = R"(
program HelloWorld;

var
  msg: string;

begin
  msg := 'Hello, World!';
  writeln(msg);
end.
)";

    EXPECT_EQ(normalizeCode(result), normalizeCode(expected));
}

TEST(CodeGenTest, ArithmeticExpression) {
    std::string source = R"(
PROGRAM Calculator
KAMUS
    a: integer
    b: integer
    c: integer
ALGORITMA
    a <- 10
    b <- 20
    c <- (a + b) * 2
    output('The result is: ', c)
)";
    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();


    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    std::shared_ptr<gate::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    gate::transpiler::PascalCodeGenerator generator;
    std::string result = generator.generate(program);

    std::string expected = R"(
program Calculator;

var
  a: integer;
  b: integer;
  c: integer;

begin
  a := 10;
  b := 20;
  c := ((a + b) * 2);
  writeln('The result is: ', c);
end.
)";

    EXPECT_EQ(normalizeCode(result), normalizeCode(expected));
}

TEST(CodeGenTest, IfStatement) {
    std::string source = R"(
PROGRAM Grader
KAMUS
    score: integer
    grade: character
ALGORITMA
    score <- 85
    if score >= 90 then
        grade <- 'A'
    elif score >= 80 then
        grade <- 'B'
    elif score >= 70 then
        grade <- 'C'
    else
        grade <- 'D'
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();

    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    std::shared_ptr<gate::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    gate::transpiler::PascalCodeGenerator generator;
    std::string result = generator.generate(program);

    std::string expected = R"(
program Grader;

var
  score: integer;
  grade: char;

begin
  score := 85;
  if (score >= 90) then
  begin
    grade := 'A';
  end
  else if (score >= 80) then
  begin
    grade := 'B';
  end
  else if (score >= 70) then
  begin
    grade := 'C';
  end
  else
  begin
    grade := 'D';
  end;
end.
)";

    EXPECT_EQ(normalizeCode(result), normalizeCode(expected));
}

TEST(CodeGenTest, ConstantsAndInput) {
    std::string source = R"(
PROGRAM TestConstantsAndInput
KAMUS
    constant PI: real = 3.14
    constant GREETING: string = 'Hello'
    nama: string
ALGORITMA
    output(GREETING)
    input(nama)
)";
    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    gate::transpiler::NotalParser parser(lexer.getAllTokens(), diagnosticEngine);
    auto program = parser.parse();
    ASSERT_NE(program, nullptr);

    gate::transpiler::PascalCodeGenerator generator;
    std::string result = generator.generate(program);

    std::string expected = R"(
program TestConstantsAndInput;

const
  PI = 3.14;
  GREETING = 'Hello';

var
  nama: string;

begin
  writeln(GREETING);
  readln(nama);
end.
)";
    ASSERT_EQ(normalizeCode(result), normalizeCode(expected));
}

TEST(CodeGenTest, WhileStatement) {
    std::string source = R"(
PROGRAM Countdown
KAMUS
    counter: integer
ALGORITMA
    counter <- 10
    while counter > 0 do
        output(counter)
        counter <- counter - 1
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();

    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    std::shared_ptr<gate::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    gate::transpiler::PascalCodeGenerator generator;
    std::string result = generator.generate(program);

    std::string expected = R"(
program Countdown;

var
  counter: integer;

begin
  counter := 10;
  while (counter > 0) do
  begin
    writeln(counter);
    counter := (counter - 1);
  end;
end.
)";
    EXPECT_EQ(normalizeCode(result), normalizeCode(expected));
}

TEST(CodeGenTest, NestedWhileStatement) {
    std::string source = R"(
PROGRAM NestedLoop
KAMUS
    i: integer
    j: integer
ALGORITMA
    i <- 3
    while i > 0 do
        j <- 2
        while j > 0 do
            output('i: ', i, ', j: ', j)
            j <- j - 1
        i <- i - 1
)";

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();

    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    std::shared_ptr<gate::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    gate::transpiler::PascalCodeGenerator generator;
    std::string result = generator.generate(program);

    std::string expected = R"(
program NestedLoop;

var
  i: integer;
  j: integer;

begin
  i := 3;
  while (i > 0) do
  begin
    j := 2;
    while (j > 0) do
    begin
      writeln('i: ', i, ', j: ', j);
      j := (j - 1);
    end;
    i := (i - 1);
  end;
end.
)";

    EXPECT_EQ(normalizeCode(result), normalizeCode(expected));
}
