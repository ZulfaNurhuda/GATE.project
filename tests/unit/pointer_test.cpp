#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"

TEST(PointerTest, PointerDeclaration) {
    std::string source = R"(
PROGRAM PointerTest
KAMUS
    p: pointer to integer
ALGORITMA
    output('pointer declared')
)";
    std::string generated_pascal = transpile(source);
    EXPECT_TRUE(generated_pascal.find("p: ^integer;") != std::string::npos);
}

TEST(PointerTest, ReferenceAndDereference) {
    std::string source = R"(
PROGRAM PointerTest
KAMUS
    p: pointer to integer
    x: integer
ALGORITMA
    x <- 10
    p <- @x
    output(p^)
)";
    std::string generated_pascal = transpile(source);
    EXPECT_TRUE(generated_pascal.find("p := @(x);") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("writeln((p^));") != std::string::npos);
}

TEST(PointerTest, AllocateAndDeallocate) {
    std::string source = R"(
PROGRAM PointerTest
KAMUS
    p: pointer to integer
ALGORITMA
    allocate(p)
    p^ <- 100
    output(p^)
    deallocate(p)
)";
    std::string generated_pascal = transpile(source);
    EXPECT_TRUE(generated_pascal.find("New(p);") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("(p^) := 100;") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("writeln((p^));") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("Dispose(p);") != std::string::npos);
}

TEST(PointerTest, PointerToRecord) {
    std::string source = R"(
PROGRAM PointerRecordTest
KAMUS
    type Student: < name: string, age: integer >
    pStudent: pointer to Student
ALGORITMA
    allocate(pStudent)
    pStudent^.name <- "Jules"
    pStudent^.age <- 21
    output(pStudent^.name)
    deallocate(pStudent)
)";
    std::string generated_pascal = transpile(source);
    EXPECT_TRUE(generated_pascal.find("pStudent: ^Student;") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("New(pStudent);") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("(pStudent^).name := 'Jules';") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("(pStudent^).age := 21;") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("writeln((pStudent^).name);") != std::string::npos);
    EXPECT_TRUE(generated_pascal.find("Dispose(pStudent);") != std::string::npos);
}
