#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <cxxopts.hpp>

#include "gate/transpiler/NotalLexer.h"
#include "gate/transpiler/NotalParser.h"
#include "gate/transpiler/PascalCodeGenerator.h"
#include "gate/core/Token.h"
#include "gate/ast/Statement.h"
#include "gate/ast/Expression.h"

// Pre-transpilation step to remove comments
std::string removeComments(const std::string& source) {
    // Using [\s\S] to match any character including newlines, as a portable alternative to dotall flag.
    // Replace comment with a space to ensure tokens are separated.
    return std::regex_replace(source, std::regex("\\{[\\s\\S]*?\\}"), " ");
}

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
        std::cerr << "Error: Input file not specified." << std::endl;
        std::cerr << options.help() << std::endl;
        return 1;
    }

    std::string inputFile = result["input"].as<std::string>();
    std::string outputFile = result["output"].as<std::string>();

    std::cout << "GATE Transpiler" << std::endl;
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
        // Use fully qualified names for clarity
        gate::transpiler::NotalLexer lexer(source);
        std::vector<gate::core::Token> tokens = lexer.getAllTokens();

        gate::transpiler::NotalParser parser(tokens);
        std::shared_ptr<gate::ast::ProgramStmt> program = parser.parse();

        if (program) {
            gate::transpiler::PascalCodeGenerator generator;
            std::string pascalCode = generator.generate(program);

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
