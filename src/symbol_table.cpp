#include "../include/symbol_table.hpp"
#include "../include/error_handler.hpp"

void SymbolTable::add_symbol(const std::string &name, const std::string &type)
{
    if (symbols.count(name))
    {
        ErrorHandler::report("Symbol already defined: " + name, 0, 0);
    }
    symbols[name] = type;
}

std::string SymbolTable::get_symbol(const std::string &name)
{
    if (!symbols.count(name))
    {
        ErrorHandler::report("Undefined symbol: " + name, 0, 0);
        return "";
    }
    return symbols.at(name);
}

bool SymbolTable::exists(const std::string &name)
{
    return symbols.count(name) > 0;
}