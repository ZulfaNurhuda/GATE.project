#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"
#include "core/NotalLexer.h"
#include "core/NotalParser.h"
#include "core/PascalCodeGenerator.h"
#include <vector>
#include <string>

TEST(DynamicArrayTest, BasicDynamicArray) {
    std::string source = R"(
PROGRAM DynamicArrayTest
KAMUS
    dynArray: array of integer
    size: integer
ALGORITMA
    size <- 5
    allocate(dynArray, size)
    dynArray[0] <- 10
    output(dynArray[0])
    deallocate(dynArray)
)";

    std::string generated_pascal = transpile(source);
    
    // Check that dynamic array operations are handled
    EXPECT_TRUE(generated_pascal.find("dynArray") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("size") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("5") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("10") != std::string::npos);
}

TEST(DynamicArrayTest, TwoDimensionalDynamicArray) {
    std::string source = R"(
PROGRAM DynamicMatrix
KAMUS
    dynMatrix: array of array of integer
    rows, cols: integer
    i, j: integer
ALGORITMA
    rows <- 3
    cols <- 4
    allocate(dynMatrix, rows, cols)
    i traversal [0..rows-1]
        j traversal [0..cols-1]
            dynMatrix[i][j] <- (i + 1) * (j + 1)
    deallocate[2](dynMatrix)
)";

    std::string generated_pascal = transpile(source);
    
    // Check that 2D dynamic array is handled
    EXPECT_TRUE(generated_pascal.find("dynMatrix") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("rows") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("cols") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("3") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("4") != std::string::npos);
}

TEST(DynamicArrayTest, DynamicArrayWithTraversal) {
    std::string source = R"(
PROGRAM DynamicArrayTraversal
KAMUS
    dynArray: array of real
    size: integer
    i: integer
ALGORITMA
    size <- 10
    allocate(dynArray, size)
    i traversal [0..size-1]
        dynArray[i] <- i * 2.5
    i traversal [0..size-1]
        output("dynArray[", i, "] = ", dynArray[i])
    deallocate(dynArray)
)";

    std::string generated_pascal = transpile(source);
    
    // Check that dynamic array with traversal is handled
    EXPECT_TRUE(generated_pascal.find("dynArray") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("real") != std::string::npos || 
                generated_pascal.find("Real") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("2.5") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("10") != std::string::npos);
}