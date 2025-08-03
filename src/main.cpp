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
#include "gate/diagnostics/DiagnosticEngine.h"
#include "gate/core/Token.h"
#include "gate/ast/Statement.h"
#include "gate/ast/Expression.h"
#include "gate/utils/SecureFileReader.h"
#include "gate/utils/InputValidator.h"

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
        std::cerr << "Error: Input file not specified. Use -i or --input." << std::endl;
        return 1;
    }

    std::string inputFile = result["input"].as<std::string>();
    std::string outputFile = result["output"].as<std::string>();

    if (!outputFile.empty() && !gate::utils::InputValidator::isValidOutputPath(outputFile)) {
        std::cerr << "Error: Invalid or potentially unsafe output file path: " << outputFile << std::endl;
        return 1;
    }

    auto readResult = gate::utils::SecureFileReader::readFile(inputFile);
    if (!readResult.success) {
        std::cerr << "Error: " << readResult.errorMessage << " (" << inputFile << ")" << std::endl;
        return 1;
    }
    std::string sourceWithComments = readResult.content;

    // The DiagnosticEngine needs the source code to provide context for errors.
    std::string source = removeComments(sourceWithComments);
    gate::diagnostics::DiagnosticEngine diagnosticEngine(source, inputFile);

    // Perform preliminary source validation (optional, but good practice)
    auto validationResult = gate::utils::InputValidator::validateNotalSource(sourceWithComments);
    if (!validationResult.isValid) {
        // Report a fatal error through the new system
        gate::diagnostics::SourceLocation loc(inputFile, 0, 0);
        auto diag = gate::diagnostics::Diagnostic::Builder(validationResult.errorMessage, loc)
                        .withLevel(gate::diagnostics::DiagnosticLevel::FATAL)
                        .build();
        diagnosticEngine.report(diag);
    }
    for (const auto& warning : validationResult.warnings) {
        gate::diagnostics::SourceLocation loc(inputFile, 0, 0);
        auto diag = gate::diagnostics::Diagnostic::Builder(warning, loc)
                        .withLevel(gate::diagnostics::DiagnosticLevel::WARNING)
                        .build();
        diagnosticEngine.report(diag);
    }

    // Lexical Analysis
    gate::transpiler::NotalLexer lexer(source, inputFile);
    std::vector<gate::core::Token> tokens = lexer.getAllTokens();

    // Syntax Analysis
    gate::transpiler::NotalParser parser(tokens, diagnosticEngine);
    std::shared_ptr<gate::ast::ProgramStmt> program = parser.parse();

    // Code Generation
    if (program && !diagnosticEngine.hasErrors()) {
        gate::transpiler::PascalCodeGenerator generator;
        std::string pascalCode = generator.generate(program);

        if (!outputFile.empty()) {
            std::ofstream outFile(outputFile);
            if (outFile.is_open()) {
                outFile << pascalCode;
                outFile.close();
                std::cout << "Transpilation successful. Pascal code written to '" << outputFile << "'" << std::endl;
            } else {
                std::cerr << "Error: Unable to open output file for writing: " << outputFile << std::endl;
            }
        } else {
            std::cout << "\n" << pascalCode << std::endl;
        }
    }

    // Always print the diagnostic report
    if (diagnosticEngine.hasErrors() || diagnosticEngine.hasWarnings()) {
        std::cerr << diagnosticEngine.generateReport();
    }
    
    return diagnosticEngine.hasErrors() ? 1 : 0;
}
