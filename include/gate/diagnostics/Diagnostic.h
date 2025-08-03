/* include/gate/diagnostics/Diagnostic.h */

#ifndef GATE_DIAGNOSTICS_DIAGNOSTIC_H
#define GATE_DIAGNOSTICS_DIAGNOSTIC_H

#include <string>
#include <vector>
#include <memory>

namespace gate::diagnostics {

/* Error severity levels */
enum class DiagnosticLevel {
    INFO,     /* Informational messages */
    WARNING,  /* Potential issues that don't prevent compilation */
    ERROR,    /* Errors that prevent successful compilation */
    FATAL     /* Unrecoverable errors that stop processing */
};

/* Categories for better error organization */
enum class DiagnosticCategory {
    LEXICAL_ERROR,        /* Token recognition errors */
    SYNTAX_ERROR,         /* Grammar violations */
    SEMANTIC_ERROR,       /* Semantic rule violations */
    TYPE_ERROR,           /* Type system errors */
    DECLARATION_ERROR,    /* Variable/function declaration issues */
    MEMORY_ERROR,         /* Memory management errors */
    CONSTRAINT_ERROR      /* Constraint violation errors */
};

/* Source location information */
struct SourceLocation {
    std::string filename;
    size_t line;
    size_t column;
    size_t length;  /* Length of the error span */

    SourceLocation(const std::string& file, size_t l, size_t c, size_t len = 1)
        : filename(file), line(l), column(c), length(len) {}

    SourceLocation() : filename(""), line(0), column(0), length(0) {}
};

/* Diagnostic message with all error information */
struct Diagnostic {
    DiagnosticLevel level;
    DiagnosticCategory category;
    std::string code;           /* Error code (e.g., "E0001") */
    std::string message;        /* Primary error message */
    SourceLocation location;    /* Where the error occurred */
    std::string context;        /* Source code context */
    std::vector<std::string> notes;      /* Additional notes */
    std::vector<std::string> suggestions; /* Fix suggestions */

    /* Builder pattern for construction */
    class Builder {
    private:
        DiagnosticLevel level_ = DiagnosticLevel::ERROR;
        DiagnosticCategory category_ = DiagnosticCategory::SYNTAX_ERROR;
        std::string code_;
        std::string message_;
        SourceLocation location_;
        std::string context_;
        std::vector<std::string> notes_;
        std::vector<std::string> suggestions_;

    public:
        Builder(const std::string& message, const SourceLocation& location)
            : message_(message), location_(location) {}

        Builder& withLevel(DiagnosticLevel level) {
            level_ = level;
            return *this;
        }
        Builder& withCategory(DiagnosticCategory category) {
            category_ = category;
            return *this;
        }
        Builder& withCode(const std::string& code) {
            code_ = code;
            return *this;
        }
        Builder& withContext(const std::string& context) {
            context_ = context;
            return *this;
        }
        Builder& addNote(const std::string& note) {
            notes_.push_back(note);
            return *this;
        }
        Builder& addSuggestion(const std::string& suggestion) {
            suggestions_.push_back(suggestion);
            return *this;
        }
        Diagnostic build() {
            return {level_, category_, code_, message_, location_, context_, notes_, suggestions_};
        }
    };
};

} // namespace gate::diagnostics

#endif // GATE_DIAGNOSTICS_DIAGNOSTIC_H
