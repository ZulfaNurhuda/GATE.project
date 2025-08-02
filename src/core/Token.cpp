/**
 * @file Token.cpp
 * @brief Implementation of token-related functionality for the GATE transpiler
 * 
 * This file contains the implementation of token utilities, including token type
 * to string conversion and token string representation methods. These utilities
 * are essential for debugging, error reporting, and logging during the
 * transpilation process.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */

#include "gate/core/Token.h"

#include <sstream>

namespace gate::core {

    /**
     * @brief Static map for converting TokenType enums to string representations
     * 
     * This map provides a convenient way to convert token type enumerations
     * to their corresponding string names. It's particularly useful for:
     * - Debugging and development
     * - Error message generation
     * - Logging and diagnostic output
     * - AST printing and visualization
     * 
     * The map includes all token types defined in the NOTAL language,
     * from keywords and operators to literals and delimiters.
     */
    static const std::unordered_map<TokenType, std::string> TOKEN_TYPE_STRINGS = {
        {TokenType::UNKNOWN, "UNKNOWN"}, {TokenType::END_OF_FILE, "END_OF_FILE"},
        {TokenType::PROGRAM, "PROGRAM"}, {TokenType::KAMUS, "KAMUS"}, {TokenType::ALGORITMA, "ALGORITMA"},
        {TokenType::CONSTANT, "CONSTANT"}, {TokenType::TYPE, "TYPE"}, {TokenType::IF, "IF"}, {TokenType::THEN, "THEN"},
        {TokenType::ELSE, "ELSE"}, {TokenType::ELIF, "ELIF"}, {TokenType::DEPEND, "DEPEND"},
        {TokenType::ON, "ON"}, {TokenType::OTHERWISE, "OTHERWISE"}, {TokenType::WHILE, "WHILE"},
        {TokenType::DO, "DO"}, {TokenType::REPEAT, "REPEAT"}, {TokenType::UNTIL, "UNTIL"},
        {TokenType::TRAVERSAL, "TRAVERSAL"}, {TokenType::STEP, "STEP"}, {TokenType::ITERATE, "ITERATE"},
        {TokenType::STOP, "STOP"}, {TokenType::SKIP, "SKIP"}, {TokenType::TIMES, "TIMES"},
        {TokenType::PROCEDURE, "PROCEDURE"}, {TokenType::FUNCTION, "FUNCTION"}, {TokenType::INPUT, "INPUT"},
        {TokenType::OUTPUT, "OUTPUT"}, {TokenType::POINTER, "POINTER"}, {TokenType::TO, "TO"},
        {TokenType::ARRAY, "ARRAY"}, {TokenType::OF, "OF"}, {TokenType::ALLOCATE, "ALLOCATE"},
        {TokenType::DEALLOCATE, "DEALLOCATE"}, {TokenType::AND, "AND"}, {TokenType::OR, "OR"},
        {TokenType::NOT, "NOT"}, {TokenType::XOR, "XOR"}, {TokenType::DIV, "DIV"},
        {TokenType::MOD, "MOD"}, {TokenType::INTEGER, "INTEGER"}, {TokenType::REAL, "REAL"},
        {TokenType::BOOLEAN, "BOOLEAN"}, {TokenType::CHARACTER, "CHARACTER"}, {TokenType::STRING, "STRING"}, {TokenType::NULL_TYPE, "NULL_TYPE"},
        {TokenType::INTEGER_LITERAL, "INTEGER_LITERAL"}, {TokenType::REAL_LITERAL, "REAL_LITERAL"},
        {TokenType::STRING_LITERAL, "STRING_LITERAL"}, {TokenType::BOOLEAN_LITERAL, "BOOLEAN_LITERAL"}, {TokenType::NULL_LITERAL, "NULL_LITERAL"},
        {TokenType::IDENTIFIER, "IDENTIFIER"}, {TokenType::ASSIGN, "ASSIGN"}, {TokenType::PLUS, "PLUS"},
        {TokenType::MINUS, "MINUS"}, {TokenType::MULTIPLY, "MULTIPLY"}, {TokenType::DIVIDE, "DIVIDE"},
        {TokenType::POWER, "POWER"}, {TokenType::EQUAL, "EQUAL"}, {TokenType::NOT_EQUAL, "NOT_EQUAL"},
        {TokenType::GREATER, "GREATER"}, {TokenType::GREATER_EQUAL, "GREATER_EQUAL"}, {TokenType::LESS, "LESS"},
        {TokenType::LESS_EQUAL, "LESS_EQUAL"}, {TokenType::AMPERSAND, "AMPERSAND"}, {TokenType::AT, "AT"},
        {TokenType::ARROW, "ARROW"}, {TokenType::LPAREN, "LPAREN"}, {TokenType::RPAREN, "RPAREN"},
        {TokenType::LBRACKET, "LBRACKET"}, {TokenType::RBRACKET, "RBRACKET"}, {TokenType::LANGLE, "LANGLE"},
        {TokenType::RANGLE, "RANGLE"}, {TokenType::COLON, "COLON"}, {TokenType::COMMA, "COMMA"},
        {TokenType::DOT, "DOT"}, {TokenType::DOT_DOT, "DOT_DOT"}, {TokenType::PIPE, "PIPE"},
        {TokenType::LBRACE, "LBRACE"}, {TokenType::RBRACE, "RBRACE"}
    };

    /**
     * @brief Converts a TokenType enum to its string representation
     * 
     * This function provides a safe way to convert token type enumerations
     * to human-readable strings. It's used extensively throughout the
     * transpiler for error reporting, debugging, and logging.
     * 
     * @param type The TokenType enum value to convert
     * @return const std::string& Reference to the string representation
     * 
     * @note Returns "UNKNOWN" for any token type not found in the map
     * @note The returned reference is valid for the lifetime of the program
     */
    const std::string& tokenTypeToString(TokenType type) {
        auto it = TOKEN_TYPE_STRINGS.find(type);
        if (it != TOKEN_TYPE_STRINGS.end()) {
            return it->second;
        }
        static const std::string unknown = "UNKNOWN";
        return unknown;
    }

    /**
     * @brief Generates a string representation of the token
     * 
     * This method creates a formatted string containing all relevant
     * information about the token, including its type, lexeme (text content),
     * and position in the source code. The format is:
     * "Token(TYPE, 'lexeme', line:column)"
     * 
     * This representation is particularly useful for:
     * - Debugging lexical analysis
     * - Error message generation
     * - Development and testing
     * - Token stream visualization
     * 
     * @return std::string Formatted string representation of the token
     * 
     * @note The lexeme is enclosed in single quotes for clarity
     * @note Position information helps locate tokens in source code
     */
    std::string Token::toString() const {
        std::stringstream ss;
        ss << "Token(" << tokenTypeToString(type) << ", '" << lexeme << "', " << line << ":" << column << ")";
        return ss.str();
    }

} // namespace gate::core
