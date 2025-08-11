#include <gtest/gtest.h>
#include "utils/SecureFileReader.h"
#include <fstream>
#include <filesystem>
#include <string>

class SecureFileReaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory
        testDir = std::filesystem::temp_directory_path() / "gate_test";
        std::filesystem::create_directories(testDir);
        
        // Create test files
        validFile = testDir / "valid.notal";
        createTestFile(validFile, "PROGRAM Test\nKAMUS\n    x: integer\nALGORITMA\n    x <- 42\n    output(x)");
        
        emptyFile = testDir / "empty.notal";
        createTestFile(emptyFile, "");
        
        largeFile = testDir / "large.notal";
        std::string largeContent(gate::utils::SecureFileReader::MAX_FILE_SIZE + 1000, 'A');
        createTestFile(largeFile, largeContent);
    }
    
    void TearDown() override {
        // Clean up test files
        std::error_code ec;
        std::filesystem::remove_all(testDir, ec);
    }
    
    std::string normalize_line_endings(const std::string& str) {
        std::string normalized_str;
        normalized_str.reserve(str.length());
        for (char c : str) {
            if (c != '\r') {
                normalized_str += c;
            }
        }
        return normalized_str;
    }

    void createTestFile(const std::filesystem::path& path, const std::string& content) {
        std::ofstream file(path, std::ios::binary);
        file << content;
        file.close();
    }
    
    std::filesystem::path testDir;
    std::filesystem::path validFile;
    std::filesystem::path emptyFile;
    std::filesystem::path largeFile;
};

TEST_F(SecureFileReaderTest, ReadValidFile) {
    auto result = gate::utils::SecureFileReader::readFile(validFile.string());
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.errorMessage.empty());
    EXPECT_FALSE(result.content.empty());

    std::string normalized_content = normalize_line_endings(result.content);
    EXPECT_TRUE(normalized_content.find("PROGRAM Test") != std::string::npos);
    EXPECT_TRUE(normalized_content.find("x <- 42") != std::string::npos);
}

TEST_F(SecureFileReaderTest, ReadEmptyFile) {
    auto result = gate::utils::SecureFileReader::readFile(emptyFile.string());
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.content.empty());
    EXPECT_TRUE(result.errorMessage.empty());
}

TEST_F(SecureFileReaderTest, ReadNonExistentFile) {
    std::string nonExistentPath = (testDir / "nonexistent.notal").string();
    
    auto result = gate::utils::SecureFileReader::readFile(nonExistentPath);
    
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.errorMessage.empty());
    EXPECT_TRUE(result.content.empty());
    EXPECT_TRUE(result.errorMessage.find("not found") != std::string::npos ||
                result.errorMessage.find("does not exist") != std::string::npos);
}

TEST_F(SecureFileReaderTest, ReadFileTooLarge) {
    auto result = gate::utils::SecureFileReader::readFile(largeFile.string());
    
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.errorMessage.empty());
    EXPECT_TRUE(result.content.empty());
    EXPECT_TRUE(result.errorMessage.find("too large") != std::string::npos ||
                result.errorMessage.find("size") != std::string::npos);
}

TEST_F(SecureFileReaderTest, PathTraversalAttack) {
    std::vector<std::string> maliciousPaths = {
        "../../../etc/passwd",
        "..\\..\\..\\windows\\system32\\config\\sam",
        "/etc/shadow",
        "C:\\Windows\\System32\\config\\SAM",
        "../../../../root/.ssh/id_rsa"
    };
    
    for (const auto& path : maliciousPaths) {
        auto result = gate::utils::SecureFileReader::readFile(path);
        
        // Should either fail to read or detect as suspicious
        EXPECT_FALSE(result.success) << "Malicious path should be rejected: " << path;
        EXPECT_FALSE(result.errorMessage.empty()) << "Should have error message for: " << path;
    }
}

TEST_F(SecureFileReaderTest, InvalidPathCharacters) {
    std::vector<std::string> invalidPaths = {
        "",
        "   ",
        "file\0hidden.notal",
        "file<script>.notal",
        "file|pipe.notal",
        "file*wildcard.notal"
    };
    
    for (const auto& path : invalidPaths) {
        auto result = gate::utils::SecureFileReader::readFile(path);
        
        EXPECT_FALSE(result.success) << "Invalid path should be rejected: " << path;
    }
}

// NOTE: Path validation is tested indirectly through readFile method
// since isSecurePath is a private method in SecureFileReader

TEST_F(SecureFileReaderTest, FilePermissions) {
    // Create a file with restricted permissions (if supported by OS)
    std::filesystem::path restrictedFile = testDir / "restricted.notal";
    createTestFile(restrictedFile, "PROGRAM Restricted\nKAMUS\nALGORITMA\n    output('test')");
    
    // Try to restrict permissions (this might not work on all systems)
    std::error_code ec;
    std::filesystem::permissions(restrictedFile, 
                                std::filesystem::perms::owner_read, 
                                std::filesystem::perm_options::replace, ec);
    
    auto result = gate::utils::SecureFileReader::readFile(restrictedFile.string());
    
    // Should either succeed (if permissions allow) or fail gracefully
    if (!result.success) {
        EXPECT_FALSE(result.errorMessage.empty());
        EXPECT_TRUE(result.errorMessage.find("permission") != std::string::npos ||
                   result.errorMessage.find("access") != std::string::npos);
    }
}

TEST_F(SecureFileReaderTest, BinaryFileDetection) {
    // Create a binary file
    std::filesystem::path binaryFile = testDir / "binary.notal";
    std::ofstream file(binaryFile, std::ios::binary);
    
    // Write some binary data
    for (int i = 0; i < 256; ++i) {
        file.put(static_cast<char>(i));
    }
    file.close();
    
    auto result = gate::utils::SecureFileReader::readFile(binaryFile.string());
    
    // Should either detect as binary and reject, or read with warning
    if (!result.success) {
        EXPECT_TRUE(result.errorMessage.find("binary") != std::string::npos ||
                   result.errorMessage.find("invalid") != std::string::npos);
    }
}

TEST_F(SecureFileReaderTest, UnicodeContent) {
    // Create file with Unicode content
    std::filesystem::path unicodeFile = testDir / "unicode.notal";
    createTestFile(unicodeFile, "PROGRAM UnicodeTest\nKAMUS\n    message: string\nALGORITMA\n    message <- 'Hello 世界 🌍'\n    output(message)");
    
    auto result = gate::utils::SecureFileReader::readFile(unicodeFile.string());
    
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.content.empty());
    EXPECT_TRUE(result.content.find("PROGRAM UnicodeTest") != std::string::npos);
}