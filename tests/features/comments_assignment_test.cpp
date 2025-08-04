#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"

TEST(CommentsAssignmentTest, SingleLineComments) {
    std::string notalCode = R"(
PROGRAM CommentsExample

{ This is a single line comment }
KAMUS
    x: integer  { Variable declaration comment }
    y: real

ALGORITMA
    { Initialize variables }
    x <- 10
    y <- 3.14  { Assign pi approximation }
    
    { Output results }
    output('x = ', x)
    output('y = ', y)
)";
    std::string expected_pascal_code = R"(program CommentsExample;

var
  x: integer;
  y: real;

begin
  x := 10;
  y := 3.14;
  writeln('x = ', x);
  writeln('y = ', y);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(CommentsAssignmentTest, MultiLineComments) {
    std::string notalCode = R"(
PROGRAM MultiLineCommentsExample

{
  This is a multi-line comment
  that spans several lines
  and explains the program purpose
}

KAMUS
    result: integer

ALGORITMA
    {
      Calculate some value
      using complex logic
    }
    result <- 42
    output('The answer is: ', result)
)";
    std::string expected_pascal_code = R"(program MultiLineCommentsExample;

var
  result: integer;

begin
  result := 42;
  writeln('The answer is: ', result);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(CommentsAssignmentTest, BasicAssignment) {
    std::string notalCode = R"(
PROGRAM AssignmentExample

KAMUS
    a: integer
    b: real
    c: string
    d: boolean
    e: character

ALGORITMA
    a <- 100
    b <- 25.5
    c <- "Hello World"
    d <- true
    e <- 'X'
    
    output('Integer: ', a)
    output('Real: ', b)
    output('String: ', c)
    output('Boolean: ', d)
    output('Character: ', e)
)";
    std::string expected_pascal_code = R"(program AssignmentExample;

var
  a: integer;
  b: real;
  c: string;
  d: boolean;
  e: char;

begin
  a := 100;
  b := 25.5;
  c := 'Hello World';
  d := true;
  e := 'X';
  writeln('Integer: ', a);
  writeln('Real: ', b);
  writeln('String: ', c);
  writeln('Boolean: ', d);
  writeln('Character: ', e);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(CommentsAssignmentTest, ComplexAssignment) {
    std::string notalCode = R"(
PROGRAM ComplexAssignmentExample

KAMUS
    x: integer
    y: integer
    temp: integer
    sum: integer
    product: integer

ALGORITMA
    { Initialize values }
    x <- 15
    y <- 25
    
    { Swap values using temporary variable }
    temp <- x
    x <- y
    y <- temp
    
    { Calculate sum and product }
    sum <- x + y
    product <- x * y
    
    output('After swap: x=', x, ', y=', y)
    output('Sum: ', sum)
    output('Product: ', product)
)";
    std::string expected_pascal_code = R"(program ComplexAssignmentExample;

var
  x: integer;
  y: integer;
  temp: integer;
  sum: integer;
  product: integer;

begin
  x := 15;
  y := 25;
  temp := x;
  x := y;
  y := temp;
  sum := (x + y);
  product := (x * y);
  writeln('After swap: x=', x, ', y=', y);
  writeln('Sum: ', sum);
  writeln('Product: ', product);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}