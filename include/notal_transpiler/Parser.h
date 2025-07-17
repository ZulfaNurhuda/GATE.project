#ifndef NOTAL_TRANSPILER_PARSER_H
#define NOTAL_TRANSPILER_PARSER_H

#include "Token.h"
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace notal {

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    std::shared_ptr<ast::ProgramStmt> parse();

private:
    std::vector<Token> tokens;
    size_t current = 0;

    // --- Grammar Rule Methods ---
    std::shared_ptr<ast::ProgramStmt> program();
    std::shared_ptr<ast::KamusStmt> kamus();
    std::shared_ptr<ast::AlgoritmaStmt> algoritma();
    std::shared_ptr<ast::Stmt> declaration();
    std::shared_ptr<ast::Stmt> varDeclaration();
    std::shared_ptr<ast::Stmt> statement();
    std::shared_ptr<ast::Stmt> ifStatement();
    std::shared_ptr<ast::Stmt> ifStatementBody(int parentIndentLevel); // Helper for recursion
    std::shared_ptr<ast::Stmt> whileStatement();
    std::shared_ptr<ast::Stmt> repeatUntilStatement();
    std::shared_ptr<ast::Stmt> expressionStatement();
    std::shared_ptr<ast::Stmt> outputStatement();
    std::vector<std::shared_ptr<ast::Stmt>> block();
    std::vector<std::shared_ptr<ast::Stmt>> parseBlockByIndentation(int expectedIndentLevel);


    std::shared_ptr<ast::Expr> expression();
    std::shared_ptr<ast::Expr> assignment();
    std::shared_ptr<ast::Expr> logic_or();
    std::shared_ptr<ast::Expr> logic_and();
    std::shared_ptr<ast::Expr> equality();
    std::shared_ptr<ast::Expr> comparison();
    std::shared_ptr<ast::Expr> term();
    std::shared_ptr<ast::Expr> factor();
    std::shared_ptr<ast::Expr> unary();
    std::shared_ptr<ast::Expr> primary();


    // --- Helper Methods ---
    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    void synchronize();

    // --- Error Handling ---
    class ParseError : public std::runtime_error {
    public:
        ParseError(const Token& token, const std::string& message)
            : std::runtime_error(message), token(token) {}
        Token token;
    };
    ParseError error(const Token& token, const std::string& message);

};

} // namespace notal

#endif // NOTAL_TRANSPILER_PARSER_H
