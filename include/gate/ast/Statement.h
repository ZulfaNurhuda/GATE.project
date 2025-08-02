/**
 * @file Statement.h
 * @brief AST statement node definitions for the NOTAL language
 * 
 * This file contains the complete definitions for all statement nodes in the Abstract Syntax Tree (AST)
 * used by the GATE transpiler. Each statement represents an executable construct in the NOTAL language,
 * such as variable declarations, control flow structures, and I/O operations.
 * 
 * The statement hierarchy implements the Visitor pattern to enable different operations
 * on the AST nodes, such as code generation, semantic analysis, and pretty printing.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */

#ifndef GATE_AST_STATEMENT_H
#define GATE_AST_STATEMENT_H

#include "gate/ast/Expression.h"
#include <any>
#include <memory>
#include <vector>

namespace gate::ast {

// Forward declarations for all statement types
// These declarations allow circular references between statement types
// and enable the visitor pattern implementation

/** @brief Expression statement - wraps an expression as a statement */
struct ExpressionStmt;
/** @brief Block statement - contains a sequence of statements */
struct BlockStmt;
/** @brief Program statement - root node of the AST */
struct ProgramStmt;
/** @brief Kamus (dictionary) statement - contains declarations */
struct KamusStmt;
/** @brief Algoritma (algorithm) statement - contains the main program body */
struct AlgoritmaStmt;
/** @brief Variable declaration statement */
struct VarDeclStmt;
/** @brief Constant declaration statement */
struct ConstDeclStmt;
/** @brief Input statement - reads from standard input */
struct InputStmt;
/** @brief Record type declaration statement */
struct RecordTypeDeclStmt;
/** @brief Enumeration type declaration statement */
struct EnumTypeDeclStmt;
/** @brief Constrained variable declaration with validation */
struct ConstrainedVarDeclStmt;
/** @brief If-then-else conditional statement */
struct IfStmt;
/** @brief While loop statement */
struct WhileStmt;
/** @brief Repeat-until loop statement */
struct RepeatUntilStmt;
/** @brief Output statement - writes to standard output */
struct OutputStmt;
/** @brief Depend-on statement - switch/case construct */
struct DependOnStmt;
/** @brief Traversal statement - for loop with iterator */
struct TraversalStmt;
/** @brief Iterate-stop statement - loop with break condition */
struct IterateStopStmt;
/** @brief Repeat N times statement - counted loop */
struct RepeatNTimesStmt;
/** @brief Stop statement - break from loop */
struct StopStmt;
/** @brief Skip statement - continue to next iteration */
struct SkipStmt;
/** @brief Procedure declaration statement */
struct ProcedureStmt;
/** @brief Function declaration statement */
struct FunctionStmt;
/** @brief Return statement - exits from function/procedure */
struct ReturnStmt;
/** @brief Static array declaration with fixed dimensions */
struct StaticArrayDeclStmt;
/** @brief Dynamic array declaration with runtime dimensions */
struct DynamicArrayDeclStmt;
/** @brief Memory allocation statement for dynamic arrays/pointers */
struct AllocateStmt;
/** @brief Memory deallocation statement */
struct DeallocateStmt;

/**
 * @brief Visitor interface for statement processing
 * 
 * This abstract class defines the visitor pattern interface for processing
 * statement nodes in the AST. Concrete implementations can perform various
 * operations such as code generation, semantic analysis, or pretty printing.
 * 
 * Each visit method corresponds to a specific statement type and returns
 * std::any to allow flexible return types depending on the visitor's purpose.
 */
class StatementVisitor {
public:
    /** @brief Visit an expression statement */
    virtual std::any visit(std::shared_ptr<ExpressionStmt> stmt) = 0;
    /** @brief Visit a block statement */
    virtual std::any visit(std::shared_ptr<BlockStmt> stmt) = 0;
    /** @brief Visit a program statement */
    virtual std::any visit(std::shared_ptr<ProgramStmt> stmt) = 0;
    /** @brief Visit a kamus (declarations) statement */
    virtual std::any visit(std::shared_ptr<KamusStmt> stmt) = 0;
    /** @brief Visit an algoritma (algorithm body) statement */
    virtual std::any visit(std::shared_ptr<AlgoritmaStmt> stmt) = 0;
    /** @brief Visit a variable declaration statement */
    virtual std::any visit(std::shared_ptr<VarDeclStmt> stmt) = 0;
    /** @brief Visit a constant declaration statement */
    virtual std::any visit(std::shared_ptr<ConstDeclStmt> stmt) = 0;
    /** @brief Visit an input statement */
    virtual std::any visit(std::shared_ptr<InputStmt> stmt) = 0;
    /** @brief Visit a record type declaration statement */
    virtual std::any visit(std::shared_ptr<RecordTypeDeclStmt> stmt) = 0;
    /** @brief Visit an enum type declaration statement */
    virtual std::any visit(std::shared_ptr<EnumTypeDeclStmt> stmt) = 0;
    /** @brief Visit a constrained variable declaration statement */
    virtual std::any visit(std::shared_ptr<ConstrainedVarDeclStmt> stmt) = 0;
    /** @brief Visit an if statement */
    virtual std::any visit(std::shared_ptr<IfStmt> stmt) = 0;
    /** @brief Visit a while loop statement */
    virtual std::any visit(std::shared_ptr<WhileStmt> stmt) = 0;
    /** @brief Visit a repeat-until loop statement */
    virtual std::any visit(std::shared_ptr<RepeatUntilStmt> stmt) = 0;
    /** @brief Visit an output statement */
    virtual std::any visit(std::shared_ptr<OutputStmt> stmt) = 0;
    /** @brief Visit a depend-on (switch-case) statement */
    virtual std::any visit(std::shared_ptr<DependOnStmt> stmt) = 0;
    /** @brief Visit a traversal (for-loop) statement */
    virtual std::any visit(std::shared_ptr<TraversalStmt> stmt) = 0;
    /** @brief Visit an iterate-stop loop statement */
    virtual std::any visit(std::shared_ptr<IterateStopStmt> stmt) = 0;
    /** @brief Visit a repeat-n-times loop statement */
    virtual std::any visit(std::shared_ptr<RepeatNTimesStmt> stmt) = 0;
    /** @brief Visit a stop statement */
    virtual std::any visit(std::shared_ptr<StopStmt> stmt) = 0;
    /** @brief Visit a skip statement */
    virtual std::any visit(std::shared_ptr<SkipStmt> stmt) = 0;
    /** @brief Visit a procedure declaration statement */
    virtual std::any visit(std::shared_ptr<ProcedureStmt> stmt) = 0;
    /** @brief Visit a function declaration statement */
    virtual std::any visit(std::shared_ptr<FunctionStmt> stmt) = 0;
    /** @brief Visit a return statement */
    virtual std::any visit(std::shared_ptr<ReturnStmt> stmt) = 0;
    /** @brief Visit a static array declaration statement */
    virtual std::any visit(std::shared_ptr<StaticArrayDeclStmt> stmt) = 0;
    /** @brief Visit a dynamic array declaration statement */
    virtual std::any visit(std::shared_ptr<DynamicArrayDeclStmt> stmt) = 0;
    /** @brief Visit an allocate statement */
    virtual std::any visit(std::shared_ptr<AllocateStmt> stmt) = 0;
    /** @brief Visit a deallocate statement */
    virtual std::any visit(std::shared_ptr<DeallocateStmt> stmt) = 0;
    virtual ~StatementVisitor() = default;
};

/**
 * @brief Base class for all statement nodes in the NOTAL AST
 * 
 * This abstract class serves as the foundation for all statement types in the NOTAL language.
 * It implements the Visitor pattern through the accept method and maintains parent relationships
 * in the AST hierarchy.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
class Statement {
public:
    /** @brief Weak reference to the parent statement in the AST hierarchy */
    std::weak_ptr<Statement> parent;
    
    /** @brief Virtual destructor for proper cleanup of derived classes */
    virtual ~Statement() = default;
    
    /**
     * @brief Accept method for the Visitor pattern
     * @param visitor The visitor object that will process this statement
     * @return std::any The result of the visitor's processing
     */
    virtual std::any accept(StatementVisitor& visitor) = 0;
};

/**
 * @brief Statement that wraps a single expression
 * 
 * Represents an expression used as a statement, such as function calls
 * or assignments that don't return a value.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct ExpressionStmt : Statement, public std::enable_shared_from_this<ExpressionStmt> {
    /** @brief The expression to be evaluated */
    std::shared_ptr<Expression> expression;
    
    /**
     * @brief Constructor for expression statement
     * @param expression The expression to wrap as a statement
     */
    explicit ExpressionStmt(std::shared_ptr<Expression> expression) : expression(std::move(expression)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Constant declaration statement
 * 
 * Represents a constant declaration in NOTAL with a name, type, and initializer value.
 * Constants are immutable once declared and must be initialized at declaration time.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct ConstDeclStmt : Statement, public std::enable_shared_from_this<ConstDeclStmt> {
    /** @brief The name token of the constant */
    core::Token name;
    /** @brief The type token of the constant */
    core::Token type;
    /** @brief The initializer expression for the constant value */
    std::shared_ptr<Expression> initializer;
    
    /**
     * @brief Constructor for constant declaration statement
     * @param name The name of the constant
     * @param type The type of the constant
     * @param initializer The expression that initializes the constant
     */
    ConstDeclStmt(core::Token name, core::Token type, std::shared_ptr<Expression> initializer) 
        : name(std::move(name)), type(std::move(type)), initializer(std::move(initializer)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Input statement for reading user input
 * 
 * Represents an input operation that reads a value from the user and stores it
 * in the specified variable.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct InputStmt : Statement, public std::enable_shared_from_this<InputStmt> {
    /** @brief The variable to store the input value */
    std::shared_ptr<Variable> variable;
    
    /**
     * @brief Constructor for input statement
     * @param variable The variable that will receive the input value
     */
    explicit InputStmt(std::shared_ptr<Variable> variable) : variable(std::move(variable)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Block statement containing a sequence of statements
 * 
 * Represents a block of statements that are executed sequentially.
 * This is used for grouping statements together in control structures
 * and function/procedure bodies.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct BlockStmt : Statement, public std::enable_shared_from_this<BlockStmt> {
    /** @brief The list of statements in this block */
    std::vector<std::shared_ptr<Statement>> statements;
    
    /**
     * @brief Constructor for block statement
     * @param statements The list of statements to include in this block
     */
    explicit BlockStmt(std::vector<std::shared_ptr<Statement>> statements) : statements(std::move(statements)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Root program statement representing the entire NOTAL program
 * 
 * This is the top-level AST node that represents a complete NOTAL program.
 * It contains the program name, declarations section (kamus), algorithm section,
 * and any subprograms (procedures/functions).
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct ProgramStmt : Statement, public std::enable_shared_from_this<ProgramStmt> {
    /** @brief The name token of the program */
    core::Token name;
    /** @brief The declarations section (kamus) of the program */
    std::shared_ptr<KamusStmt> kamus;
    /** @brief The algorithm section containing the main program logic */
    std::shared_ptr<AlgoritmaStmt> algoritma;
    /** @brief List of subprograms (procedures and functions) */
    std::vector<std::shared_ptr<Statement>> subprograms;
    
    /**
     * @brief Constructor for program statement
     * @param name The name of the program
     * @param kamus The declarations section
     * @param algoritma The algorithm section
     * @param subprograms List of subprograms
     */
    ProgramStmt(core::Token name, std::shared_ptr<KamusStmt> kamus, std::shared_ptr<AlgoritmaStmt> algoritma, 
                std::vector<std::shared_ptr<Statement>> subprograms) 
        : name(std::move(name)), kamus(std::move(kamus)), algoritma(std::move(algoritma)), 
          subprograms(std::move(subprograms)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Kamus (declarations) statement containing all variable and constant declarations
 * 
 * Represents the declarations section of a NOTAL program or subprogram.
 * Contains all variable declarations, constant declarations, and type declarations.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct KamusStmt : Statement, public std::enable_shared_from_this<KamusStmt> {
    /** @brief List of all declarations in this kamus section */
    std::vector<std::shared_ptr<Statement>> declarations;
    
    /**
     * @brief Constructor for kamus statement
     * @param declarations List of declaration statements
     */
    explicit KamusStmt(std::vector<std::shared_ptr<Statement>> declarations) : declarations(std::move(declarations)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Algoritma (algorithm) statement containing the executable code
 * 
 * Represents the algorithm section of a NOTAL program or subprogram.
 * Contains the main executable logic as a block of statements.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct AlgoritmaStmt : Statement, public std::enable_shared_from_this<AlgoritmaStmt> {
    /** @brief The body of the algorithm as a block statement */
    std::shared_ptr<BlockStmt> body;
    
    /**
     * @brief Constructor for algoritma statement
     * @param body The block statement containing the algorithm logic
     */
    explicit AlgoritmaStmt(std::shared_ptr<BlockStmt> body) : body(std::move(body)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Variable declaration statement
 * 
 * Represents a variable declaration in NOTAL with a name and type.
 * Can also represent pointer variables with a pointed-to type.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct VarDeclStmt : Statement, public std::enable_shared_from_this<VarDeclStmt> {
    /** @brief The name token of the variable */
    core::Token name;
    /** @brief The type token of the variable */
    core::Token type;
    /** @brief The pointed-to type token (for pointer variables) */
    core::Token pointedToType;
    
    /**
     * @brief Constructor for variable declaration statement
     * @param name The name of the variable
     * @param type The type of the variable
     * @param pointedToType The pointed-to type (optional, for pointers)
     */
    VarDeclStmt(core::Token name, core::Token type, core::Token pointedToType = {core::TokenType::UNKNOWN, ""}) : name(std::move(name)), type(std::move(type)), pointedToType(std::move(pointedToType)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief If conditional statement
 * 
 * Represents an if-then-else conditional statement in NOTAL.
 * Executes the then branch if the condition is true, otherwise executes the else branch.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct IfStmt : Statement, public std::enable_shared_from_this<IfStmt> {
    /** @brief The condition expression to evaluate */
    std::shared_ptr<Expression> condition;
    /** @brief The statement to execute if condition is true */
    std::shared_ptr<Statement> thenBranch;
    /** @brief The statement to execute if condition is false (optional) */
    std::shared_ptr<Statement> elseBranch;
    
    /**
     * @brief Constructor for if statement
     * @param condition The condition to evaluate
     * @param thenBranch Statement to execute if true
     * @param elseBranch Statement to execute if false (can be null)
     */
    IfStmt(std::shared_ptr<Expression> condition, std::shared_ptr<Statement> thenBranch, std::shared_ptr<Statement> elseBranch) 
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief While loop statement
 * 
 * Represents a while loop that executes its body repeatedly as long as
 * the condition remains true.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct WhileStmt : Statement, public std::enable_shared_from_this<WhileStmt> {
    /** @brief The condition expression to evaluate before each iteration */
    std::shared_ptr<Expression> condition;
    /** @brief The body of the loop to execute repeatedly */
    std::shared_ptr<BlockStmt> body;
    
    /**
     * @brief Constructor for while statement
     * @param condition The loop condition
     * @param body The loop body
     */
    WhileStmt(std::shared_ptr<Expression> condition, std::shared_ptr<BlockStmt> body) 
        : condition(std::move(condition)), body(std::move(body)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Output statement for displaying values
 * 
 * Represents an output operation that displays one or more expressions
 * to the user or output device.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct OutputStmt : Statement, public std::enable_shared_from_this<OutputStmt> {
    /** @brief List of expressions to output */
    std::vector<std::shared_ptr<Expression>> expressions;
    
    /**
     * @brief Constructor for output statement
     * @param expressions List of expressions to display
     */
    explicit OutputStmt(std::vector<std::shared_ptr<Expression>> expressions) : expressions(std::move(expressions)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Repeat-until loop statement
 * 
 * Represents a repeat-until loop that executes its body at least once,
 * then continues until the condition becomes true.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct RepeatUntilStmt : Statement, public std::enable_shared_from_this<RepeatUntilStmt> {
    /** @brief The body of the loop to execute */
    std::shared_ptr<BlockStmt> body;
    /** @brief The condition to check after each iteration */
    std::shared_ptr<Expression> condition;
    
    /**
     * @brief Constructor for repeat-until statement
     * @param body The loop body
     * @param condition The termination condition
     */
    RepeatUntilStmt(std::shared_ptr<BlockStmt> body, std::shared_ptr<Expression> condition) 
        : body(std::move(body)), condition(std::move(condition)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Depend-on statement (switch-case equivalent)
 * 
 * Represents a multi-way conditional statement that matches expressions
 * against multiple cases and executes the corresponding body.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct DependOnStmt : Statement, public std::enable_shared_from_this<DependOnStmt> {
    /**
     * @brief A single case in the depend-on statement
     * 
     * Contains the conditions to match and the body to execute if matched.
     */
    struct Case {
        /** @brief List of condition expressions for this case */
        std::vector<std::shared_ptr<Expression>> conditions;
        /** @brief The statement body to execute if conditions match */
        std::shared_ptr<Statement> body;
        
        /**
         * @brief Constructor for a case
         * @param conditions List of expressions to match
         * @param body Statement to execute if matched
         */
        Case(std::vector<std::shared_ptr<Expression>> conditions, std::shared_ptr<Statement> body) 
            : conditions(std::move(conditions)), body(std::move(body)) {}
    };
    
    /** @brief The expressions to evaluate and match against cases */
    std::vector<std::shared_ptr<Expression>> expressions;
    /** @brief List of all cases in this depend-on statement */
    std::vector<Case> cases;
    /** @brief The default branch to execute if no cases match (optional) */
    std::shared_ptr<Statement> otherwiseBranch;
    
    /**
     * @brief Constructor for depend-on statement
     * @param expressions Expressions to evaluate
     * @param cases List of cases to match against
     * @param otherwiseBranch Default branch (can be null)
     */
    DependOnStmt(std::vector<std::shared_ptr<Expression>> expressions, std::vector<Case> cases, std::shared_ptr<Statement> otherwiseBranch) 
        : expressions(std::move(expressions)), cases(std::move(cases)), otherwiseBranch(std::move(otherwiseBranch)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Record type declaration statement
 * 
 * Represents a record (struct) type declaration with named fields.
 * Each field has a name and type.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct RecordTypeDeclStmt : Statement, public std::enable_shared_from_this<RecordTypeDeclStmt> {
    /**
     * @brief A field in the record type
     * 
     * Contains the name and type of a single field in the record.
     */
    struct Field {
        /** @brief The name of the field */
        core::Token name;
        /** @brief The type of the field */
        core::Token type;
        
        /**
         * @brief Constructor for a field
         * @param name The field name
         * @param type The field type
         */
        Field(core::Token name, core::Token type) : name(std::move(name)), type(std::move(type)) {}
    };
    
    /** @brief The name of the record type */
    core::Token typeName;
    /** @brief List of fields in the record */
    std::vector<Field> fields;
    
    /**
     * @brief Constructor for record type declaration
     * @param typeName The name of the record type
     * @param fields List of fields in the record
     */
    RecordTypeDeclStmt(core::Token typeName, std::vector<Field> fields) 
        : typeName(std::move(typeName)), fields(std::move(fields)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Enumeration type declaration statement
 * 
 * Represents an enumeration type declaration with a list of possible values.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct EnumTypeDeclStmt : Statement, public std::enable_shared_from_this<EnumTypeDeclStmt> {
    /** @brief The name of the enumeration type */
    core::Token typeName;
    /** @brief List of enumeration values */
    std::vector<core::Token> values;
    
    /**
     * @brief Constructor for enum type declaration
     * @param typeName The name of the enum type
     * @param values List of enumeration values
     */
    EnumTypeDeclStmt(core::Token typeName, std::vector<core::Token> values) 
        : typeName(std::move(typeName)), values(std::move(values)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Constrained variable declaration statement
 * 
 * Represents a variable declaration with a constraint expression
 * that must be satisfied by the variable's value.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct ConstrainedVarDeclStmt : Statement, public std::enable_shared_from_this<ConstrainedVarDeclStmt> {
    /** @brief The name of the constrained variable */
    core::Token name;
    /** @brief The type of the constrained variable */
    core::Token type;
    /** @brief The constraint expression that must be satisfied */
    std::shared_ptr<Expression> constraint;
    
    /**
     * @brief Constructor for constrained variable declaration
     * @param name The variable name
     * @param type The variable type
     * @param constraint The constraint expression
     */
    ConstrainedVarDeclStmt(core::Token name, core::Token type, std::shared_ptr<Expression> constraint) 
        : name(std::move(name)), type(std::move(type)), constraint(std::move(constraint)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Traversal loop statement (for-loop equivalent)
 * 
 * Represents a for-loop that iterates over a range with an optional step.
 * The iterator variable takes values from start to end with the given step.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct TraversalStmt : Statement, public std::enable_shared_from_this<TraversalStmt> {
    /** @brief The iterator variable */
    core::Token iterator;
    /** @brief The starting value expression */
    std::shared_ptr<Expression> start;
    /** @brief The ending value expression */
    std::shared_ptr<Expression> end;
    /** @brief The step value expression (optional) */
    std::shared_ptr<Expression> step;
    /** @brief The body of the loop */
    std::shared_ptr<BlockStmt> body;
    
    /**
     * @brief Constructor for traversal statement
     * @param iterator The iterator variable
     * @param start The start value
     * @param end The end value
     * @param step The step value
     * @param body The loop body
     */
    TraversalStmt(core::Token iterator, std::shared_ptr<Expression> start, std::shared_ptr<Expression> end, 
                  std::shared_ptr<Expression> step, std::shared_ptr<BlockStmt> body) 
        : iterator(std::move(iterator)), start(std::move(start)), end(std::move(end)), 
          step(std::move(step)), body(std::move(body)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Iterate-stop loop statement
 * 
 * Represents a loop that executes its body and then checks a condition.
 * The loop continues until the condition becomes true.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct IterateStopStmt : Statement, public std::enable_shared_from_this<IterateStopStmt> {
    /** @brief The body of the loop */
    std::shared_ptr<BlockStmt> body;
    /** @brief The stop condition */
    std::shared_ptr<Expression> condition;
    
    /**
     * @brief Constructor for iterate-stop statement
     * @param body The loop body
     * @param condition The stop condition
     */
    IterateStopStmt(std::shared_ptr<BlockStmt> body, std::shared_ptr<Expression> condition) 
        : body(std::move(body)), condition(std::move(condition)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Repeat N times statement
 * 
 * Represents a loop that executes its body a specified number of times.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct RepeatNTimesStmt : Statement, public std::enable_shared_from_this<RepeatNTimesStmt> {
    /** @brief The number of times to repeat */
    std::shared_ptr<Expression> times;
    /** @brief The body of the loop */
    std::shared_ptr<BlockStmt> body;
    
    /**
     * @brief Constructor for repeat N times statement
     * @param times The number of repetitions
     * @param body The loop body
     */
    RepeatNTimesStmt(std::shared_ptr<Expression> times, std::shared_ptr<BlockStmt> body) 
        : times(std::move(times)), body(std::move(body)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Stop statement
 * 
 * Represents a break statement that exits the current loop.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct StopStmt : Statement, public std::enable_shared_from_this<StopStmt> {
    /** @brief Default constructor */
    StopStmt() {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Skip statement
 * 
 * Represents a continue statement that skips to the next iteration of the current loop.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct SkipStmt : Statement, public std::enable_shared_from_this<SkipStmt> {
    /** @brief Default constructor */
    SkipStmt() {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Parameter mode enumeration
 * 
 * Defines the parameter passing modes for functions and procedures.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
enum class ParameterMode {
    INPUT,        /**< Input parameter (pass by value) */
    OUTPUT,       /**< Output parameter (pass by reference, write-only) */
    INPUT_OUTPUT  /**< Input-output parameter (pass by reference, read-write) */
};

/**
 * @brief Parameter structure for functions and procedures
 * 
 * Represents a single parameter with its mode, name, and type.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct Parameter {
    /** @brief The parameter passing mode */
    ParameterMode mode;
    /** @brief The parameter name */
    core::Token name;
    /** @brief The parameter type */
    core::Token type;
    
    /**
     * @brief Constructor for parameter
     * @param mode The parameter mode
     * @param name The parameter name
     * @param type The parameter type
     */
    Parameter(ParameterMode mode, core::Token name, core::Token type) 
        : mode(mode), name(std::move(name)), type(std::move(type)) {}
};

/**
 * @brief Procedure statement
 * 
 * Represents a procedure declaration with parameters, local declarations, and body.
 * Procedures are subroutines that do not return a value.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct ProcedureStmt : Statement, public std::enable_shared_from_this<ProcedureStmt> {
    /** @brief The procedure name */
    core::Token name;
    /** @brief List of procedure parameters */
    std::vector<Parameter> params;
    /** @brief Local declarations section */
    std::shared_ptr<KamusStmt> kamus;
    /** @brief Procedure body */
    std::shared_ptr<AlgoritmaStmt> body;
    
    /**
     * @brief Constructor for procedure statement
     * @param name The procedure name
     * @param params List of parameters
     * @param kamus Local declarations
     * @param body Procedure body
     */
    ProcedureStmt(core::Token name, std::vector<Parameter> params, std::shared_ptr<KamusStmt> kamus, std::shared_ptr<AlgoritmaStmt> body) 
        : name(std::move(name)), params(std::move(params)), kamus(std::move(kamus)), body(std::move(body)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Function statement
 * 
 * Represents a function declaration with parameters, return type, local declarations, and body.
 * Functions are subroutines that return a value of the specified type.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct FunctionStmt : Statement, public std::enable_shared_from_this<FunctionStmt> {
    /** @brief The function name */
    core::Token name;
    /** @brief List of function parameters */
    std::vector<Parameter> params;
    /** @brief The return type of the function */
    core::Token returnType;
    /** @brief Local declarations section */
    std::shared_ptr<KamusStmt> kamus;
    /** @brief Function body */
    std::shared_ptr<AlgoritmaStmt> body;
    
    /**
     * @brief Constructor for function statement
     * @param name The function name
     * @param params List of parameters
     * @param returnType The return type
     * @param kamus Local declarations
     * @param body Function body
     */
    FunctionStmt(core::Token name, std::vector<Parameter> params, core::Token returnType, 
                 std::shared_ptr<KamusStmt> kamus, std::shared_ptr<AlgoritmaStmt> body) 
        : name(std::move(name)), params(std::move(params)), returnType(std::move(returnType)), 
          kamus(std::move(kamus)), body(std::move(body)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Return statement
 * 
 * Represents a return statement that exits a function and optionally returns a value.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct ReturnStmt : Statement, public std::enable_shared_from_this<ReturnStmt> {
    /** @brief The return keyword token */
    core::Token keyword;
    /** @brief The value to return (optional) */
    std::shared_ptr<Expression> value;
    
    /**
     * @brief Constructor for return statement
     * @param keyword The return keyword
     * @param value The return value expression
     */
    explicit ReturnStmt(core::Token keyword, std::shared_ptr<Expression> value) 
        : keyword(std::move(keyword)), value(std::move(value)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Static array declaration statement
 * 
 * Represents a static array declaration with fixed dimensions known at compile time.
 * Each dimension has a start and end range.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct StaticArrayDeclStmt : Statement, public std::enable_shared_from_this<StaticArrayDeclStmt> {
    /**
     * @brief A dimension of the static array
     * 
     * Represents a single dimension with start and end bounds.
     */
    struct Dimension {
        /** @brief The starting index of the dimension */
        std::shared_ptr<Expression> start;
        /** @brief The ending index of the dimension */
        std::shared_ptr<Expression> end;
    };
    
    /** @brief The array name */
    core::Token name;
    /** @brief List of array dimensions */
    std::vector<Dimension> dimensions;
    /** @brief The element type of the array */
    core::Token elementType;
    
    /**
     * @brief Constructor for static array declaration
     * @param name The array name
     * @param dimensions List of array dimensions
     * @param elementType The element type
     */
    StaticArrayDeclStmt(core::Token name, std::vector<Dimension> dimensions, core::Token elementType) 
        : name(std::move(name)), dimensions(std::move(dimensions)), elementType(std::move(elementType)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Dynamic array declaration statement
 * 
 * Represents a dynamic array declaration with dimensions determined at runtime.
 * The actual sizes are allocated later using allocate statements.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct DynamicArrayDeclStmt : Statement, public std::enable_shared_from_this<DynamicArrayDeclStmt> {
    /** @brief The array name */
    core::Token name;
    /** @brief The number of dimensions */
    int dimensions;
    /** @brief The element type of the array */
    core::Token elementType;
    
    /**
     * @brief Constructor for dynamic array declaration
     * @param name The array name
     * @param dimensions Number of dimensions
     * @param elementType The element type
     */
    DynamicArrayDeclStmt(core::Token name, int dimensions, core::Token elementType) 
        : name(std::move(name)), dimensions(dimensions), elementType(std::move(elementType)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Allocate statement
 * 
 * Represents a memory allocation statement for dynamic arrays.
 * Allocates memory for the specified array with the given sizes.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct AllocateStmt : Statement, public std::enable_shared_from_this<AllocateStmt> {
    /** @brief The array to allocate memory for */
    std::shared_ptr<Expression> callee;
    /** @brief List of size expressions for each dimension */
    std::vector<std::shared_ptr<Expression>> sizes;
    
    /**
     * @brief Constructor for allocate statement
     * @param callee The array to allocate
     * @param sizes List of dimension sizes
     */
    AllocateStmt(std::shared_ptr<Expression> callee, std::vector<std::shared_ptr<Expression>> sizes) 
        : callee(std::move(callee)), sizes(std::move(sizes)) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

/**
 * @brief Deallocate statement
 * 
 * Represents a memory deallocation statement for dynamic arrays.
 * Frees the memory allocated for the specified array.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct DeallocateStmt : Statement, public std::enable_shared_from_this<DeallocateStmt> {
    /** @brief The array to deallocate memory for */
    std::shared_ptr<Expression> callee;
    /** @brief The dimension to deallocate */
    int dimension;
    
    /**
     * @brief Constructor for deallocate statement
     * @param callee The array to deallocate
     * @param dimension The dimension to deallocate
     */
    DeallocateStmt(std::shared_ptr<Expression> callee, int dimension) 
        : callee(std::move(callee)), dimension(dimension) {}
    
    /** @brief Accept method for visitor pattern */
    std::any accept(StatementVisitor& visitor) override { return visitor.visit(shared_from_this()); }
};

} // namespace gate::ast

#endif // GATE_AST_STATEMENT_H
