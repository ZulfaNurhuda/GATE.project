/**
 * @file main.cpp
 * @brief Main entry point for the GATE transpiler - converts NOTAL language to Pascal
 * 
 * This file contains the main function that orchestrates the transpilation process
 * from NOTAL (a pseudocode-like language) to Pascal. The transpilation involves
 * several stages: lexical analysis, parsing, and code generation.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <cxxopts.hpp>

// GATE transpiler components
#include "gate/transpiler/NotalLexer.h"
#include "gate/transpiler/NotalParser.h"
#include "gate/transpiler/PascalCodeGenerator.h"
#include "gate/core/Token.h"
#include "gate/ast/Statement.h"
#include "gate/ast/Expression.h"
#include "gate/utils/SecureFileReader.h"
#include "gate/utils/InputValidator.h"
#include "gate/utils/ErrorHandler.h"

/**
 * @brief Removes comments from NOTAL source code before transpilation
 * 
 * This function performs a pre-processing step to remove all comments from the
 * NOTAL source code. Comments in NOTAL are enclosed in curly braces { }.
 * The function uses regex to find and replace all comment blocks with spaces
 * to maintain proper token separation.
 * 
 * @param source The original NOTAL source code containing comments
 * @return std::string The source code with all comments removed
 * 
 * @note Uses [\s\S] pattern to match any character including newlines,
 *       providing a portable alternative to the dotall regex flag
 * @note Replaces comments with spaces rather than empty strings to ensure
 *       proper token separation in the lexical analysis phase
 */
std::string removeComments(const std::string& source) {
    // Using [\s\S] to match any character including newlines, as a portable alternative to dotall flag.
    // Replace comment with a space to ensure tokens are separated.
    return std::regex_replace(source, std::regex("\\{[\\s\\S]*?\\}"), " ");
}

/**
 * @brief Main function - Entry point for the GATE transpiler application
 * 
 * This function handles command-line argument parsing, file I/O operations,
 * and orchestrates the complete transpilation pipeline from NOTAL to Pascal.
 * The transpilation process includes:
 * 1. Command-line argument validation
 * 2. Input file reading and validation
 * 3. Comment removal (pre-processing)
 * 4. Lexical analysis (tokenization)
 * 5. Syntax analysis (parsing to AST)
 * 6. Code generation (Pascal output)
 * 
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return int Exit code: 0 for success, 1 for failure
 * 
 * @note Uses cxxopts library for robust command-line argument parsing
 * @note Implements comprehensive error handling with detailed error messages
 * @note Supports both file output and console output modes
 */
int main(int argc, char* argv[]) {
    // Initialize error handler for this transpilation session
    gate::utils::ErrorHandler::getInstance().clear();

    try {
        // Configure command-line options using cxxopts library
        cxxopts::Options options("gate", "A transpiler from NOTAL to Pascal.");

        options.add_options()
            ("i,input", "Input NOTAL file", cxxopts::value<std::string>())
            ("o,output", "Output Pascal file (optional)", cxxopts::value<std::string>()->default_value(""))
            ("h,help", "Print usage");

        options.parse_positional("input");
        options.positional_help("[<input file>]");

        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        if (!result.count("input")) {
            GATE_ERROR("Input file not specified.", "Use the -i or --input flag.", 0, 0, "Provide an input file, e.g., 'gate -i myprogram.notal'");
            gate::utils::ErrorHandler::getInstance().printSummary();
            return 1;
        }

        // Extract command-line arguments
        std::string inputFile = result["input"].as<std::string>();
        std::string outputFile = result["output"].as<std::string>();

        // Validate output file path for security (prevent path traversal attacks)
        if (!outputFile.empty() && !gate::utils::InputValidator::isValidOutputPath(outputFile)) {
            GATE_ERROR("Invalid or potentially unsafe output file path.", outputFile);
            gate::utils::ErrorHandler::getInstance().printSummary();
            return 1;
        }

        // STEP 1: Secure file reading with validation
        // Read the NOTAL source file using secure file reader to prevent security vulnerabilities
        auto readResult = gate::utils::SecureFileReader::readFile(inputFile);
        if (!readResult.success) {
            GATE_ERROR(readResult.errorMessage, inputFile);
            gate::utils::ErrorHandler::getInstance().printSummary();
            return 1;
        }
        std::string sourceWithComments = readResult.content;

        // STEP 2: Source code validation
        // Validate the NOTAL source code for basic syntax and security issues
        auto validationResult = gate::utils::InputValidator::validateNotalSource(sourceWithComments);
        if (!validationResult.isValid) {
            GATE_ERROR(validationResult.errorMessage, inputFile);
            gate::utils::ErrorHandler::getInstance().printSummary();
            return 1;
        }
        // Display any warnings found during validation
        for (const auto& warning : validationResult.warnings) {
            GATE_WARNING(warning, inputFile);
        }

        // STEP 3: Pre-processing - Comment removal
        // Remove all comments from source code before tokenization
        std::string source = removeComments(sourceWithComments);

        // STEP 4: Lexical Analysis (Tokenization)
        // Convert the source code into a stream of tokens for parsing
        gate::transpiler::NotalLexer lexer(source);
        std::vector<gate::core::Token> tokens = lexer.getAllTokens();

        // STEP 5: Syntax Analysis (Parsing)
        // Parse the token stream into an Abstract Syntax Tree (AST)
        gate::transpiler::NotalParser parser(tokens);
        std::shared_ptr<gate::ast::ProgramStmt> program = parser.parse();

        // STEP 6: Code Generation and Output
        // Generate Pascal code from the AST if parsing was successful
        if (program && !gate::utils::ErrorHandler::getInstance().hasErrors()) {
            // Initialize the Pascal code generator
            gate::transpiler::PascalCodeGenerator generator;
            std::string pascalCode = generator.generate(program);

            // Handle output: either write to file or display on console
            if (!outputFile.empty()) {
                // Write generated Pascal code to the specified output file
                std::ofstream outFile(outputFile);
                if (outFile.is_open()) {
                    outFile << pascalCode;
                    outFile.close();
                    std::cout << "Transpilation successful. Pascal code written to '" << outputFile << "'" << std::endl;
                } else {
                    GATE_ERROR("Unable to open output file for writing.", outputFile);
                }
            } else {
                // No output file specified, display Pascal code on console
                std::cout << "\n" << pascalCode << std::endl;
            }
        } else {
            // Parsing failed, report error
            GATE_ERROR("Transpilation failed due to parsing errors.", inputFile);
        }

    } catch (const std::exception& e) {
        // Handle any standard C++ exceptions that weren't caught earlier
        GATE_FATAL("An unexpected error occurred: " + std::string(e.what()));
    } catch (...) {
        // Handle any other unknown exceptions to prevent application crash
        GATE_FATAL("An unknown and unexpected error occurred.");
    }

    // Print summary of all errors and warnings encountered during transpilation
    gate::utils::ErrorHandler::getInstance().printSummary();
    
    // Return appropriate exit code: 0 for success, 1 for failure
    return gate::utils::ErrorHandler::getInstance().hasErrors() ? 1 : 0;
}
