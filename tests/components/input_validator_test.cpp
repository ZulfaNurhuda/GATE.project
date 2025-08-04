#include <gtest/gtest.h>
#include "utils/InputValidator.h"
#include <string>
#include <vector>

TEST(InputValidatorTest, ValidNotalSource) {
    std::string validSource = R"(
PROGRAM ValidTest
KAMUS
    x: integer
ALGORITMA
    x <- 42
    output(x)
)";

    auto result = gate::utils::InputValidator::validateNotalSource(validSource);
    
    EXPECT_TRUE(result.isValid);
    EXPECT_TRUE(result.errorMessage.empty());
    EXPECT_TRUE(result.warnings.empty());
}

TEST(InputValidatorTest, EmptySource) {
    std::string emptySource = "";
    
    auto result = gate::utils::InputValidator::validateNotalSource(emptySource);
    
    EXPECT_FALSE(result.isValid);
    EXPECT_FALSE(result.errorMessage.empty());
    EXPECT_TRUE(result.errorMessage.find("empty") != std::string::npos);
}

TEST(InputValidatorTest, WhitespaceOnlySource) {
    std::string whitespaceSource = "   \n\t  \r\n   ";
    
    auto result = gate::utils::InputValidator::validateNotalSource(whitespaceSource);
    
    EXPECT_FALSE(result.isValid);
    EXPECT_FALSE(result.errorMessage.empty());
}

TEST(InputValidatorTest, SourceTooLarge) {
    // Create a source larger than MAX_SOURCE_SIZE (5MB)
    std::string largeSource;
    largeSource.reserve(gate::utils::InputValidator::MAX_SOURCE_SIZE + 1000);
    
    // Fill with valid NOTAL content
    largeSource = "PROGRAM LargeTest\nKAMUS\n";
    while (largeSource.size() < gate::utils::InputValidator::MAX_SOURCE_SIZE + 100) {
        largeSource += "    var" + std::to_string(largeSource.size()) + ": integer\n";
    }
    largeSource += "ALGORITMA\n    output('test')";
    
    auto result = gate::utils::InputValidator::validateNotalSource(largeSource);
    
    EXPECT_FALSE(result.isValid);
    EXPECT_TRUE(result.errorMessage.find("size") != std::string::npos || 
                result.errorMessage.find("large") != std::string::npos);
}

TEST(InputValidatorTest, MissingProgramKeyword) {
    std::string sourceWithoutProgram = R"(
KAMUS
    x: integer
ALGORITMA
    x <- 42
    output(x)
)";
    
    auto result = gate::utils::InputValidator::validateNotalSource(sourceWithoutProgram);
    
    EXPECT_FALSE(result.isValid);
    EXPECT_TRUE(result.errorMessage.find("PROGRAM") != std::string::npos);
}

TEST(InputValidatorTest, SecurityMaliciousPatterns) {
    std::vector<std::string> maliciousSources = {
        "PROGRAM Test\nKAMUS\nALGORITMA\n    system('rm -rf /')",
        "PROGRAM Test\nKAMUS\nALGORITMA\n    exec('malicious_command')",
        "PROGRAM Test\nKAMUS\nALGORITMA\n    eval('dangerous_code')",
        "PROGRAM Test\nKAMUS\nALGORITMA\n    __import__('os').system('bad')"
    };
    
    for (const auto& source : maliciousSources) {
        auto result = gate::utils::InputValidator::validateNotalSource(source);
        
        // Should either be invalid or have warnings about suspicious content
        if (result.isValid) {
            EXPECT_FALSE(result.warnings.empty()) << "Source: " << source;
        } else {
            EXPECT_TRUE(result.errorMessage.find("security") != std::string::npos ||
                       result.errorMessage.find("malicious") != std::string::npos ||
                       result.errorMessage.find("suspicious") != std::string::npos) 
                       << "Source: " << source;
        }
    }
}

// Note: InputValidator only provides validateNotalSource and isValidOutputPath methods
// Input path validation is not implemented in the current version

TEST(InputValidatorTest, OutputPathValidation) {
    std::vector<std::string> validOutputPaths = {
        "output.pas",
        "generated/result.pas",
        "C:\\Output\\result.pas",
        "/tmp/output.pas"
    };
    
    for (const auto& path : validOutputPaths) {
        bool result = gate::utils::InputValidator::isValidOutputPath(path);
        EXPECT_TRUE(result) << "Output path: " << path;
    }
}

TEST(InputValidatorTest, InvalidOutputPathValidation) {
    std::vector<std::string> invalidOutputPaths = {
        "",
        "   ",
        "output.txt",
        "output.notal",
        "../../../etc/passwd.pas",
        std::string(500, 'a') + ".pas"  // Too long
    };
    
    for (const auto& path : invalidOutputPaths) {
        bool result = gate::utils::InputValidator::isValidOutputPath(path);
        EXPECT_FALSE(result) << "Output path: " << path;
    }
}

TEST(InputValidatorTest, SourceWithComments) {
    std::string sourceWithComments = R"(
PROGRAM CommentTest
{ This is a valid comment }
KAMUS
    x: integer { Variable declaration }
ALGORITMA
    x <- 42 { Assignment }
    output(x) { Output statement }
)";
    
    auto result = gate::utils::InputValidator::validateNotalSource(sourceWithComments);
    
    EXPECT_TRUE(result.isValid);
    EXPECT_TRUE(result.errorMessage.empty());
}

TEST(InputValidatorTest, SourceWithSpecialCharacters) {
    std::string sourceWithSpecialChars = R"(
PROGRAM SpecialTest
KAMUS
    message: string
ALGORITMA
    message <- 'Hello, World! @#$%^&*()'
    output(message)
)";
    
    auto result = gate::utils::InputValidator::validateNotalSource(sourceWithSpecialChars);
    
    EXPECT_TRUE(result.isValid);
}