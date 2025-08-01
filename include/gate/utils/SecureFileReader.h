#pragma once
#include <filesystem>
#include <string>
#include <fstream>
#include <limits>
#include <system_error>

namespace gate::utils {

class SecureFileReader {
public:
    static constexpr size_t MAX_FILE_SIZE = 10 * 1024 * 1024; // 10MB limit

    struct ReadResult {
        bool success;
        std::string content;
        std::string errorMessage;
    };

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
