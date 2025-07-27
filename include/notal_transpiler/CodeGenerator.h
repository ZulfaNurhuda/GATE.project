#ifndef NOTAL_TRANSPILER_CODE_GENERATOR_H
#define NOTAL_TRANSPILER_CODE_GENERATOR_H

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <string>
#include <sstream>
#include <map>

namespace notal {

class CodeGenerator : public ast::ExprVisitor, public ast::StmtVisitor {
public:
    std::string generate(std::shared_ptr<ast::ProgramStmt> program);

    // Statement visitors
    std::any visit(std::shared_ptr<ast::ExpressionStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::BlockStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::ProgramStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::KamusStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::AlgoritmaStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::VarDeclStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::ConstDeclStmt> stmt) override; // Added
    std::any visit(std::shared_ptr<ast::InputStmt> stmt) override;     // Added
    std::any visit(std::shared_ptr<ast::RecordTypeDeclStmt> stmt) override; // Added
    std::any visit(std::shared_ptr<ast::EnumTypeDeclStmt> stmt) override;   // Added
    std::any visit(std::shared_ptr<ast::ConstrainedVarDeclStmt> stmt) override; // Added
    std::any visit(std::shared_ptr<ast::IfStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::WhileStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::RepeatUntilStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::DependOnStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::OutputStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::TraversalStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::IterateStopStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::RepeatNTimesStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::StopStmt> stmt) override;
    std::any visit(std::shared_ptr<ast::SkipStmt> stmt) override;

    // Expression Visitors
    std::any visit(std::shared_ptr<ast::Assign> expr) override;
    std::any visit(std::shared_ptr<ast::Binary> expr) override;
    std::any visit(std::shared_ptr<ast::Unary> expr) override;
    std::any visit(std::shared_ptr<ast::Grouping> expr) override;
    std::any visit(std::shared_ptr<ast::Literal> expr) override;
    std::any visit(std::shared_ptr<ast::Variable> expr) override;
    std::any visit(std::shared_ptr<ast::Call> expr) override;
    std::any visit(std::shared_ptr<ast::FieldAccess> expr) override;
    std::any visit(std::shared_ptr<ast::FieldAssign> expr) override;

private:
    std::stringstream out;
    int indentLevel = 0;
    int loopCounter = 0;
    std::map<std::string, std::shared_ptr<ast::Literal>> constants;
    std::map<std::string, std::shared_ptr<ast::ConstrainedVarDeclStmt>> constrainedVars; // Track constrained variables
    std::vector<std::string> loopVariables; // Track loop variables
    
    void indent();
    std::string pascalType(const Token& token);
    std::string evaluate(std::shared_ptr<ast::Expr> expr);
    void execute(std::shared_ptr<ast::Stmt> stmt);
    std::string generateConstraintCheck(std::shared_ptr<ast::ConstrainedVarDeclStmt> constrainedVar);
    void preScan(std::shared_ptr<ast::Stmt> stmt);
};

} // namespace notal

#endif // NOTAL_TRANSPILER_CODE_GENERATOR_H
