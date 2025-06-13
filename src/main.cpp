#include "../include/lexer.hpp"
#include "../include/parser.hpp" // Needs to be new parser.hpp for ProgramNode
#include "../include/generator.hpp" // Needs to be new generator.hpp for ASTVisitor
#include "../include/ui.hpp"
#include "../include/logger.hpp"
#include <fstream>
#include <iostream>
#include <string>    // For std::string
#include <algorithm> // For std::transform
#include <vector>    // For std::vector if reconstructing argv

int main(int argc, char *argv[])
{
    Logger logger; // Instantiate logger early
    LogLevel log_level_to_set = LogLevel::INFO; // Default log level

    // --- Argument Parsing for --log-level ---
    // This loop searches for --log-level and its value.
    // It assumes --log-level and its value are two separate arguments.
    std::vector<char*> remaining_argv;
    remaining_argv.push_back(argv[0]); // Program name

    for (int i = 1; i < argc; ++i) {
        std::string arg_str = argv[i];
        if (arg_str == "--log-level") {
            if (i + 1 < argc) { // Check if there's a next argument for the level value
                std::string level_str = argv[i+1];
                std::transform(level_str.begin(), level_str.end(), level_str.begin(), ::toupper);
                if (level_str == "DEBUG") log_level_to_set = LogLevel::DEBUG;
                else if (level_str == "INFO") log_level_to_set = LogLevel::INFO;
                else if (level_str == "WARNING") log_level_to_set = LogLevel::WARNING;
                else if (level_str == "ERROR") log_level_to_set = LogLevel::ERROR;
                else if (level_str == "NONE") log_level_to_set = LogLevel::NONE;
                else {
                    // Using std::cerr as UI::printMessage might not be configured yet
                    // or might use the logger itself.
                    std::cerr << "Warning: Unrecognized log level '" << argv[i+1] << "'. Defaulting to INFO." << std::endl;
                }
                i++; // Skip the next argument (level value) as it has been processed
            } else {
                 std::cerr << "Warning: '--log-level' option requires a value. Defaulting to INFO." << std::endl;
            }
        } else {
            remaining_argv.push_back(argv[i]);
        }
    }

    logger.set_level(log_level_to_set); // Initialize logger with determined level

    // --- Argument Parsing for input/output files (using remaining_argv) ---
    // The original logic expected argv[1] as input, argv[2] as "-o", argv[3] as output.
    // After filtering out --log-level, remaining_argv should hold these.
    // remaining_argv[0] is program name.
    // remaining_argv[1] should be input_file.
    // remaining_argv[2] should be "-o".
    // remaining_argv[3] should be output_file.

    if (remaining_argv.size() < 4 || std::string(remaining_argv[2]) != "-o") {
        UI::printUsage();
        return 1;
    }

    std::string input_file = remaining_argv[1];
    std::string output_file = remaining_argv[3];

    std::ifstream in(input_file);
    if (!in.is_open()) {
        // UI::printMessage might use the logger. If logger is NONE, this won't show.
        // Direct std::cerr might be better for critical file errors if logger can be NONE.
        logger.error("Cannot open input file " + input_file);
        if (logger.get_level() == LogLevel::NONE) { // Ensure critical errors are shown
            std::cerr << "Error: Cannot open input file " << input_file << std::endl;
        }
        return 1;
    }

    std::string source_code((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    UI::printHeader(); // This might use logger or std::cout.
    logger.info("Starting transpilation process...");
    logger.debug("Input file: " + input_file);
    logger.debug("Output file: " + output_file);
    logger.debug("Log level set to: " + logger.level_to_string(logger.get_level()));


    Lexer lexer;
    std::vector<Token> tokens = lexer.tokenize(source_code);
    logger.debug("Lexer finished. Number of tokens: " + std::to_string(tokens.size()));

    Parser parser;
    ParseResult parse_result = parser.parse(tokens); // Use ParseResult
    logger.debug("Parser finished.");

    if (!parse_result.ast_root) { // Check ast_root from ParseResult
        logger.error("Parsing failed, AST root is null.");
        // Optionally, log symbol table status if available even if AST is null
        if (parse_result.symbol_table) {
            logger.debug("Symbol table was created despite AST generation failure.");
        }
        return 1;
    }
    if (!parse_result.symbol_table) { // Should not happen if ast_root is valid
        logger.error("Parsing succeeded (AST not null) but symbol table is null. This is unexpected.");
        return 1;
    }
    logger.debug("Symbol table created. Current scope level (global): " + std::to_string(parse_result.symbol_table->getCurrentScopeLevel()));


    std::ofstream out(output_file);
    if (!out.is_open()) {
        logger.error("Cannot open output file " + output_file);
         if (logger.get_level() == LogLevel::NONE) {
            std::cerr << "Error: Cannot open output file " << output_file << std::endl;
        }
        return 1;
    }

    CodeGenerator generator;
    // Pass the populated symbol table to the generator
    generator.generate_c_code(parse_result.ast_root.get(), *(parse_result.symbol_table), out);
    logger.debug("Generator finished writing to output file.");

    out.close();

    logger.info("Transpilation completed successfully.");
    // UI::printMessage("Output written to " + output_file); // This is fine if it uses std::cout

    return 0;
}