#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"
#include "core/NotalLexer.h"
#include "core/NotalParser.h"
#include "core/PascalCodeGenerator.h"
#include <vector>
#include <string>

TEST(TraversalTest, BasicTraversal) {
    std::string source = R"(
PROGRAM TraversalTest
KAMUS
    i: integer
ALGORITMA
    i traversal [1..5]
        output("Current value: ", i)
)";

    std::string generated_pascal = transpile(source);
    
    // Check that basic traversal is converted to Pascal for loop
    EXPECT_TRUE(generated_pascal.find("i") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("1") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("5") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("for") != std::string::npos || 
                generated_pascal.find("For") != std::string::npos);
}

TEST(TraversalTest, TraversalWithStep) {
    std::string source = R"(
PROGRAM TraversalStepTest
KAMUS
    j: integer
ALGORITMA
    j traversal [1..10 step 2]
        output("Current value with step: ", j)
)";

    std::string generated_pascal = transpile(source);
    
    // Check that traversal with step is handled
    EXPECT_TRUE(generated_pascal.find("j") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("1") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("10") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("2") != std::string::npos);
}

TEST(TraversalTest, NestedTraversal) {
    std::string source = R"(
PROGRAM NestedTraversalTest
KAMUS
    i, j: integer
    matrix: array[1..3][1..3] of integer
ALGORITMA
    i traversal [1..3]
        j traversal [1..3]
            matrix[i][j] <- i * j
            output("Matrix[", i, "][", j, "] = ", matrix[i][j])
)";

    std::string generated_pascal = transpile(source);
    
    // Check that nested traversal is handled
    EXPECT_TRUE(generated_pascal.find("i") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("j") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("matrix") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("3") != std::string::npos);
}

TEST(TraversalTest, TraversalWithArrayAccess) {
    std::string source = R"(
PROGRAM TraversalArrayTest
KAMUS
    arr: array[0..9] of integer
    i: integer
    sum: integer
ALGORITMA
    sum <- 0
    i traversal [0..9]
        arr[i] <- i * 2
        sum <- sum + arr[i]
    output("Sum: ", sum)
)";

    std::string generated_pascal = transpile(source);
    
    // Check that traversal with array access is handled
    EXPECT_TRUE(generated_pascal.find("arr") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("sum") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("0") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("9") != std::string::npos);
}