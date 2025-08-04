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
uses SysUtils;

var
  myInt: integer;
  myStr: string;
  myBool: boolean;
  success: boolean;

procedure IntegerToString(inInt: Int64; var outStr: string); forward;
procedure IntegerToString(inInt: integer; var outStr: string); forward;
function StringToBoolean(const inStr: string; var outBool: boolean): boolean; forward;

procedure IntegerToString(inInt: Int64; var outStr: string);
begin
  outStr := IntToStr(inInt);
end;

procedure IntegerToString(inInt: integer; var outStr: string);
begin
  IntegerToString(Int64(inInt), outStr);
end;

function StringToBoolean(const inStr: string; var outBool: boolean): boolean;
var
  lowerS: string;
  numValue: Int64;
  code: integer;
begin
  lowerS := LowerCase(Trim(inStr));
  Val(lowerS, numValue, code);
  if code = 0 then
  begin
    outBool := (numValue <> 0);
    StringToBoolean := true;
  end
  else if (lowerS = 'true') then
  begin
    outBool := true;
    StringToBoolean := true;
  end
  else if (lowerS = 'false') then
  begin
    outBool := false;
    StringToBoolean := true;
  end
  else
  begin
    outBool := false;
    StringToBoolean := false;
  end;
end;

begin
  myInt := 123;
  IntegerToString(myInt, myStr);
  writeln('The integer as a string is: ', myStr);
  success := StringToBoolean('True', myBool);
  if success then
  begin
    writeln('StringToBoolean was successful. The value is: ', myBool);
  end
  else
  begin
    writeln('StringToBoolean failed.');
  end;
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
  c := (a + (b * 2));
  d := ((a + b) / 2);
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
  result := (((x * y) + 5) / (x - y));
  isPositive := (result > 0);
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
uses SysUtils;

var
  intVal: integer;
  realVal: real;
  strVal: string;
  boolVal: boolean;
  charVal: char;

procedure BooleanToString(inBool: Boolean; var outStr: string); forward;
function CharToInteger(inChar: Char; var outInt: Integer): Boolean; forward;
procedure IntegerToReal(inInt: Integer; var outReal: Real); forward;
procedure IntegerToReal(inInt: Int64; var outReal: Double); forward;
procedure IntegerToReal(inInt: Integer; var outReal: Double); forward;
procedure RealToInteger(inReal: Double; var outInt: Int64); forward;
procedure RealToInteger(inReal: Real; var outInt: Integer); forward;
procedure RealToInteger(inReal: Real; var outInt: Int64); forward;
procedure RealToInteger(inReal: Double; var outInt: Integer); forward;

procedure BooleanToString(inBool: Boolean; var outStr: string);
begin
  if inBool then
    outStr := 'True'
  else
    outStr := 'False';
end;

function CharToInteger(inChar: Char; var outInt: Integer): Boolean;
begin
  if (inChar >= '0') and (inChar <= '9') then
  begin
    outInt := Ord(inChar) - Ord('0');
    Result := True;
  end
  else
  begin
    outInt := 0;
    Result := False;
  end;
end;

procedure IntegerToReal(inInt: Integer; var outReal: Real);
begin
  outReal := inInt;
end;

procedure IntegerToReal(inInt: Int64; var outReal: Double);
begin
  outReal := inInt;
end;

procedure IntegerToReal(inInt: Integer; var outReal: Double);
begin
  outReal := inInt;
end;

procedure RealToInteger(inReal: Double; var outInt: Int64);
begin
  outInt := Round(inReal);
end;

procedure RealToInteger(inReal: Real; var outInt: Integer);
begin
  outInt := Round(inReal);
end;

procedure RealToInteger(inReal: Real; var outInt: Int64);
begin
  outInt := Round(inReal);
end;

procedure RealToInteger(inReal: Double; var outInt: Integer);
var
  tempInt64: Int64;
begin
  tempInt64 := Round(inReal);
  if (tempInt64 >= Low(Integer)) and (tempInt64 <= High(Integer)) then
    outInt := Integer(tempInt64)
  else
    outInt := 0;
end;

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