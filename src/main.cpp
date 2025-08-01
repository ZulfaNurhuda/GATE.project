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
#include "gate/utils/SecureFileReader.h"
#include "gate/utils/InputValidator.h"
#include "gate/utils/ErrorHandler.h"

// Pre-transpilation step to remove comments
std::string removeComments(const std::string& source) {
    // Using [\s\S] to match any character including newlines, as a portable alternative to dotall flag.
    // Replace comment with a space to ensure tokens are separated.
    return std::regex_replace(source, std::regex("\\{[\\s\\S]*?\\}"), " ");
}

int main(int argc, char* argv[]) {
    gate::utils::ErrorHandler::getInstance().clear();

    try {
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

        std::string inputFile = result["input"].as<std::string>();
        std::string outputFile = result["output"].as<std::string>();

        if (!outputFile.empty() && !gate::utils::InputValidator::isValidOutputPath(outputFile)) {
            GATE_ERROR("Invalid or potentially unsafe output file path.", outputFile);
            gate::utils::ErrorHandler::getInstance().printSummary();
            return 1;
        }

        // 1. Read file content from inputFile
        auto readResult = gate::utils::SecureFileReader::readFile(inputFile);
        if (!readResult.success) {
            GATE_ERROR(readResult.errorMessage, inputFile);
            gate::utils::ErrorHandler::getInstance().printSummary();
            return 1;
        }
        std::string sourceWithComments = readResult.content;

        // 2. Validate source code
        auto validationResult = gate::utils::InputValidator::validateNotalSource(sourceWithComments);
        if (!validationResult.isValid) {
            GATE_ERROR(validationResult.errorMessage, inputFile);
            gate::utils::ErrorHandler::getInstance().printSummary();
            return 1;
        }
        for (const auto& warning : validationResult.warnings) {
            GATE_WARNING(warning, inputFile);
        }

        // 3. Pre-transpilation: Remove comments
        std::string source = removeComments(sourceWithComments);

        // 4. Transpile
        gate::transpiler::NotalLexer lexer(source);
        std::vector<gate::core::Token> tokens = lexer.getAllTokens();

        gate::transpiler::NotalParser parser(tokens);
        std::shared_ptr<gate::ast::ProgramStmt> program = parser.parse();

        if (program && !gate::utils::ErrorHandler::getInstance().hasErrors()) {
            gate::transpiler::PascalCodeGenerator generator;
            std::string pascalCode = generator.generate(program);

            if (!outputFile.empty()) {
                std::ofstream outFile(outputFile);
                if (outFile.is_open()) {
                    outFile << pascalCode;
                    outFile.close();
                    std::cout << "Transpilation successful. Pascal code written to '" << outputFile << "'" << std::endl;
                } else {
                    GATE_ERROR("Unable to open output file for writing.", outputFile);
                }
            } else {
                std::cout << "\n" << pascalCode << std::endl;
            }
        } else {
            GATE_ERROR("Transpilation failed due to parsing errors.", inputFile);
        }

    } catch (const std::exception& e) {
        GATE_FATAL("An unexpected error occurred: " + std::string(e.what()));
    } catch (...) {
        GATE_FATAL("An unknown and unexpected error occurred.");
    }

    gate::utils::ErrorHandler::getInstance().printSummary();
    return gate::utils::ErrorHandler::getInstance().hasErrors() ? 1 : 0;
}
