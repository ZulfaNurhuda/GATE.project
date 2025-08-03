/**
 * @file NotalLexer.cpp
 * @brief Implementation of the lexical analyzer for the NOTAL language
 * 
 * This file contains the implementation of the NotalLexer class, which performs
 * lexical analysis (tokenization) of NOTAL source code. The lexer converts the
 * input source code into a sequence of tokens that can be processed by the parser.
 * 
 * The lexer recognizes:
 * - Keywords (PROGRAM, KAMUS, ALGORITMA, etc.)
 * - Identifiers (variable names, function names)
 * - Literals (numbers, strings, booleans)
 * - Operators (+, -, *, /, etc.)
 * - Delimiters (parentheses, brackets, commas, etc.)
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */

#include "core/NotalLexer.h"
#include <cctype>
#include <iostream>

namespace gate::transpiler {

/**
 * @brief Constructor for NotalLexer
 * 
 * Initializes the lexer with the source code to be tokenized.
 * Sets up internal state for lexical analysis.
 * 
 * @param source The NOTAL source code to be tokenized
 */
NotalLexer::NotalLexer(const std::string& source, const std::string& filename)
    : source_(source), filename_(filename) {}

/**
 * @brief Tokenizes the entire source code and returns all tokens
 * 
 * This method performs complete lexical analysis of the source code,
 * generating all tokens from start to end of file. It continues tokenizing
 * until it encounters the END_OF_FILE token.
 * 
 * @return std::vector<core::Token> Vector containing all tokens in the source code
 * 
 * @note The returned vector always ends with an END_OF_FILE token
 * @note This method is typically called once at the beginning of parsing
 */
std::vector<core::Token> NotalLexer::getAllTokens() {
    std::vector<core::Token> tokens;
    core::Token token;
    do {
        token = nextToken();
        tokens.push_back(token);
    } while (token.type != core::TokenType::END_OF_FILE);
    return tokens;
}


/**
 * @brief Scans and returns the next token from the source code
 * 
 * This is the core method of the lexer that performs the actual tokenization.
 * It reads the next character(s) from the source and determines what type of
 * token they represent. The method handles:
 * - Single-character tokens (operators, delimiters)
 * - Multi-character tokens (arrows, ranges, compound operators)
 * - Identifiers and keywords
 * - Numeric literals
 * - String literals
 * 
 * @return core::Token The next token found in the source code
 * 
 * @note This method advances the current position in the source code
 * @note Returns END_OF_FILE token when no more characters are available
 */
core::Token NotalLexer::nextToken() {
    // Skip any whitespace and comments before tokenizing
    skipWhitespaceAndComments();

    // Mark the start of the current token
    start_ = current_;

    // Check if we've reached the end of the source code
    if (isAtEnd()) {
        return makeToken(core::TokenType::END_OF_FILE);
    }

    // Get the next character and advance the position
    char c = advance();

    // Handle identifiers and keywords (start with letter or underscore)
    if (std::isalpha(c) || c == '_') {
        return identifier();
    }
    
    // Handle numeric literals (start with digit)
    if (std::isdigit(c)) {
        return numberLiteral();
    }

    // Handle single and multi-character operators and delimiters
    switch (c) {
        // Parentheses and brackets
        case '(': return makeToken(core::TokenType::LPAREN);
        case ')': return makeToken(core::TokenType::RPAREN);
        case '[': return makeToken(core::TokenType::LBRACKET);
        case ']': return makeToken(core::TokenType::RBRACKET);
        
        // Basic delimiters
        case ':': return makeToken(core::TokenType::COLON);
        case ',': return makeToken(core::TokenType::COMMA);
        
        // Dot and range operator
        case '.':
            if (match('.')) return makeToken(core::TokenType::DOT_DOT);  // Range operator ..
            return makeToken(core::TokenType::DOT);
            
        // Arithmetic operators
        case '+': return makeToken(core::TokenType::PLUS);
        case '-':
            if (match('>')) return makeToken(core::TokenType::ARROW);  // Arrow operator ->
            return makeToken(core::TokenType::MINUS);
        case '*': return makeToken(core::TokenType::MULTIPLY);
        case '/': return makeToken(core::TokenType::DIVIDE);
        case '^': return makeToken(core::TokenType::POWER);
        
        // Special operators
        case '&': return makeToken(core::TokenType::AMPERSAND);
        case '@': return makeToken(core::TokenType::AT);
        case '|': return makeToken(core::TokenType::PIPE);
        
        // Comparison and assignment operators
        case '=': return makeToken(core::TokenType::EQUAL);
        case '<':
            if (match('-')) return makeToken(core::TokenType::ASSIGN);  // Assignment operator <-
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

/**
 * @brief Skips whitespace characters and comments, updating line/column tracking
 * 
 * This method advances through whitespace characters (spaces, tabs, carriage returns,
 * and newlines) and comments without generating tokens. It also maintains accurate 
 * line and column position tracking for error reporting.
 * 
 * Comments in NOTAL are enclosed in curly braces { ... }
 * 
 * @note Newlines increment the line counter and reset column to 1
 * @note This method is called before each token is scanned
 * @note Unterminated comments are handled gracefully
 */
void NotalLexer::skipWhitespaceAndComments() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':     // Space
            case '\r':    // Carriage return
            case '\t':    // Tab
                advance();
                break;
            case '\n':    // Newline
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
                return;   // Non-whitespace character found
        }
    }
}

/**
 * @brief Checks if the lexer has reached the end of the source code
 * 
 * @return bool True if at end of source, false otherwise
 */
bool NotalLexer::isAtEnd() {
    return current_ >= source_.length();
}

/**
 * @brief Advances to the next character and updates position tracking
 * 
 * @return char The character at the current position before advancing
 * 
 * @note Increments both current position and column counter
 * @note Returns the character that was at the current position
 */
char NotalLexer::advance() {
    current_++;
    column_++;
    return source_[current_ - 1];
}

/**
 * @brief Looks at the current character without advancing
 * 
 * @return char The character at the current position, or '\0' if at end
 */
char NotalLexer::peek() {
    if (isAtEnd()) return '\0';
    return source_[current_];
}

/**
 * @brief Looks at the next character without advancing
 * 
 * @return char The character after the current position, or '\0' if at/past end
 */
char NotalLexer::peekNext() {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

/**
 * @brief Conditionally advances if the current character matches expected
 * 
 * This method is used for recognizing multi-character tokens like "<-", "->", "..", etc.
 * 
 * @param expected The character to match against
 * @return bool True if character matched and position advanced, false otherwise
 */
bool NotalLexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source_[current_] != expected) return false;
    current_++;
    column_++;
    return true;
}

/**
 * @brief Creates a token from the current lexeme
 * 
 * This method creates a token using the characters between start_ and current_
 * positions as the lexeme. It automatically calculates the correct column position
 * for the start of the token.
 * 
 * @param type The type of token to create
 * @return core::Token The created token with proper position information
 */
core::Token NotalLexer::makeToken(core::TokenType type) {
    return makeToken(type, source_.substr(start_, current_ - start_));
}

/**
 * @brief Creates a token with a custom lexeme
 * 
 * This overload allows creating a token with a lexeme different from the
 * characters in the source (useful for processed string literals, etc.)
 * 
 * @param type The type of token to create
 * @param lexeme The custom lexeme for the token
 * @return core::Token The created token with the custom lexeme
 */
core::Token NotalLexer::makeToken(core::TokenType type, const std::string& lexeme) {
    return core::Token{type, lexeme, filename_, line_, column_ - (int)lexeme.length()};
}

/**
 * @brief Creates an error token for lexical errors
 * 
 * This method is used when the lexer encounters invalid characters or
 * malformed tokens that cannot be properly tokenized.
 * 
 * @param message Error message describing the lexical error
 * @return core::Token An error token with UNKNOWN type
 */
core::Token NotalLexer::errorToken(const std::string& message) {
    return core::Token{core::TokenType::UNKNOWN, message, filename_, line_, column_};
}

/**
 * @brief Parses a string literal token
 * 
 * This method handles string literals enclosed in quotes (single or double).
 * It continues reading characters until it finds the matching closing quote
 * or reaches the end of the source. The method properly handles newlines
 * within strings and maintains line/column tracking.
 * 
 * @param startChar The opening quote character (' or ")
 * @return core::Token A STRING_LITERAL token or an error token if unterminated
 * 
 * @note The returned token's lexeme excludes the surrounding quotes
 * @note Supports multi-line strings with proper line tracking
 */
core::Token NotalLexer::stringLiteral(char startChar) {
    // Continue until we find the matching closing quote or reach end of file
    while (peek() != startChar && !isAtEnd()) {
        if (peek() == '\n') {
            line_++;           // Track line numbers for multi-line strings
            column_ = 1;
        }
        advance();
    }

    // Check for unterminated string
    if (isAtEnd()) {
        return errorToken("Unterminated string.");
    }

    advance(); // Consume the closing quote
    
    // Extract the string content (excluding the quotes)
    std::string value = source_.substr(start_ + 1, current_ - start_ - 2);
    return makeToken(core::TokenType::STRING_LITERAL, value);
}

/**
 * @brief Parses a numeric literal token (integer or real)
 * 
 * This method handles both integer and floating-point number literals.
 * It first consumes all consecutive digits, then checks for a decimal point
 * to determine if it's a real number. The method ensures proper tokenization
 * of numeric values in NOTAL source code.
 * 
 * @return core::Token Either INTEGER_LITERAL or REAL_LITERAL token
 * 
 * @note Supports both integer (123) and real (123.456) number formats
 * @note Does not handle scientific notation or negative signs (handled separately)
 */
core::Token NotalLexer::numberLiteral() {
    // Consume all consecutive digits
    while (std::isdigit(peek())) {
        advance();
    }

    // Check for decimal point to distinguish between integer and real numbers
    bool isReal = false;
    if (peek() == '.' && std::isdigit(peekNext())) {
        isReal = true;
        advance(); // Consume the decimal point
        
        // Consume fractional part digits
        while (std::isdigit(peek())) {
            advance();
        }
    }

    // Return appropriate token type based on whether decimal point was found
    return makeToken(isReal ? core::TokenType::REAL_LITERAL : core::TokenType::INTEGER_LITERAL);
}

/**
 * @brief Parses an identifier or keyword token
 * 
 * This method handles identifiers (variable names, function names) and keywords.
 * It first consumes all alphanumeric characters and underscores, then checks
 * if the resulting text matches any NOTAL keywords or special literals.
 * 
 * The method recognizes:
 * - Boolean literals: "true", "false"
 * - Null literal: "NULL"
 * - Language keywords: "PROGRAM", "KAMUS", "ALGORITMA", etc.
 * - User-defined identifiers: variable names, function names, etc.
 * 
 * @return core::Token The appropriate token type (keyword, literal, or identifier)
 * 
 * @note Keywords take precedence over identifiers
 * @note Identifiers must start with a letter or underscore
 * @note Case-sensitive keyword matching
 */
core::Token NotalLexer::identifier() {
    // Consume all alphanumeric characters and underscores
    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }

    // Extract the identifier text
    std::string text = source_.substr(start_, current_ - start_);
    
    // Check for boolean literals
    if (text == "true" || text == "false") {
        return makeToken(core::TokenType::BOOLEAN_LITERAL);
    } else if (text == "NULL") {
        return makeToken(core::TokenType::NULL_LITERAL);
    }
    
    // Check if the text matches any language keywords
    auto it = core::KEYWORDS.find(text);
    if (it != core::KEYWORDS.end()) {
        return makeToken(it->second);
    }

    // If not a keyword or special literal, treat as user-defined identifier
    return makeToken(core::TokenType::IDENTIFIER);
}

} // namespace gate::transpiler
