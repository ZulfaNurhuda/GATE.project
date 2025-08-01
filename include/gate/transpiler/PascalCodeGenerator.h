#ifndef GATE_TRANSPILER_PASCAL_CODE_GENERATOR_H
#define GATE_TRANSPILER_PASCAL_CODE_GENERATOR_H

#include "gate/ast/Expression.h"
#include "gate/ast/Statement.h"
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <set>

namespace gate::transpiler {

// Using directives for convenience
using namespace gate::ast;

class PascalCodeGenerator : public ExpressionVisitor, public StatementVisitor {
public:
    std::string generate(std::shared_ptr<ProgramStmt> program);

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

    // Expression Visitors
    std::any visit(std::shared_ptr<Assign> expr) override;
    std::any visit(std::shared_ptr<Binary> expr) override;
    std::any visit(std::shared_ptr<Unary> expr) override;
    std::any visit(std::shared_ptr<Grouping> expr) override;
    std::any visit(std::shared_ptr<Literal> expr) override;
    std::any visit(std::shared_ptr<Variable> expr) override;
    std::any visit(std::shared_ptr<Call> expr) override;
    std::any visit(std::shared_ptr<FieldAccess> expr) override;
    std::any visit(std::shared_ptr<FieldAssign> expr) override;
    std::any visit(std::shared_ptr<ArrayAccess> expr) override;

private:
    std::stringstream out_;
    int indentLevel_ = 0;
    int loopCounter_ = 0;
    bool forwardDeclare_ = false;
    std::string currentFunctionName_;
    std::map<std::string, std::shared_ptr<Literal>> constants_;
    std::map<std::string, std::shared_ptr<ConstrainedVarDeclStmt>> constrainedVars_;
    std::map<std::string, int> dynamicArrayDimensions_;
    std::vector<std::string> loopVariables_;
    std::set<std::string> usedCastingFunctions_;

    void indent();
    void generateParameterList(const std::vector<Parameter>& params);
    std::string pascalType(const core::Token& token);
    std::string evaluate(std::shared_ptr<Expression> expr);
    void execute(std::shared_ptr<Statement> stmt);
    std::string generateConstraintCheck(std::shared_ptr<ConstrainedVarDeclStmt> constrainedVar);
    void preScan(std::shared_ptr<Statement> stmt);

    void scanForCastingFunctions(std::shared_ptr<Statement> stmt);
    void scanExpression(std::shared_ptr<Expression> expr);
    void generateCastingForwardDecls();
    void generateCastingImplementations();
};

} // namespace gate::transpiler

#endif // GATE_TRANSPILER_PASCAL_CODE_GENERATOR_H
