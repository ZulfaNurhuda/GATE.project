#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <unordered_map>
#include <string>

class SymbolTable
{
private:
    std::unordered_map<std::string, std::string> symbols;

public:
    void add_symbol(const std::string &name, const std::string &type);
    std::string get_symbol(const std::string &name);
    bool exists(const std::string &name);
};

#endif