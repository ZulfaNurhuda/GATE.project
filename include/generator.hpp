#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <string>
#include "parser.hpp"
#include "symbol_table.hpp"

class CodeGenerator
{
private:
    SymbolTable symbol_table;
    std::string generate_node(const ASTNode &node, int indent = 0);
    std::string indent(int level);

public:
    std::string generate(const ASTNode &ast);
};

#endif