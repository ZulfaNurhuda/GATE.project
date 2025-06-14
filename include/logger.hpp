#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream> // For std::ostream, std::clog, std::cerr

// LogLevel Enum Definition
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    NONE // To disable all logging
};

class Logger {
private:
    LogLevel current_log_level;
    std::ostream* log_stream; // Pointer to the output stream

public:
    // Optional helper to convert LogLevel to its string representation for prefixes
    // This can be implemented in the .cpp file
    std::string level_to_string(LogLevel level) const;

    // Default constructor
    Logger();

    // Initialize with a specific log level and optionally a different output stream
    void init(LogLevel level, std::ostream& stream = std::clog);

    // Set the logging level
    void set_level(LogLevel level);

    // Get the current logging level
    LogLevel get_level() const;

    // Logging methods for different severities
    // These methods will check the current_log_level before writing to the stream.
    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
};

#endif // LOGGER_HPP