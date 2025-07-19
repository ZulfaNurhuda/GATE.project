#ifndef NOTAL_TRANSPILER_AST_STMT_H
#define NOTAL_TRANSPILER_AST_STMT_H

#include "Expr.h"
#include <memory>
#include <vector>
#include <any>

namespace notal {
namespace ast {

// Forward declarations
struct ExpressionStmt;
struct BlockStmt;
struct ProgramStmt;
struct KamusStmt;
struct AlgoritmaStmt;
struct VarDeclStmt;
struct IfStmt;
struct WhileStmt;
struct RepeatUntilStmt;
struct OutputStmt;
struct DependOnStmt;

// Visitor interface for statements
class StmtVisitor {
public:
    virtual std::any visit(std::shared_ptr<ExpressionStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<BlockStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<ProgramStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<KamusStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<AlgoritmaStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<VarDeclStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<IfStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<WhileStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<RepeatUntilStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<OutputStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<DependOnStmt> stmt) = 0;
    virtual ~StmtVisitor() = default;
};

// Base class for all statement nodes
class Stmt {
public:
    virtual ~Stmt() = default;
    virtual std::any accept(StmtVisitor& visitor) = 0;
};

// Expression statement node
struct ExpressionStmt : Stmt, public std::enable_shared_from_this<ExpressionStmt> {
    std::shared_ptr<Expr> expression;

    explicit ExpressionStmt(std::shared_ptr<Expr> expression) : expression(std::move(expression)) {}

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Block statement node
struct BlockStmt : Stmt, public std::enable_shared_from_this<BlockStmt> {
    std::vector<std::shared_ptr<Stmt>> statements;

    explicit BlockStmt(std::vector<std::shared_ptr<Stmt>> statements) : statements(std::move(statements)) {}

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Program statement node
struct ProgramStmt : Stmt, public std::enable_shared_from_this<ProgramStmt> {
    Token name;
    std::shared_ptr<KamusStmt> kamus;
    std::shared_ptr<AlgoritmaStmt> algoritma;

    ProgramStmt(Token name, std::shared_ptr<KamusStmt> kamus, std::shared_ptr<AlgoritmaStmt> algoritma)
        : name(std::move(name)), kamus(std::move(kamus)), algoritma(std::move(algoritma)) {}

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Kamus (dictionary) block statement node
struct KamusStmt : Stmt, public std::enable_shared_from_this<KamusStmt> {
    std::vector<std::shared_ptr<Stmt>> declarations;

    explicit KamusStmt(std::vector<std::shared_ptr<Stmt>> declarations) : declarations(std::move(declarations)) {}

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Algoritma (algorithm) block statement node
struct AlgoritmaStmt : Stmt, public std::enable_shared_from_this<AlgoritmaStmt> {
    std::shared_ptr<BlockStmt> body;

    explicit AlgoritmaStmt(std::shared_ptr<BlockStmt> body) : body(std::move(body)) {}

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Variable declaration statement node
struct VarDeclStmt : Stmt, public std::enable_shared_from_this<VarDeclStmt> {
    Token name;
    Token type; // This will be the type keyword token, e.g., INTEGER
    // std::shared_ptr<Expr> initializer; // NOTAL doesn't have initializers in KAMUS

    VarDeclStmt(Token name, Token type)
        : name(std::move(name)), type(std::move(type)) {}

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// If statement node
struct IfStmt : Stmt, public std::enable_shared_from_this<IfStmt> {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch; // Can be null

    IfStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// While loop statement node
struct WhileStmt : Stmt, public std::enable_shared_from_this<WhileStmt> {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<BlockStmt> body;

    WhileStmt(std::shared_ptr<Expr> condition, std::shared_ptr<BlockStmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Output statement node
struct OutputStmt : Stmt, public std::enable_shared_from_this<OutputStmt> {
    std::vector<std::shared_ptr<Expr>> expressions;

    explicit OutputStmt(std::vector<std::shared_ptr<Expr>> expressions)
        : expressions(std::move(expressions)) {}

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Repeat..Until loop statement node
struct RepeatUntilStmt : Stmt, public std::enable_shared_from_this<RepeatUntilStmt> {
    std::shared_ptr<BlockStmt> body;
    std::shared_ptr<Expr> condition;

    RepeatUntilStmt(std::shared_ptr<BlockStmt> body, std::shared_ptr<Expr> condition)
        : body(std::move(body)), condition(std::move(condition)) {}

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

// Depend On statement node
struct DependOnStmt : Stmt, public std::enable_shared_from_this<DependOnStmt> {
    struct Case {
        std::vector<std::shared_ptr<Expr>> conditions;
        std::shared_ptr<Stmt> body;

        Case(std::vector<std::shared_ptr<Expr>> conditions, std::shared_ptr<Stmt> body)
            : conditions(std::move(conditions)), body(std::move(body)) {}
    };

    std::shared_ptr<Expr> expression;
    std::vector<Case> cases;
    std::shared_ptr<Stmt> otherwiseBranch; // Can be null

    DependOnStmt(std::shared_ptr<Expr> expression, std::vector<Case> cases, std::shared_ptr<Stmt> otherwiseBranch)
        : expression(std::move(expression)), cases(std::move(cases)), otherwiseBranch(std::move(otherwiseBranch)) {}

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

} // namespace ast
} // namespace notal

#endif // NOTAL_TRANSPILER_AST_STMT_H
