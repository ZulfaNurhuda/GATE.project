#ifndef GATE_TRANSPILER_NOTAL_LEXER_H
#define GATE_TRANSPILER_NOTAL_LEXER_H

#include "gate/core/Token.h"
#include <string>
#include <vector>

namespace gate::transpiler {

    class NotalLexer {
    public:
        explicit NotalLexer(const std::string& source);

        core::Token nextToken();
        std::vector<core::Token> getAllTokens();

    private:
        std::string source_;
        size_t current_ = 0;
        size_t start_ = 0;
        int line_ = 1;
        int column_ = 1;

        bool isAtEnd();
        char advance();
        char peek();
        char peekNext();
        bool match(char expected);
        void skipWhitespaceAndComments();

        core::Token makeToken(core::TokenType type);
        core::Token makeToken(core::TokenType type, const std::string& lexeme);
        core::Token errorToken(const std::string& message);
        core::Token stringLiteral(char startChar);
        core::Token numberLiteral();
        core::Token identifier();
    };

} // namespace gate::transpiler

#endif // GATE_TRANSPILER_NOTAL_LEXER_H
