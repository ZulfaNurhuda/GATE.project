#ifndef GATE_AST_STATEMENT_H
#define GATE_AST_STATEMENT_H

#include "gate/ast/Expression.h"
#include <any>
#include <memory>
#include <vector>

namespace gate::ast {

// Forward declarations for all statement types
struct ExpressionStmt;
struct BlockStmt;
struct ProgramStmt;
struct KamusStmt;
struct AlgoritmaStmt;
struct VarDeclStmt;
struct ConstDeclStmt;
struct InputStmt;
struct RecordTypeDeclStmt;
struct EnumTypeDeclStmt;
struct ConstrainedVarDeclStmt;
struct IfStmt;
struct WhileStmt;
struct RepeatUntilStmt;
struct OutputStmt;
struct DependOnStmt;
struct TraversalStmt;
struct IterateStopStmt;
struct RepeatNTimesStmt;
struct StopStmt;
struct SkipStmt;
struct ProcedureStmt;
struct FunctionStmt;
struct ReturnStmt;
struct StaticArrayDeclStmt;
struct DynamicArrayDeclStmt;
struct AllocateStmt;
struct DeallocateStmt;

// Visitor interface for statements
class StatementVisitor {
public:
    virtual std::any visit(std::shared_ptr<ExpressionStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<BlockStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<ProgramStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<KamusStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<AlgoritmaStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<VarDeclStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<ConstDeclStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<InputStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<RecordTypeDeclStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<EnumTypeDeclStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<ConstrainedVarDeclStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<IfStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<WhileStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<RepeatUntilStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<OutputStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<DependOnStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<TraversalStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<IterateStopStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<RepeatNTimesStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<StopStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<SkipStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<ProcedureStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<FunctionStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<ReturnStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<StaticArrayDeclStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<DynamicArrayDeclStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<AllocateStmt> stmt) = 0;
    virtual std::any visit(std::shared_ptr<DeallocateStmt> stmt) = 0;
    virtual ~StatementVisitor() = default;
};

// Base class for all statement nodes
class Statement {
public:
    std::weak_ptr<Statement> parent;
    virtual ~Statement() = default;
    virtual std::any accept(StatementVisitor& visitor) = 0;
};

// Expression statement node
struct ExpressionStmt : Statement, public std::enable_shared_from_this<ExpressionStmt> {
    std::shared_ptr<Expression> expression;
    explicit ExpressionStmt(std::shared_ptr<Expression> expression) : expression(std::move(expression)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Constant declaration statement node
struct ConstDeclStmt : Statement, public std::enable_shared_from_this<ConstDeclStmt> {
    core::Token name;
    core::Token type;
    std::shared_ptr<Expression> initializer;
    ConstDeclStmt(core::Token name, core::Token type, std::shared_ptr<Expression> initializer) : name(std::move(name)), type(std::move(type)), initializer(std::move(initializer)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Input statement node
struct InputStmt : Statement, public std::enable_shared_from_this<InputStmt> {
    std::shared_ptr<Variable> variable;
    explicit InputStmt(std::shared_ptr<Variable> variable) : variable(std::move(variable)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Block statement node
struct BlockStmt : Statement, public std::enable_shared_from_this<BlockStmt> {
    std::vector<std::shared_ptr<Statement>> statements;
    explicit BlockStmt(std::vector<std::shared_ptr<Statement>> statements) : statements(std::move(statements)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Program statement node
struct ProgramStmt : Statement, public std::enable_shared_from_this<ProgramStmt> {
    core::Token name;
    std::shared_ptr<KamusStmt> kamus;
    std::shared_ptr<AlgoritmaStmt> algoritma;
    std::vector<std::shared_ptr<Statement>> subprograms;
    ProgramStmt(core::Token name, std::shared_ptr<KamusStmt> kamus, std::shared_ptr<AlgoritmaStmt> algoritma, std::vector<std::shared_ptr<Statement>> subprograms) : name(std::move(name)), kamus(std::move(kamus)), algoritma(std::move(algoritma)), subprograms(std::move(subprograms)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Kamus (dictionary) block statement node
struct KamusStmt : Statement, public std::enable_shared_from_this<KamusStmt> {
    std::vector<std::shared_ptr<Statement>> declarations;
    explicit KamusStmt(std::vector<std::shared_ptr<Statement>> declarations) : declarations(std::move(declarations)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Algoritma (algorithm) block statement node
struct AlgoritmaStmt : Statement, public std::enable_shared_from_this<AlgoritmaStmt> {
    std::shared_ptr<BlockStmt> body;
    explicit AlgoritmaStmt(std::shared_ptr<BlockStmt> body) : body(std::move(body)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Variable declaration statement node
struct VarDeclStmt : Statement, public std::enable_shared_from_this<VarDeclStmt> {
    core::Token name;
    core::Token type;
    core::Token pointedToType;
    VarDeclStmt(core::Token name, core::Token type, core::Token pointedToType = {core::TokenType::UNKNOWN, ""}) : name(std::move(name)), type(std::move(type)), pointedToType(std::move(pointedToType)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// If statement node
struct IfStmt : Statement, public std::enable_shared_from_this<IfStmt> {
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> thenBranch;
    std::shared_ptr<Statement> elseBranch;
    IfStmt(std::shared_ptr<Expression> condition, std::shared_ptr<Statement> thenBranch, std::shared_ptr<Statement> elseBranch) : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// While loop statement node
struct WhileStmt : Statement, public std::enable_shared_from_this<WhileStmt> {
    std::shared_ptr<Expression> condition;
    std::shared_ptr<BlockStmt> body;
    WhileStmt(std::shared_ptr<Expression> condition, std::shared_ptr<BlockStmt> body) : condition(std::move(condition)), body(std::move(body)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Output statement node
struct OutputStmt : Statement, public std::enable_shared_from_this<OutputStmt> {
    std::vector<std::shared_ptr<Expression>> expressions;
    explicit OutputStmt(std::vector<std::shared_ptr<Expression>> expressions) : expressions(std::move(expressions)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Repeat..Until loop statement node
struct RepeatUntilStmt : Statement, public std::enable_shared_from_this<RepeatUntilStmt> {
    std::shared_ptr<BlockStmt> body;
    std::shared_ptr<Expression> condition;
    RepeatUntilStmt(std::shared_ptr<BlockStmt> body, std::shared_ptr<Expression> condition) : body(std::move(body)), condition(std::move(condition)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Depend On statement node
struct DependOnStmt : Statement, public std::enable_shared_from_this<DependOnStmt> {
    struct Case {
        std::vector<std::shared_ptr<Expression>> conditions;
        std::shared_ptr<Statement> body;
        Case(std::vector<std::shared_ptr<Expression>> conditions, std::shared_ptr<Statement> body) : conditions(std::move(conditions)), body(std::move(body)) {}
    };
    std::vector<std::shared_ptr<Expression>> expressions;
    std::vector<Case> cases;
    std::shared_ptr<Statement> otherwiseBranch;
    DependOnStmt(std::vector<std::shared_ptr<Expression>> expressions, std::vector<Case> cases, std::shared_ptr<Statement> otherwiseBranch) : expressions(std::move(expressions)), cases(std::move(cases)), otherwiseBranch(std::move(otherwiseBranch)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Record type declaration statement node
struct RecordTypeDeclStmt : Statement, public std::enable_shared_from_this<RecordTypeDeclStmt> {
    struct Field {
        core::Token name;
        core::Token type;
        Field(core::Token name, core::Token type) : name(std::move(name)), type(std::move(type)) {}
    };
    core::Token typeName;
    std::vector<Field> fields;
    RecordTypeDeclStmt(core::Token typeName, std::vector<Field> fields) : typeName(std::move(typeName)), fields(std::move(fields)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Enum type declaration statement node
struct EnumTypeDeclStmt : Statement, public std::enable_shared_from_this<EnumTypeDeclStmt> {
    core::Token typeName;
    std::vector<core::Token> values;
    EnumTypeDeclStmt(core::Token typeName, std::vector<core::Token> values) : typeName(std::move(typeName)), values(std::move(values)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Constrained variable declaration statement node
struct ConstrainedVarDeclStmt : Statement, public std::enable_shared_from_this<ConstrainedVarDeclStmt> {
    core::Token name;
    core::Token type;
    std::shared_ptr<Expression> constraint;
    ConstrainedVarDeclStmt(core::Token name, core::Token type, std::shared_ptr<Expression> constraint) : name(std::move(name)), type(std::move(type)), constraint(std::move(constraint)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Traversal loop statement node
struct TraversalStmt : Statement, public std::enable_shared_from_this<TraversalStmt> {
    core::Token iterator;
    std::shared_ptr<Expression> start;
    std::shared_ptr<Expression> end;
    std::shared_ptr<Expression> step;
    std::shared_ptr<BlockStmt> body;
    TraversalStmt(core::Token iterator, std::shared_ptr<Expression> start, std::shared_ptr<Expression> end, std::shared_ptr<Expression> step, std::shared_ptr<BlockStmt> body) : iterator(std::move(iterator)), start(std::move(start)), end(std::move(end)), step(std::move(step)), body(std::move(body)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Iterate-stop loop statement node
struct IterateStopStmt : Statement, public std::enable_shared_from_this<IterateStopStmt> {
    std::shared_ptr<BlockStmt> body;
    std::shared_ptr<Expression> condition;
    IterateStopStmt(std::shared_ptr<BlockStmt> body, std::shared_ptr<Expression> condition) : body(std::move(body)), condition(std::move(condition)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Repeat N times statement node
struct RepeatNTimesStmt : Statement, public std::enable_shared_from_this<RepeatNTimesStmt> {
    std::shared_ptr<Expression> times;
    std::shared_ptr<BlockStmt> body;
    RepeatNTimesStmt(std::shared_ptr<Expression> times, std::shared_ptr<BlockStmt> body) : times(std::move(times)), body(std::move(body)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Stop statement node
struct StopStmt : Statement, public std::enable_shared_from_this<StopStmt> {
    StopStmt() {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Skip statement node
struct SkipStmt : Statement, public std::enable_shared_from_this<SkipStmt> {
    SkipStmt() {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Parameter mode for functions/procedures
enum class ParameterMode {
    INPUT,
    OUTPUT,
    INPUT_OUTPUT
};

// Parameter struct for functions/procedures
struct Parameter {
    ParameterMode mode;
    core::Token name;
    core::Token type;
    Parameter(ParameterMode mode, core::Token name, core::Token type) : mode(mode), name(std::move(name)), type(std::move(type)) {}
};

// Procedure statement node
struct ProcedureStmt : Statement, public std::enable_shared_from_this<ProcedureStmt> {
    core::Token name;
    std::vector<Parameter> params;
    std::shared_ptr<KamusStmt> kamus;
    std::shared_ptr<AlgoritmaStmt> body;
    ProcedureStmt(core::Token name, std::vector<Parameter> params, std::shared_ptr<KamusStmt> kamus, std::shared_ptr<AlgoritmaStmt> body) : name(std::move(name)), params(std::move(params)), kamus(std::move(kamus)), body(std::move(body)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Function statement node
struct FunctionStmt : Statement, public std::enable_shared_from_this<FunctionStmt> {
    core::Token name;
    std::vector<Parameter> params;
    core::Token returnType;
    std::shared_ptr<KamusStmt> kamus;
    std::shared_ptr<AlgoritmaStmt> body;
    FunctionStmt(core::Token name, std::vector<Parameter> params, core::Token returnType, std::shared_ptr<KamusStmt> kamus, std::shared_ptr<AlgoritmaStmt> body) : name(std::move(name)), params(std::move(params)), returnType(std::move(returnType)), kamus(std::move(kamus)), body(std::move(body)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Return statement node
struct ReturnStmt : Statement, public std::enable_shared_from_this<ReturnStmt> {
    core::Token keyword;
    std::shared_ptr<Expression> value;
    explicit ReturnStmt(core::Token keyword, std::shared_ptr<Expression> value) : keyword(std::move(keyword)), value(std::move(value)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Static array declaration statement node
struct StaticArrayDeclStmt : Statement, public std::enable_shared_from_this<StaticArrayDeclStmt> {
    struct Dimension {
        std::shared_ptr<Expression> start;
        std::shared_ptr<Expression> end;
    };
    core::Token name;
    std::vector<Dimension> dimensions;
    core::Token elementType;
    StaticArrayDeclStmt(core::Token name, std::vector<Dimension> dimensions, core::Token elementType) : name(std::move(name)), dimensions(std::move(dimensions)), elementType(std::move(elementType)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Dynamic array declaration statement node
struct DynamicArrayDeclStmt : Statement, public std::enable_shared_from_this<DynamicArrayDeclStmt> {
    core::Token name;
    int dimensions;
    core::Token elementType;
    DynamicArrayDeclStmt(core::Token name, int dimensions, core::Token elementType) : name(std::move(name)), dimensions(dimensions), elementType(std::move(elementType)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Allocate statement node
struct AllocateStmt : Statement, public std::enable_shared_from_this<AllocateStmt> {
    std::shared_ptr<Expression> callee;
    std::vector<std::shared_ptr<Expression>> sizes;
    AllocateStmt(std::shared_ptr<Expression> callee, std::vector<std::shared_ptr<Expression>> sizes) : callee(std::move(callee)), sizes(std::move(sizes)) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

// Deallocate statement node
struct DeallocateStmt : Statement, public std::enable_shared_from_this<DeallocateStmt> {
    std::shared_ptr<Expression> callee;
    int dimension;
    DeallocateStmt(std::shared_ptr<Expression> callee, int dimension) : callee(std::move(callee)), dimension(dimension) {}
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};


} // namespace gate::ast

#endif // GATE_AST_STATEMENT_H
