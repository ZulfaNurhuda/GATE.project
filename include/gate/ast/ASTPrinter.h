#ifndef GATE_AST_AST_PRINTER_H
#define GATE_AST_AST_PRINTER_H

#include "gate/ast/Expression.h"
#include "gate/ast/Statement.h"
#include <string>
#include <any>
#include <vector>

namespace gate::ast {

class ASTPrinter : public ExpressionVisitor, public StatementVisitor {
public:
    std::string print(std::shared_ptr<ProgramStmt> program);

    // Statement visitors
    std::any visit(std::shared_ptr<ExpressionStmt> stmt) override;
    std::any visit(std::shared_ptr<BlockStmt> stmt) override;
    std::any visit(std::shared_ptr<ProgramStmt> stmt) override;
    std::any visit(std::shared_ptr<KamusStmt> stmt) override;
    std::any visit(std::shared_ptr<AlgoritmaStmt> stmt) override;
    std::any visit(std::shared_ptr<VarDeclStmt> stmt) override;
    std::any visit(std::shared_ptr<StaticArrayDeclStmt> stmt) override;
    std::any visit(std::shared_ptr<DynamicArrayDeclStmt> stmt) override;
    std::any visit(std::shared_ptr<AllocateStmt> stmt) override;
    std::any visit(std::shared_ptr<DeallocateStmt> stmt) override;
    std::any visit(std::shared_ptr<ConstDeclStmt> stmt) override;
    std::any visit(std::shared_ptr<InputStmt> stmt) override;
    std::any visit(std::shared_ptr<RecordTypeDeclStmt> stmt) override;
    std::any visit(std::shared_ptr<EnumTypeDeclStmt> stmt) override;
    std::any visit(std::shared_ptr<ConstrainedVarDeclStmt> stmt) override;
    std::any visit(std::shared_ptr<IfStmt> stmt) override;
    std::any visit(std::shared_ptr<WhileStmt> stmt) override;
    std::any visit(std::shared_ptr<RepeatUntilStmt> stmt) override;
    std::any visit(std::shared_ptr<DependOnStmt> stmt) override;
    std::any visit(std::shared_ptr<OutputStmt> stmt) override;
    std::any visit(std::shared_ptr<TraversalStmt> stmt) override;
    std::any visit(std::shared_ptr<IterateStopStmt> stmt) override;
    std::any visit(std::shared_ptr<RepeatNTimesStmt> stmt) override;
    std::any visit(std::shared_ptr<StopStmt> stmt) override;
    std::any visit(std::shared_ptr<SkipStmt> stmt) override;
    std::any visit(std::shared_ptr<ProcedureStmt> stmt) override;
    std::any visit(std::shared_ptr<FunctionStmt> stmt) override;
    std::any visit(std::shared_ptr<ReturnStmt> stmt) override;

    // Expression visitors
    std::any visit(std::shared_ptr<Binary> expr) override;
    std::any visit(std::shared_ptr<Unary> expr) override;
    std::any visit(std::shared_ptr<Literal> expr) override;
    std::any visit(std::shared_ptr<Variable> expr) override;
    std::any visit(std::shared_ptr<Grouping> expr) override;
    std::any visit(std::shared_ptr<Assign> expr) override;
    std::any visit(std::shared_ptr<Call> expr) override;
    std::any visit(std::shared_ptr<FieldAccess> expr) override;
    std::any visit(std::shared_ptr<FieldAssign> expr) override;
    std::any visit(std::shared_ptr<ArrayAccess> expr) override;

private:
    std::string parenthesize(const std::string& name, const std::vector<std::shared_ptr<Expression>>& exprs);
    std::string parenthesizeStatement(const std::string& name, const std::vector<std::shared_ptr<Statement>>& stmts);
    int indentLevel_ = 0;
    std::string indent();
};

} // namespace gate::ast

#endif // GATE_AST_AST_PRINTER_H
