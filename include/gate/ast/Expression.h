#ifndef GATE_AST_EXPRESSION_H
#define GATE_AST_EXPRESSION_H

#include "gate/core/Token.h"
#include <any>
#include <memory>
#include <vector>

namespace gate::ast {

// Forward declarations
struct Binary;
struct Unary;
struct Literal;
struct Variable;
struct Grouping;
struct Assign;
struct Call;
struct FieldAccess;
struct FieldAssign;
struct ArrayAccess;

// Visitor interface for expressions
class ExpressionVisitor {
public:
    virtual std::any visit(std::shared_ptr<Binary> expr) = 0;
    virtual std::any visit(std::shared_ptr<Unary> expr) = 0;
    virtual std::any visit(std::shared_ptr<Literal> expr) = 0;
    virtual std::any visit(std::shared_ptr<Variable> expr) = 0;
    virtual std::any visit(std::shared_ptr<Grouping> expr) = 0;
    virtual std::any visit(std::shared_ptr<Assign> expr) = 0;
    virtual std::any visit(std::shared_ptr<Call> expr) = 0;
    virtual std::any visit(std::shared_ptr<FieldAccess> expr) = 0;
    virtual std::any visit(std::shared_ptr<FieldAssign> expr) = 0;
    virtual std::any visit(std::shared_ptr<ArrayAccess> expr) = 0;
    virtual ~ExpressionVisitor() = default;
};

// Base class for all expression nodes
class Expression {
public:
    virtual ~Expression() = default;
    virtual std::any accept(ExpressionVisitor& visitor) = 0;
};

// Binary expression node
struct Binary : Expression, public std::enable_shared_from_this<Binary> {
    std::shared_ptr<Expression> left;
    core::Token op;
    std::shared_ptr<Expression> right;

    Binary(std::shared_ptr<Expression> left, core::Token op, std::shared_ptr<Expression> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Unary expression node
struct Unary : Expression, public std::enable_shared_from_this<Unary> {
    core::Token op;
    std::shared_ptr<Expression> right;

    Unary(core::Token op, std::shared_ptr<Expression> right)
        : op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Literal expression node
struct Literal : Expression, public std::enable_shared_from_this<Literal> {
    std::any value;

    explicit Literal(std::any value) : value(std::move(value)) {}

    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Variable expression node
struct Variable : Expression, public std::enable_shared_from_this<Variable> {
    core::Token name;

    explicit Variable(core::Token name) : name(std::move(name)) {}

    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Grouping expression node
struct Grouping : Expression, public std::enable_shared_from_this<Grouping> {
    std::shared_ptr<Expression> expression;

    explicit Grouping(std::shared_ptr<Expression> expression) : expression(std::move(expression)) {}

    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Assignment expression node
struct Assign : Expression, public std::enable_shared_from_this<Assign> {
    std::shared_ptr<Expression> target; // The left-hand side of the assignment
    std::shared_ptr<Expression> value;

    Assign(std::shared_ptr<Expression> target, std::shared_ptr<Expression> value)
        : target(std::move(target)), value(std::move(value)) {}

    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Call expression node
struct Call : Expression, public std::enable_shared_from_this<Call> {
    std::shared_ptr<Expression> callee;
    core::Token paren; // The closing parenthesis, for error reporting
    std::vector<std::shared_ptr<Expression>> arguments;

    Call(std::shared_ptr<Expression> callee, core::Token paren, std::vector<std::shared_ptr<Expression>> arguments)
        : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}

    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Field access expression node (e.g., student.name)
struct FieldAccess : Expression, public std::enable_shared_from_this<FieldAccess> {
    std::shared_ptr<Expression> object;
    core::Token name; // The field name

    FieldAccess(std::shared_ptr<Expression> object, core::Token name)
        : object(std::move(object)), name(std::move(name)) {}

    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Field assignment expression node (e.g., student.name <- value)
struct FieldAssign : Expression, public std::enable_shared_from_this<FieldAssign> {
    std::shared_ptr<FieldAccess> target;
    std::shared_ptr<Expression> value;

    FieldAssign(std::shared_ptr<FieldAccess> target, std::shared_ptr<Expression> value)
        : target(std::move(target)), value(std::move(value)) {}

    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Array access expression node (e.g., myArray[index])
struct ArrayAccess : Expression, public std::enable_shared_from_this<ArrayAccess> {
    std::shared_ptr<Expression> callee; // The expression that evaluates to the array
    core::Token bracket; // The closing ']' token, for error reporting
    std::vector<std::shared_ptr<Expression>> indices;

    ArrayAccess(std::shared_ptr<Expression> callee, core::Token bracket, std::vector<std::shared_ptr<Expression>> indices)
        : callee(std::move(callee)), bracket(std::move(bracket)), indices(std::move(indices)) {}

    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};


} // namespace gate::ast

#endif // GATE_AST_EXPRESSION_H
