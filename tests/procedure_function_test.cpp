#include <gtest/gtest.h>
#include "test_helpers.h"
#include "notal_transpiler/Lexer.h"
#include "notal_transpiler/Parser.h"
#include "notal_transpiler/CodeGenerator.h"
#include <vector>
#include <string>

TEST(ProcedureFunctionTest, RecursiveFunctionAndProcedureCall) {
    std::string source = R"(
PROGRAM FactorialTest
KAMUS
    result: integer
    function factorial(input n: integer) -> integer
    procedure printResult(input val: integer)

ALGORITMA
    result <- factorial(5)
    printResult(result)

procedure printResult(input val: integer)
ALGORITMA
    output('Factorial is: ', val)

function factorial(input n: integer) -> integer
KAMUS
    temp: integer
ALGORITMA
    if n <= 1 then
        -> 1
    else
        temp <- factorial(n - 1)
        -> n * temp
)";

    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();
    notal::Parser parser(tokens);
    std::shared_ptr<notal::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    notal::CodeGenerator generator;
    std::string result = generator.generate(program);

    std::string expected = R"(
program FactorialTest;

var
  result: integer;

function factorial(n: integer): integer; forward;
procedure printResult(val: integer); forward;

function factorial(n: integer): integer;
var
  temp: integer;
begin
  if (n <= 1) then
  begin
    factorial := 1;
  end
  else
  begin
    temp := factorial((n - 1));
    factorial := (n * temp);
  end;
end;

procedure printResult(val: integer);
begin
  writeln('Factorial is: ', val);
end;

begin
  result := factorial(5);
  printResult(result);
end.
)";

    ASSERT_EQ(normalizeCode(result), normalizeCode(expected));
}
