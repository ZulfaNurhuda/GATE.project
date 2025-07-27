#include <gtest/gtest.h>
#include "test_helpers.h"

TEST(LoopControlTest, StopAndSkip) {
    std::string notal_code = R"(PROGRAM LoopControlExample
{ Example demonstrating 'stop' and 'skip' loop control }

KAMUS
  i: integer
  sum: integer

ALGORITMA
  sum <- 0
  i traversal [1..10]
    if i = 3 then
      skip
    if i = 8 then
      stop
    sum <- sum + i
    output('Current i: ', i, ', Current sum: ', sum)
  
  output('Final sum: ', sum)
)";
    std::string expected_pascal = R"(program LoopControlExample;

var
  i: integer;
  sum: integer;

begin
  sum := 0;
  i := 1;
  while (i <= 10) do
  begin
    if (i = 3) then
    begin
      continue;
    end;
    if (i = 8) then
    begin
      break;
    end;
    sum := (sum + i);
    writeln('Current i: ', i, ', Current sum: ', sum);
    Inc(i, 1);
  end;
  writeln('Final sum: ', sum);
end.
)";
    std::string generated_pascal = transpile(notal_code);
    EXPECT_EQ(normalizeCode(generated_pascal), normalizeCode(expected_pascal));
}