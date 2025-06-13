#include "../include/ui.hpp"
#include <iostream>

void UI::enableColors()
{
    // No-op in basic implementation
}

void UI::printHeader()
{
    std::cout << "Ganesha Algorithmic Transpiler Engine (GATE)\n";
}

void UI::printUsage()
{
    std::cout << "Usage: ./gate <input.notal> -o <output.c>\n";
}

void UI::printSteps()
{
    std::cout << "1. Reading input file\n2. Tokenizing\n3. Parsing\n4. Generating code\n5. Writing output\n";
}

void UI::printMessage(const std::string &msg)
{
    std::cout << msg << std::endl;
}