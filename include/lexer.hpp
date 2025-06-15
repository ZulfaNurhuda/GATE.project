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
    // ENDIF, // Removed for indentation-based parsing
    FOR,
    // TO, // Replaced by TO_KW for consistency, parser will need to use TO_KW
    // ENDFOR, // Removed for indentation-based parsing
    INPUT,
    OUTPUT,
    // DISPOSE, // Will be replaced by DEALLOCATE_KW token for "dispose" keyword
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
    // ENDWHILE, // Removed for indentation-based parsing
    REPEAT,
    UNTIL,
    DEPEND,   // For 'depend on'
    ON,       // For 'depend on'
    CASE,
    OTHERWISE,
    // ENDDEPENDON, // Removed for indentation-based parsing

    INPUTOUTPUT_KEYWORD, // For 'input/output' parameter modifier

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

    ASSIGN_OP,    // For <- (already have LEFT_ARROW, but this could be more semantic)
                  // Decided to keep LEFT_ARROW as per existing list. ASSIGN_OP would be redundant if LEFT_ARROW is used for assignment.
                  // The parser.cpp implementation used ASSIGN_OP, so adding it for consistency or it needs to be changed in parser.cpp
                  // Let's add ASSIGN_OP for now, assuming it's distinct or preferred over LEFT_ARROW for actual assignment.
                  // If LEFT_ARROW is purely for other grammar constructs (like Pascal array ranges), then it's fine.
                  // Given the existing list, LEFT_ARROW is likely the assignment. Let's clarify.
                  // The brief for previous parser.cpp changes implied ASSIGN_OP for "<-".
                  // The existing list has LEFT_ARROW. I will assume LEFT_ARROW IS the assignment operator.
                  // The parser.cpp used consume(TokenType::ASSIGN_OP, ...). This means ASSIGN_OP *should* exist.
                  // I will add it, and it's possible LEFT_ARROW is for something else or can be deprecated.
    ASSIGN_OP,    // Explicit assignment operator token, e.g., for '<-'

    INLINE_KEYWORD, // For INLINE keyword in output statements

    ENDPROGRAM, // Specific keyword for program termination
    END, // General block end keyword

    // Pointer and Memory Keywords
    POINTER_KW,
    TO_KW,          // For POINTER TO type
    REFERENCE_KW,
    DEREFERENCE_KW,
    ALLOCATE_KW,
    REALLOCATE_KW,
    DEALLOCATE_KW,  // "deallocate" and "dispose" will map to this
    NULL_KW,        // For NULL keyword

    // Indentation tokens
    INDENT,
    DEDENT,

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