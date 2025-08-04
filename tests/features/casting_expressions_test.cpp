#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"

TEST(CastingExpressionsTest, BasicCasting) {
    std::string notalCode = R"(
PROGRAM CastingExample

KAMUS
  myInt: integer
  myStr: string
  myBool: boolean
  success: boolean

ALGORITMA
  myInt <- 123
  IntegerToString(myInt, myStr)
  output("The integer as a string is: ", myStr)

  success <- StringToBoolean("True", myBool)
  if success then
    output("StringToBoolean was successful. The value is: ", myBool)
  else
    output("StringToBoolean failed.")
)";
    std::string expected_pascal_code = R"(program CastingExample;

var
  myInt: integer;
  myStr: string;
  myBool: boolean;
  success: boolean;

begin
  myInt := 123;
  IntegerToString(myInt, myStr);
  writeln('The integer as a string is: ', myStr);
  success := StringToBoolean('True', myBool);
  if success then
    writeln('StringToBoolean was successful. The value is: ', myBool)
  else
    writeln('StringToBoolean failed.');
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(CastingExpressionsTest, ArithmeticExpressions) {
    std::string notalCode = R"(
PROGRAM ExpressionsExample

KAMUS
    a: integer
    b: integer
    c: integer
    d: real

ALGORITMA
    a <- 10
    b <- 5
    c <- a + b * 2
    d <- (a + b) / 2.0

    output('c = ')
    output(c)
    output('d = ')
    output(d)
)";
    std::string expected_pascal_code = R"(program ExpressionsExample;

var
  a: integer;
  b: integer;
  c: integer;
  d: real;

begin
  a := 10;
  b := 5;
  c := a + b * 2;
  d := (a + b) / 2.0;
  writeln('c = ');
  writeln(c);
  writeln('d = ');
  writeln(d);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(CastingExpressionsTest, ComplexExpressions) {
    std::string notalCode = R"(
PROGRAM ComplexExpressionsExample

KAMUS
    x: real
    y: real
    result: real
    isPositive: boolean

ALGORITMA
    x <- 3.5
    y <- 2.1
    result <- (x * y + 5.0) / (x - y)
    isPositive <- result > 0
    
    output('Result: ', result)
    output('Is positive: ', isPositive)
)";
    std::string expected_pascal_code = R"(program ComplexExpressionsExample;

var
  x: real;
  y: real;
  result: real;
  isPositive: boolean;

begin
  x := 3.5;
  y := 2.1;
  result := (x * y + 5.0) / (x - y);
  isPositive := result > 0;
  writeln('Result: ', result);
  writeln('Is positive: ', isPositive);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(CastingExpressionsTest, TypeConversions) {
    std::string notalCode = R"(
PROGRAM TypeConversionsExample

KAMUS
    intVal: integer
    realVal: real
    strVal: string
    boolVal: boolean
    charVal: character

ALGORITMA
    intVal <- 42
    RealToInteger(3.14, intVal)
    IntegerToReal(intVal, realVal)
    BooleanToString(true, strVal)
    CharToInteger('A', intVal)
    
    output('Converted values:')
    output('Integer: ', intVal)
    output('Real: ', realVal)
    output('String: ', strVal)
)";
    std::string expected_pascal_code = R"(program TypeConversionsExample;

var
  intVal: integer;
  realVal: real;
  strVal: string;
  boolVal: boolean;
  charVal: char;

begin
  intVal := 42;
  RealToInteger(3.14, intVal);
  IntegerToReal(intVal, realVal);
  BooleanToString(true, strVal);
  CharToInteger('A', intVal);
  writeln('Converted values:');
  writeln('Integer: ', intVal);
  writeln('Real: ', realVal);
  writeln('String: ', strVal);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}