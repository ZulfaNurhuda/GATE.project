#ifndef NOTAL_TRANSPILER_LEXER_H
#define NOTAL_TRANSPILER_LEXER_H

#include "Token.h"
#include <string>
#include <vector>

namespace notal {

    class Lexer {
    public:
        explicit Lexer(const std::string& source);

        Token nextToken();
        std::vector<Token> allTokens();

    private:
        std::string source;
        size_t current = 0;
        size_t start = 0;
        int line = 1;
        int column = 1;

        bool isAtEnd();
        char advance();
        char peek();
        char peekNext();
        bool match(char expected);
        void skipWhitespaceAndComments();

        Token makeToken(TokenType type);
        Token makeToken(TokenType type, const std::string& lexeme);
        Token errorToken(const std::string& message);
        Token stringLiteral(char startChar);
        Token numberLiteral();
        Token identifier();
    };

} // namespace notal

#endif // NOTAL_TRANSPILER_LEXER_H
