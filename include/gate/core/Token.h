/**
 * @file Token.h
 * @brief Token definitions for NOTAL Language Lexical Analysis
 * 
 * This file defines the Token structure and TokenType enumeration used
 * throughout the NOTAL language compiler for lexical analysis. It includes
 * all token types for keywords, operators, literals, identifiers, and
 * punctuation marks, along with utility functions for token manipulation.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */

#ifndef GATE_CORE_TOKEN_H
#define GATE_CORE_TOKEN_H

#include <string>
#include <unordered_map>

namespace gate::core {

    /**
     * @brief Enumeration of all token types in the NOTAL language
     * 
     * This enumeration defines all possible token types that can be
     * recognized by the lexical analyzer, including keywords, operators,
     * literals, identifiers, and punctuation marks.
     * 
     * @author GATE Project Team
     * @version 1.0
     * @date 2025
     */
    enum class TokenType {
        // Special tokens
        /** @brief Unknown or invalid token */
        UNKNOWN,
        /** @brief End of file marker */
        END_OF_FILE,

        // Program structure keywords
        /** @brief PROGRAM keyword - program declaration */
        PROGRAM,
        /** @brief KAMUS keyword - dictionary/declarations section */
        KAMUS,
        /** @brief ALGORITMA keyword - algorithm/implementation section */
        ALGORITMA,
        /** @brief constant keyword - constant declaration */
        CONSTANT,
        /** @brief type keyword - type declaration */
        TYPE,

        // Control flow keywords
        /** @brief if keyword - conditional statement */
        IF,
        /** @brief then keyword - if statement body */
        THEN,
        /** @brief else keyword - alternative branch */
        ELSE,
        /** @brief elif keyword - else if branch */
        ELIF,
        /** @brief depend keyword - switch statement */
        DEPEND,
        /** @brief on keyword - switch case */
        ON,
        /** @brief otherwise keyword - default case */
        OTHERWISE,

        // Loop keywords
        /** @brief while keyword - while loop */
        WHILE,
        /** @brief do keyword - loop body */
        DO,
        /** @brief repeat keyword - repeat-until loop */
        REPEAT,
        /** @brief until keyword - repeat condition */
        UNTIL,
        /** @brief traversal keyword - traversal loop */
        TRAVERSAL,
        /** @brief step keyword - traversal step */
        STEP,
        /** @brief iterate keyword - iteration control */
        ITERATE,
        /** @brief stop keyword - break statement */
        STOP,
        /** @brief skip keyword - continue statement */
        SKIP,
        /** @brief times keyword - repeat count */
        TIMES,

        // Subprogram keywords
        /** @brief procedure keyword - procedure declaration */
        PROCEDURE,
        /** @brief function keyword - function declaration */
        FUNCTION,

        // I/O keywords
        /** @brief input keyword - input statement */
        INPUT,
        /** @brief output keyword - output statement */
        OUTPUT,

        // Memory management keywords
        /** @brief pointer keyword - pointer type */
        POINTER,
        /** @brief to keyword - pointer target */
        TO,
        /** @brief array keyword - array type */
        ARRAY,
        /** @brief of keyword - array element type */
        OF,
        /** @brief allocate keyword - dynamic allocation */
        ALLOCATE,
        /** @brief deallocate keyword - memory deallocation */
        DEALLOCATE,
        
        // Logical operators
        /** @brief and keyword - logical AND */
        AND,
        /** @brief or keyword - logical OR */
        OR,
        /** @brief not keyword - logical NOT */
        NOT,
        /** @brief xor keyword - logical XOR */
        XOR,

        // Arithmetic operators
        /** @brief div keyword - integer division */
        DIV,
        /** @brief mod keyword - modulo operation */
        MOD,

        // Data type keywords
        /** @brief integer keyword - integer type */
        INTEGER,
        /** @brief real keyword - real number type */
        REAL,
        /** @brief boolean keyword - boolean type */
        BOOLEAN,
        /** @brief character keyword - character type */
        CHARACTER,
        /** @brief string keyword - string type */
        STRING,
        /** @brief NULL keyword - null type */
        NULL_TYPE,

        // Literal values
        /** @brief Integer literal (e.g., 42, -17) */
        INTEGER_LITERAL,
        /** @brief Real number literal (e.g., 3.14, -2.5) */
        REAL_LITERAL,
        /** @brief String literal (e.g., "hello", 'world') */
        STRING_LITERAL,
        /** @brief Boolean literal (true, false) */
        BOOLEAN_LITERAL,
        /** @brief Null literal */
        NULL_LITERAL,

        // Identifiers
        /** @brief User-defined identifier (variable, function names) */
        IDENTIFIER,

        // Assignment and arithmetic operators
        /** @brief Assignment operator (<-) */
        ASSIGN,
        /** @brief Addition operator (+) */
        PLUS,
        /** @brief Subtraction operator (-) */
        MINUS,
        /** @brief Multiplication operator (*) */
        MULTIPLY,
        /** @brief Division operator (/) */
        DIVIDE,
        /** @brief Power operator (^) */
        POWER,

        // Comparison operators
        /** @brief Equality operator (=) */
        EQUAL,
        /** @brief Inequality operator (<>) */
        NOT_EQUAL,
        /** @brief Greater than operator (>) */
        GREATER,
        /** @brief Greater than or equal operator (>=) */
        GREATER_EQUAL,
        /** @brief Less than operator (<) */
        LESS,
        /** @brief Less than or equal operator (<=) */
        LESS_EQUAL,
        // Special operators
        /** @brief Ampersand operator (&) - address-of */
        AMPERSAND,
        /** @brief At symbol (@) - special operations */
        AT,
        /** @brief Arrow operator (->) - pointer dereference */
        ARROW,

        // Punctuation marks
        /** @brief Left parenthesis ( */
        LPAREN,
        /** @brief Right parenthesis ) */
        RPAREN,
        /** @brief Left bracket [ */
        LBRACKET,
        /** @brief Right bracket ] */
        RBRACKET,
        /** @brief Left angle bracket < (for record types) */
        LANGLE,
        /** @brief Right angle bracket > (for record types) */
        RANGLE,
        /** @brief Colon : */
        COLON,
        /** @brief Comma , */
        COMMA,
        /** @brief Dot . */
        DOT,
        /** @brief Double dot .. (range operator) */
        DOT_DOT,
        /** @brief Pipe | (union separator) */
        PIPE,

        // Delimiters
        /** @brief Left brace { */
        LBRACE,
        /** @brief Right brace } */
        RBRACE,
    };

    /**
     * @brief Convert TokenType to string representation
     * 
     * Converts a TokenType enumeration value to its corresponding
     * string representation for debugging and error reporting.
     * 
     * @param type The TokenType to convert
     * @return String representation of the token type
     * 
     * @author GATE Project Team
     * @version 1.0
     * @date 2025
     */
    const std::string& tokenTypeToString(TokenType type);

    /**
     * @brief Token structure representing a lexical token
     * 
     * Represents a single token produced by the lexical analyzer,
     * containing the token type, lexeme (actual text), and position
     * information for error reporting and debugging.
     * 
     * @author GATE Project Team
     * @version 1.0
     * @date 2025
     */
    struct Token {
        /** @brief Type of the token */
        TokenType type;
        /** @brief Actual text of the token from source code */
        std::string lexeme;
        /** @brief Source file where the token is located */
        std::string filename;
        /** @brief Line number where token appears */
        int line;
        /** @brief Column number where token starts */
        int column;

        /**
         * @brief Convert token to string representation
         * @return String representation of the token
         */
        std::string toString() const;
    };

    /**
     * @brief Keyword lookup table
     * 
     * Maps NOTAL language keywords to their corresponding TokenType values.
     * Used by the lexical analyzer to distinguish between keywords and
     * user-defined identifiers.
     * 
     * @author GATE Project Team
     * @version 1.0
     * @date 2025
     */
    static const std::unordered_map<std::string, TokenType> KEYWORDS = {
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
        {"xor", TokenType::XOR},
        {"div", TokenType::DIV},
        {"mod", TokenType::MOD},
        {"integer", TokenType::INTEGER},
        {"real", TokenType::REAL},
        {"boolean", TokenType::BOOLEAN},
        {"character", TokenType::CHARACTER},
        {"string", TokenType::STRING},
        {"NULL", TokenType::NULL_TYPE},
        {"true", TokenType::BOOLEAN_LITERAL},
        {"false", TokenType::BOOLEAN_LITERAL}
    };

} // namespace gate::core

#endif // GATE_CORE_TOKEN_H
