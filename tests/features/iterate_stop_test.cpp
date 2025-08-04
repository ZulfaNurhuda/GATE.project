#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"
#include "core/NotalLexer.h"
#include "core/NotalParser.h"
#include "core/PascalCodeGenerator.h"
#include <vector>
#include <string>

TEST(IterateStopTest, BasicIterateStop) {
    std::string source = R"(
PROGRAM IterateStopTest
KAMUS
    command: string
ALGORITMA
    iterate
        output("Enter command:")
        input(command)
    stop (command = "quit")
        output("Processing: ", command)
)";

    std::string generated_pascal = transpile(source);
    
    // Check that iterate-stop is converted to appropriate Pascal loop
    EXPECT_TRUE(generated_pascal.find("command") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("quit") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("repeat") != std::string::npos || 
                generated_pascal.find("while") != std::string::npos);
}

TEST(IterateStopTest, IterateStopWithComplexCondition) {
    std::string source = R"(
PROGRAM ComplexIterateStopTest
KAMUS
    num: integer
    sum: integer
ALGORITMA
    sum <- 0
    iterate
        input(num)
        sum <- sum + num
    stop (num = 0 or sum > 100)
        output("Sum: ", sum)
)";

    std::string generated_pascal = transpile(source);
    
    // Check that complex condition is handled
    EXPECT_TRUE(generated_pascal.find("num") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("sum") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("100") != std::string::npos);
}

TEST(IterateStopTest, NestedIterateStop) {
    std::string source = R"(
PROGRAM NestedIterateStopTest
KAMUS
    outer: integer
    inner: integer
ALGORITMA
    outer <- 1
    iterate
        inner <- 1
        iterate
            output("Inner: ", inner)
            inner <- inner + 1
        stop (inner > 3)
        outer <- outer + 1
    stop (outer > 2)
)";

    std::string generated_pascal = transpile(source);
    
    // Check that nested iterate-stop is handled
    EXPECT_TRUE(generated_pascal.find("outer") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("inner") != std::string::npos);
}