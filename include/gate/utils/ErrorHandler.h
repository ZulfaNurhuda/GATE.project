#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <chrono>
#include <algorithm>
#include <numeric>

namespace gate::utils {

enum class ErrorLevel {
    INFO = 0,
    WARNING = 1,
    ERROR = 2,
    FATAL = 3
};

struct SourceLocation {
    std::string filename;
    int line = 0;
    int column = 0;

    bool isValid() const {
        return !filename.empty() && line > 0;
    }
};

struct ErrorInfo {
    ErrorLevel level;
    std::string message;
    SourceLocation location;
    std::string suggestion;
    std::chrono::system_clock::time_point timestamp;

    ErrorInfo(ErrorLevel lvl, std::string msg, SourceLocation loc = {}, std::string hint = "")
        : level(lvl), message(std::move(msg)), location(std::move(loc)),
          suggestion(std::move(hint)), timestamp(std::chrono::system_clock::now()) {}
};

class ErrorHandler {
public:
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }

    void reportError(ErrorLevel level, const std::string& message,
                    const SourceLocation& location = {},
                    const std::string& suggestion = "") {
        errors_.emplace_back(level, message, location, suggestion);

        if (level >= ErrorLevel::ERROR) {
            printError(errors_.back());
        }
    }

    void reportError(ErrorLevel level, const std::string& message,
                    const std::string& filename, int line = 0, int column = 0,
                    const std::string& suggestion = "") {
        SourceLocation loc{filename, line, column};
        reportError(level, message, loc, suggestion);
    }

    bool hasErrors() const {
        return std::any_of(errors_.begin(), errors_.end(),
            [](const ErrorInfo& err) { return err.level >= ErrorLevel::ERROR; });
    }

    bool hasFatalErrors() const {
        return std::any_of(errors_.begin(), errors_.end(),
            [](const ErrorInfo& err) { return err.level == ErrorLevel::FATAL; });
    }

    size_t getErrorCount() const {
        return std::count_if(errors_.begin(), errors_.end(),
            [](const ErrorInfo& err) { return err.level >= ErrorLevel::ERROR; });
    }

    size_t getWarningCount() const {
        return std::count_if(errors_.begin(), errors_.end(),
            [](const ErrorInfo& err) { return err.level == ErrorLevel::WARNING; });
    }

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

#define GATE_ERROR(msg, ...) \
    gate::utils::ErrorHandler::getInstance().reportError(gate::utils::ErrorLevel::ERROR, msg, ##__VA_ARGS__)

#define GATE_WARNING(msg, ...) \
    gate::utils::ErrorHandler::getInstance().reportError(gate::utils::ErrorLevel::WARNING, msg, ##__VA_ARGS__)

#define GATE_FATAL(msg, ...) \
    gate::utils::ErrorHandler::getInstance().reportError(gate::utils::ErrorLevel::FATAL, msg, ##__VA_ARGS__)

} // namespace gate::utils
