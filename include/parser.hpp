#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include "lexer.hpp"

struct ASTNode
{
    std::string type;
    std::vector<ASTNode> children;
    std::string value;
    int line;
    int col;
};

class Parser
{
private:
    std::vector<Token> tokens;
    size_t pos;

    Token current_token();
    void advance();
    ASTNode parse_program();
    ASTNode parse_kamus();
    ASTNode parse_algoritma();
    ASTNode parse_statement();
    ASTNode parse_expression();

public:
    ASTNode parse(const std::vector<Token> &tokens);
};

#endif