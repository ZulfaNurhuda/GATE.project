#include "gate/transpiler/NotalLexer.h"
#include <cctype>
#include <iostream>

namespace gate::transpiler {

NotalLexer::NotalLexer(const std::string& source) : source_(source) {}

std::vector<core::Token> NotalLexer::getAllTokens() {
    std::vector<core::Token> tokens;
    core::Token token;
    do {
        token = nextToken();
        tokens.push_back(token);
    } while (token.type != core::TokenType::END_OF_FILE);
    return tokens;
}


core::Token NotalLexer::nextToken() {
    skipWhitespaceAndComments();

    start_ = current_;

    if (isAtEnd()) {
        return makeToken(core::TokenType::END_OF_FILE);
    }

    char c = advance();

    if (std::isalpha(c) || c == '_') {
        return identifier();
    }
    if (std::isdigit(c)) {
        return numberLiteral();
    }

    switch (c) {
        case '(': return makeToken(core::TokenType::LPAREN);
        case ')': return makeToken(core::TokenType::RPAREN);
        case '[': return makeToken(core::TokenType::LBRACKET);
        case ']': return makeToken(core::TokenType::RBRACKET);
        case ':': return makeToken(core::TokenType::COLON);
        case ',': return makeToken(core::TokenType::COMMA);
        case '.':
            if (match('.')) return makeToken(core::TokenType::DOT_DOT);
            return makeToken(core::TokenType::DOT);
        case '+': return makeToken(core::TokenType::PLUS);
        case '-':
            if (match('>')) return makeToken(core::TokenType::ARROW);
            return makeToken(core::TokenType::MINUS);
        case '*': return makeToken(core::TokenType::MULTIPLY);
        case '/': return makeToken(core::TokenType::DIVIDE);
        case '^': return makeToken(core::TokenType::POWER);
        case '&': return makeToken(core::TokenType::AMPERSAND);
        case '@': return makeToken(core::TokenType::AT);
        case '|': return makeToken(core::TokenType::PIPE);
        case '=': return makeToken(core::TokenType::EQUAL);
        case '<':
            if (match('-')) return makeToken(core::TokenType::ASSIGN);
            if (match('>')) return makeToken(core::TokenType::NOT_EQUAL);
            if (match('=')) return makeToken(core::TokenType::LESS_EQUAL);
            return makeToken(core::TokenType::LESS);
        case '>':
            if (match('=')) return makeToken(core::TokenType::GREATER_EQUAL);
            return makeToken(core::TokenType::GREATER);
        case '\'': return stringLiteral('\'');
        case '"': return stringLiteral('"');
    }

    return errorToken("Unexpected character.");
}

void NotalLexer::skipWhitespaceAndComments() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line_++;
                column_ = 1;
                advance();
                break;
            case '{':
                // A { ... } comment.
                while (peek() != '}' && !isAtEnd()) {
                    if (peek() == '\n') {
                        line_++;
                        column_ = 1;
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

bool NotalLexer::isAtEnd() {
    return current_ >= source_.length();
}

char NotalLexer::advance() {
    current_++;
    column_++;
    return source_[current_ - 1];
}

char NotalLexer::peek() {
    if (isAtEnd()) return '\0';
    return source_[current_];
}

char NotalLexer::peekNext() {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

bool NotalLexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source_[current_] != expected) return false;
    current_++;
    column_++;
    return true;
}

core::Token NotalLexer::makeToken(core::TokenType type) {
    return makeToken(type, source_.substr(start_, current_ - start_));
}

core::Token NotalLexer::makeToken(core::TokenType type, const std::string& lexeme) {
    return core::Token{type, lexeme, line_, column_ - (int)lexeme.length()};
}

core::Token NotalLexer::errorToken(const std::string& message) {
    return core::Token{core::TokenType::UNKNOWN, message, line_, column_};
}

core::Token NotalLexer::stringLiteral(char startChar) {
    while (peek() != startChar && !isAtEnd()) {
        if (peek() == '\n') {
            line_++;
            column_ = 1;
        }
        advance();
    }

    if (isAtEnd()) {
        return errorToken("Unterminated string.");
    }

    advance(); // The closing quote
    std::string value = source_.substr(start_ + 1, current_ - start_ - 2);
    return makeToken(core::TokenType::STRING_LITERAL, value);
}

core::Token NotalLexer::numberLiteral() {
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

    return makeToken(isReal ? core::TokenType::REAL_LITERAL : core::TokenType::INTEGER_LITERAL);
}

core::Token NotalLexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }

    std::string text = source_.substr(start_, current_ - start_);
    
    if (text == "true" || text == "false") {
        return makeToken(core::TokenType::BOOLEAN_LITERAL);
    } else if (text == "NULL") {
        return makeToken(core::TokenType::NULL_LITERAL);
    }
    
    auto it = core::KEYWORDS.find(text);
    if (it != core::KEYWORDS.end()) {
        return makeToken(it->second);
    }

    return makeToken(core::TokenType::IDENTIFIER);
}

} // namespace gate::transpiler
