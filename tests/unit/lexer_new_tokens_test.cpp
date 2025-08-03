#include <gtest/gtest.h>
#include "gate/transpiler/NotalLexer.h"
#include <vector>

TEST(LexerNewTokensTest, RecordSyntaxTokens) {
    std::string source = "type Student: < name: string, age: integer >";
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();

    std::vector<gate::core::TokenType> expected_types = {
        gate::core::TokenType::TYPE, gate::core::TokenType::IDENTIFIER, gate::core::TokenType::COLON,
        gate::core::TokenType::LESS, // < for record start
        gate::core::TokenType::IDENTIFIER, gate::core::TokenType::COLON, gate::core::TokenType::STRING,
        gate::core::TokenType::COMMA,
        gate::core::TokenType::IDENTIFIER, gate::core::TokenType::COLON, gate::core::TokenType::INTEGER,
        gate::core::TokenType::GREATER, // > for record end
        gate::core::TokenType::END_OF_FILE
    };

    ASSERT_EQ(static_cast<long long>(tokens.size()), static_cast<long long>(expected_types.size()));
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]) << "Token " << i << " has wrong type. Lexeme: " << tokens[i].lexeme;
    }
}

TEST(LexerNewTokensTest, FunctionArrowToken) {
    std::string source = "-> integer";
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();

    std::vector<gate::core::TokenType> expected_types = {
        gate::core::TokenType::ARROW,
        gate::core::TokenType::INTEGER,
        gate::core::TokenType::END_OF_FILE
    };

    ASSERT_EQ(static_cast<long long>(tokens.size()), static_cast<long long>(expected_types.size()));
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]) << "Token " << i << " has wrong type. Lexeme: " << tokens[i].lexeme;
    }
}

TEST(LexerNewTokensTest, EnumSyntaxTokens) {
    std::string source = "type Day: (monday, tuesday, wednesday)";
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();

    std::vector<gate::core::TokenType> expected_types = {
        gate::core::TokenType::TYPE, gate::core::TokenType::IDENTIFIER, gate::core::TokenType::COLON,
        gate::core::TokenType::LPAREN, // ( for enum start
        gate::core::TokenType::IDENTIFIER, gate::core::TokenType::COMMA,
        gate::core::TokenType::IDENTIFIER, gate::core::TokenType::COMMA,
        gate::core::TokenType::IDENTIFIER,
        gate::core::TokenType::RPAREN, // ) for enum end
        gate::core::TokenType::END_OF_FILE
    };

    ASSERT_EQ(static_cast<long long>(tokens.size()), static_cast<long long>(expected_types.size()));
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]) << "Token " << i << " has wrong type. Lexeme: " << tokens[i].lexeme;
    }
}

TEST(LexerNewTokensTest, ConstraintSyntaxTokens) {
    std::string source = "age: integer | age >= 0 and age <= 150";
    gate::transpiler::NotalLexer lexer(source, "test");
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();

    std::vector<gate::core::TokenType> expected_types = {
        gate::core::TokenType::IDENTIFIER, gate::core::TokenType::COLON, gate::core::TokenType::INTEGER,
        gate::core::TokenType::PIPE, // | for constraint separator
        gate::core::TokenType::IDENTIFIER, gate::core::TokenType::GREATER_EQUAL, gate::core::TokenType::INTEGER_LITERAL,
        gate::core::TokenType::AND,
        gate::core::TokenType::IDENTIFIER, gate::core::TokenType::LESS_EQUAL, gate::core::TokenType::INTEGER_LITERAL,
        gate::core::TokenType::END_OF_FILE
    };

    ASSERT_EQ(static_cast<long long>(tokens.size()), static_cast<long long>(expected_types.size()));
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]) << "Token " << i << " has wrong type. Lexeme: " << tokens[i].lexeme;
    }
}