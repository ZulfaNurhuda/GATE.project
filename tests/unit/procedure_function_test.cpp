#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"
#include "gate/transpiler/NotalLexer.h"
#include "gate/transpiler/NotalParser.h"
#include "gate/diagnostics/DiagnosticEngine.h"
#include "gate/transpiler/PascalCodeGenerator.h"
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

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    std::shared_ptr<gate::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    gate::transpiler::PascalCodeGenerator generator;
    std::string result = generator.generate(program);

    std::string expected = R"(
program FactorialTest;

var
  result: integer;

function factorial(n: integer): integer; forward;
procedure printResult(val: integer); forward;

procedure printResult(val: integer);
begin
  writeln('Factorial is: ', val);
end;

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

begin
  result := factorial(5);
  printResult(result);
end.
)";

    ASSERT_EQ(normalizeCode(result), normalizeCode(expected));
}
