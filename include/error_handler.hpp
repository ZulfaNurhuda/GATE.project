#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include <string>
#include <iostream> // For std::cerr or a custom log stream

// Definition for ErrorCode Enum
enum class ErrorCode {
    // Lexical Errors
    LEXICAL_ERROR, // Generic lexical error
    LEXICAL_UNRECOGNIZED_TOKEN,
    LEXICAL_INVALID_STRING_LITERAL,
    LEXICAL_UNTERMINATED_COMMENT,
    LEXICAL_INVALID_NUMBER_FORMAT,

    // Syntax/Parsing Errors
    SYNTAX_ERROR, // Generic syntax error
    SYNTAX_UNEXPECTED_TOKEN,
    SYNTAX_MISSING_EXPECTED_TOKEN, // e.g., missing semicolon, parenthesis
    SYNTAX_INVALID_EXPRESSION,
    SYNTAX_INVALID_STATEMENT,
    SYNTAX_PROGRAM_STRUCTURE_ERROR, // e.g., KAMUS before PROGRAM

    // Semantic Errors
    SEMANTIC_ERROR, // Generic semantic error
    SEMANTIC_UNDECLARED_IDENTIFIER,
    SEMANTIC_REDEFINITION_IDENTIFIER, // Variable, function, type
    SEMANTIC_TYPE_MISMATCH,
    SEMANTIC_INVALID_ARGUMENT_COUNT,
    SEMANTIC_INVALID_OPERATION,      // e.g., applying DIV to strings
    SEMANTIC_RETURN_TYPE_MISMATCH,
    SEMANTIC_ASSIGNMENT_TO_NON_VARIABLE, // Assigning to literal or const

    // File/System Errors
    FILE_OPEN_ERROR,
    FILE_READ_ERROR,
    FILE_WRITE_ERROR,
    MEMORY_ALLOCATION_ERROR,

    // General/Internal/Tooling
    NOT_IMPLEMENTED_ERROR,
    INTERNAL_COMPILER_ERROR,
    GENERAL_ERROR, // For messages not fitting elsewhere
    UNKNOWN_ERROR  // Fallback
};

class ErrorHandler {
public:
    // Tracks if any error has been reported during a compilation/transpilation phase
    static bool an_error_occurred;

    // Main reporting function
    // 'line' and 'col' are 1-based for user display.
    // 'message_detail' provides specific context for this instance of the error.
    static void report(ErrorCode code, int line, int col, const std::string& message_detail);

    // Optional: Helper to convert ErrorCode to a general string representation or prefix
    // This can be implemented in the .cpp file.
    static std::string code_to_string_prefix(ErrorCode code);
};

#endif // ERROR_HANDLER_HPP