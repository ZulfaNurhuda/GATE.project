/**
 * @file SecureFileReader.h
 * @brief Secure file reading utilities with safety checks
 * 
 * This file defines the SecureFileReader class that provides secure
 * file reading capabilities with built-in safety checks including
 * path validation, size limits, and protection against common
 * file system security vulnerabilities.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */

#pragma once
#include <filesystem>
#include <string>
#include <fstream>
#include <limits>
#include <system_error>

namespace gate::utils {

/**
 * @brief Secure file reader with safety validation
 * 
 * The SecureFileReader provides static methods for safely reading files
 * with comprehensive security checks. It validates file paths, enforces
 * size limits, and protects against common file system vulnerabilities
 * such as path traversal attacks.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
class SecureFileReader {
public:
    /** @brief Maximum allowed file size (10MB) */
    static constexpr size_t MAX_FILE_SIZE = 10 * 1024 * 1024;

    /**
     * @brief Result structure for file reading operations
     * 
     * Contains the success status, file content, and any error
     * messages generated during the reading process.
     */
    struct ReadResult {
        /** @brief Whether the file reading succeeded */
        bool success;
        /** @brief File content (if successful) */
        std::string content;
        /** @brief Error message (if failed) */
        std::string errorMessage;
    };

    /**
     * @brief Securely read file content with safety checks
     * @param path The filesystem path to the file to read
     * @return ReadResult containing success status, content, and error messages
     * 
     * Performs comprehensive security validation:
     * - Path security validation (prevents traversal attacks)
     * - File existence and accessibility checks
     * - File size validation (max 10MB)
     * - Safe binary file reading
     */
    static ReadResult readFile(const std::filesystem::path& path) {
        // Validate path for security
        if (!isSecurePath(path)) {
            return {false, "", "Invalid or potentially unsafe file path"};
        }

        // Check if file exists and is readable
        std::error_code ec;
        if (!std::filesystem::exists(path, ec) || ec) {
            return {false, "", "File does not exist: " + path.string()};
        }

        // Check file size
        auto fileSize = std::filesystem::file_size(path, ec);
        if (ec || fileSize > MAX_FILE_SIZE) {
            return {false, "", "File too large or cannot determine size"};
        }

        // Safe file reading
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return {false, "", "Cannot open file for reading"};
        }

        std::string content;
        content.reserve(fileSize);
        content.assign(std::istreambuf_iterator<char>(file),
                      std::istreambuf_iterator<char>());

        return {true, std::move(content), ""};
    }

private:
    /**
     * @brief Validate file path for security vulnerabilities
     * @param path The filesystem path to validate
     * @return true if path is secure, false otherwise
     * 
     * Checks for common security issues:
     * - Path traversal attempts (..)
     * - Home directory references (~)
     * - Command injection patterns (|)
     */
    static bool isSecurePath(const std::filesystem::path& path) {
        std::string pathStr = path.string();

        // Prevent path traversal
        if (pathStr.find("..") != std::string::npos) return false;
        if (pathStr.find("~") != std::string::npos) return false;

        // Ensure it's a regular file path
        if (pathStr.empty() || pathStr[0] == '|') return false;

        return true;
    }
};

} // namespace gate::utils
