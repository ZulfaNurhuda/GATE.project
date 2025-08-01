#ifndef GATE_TRANSPILER_NOTAL_PARSER_H
#define GATE_TRANSPILER_NOTAL_PARSER_H

#include "gate/core/Token.h"
#include "gate/ast/Expression.h"
#include "gate/ast/Statement.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <map>

namespace gate::transpiler {

class NotalParser {
public:
    explicit NotalParser(const std::vector<core::Token>& tokens);

    std::shared_ptr<ast::ProgramStmt> parse();

private:
    std::vector<core::Token> tokens_;
    size_t current_ = 0;
    std::vector<std::shared_ptr<ast::Statement>> subprogramDeclarations_;

    // --- Grammar Rule Methods ---
    std::shared_ptr<ast::ProgramStmt> program();
    std::shared_ptr<ast::KamusStmt> kamus();
    std::shared_ptr<ast::AlgoritmaStmt> algoritma();
    std::shared_ptr<ast::Statement> declaration();
    std::shared_ptr<ast::Statement> varDeclaration();
    std::shared_ptr<ast::Statement> arrayDeclaration(core::Token name);
    std::shared_ptr<ast::Statement> constantDeclaration();
    std::shared_ptr<ast::Statement> typeDeclaration();
    std::shared_ptr<ast::Statement> statement();
    std::shared_ptr<ast::Statement> allocateStatement();
    std::shared_ptr<ast::Statement> deallocateStatement();
    std::shared_ptr<ast::Statement> inputStatement();
    std::shared_ptr<ast::Statement> ifStatement();
    std::shared_ptr<ast::Statement> ifStatementBody(int parentIndentLevel);
    std::shared_ptr<ast::Statement> whileStatement();
    std::shared_ptr<ast::Statement> repeatUntilStatement();
    std::shared_ptr<ast::Statement> traversalStatement();
    std::shared_ptr<ast::Statement> iterateStopStatement();
    std::shared_ptr<ast::Statement> repeatNTimesStatement();
    std::shared_ptr<ast::Statement> dependOnStatement();
    std::shared_ptr<ast::Statement> expressionStatement();
    std::shared_ptr<ast::Statement> outputStatement();
    std::shared_ptr<ast::Statement> stopStatement();
    std::shared_ptr<ast::Statement> skipStatement();
    std::vector<std::shared_ptr<ast::Statement>> block();
    std::vector<std::shared_ptr<ast::Statement>> parseBlockByIndentation(int expectedIndentLevel);
    std::shared_ptr<ast::Statement> subprogramDeclaration();
    std::shared_ptr<ast::Statement> procedureDeclaration();
    std::shared_ptr<ast::Statement> functionDeclaration();
    void subprogramImplementation(const core::Token& subprogramKeyword, const core::Token& subprogramName);
    std::vector<ast::Parameter> parameterList();
    std::shared_ptr<ast::Statement> returnStatement();


    std::shared_ptr<ast::Expression> expression();
    std::shared_ptr<ast::Expression> assignment();
    std::shared_ptr<ast::Expression> logic_or();
    std::shared_ptr<ast::Expression> logic_and();
    std::shared_ptr<ast::Expression> equality();
    std::shared_ptr<ast::Expression> comparison();
    std::shared_ptr<ast::Expression> term();
    std::shared_ptr<ast::Expression> factor();
    std::shared_ptr<ast::Expression> power();
    std::shared_ptr<ast::Expression> unary();
    std::shared_ptr<ast::Expression> call();
    std::shared_ptr<ast::Expression> finishCall(std::shared_ptr<ast::Expression> callee);
    std::shared_ptr<ast::Expression> arrayAccess(std::shared_ptr<ast::Expression> callee);
    std::shared_ptr<ast::Expression> primary();


    // --- Helper Methods ---
    bool isAtEnd();
    core::Token peek();
    core::Token peekNext();
    core::Token previous();
    core::Token advance();
    bool check(core::TokenType type);
    bool match(const std::vector<core::TokenType>& types);
    core::Token consume(core::TokenType type, const std::string& message);
    void synchronize();

    // --- Error Handling ---
    class ParseError : public std::runtime_error {
    public:
        ParseError(const core::Token& token, const std::string& message)
            : std::runtime_error(message), token(token) {}
        core::Token token;
    };
    ParseError error(const core::Token& token, const std::string& message);

};

} // namespace gate::transpiler

#endif // GATE_TRANSPILER_NOTAL_PARSER_H
