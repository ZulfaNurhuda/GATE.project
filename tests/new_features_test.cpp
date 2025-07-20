#include <gtest/gtest.h>
#include "notal_transpiler/Lexer.h"
#include "notal_transpiler/Parser.h"
#include "notal_transpiler/CodeGenerator.h"
#include <vector>
#include <string>
#include <regex>
#include <algorithm>
#include <cctype>

// Helper function to normalize whitespace and remove case sensitivity for comparison
std::string normalizeCode(const std::string& s) {
    std::string result = s;
    // Convert to lower case
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    // Remove newlines, tabs, and carriage returns
    result = std::regex_replace(result, std::regex("[\n\t\r]"), " ");
    // Replace multiple spaces with a single space
    result = std::regex_replace(result, std::regex(" +"), " ");
    // Trim leading/trailing space
    result = std::regex_replace(result, std::regex("^ +| +$"), "");
    return result;
}

TEST(NewFeaturesTest, RecordTypeDeclaration) {
    std::string source = R"(
PROGRAM RecordTest
KAMUS
    type Student: <
        name: string,
        age: integer,
        gpa: real
    >
    
    student1: Student
    
ALGORITMA
    output('Record declared')
)";

    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();

    notal::Parser parser(tokens);
    std::shared_ptr<notal::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    // Check that we have the expected declarations
    ASSERT_NE(program->kamus, nullptr);
    ASSERT_EQ(program->kamus->declarations.size(), 2);

    // Check first declaration is a record type
    auto recordDecl = std::dynamic_pointer_cast<notal::ast::RecordTypeDeclStmt>(program->kamus->declarations[0]);
    ASSERT_NE(recordDecl, nullptr);
    EXPECT_EQ(recordDecl->typeName.lexeme, "Student");
    EXPECT_EQ(recordDecl->fields.size(), 3);
    EXPECT_EQ(recordDecl->fields[0].name.lexeme, "name");
    EXPECT_EQ(recordDecl->fields[0].type.type, notal::TokenType::STRING);
    EXPECT_EQ(recordDecl->fields[1].name.lexeme, "age");
    EXPECT_EQ(recordDecl->fields[1].type.type, notal::TokenType::INTEGER);
    EXPECT_EQ(recordDecl->fields[2].name.lexeme, "gpa");
    EXPECT_EQ(recordDecl->fields[2].type.type, notal::TokenType::REAL);

    // Generate Pascal code
    notal::CodeGenerator generator;
    std::string result = generator.generate(program);

    // Check that Pascal record is generated correctly
    EXPECT_TRUE(result.find("Student = record") != std::string::npos);
    EXPECT_TRUE(result.find("name: string;") != std::string::npos);
    EXPECT_TRUE(result.find("age: integer;") != std::string::npos);
    EXPECT_TRUE(result.find("gpa: real;") != std::string::npos);
    EXPECT_TRUE(result.find("end;") != std::string::npos);
    EXPECT_TRUE(result.find("student1: Student;") != std::string::npos);
}

TEST(NewFeaturesTest, EnumTypeDeclaration) {
    std::string source = R"(
PROGRAM EnumTest
KAMUS
    type Day: (monday, tuesday, wednesday, thursday, friday)
    type Grade: (A, B, C, D, F)
    
    today: Day
    myGrade: Grade
    
ALGORITMA
    output('Enums declared')
)";

    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();

    notal::Parser parser(tokens);
    std::shared_ptr<notal::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    // Check that we have the expected declarations
    ASSERT_NE(program->kamus, nullptr);
    ASSERT_EQ(program->kamus->declarations.size(), 4);

    // Check first declaration is an enum type
    auto enumDecl1 = std::dynamic_pointer_cast<notal::ast::EnumTypeDeclStmt>(program->kamus->declarations[0]);
    ASSERT_NE(enumDecl1, nullptr);
    EXPECT_EQ(enumDecl1->typeName.lexeme, "Day");
    EXPECT_EQ(enumDecl1->values.size(), 5);
    EXPECT_EQ(enumDecl1->values[0].lexeme, "monday");
    EXPECT_EQ(enumDecl1->values[4].lexeme, "friday");

    // Check second declaration is an enum type
    auto enumDecl2 = std::dynamic_pointer_cast<notal::ast::EnumTypeDeclStmt>(program->kamus->declarations[1]);
    ASSERT_NE(enumDecl2, nullptr);
    EXPECT_EQ(enumDecl2->typeName.lexeme, "Grade");
    EXPECT_EQ(enumDecl2->values.size(), 5);
    EXPECT_EQ(enumDecl2->values[0].lexeme, "A");
    EXPECT_EQ(enumDecl2->values[4].lexeme, "F");

    // Generate Pascal code
    notal::CodeGenerator generator;
    std::string result = generator.generate(program);

    // Check that Pascal enums are generated correctly
    EXPECT_TRUE(result.find("Day = (monday, tuesday, wednesday, thursday, friday);") != std::string::npos);
    EXPECT_TRUE(result.find("Grade = (A, B, C, D, F);") != std::string::npos);
    EXPECT_TRUE(result.find("today: Day;") != std::string::npos);
    EXPECT_TRUE(result.find("myGrade: Grade;") != std::string::npos);
}

TEST(NewFeaturesTest, ConstrainedVariableDeclaration) {
    std::string source = R"(
PROGRAM ConstraintTest
KAMUS
    age: integer | age >= 0 and age <= 150
    score: real | score >= 0.0 and score <= 100.0
    
ALGORITMA
    output('Constraints declared')
)";

    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();

    notal::Parser parser(tokens);
    std::shared_ptr<notal::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    // Check that we have the expected declarations
    ASSERT_NE(program->kamus, nullptr);
    ASSERT_EQ(program->kamus->declarations.size(), 2);

    // Check first declaration is a constrained variable
    auto constrainedVar1 = std::dynamic_pointer_cast<notal::ast::ConstrainedVarDeclStmt>(program->kamus->declarations[0]);
    ASSERT_NE(constrainedVar1, nullptr);
    EXPECT_EQ(constrainedVar1->name.lexeme, "age");
    EXPECT_EQ(constrainedVar1->type.type, notal::TokenType::INTEGER);
    ASSERT_NE(constrainedVar1->constraint, nullptr);

    // Check second declaration is a constrained variable
    auto constrainedVar2 = std::dynamic_pointer_cast<notal::ast::ConstrainedVarDeclStmt>(program->kamus->declarations[1]);
    ASSERT_NE(constrainedVar2, nullptr);
    EXPECT_EQ(constrainedVar2->name.lexeme, "score");
    EXPECT_EQ(constrainedVar2->type.type, notal::TokenType::REAL);
    ASSERT_NE(constrainedVar2->constraint, nullptr);

    // Generate Pascal code
    notal::CodeGenerator generator;
    std::string result = generator.generate(program);

    // Check that Pascal constraint setters are generated correctly
    EXPECT_TRUE(result.find("procedure Setage(var age: integer; value: integer);") != std::string::npos);
    EXPECT_TRUE(result.find("procedure Setscore(var score: real; value: real);") != std::string::npos);
    EXPECT_TRUE(result.find("Assert(") != std::string::npos);
    EXPECT_TRUE(result.find("age >= 0") != std::string::npos);
    EXPECT_TRUE(result.find("age <= 150") != std::string::npos);
    EXPECT_TRUE(result.find("score >= 0") != std::string::npos);
    EXPECT_TRUE(result.find("score <= 100") != std::string::npos);
}

TEST(NewFeaturesTest, ComplexExample) {
    std::string source = R"(
PROGRAM ComplexExample
KAMUS
    type Person: <
        name: string,
        age: integer
    >
    type Status: (active, inactive, pending)
    
    person1: Person
    currentStatus: Status
    validAge: integer | validAge >= 0 and validAge <= 120
    
ALGORITMA
    validAge <- 25
    output('Complex example works')
)";

    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();

    notal::Parser parser(tokens);
    std::shared_ptr<notal::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    // Generate Pascal code
    notal::CodeGenerator generator;
    std::string result = generator.generate(program);

    // Check that all features work together
    EXPECT_TRUE(result.find("Person = record") != std::string::npos);
    EXPECT_TRUE(result.find("Status = (active, inactive, pending);") != std::string::npos);
    EXPECT_TRUE(result.find("person1: Person;") != std::string::npos);
    EXPECT_TRUE(result.find("currentStatus: Status;") != std::string::npos);
    EXPECT_TRUE(result.find("validAge: integer;") != std::string::npos);
    EXPECT_TRUE(result.find("procedure SetvalidAge") != std::string::npos);
    EXPECT_TRUE(result.find("validAge := 25;") != std::string::npos);
}