#include <gtest/gtest.h>
#include "notal_transpiler/Lexer.h"
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

    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();

    std::vector<notal::TokenType> expected_types = {
        notal::TokenType::PROGRAM, notal::TokenType::IDENTIFIER,
        notal::TokenType::KAMUS,
        notal::TokenType::IDENTIFIER, notal::TokenType::COLON, notal::TokenType::INTEGER,
        notal::TokenType::ALGORITMA,
        notal::TokenType::IDENTIFIER, notal::TokenType::ASSIGN, notal::TokenType::INTEGER_LITERAL,
        notal::TokenType::OUTPUT, notal::TokenType::LPAREN, notal::TokenType::IDENTIFIER, notal::TokenType::RPAREN,
        notal::TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected_types.size());

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
    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();

    std::vector<notal::TokenType> expected_types = {
        notal::TokenType::PLUS, notal::TokenType::MINUS, notal::TokenType::MULTIPLY,
        notal::TokenType::DIVIDE, notal::TokenType::POWER, notal::TokenType::EQUAL,
        notal::TokenType::NOT_EQUAL, notal::TokenType::GREATER, notal::TokenType::GREATER_EQUAL,
        notal::TokenType::LESS, notal::TokenType::LESS_EQUAL, notal::TokenType::AMPERSAND,
        notal::TokenType::AT, notal::TokenType::LPAREN, notal::TokenType::RPAREN,
        notal::TokenType::LBRACKET, notal::TokenType::RBRACKET, notal::TokenType::COLON,
        notal::TokenType::COMMA, notal::TokenType::DOT, notal::TokenType::PIPE,
        notal::TokenType::ASSIGN, notal::TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected_types.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]) << "Token " << i << " has wrong type. Lexeme: " << tokens[i].lexeme;
    }
}


