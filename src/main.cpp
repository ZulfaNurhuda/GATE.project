#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <cxxopts.hpp>

#include "notal_transpiler/Lexer.h"
#include "notal_transpiler/Parser.h"
#include "notal_transpiler/CodeGenerator.h"
#include "notal_transpiler/Token.h"
#include "notal_transpiler/AST/Stmt.h"
#include "notal_transpiler/AST/Expr.h"

// Pre-transpilation step to remove comments
std::string removeComments(const std::string& source) {
    // Using [\s\S] to match any character including newlines, as a portable alternative to dotall flag.
    // Replace comment with a space to ensure tokens are separated.
    return std::regex_replace(source, std::regex("\\{[\\s\\S]*?\\}"), " ");
}

int main(int argc, char* argv[]) {
    cxxopts::Options options("notal_transpiler", "A transpiler from NOTAL to Pascal.");

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
        std::cerr << "Error: Input file not specified." << std::endl;
        std::cerr << options.help() << std::endl;
        return 1;
    }

    std::string inputFile = result["input"].as<std::string>();
    std::string outputFile = result["output"].as<std::string>();

    std::cout << "NOTAL Transpiler" << std::endl;
    std::cout << "Input file: " << inputFile << std::endl;
    if (!outputFile.empty()) {
        std::cout << "Output file: " << outputFile << std::endl;
    } else {
        std::cout << "Output file: (stdout)" << std::endl;
    }

    // 1. Read file content from inputFile
    std::ifstream inFile(inputFile);
    if (!inFile.is_open()) {
        std::cerr << "Error: Could not open input file '" << inputFile << "'" << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string sourceWithComments = buffer.str();
    inFile.close();

    // Pre-transpilation: Remove comments
    std::string source = removeComments(sourceWithComments);

    try {
        // 2. Initialize Lexer
        notal::Lexer lexer(source);
        std::vector<notal::Token> tokens = lexer.allTokens();

        // 3. Initialize Parser
        notal::Parser parser(tokens);
        std::shared_ptr<notal::ast::ProgramStmt> program = parser.parse();

        if (program) {
            // 4. Generate Pascal code
            notal::CodeGenerator generator;
            std::string pascalCode = generator.generate(program);
            
            // 5. Write to outputFile or stdout
            if (!outputFile.empty()) {
                std::ofstream outFile(outputFile);
                if (outFile.is_open()) {
                    outFile << pascalCode;
                    outFile.close();
                    std::cout << "Transpilation successful. Pascal code written to '" << outputFile << "'" << std::endl;
                } else {
                    std::cerr << "Error: Unable to open output file '" << outputFile << "' for writing." << std::endl;
                    return 1;
                }
            } else {
                std::cout << "\n" << pascalCode << std::endl;
            }
        } else {
            std::cerr << "Error: Parsing failed. See above for details." << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during transpilation: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}