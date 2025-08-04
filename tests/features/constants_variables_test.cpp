#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"

TEST(ConstantsVariablesTest, BasicConstants) {
    std::string notalCode = R"(
PROGRAM ConstantsExample

KAMUS
  constant MAX: real = 100
  constant PI: real = 3.14159
  constant HELLO: string = "Hello, World!"
  constant ACTIVE: boolean = true

ALGORITMA
    output(HELLO)
    output(MAX)
    output(PI)
)";
    std::string expected_pascal_code = R"(program ConstantsExample;

const
  MAX = 100;
  PI = 3.14159;
  HELLO = 'Hello, World!';
  ACTIVE = true;

begin
  writeln(HELLO);
  writeln(MAX);
  writeln(PI);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(ConstantsVariablesTest, TypeInferredConstants) {
    std::string notalCode = R"(
PROGRAM TypeInferredExample

KAMUS
  constant COUNT = 50
  constant RATE = 2.5
  constant TITLE = "Welcome"
  constant ACTIVE = true

ALGORITMA
    output(COUNT)
    output(RATE)
    output(TITLE)
)";
    std::string expected_pascal_code = R"(program TypeInferredExample;

const
  COUNT = 50;
  RATE = 2.5;
  TITLE = 'Welcome';
  ACTIVE = true;

begin
  writeln(COUNT);
  writeln(RATE);
  writeln(TITLE);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(ConstantsVariablesTest, VariableDeclarations) {
    std::string notalCode = R"(
PROGRAM VariableDeclarationExample

KAMUS
    age: integer
    name: string
    isStudent: boolean
    gpa: real

ALGORITMA
    age <- 25
    name <- 'John Doe'
    isStudent <- true
    gpa <- 3.8

    output('Variable declaration and initialization example')
)";
    std::string expected_pascal_code = R"(program VariableDeclarationExample;

var
  age: integer;
  name: string;
  isStudent: boolean;
  gpa: real;

begin
  age := 25;
  name := 'John Doe';
  isStudent := true;
  gpa := 3.8;
  writeln('Variable declaration and initialization example');
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(ConstantsVariablesTest, MixedDeclarations) {
    std::string notalCode = R"(
PROGRAM MixedExample

KAMUS
    constant PI = 3.14159
    radius: real
    area: real

ALGORITMA
    radius <- 5.0
    area <- PI * radius * radius
    output('Area: ', area)
)";
    std::string expected_pascal_code = R"(program MixedExample;

const
  PI = 3.14159;

var
  radius: real;
  area: real;

begin
  radius := 5;
  area := ((PI * radius) * radius);
  writeln('Area: ', area);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}