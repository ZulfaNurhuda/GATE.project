#include "../include/parser.hpp"
#include "../include/error_handler.hpp"

Token Parser::current_token()
{
    if (pos < tokens.size())
        return tokens[pos];
    return {TokenType::SEMICOLON, "", -1, -1};
}

void Parser::advance()
{
    pos++;
}

ASTNode Parser::parse_program()
{
    ASTNode root{"program", {}, "", 1, 1};
    if (current_token().type == TokenType::PROGRAM)
        advance();
    if (current_token().type == TokenType::IDENTIFIER)
        advance();
    root.children.push_back(parse_kamus());
    root.children.push_back(parse_algoritma());
    return root;
}

ASTNode Parser::parse_kamus()
{
    ASTNode kamus{"kamus", {}, "", current_token().line, current_token().col};
    if (current_token().type != TokenType::KAMUS)
    {
        ErrorHandler::report("Expected KAMUS", current_token().line, current_token().col);
    }
    advance();
    while (current_token().type == TokenType::IDENTIFIER)
    {
        ASTNode decl{"declaration", {}, "", current_token().line, current_token().col};
        decl.value = current_token().value;
        advance();
        if (current_token().type == TokenType::COLON)
            advance();
        if (current_token().type == TokenType::TYPE)
        {
            decl.children.push_back({"type", {}, current_token().value, current_token().line, current_token().col});
            advance();
        }
        if (current_token().type == TokenType::SEMICOLON)
            advance();
        kamus.children.push_back(decl);
    }
    return kamus;
}

ASTNode Parser::parse_algoritma()
{
    ASTNode algo{"algoritma", {}, "", current_token().line, current_token().col};
    if (current_token().type != TokenType::ALGORITMA)
    {
        ErrorHandler::report("Expected ALGORITMA", current_token().line, current_token().col);
    }
    advance();
    while (pos < tokens.size() && current_token().type != TokenType::DOT)
    {
        algo.children.push_back(parse_statement());
    }
    if (current_token().type == TokenType::DOT)
        advance();
    return algo;
}

ASTNode Parser::parse_statement()
{
    Token tok = current_token();
    if (tok.type == TokenType::IDENTIFIER)
    {
        ASTNode assign{"assignment", {}, tok.value, tok.line, tok.col};
        advance();
        if (current_token().type == TokenType::LEFT_ARROW)
        {
            advance();
            assign.children.push_back(parse_expression());
        }
        if (current_token().type == TokenType::SEMICOLON)
            advance();
        return assign;
    }
    else if (tok.type == TokenType::OUTPUT)
    {
        ASTNode output{"output", {}, "", tok.line, tok.col};
        advance();
        if (current_token().type == TokenType::LPAREN)
            advance();
        output.children.push_back(parse_expression());
        if (current_token().type == TokenType::RPAREN)
            advance();
        if (current_token().type == TokenType::SEMICOLON)
            advance();
        return output;
    }
    ErrorHandler::report("Invalid statement", tok.line, tok.col);
    return {"error", {}, "", tok.line, tok.col};
}

ASTNode Parser::parse_expression()
{
    Token tok = current_token();
    if (tok.type == TokenType::IDENTIFIER || tok.type == TokenType::INTEGER_LITERAL)
    {
        ASTNode expr{"expression", {}, tok.value, tok.line, tok.col};
        advance();
        return expr;
    }
    ErrorHandler::report("Invalid expression", tok.line, tok.col);
    return {"error", {}, "", tok.line, tok.col};
}

ASTNode Parser::parse(const std::vector<Token> &input_tokens)
{
    tokens = input_tokens;
    pos = 0;
    return parse_program();
}