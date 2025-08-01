#pragma once
#include <string>
#include <vector>
#include <algorithm>

namespace gate::utils {

class InputValidator {
public:
    static constexpr size_t MAX_SOURCE_SIZE = 5 * 1024 * 1024; // 5MB for source code

    struct ValidationResult {
        bool isValid;
        std::string errorMessage;
        std::vector<std::string> warnings;
    };

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

    static bool isValidOutputPath(const std::string& path) {
        if (path.empty()) return false;
        if (path.find("..") != std::string::npos) return false;
        if (path.find("~") != std::string::npos) return false;
        if (path[0] == '|' || path[0] == '>') return false;
        return true;
    }
};

} // namespace gate::utils
