#ifndef NOTAL_TRANSPILER_TOKEN_H
#define NOTAL_TRANSPILER_TOKEN_H

#include <string>
#include <unordered_map>

namespace notal {

    enum class TokenType {
        // Special
        UNKNOWN,
        END_OF_FILE,

        // Keywords
        PROGRAM, KAMUS, ALGORITMA,
        CONSTANT,
        TYPE,
        IF, THEN, ELSE, ELIF,
        DEPEND, ON, OTHERWISE,
        WHILE, DO,
        REPEAT, UNTIL,
        TRAVERSAL, STEP,
        ITERATE,
        STOP, SKIP,
        TIMES,
        PROCEDURE, FUNCTION,
        INPUT, OUTPUT,
        POINTER, TO,
        ARRAY, OF,
        ALLOCATE, DEALLOCATE,
        
        // Logical Operators
        AND, OR, NOT,

        // Arithmetic Operators
        DIV, MOD,

        // Data Types
        INTEGER, REAL, BOOLEAN, CHARACTER, STRING,

        // Literals
        INTEGER_LITERAL, REAL_LITERAL, STRING_LITERAL, BOOLEAN_LITERAL,

        // Identifiers
        IDENTIFIER,

        // Operators
        ASSIGN, // <-
        PLUS, MINUS, MULTIPLY, DIVIDE, POWER, // + - * / ^
        EQUAL, NOT_EQUAL, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL, // = <> > >= < <=
        AMPERSAND, // &
        AT, // @
        ARROW, // ->

        // Punctuation
        LPAREN, RPAREN,     // ( )
        LBRACKET, RBRACKET, // [ ]
        LANGLE, RANGLE,     // < > (for record types)
        COLON, COMMA, DOT, DOT_DOT, // : , . ..
        PIPE, // |

        // Comments
        LBRACE, RBRACE, // { }
    };

    const std::string& TokenTypeToString(TokenType type);

    struct Token {
        TokenType type;
        std::string lexeme;
        int line;
        int column;

        std::string toString() const;
    };

    static std::unordered_map<std::string, TokenType> KEYWORDS = {
        {"PROGRAM", TokenType::PROGRAM},
        {"KAMUS", TokenType::KAMUS},
        {"ALGORITMA", TokenType::ALGORITMA},
        {"constant", TokenType::CONSTANT},
        {"type", TokenType::TYPE},
        {"if", TokenType::IF},
        {"then", TokenType::THEN},
        {"else", TokenType::ELSE},
        {"elif", TokenType::ELIF},
        {"depend", TokenType::DEPEND},
        {"on", TokenType::ON},
        {"otherwise", TokenType::OTHERWISE},
        {"while", TokenType::WHILE},
        {"do", TokenType::DO},
        {"repeat", TokenType::REPEAT},
        {"until", TokenType::UNTIL},
        {"traversal", TokenType::TRAVERSAL},
        {"step", TokenType::STEP},
        {"iterate", TokenType::ITERATE},
        {"stop", TokenType::STOP},
        {"skip", TokenType::SKIP},
        {"times", TokenType::TIMES},
        {"procedure", TokenType::PROCEDURE},
        {"function", TokenType::FUNCTION},
        {"input", TokenType::INPUT},
        {"output", TokenType::OUTPUT},
        {"pointer", TokenType::POINTER},
        {"to", TokenType::TO},
        {"array", TokenType::ARRAY},
        {"of", TokenType::OF},
        {"allocate", TokenType::ALLOCATE},
        {"deallocate", TokenType::DEALLOCATE},
        {"and", TokenType::AND},
        {"or", TokenType::OR},
        {"not", TokenType::NOT},
        {"div", TokenType::DIV},
        {"mod", TokenType::MOD},
        {"integer", TokenType::INTEGER},
        {"real", TokenType::REAL},
        {"boolean", TokenType::BOOLEAN},
        {"character", TokenType::CHARACTER},
        {"string", TokenType::STRING},
        {"true", TokenType::BOOLEAN_LITERAL},
        {"false", TokenType::BOOLEAN_LITERAL}
    };

} // namespace notal

#endif // NOTAL_TRANSPILER_TOKEN_H
