/**
 * @file ErrorHandler.h
 * @brief Comprehensive error handling and reporting system
 * 
 * This file defines the ErrorHandler class and related structures for
 * managing errors, warnings, and informational messages throughout the
 * GATE transpiler. It provides centralized error collection, formatting,
 * and reporting with source location tracking and colored output.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */

#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <chrono>
#include <algorithm>
#include <numeric>

namespace gate::utils {

/**
 * @brief Error severity levels
 * 
 * Defines the hierarchy of error severity from informational
 * messages to fatal errors that prevent program execution.
 */
enum class ErrorLevel {
    /** @brief Informational message */
    INFO = 0,
    /** @brief Warning that doesn't prevent execution */
    WARNING = 1,
    /** @brief Error that may prevent successful compilation */
    ERROR = 2,
    /** @brief Fatal error that stops execution */
    FATAL = 3
};

/**
 * @brief Source code location information
 * 
 * Represents a specific location in source code with filename,
 * line number, and column position for precise error reporting.
 */
struct SourceLocation {
    /** @brief Source filename */
    std::string filename;
    /** @brief Line number (1-based) */
    int line = 0;
    /** @brief Column number (1-based) */
    int column = 0;

    /**
     * @brief Check if location information is valid
     * @return true if filename is not empty and line > 0
     */
    bool isValid() const {
        return !filename.empty() && line > 0;
    }
};

/**
 * @brief Complete error information structure
 * 
 * Contains all information about an error including severity level,
 * message, source location, helpful suggestions, and timestamp.
 */
struct ErrorInfo {
    /** @brief Error severity level */
    ErrorLevel level;
    /** @brief Error message text */
    std::string message;
    /** @brief Source code location where error occurred */
    SourceLocation location;
    /** @brief Helpful suggestion for fixing the error */
    std::string suggestion;
    /** @brief Timestamp when error was reported */
    std::chrono::system_clock::time_point timestamp;

    /**
     * @brief Constructor for ErrorInfo
     * @param lvl Error severity level
     * @param msg Error message
     * @param loc Source location (optional)
     * @param hint Helpful suggestion (optional)
     */
    ErrorInfo(ErrorLevel lvl, std::string msg, SourceLocation loc = {}, std::string hint = "")
        : level(lvl), message(std::move(msg)), location(std::move(loc)),
          suggestion(std::move(hint)), timestamp(std::chrono::system_clock::now()) {}
};

/**
 * @brief Singleton error handler for centralized error management
 * 
 * The ErrorHandler provides a centralized system for collecting, storing,
 * and reporting errors throughout the GATE transpiler. It supports different
 * error levels, source location tracking, colored output, and error statistics.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
class ErrorHandler {
public:
    /**
     * @brief Get singleton instance of ErrorHandler
     * @return Reference to the global ErrorHandler instance
     */
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }

    /**
     * @brief Report an error with full location information
     * @param level Error severity level
     * @param message Error message text
     * @param location Source code location (optional)
     * @param suggestion Helpful suggestion for fixing (optional)
     */
    void reportError(ErrorLevel level, const std::string& message,
                    const SourceLocation& location = {},
                    const std::string& suggestion = "") {
        errors_.emplace_back(level, message, location, suggestion);

        if (level >= ErrorLevel::ERROR) {
            printError(errors_.back());
        }
    }

    /**
     * @brief Report an error with separate location parameters
     * @param level Error severity level
     * @param message Error message text
     * @param filename Source filename
     * @param line Line number (optional)
     * @param column Column number (optional)
     * @param suggestion Helpful suggestion for fixing (optional)
     */
    void reportError(ErrorLevel level, const std::string& message,
                    const std::string& filename, int line = 0, int column = 0,
                    const std::string& suggestion = "") {
        SourceLocation loc{filename, line, column};
        reportError(level, message, loc, suggestion);
    }

    /**
     * @brief Check if any errors (ERROR or FATAL level) have been reported
     * @return true if there are errors, false otherwise
     */
    bool hasErrors() const {
        return std::any_of(errors_.begin(), errors_.end(),
            [](const ErrorInfo& err) { return err.level >= ErrorLevel::ERROR; });
    }

    /**
     * @brief Check if any fatal errors have been reported
     * @return true if there are fatal errors, false otherwise
     */
    bool hasFatalErrors() const {
        return std::any_of(errors_.begin(), errors_.end(),
            [](const ErrorInfo& err) { return err.level == ErrorLevel::FATAL; });
    }

    /**
     * @brief Get count of errors (ERROR and FATAL levels)
     * @return Number of errors reported
     */
    size_t getErrorCount() const {
        return std::count_if(errors_.begin(), errors_.end(),
            [](const ErrorInfo& err) { return err.level >= ErrorLevel::ERROR; });
    }

    /**
     * @brief Get count of warnings
     * @return Number of warnings reported
     */
    size_t getWarningCount() const {
        return std::count_if(errors_.begin(), errors_.end(),
            [](const ErrorInfo& err) { return err.level == ErrorLevel::WARNING; });
    }

    /**
     * @brief Print a summary of all errors and warnings
     * 
     * Displays a colored summary showing the total count of errors and warnings.
     * If no errors or warnings exist, displays "No errors or warnings."
     */
    void printSummary() const {
        auto errorCount = getErrorCount();
        auto warningCount = getWarningCount();

        if (errorCount > 0 || warningCount > 0) {
            std::cerr << "\nSummary: ";
            if (errorCount > 0) {
                std::cerr << errorCount << " error(s)";
                if (warningCount > 0) std::cerr << ", ";
            }
            if (warningCount > 0) {
                std::cerr << warningCount << " warning(s)";
            }
            std::cerr << std::endl;
        }
    }

    /**
     * @brief Clear all stored errors and warnings
     * 
     * Removes all error records from the handler, resetting it to a clean state.
     */
    void clear() {
        errors_.clear();
    }

private:
    ErrorHandler() = default;
    std::vector<ErrorInfo> errors_;

    void printError(const ErrorInfo& error) const {
        const char* colorCode = getColorCode(error.level);
        const char* resetColor = "\033[0m";

        std::cerr << colorCode << getLevelString(error.level) << resetColor;

        if (error.location.isValid()) {
            std::cerr << " at " << error.location.filename
                      << ":" << error.location.line;
            if (error.location.column > 0) {
                std::cerr << ":" << error.location.column;
            }
        }

        std::cerr << ": " << error.message << std::endl;

        if (!error.suggestion.empty()) {
            std::cerr << "  Suggestion: " << error.suggestion << std::endl;
        }
    }

    const char* getColorCode(ErrorLevel level) const {
        switch (level) {
            case ErrorLevel::INFO: return "\033[36m";    // Cyan
            case ErrorLevel::WARNING: return "\033[33m"; // Yellow
            case ErrorLevel::ERROR: return "\033[31m";   // Red
            case ErrorLevel::FATAL: return "\033[35m";   // Magenta
            default: return "";
        }
    }

    const char* getLevelString(ErrorLevel level) const {
        switch (level) {
            case ErrorLevel::INFO: return "Info";
            case ErrorLevel::WARNING: return "Warning";
            case ErrorLevel::ERROR: return "Error";
            case ErrorLevel::FATAL: return "Fatal";
            default: return "Unknown";
        }
    }
};

/**
 * @brief Convenience macro for reporting ERROR level messages
 * @param msg Error message
 * @param ... Optional parameters: filename, line, column, suggestion
 */
#define GATE_ERROR(msg, ...) \
    gate::utils::ErrorHandler::getInstance().reportError(gate::utils::ErrorLevel::ERROR, msg, ##__VA_ARGS__)

/**
 * @brief Convenience macro for reporting WARNING level messages
 * @param msg Warning message
 * @param ... Optional parameters: filename, line, column, suggestion
 */
#define GATE_WARNING(msg, ...) \
    gate::utils::ErrorHandler::getInstance().reportError(gate::utils::ErrorLevel::WARNING, msg, ##__VA_ARGS__)

/**
 * @brief Convenience macro for reporting FATAL level messages
 * @param msg Fatal error message
 * @param ... Optional parameters: filename, line, column, suggestion
 */
#define GATE_FATAL(msg, ...) \
    gate::utils::ErrorHandler::getInstance().reportError(gate::utils::ErrorLevel::FATAL, msg, ##__VA_ARGS__)

} // namespace gate::utils
