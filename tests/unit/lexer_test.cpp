#include <gtest/gtest.h>
#include "gate/transpiler/NotalLexer.h"
#include <vector>

TEST(LexerTest, BasicProgram) {
    std::string source = R"(
PROGRAM SimpleProgram
{ This is a comment }
KAMUS
    age: integer
ALGORITMA
    age <- 10
    output(age)
)";

    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();

    std::vector<gate::core::TokenType> expected_types = {
        gate::core::TokenType::PROGRAM, gate::core::TokenType::IDENTIFIER,
        gate::core::TokenType::KAMUS,
        gate::core::TokenType::IDENTIFIER, gate::core::TokenType::COLON, gate::core::TokenType::INTEGER,
        gate::core::TokenType::ALGORITMA,
        gate::core::TokenType::IDENTIFIER, gate::core::TokenType::ASSIGN, gate::core::TokenType::INTEGER_LITERAL,
        gate::core::TokenType::OUTPUT, gate::core::TokenType::LPAREN, gate::core::TokenType::IDENTIFIER, gate::core::TokenType::RPAREN,
        gate::core::TokenType::END_OF_FILE
    };

    ASSERT_EQ(static_cast<long long>(tokens.size()), static_cast<long long>(expected_types.size()));

    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]) << "Token " << i << " has wrong type. Lexeme: " << tokens[i].lexeme;
    }

    EXPECT_EQ(tokens[1].lexeme, "SimpleProgram");
    EXPECT_EQ(tokens[3].lexeme, "age");
    EXPECT_EQ(tokens[5].lexeme, "integer");
    EXPECT_EQ(tokens[7].lexeme, "age");
    EXPECT_EQ(tokens[9].lexeme, "10");
}

TEST(LexerTest, AllOperatorsAndPunctuation) {
    std::string source = "+ - * / ^ = <> > >= < <= & @ ( ) [ ] : , . | <-";
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();

    std::vector<gate::core::TokenType> expected_types = {
        gate::core::TokenType::PLUS, gate::core::TokenType::MINUS, gate::core::TokenType::MULTIPLY,
        gate::core::TokenType::DIVIDE, gate::core::TokenType::POWER, gate::core::TokenType::EQUAL,
        gate::core::TokenType::NOT_EQUAL, gate::core::TokenType::GREATER, gate::core::TokenType::GREATER_EQUAL,
        gate::core::TokenType::LESS, gate::core::TokenType::LESS_EQUAL, gate::core::TokenType::AMPERSAND,
        gate::core::TokenType::AT, gate::core::TokenType::LPAREN, gate::core::TokenType::RPAREN,
        gate::core::TokenType::LBRACKET, gate::core::TokenType::RBRACKET, gate::core::TokenType::COLON,
        gate::core::TokenType::COMMA, gate::core::TokenType::DOT, gate::core::TokenType::PIPE,
        gate::core::TokenType::ASSIGN, gate::core::TokenType::END_OF_FILE
    };

    ASSERT_EQ(static_cast<long long>(tokens.size()), static_cast<long long>(expected_types.size()));

    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]) << "Token " << i << " has wrong type. Lexeme: " << tokens[i].lexeme;
    }
}
