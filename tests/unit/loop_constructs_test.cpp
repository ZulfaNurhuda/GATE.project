#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"

TEST(LoopConstructsTest, TraversalLoopSimple) {
    std::string notal_code = R"(PROGRAM TraversalTest
KAMUS
    i: integer
ALGORITMA
    i traversal [1..5]
        output(i)
)";
    std::string expected_pascal = R"(program TraversalTest;
var
  i: integer;
begin
  i := 1;
  while (i <= 5) do
  begin
    writeln(i);
    Inc(i, 1);
  end;
end.
)";
    std::string generated_pascal = transpile(notal_code);
    EXPECT_EQ(normalizeCode(generated_pascal), normalizeCode(expected_pascal));
}

TEST(LoopConstructsTest, TraversalLoopWithStep) {
    std::string notal_code = R"(PROGRAM TraversalStepTest
KAMUS
    i: integer
ALGORITMA
    i traversal [1..10 step 2]
        output(i)
)";
    std::string expected_pascal = R"(program TraversalStepTest;
var
  i: integer;
begin
  i := 1;
  while (i <= 10) do
  begin
    writeln(i);
    Inc(i, 2);
  end;
end.
)";
    std::string generated_pascal = transpile(notal_code);
    EXPECT_EQ(normalizeCode(generated_pascal), normalizeCode(expected_pascal));
}

TEST(LoopConstructsTest, IterateStopLoop) {
    std::string notal_code = R"(PROGRAM IterateStopTest
KAMUS
    command: string
ALGORITMA
    iterate
        input(command)
    stop (command = "quit")
)";
    std::string expected_pascal = R"(program IterateStopTest;
var
  command: string;
begin
  while true do
  begin
    readln(command);
    if (command = 'quit') then break;
  end;
end.
)";
    std::string generated_pascal = transpile(notal_code);
    EXPECT_EQ(normalizeCode(generated_pascal), normalizeCode(expected_pascal));
}

TEST(LoopConstructsTest, RepeatNTimesLoop) {
    std::string notal_code = R"(PROGRAM RepeatNTimesTest
KAMUS
ALGORITMA
    repeat 5 times
        output("Hello")
)";
    std::string expected_pascal = R"(program RepeatNTimesTest;
var
  _loop_iterator_0: integer;
begin
  for _loop_iterator_0 := 1 to 5 do
  begin
    writeln('Hello');
  end;
end.
)";
    std::string generated_pascal = transpile(notal_code);
    EXPECT_EQ(normalizeCode(generated_pascal), normalizeCode(expected_pascal));
}
