/**
 * @file PascalCodeGenerator.h
 * @brief Pascal code generator for NOTAL AST transpilation
 * 
 * This file defines the PascalCodeGenerator class that implements the visitor
 * pattern to traverse a NOTAL Abstract Syntax Tree (AST) and generate
 * equivalent Pascal code. The generator handles all NOTAL language constructs
 * including variables, arrays, control structures, subprograms, and expressions.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */

#ifndef GATE_TRANSPILER_PASCAL_CODE_GENERATOR_H
#define GATE_TRANSPILER_PASCAL_CODE_GENERATOR_H

#include "ast/Expression.h"
#include "ast/Statement.h"
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <set>

namespace gate::transpiler {

// Using directives for convenience
using namespace gate::ast;

/**
 * @brief Pascal code generator using visitor pattern
 * 
 * The PascalCodeGenerator traverses a NOTAL AST and generates equivalent
 * Pascal code. It implements both ExpressionVisitor and StatementVisitor
 * interfaces to handle all AST node types. The generator maintains state
 * for proper indentation, variable tracking, and Pascal-specific constructs.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
class PascalCodeGenerator : public ExpressionVisitor, public StatementVisitor {
public:
    /**
     * @brief Generate Pascal code from NOTAL program AST
     * @param program Root program statement to transpile
     * @return Generated Pascal code as string
     */
    std::string generate(std::shared_ptr<ProgramStmt> program);

    // Statement visitors
    /** @brief Visit expression statement */
    std::any visit(std::shared_ptr<ExpressionStmt> stmt) override;
    /** @brief Visit block statement */
    std::any visit(std::shared_ptr<BlockStmt> stmt) override;
    /** @brief Visit program statement */
    std::any visit(std::shared_ptr<ProgramStmt> stmt) override;
    /** @brief Visit KAMUS (declarations) statement */
    std::any visit(std::shared_ptr<KamusStmt> stmt) override;
    /** @brief Visit ALGORITMA (implementation) statement */
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
    /** @brief Visit if-then-else statement */
    std::any visit(std::shared_ptr<IfStmt> stmt) override;
    /** @brief Visit while loop statement */
    std::any visit(std::shared_ptr<WhileStmt> stmt) override;
    /** @brief Visit repeat-until loop statement */
    std::any visit(std::shared_ptr<RepeatUntilStmt> stmt) override;
    /** @brief Visit depend-on (switch) statement */
    std::any visit(std::shared_ptr<DependOnStmt> stmt) override;
    /** @brief Visit output statement */
    std::any visit(std::shared_ptr<OutputStmt> stmt) override;
    /** @brief Visit traversal loop statement */
    std::any visit(std::shared_ptr<TraversalStmt> stmt) override;
    /** @brief Visit iterate/stop control statement */
    std::any visit(std::shared_ptr<IterateStopStmt> stmt) override;
    /** @brief Visit repeat N times loop statement */
    std::any visit(std::shared_ptr<RepeatNTimesStmt> stmt) override;
    /** @brief Visit stop (break) statement */
    std::any visit(std::shared_ptr<StopStmt> stmt) override;
    /** @brief Visit skip (continue) statement */
    std::any visit(std::shared_ptr<SkipStmt> stmt) override;
    /** @brief Visit procedure declaration statement */
    std::any visit(std::shared_ptr<ProcedureStmt> stmt) override;
    /** @brief Visit function declaration statement */
    std::any visit(std::shared_ptr<FunctionStmt> stmt) override;
    /** @brief Visit return statement */
    std::any visit(std::shared_ptr<ReturnStmt> stmt) override;

    // Expression visitors
    /** @brief Visit assignment expression */
    std::any visit(std::shared_ptr<Assign> expr) override;
    /** @brief Visit binary expression */
    std::any visit(std::shared_ptr<Binary> expr) override;
    /** @brief Visit unary expression */
    std::any visit(std::shared_ptr<Unary> expr) override;
    /** @brief Visit grouping (parentheses) expression */
    std::any visit(std::shared_ptr<Grouping> expr) override;
    /** @brief Visit literal expression */
    std::any visit(std::shared_ptr<Literal> expr) override;
    /** @brief Visit variable expression */
    std::any visit(std::shared_ptr<Variable> expr) override;
    /** @brief Visit function call expression */
    std::any visit(std::shared_ptr<Call> expr) override;
    /** @brief Visit field access expression */
    std::any visit(std::shared_ptr<FieldAccess> expr) override;
    /** @brief Visit field assignment expression */
    std::any visit(std::shared_ptr<FieldAssign> expr) override;
    /** @brief Visit array access expression */
    std::any visit(std::shared_ptr<ArrayAccess> expr) override;

private:
    /** @brief Output stream for generated Pascal code */
    std::stringstream out_;
    /** @brief Current indentation level */
    int indentLevel_ = 0;
    /** @brief Counter for generating unique loop variable names */
    int loopCounter_ = 0;
    /** @brief Flag indicating forward declaration mode */
    bool forwardDeclare_ = false;
    /** @brief Name of currently processing function */
    std::string currentFunctionName_;
    /** @brief Map of constant names to their literal values */
    std::map<std::string, std::shared_ptr<Literal>> constants_;
    /** @brief Map of constrained variable declarations */
    std::map<std::string, std::shared_ptr<ConstrainedVarDeclStmt>> constrainedVars_;
    /** @brief Map of dynamic array names to their dimensions */
    std::map<std::string, int> dynamicArrayDimensions_;
    /** @brief Stack of loop variable names for nested loops */
    std::vector<std::string> loopVariables_;
    /** @brief Set of casting functions used in the program */
    std::set<std::string> usedCastingFunctions_;

    /** @brief Add proper indentation to output stream */
    void indent();
    /** @brief Generate Pascal parameter list from NOTAL parameters */
    void generateParameterList(const std::vector<Parameter>& params);
    /** @brief Convert NOTAL type token to Pascal type string */
    std::string pascalType(const core::Token& token);
    /** @brief Evaluate expression and return Pascal code string */
    std::string evaluate(std::shared_ptr<Expression> expr);
    /** @brief Execute statement and generate Pascal code */
    void execute(std::shared_ptr<Statement> stmt);
    /** @brief Generate Pascal constraint checking code */
    std::string generateConstraintCheck(std::shared_ptr<ConstrainedVarDeclStmt> constrainedVar);
    /** @brief Pre-scan AST to collect information before code generation */
    void preScan(std::shared_ptr<Statement> stmt);

    /** @brief Scan statement for casting function usage */
    void scanForCastingFunctions(std::shared_ptr<Statement> stmt);
    /** @brief Scan expression for casting function usage */
    void scanExpression(std::shared_ptr<Expression> expr);
    /** @brief Generate forward declarations for casting functions */
    void generateCastingForwardDecls();
    /** @brief Generate implementations for casting functions */
    void generateCastingImplementations();
};

} // namespace gate::transpiler

#endif // GATE_TRANSPILER_PASCAL_CODE_GENERATOR_H