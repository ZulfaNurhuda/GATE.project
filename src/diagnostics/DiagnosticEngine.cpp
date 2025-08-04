#include "diagnostics/DiagnosticEngine.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iomanip>

namespace gate::diagnostics {

// Define color codes for terminal output
const char* RESET_COLOR = "\033[0m";
const char* RED_COLOR = "\033[31m";
const char* YELLOW_COLOR = "\033[33m";
const char* BLUE_COLOR = "\033[34m";
const char* CYAN_COLOR = "\033[36m";

// Error message templates (can be expanded)
const std::map<std::string, std::string> DiagnosticEngine::ERROR_TEMPLATES = {
    {"E0001", "Unknown syntax error"},
    {"E0012", "Type mismatch in assignment"},
    {"E0025", "Undefined variable"},
    {"W0003", "Unused variable"}
};

DiagnosticEngine::DiagnosticEngine(const std::string& source, const std::string& filename)
    : sourceCode_(source), filename_(filename), treatWarningsAsErrors_(false),
      errorCount_(0), warningCount_(0) {}

void DiagnosticEngine::clear() {
    diagnostics_.clear();
    errorCount_ = 0;
    warningCount_ = 0;
}

void DiagnosticEngine::report(Diagnostic diagnostic) {
    // The context will be generated on-the-fly when the report is created.
    diagnostics_.push_back(diagnostic);

    if (diagnostic.level == DiagnosticLevel::WARNING) {
        warningCount_++;
        if (treatWarningsAsErrors_) {
            errorCount_++;
        }
    } else if (diagnostic.level >= DiagnosticLevel::ERROR) {
        errorCount_++;
    }

    if (customHandler_) {
        customHandler_(diagnostic);
    }
}

// --- Convenience Methods ---

void DiagnosticEngine::reportSyntaxError(const SourceLocation& location, const std::string& message) {
    Diagnostic diagnostic = Diagnostic::Builder(message, location)
        .withLevel(DiagnosticLevel::ERROR)
        .withCategory(DiagnosticCategory::SYNTAX_ERROR)
        .withCode("E0001")
        .build();
    report(diagnostic);
}

void DiagnosticEngine::reportTypeError(const SourceLocation& location, const std::string& expected, const std::string& actual) {
    std::string message = "Cannot assign value of type '" + actual + "' to variable of type '" + expected + "'";
    Diagnostic diagnostic = Diagnostic::Builder(message, location)
        .withLevel(DiagnosticLevel::ERROR)
        .withCategory(DiagnosticCategory::TYPE_ERROR)
        .withCode("E0012")
        .build();
    report(diagnostic);
}

void DiagnosticEngine::reportUndefinedVariable(const SourceLocation& location, const std::string& variableName) {
    std::string message = "Variable '" + variableName + "' is not defined in this scope";
    Diagnostic diagnostic = Diagnostic::Builder(message, location)
        .withLevel(DiagnosticLevel::ERROR)
        .withCategory(DiagnosticCategory::DECLARATION_ERROR)
        .withCode("E0025")
        .build();
    report(diagnostic);
}

// --- Formatting and Reporting ---

std::string DiagnosticEngine::generateReport() const {
    std::stringstream ss;
    for (const auto& diagnostic : diagnostics_) {
        ss << formatDiagnostic(diagnostic);
    }

    if (hasErrors() || hasWarnings()) {
        ss << "\nCompilation " << (hasErrors() ? "failed" : "succeeded") << ": ";
        if (hasErrors()) {
            ss << errorCount_ << " error(s)";
        }
        if (hasWarnings()) {
            if (hasErrors()) ss << ", ";
            ss << warningCount_ << " warning(s)";
        }
        ss << "\n";
    }
    return ss.str();
}

std::string DiagnosticEngine::formatDiagnostic(const Diagnostic& diag) const {
    std::stringstream ss;
    const char* levelColor = RESET_COLOR;
    std::string levelString;
    std::string categoryString;

    switch (diag.level) {
        case DiagnosticLevel::ERROR:
        case DiagnosticLevel::FATAL:
            levelColor = RED_COLOR;
            levelString = (diag.level == DiagnosticLevel::FATAL) ? "Fatal" : "Error";
            break;
        case DiagnosticLevel::WARNING:
            levelColor = YELLOW_COLOR;
            levelString = "Warning";
            break;
        case DiagnosticLevel::INFO:
            levelColor = BLUE_COLOR;
            levelString = "Info";
            break;
    }

    // Add category information for specific error types
    switch (diag.category) {
        case DiagnosticCategory::TYPE_ERROR:
            categoryString = "Type error";
            break;
        case DiagnosticCategory::DECLARATION_ERROR:
            categoryString = "Undefined variable";
            break;
        default:
            categoryString = "";
            break;
    }

    ss << levelColor << levelString << "[" << diag.code << "]: " << RESET_COLOR;
    if (!categoryString.empty()) {
        ss << categoryString << " - ";
    }
    ss << diag.message << "\n";
    ss << "   " << BLUE_COLOR << "--> " << RESET_COLOR << diag.location.filename << ":" << diag.location.line << ":" << diag.location.column << "\n";

    // Generate and append the source context with highlighting
    ss << extractSourceContext(diag.location, levelColor, diag.message);

    for (const auto& note : diag.notes) {
        ss << "   " << CYAN_COLOR << "= note: " << RESET_COLOR << note << "\n";
    }

    for (const auto& suggestion : diag.suggestions) {
         ss << "   " << CYAN_COLOR << "= help: " << RESET_COLOR << suggestion << "\n";
    }

    return ss.str();
}

std::string DiagnosticEngine::extractSourceContext(const SourceLocation& location, const char* levelColor, const std::string& message) const {
    std::stringstream ss;
    std::stringstream sourceStream(sourceCode_);
    std::string lineContent;
    size_t currentLine = 1;

    if (location.line == 0) return ""; // Cannot extract context if line is 0

    // Show 1 line of context before the error line
    size_t startLine = (location.line > 1) ? location.line - 1 : 1;
    size_t endLine = location.line;

    ss << "   " << BLUE_COLOR << "|" << RESET_COLOR << "\n";

    while (currentLine <= endLine && std::getline(sourceStream, lineContent)) {
        if (currentLine >= startLine) {
            if (!lineContent.empty() && (lineContent.back() == '\r' || lineContent.back() == '\n')) {
                lineContent.pop_back();
            }

            ss << std::setw(2) << currentLine << " " << BLUE_COLOR << "| " << RESET_COLOR << lineContent << "\n";

            if (currentLine == location.line) {
                ss << "   " << BLUE_COLOR << "| " << RESET_COLOR;
                // Add padding for the column number
                for (size_t i = 1; i < location.column; ++i) {
                    ss << ' ';
                }
                ss << levelColor << std::string(location.length, '^') << " " << message << RESET_COLOR << "\n";
            }
        }
        currentLine++;
    }

    ss << "   " << BLUE_COLOR << "|" << RESET_COLOR << "\n";
    return ss.str();
}


} // namespace gate::diagnostics
