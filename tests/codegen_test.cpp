#include <gtest/gtest.h>
#include "notal_transpiler/Lexer.h"
#include "notal_transpiler/Parser.h"
#include "notal_transpiler/CodeGenerator.h"
#include <vector>
#include <string>
#include <regex>
#include <algorithm>
#include <cctype>

// Helper function to normalize whitespace and remove case sensitivity for comparison
std::string normalizeCode(const std::string& s) {
    std::string result = s;
    // Convert to lower case
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    // Remove newlines, tabs, and carriage returns
    result = std::regex_replace(result, std::regex("[\n\t\r]"), " ");
    // Replace multiple spaces with a single space
    result = std::regex_replace(result, std::regex(" +"), " ");
    // Trim leading/trailing space
    result = std::regex_replace(result, std::regex("^ +| +$"), "");
    return result;
}


TEST(CodeGenTest, BasicProgram) {
    std::string source = R"(
PROGRAM HelloWorld
KAMUS
    msg: string
ALGORITMA
    msg <- 'Hello, World!'
    output(msg)
)";

    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();

    notal::Parser parser(tokens);
    std::shared_ptr<notal::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    notal::CodeGenerator generator;
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
    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();


    notal::Parser parser(tokens);
    std::shared_ptr<notal::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    notal::CodeGenerator generator;
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

    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();

    notal::Parser parser(tokens);
    std::shared_ptr<notal::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    notal::CodeGenerator generator;
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
    notal::Lexer lexer(source);
    notal::Parser parser(lexer.allTokens());
    auto program = parser.parse();
    ASSERT_NE(program, nullptr);

    notal::CodeGenerator generator;
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

    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();

    notal::Parser parser(tokens);
    std::shared_ptr<notal::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    notal::CodeGenerator generator;
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
    std::cout << "Result:\n" << result << std::endl;
    std::cout << "Expected:\n" << expected << std::endl;
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

    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();

    notal::Parser parser(tokens);
    std::shared_ptr<notal::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    notal::CodeGenerator generator;
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
