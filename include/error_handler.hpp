#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include <string>

class ErrorHandler
{
public:
    static void report(const std::string &message, int line, int col);
};

#endif