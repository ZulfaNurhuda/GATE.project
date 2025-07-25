#include <gtest/gtest.h>
#include "test_helpers.h"
#include "notal_transpiler/Lexer.h"
#include "notal_transpiler/Parser.h"
#include "notal_transpiler/CodeGenerator.h"
#include <vector>
#include <string>

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

    // Use transpile from test_helpers.h
    std::string generated_pascal = transpile(source);

    // Parse the source to check AST structure
    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();
    notal::Parser parser(tokens);
    std::shared_ptr<notal::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    // Check that we have the expected declarations
    ASSERT_NE(program->kamus, nullptr);
    ASSERT_EQ(static_cast<long long>(program->kamus->declarations.size()), 2LL);

    // Check first declaration is a record type
    auto recordDecl = std::dynamic_pointer_cast<notal::ast::RecordTypeDeclStmt>(program->kamus->declarations[0]);
    ASSERT_NE(recordDecl, nullptr);
    EXPECT_EQ(recordDecl->typeName.lexeme, "Student");
    EXPECT_EQ(static_cast<long long>(recordDecl->fields.size()), 3LL);
    EXPECT_EQ(recordDecl->fields[0].name.lexeme, "name");
    EXPECT_EQ(recordDecl->fields[0].type.type, notal::TokenType::STRING);
    EXPECT_EQ(recordDecl->fields[1].name.lexeme, "age");
    EXPECT_EQ(recordDecl->fields[1].type.type, notal::TokenType::INTEGER);
    EXPECT_EQ(recordDecl->fields[2].name.lexeme, "gpa");
    EXPECT_EQ(recordDecl->fields[2].type.type, notal::TokenType::REAL);

    // Check that Pascal record is generated correctly
    EXPECT_TRUE(generated_pascal.find("Student = record") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("name: string;") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("age: integer;") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("gpa: real;") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("end;") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("student1: Student;") != std::string::npos);
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

    // Use transpile from test_helpers.h
    std::string generated_pascal = transpile(source);

    // Parse the source to check AST structure
    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();
    notal::Parser parser(tokens);
    std::shared_ptr<notal::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    // Check that we have the expected declarations
    ASSERT_NE(program->kamus, nullptr);
    ASSERT_EQ(static_cast<long long>(program->kamus->declarations.size()), 4LL);

    // Check first declaration is an enum type
    auto enumDecl1 = std::dynamic_pointer_cast<notal::ast::EnumTypeDeclStmt>(program->kamus->declarations[0]);
    ASSERT_NE(enumDecl1, nullptr);
    EXPECT_EQ(enumDecl1->typeName.lexeme, "Day");
    EXPECT_EQ(static_cast<long long>(enumDecl1->values.size()), 5LL);
    EXPECT_EQ(enumDecl1->values[0].lexeme, "monday");
    EXPECT_EQ(enumDecl1->values[4].lexeme, "friday");

    // Check second declaration is an enum type
    auto enumDecl2 = std::dynamic_pointer_cast<notal::ast::EnumTypeDeclStmt>(program->kamus->declarations[1]);
    ASSERT_NE(enumDecl2, nullptr);
    EXPECT_EQ(enumDecl2->typeName.lexeme, "Grade");
    EXPECT_EQ(static_cast<long long>(enumDecl2->values.size()), 5LL);
    EXPECT_EQ(enumDecl2->values[0].lexeme, "A");
    EXPECT_EQ(enumDecl2->values[4].lexeme, "F");

    // Check that Pascal enums are generated correctly
    EXPECT_TRUE(generated_pascal.find("Day = (monday, tuesday, wednesday, thursday, friday);") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("Grade = (A, B, C, D, F);") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("today: Day;") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("myGrade: Grade;") != std::string::npos);
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

    // Use transpile from test_helpers.h
    std::string generated_pascal = transpile(source);

    // Parse the source to check AST structure
    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();
    notal::Parser parser(tokens);
    std::shared_ptr<notal::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    // Check that we have the expected declarations
    ASSERT_NE(program->kamus, nullptr);
    ASSERT_EQ(static_cast<long long>(program->kamus->declarations.size()), 2LL);

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

    // Check that Pascal constraint setters are generated correctly
    EXPECT_TRUE(generated_pascal.find("procedure Setage(var age: integer; value: integer);") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("procedure Setscore(var score: real; value: real);") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("Assert(") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("value >= 0") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("value <= 150") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("value >= 0") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("value <= 100") != std::string::npos);
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

    // Use transpile from test_helpers.h
    std::string generated_pascal = transpile(source);

    // Parse the source to check AST structure
    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();
    notal::Parser parser(tokens);
    std::shared_ptr<notal::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    // Check that we have the expected declarations
    ASSERT_NE(program->kamus, nullptr);
    ASSERT_EQ(static_cast<long long>(program->kamus->declarations.size()), 5LL);

    // Check that all features work together
    EXPECT_TRUE(generated_pascal.find("Person = record") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("Status = (active, inactive, pending);") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("person1: Person;") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("currentStatus: Status;") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("validAge: integer;") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("procedure SetvalidAge") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("SetvalidAge(validAge, 25);") != std::string::npos);
}