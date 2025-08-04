#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"

TEST(WhileRepeatTest, BasicWhileLoop) {
    std::string notalCode = R"(
PROGRAM WhileExample

KAMUS
    counter: integer

ALGORITMA
    counter <- 1
    while (counter <= 5) do
        output('Counter: ', counter)
        counter <- counter + 1
)";
    std::string expected_pascal_code = R"(program WhileExample;

var
  counter: integer;

begin
  counter := 1;
  while (counter <= 5) do
  begin
    writeln('Counter: ', counter);
    counter := (counter + 1);
  end;
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(WhileRepeatTest, RepeatUntilLoop) {
    std::string notalCode = R"(
PROGRAM RepeatUntilExample

KAMUS
    x: integer

ALGORITMA
    x <- 1
    repeat
        output('x = ', x)
        x <- x + 1
    until (x > 3)
)";
    std::string expected_pascal_code = R"(program RepeatUntilExample;

var
  x: integer;

begin
  x := 1;
  repeat
    writeln('x = ', x);
    x := (x + 1);
  until (x > 3);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(WhileRepeatTest, RepeatNTimes) {
    std::string notalCode = R"(
PROGRAM RepeatNTimesExample

KAMUS
    i: integer
    sum: integer

ALGORITMA
    sum <- 0
    repeat 5 times
        i <- i + 1
        sum <- sum + i
        output('Step ', i, ': sum = ', sum)
)";
    std::string expected_pascal_code = R"(program RepeatNTimesExample;

var
  i: integer;
  sum: integer;
  _loop_iterator_0: integer;

begin
  sum := 0;
  for _loop_iterator_0 := 1 to 5 do
  begin
    i := (i + 1);
    sum := (sum + i);
    writeln('Step ', i, ': sum = ', sum);
  end;
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(WhileRepeatTest, NestedLoops) {
    std::string notalCode = R"(
PROGRAM NestedLoopsExample

KAMUS
    i: integer
    j: integer

ALGORITMA
    i <- 1
    while (i <= 3) do
        j <- 1
        while (j <= 2) do
            output('i=', i, ', j=', j)
            j <- j + 1
        i <- i + 1
)";
    std::string expected_pascal_code = R"(program NestedLoopsExample;

var
  i: integer;
  j: integer;

begin
  i := 1;
  while (i <= 3) do
  begin
    j := 1;
    while (j <= 2) do
    begin
      writeln('i=', i, ', j=', j);
      j := (j + 1);
    end;
    i := (i + 1);
  end;
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(WhileRepeatTest, LoopWithComplexCondition) {
    std::string notalCode = R"(
PROGRAM ComplexConditionExample

KAMUS
    x: integer
    y: integer
    found: boolean

ALGORITMA
    x <- 1
    y <- 10
    found <- false
    
    while (x < y and not found) do
        if (x * x = 25) then
            found <- true
        else
            x <- x + 1
            
    output('Result: x=', x, ', found=', found)
)";
    std::string expected_pascal_code = R"(program ComplexConditionExample;

var
  x: integer;
  y: integer;
  found: boolean;

begin
  x := 1;
  y := 10;
  found := false;
  while ((x < y) and (not found)) do
  begin
    if ((x * x) = 25) then
    begin
      found := true;
    end
    else
    begin
      x := (x + 1);
    end;
  end;
  writeln('Result: x=', x, ', found=', found);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}