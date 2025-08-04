#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"

TEST(ConditionalTest, BasicIfElse) {
    std::string notalCode = R"(
PROGRAM ConditionalExample

KAMUS
    score: integer

ALGORITMA
    score <- 85

    if (score >= 90) then
      output('Grade: A')
    elif (score >= 80) then
      output('Grade: B')
    else { score < 80 }
      output('Grade: C')
)";
    std::string expected_pascal_code = R"(program ConditionalExample;

var
  score: integer;

begin
  score := 85;
  if (score >= 90) then
    writeln('Grade: A')
  else if (score >= 80) then
    writeln('Grade: B')
  else
    writeln('Grade: C');
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(ConditionalTest, SimpleIfStatement) {
    std::string notalCode = R"(
PROGRAM SimpleIfExample

KAMUS
    x: integer

ALGORITMA
    x <- 10
    if (x > 5) then
        output('x is greater than 5')
)";
    std::string expected_pascal_code = R"(program SimpleIfExample;

var
  x: integer;

begin
  x := 10;
  if (x > 5) then
    writeln('x is greater than 5');
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(ConditionalTest, NestedConditionals) {
    std::string notalCode = R"(
PROGRAM NestedIfExample

KAMUS
    age: integer
    hasLicense: boolean

ALGORITMA
    age <- 20
    hasLicense <- true
    
    if (age >= 18) then
        if (hasLicense) then
            output('Can drive')
        else
            output('Need license')
    else
        output('Too young')
)";
    std::string expected_pascal_code = R"(program NestedIfExample;

var
  age: integer;
  hasLicense: boolean;

begin
  age := 20;
  hasLicense := true;
  if (age >= 18) then
    if (hasLicense) then
      writeln('Can drive')
    else
      writeln('Need license')
  else
    writeln('Too young');
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}