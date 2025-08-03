/**
 * @file ASTPrinter.h
 * @brief AST Printer for NOTAL Language
 * 
 * This file defines the ASTPrinter class that implements the Visitor pattern
 * to traverse and print Abstract Syntax Tree (AST) nodes for the NOTAL language.
 * It provides a string representation of the AST structure for debugging and
 * visualization purposes.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */

#ifndef GATE_AST_AST_PRINTER_H
#define GATE_AST_AST_PRINTER_H

#include "ast/Expression.h"
#include "ast/Statement.h"
#include <string>
#include <any>
#include <vector>

namespace gate::ast {

/**
 * @brief AST Printer class for NOTAL language
 * 
 * Implements both ExpressionVisitor and StatementVisitor interfaces to traverse
 * and generate string representations of AST nodes. Uses the Visitor pattern
 * to provide a clean separation between AST structure and printing logic.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
class ASTPrinter : public ExpressionVisitor, public StatementVisitor {
public:
    /**
     * @brief Print the entire program AST
     * @param program The root program statement
     * @return String representation of the AST
     */
    std::string print(std::shared_ptr<ProgramStmt> program);

    // Statement visitors
    /** @brief Visit expression statement */
    std::any visit(std::shared_ptr<ExpressionStmt> stmt) override;
    /** @brief Visit block statement */
    std::any visit(std::shared_ptr<BlockStmt> stmt) override;
    /** @brief Visit program statement */
    std::any visit(std::shared_ptr<ProgramStmt> stmt) override;
    /** @brief Visit kamus (declarations) statement */
    std::any visit(std::shared_ptr<KamusStmt> stmt) override;
    /** @brief Visit algoritma (algorithm) statement */
    std::any visit(std::shared_ptr<AlgoritmaStmt> stmt) override;
    /** @brief Visit variable declaration statement */
    std::any visit(std::shared_ptr<VarDeclStmt> stmt) override;
    /** @brief Visit static array declaration statement */
    std::any visit(std::shared_ptr<StaticArrayDeclStmt> stmt) override;
    /** @brief Visit dynamic array declaration statement */
    std::any visit(std::shared_ptr<DynamicArrayDeclStmt> stmt) override;
    /** @brief Visit allocate statement */
    std::any visit(std::shared_ptr<AllocateStmt> stmt) override;
    /** @brief Visit deallocate statement */
    std::any visit(std::shared_ptr<DeallocateStmt> stmt) override;
    /** @brief Visit constant declaration statement */
    std::any visit(std::shared_ptr<ConstDeclStmt> stmt) override;
    /** @brief Visit input statement */
    std::any visit(std::shared_ptr<InputStmt> stmt) override;
    /** @brief Visit record type declaration statement */
    std::any visit(std::shared_ptr<RecordTypeDeclStmt> stmt) override;
    /** @brief Visit enum type declaration statement */
    std::any visit(std::shared_ptr<EnumTypeDeclStmt> stmt) override;
    /** @brief Visit constrained variable declaration statement */
    std::any visit(std::shared_ptr<ConstrainedVarDeclStmt> stmt) override;
    /** @brief Visit if statement */
    std::any visit(std::shared_ptr<IfStmt> stmt) override;
    /** @brief Visit while statement */
    std::any visit(std::shared_ptr<WhileStmt> stmt) override;
    /** @brief Visit repeat-until statement */
    std::any visit(std::shared_ptr<RepeatUntilStmt> stmt) override;
    /** @brief Visit depend-on statement */
    std::any visit(std::shared_ptr<DependOnStmt> stmt) override;
    /** @brief Visit output statement */
    std::any visit(std::shared_ptr<OutputStmt> stmt) override;
    /** @brief Visit traversal statement */
    std::any visit(std::shared_ptr<TraversalStmt> stmt) override;
    /** @brief Visit iterate-stop statement */
    std::any visit(std::shared_ptr<IterateStopStmt> stmt) override;
    /** @brief Visit repeat N times statement */
    std::any visit(std::shared_ptr<RepeatNTimesStmt> stmt) override;
    /** @brief Visit stop statement */
    std::any visit(std::shared_ptr<StopStmt> stmt) override;
    /** @brief Visit skip statement */
    std::any visit(std::shared_ptr<SkipStmt> stmt) override;
    /** @brief Visit procedure statement */
    std::any visit(std::shared_ptr<ProcedureStmt> stmt) override;
    /** @brief Visit function statement */
    std::any visit(std::shared_ptr<FunctionStmt> stmt) override;
    /** @brief Visit return statement */
    std::any visit(std::shared_ptr<ReturnStmt> stmt) override;

    // Expression visitors
    /** @brief Visit binary expression */
    std::any visit(std::shared_ptr<Binary> expr) override;
    /** @brief Visit unary expression */
    std::any visit(std::shared_ptr<Unary> expr) override;
    /** @brief Visit literal expression */
    std::any visit(std::shared_ptr<Literal> expr) override;
    /** @brief Visit variable expression */
    std::any visit(std::shared_ptr<Variable> expr) override;
    /** @brief Visit grouping expression */
    std::any visit(std::shared_ptr<Grouping> expr) override;
    /** @brief Visit assignment expression */
    std::any visit(std::shared_ptr<Assign> expr) override;
    /** @brief Visit call expression */
    std::any visit(std::shared_ptr<Call> expr) override;
    /** @brief Visit field access expression */
    std::any visit(std::shared_ptr<FieldAccess> expr) override;
    /** @brief Visit field assignment expression */
    std::any visit(std::shared_ptr<FieldAssign> expr) override;
    /** @brief Visit array access expression */
    std::any visit(std::shared_ptr<ArrayAccess> expr) override;

private:
    /**
     * @brief Helper to parenthesize expressions
     * @param name The operation name
     * @param exprs List of expressions
     * @return Parenthesized string representation
     */
    std::string parenthesize(const std::string& name, const std::vector<std::shared_ptr<Expression>>& exprs);
    
    /**
     * @brief Helper to parenthesize statements
     * @param name The statement name
     * @param stmts List of statements
     * @return Parenthesized string representation
     */
    std::string parenthesizeStatement(const std::string& name, const std::vector<std::shared_ptr<Statement>>& stmts);
    
    /** @brief Current indentation level */
    int indentLevel_ = 0;
    
    /**
     * @brief Generate indentation string
     * @return String with appropriate indentation
     */
    std::string indent();
};

} // namespace gate::ast

#endif // GATE_AST_AST_PRINTER_H
