/**
 * @file InputValidator.h
 * @brief Input validation utilities for NOTAL source code
 * 
 * This file defines the InputValidator class that provides static methods
 * for validating NOTAL source code input and output paths. It includes
 * security checks to prevent malicious content and ensures input meets
 * basic requirements for safe processing.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */

#pragma once
#include <string>
#include <vector>
#include <algorithm>

namespace gate::utils {

/**
 * @brief Utility class for validating NOTAL source code and file paths
 * 
 * The InputValidator provides static methods for validating NOTAL source code
 * input to ensure it meets safety and structural requirements. It performs
 * size checks, content validation, and security screening to prevent
 * malicious code execution.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
class InputValidator {
public:
    /** @brief Maximum allowed source code size (5MB) */
    static constexpr size_t MAX_SOURCE_SIZE = 5 * 1024 * 1024;

    /**
     * @brief Result structure for validation operations
     * 
     * Contains validation status, error messages, and warnings
     * generated during the validation process.
     */
    struct ValidationResult {
        /** @brief Whether the validation passed */
        bool isValid;
        /** @brief Error message if validation failed */
        std::string errorMessage;
        /** @brief List of warnings (non-fatal issues) */
        std::vector<std::string> warnings;
    };

    /**
     * @brief Validate NOTAL source code for safety and structure
     * @param source The NOTAL source code to validate
     * @return ValidationResult containing validation status and messages
     * 
     * Performs comprehensive validation including:
     * - Size limits (max 5MB)
     * - Empty content check
     * - Basic structure validation (PROGRAM keyword)
     * - Security screening for malicious patterns
     */
    static ValidationResult validateNotalSource(const std::string& source) {
        ValidationResult result{true, "", {}};

        // Size check
        if (source.size() > MAX_SOURCE_SIZE) {
            result.isValid = false;
            result.errorMessage = "Source code too large (max 5MB)";
            return result;
        }

        // Empty check
        if (source.empty()) {
            result.isValid = false;
            result.errorMessage = "Source code is empty";
            return result;
        }

        // Basic structure validation
        if (source.find("PROGRAM") == std::string::npos) {
            result.warnings.push_back("No PROGRAM declaration found");
        }

        // Check for suspicious patterns
        std::vector<std::string> suspicious = {
            "<script", "<?php", "#!/bin/", "cmd.exe", "powershell"
        };

        for (const auto& pattern : suspicious) {
            if (source.find(pattern) != std::string::npos) {
                result.isValid = false;
                result.errorMessage = "Source contains potentially malicious content";
                return result;
            }
        }

        return result;
    }

    /**
     * @brief Validate output file path for security
     * @param path The output file path to validate
     * @return true if path is safe, false otherwise
     * 
     * Checks for potentially dangerous path patterns:
     * - Directory traversal attempts (..)
     * - Home directory references (~)
     * - Command injection attempts (|, >)
     */
    static bool isValidOutputPath(const std::string& path) {
        if (path.empty()) return false;
        if (path.find("..") != std::string::npos) return false;
        if (path.find("~") != std::string::npos) return false;
        if (path[0] == '|' || path[0] == '>') return false;
        return true;
    }
};

} // namespace gate::utils
