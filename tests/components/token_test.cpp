#include <gtest/gtest.h>
#include "core/Token.h"
#include <string>
#include <vector>

TEST(TokenTest, TokenCreation) {
    gate::core::Token token{gate::core::TokenType::IDENTIFIER, "testVar", "test.notal", 1, 5};
    
    EXPECT_EQ(token.type, gate::core::TokenType::IDENTIFIER);
    EXPECT_EQ(token.lexeme, "testVar");
    EXPECT_EQ(token.filename, "test.notal");
    EXPECT_EQ(token.line, 1);
    EXPECT_EQ(token.column, 5);
}

TEST(TokenTest, Keywords) {
    // Test keyword token types exist
    std::vector<gate::core::TokenType> keywords = {
        gate::core::TokenType::PROGRAM,
        gate::core::TokenType::KAMUS,
        gate::core::TokenType::ALGORITMA,
        gate::core::TokenType::IF,
        gate::core::TokenType::THEN,
        gate::core::TokenType::ELSE,
        gate::core::TokenType::WHILE,
        gate::core::TokenType::TO,
        gate::core::TokenType::REPEAT,
        gate::core::TokenType::UNTIL,
        gate::core::TokenType::FUNCTION,
        gate::core::TokenType::PROCEDURE,
        gate::core::TokenType::INPUT,
        gate::core::TokenType::OUTPUT,
        gate::core::TokenType::AND,
        gate::core::TokenType::OR,
        gate::core::TokenType::NOT,
        gate::core::TokenType::XOR,
        gate::core::TokenType::BOOLEAN_LITERAL,
        gate::core::TokenType::NULL_LITERAL
    };
    
    for (const auto& tokenType : keywords) {
        std::string typeStr = gate::core::tokenTypeToString(tokenType);
        EXPECT_FALSE(typeStr.empty()) << "Token type should have string representation";
        EXPECT_NE(typeStr, "UNKNOWN") << "Token type should not be unknown";
    }
}

TEST(TokenTest, DataTypeKeywords) {
    // Test data type token types exist
    std::vector<gate::core::TokenType> dataTypes = {
        gate::core::TokenType::INTEGER,
        gate::core::TokenType::REAL,
        gate::core::TokenType::CHARACTER,
        gate::core::TokenType::STRING,
        gate::core::TokenType::BOOLEAN,
        gate::core::TokenType::ARRAY,
        gate::core::TokenType::OF,
        gate::core::TokenType::TYPE
    };
    
    for (const auto& tokenType : dataTypes) {
        std::string typeStr = gate::core::tokenTypeToString(tokenType);
        EXPECT_FALSE(typeStr.empty()) << "Token type should have string representation";
        EXPECT_NE(typeStr, "UNKNOWN") << "Token type should not be unknown";
    }
}

TEST(TokenTest, OperatorTokens) {
    std::vector<std::pair<std::string, gate::core::TokenType>> operators = {
        {"+", gate::core::TokenType::PLUS},
        {"-", gate::core::TokenType::MINUS},
        {"*", gate::core::TokenType::MULTIPLY},
        {"/", gate::core::TokenType::DIVIDE},
        {"^", gate::core::TokenType::POWER},
        {"=", gate::core::TokenType::EQUAL},
        {"<>", gate::core::TokenType::NOT_EQUAL},
        {">", gate::core::TokenType::GREATER},
        {">=", gate::core::TokenType::GREATER_EQUAL},
        {"<", gate::core::TokenType::LESS},
        {"<=", gate::core::TokenType::LESS_EQUAL},
        {"<-", gate::core::TokenType::ASSIGN}
    };
    
    for (const auto& [op, expectedType] : operators) {
        gate::core::Token token{expectedType, op, "test.notal", 1, 1};
        EXPECT_EQ(token.type, expectedType) << "Operator: " << op;
        EXPECT_EQ(token.lexeme, op);
    }
}

TEST(TokenTest, PunctuationTokens) {
    std::vector<std::pair<std::string, gate::core::TokenType>> punctuation = {
        {"(", gate::core::TokenType::LPAREN},
        {")", gate::core::TokenType::RPAREN},
        {"[", gate::core::TokenType::LBRACKET},
        {"]", gate::core::TokenType::RBRACKET},
        {":", gate::core::TokenType::COLON},
        {",", gate::core::TokenType::COMMA},
        {".", gate::core::TokenType::DOT},
        {"|", gate::core::TokenType::PIPE},
        {"&", gate::core::TokenType::AMPERSAND},
        {"@", gate::core::TokenType::AT}
    };
    
    for (const auto& [punct, expectedType] : punctuation) {
        gate::core::Token token{expectedType, punct, "test.notal", 1, 1};
        EXPECT_EQ(token.type, expectedType) << "Punctuation: " << punct;
        EXPECT_EQ(token.lexeme, punct);
    }
}

TEST(TokenTest, LiteralTokens) {
    // Integer literals
    gate::core::Token intToken{gate::core::TokenType::INTEGER_LITERAL, "42", "test.notal", 1, 1};
    EXPECT_EQ(intToken.type, gate::core::TokenType::INTEGER_LITERAL);
    EXPECT_EQ(intToken.lexeme, "42");
    
    // Real literals
    gate::core::Token realToken{gate::core::TokenType::REAL_LITERAL, "3.14", "test.notal", 1, 1};
    EXPECT_EQ(realToken.type, gate::core::TokenType::REAL_LITERAL);
    EXPECT_EQ(realToken.lexeme, "3.14");
    
    // String literals
    gate::core::Token stringToken{gate::core::TokenType::STRING_LITERAL, "'Hello'", "test.notal", 1, 1};
    EXPECT_EQ(stringToken.type, gate::core::TokenType::STRING_LITERAL);
    EXPECT_EQ(stringToken.lexeme, "'Hello'");
    
    // Character literals (using STRING_LITERAL for single characters)
    gate::core::Token charToken{gate::core::TokenType::STRING_LITERAL, "'A'", "test.notal", 1, 1};
    EXPECT_EQ(charToken.type, gate::core::TokenType::STRING_LITERAL);
    EXPECT_EQ(charToken.lexeme, "'A'");
    
    // Boolean literals
    gate::core::Token boolToken{gate::core::TokenType::BOOLEAN_LITERAL, "true", "test.notal", 1, 1};
    EXPECT_EQ(boolToken.type, gate::core::TokenType::BOOLEAN_LITERAL);
    EXPECT_EQ(boolToken.lexeme, "true");
}

TEST(TokenTest, SpecialTokens) {
    // End of file token
    gate::core::Token eofToken{gate::core::TokenType::END_OF_FILE, "", "test.notal", 1, 1};
    EXPECT_EQ(eofToken.type, gate::core::TokenType::END_OF_FILE);
    EXPECT_TRUE(eofToken.lexeme.empty());
    
    // Unknown token
    gate::core::Token unknownToken{gate::core::TokenType::UNKNOWN, "???", "test.notal", 1, 1};
    EXPECT_EQ(unknownToken.type, gate::core::TokenType::UNKNOWN);
    EXPECT_EQ(unknownToken.lexeme, "???");
}

TEST(TokenTest, IdentifierRecognition) {
    std::vector<std::string> validIdentifiers = {
        "variable",
        "myVar",
        "test123",
        "_underscore",
        "camelCase",
        "PascalCase",
        "var_with_underscores",
        "a",
        "x1",
        "temp_var_2"
    };
    
    for (const auto& identifier : validIdentifiers) {
        // Should be treated as identifier
        gate::core::Token token{gate::core::TokenType::IDENTIFIER, identifier, "test.notal", 1, 1};
        EXPECT_EQ(token.type, gate::core::TokenType::IDENTIFIER) << "Identifier: " << identifier;
        EXPECT_EQ(token.lexeme, identifier);
        EXPECT_EQ(token.filename, "test.notal");
        EXPECT_EQ(token.line, 1);
        EXPECT_EQ(token.column, 1);
    }
}

TEST(TokenTest, TokenComparison) {
    gate::core::Token token1{gate::core::TokenType::IDENTIFIER, "test", "test.notal", 1, 5};
    gate::core::Token token2{gate::core::TokenType::IDENTIFIER, "test", "test.notal", 1, 5};
    gate::core::Token token3{gate::core::TokenType::INTEGER_LITERAL, "42", "test.notal", 2, 10};
    
    // Test equality (if implemented)
    EXPECT_EQ(token1.type, token2.type);
    EXPECT_EQ(token1.lexeme, token2.lexeme);
    EXPECT_EQ(token1.line, token2.line);
    EXPECT_EQ(token1.column, token2.column);
    
    // Test inequality
    EXPECT_NE(token1.type, token3.type);
    EXPECT_NE(token1.lexeme, token3.lexeme);
    EXPECT_NE(token1.line, token3.line);
    EXPECT_NE(token1.column, token3.column);
}

TEST(TokenTest, TokenToString) {
    gate::core::Token token{gate::core::TokenType::IDENTIFIER, "testVar", "test.notal", 5, 10};
    
    // Test string representation
    std::string tokenStr = gate::core::tokenTypeToString(token.type);
    EXPECT_FALSE(tokenStr.empty());
    EXPECT_TRUE(tokenStr.find("IDENTIFIER") != std::string::npos);
    
    // Test token toString method
    std::string fullStr = token.toString();
    EXPECT_FALSE(fullStr.empty());
    EXPECT_TRUE(fullStr.find("testVar") != std::string::npos);
}

TEST(TokenTest, AdvancedKeywords) {
    // Test advanced keyword token types exist
    std::vector<gate::core::TokenType> advancedTypes = {
        gate::core::TokenType::TRAVERSAL,
        gate::core::TokenType::ITERATE,
        gate::core::TokenType::STOP,
        gate::core::TokenType::DEPEND,
        gate::core::TokenType::ON,
        gate::core::TokenType::ALLOCATE,
        gate::core::TokenType::DEALLOCATE,
        gate::core::TokenType::STEP,
        gate::core::TokenType::DIV,
        gate::core::TokenType::MOD
    };
    
    for (const auto& tokenType : advancedTypes) {
        std::string typeStr = gate::core::tokenTypeToString(tokenType);
        EXPECT_FALSE(typeStr.empty()) << "Token type should have string representation";
        EXPECT_NE(typeStr, "UNKNOWN") << "Token type should not be unknown";
    }
}