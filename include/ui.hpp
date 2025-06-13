#ifndef UI_HPP
#define UI_HPP

#include <string>

class UI
{
public:
    static void enableColors();
    static void printHeader();
    static void printUsage();
    static void printSteps();
    static void printMessage(const std::string &msg);
};

#endif