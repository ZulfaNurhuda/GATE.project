#include "notal_transpiler/Token.h"

namespace notal {

    // This is a helper map to convert TokenType enums to their string representations.
    // It's useful for debugging and logging.
    static const std::unordered_map<TokenType, std::string> TOKEN_TYPE_STRINGS = {
        {TokenType::UNKNOWN, "UNKNOWN"}, {TokenType::END_OF_FILE, "END_OF_FILE"},
        {TokenType::PROGRAM, "PROGRAM"}, {TokenType::KAMUS, "KAMUS"}, {TokenType::ALGORITMA, "ALGORITMA"},
        {TokenType::TYPE, "TYPE"}, {TokenType::IF, "IF"}, {TokenType::THEN, "THEN"},
        {TokenType::ELSE, "ELSE"}, {TokenType::ELIF, "ELIF"}, {TokenType::DEPEND, "DEPEND"},
        {TokenType::ON, "ON"}, {TokenType::OTHERWISE, "OTHERWISE"}, {TokenType::WHILE, "WHILE"},
        {TokenType::DO, "DO"}, {TokenType::REPEAT, "REPEAT"}, {TokenType::UNTIL, "UNTIL"},
        {TokenType::TRAVERSAL, "TRAVERSAL"}, {TokenType::STEP, "STEP"}, {TokenType::ITERATE, "ITERATE"},
        {TokenType::STOP, "STOP"}, {TokenType::SKIP, "SKIP"}, {TokenType::PROCEDURE, "PROCEDURE"},
        {TokenType::FUNCTION, "FUNCTION"}, {TokenType::INPUT, "INPUT"}, {TokenType::OUTPUT, "OUTPUT"},
        {TokenType::POINTER, "POINTER"}, {TokenType::TO, "TO"}, {TokenType::ALLOCATE, "ALLOCATE"},
        {TokenType::DEALLOCATE, "DEALLOCATE"}, {TokenType::AND, "AND"}, {TokenType::OR, "OR"},
        {TokenType::NOT, "NOT"}, {TokenType::DIV, "DIV"}, {TokenType::MOD, "MOD"},
        {TokenType::INTEGER, "INTEGER"}, {TokenType::REAL, "REAL"}, {TokenType::BOOLEAN, "BOOLEAN"},
        {TokenType::CHARACTER, "CHARACTER"}, {TokenType::STRING, "STRING"},
        {TokenType::INTEGER_LITERAL, "INTEGER_LITERAL"}, {TokenType::REAL_LITERAL, "REAL_LITERAL"},
        {TokenType::STRING_LITERAL, "STRING_LITERAL"}, {TokenType::BOOLEAN_LITERAL, "BOOLEAN_LITERAL"},
        {TokenType::IDENTIFIER, "IDENTIFIER"}, {TokenType::ASSIGN, "ASSIGN"}, {TokenType::PLUS, "PLUS"},
        {TokenType::MINUS, "MINUS"}, {TokenType::MULTIPLY, "MULTIPLY"}, {TokenType::DIVIDE, "DIVIDE"},
        {TokenType::POWER, "POWER"}, {TokenType::EQUAL, "EQUAL"}, {TokenType::NOT_EQUAL, "NOT_EQUAL"},
        {TokenType::GREATER, "GREATER"}, {TokenType::GREATER_EQUAL, "GREATER_EQUAL"}, {TokenType::LESS, "LESS"},
        {TokenType::LESS_EQUAL, "LESS_EQUAL"}, {TokenType::LANGLE, "LANGLE"}, {TokenType::RANGLE, "RANGLE"},
        {TokenType::AMPERSAND, "AMPERSAND"}, {TokenType::AT, "AT"},
        {TokenType::LPAREN, "LPAREN"}, {TokenType::RPAREN, "RPAREN"}, {TokenType::LBRACKET, "LBRACKET"},
        {TokenType::RBRACKET, "RBRACKET"},
        {TokenType::COLON, "COLON"}, {TokenType::COMMA, "COMMA"}, {TokenType::DOT, "DOT"},
        {TokenType::PIPE, "PIPE"}, {TokenType::LBRACE, "LBRACE"}, {TokenType::RBRACE, "RBRACE"}
    };

    const std::string& TokenTypeToString(TokenType type) {
        auto it = TOKEN_TYPE_STRINGS.find(type);
        if (it != TOKEN_TYPE_STRINGS.end()) {
            return it->second;
        }
        static const std::string unknown = "UNKNOWN";
        return unknown;
    }

    std::string Token::toString() const {
        return "Token(" + TokenTypeToString(type) + ", '" + lexeme + "', " + std::to_string(line) + ":" + std::to_string(column) + ")";
    }

} // namespace notal
