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

std::any ASTPrinter::visit(std::shared_ptr<ast::StaticArrayDeclStmt> stmt) {
    (void)stmt;
    return "(static_array_decl ...)";
}

std::any ASTPrinter::visit(std::shared_ptr<ast::DynamicArrayDeclStmt> stmt) {
    (void)stmt;
    return "(dynamic_array_decl ...)";
}

std::any ASTPrinter::visit(std::shared_ptr<ast::AllocateStmt> stmt) {
    (void)stmt;
    return "(allocate ...)";
}

std::any ASTPrinter::visit(std::shared_ptr<ast::DeallocateStmt> stmt) {
    (void)stmt;
    return "(deallocate ...)";
}

std::any ASTPrinter::visit(std::shared_ptr<ast::ConstDeclStmt> stmt) {
    return parenthesize("CONST_DECL " + stmt->name.lexeme + " : " + stmt->type.lexeme, {stmt->initializer});
}

std::any ASTPrinter::visit(std::shared_ptr<ast::InputStmt> stmt) {
    return parenthesize("input", {stmt->variable});
}

std::any ASTPrinter::visit(std::shared_ptr<ast::ExpressionStmt> stmt) {
    return parenthesize("expr-stmt", {stmt->expression});
}

std::any ASTPrinter::visit(std::shared_ptr<ast::OutputStmt> stmt) {
    return parenthesize("output", stmt->expressions);
}

// --- Expression Visitors ---

std::any ASTPrinter::visit(std::shared_ptr<ast::Assign> expr) {
    return parenthesize("<-", {expr->target, expr->value});
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

std::any ASTPrinter::visit(std::shared_ptr<ast::ArrayAccess> expr) {
    (void)expr;
    return "(array_access ...)";
}

// --- Unimplemented Visitors (for now) ---

std::any ASTPrinter::visit(std::shared_ptr<ast::IfStmt> stmt) {
    (void)stmt;
    return std::string("(if ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<ast::WhileStmt> stmt) {
    (void)stmt;
    return std::string("(while ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<ast::Call> expr) {
    (void)expr;
    return std::string("(call ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<ast::FieldAccess> expr) {
    (void)expr;
    return std::string("(field access ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<ast::FieldAssign> expr) {
    (void)expr;
    return std::string("(field assign ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<ast::RepeatUntilStmt> stmt) {
	(void)stmt;
	return std::string("(repeat ... until ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<ast::DependOnStmt> stmt) {
	(void)stmt;
	return std::string("(depend on ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<ast::TraversalStmt> stmt) {
    (void)stmt;
    return std::string("(traversal ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<ast::IterateStopStmt> stmt) {
    (void)stmt;
    return std::string("(iterate ... stop ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<ast::RepeatNTimesStmt> stmt) {
    (void)stmt;
    return std::string("(repeat N times ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<ast::RecordTypeDeclStmt> stmt) {
    std::stringstream ss;
    ss << "(RECORD_TYPE_DECL " << stmt->typeName.lexeme;
    for (const auto& field : stmt->fields) {
        ss << " (" << field.name.lexeme << " : " << field.type.lexeme << ")";
    }
    ss << ")";
    return ss.str();
}

std::any ASTPrinter::visit(std::shared_ptr<ast::EnumTypeDeclStmt> stmt) {
    std::stringstream ss;
    ss << "(ENUM_TYPE_DECL " << stmt->typeName.lexeme;
    for (const auto& value : stmt->values) {
        ss << " " << value.lexeme;
    }
    ss << ")";
    return ss.str();
}

std::any ASTPrinter::visit(std::shared_ptr<ast::ConstrainedVarDeclStmt> stmt) {
    return parenthesize("CONSTRAINED_VAR_DECL " + stmt->name.lexeme + " : " + stmt->type.lexeme, {stmt->constraint});
}

std::any ASTPrinter::visit(std::shared_ptr<ast::StopStmt> stmt) {
    (void)stmt;
    return "(stop)";
}

std::any ASTPrinter::visit(std::shared_ptr<ast::SkipStmt> stmt) {
    (void)stmt;
    return "(skip)";
}

std::any ASTPrinter::visit(std::shared_ptr<ast::ProcedureStmt> stmt) {
    (void)stmt;
    return "(procedure ...)";
}

std::any ASTPrinter::visit(std::shared_ptr<ast::FunctionStmt> stmt) {
    (void)stmt;
    return "(function ...)";
}

std::any ASTPrinter::visit(std::shared_ptr<ast::ReturnStmt> stmt) {
    (void)stmt;
    return "(return ...)";
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
