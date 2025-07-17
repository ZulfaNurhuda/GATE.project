#include "notal_transpiler/Parser.h"
#include <iostream>
#include <memory>
#include <vector>

namespace notal {

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

// --- Public Parse Method ---

std::shared_ptr<ast::ProgramStmt> Parser::parse() {
    try {
        return program();
    } catch (const ParseError& error) {
        std::cerr << "[Line " << error.token.line << "] Error";
        if (error.token.type == TokenType::END_OF_FILE) {
            std::cerr << " at end";
        } else {
            std::cerr << " at '" << error.token.lexeme << "'";
        }
        std::cerr << ": " << error.what() << std::endl;
        return nullptr;
    }
}

// --- Grammar Rule Methods ---

// program -> "PROGRAM" IDENTIFIER kamus algoritma
std::shared_ptr<ast::ProgramStmt> Parser::program() {
    consume(TokenType::PROGRAM, "Expect 'PROGRAM'.");
    Token name = consume(TokenType::IDENTIFIER, "Expect program name.");
    
    std::shared_ptr<ast::KamusStmt> kamusBlock = kamus();
    std::shared_ptr<ast::AlgoritmaStmt> algoritmaBlock = algoritma();

    if (!isAtEnd()) {
        // Handle trailing procedures/functions later
    }

    return std::make_shared<ast::ProgramStmt>(name, kamusBlock, algoritmaBlock);
}

// kamus -> "KAMUS" declaration*
std::shared_ptr<ast::KamusStmt> Parser::kamus() {
    consume(TokenType::KAMUS, "Expect 'KAMUS'.");
    std::vector<std::shared_ptr<ast::Stmt>> declarations;
    while (!check(TokenType::ALGORITMA) && !isAtEnd()) {
        declarations.push_back(declaration());
    }
    return std::make_shared<ast::KamusStmt>(declarations);
}

// algoritma -> "ALGORITMA" block
std::shared_ptr<ast::AlgoritmaStmt> Parser::algoritma() {
    consume(TokenType::ALGORITMA, "Expect 'ALGORITMA'.");
    std::vector<std::shared_ptr<ast::Stmt>> statements = block();
    auto body = std::make_shared<ast::BlockStmt>(statements);
    return std::make_shared<ast::AlgoritmaStmt>(body);
}

// declaration -> varDeclaration
std::shared_ptr<ast::Stmt> Parser::declaration() {
    return varDeclaration();
}

// varDeclaration -> IDENTIFIER ":" type
std::shared_ptr<ast::Stmt> Parser::varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    consume(TokenType::COLON, "Expect ':' after variable name.");
    
    Token type = advance();
    if (type.type != TokenType::INTEGER && type.type != TokenType::REAL &&
        type.type != TokenType::STRING && type.type != TokenType::BOOLEAN &&
        type.type != TokenType::CHARACTER) {
            throw error(type, "Expect a type name.");
        }

    return std::make_shared<ast::VarDeclStmt>(name, type);
}

// block -> (statement)*
std::vector<std::shared_ptr<notal::ast::Stmt>> Parser::block() {
    // This block is for the top-level ALGORITMA, which starts at column 0 (or whatever the initial indentation is)
    // We'll use the column of the first statement to determine the expected indentation for subsequent statements.
    // If the ALGORITMA block is empty, peek().column might be irrelevant, so handle that.
    int initialIndent = 0;
    if (!isAtEnd()) {
        initialIndent = peek().column;
    }
    return parseBlockByIndentation(initialIndent);
}

std::vector<std::shared_ptr<notal::ast::Stmt>> Parser::parseBlockByIndentation(int expectedIndentLevel) {
    std::vector<std::shared_ptr<notal::ast::Stmt>> statements;
    while (!isAtEnd() && peek().column >= expectedIndentLevel) {
        if (peek().column < expectedIndentLevel) {
            break;
        }
        statements.push_back(statement());
    }
    return statements;
}

// statement -> ifStatement | whileStatement | repeatUntilStatement | outputStatement | expressionStatement
std::shared_ptr<notal::ast::Stmt> Parser::statement() {
    if (peek().type == TokenType::IF) {
        return ifStatement();
    }
    if (peek().type == TokenType::WHILE) {
        return whileStatement();
    }
    if (peek().type == TokenType::REPEAT) {
        return repeatUntilStatement();
    }
    if (peek().type == TokenType::OUTPUT) {
        return outputStatement();
    }
    return expressionStatement();
}

std::shared_ptr<notal::ast::Stmt> Parser::ifStatement() {
    Token ifToken = consume(TokenType::IF, "Expect 'if'."); // Capture the IF token
    return ifStatementBody(ifToken.column); // Pass the column of 'if'
}

std::shared_ptr<notal::ast::Stmt> Parser::ifStatementBody(int parentIndentLevel) {
    std::shared_ptr<notal::ast::Expr> condition = expression();
    consume(TokenType::THEN, "Expect 'then' after condition.");
    
    int thenBranchIndent = 0;
    if (!isAtEnd()) {
        thenBranchIndent = peek().column;
    }

    std::shared_ptr<notal::ast::BlockStmt> thenBranch;
    if (thenBranchIndent > parentIndentLevel) {
        thenBranch = std::make_shared<ast::BlockStmt>(parseBlockByIndentation(thenBranchIndent));
    } else {
        thenBranch = std::make_shared<ast::BlockStmt>(std::vector<std::shared_ptr<ast::Stmt>>{});
    }
    
    std::shared_ptr<notal::ast::Stmt> elseBranch = nullptr;

    if (match({TokenType::ELIF})) {
        if (previous().column != parentIndentLevel) {
            throw error(previous(), "'elif' must be at the same indentation level as 'if'.");
        }
        elseBranch = ifStatementBody(parentIndentLevel);
    } else if (match({TokenType::ELSE})) {
        if (previous().column != parentIndentLevel) {
            throw error(previous(), "'else' must be at the same indentation level as 'if'.");
        }
        int elseBranchIndent = 0;
        if (!isAtEnd()) {
            elseBranchIndent = peek().column;
        }

        if (elseBranchIndent > parentIndentLevel) {
            elseBranch = std::make_shared<ast::BlockStmt>(parseBlockByIndentation(elseBranchIndent));
        } else {
            elseBranch = std::make_shared<ast::BlockStmt>(std::vector<std::shared_ptr<ast::Stmt>>{});
        }
    }

    return std::make_shared<notal::ast::IfStmt>(condition, thenBranch, elseBranch);
}

std::shared_ptr<notal::ast::Stmt> Parser::whileStatement() {
    Token whileToken = consume(TokenType::WHILE, "Expect 'while'."); // Capture the WHILE token
    std::shared_ptr<notal::ast::Expr> condition = expression();
    consume(TokenType::DO, "Expect 'do' after while condition.");
    
    int whileBodyIndent = 0;
    if (!isAtEnd()) {
        whileBodyIndent = peek().column;
    }
    
    // The body must be more indented than the while statement itself.
    if (whileBodyIndent <= whileToken.column) {
        // This could be an empty loop body.
        auto bodyBlock = std::make_shared<ast::BlockStmt>(std::vector<std::shared_ptr<ast::Stmt>>{});
        return std::make_shared<ast::WhileStmt>(condition, bodyBlock);
    }

    std::shared_ptr<notal::ast::BlockStmt> bodyBlock = std::make_shared<ast::BlockStmt>(parseBlockByIndentation(whileBodyIndent));

    return std::make_shared<notal::ast::WhileStmt>(condition, bodyBlock);
}

std::shared_ptr<ast::Stmt> Parser::repeatUntilStatement() {
    Token repeatToken = consume(TokenType::REPEAT, "Expect 'repeat'.");

    int bodyIndent = 0;
    if (!isAtEnd()) {
        bodyIndent = peek().column;
    }

    if (bodyIndent <= repeatToken.column) {
        throw error(peek(), "The body of a repeat-until loop must be indented.");
    }

    auto body = std::make_shared<ast::BlockStmt>(parseBlockByIndentation(bodyIndent));

    consume(TokenType::UNTIL, "Expect 'until' after repeat block.");

    std::shared_ptr<ast::Expr> condition = expression();

    return std::make_shared<ast::RepeatUntilStmt>(body, condition);
}

std::shared_ptr<ast::Stmt> Parser::outputStatement() {
    consume(TokenType::OUTPUT, "Expect 'output'.");
    consume(TokenType::LPAREN, "Expect '(' after 'output'.");
    
    std::vector<std::shared_ptr<ast::Expr>> expressions;
    if (!check(TokenType::RPAREN)) {
        do {
            expressions.push_back(expression());
        } while (match({TokenType::COMMA}));
    }

    consume(TokenType::RPAREN, "Expect ')' after output arguments.");
    return std::make_shared<ast::OutputStmt>(expressions);
}

std::shared_ptr<ast::Stmt> Parser::expressionStatement() {
    std::shared_ptr<ast::Expr> expr = expression();
    return std::make_shared<ast::ExpressionStmt>(expr);
}

// --- Expression Parsing ---

std::shared_ptr<ast::Expr> Parser::expression() {
    return assignment();
}

std::shared_ptr<ast::Expr> Parser::assignment() {
    std::shared_ptr<ast::Expr> expr = equality();

    if (match({TokenType::ASSIGN})) {
        Token equals = previous();
        std::shared_ptr<ast::Expr> value = assignment();

        if (auto var = std::dynamic_pointer_cast<ast::Variable>(expr)) {
            Token name = var->name;
            return std::make_shared<ast::Assign>(name, value);
        }

        throw error(equals, "Invalid assignment target.");
    }

    return expr;
}

std::shared_ptr<ast::Expr> Parser::equality() {
    std::shared_ptr<ast::Expr> expr = comparison();
    while (match({TokenType::NOT_EQUAL, TokenType::EQUAL})) {
        Token op = previous();
        std::shared_ptr<ast::Expr> right = comparison();
        expr = std::make_shared<ast::Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<ast::Expr> Parser::comparison() {
    std::shared_ptr<ast::Expr> expr = term();
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        std::shared_ptr<ast::Expr> right = term();
        expr = std::make_shared<ast::Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<ast::Expr> Parser::term() {
    std::shared_ptr<ast::Expr> expr = factor();
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        std::shared_ptr<ast::Expr> right = factor();
        expr = std::make_shared<ast::Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<ast::Expr> Parser::factor() {
    std::shared_ptr<ast::Expr> expr = unary();
    while (match({TokenType::DIVIDE, TokenType::MULTIPLY, TokenType::MOD, TokenType::DIV})) {
        Token op = previous();
        std::shared_ptr<ast::Expr> right = unary();
        expr = std::make_shared<ast::Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<ast::Expr> Parser::unary() {
    if (match({TokenType::NOT, TokenType::MINUS})) {
        Token op = previous();
        std::shared_ptr<ast::Expr> right = unary();
        return std::make_shared<ast::Unary>(op, right);
    }
    return primary();
}

std::shared_ptr<ast::Expr> Parser::primary() {
    if (match({TokenType::BOOLEAN_LITERAL})) return std::make_shared<ast::Literal>(previous().lexeme == "true");
    if (match({TokenType::INTEGER_LITERAL})) return std::make_shared<ast::Literal>(std::stoi(previous().lexeme));
    if (match({TokenType::REAL_LITERAL})) return std::make_shared<ast::Literal>(std::stod(previous().lexeme));
    if (match({TokenType::STRING_LITERAL})) return std::make_shared<ast::Literal>(previous().lexeme);
    if (match({TokenType::IDENTIFIER})) return std::make_shared<ast::Variable>(previous());
    if (match({TokenType::LPAREN})) {
        std::shared_ptr<ast::Expr> expr = expression();
        consume(TokenType::RPAREN, "Expect ')' after expression.");
        return std::make_shared<ast::Grouping>(expr);
    }
    throw error(peek(), "Expect expression.");
}

// --- Helper Methods ---

bool Parser::isAtEnd() { return peek().type == TokenType::END_OF_FILE; }
Token Parser::peek() { return tokens[current]; }
Token Parser::previous() { return tokens[current - 1]; }
Token Parser::advance() { if (!isAtEnd()) current++; return previous(); }
bool Parser::check(TokenType type) { if (isAtEnd()) return false; return peek().type == type; }

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

Parser::ParseError Parser::error(const Token& token, const std::string& message) {
    return ParseError(token, message);
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        switch (peek().type) {
            case TokenType::PROGRAM:
            case TokenType::KAMUS:
            case TokenType::ALGORITMA:
            case TokenType::PROCEDURE:
            case TokenType::FUNCTION:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::REPEAT:
            case TokenType::OUTPUT:
                return;
            default:
                break;
        }
        advance();
    }
}

} // namespace notal
