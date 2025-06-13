#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>

enum class TokenType
{
    PROGRAM,
    KAMUS,
    ALGORITMA,
    FUNCTION,
    TYPE,
    IF,
    THEN,
    ELSE,
    ENDIF,
    FOR,
    TO,
    ENDFOR,
    INPUT,
    OUTPUT,
    DISPOSE,
    TRUE,
    FALSE,
    LEFT_ARROW,
    COLON,
    SEMICOLON,
    COMMA,
    DOT,
    LPAREN,
    RPAREN,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    EQUAL,
    GREATER,
    GREATEREQUAL,
    IDENTIFIER,
    INTEGER_LITERAL,
    STRING_LITERAL
};

struct Token
{
    TokenType type;
    std::string value;
    int line;
    int col;
};

class Lexer
{
public:
    std::vector<Token> tokenize(const std::string &source_code);
};

#endif