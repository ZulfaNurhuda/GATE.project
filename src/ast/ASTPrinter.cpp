/**
 * @file ASTPrinter.cpp
 * @brief Implementation of the AST printer for the GATE transpiler
 * 
 * This file contains the implementation of the ASTPrinter class, which provides
 * functionality to convert Abstract Syntax Tree (AST) nodes into human-readable
 * string representations. This is essential for debugging, development, and
 * understanding the structure of parsed NOTAL programs.
 * 
 * The ASTPrinter implements the Visitor pattern to traverse and print different
 * types of AST nodes, including statements, expressions, and declarations.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */

#include "gate/ast/ASTPrinter.h"
#include <sstream>
#include <any>

namespace gate::ast {

/**
 * @brief Prints the entire program AST to a string
 * 
 * This method serves as the main entry point for AST printing. It takes
 * a program statement (the root of the AST) and converts it to a
 * human-readable string representation.
 * 
 * @param program Shared pointer to the root ProgramStmt node
 * @return std::string String representation of the entire AST
 * 
 * @note Uses the visitor pattern to traverse the AST
 * @note The returned string includes proper indentation and formatting
 */
std::string ASTPrinter::print(std::shared_ptr<ProgramStmt> program) {
    if (!program) return "(null program)";
    try {
        return std::any_cast<std::string>(program->accept(*this));
    } catch (const std::bad_any_cast& e) {
        return "Error printing AST.";
    }
}

// --- Statement Visitors ---

/**
 * @brief Visits a program statement node
 * 
 * Generates a string representation of the entire program, including
 * the program name, KAMUS section, and ALGORITMA section with proper
 * indentation and formatting.
 * 
 * @param stmt Shared pointer to the ProgramStmt node
 * @return std::any String representation wrapped in std::any
 */
std::any ASTPrinter::visit(std::shared_ptr<ProgramStmt> stmt) {
    std::stringstream ss;
    ss << "(PROGRAM " << stmt->name.lexeme << "\n";
    indentLevel_++;
    ss << indent() << std::any_cast<std::string>(stmt->kamus->accept(*this)) << "\n";
    ss << indent() << std::any_cast<std::string>(stmt->algoritma->accept(*this)) << "\n";
    indentLevel_--;
    ss << indent() << ")";
    return ss.str();
}

/**
 * @brief Visits a KAMUS (dictionary/declarations) statement node
 * 
 * @param stmt Shared pointer to the KamusStmt node
 * @return std::any String representation of all declarations
 */
std::any ASTPrinter::visit(std::shared_ptr<KamusStmt> stmt) {
    return parenthesizeStatement("KAMUS", stmt->declarations);
}

/**
 * @brief Visits an ALGORITMA (algorithm) statement node
 * 
 * @param stmt Shared pointer to the AlgoritmaStmt node
 * @return std::any String representation of the algorithm body
 */
std::any ASTPrinter::visit(std::shared_ptr<AlgoritmaStmt> stmt) {
    return "(ALGORITMA " + std::any_cast<std::string>(stmt->body->accept(*this)) + ")";
}

/**
 * @brief Visits a block statement node
 * 
 * @param stmt Shared pointer to the BlockStmt node
 * @return std::any String representation of all statements in the block
 */
std::any ASTPrinter::visit(std::shared_ptr<BlockStmt> stmt) {
    return parenthesizeStatement("block", stmt->statements);
}

/**
 * @brief Visits a variable declaration statement node
 * 
 * @param stmt Shared pointer to the VarDeclStmt node
 * @return std::any String representation of the variable declaration
 */
std::any ASTPrinter::visit(std::shared_ptr<VarDeclStmt> stmt) {
    return "(VAR_DECL " + stmt->name.lexeme + " : " + stmt->type.lexeme + ")";
}

std::any ASTPrinter::visit(std::shared_ptr<StaticArrayDeclStmt> stmt) {
    (void)stmt;
    return "(static_array_decl ...)";
}

std::any ASTPrinter::visit(std::shared_ptr<DynamicArrayDeclStmt> stmt) {
    (void)stmt;
    return "(dynamic_array_decl ...)";
}

std::any ASTPrinter::visit(std::shared_ptr<AllocateStmt> stmt) {
    (void)stmt;
    return "(allocate ...)";
}

std::any ASTPrinter::visit(std::shared_ptr<DeallocateStmt> stmt) {
    (void)stmt;
    return "(deallocate ...)";
}

std::any ASTPrinter::visit(std::shared_ptr<ConstDeclStmt> stmt) {
    return parenthesize("CONST_DECL " + stmt->name.lexeme + " : " + stmt->type.lexeme, {stmt->initializer});
}

std::any ASTPrinter::visit(std::shared_ptr<InputStmt> stmt) {
    return parenthesize("input", {stmt->variable});
}

std::any ASTPrinter::visit(std::shared_ptr<ExpressionStmt> stmt) {
    return parenthesize("expr-stmt", {stmt->expression});
}

std::any ASTPrinter::visit(std::shared_ptr<OutputStmt> stmt) {
    return parenthesize("output", stmt->expressions);
}

// --- Expression Visitors ---

std::any ASTPrinter::visit(std::shared_ptr<Assign> expr) {
    return parenthesize("<-", {expr->target, expr->value});
}

std::any ASTPrinter::visit(std::shared_ptr<Binary> expr) {
    return parenthesize(expr->op.lexeme, {expr->left, expr->right});
}

std::any ASTPrinter::visit(std::shared_ptr<Unary> expr) {
    return parenthesize(expr->op.lexeme, {expr->right});
}

std::any ASTPrinter::visit(std::shared_ptr<Grouping> expr) {
    return parenthesize("group", {expr->expression});
}

std::any ASTPrinter::visit(std::shared_ptr<Variable> expr) {
    return expr->name.lexeme;
}

std::any ASTPrinter::visit(std::shared_ptr<Literal> expr) {
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

std::any ASTPrinter::visit(std::shared_ptr<ArrayAccess> expr) {
    (void)expr;
    return "(array_access ...)";
}

// --- Unimplemented Visitors (for now) ---

std::any ASTPrinter::visit(std::shared_ptr<IfStmt> stmt) {
    (void)stmt;
    return std::string("(if ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<WhileStmt> stmt) {
    (void)stmt;
    return std::string("(while ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<Call> expr) {
    (void)expr;
    return std::string("(call ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<FieldAccess> expr) {
    (void)expr;
    return std::string("(field access ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<FieldAssign> expr) {
    (void)expr;
    return std::string("(field assign ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<RepeatUntilStmt> stmt) {
	(void)stmt;
	return std::string("(repeat ... until ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<DependOnStmt> stmt) {
	(void)stmt;
	return std::string("(depend on ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<TraversalStmt> stmt) {
    (void)stmt;
    return std::string("(traversal ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<IterateStopStmt> stmt) {
    (void)stmt;
    return std::string("(iterate ... stop ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<RepeatNTimesStmt> stmt) {
    (void)stmt;
    return std::string("(repeat N times ...)");
}

std::any ASTPrinter::visit(std::shared_ptr<RecordTypeDeclStmt> stmt) {
    std::stringstream ss;
    ss << "(RECORD_TYPE_DECL " << stmt->typeName.lexeme;
    for (const auto& field : stmt->fields) {
        ss << " (" << field.name.lexeme << " : " << field.type.lexeme << ")";
    }
    ss << ")";
    return ss.str();
}

std::any ASTPrinter::visit(std::shared_ptr<EnumTypeDeclStmt> stmt) {
    std::stringstream ss;
    ss << "(ENUM_TYPE_DECL " << stmt->typeName.lexeme;
    for (const auto& value : stmt->values) {
        ss << " " << value.lexeme;
    }
    ss << ")";
    return ss.str();
}

std::any ASTPrinter::visit(std::shared_ptr<ConstrainedVarDeclStmt> stmt) {
    return parenthesize("CONSTRAINED_VAR_DECL " + stmt->name.lexeme + " : " + stmt->type.lexeme, {stmt->constraint});
}

std::any ASTPrinter::visit(std::shared_ptr<StopStmt> stmt) {
    (void)stmt;
    return "(stop)";
}

std::any ASTPrinter::visit(std::shared_ptr<SkipStmt> stmt) {
    (void)stmt;
    return "(skip)";
}

std::any ASTPrinter::visit(std::shared_ptr<ProcedureStmt> stmt) {
    (void)stmt;
    return "(procedure ...)";
}

std::any ASTPrinter::visit(std::shared_ptr<FunctionStmt> stmt) {
    (void)stmt;
    return "(function ...)";
}

std::any ASTPrinter::visit(std::shared_ptr<ReturnStmt> stmt) {
    (void)stmt;
    return "(return ...)";
}


// --- Helper Methods ---

/**
 * @brief Helper method to format expressions with parentheses
 * 
 * This utility method creates a parenthesized string representation
 * of a named construct with a list of expressions. It's used for
 * formatting various expression types in a consistent manner.
 * 
 * @param name The name/type of the construct
 * @param exprs Vector of expression pointers to include
 * @return std::string Formatted string with parentheses
 * 
 * @note All expressions are visited and their string representations concatenated
 */
std::string ASTPrinter::parenthesize(const std::string& name, const std::vector<std::shared_ptr<Expression>>& exprs) {
    std::stringstream ss;
    ss << "(" << name;
    for (const auto& expr : exprs) {
        ss << " " << std::any_cast<std::string>(expr->accept(*this));
    }
    ss << ")";
    return ss.str();
}

/**
 * @brief Helper method to format statements with parentheses and indentation
 * 
 * This utility method creates a parenthesized string representation
 * of a named construct with a list of statements. It handles proper
 * indentation for nested structures.
 * 
 * @param name The name/type of the construct
 * @param stmts Vector of statement pointers to include
 * @return std::string Formatted string with parentheses and indentation
 * 
 * @note Increases indentation level for nested statements
 * @note Each statement appears on a new line with proper indentation
 */
std::string ASTPrinter::parenthesizeStatement(const std::string& name, const std::vector<std::shared_ptr<Statement>>& stmts) {
    std::stringstream ss;
    ss << "(" << name;
    indentLevel_++;
    for (const auto& stmt : stmts) {
        ss << "\n" << indent() << std::any_cast<std::string>(stmt->accept(*this));
    }
    indentLevel_--;
    ss << "\n" << indent() << ")";
    return ss.str();
}

/**
 * @brief Generates indentation string based on current nesting level
 * 
 * This helper method creates a string of spaces for proper indentation
 * of nested AST structures. Each indentation level corresponds to 2 spaces.
 * 
 * @return std::string String containing appropriate number of spaces
 * 
 * @note Uses 2 spaces per indentation level
 * @note Indentation level is managed by the visitor methods
 */
std::string ASTPrinter::indent() {
    return std::string(indentLevel_ * 2, ' ');
}

} // namespace gate::ast
