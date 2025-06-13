#include "../include/error_handler.hpp"
#include <iostream> // For std::cerr
#include <string>   // For std::string

// Initialize static member
bool ErrorHandler::an_error_occurred = false;

// Helper to convert ErrorCode to a string prefix
std::string ErrorHandler::code_to_string_prefix(ErrorCode code) {
    switch (code) {
        // Lexical Errors
        case ErrorCode::LEXICAL_ERROR:                   return "[LEX_E000]";
        case ErrorCode::LEXICAL_UNRECOGNIZED_TOKEN:    return "[LEX_E001]";
        case ErrorCode::LEXICAL_INVALID_STRING_LITERAL: return "[LEX_E002]";
        case ErrorCode::LEXICAL_UNTERMINATED_COMMENT:  return "[LEX_E003]";
        case ErrorCode::LEXICAL_INVALID_NUMBER_FORMAT: return "[LEX_E004]";

        // Syntax/Parsing Errors
        case ErrorCode::SYNTAX_ERROR:                   return "[SYN_E000]";
        case ErrorCode::SYNTAX_UNEXPECTED_TOKEN:         return "[SYN_E001]";
        case ErrorCode::SYNTAX_MISSING_EXPECTED_TOKEN:  return "[SYN_E002]";
        case ErrorCode::SYNTAX_INVALID_EXPRESSION:       return "[SYN_E003]";
        case ErrorCode::SYNTAX_INVALID_STATEMENT:       return "[SYN_E004]";
        case ErrorCode::SYNTAX_PROGRAM_STRUCTURE_ERROR: return "[SYN_E005]";

        // Semantic Errors
        case ErrorCode::SEMANTIC_ERROR:                   return "[SEM_E000]";
        case ErrorCode::SEMANTIC_UNDECLARED_IDENTIFIER:  return "[SEM_E001]";
        case ErrorCode::SEMANTIC_REDEFINITION_IDENTIFIER: return "[SEM_E002]";
        case ErrorCode::SEMANTIC_TYPE_MISMATCH:          return "[SEM_E003]";
        case ErrorCode::SEMANTIC_INVALID_ARGUMENT_COUNT:return "[SEM_E004]";
        case ErrorCode::SEMANTIC_INVALID_OPERATION:     return "[SEM_E005]";
        case ErrorCode::SEMANTIC_RETURN_TYPE_MISMATCH:  return "[SEM_E006]";
        case ErrorCode::SEMANTIC_ASSIGNMENT_TO_NON_VARIABLE: return "[SEM_E007]";


        // File/System Errors
        case ErrorCode::FILE_OPEN_ERROR:             return "[SYS_E001]";
        case ErrorCode::FILE_READ_ERROR:             return "[SYS_E002]";
        case ErrorCode::FILE_WRITE_ERROR:            return "[SYS_E003]";
        case ErrorCode::MEMORY_ALLOCATION_ERROR:     return "[SYS_E004]";

        // General/Internal/Tooling
        case ErrorCode::NOT_IMPLEMENTED_ERROR:       return "[GEN_E001]";
        case ErrorCode::INTERNAL_COMPILER_ERROR:     return "[GEN_E002]";
        case ErrorCode::GENERAL_ERROR:               return "[GEN_E003]";
        case ErrorCode::UNKNOWN_ERROR:               return "[GEN_E004]";

        default:                                     return "[ERR_UNKNOWN]"; // Fallback for unmapped codes
    }
}

// Main reporting function
void ErrorHandler::report(ErrorCode code, int line, int col, const std::string& message_detail) {
    an_error_occurred = true; // Set the flag indicating an error has occurred
    std::string prefix = code_to_string_prefix(code);

    // Output error to std::cerr
    // Line and column are expected to be 1-based for user display.
    // If line or col is 0 or negative, it might indicate an error not tied to a specific location.
    if (line > 0 && col > 0) {
        std::cerr << "Error " << prefix << " (L" << line << ", C" << col << "): " << message_detail << std::endl;
    } else if (line > 0) { // Only line number is valid
        std::cerr << "Error " << prefix << " (L" << line << "): " << message_detail << std::endl;
    } else { // No valid location info
        std::cerr << "Error " << prefix << ": " << message_detail << std::endl;
    }
}