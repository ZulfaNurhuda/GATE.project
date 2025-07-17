#include "notal_transpiler/ASTPrinter.h"
#include <sstream>
#include <any>

namespace notal {

std::string ASTPrinter::print(std::shared_ptr<ast::ProgramStmt> program) {
    if (!program) return "(null program)";
    return std::any_cast<std::string>(program->accept(*this));
}

// --- Statement Visitors ---

std::any ASTPrinter::visit(std::shared_ptr<ast::ProgramStmt> stmt) {
    std::stringstream ss;
    ss << "(PROGRAM " << stmt->name.lexeme << "\n";
    indentLevel++;
    ss << indent() << std::any_cast<std::string>(stmt->kamus->accept(*this)) << "\n";
    ss << indent() << std::any_cast<std::string>(stmt->algoritma->accept(*this)) << "\n";
    indentLevel--;
    ss << indent() << ")";
    return ss.str();
}

std::any ASTPrinter::visit(std::shared_ptr<ast::KamusStmt> stmt) {
    return parenthesize_stmt("KAMUS", stmt->declarations);
}

std::any ASTPrinter::visit(std::shared_ptr<ast::AlgoritmaStmt> stmt) {
    return "(ALGORITMA " + std::any_cast<std::string>(stmt->body->accept(*this)) + ")";
}

std::any ASTPrinter::visit(std::shared_ptr<ast::BlockStmt> stmt) {
    return parenthesize_stmt("block", stmt->statements);
}

std::any ASTPrinter::visit(std::shared_ptr<ast::VarDeclStmt> stmt) {
    return "(VAR_DECL " + stmt->name.lexeme + " : " + stmt->type.lexeme + ")";
}

std::any ASTPrinter::visit(std::shared_ptr<ast::ExpressionStmt> stmt) {
    return parenthesize("expr-stmt", {stmt->expression});
}

std::any ASTPrinter::visit(std::shared_ptr<ast::OutputStmt> stmt) {
    return parenthesize("output", stmt->expressions);
}

// --- Expression Visitors ---

std::any ASTPrinter::visit(std::shared_ptr<ast::Assign> expr) {
    return parenthesize("<- " + expr->name.lexeme, {expr->value});
}

std::any ASTPrinter::visit(std::shared_ptr<ast::Binary> expr) {
    return parenthesize(expr->op.lexeme, {expr->left, expr->right});
}

std::any ASTPrinter::visit(std::shared_ptr<ast::Unary> expr) {
    return parenthesize(expr->op.lexeme, {expr->right});
}

std::any ASTPrinter::visit(std::shared_ptr<ast::Grouping> expr) {
    return parenthesize("group", {expr->expression});
}

std::any ASTPrinter::visit(std::shared_ptr<ast::Variable> expr) {
    return expr->name.lexeme;
}

std::any ASTPrinter::visit(std::shared_ptr<ast::Literal> expr) {
    if (expr->value.type() == typeid(int)) {
        return std::to_string(std::any_cast<int>(expr->value));
    }
    if (expr->value.type() == typeid(double)) {
        return std::to_string(std::any_cast<double>(expr->value));
    }
    if (expr->value.type() == typeid(bool)) {
        return std::any_cast<bool>(expr->value) ? "true" : "false";
    }
    if (expr->value.type() == typeid(std::string)) {
        return "'" + std::any_cast<std::string>(expr->value) + "'";
    }
    return std::string("null");
}

// --- Unimplemented Visitors (for now) ---

std::any ASTPrinter::visit(std::shared_ptr<ast::IfStmt> stmt) {
    return std::string("(if ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<ast::WhileStmt> stmt) {
    return std::string("(while ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<ast::Call> expr) {
    return std::string("(call ...)");
}


// --- Helper Methods ---

std::string ASTPrinter::parenthesize(const std::string& name, const std::vector<std::shared_ptr<ast::Expr>>& exprs) {
    std::stringstream ss;
    ss << "(" << name;
    for (const auto& expr : exprs) {
        ss << " " << std::any_cast<std::string>(expr->accept(*this));
    }
    ss << ")";
    return ss.str();
}

std::string ASTPrinter::parenthesize_stmt(const std::string& name, const std::vector<std::shared_ptr<ast::Stmt>>& stmts) {
    std::stringstream ss;
    ss << "(" << name;
    indentLevel++;
    for (const auto& stmt : stmts) {
        ss << "\n" << indent() << std::any_cast<std::string>(stmt->accept(*this));
    }
    indentLevel--;
    ss << "\n" << indent() << ")";
    return ss.str();
}

std::string ASTPrinter::indent() {
    return std::string(indentLevel * 2, ' ');
}

} // namespace notal

