#include "../include/generator.hpp"
#include <sstream>

std::string CodeGenerator::indent(int level)
{
    return std::string(level * 4, ' ');
}

std::string CodeGenerator::generate_node(const ASTNode &node, int indent_level)
{
    std::stringstream ss;
    if (node.type == "program")
    {
        ss << "#include <stdio.h>\n\nint main() {\n";
        for (const auto &child : node.children)
        {
            ss << generate_node(child, 1);
        }
        ss << "    return 0;\n}\n";
    }
    else if (node.type == "kamus")
    {
        for (const auto &decl : node.children)
        {
            std::string var_name = decl.value;
            std::string var_type = decl.children[0].value == "integer" ? "int" : "char*";
            symbol_table.add_symbol(var_name, var_type);
            ss << indent(indent_level) << var_type << " " << var_name << ";\n";
        }
    }
    else if (node.type == "algoritma")
    {
        for (const auto &stmt : node.children)
        {
            ss << generate_node(stmt, indent_level);
        }
    }
    else if (node.type == "assignment")
    {
        ss << indent(indent_level) << node.value << " = " << generate_node(node.children[0], 0) << ";\n";
    }
    else if (node.type == "output")
    {
        ss << indent(indent_level) << "printf(\"%d\\n\", " << generate_node(node.children[0], 0) << ");\n";
    }
    else if (node.type == "expression")
    {
        ss << node.value;
    }
    return ss.str();
}

std::string CodeGenerator::generate(const ASTNode &ast)
{
    return generate_node(ast);
}