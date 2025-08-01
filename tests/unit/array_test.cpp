#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"

TEST(ArrayTest, StaticArrayDeclaration) {
    std::string notalCode = R"(
PROGRAM StaticArrayTest
KAMUS
    matrix: array[1..10][1..5] of integer
    cube: array[0..2][0..2][0..2] of real
ALGORITMA
    matrix[1][1] <- 10
)";
    std::string expected_pascal_code = R"(program StaticArrayTest;

var
  matrix: array[1..10, 1..5] of integer;
  cube: array[0..2, 0..2, 0..2] of real;

begin
  matrix[1, 1] := 10;
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(ArrayTest, DynamicArrayDeclarationAndAllocation) {
    std::string notalCode = R"(
PROGRAM DynamicArrayTest
KAMUS
    data1D: array of integer
    data2D: array of array of real
    data3D: array of array of array of boolean
ALGORITMA
    allocate(data1D, 10)
    allocate(data2D, 5, 5)
    allocate(data3D, 2, 3, 4)
)";
    std::string expected_pascal_code = R"(program DynamicArrayTest;

var
  data1D: array of integer;
  data2D: array of array of real;
  data3D: array of array of array of boolean;

begin
  SetLength(data1D, 10);
  SetLength(data2D, 5, 5);
  SetLength(data3D, 2, 3, 4);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(ArrayTest, ArrayAccessAndAssignment) {
    std::string notalCode = R"(
PROGRAM ArrayAccessTest
KAMUS
    staticArr: array[0..4] of integer
    dynArr: array of string
ALGORITMA
    allocate(dynArr, 5)
    staticArr[0] <- 100
    staticArr[1] <- staticArr[0] * 2
    dynArr[4] <- "Hello"
    output(staticArr[1])
    output(dynArr[4])
)";
    std::string expected_pascal_code = R"(program ArrayAccessTest;

var
  staticArr: array[0..4] of integer;
  dynArr: array of string;

begin
  SetLength(dynArr, 5);
  staticArr[0] := 100;
  staticArr[1] := (staticArr[0] * 2);
  dynArr[4] := 'Hello';
  writeln(staticArr[1]);
  writeln(dynArr[4]);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(ArrayTest, MultiDimensionalAccess) {
    std::string notalCode = R"(
PROGRAM MultiDimTest
KAMUS
    matrix: array[1..2][1..2] of integer
ALGORITMA
    matrix[1][1] <- 1
    matrix[1][2] <- 2
    matrix[2][1] <- 3
    matrix[2][2] <- matrix[1][1] + matrix[1][2] + matrix[2][1]
    output(matrix[2][2])
)";
    std::string expected_pascal_code = R"(program MultiDimTest;

var
  matrix: array[1..2, 1..2] of integer;

begin
  matrix[1, 1] := 1;
  matrix[1, 2] := 2;
  matrix[2, 1] := 3;
  matrix[2, 2] := ((matrix[1, 1] + matrix[1, 2]) + matrix[2, 1]);
  writeln(matrix[2, 2]);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(ArrayTest, Deallocation) {
    std::string notalCode = R"(
PROGRAM DeallocationTest
KAMUS
    data1D: array of integer
    data2D: array of array of integer
    ptr: pointer to integer
ALGORITMA
    allocate(data1D, 10)
    deallocate[1](data1D)

    allocate(data2D, 5, 5)
    deallocate[2](data2D)

    allocate(ptr)
    deallocate(ptr)
)";
    std::string expected_pascal_code = R"(program DeallocationTest;

var
  data1D: array of integer;
  data2D: array of array of integer;
  ptr: ^integer;

begin
  SetLength(data1D, 10);
  SetLength(data1D, 0);
  SetLength(data2D, 5, 5);
  SetLength(data2D, 0, 0);
  New(ptr);
  Dispose(ptr);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(ArrayTest, DeallocationMismatchError) {
    std::string notalCode = R"(
PROGRAM DeallocationMismatchTest
KAMUS
    data2D: array of array of integer
ALGORITMA
    allocate(data2D, 5, 5)
    deallocate[3](data2D)
)";
    ASSERT_THROW(transpile(notalCode), std::runtime_error);
}

TEST(ArrayTest, DeallocationWithEmptyBrackets) {
    std::string notalCode = R"(
PROGRAM DeallocEmptyBracketTest
KAMUS
    data1D: array of integer
ALGORITMA
    allocate(data1D, 10)
    deallocate[](data1D)
)";
    std::string expected_pascal_code = R"(program DeallocEmptyBracketTest;

var
  data1D: array of integer;

begin
  SetLength(data1D, 10);
  SetLength(data1D, 0);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}
