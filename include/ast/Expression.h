/**
 * @file Expression.h
 * @brief Expression AST nodes for NOTAL Language
 * 
 * This file defines all expression-related Abstract Syntax Tree (AST) nodes
 * for the NOTAL language. It includes the base Expression class, the
 * ExpressionVisitor interface, and concrete expression types such as binary
 * operations, unary operations, literals, variables, function calls, and
 * field/array access operations.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */

#ifndef GATE_AST_EXPRESSION_H
#define GATE_AST_EXPRESSION_H

#include "core/Token.h"
#include <any>
#include <memory>
#include <vector>

namespace gate::ast {

// Forward declarations for expression types
/** @brief Binary operation expression (e.g., a + b, x * y) */
struct Binary;
/** @brief Unary operation expression (e.g., -x, not y) */
struct Unary;
/** @brief Literal value expression (e.g., 42, "hello", true) */
struct Literal;
/** @brief Variable reference expression (e.g., myVar) */
struct Variable;
/** @brief Parenthesized expression (e.g., (a + b)) */
struct Grouping;
/** @brief Assignment expression (e.g., x <- 5) */
struct Assign;
/** @brief Function/procedure call expression (e.g., func(a, b)) */
struct Call;
/** @brief Record field access expression (e.g., student.name) */
struct FieldAccess;
/** @brief Record field assignment expression (e.g., student.name <- "John") */
struct FieldAssign;
/** @brief Array element access expression (e.g., arr[i]) */
struct ArrayAccess;

/**
 * @brief Visitor interface for expression nodes
 * 
 * Defines the visitor pattern interface for traversing expression AST nodes.
 * Implementations of this interface can perform various operations on
 * expressions such as evaluation, type checking, or code generation.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
class ExpressionVisitor {
public:
    /** @brief Visit binary expression */
    virtual std::any visit(std::shared_ptr<Binary> expr) = 0;
    /** @brief Visit unary expression */
    virtual std::any visit(std::shared_ptr<Unary> expr) = 0;
    /** @brief Visit literal expression */
    virtual std::any visit(std::shared_ptr<Literal> expr) = 0;
    /** @brief Visit variable expression */
    virtual std::any visit(std::shared_ptr<Variable> expr) = 0;
    /** @brief Visit grouping expression */
    virtual std::any visit(std::shared_ptr<Grouping> expr) = 0;
    /** @brief Visit assignment expression */
    virtual std::any visit(std::shared_ptr<Assign> expr) = 0;
    /** @brief Visit call expression */
    virtual std::any visit(std::shared_ptr<Call> expr) = 0;
    /** @brief Visit field access expression */
    virtual std::any visit(std::shared_ptr<FieldAccess> expr) = 0;
    /** @brief Visit field assignment expression */
    virtual std::any visit(std::shared_ptr<FieldAssign> expr) = 0;
    /** @brief Visit array access expression */
    virtual std::any visit(std::shared_ptr<ArrayAccess> expr) = 0;
    /** @brief Virtual destructor */
    virtual ~ExpressionVisitor() = default;
};

/**
 * @brief Base class for all expression AST nodes
 * 
 * This abstract base class defines the interface for all expression nodes
 * in the AST. It uses the Visitor pattern to allow different operations
 * to be performed on expressions without modifying the expression classes.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
class Expression {
public:
    /** @brief Virtual destructor */
    virtual ~Expression() = default;
    /** @brief Accept a visitor for processing this expression */
    virtual std::any accept(ExpressionVisitor& visitor) = 0;
};

/**
 * @brief Binary operation expression
 * 
 * Represents binary operations such as arithmetic (+, -, *, /), comparison
 * (=, <>, <, >, <=, >=), logical (and, or), and other binary operators.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct Binary : Expression, public std::enable_shared_from_this<Binary> {
    /** @brief Left operand expression */
    std::shared_ptr<Expression> left;
    /** @brief Binary operator token */
    core::Token op;
    /** @brief Right operand expression */
    std::shared_ptr<Expression> right;

    /**
     * @brief Constructor for binary expression
     * @param left Left operand expression
     * @param op Binary operator token
     * @param right Right operand expression
     */
    Binary(std::shared_ptr<Expression> left, core::Token op, std::shared_ptr<Expression> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    /** @brief Accept visitor for processing this binary expression */
    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

/**
 * @brief Unary operation expression
 * 
 * Represents unary operations such as negation (-), logical not (not),
 * and other unary operators that operate on a single operand.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct Unary : Expression, public std::enable_shared_from_this<Unary> {
    /** @brief Unary operator token */
    core::Token op;
    /** @brief Operand expression */
    std::shared_ptr<Expression> right;

    /**
     * @brief Constructor for unary expression
     * @param op Unary operator token
     * @param right Operand expression
     */
    Unary(core::Token op, std::shared_ptr<Expression> right)
        : op(std::move(op)), right(std::move(right)) {}

    /** @brief Accept visitor for processing this unary expression */
    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

/**
 * @brief Literal value expression
 * 
 * Represents literal values such as numbers, strings, booleans,
 * and other constant values in the source code.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct Literal : Expression, public std::enable_shared_from_this<Literal> {
    /** @brief The literal value */
    std::any value;

    /**
     * @brief Constructor for literal expression
     * @param value The literal value
     */
    explicit Literal(std::any value) : value(std::move(value)) {}

    /** @brief Accept visitor for processing this literal expression */
    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

/**
 * @brief Variable reference expression
 * 
 * Represents a reference to a variable, constant, or identifier
 * in the source code.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct Variable : Expression, public std::enable_shared_from_this<Variable> {
    /** @brief Variable name token */
    core::Token name;

    /**
     * @brief Constructor for variable expression
     * @param name Variable name token
     */
    explicit Variable(core::Token name) : name(std::move(name)) {}

    /** @brief Accept visitor for processing this variable expression */
    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

/**
 * @brief Parenthesized expression
 * 
 * Represents an expression enclosed in parentheses for grouping
 * and controlling operator precedence.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct Grouping : Expression, public std::enable_shared_from_this<Grouping> {
    /** @brief The grouped expression */
    std::shared_ptr<Expression> expression;

    /**
     * @brief Constructor for grouping expression
     * @param expression The expression to be grouped
     */
    explicit Grouping(std::shared_ptr<Expression> expression) : expression(std::move(expression)) {}

    /** @brief Accept visitor for processing this grouping expression */
    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

/**
 * @brief Assignment expression
 * 
 * Represents an assignment operation where a value is assigned to a target
 * (variable, field, or array element). Uses the <- operator in NOTAL.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct Assign : Expression, public std::enable_shared_from_this<Assign> {
    /** @brief The left-hand side target of the assignment */
    std::shared_ptr<Expression> target;
    /** @brief The value to be assigned */
    std::shared_ptr<Expression> value;

    /**
     * @brief Constructor for assignment expression
     * @param target The assignment target
     * @param value The value to assign
     */
    Assign(std::shared_ptr<Expression> target, std::shared_ptr<Expression> value)
        : target(std::move(target)), value(std::move(value)) {}

    /** @brief Accept visitor for processing this assignment expression */
    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

/**
 * @brief Function/procedure call expression
 * 
 * Represents a call to a function or procedure with optional arguments.
 * The callee can be a simple identifier or a more complex expression.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct Call : Expression, public std::enable_shared_from_this<Call> {
    /** @brief The expression that evaluates to the callable */
    std::shared_ptr<Expression> callee;
    /** @brief The closing parenthesis token (for error reporting) */
    core::Token paren;
    /** @brief List of argument expressions */
    std::vector<std::shared_ptr<Expression>> arguments;

    /**
     * @brief Constructor for call expression
     * @param callee The callable expression
     * @param paren The closing parenthesis token
     * @param arguments List of argument expressions
     */
    Call(std::shared_ptr<Expression> callee, core::Token paren, std::vector<std::shared_ptr<Expression>> arguments)
        : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}

    /** @brief Accept visitor for processing this call expression */
    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

/**
 * @brief Record field access expression
 * 
 * Represents accessing a field of a record/struct using dot notation.
 * For example: student.name or person.address.street.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct FieldAccess : Expression, public std::enable_shared_from_this<FieldAccess> {
    /** @brief The object expression whose field is being accessed */
    std::shared_ptr<Expression> object;
    /** @brief The field name token */
    core::Token name;

    /**
     * @brief Constructor for field access expression
     * @param object The object expression
     * @param name The field name token
     */
    FieldAccess(std::shared_ptr<Expression> object, core::Token name)
        : object(std::move(object)), name(std::move(name)) {}

    /** @brief Accept visitor for processing this field access expression */
    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

/**
 * @brief Record field assignment expression
 * 
 * Represents assigning a value to a field of a record/struct.
 * For example: student.name <- "John" or person.age <- 25.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct FieldAssign : Expression, public std::enable_shared_from_this<FieldAssign> {
    /** @brief The field access target for assignment */
    std::shared_ptr<FieldAccess> target;
    /** @brief The value to be assigned to the field */
    std::shared_ptr<Expression> value;

    /**
     * @brief Constructor for field assignment expression
     * @param target The field access target
     * @param value The value to assign
     */
    FieldAssign(std::shared_ptr<FieldAccess> target, std::shared_ptr<Expression> value)
        : target(std::move(target)), value(std::move(value)) {}

    /** @brief Accept visitor for processing this field assignment expression */
    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

/**
 * @brief Array element access expression
 * 
 * Represents accessing elements of an array using bracket notation.
 * Supports multi-dimensional arrays with multiple indices.
 * For example: arr[i] or matrix[i][j].
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
struct ArrayAccess : Expression, public std::enable_shared_from_this<ArrayAccess> {
    /** @brief The expression that evaluates to the array */
    std::shared_ptr<Expression> callee;
    /** @brief The closing ']' token (for error reporting) */
    core::Token bracket;
    /** @brief List of index expressions for multi-dimensional access */
    std::vector<std::shared_ptr<Expression>> indices;

    /**
     * @brief Constructor for array access expression
     * @param callee The array expression
     * @param bracket The closing bracket token
     * @param indices List of index expressions
     */
    ArrayAccess(std::shared_ptr<Expression> callee, core::Token bracket, std::vector<std::shared_ptr<Expression>> indices)
        : callee(std::move(callee)), bracket(std::move(bracket)), indices(std::move(indices)) {}

    /** @brief Accept visitor for processing this array access expression */
    std::any accept(ExpressionVisitor& visitor) override {
        return visitor.visit(shared_from_this());
    }
};

} // namespace gate::ast

#endif // GATE_AST_EXPRESSION_H
