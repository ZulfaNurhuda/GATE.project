#ifndef NOTAL_TRANSPILER_PARSER_H
#define NOTAL_TRANSPILER_PARSER_H

#include "Token.h"
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <map>

namespace notal {

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    std::shared_ptr<ast::ProgramStmt> parse();

private:
    std::vector<Token> tokens;
    size_t current = 0;
    std::map<std::string, std::shared_ptr<ast::Stmt>> subprogramDeclarations;

    // --- Grammar Rule Methods ---
    std::shared_ptr<ast::ProgramStmt> program();
    std::shared_ptr<ast::KamusStmt> kamus();
    std::shared_ptr<ast::AlgoritmaStmt> algoritma();
    std::shared_ptr<ast::Stmt> declaration();
    std::shared_ptr<ast::Stmt> varDeclaration();
    std::shared_ptr<ast::Stmt> arrayDeclaration(Token name);
    std::shared_ptr<ast::Stmt> constantDeclaration(); // Added
    std::shared_ptr<ast::Stmt> typeDeclaration();     // Added for type declarations
    std::shared_ptr<ast::Stmt> statement();
    std::shared_ptr<ast::Stmt> allocateStatement();
    std::shared_ptr<ast::Stmt> deallocateStatement();
    std::shared_ptr<ast::Stmt> inputStatement();      // Added
    std::shared_ptr<ast::Stmt> ifStatement();
    std::shared_ptr<ast::Stmt> ifStatementBody(int parentIndentLevel); // Helper for recursion
    std::shared_ptr<ast::Stmt> whileStatement();
    std::shared_ptr<ast::Stmt> repeatUntilStatement();
    std::shared_ptr<ast::Stmt> traversalStatement();
    std::shared_ptr<ast::Stmt> iterateStopStatement();
    std::shared_ptr<ast::Stmt> repeatNTimesStatement();
    std::shared_ptr<ast::Stmt> dependOnStatement();
    std::shared_ptr<ast::Stmt> expressionStatement();
    std::shared_ptr<ast::Stmt> outputStatement();
    std::shared_ptr<ast::Stmt> stopStatement();
    std::shared_ptr<ast::Stmt> skipStatement();
    std::vector<std::shared_ptr<ast::Stmt>> block();
    std::vector<std::shared_ptr<ast::Stmt>> parseBlockByIndentation(int expectedIndentLevel);
    std::shared_ptr<ast::Stmt> subprogramDeclaration();
    std::shared_ptr<ast::Stmt> procedureDeclaration();
    std::shared_ptr<ast::Stmt> functionDeclaration();
    void subprogramImplementation(const Token& subprogramKeyword, const Token& subprogramName);
    std::vector<ast::Parameter> parameterList();
    std::shared_ptr<ast::Stmt> returnStatement();


    std::shared_ptr<ast::Expr> expression();
    std::shared_ptr<ast::Expr> assignment();
    std::shared_ptr<ast::Expr> logic_or();
    std::shared_ptr<ast::Expr> logic_and();
    std::shared_ptr<ast::Expr> equality();
    std::shared_ptr<ast::Expr> comparison();
    std::shared_ptr<ast::Expr> term();
    std::shared_ptr<ast::Expr> factor();
        std::shared_ptr<ast::Expr> power();
    std::shared_ptr<ast::Expr> unary();
    std::shared_ptr<ast::Expr> call();
    std::shared_ptr<ast::Expr> finishCall(std::shared_ptr<ast::Expr> callee);
    std::shared_ptr<ast::Expr> arrayAccess(std::shared_ptr<ast::Expr> callee);
    std::shared_ptr<ast::Expr> primary();


    // --- Helper Methods ---
    bool isAtEnd();
    Token peek();
    Token peekNext();
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
