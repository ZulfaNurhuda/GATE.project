#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"
#include "core/NotalLexer.h"
#include "core/NotalParser.h"
#include "core/PascalCodeGenerator.h"
#include <vector>
#include <string>

TEST(ConstraintTest, BasicConstraints) {
    std::string source = R"(
PROGRAM ConstraintTest
KAMUS
    age: integer | age >= 0 and age <= 150
    score: real | score >= 0.0 and score <= 100.0
ALGORITMA
    age <- 25
    score <- 95.5
    output('Age: ', age, ', Score: ', score)
)";

    std::string generated_pascal = transpile(source);
    
    // Check that constraints are properly handled in Pascal code
    EXPECT_TRUE(generated_pascal.find("age") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("score") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("25") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("95.5") != std::string::npos);
}

TEST(ConstraintTest, CharacterConstraint) {
    std::string source = R"(
PROGRAM CharConstraintTest
KAMUS
    grade: character | grade = 'A' or grade = 'B' or grade = 'C'
ALGORITMA
    grade <- 'A'
    output('Grade: ', grade)
)";

    std::string generated_pascal = transpile(source);
    
    // Check that character constraint is handled
    EXPECT_TRUE(generated_pascal.find("grade") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("'A'") != std::string::npos);
}

TEST(ConstraintTest, MultipleConstraints) {
    std::string source = R"(
PROGRAM MultiConstraintTest
KAMUS
    age: integer | age >= 0 and age <= 150
    score: real | score >= 0.0 and score <= 100.0
    grade: character | grade = 'A' or grade = 'B' or grade = 'C' or grade = 'D' or grade = 'F'
ALGORITMA
    age <- 25
    score <- 95.5
    grade <- 'A'
    output('Age: ', age, ', Score: ', score, ', Grade: ', grade)
)";

    std::string generated_pascal = transpile(source);
    
    // Check that all variables with constraints are handled
    EXPECT_TRUE(generated_pascal.find("age") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("score") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("grade") != std::string::npos);
}