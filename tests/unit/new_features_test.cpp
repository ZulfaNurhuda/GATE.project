#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"
#include "gate/transpiler/NotalLexer.h"
#include "gate/transpiler/NotalParser.h"
#include "gate/transpiler/PascalCodeGenerator.h"
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

    std::string generated_pascal = transpile(source);

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    std::shared_ptr<gate::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    ASSERT_NE(program->kamus, nullptr);
    ASSERT_EQ(static_cast<long long>(program->kamus->declarations.size()), 2LL);

    auto recordDecl = std::dynamic_pointer_cast<gate::ast::RecordTypeDeclStmt>(program->kamus->declarations[0]);
    ASSERT_NE(recordDecl, nullptr);
    EXPECT_EQ(recordDecl->typeName.lexeme, "Student");
    EXPECT_EQ(static_cast<long long>(recordDecl->fields.size()), 3LL);
    EXPECT_EQ(recordDecl->fields[0].name.lexeme, "name");
    EXPECT_EQ(recordDecl->fields[0].type.type, gate::core::TokenType::STRING);
    EXPECT_EQ(recordDecl->fields[1].name.lexeme, "age");
    EXPECT_EQ(recordDecl->fields[1].type.type, gate::core::TokenType::INTEGER);
    EXPECT_EQ(recordDecl->fields[2].name.lexeme, "gpa");
    EXPECT_EQ(recordDecl->fields[2].type.type, gate::core::TokenType::REAL);

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

    std::string generated_pascal = transpile(source);

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    std::shared_ptr<gate::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    ASSERT_NE(program->kamus, nullptr);
    ASSERT_EQ(static_cast<long long>(program->kamus->declarations.size()), 4LL);

    auto enumDecl1 = std::dynamic_pointer_cast<gate::ast::EnumTypeDeclStmt>(program->kamus->declarations[0]);
    ASSERT_NE(enumDecl1, nullptr);
    EXPECT_EQ(enumDecl1->typeName.lexeme, "Day");
    EXPECT_EQ(static_cast<long long>(enumDecl1->values.size()), 5LL);
    EXPECT_EQ(enumDecl1->values[0].lexeme, "monday");
    EXPECT_EQ(enumDecl1->values[4].lexeme, "friday");

    auto enumDecl2 = std::dynamic_pointer_cast<gate::ast::EnumTypeDeclStmt>(program->kamus->declarations[1]);
    ASSERT_NE(enumDecl2, nullptr);
    EXPECT_EQ(enumDecl2->typeName.lexeme, "Grade");
    EXPECT_EQ(static_cast<long long>(enumDecl2->values.size()), 5LL);
    EXPECT_EQ(enumDecl2->values[0].lexeme, "A");
    EXPECT_EQ(enumDecl2->values[4].lexeme, "F");

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

    std::string generated_pascal = transpile(source);

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    std::shared_ptr<gate::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    ASSERT_NE(program->kamus, nullptr);
    ASSERT_EQ(static_cast<long long>(program->kamus->declarations.size()), 2LL);

    auto constrainedVar1 = std::dynamic_pointer_cast<gate::ast::ConstrainedVarDeclStmt>(program->kamus->declarations[0]);
    ASSERT_NE(constrainedVar1, nullptr);
    EXPECT_EQ(constrainedVar1->name.lexeme, "age");
    EXPECT_EQ(constrainedVar1->type.type, gate::core::TokenType::INTEGER);
    ASSERT_NE(constrainedVar1->constraint, nullptr);

    auto constrainedVar2 = std::dynamic_pointer_cast<gate::ast::ConstrainedVarDeclStmt>(program->kamus->declarations[1]);
    ASSERT_NE(constrainedVar2, nullptr);
    EXPECT_EQ(constrainedVar2->name.lexeme, "score");
    EXPECT_EQ(constrainedVar2->type.type, gate::core::TokenType::REAL);
    ASSERT_NE(constrainedVar2->constraint, nullptr);

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

    std::string generated_pascal = transpile(source);

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    std::shared_ptr<gate::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);

    ASSERT_NE(program->kamus, nullptr);
    ASSERT_EQ(static_cast<long long>(program->kamus->declarations.size()), 5LL);

    EXPECT_TRUE(generated_pascal.find("Person = record") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("Status = (active, inactive, pending);") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("person1: Person;") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("currentStatus: Status;") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("validAge: integer;") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("procedure SetvalidAge") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("SetvalidAge(validAge, 25);") != std::string::npos);
}

TEST(NewFeaturesTest, XorOperator) {
    std::string source = R"(
PROGRAM XorTest
KAMUS
    a: boolean
    b: boolean
    c: boolean
ALGORITMA
    a <- true
    b <- false
    c <- a xor b
)";

    std::string generated_pascal = transpile(source);

    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, "test");
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    std::shared_ptr<gate::ast::ProgramStmt> program = parser.parse();
    ASSERT_NE(program, nullptr);
    ASSERT_NE(program->algoritma, nullptr);
    ASSERT_NE(program->algoritma->body, nullptr);
    ASSERT_EQ(static_cast<long long>(program->algoritma->body->statements.size()), 3LL);

    auto exprStmt = std::dynamic_pointer_cast<gate::ast::ExpressionStmt>(program->algoritma->body->statements[2]);
    ASSERT_NE(exprStmt, nullptr);
    auto assignment = std::dynamic_pointer_cast<gate::ast::Assign>(exprStmt->expression);
    ASSERT_NE(assignment, nullptr);
    auto binaryExpr = std::dynamic_pointer_cast<gate::ast::Binary>(assignment->value);
    ASSERT_NE(binaryExpr, nullptr);
    EXPECT_EQ(binaryExpr->op.type, gate::core::TokenType::XOR);

    EXPECT_TRUE(generated_pascal.find("c := (a xor b);") != std::string::npos);
}
