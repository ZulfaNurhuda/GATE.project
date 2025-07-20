#include <gtest/gtest.h>
#include "notal_transpiler/Lexer.h"
#include <vector>

TEST(LexerNewTokensTest, RecordSyntaxTokens) {
    std::string source = "type Student: < name: string, age: integer >";
    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();

    std::vector<notal::TokenType> expected_types = {
        notal::TokenType::TYPE, notal::TokenType::IDENTIFIER, notal::TokenType::COLON,
        notal::TokenType::LESS, // < for record start
        notal::TokenType::IDENTIFIER, notal::TokenType::COLON, notal::TokenType::STRING,
        notal::TokenType::COMMA,
        notal::TokenType::IDENTIFIER, notal::TokenType::COLON, notal::TokenType::INTEGER,
        notal::TokenType::GREATER, // > for record end
        notal::TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected_types.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]) << "Token " << i << " has wrong type. Lexeme: " << tokens[i].lexeme;
    }
}

TEST(LexerNewTokensTest, EnumSyntaxTokens) {
    std::string source = "type Day: (monday, tuesday, wednesday)";
    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();

    std::vector<notal::TokenType> expected_types = {
        notal::TokenType::TYPE, notal::TokenType::IDENTIFIER, notal::TokenType::COLON,
        notal::TokenType::LPAREN, // ( for enum start
        notal::TokenType::IDENTIFIER, notal::TokenType::COMMA,
        notal::TokenType::IDENTIFIER, notal::TokenType::COMMA,
        notal::TokenType::IDENTIFIER,
        notal::TokenType::RPAREN, // ) for enum end
        notal::TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected_types.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]) << "Token " << i << " has wrong type. Lexeme: " << tokens[i].lexeme;
    }
}

TEST(LexerNewTokensTest, ConstraintSyntaxTokens) {
    std::string source = "age: integer | age >= 0 and age <= 150";
    notal::Lexer lexer(source);
    std::vector<notal::Token> tokens = lexer.allTokens();

    std::vector<notal::TokenType> expected_types = {
        notal::TokenType::IDENTIFIER, notal::TokenType::COLON, notal::TokenType::INTEGER,
        notal::TokenType::PIPE, // | for constraint separator
        notal::TokenType::IDENTIFIER, notal::TokenType::GREATER_EQUAL, notal::TokenType::INTEGER_LITERAL,
        notal::TokenType::AND,
        notal::TokenType::IDENTIFIER, notal::TokenType::LESS_EQUAL, notal::TokenType::INTEGER_LITERAL,
        notal::TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected_types.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]) << "Token " << i << " has wrong type. Lexeme: " << tokens[i].lexeme;
    }
}