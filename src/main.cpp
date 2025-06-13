#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include "../include/generator.hpp"
#include "../include/ui.hpp"
#include "../include/logger.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc < 4 || std::string(argv[2]) != "-o")
    {
        UI::printUsage();
        return 1;
    }

    std::string input_file = argv[1];
    std::string output_file = argv[3];

    std::ifstream in(input_file);
    if (!in.is_open())
    {
        UI::printMessage("Error: Cannot open input file " + input_file);
        return 1;
    }

    std::string source_code((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    UI::printHeader();
    Logger logger;
    logger.log("Starting transpilation process...");

    Lexer lexer;
    std::vector<Token> tokens = lexer.tokenize(source_code);

    Parser parser;
    ASTNode ast = parser.parse(tokens);

    CodeGenerator generator;
    std::string c_code = generator.generate(ast);

    std::ofstream out(output_file);
    if (!out.is_open())
    {
        UI::printMessage("Error: Cannot open output file " + output_file);
        return 1;
    }
    out << c_code;
    out.close();

    logger.log("Transpilation completed successfully.");
    UI::printMessage("Output written to " + output_file);

    return 0;
}