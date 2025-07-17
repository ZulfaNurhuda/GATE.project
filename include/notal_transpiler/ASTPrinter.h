#ifndef NOTAL_TRANSPILER_AST_PRINTER_H
#define NOTAL_TRANSPILER_AST_PRINTER_H

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <string>
#include <any>

namespace notal {

class ASTPrinter : public ast::ExprVisitor, public ast::StmtVisitor {
public:
    std::string print(std::shared_ptr<ast::ProgramStmt> program);

    // Statement visitors
    std::any visit(std::shared_ptr<ast::ExpressionStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::BlockStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::ProgramStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::KamusStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::AlgoritmaStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::VarDeclStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::IfStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::WhileStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::OutputStmt> stmt) override;

    // Expression visitors
    std::any visit(std::shared_ptr<ast::Binary> expr) override;
    std::any visit(std::shared_ptr<ast::Unary> expr) override;
    std::any visit(std::shared_ptr<ast::Literal> expr) override;
    std::any visit(std::shared_ptr<ast::Variable> expr) override;
    std::any visit(std::shared_ptr<ast::Grouping> expr) override;
    std::any visit(std::shared_ptr<ast::Assign> expr) override;
    std::any visit(std::shared_ptr<ast::Call> expr) override;

private:
    std::string parenthesize(const std::string& name, const std::vector<std::shared_ptr<ast::Expr>>& exprs);
    std::string parenthesize_stmt(const std::string& name, const std::vector<std::shared_ptr<ast::Stmt>>& stmts);
    int indentLevel = 0;
    std::string indent();
};

} // namespace notal

#endif // NOTAL_TRANSPILER_AST_PRINTER_H
