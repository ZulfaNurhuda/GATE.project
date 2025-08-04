#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"

TEST(InputOutputTest, BasicOutput) {
    std::string notalCode = R"(
PROGRAM OutputExample

KAMUS
    message: string
    
ALGORITMA
    message <- "Hello"
    output(message)
)";
    std::string expected_pascal_code = R"(program OutputExample;

var
  message: string;

begin
  message := 'Hello';
  writeln(message);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(InputOutputTest, BasicInput) {
    std::string notalCode = R"(
PROGRAM InputExample

KAMUS
    name: string

ALGORITMA
    output("Please enter your name:")
    input(name)
    output("Hello, ", name, "!")
)";
    std::string expected_pascal_code = R"(program InputExample;

var
  name: string;

begin
  writeln('Please enter your name:');
  readln(name);
  writeln('Hello, ', name, '!');
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(InputOutputTest, MultipleOutputParameters) {
    std::string notalCode = R"(
PROGRAM MultiOutputExample

KAMUS
    x: integer
    y: real
    
ALGORITMA
    x <- 10
    y <- 3.14
    output("Values: ", x, " and ", y)
)";
    std::string expected_pascal_code = R"(program MultiOutputExample;

var
  x: integer;
  y: real;

begin
  x := 10;
  y := 3.14;
  writeln('Values: ', x, ' and ', y);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}