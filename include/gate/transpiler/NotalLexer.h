/**
 * @file NotalLexer.h
 * @brief Lexical analyzer for the NOTAL language
 * 
 * This file defines the NotalLexer class responsible for tokenizing NOTAL
 * source code. The lexer performs lexical analysis by scanning the input
 * source code and producing a stream of tokens that can be consumed by
 * the parser.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */

#ifndef GATE_TRANSPILER_NOTAL_LEXER_H
#define GATE_TRANSPILER_NOTAL_LEXER_H

#include "gate/core/Token.h"
#include <string>
#include <vector>

namespace gate::transpiler {

    /**
     * @brief Lexical analyzer for NOTAL source code
     * 
     * The NotalLexer class performs lexical analysis on NOTAL source code,
     * converting the input text into a sequence of tokens. It handles all
     * NOTAL language constructs including keywords, operators, literals,
     * identifiers, and punctuation marks.
     * 
     * @author GATE Project Team
     * @version 1.0
     * @date 2025
     */
    class NotalLexer {
    public:
        /**
         * @brief Constructor for NotalLexer
         * @param source The NOTAL source code to tokenize
         */
        explicit NotalLexer(const std::string& source);

        /**
         * @brief Get the next token from the source
         * @return The next token in the input stream
         */
        core::Token nextToken();
        
        /**
         * @brief Get all tokens from the source
         * @return Vector containing all tokens from the input
         */
        std::vector<core::Token> getAllTokens();

    private:
        /** @brief The source code being tokenized */
        std::string source_;
        /** @brief Current position in the source */
        size_t current_ = 0;
        /** @brief Start position of current token */
        size_t start_ = 0;
        /** @brief Current line number */
        int line_ = 1;
        /** @brief Current column number */
        int column_ = 1;

        /** @brief Check if we've reached the end of source */
        bool isAtEnd();
        /** @brief Advance to next character and return current */
        char advance();
        /** @brief Peek at current character without advancing */
        char peek();
        /** @brief Peek at next character without advancing */
        char peekNext();
        /** @brief Check if current character matches expected and advance if so */
        bool match(char expected);
        /** @brief Skip whitespace and comments */
        void skipWhitespaceAndComments();

        /** @brief Create token with current lexeme */
        core::Token makeToken(core::TokenType type);
        /** @brief Create token with specified lexeme */
        core::Token makeToken(core::TokenType type, const std::string& lexeme);
        /** @brief Create error token with message */
        core::Token errorToken(const std::string& message);
        /** @brief Parse string literal starting with given character */
        core::Token stringLiteral(char startChar);
        /** @brief Parse numeric literal */
        core::Token numberLiteral();
        /** @brief Parse identifier or keyword */
        core::Token identifier();
    };

} // namespace gate::transpiler

#endif // GATE_TRANSPILER_NOTAL_LEXER_H
