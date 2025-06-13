#include "../include/logger.hpp"
#include <iostream> // For std::clog, std::endl
#include <chrono>   // For timestamps
#include <iomanip>  // For std::put_time
#include <ctime>    // For std::time_t, std::localtime_r or localtime_s

// Constructor
Logger::Logger() : current_log_level(LogLevel::INFO), log_stream(&std::clog) {} // Corrected to log_stream

// Initialize with a specific log level and optionally a different output stream
void Logger::init(LogLevel level, std::ostream& stream) {
    this->current_log_level = level;
    this->log_stream = &stream; // Corrected to log_stream
}

// Set the logging level
void Logger::set_level(LogLevel level) {
    this->current_log_level = level;
}

// Get the current logging level
LogLevel Logger::get_level() const {
    return this->current_log_level;
}

// Private helper to convert LogLevel to its string representation
// This was declared const in .hpp, so should be const here.
std::string Logger::level_to_string(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::NONE:    return "NONE";
        default:                return "UNKNOWN";
    }
}

// Logging methods
void Logger::debug(const std::string& message) {
    if (log_stream && current_log_level != LogLevel::NONE && current_log_level <= LogLevel::DEBUG) { // Corrected to log_stream
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm buf{};
        localtime_r(&in_time_t, &buf);

        (*log_stream) << std::put_time(&buf, "%Y-%m-%d %H:%M:%S") << " "; // Corrected to log_stream
        (*log_stream) << "[" << level_to_string(LogLevel::DEBUG) << "] " << message << std::endl; // Corrected to log_stream
    }
}

void Logger::info(const std::string& message) {
    if (log_stream && current_log_level != LogLevel::NONE && current_log_level <= LogLevel::INFO) { // Corrected to log_stream
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm buf{};
        localtime_r(&in_time_t, &buf);

        (*log_stream) << std::put_time(&buf, "%Y-%m-%d %H:%M:%S") << " "; // Corrected to log_stream
        (*log_stream) << "[" << level_to_string(LogLevel::INFO) << "] " << message << std::endl; // Corrected to log_stream
    }
}

void Logger::warn(const std::string& message) {
    if (log_stream && current_log_level != LogLevel::NONE && current_log_level <= LogLevel::WARNING) { // Corrected to log_stream
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm buf{};
        localtime_r(&in_time_t, &buf);

        (*log_stream) << std::put_time(&buf, "%Y-%m-%d %H:%M:%S") << " "; // Corrected to log_stream
        (*log_stream) << "[" << level_to_string(LogLevel::WARNING) << "] " << message << std::endl; // Corrected to log_stream
    }
}

void Logger::error(const std::string& message) {
    if (log_stream && current_log_level != LogLevel::NONE && current_log_level <= LogLevel::ERROR) { // Corrected to log_stream
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm buf{};
        localtime_r(&in_time_t, &buf);

        (*log_stream) << std::put_time(&buf, "%Y-%m-%d %H:%M:%S") << " "; // Corrected to log_stream
        (*log_stream) << "[" << level_to_string(LogLevel::ERROR) << "] " << message << std::endl; // Corrected to log_stream
    }
}
