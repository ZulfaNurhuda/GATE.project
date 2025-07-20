#include "notal_transpiler/Lexer.h"
#include <cctype>
#include <iostream>

namespace notal {

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::allTokens() {
    std::vector<Token> tokens;
    Token token;
    do {
        token = nextToken();
        tokens.push_back(token);
    } while (token.type != TokenType::END_OF_FILE);
    return tokens;
}


Token Lexer::nextToken() {
    skipWhitespaceAndComments();

    start = current;

    if (isAtEnd()) {
        return makeToken(TokenType::END_OF_FILE);
    }

    char c = advance();

    if (std::isalpha(c) || c == '_') {
        return identifier();
    }
    if (std::isdigit(c)) {
        return numberLiteral();
    }

    switch (c) {
        case '(': return makeToken(TokenType::LPAREN);
        case ')': return makeToken(TokenType::RPAREN);
        case '[': return makeToken(TokenType::LBRACKET);
        case ']': return makeToken(TokenType::RBRACKET);
        case ':': return makeToken(TokenType::COLON);
        case ',': return makeToken(TokenType::COMMA);
        case '.': return makeToken(TokenType::DOT);
        case '+': return makeToken(TokenType::PLUS);
        case '-': return makeToken(TokenType::MINUS);
        case '*': return makeToken(TokenType::MULTIPLY);
        case '/': return makeToken(TokenType::DIVIDE);
        case '^': return makeToken(TokenType::POWER);
        case '&': return makeToken(TokenType::AMPERSAND);
        case '@': return makeToken(TokenType::AT);
        case '|': return makeToken(TokenType::PIPE);
        case '=': return makeToken(TokenType::EQUAL);
        case '<':
            if (match('-')) return makeToken(TokenType::ASSIGN);
            if (match('>')) return makeToken(TokenType::NOT_EQUAL);
            if (match('=')) return makeToken(TokenType::LESS_EQUAL);
            return makeToken(TokenType::LANGLE);
        case '>':
            if (match('=')) return makeToken(TokenType::GREATER_EQUAL);
            return makeToken(TokenType::RANGLE);
        case '\'': return stringLiteral('\'');
        case '"': return stringLiteral('"');
    }

    return errorToken("Unexpected character.");
}

void Lexer::skipWhitespaceAndComments() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line++;
                column = 1;
                advance();
                break;
            case '{':
                // A { ... } comment.
                while (peek() != '}' && !isAtEnd()) {
                    if (peek() == '\n') {
                        line++;
                        column = 1;
                    }
                    advance();
                }
                if (isAtEnd()) {
                    // This state will be caught by nextToken() to create an error token
                    return;
                }
                advance(); // consume }
                break;
            default:
                return;
        }
    }
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

char Lexer::advance() {
    current++;
    column++;
    return source[current - 1];
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    column++;
    return true;
}

Token Lexer::makeToken(TokenType type) {
    return makeToken(type, source.substr(start, current - start));
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme) {
    return Token{type, lexeme, line, column - (int)lexeme.length()};
}

Token Lexer::errorToken(const std::string& message) {
    return Token{TokenType::UNKNOWN, message, line, column};
}

Token Lexer::stringLiteral(char startChar) {
    while (peek() != startChar && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }

    if (isAtEnd()) {
        return errorToken("Unterminated string.");
    }

    advance(); // The closing quote
    std::string value = source.substr(start + 1, current - start - 2);
    return makeToken(TokenType::STRING_LITERAL, value);
}

Token Lexer::numberLiteral() {
    while (std::isdigit(peek())) {
        advance();
    }

    bool isReal = false;
    if (peek() == '.' && std::isdigit(peekNext())) {
        isReal = true;
        advance(); // Consume the .
        while (std::isdigit(peek())) {
            advance();
        }
    }

    return makeToken(isReal ? TokenType::REAL_LITERAL : TokenType::INTEGER_LITERAL);
}

Token Lexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }

    std::string text = source.substr(start, current - start);
    auto it = KEYWORDS.find(text);
    if (it != KEYWORDS.end()) {
        return makeToken(it->second);
    }

    return makeToken(TokenType::IDENTIFIER);
}

} // namespace notal
