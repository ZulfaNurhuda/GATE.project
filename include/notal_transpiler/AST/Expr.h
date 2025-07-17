#ifndef NOTAL_TRANSPILER_AST_EXPR_H
#define NOTAL_TRANSPILER_AST_EXPR_H

#include "../Token.h"
#include <memory>
#include <any>
#include <vector>

namespace notal {
namespace ast {

// Forward declarations
struct Binary;
struct Unary;
struct Literal;
struct Variable;
struct Grouping;
struct Assign;
struct Call;

// Visitor interface for expressions
class ExprVisitor {
public:
    virtual std::any visit(std::shared_ptr<Binary> expr) = 0;
    virtual std::any visit(std::shared_ptr<Unary> expr) = 0;
    virtual std::any visit(std::shared_ptr<Literal> expr) = 0;
    virtual std::any visit(std::shared_ptr<Variable> expr) = 0;
    virtual std::any visit(std::shared_ptr<Grouping> expr) = 0;
    virtual std::any visit(std::shared_ptr<Assign> expr) = 0;
    virtual std::any visit(std::shared_ptr<Call> expr) = 0;
    virtual ~ExprVisitor() = default;
};

// Base class for all expression nodes
class Expr {
public:
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor& visitor) = 0;
};

// Binary expression node
struct Binary : Expr, public std::enable_shared_from_this<Binary> {
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;

    Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Unary expression node
struct Unary : Expr, public std::enable_shared_from_this<Unary> {
    Token op;
    std::shared_ptr<Expr> right;

    Unary(Token op, std::shared_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Literal expression node
struct Literal : Expr, public std::enable_shared_from_this<Literal> {
    std::any value;

    explicit Literal(std::any value) : value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Variable expression node
struct Variable : Expr, public std::enable_shared_from_this<Variable> {
    Token name;

    explicit Variable(Token name) : name(std::move(name)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Grouping expression node
struct Grouping : Expr, public std::enable_shared_from_this<Grouping> {
    std::shared_ptr<Expr> expression;

    explicit Grouping(std::shared_ptr<Expr> expression) : expression(std::move(expression)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Assignment expression node
struct Assign : Expr, public std::enable_shared_from_this<Assign> {
    Token name;
    std::shared_ptr<Expr> value;

    Assign(Token name, std::shared_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Call expression node
struct Call : Expr, public std::enable_shared_from_this<Call> {
    std::shared_ptr<Expr> callee;
    Token paren; // The closing parenthesis, for error reporting
    std::vector<std::shared_ptr<Expr>> arguments;

    Call(std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};


} // namespace ast
} // namespace notal

#endif // NOTAL_TRANSPILER_AST_EXPR_H
