#include "../include/error_handler.hpp"
#include <iostream>

void ErrorHandler::report(const std::string &message, int line, int col)
{
    std::cerr << "Error at line " << line << ", col " << col << ": " << message << std::endl;
    exit(1);
}