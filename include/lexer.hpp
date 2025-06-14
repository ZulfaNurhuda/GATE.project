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
    READ,     // Added for read operations
    RETURN,   // Added for function returns
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
    STRING_LITERAL,

    // For array type declarations
    ARRAY,
    OF,
    DOTDOT, // For '..' range in array declarations

    // Operators
    DIV,     // Integer division: div
    MOD,     // Modulo: mod
    CARET,   // Power: ^ (using CARET as token name)
    NOT,     // Logical: not
    AND,     // Logical: and
    OR,      // Logical: or
    LESS,    // Relational: <
    LESSEQUAL, // Relational: <=
    NOTEQUAL,  // Relational: <>

    // Control Flow Keywords
    WHILE,
    DO,
    ENDWHILE,
    REPEAT,
    UNTIL,
    DEPEND,   // For 'depend on'
    ON,       // For 'depend on'
    CASE,
    OTHERWISE,
    ENDDEPENDON,

    // Subprogram Keywords
    PROCEDURE,
    RIGHT_ARROW, // For '->' in function returns and return type declaration

    // Brackets for arrays
    LBRACKET,  // [
    RBRACKET,   // ]

    // Standard/utility tokens (often useful)
    // These were not explicitly requested but are standard practice
    TYPE_INTEGER, // Explicit type keyword e.g. "integer"
    TYPE_REAL,    // Explicit type keyword e.g. "real"
    TYPE_BOOLEAN, // Explicit type keyword e.g. "boolean"
    TYPE_STRING,  // Explicit type keyword e.g. "string"
    TYPE_CHARACTER,// Explicit type keyword e.g. "character"
    TYPE_VOID,    // Explicit type keyword e.g. "void" for procedures

    ASSIGN_OP,    // Explicit assignment operator token, e.g., for '<-'

    ENDPROGRAM, // Specific keyword for program termination
    END, // General block end keyword

    // Utility tokens
    UNKNOWN,   // For unrecognized tokens
    EOF_TOKEN  // End Of File
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