#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>
#include <memory> // For std::unique_ptr
#include "lexer.hpp" // Assuming Token is defined here
#include "symbol_table.hpp" // Added for SymbolTable integration

// Forward declaration of the visitor
class ASTVisitor;

// Forward declaration for ProgramNode, used in ParseResult
class ProgramNode;

// ParameterMode is now defined in symbol_table.hpp and included.
// enum class ParameterMode {
//     IN,    // Default, or explicitly 'input'
//     OUT,   // Explicitly 'output'
//     IN_OUT // Explicitly 'input/output'
// };

// Structure to hold the results of parsing
struct ParseResult {
    std::unique_ptr<ProgramNode> ast_root;
    std::unique_ptr<SymbolTable> symbol_table;
};

// --- Base AST Node ---
class ASTNode {
public:
    int line;
    int col;

    ASTNode(int l, int c) : line(l), col(c) {}
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor* visitor) = 0;
};

// --- Intermediate Base Nodes (Optional but good practice) ---
class ExpressionNode : public ASTNode {
public:
    using ASTNode::ASTNode; // Inherit constructor
    virtual void accept(ASTVisitor* visitor) = 0; // Still pure for direct ExpressionNode
};

class StatementNode : public ASTNode {
public:
    using ASTNode::ASTNode; // Inherit constructor
    virtual void accept(ASTVisitor* visitor) = 0; // Still pure for direct StatementNode
};

class DeclarationNode : public ASTNode {
public:
    using ASTNode::ASTNode; // Inherit constructor
    virtual void accept(ASTVisitor* visitor) = 0; // Still pure for direct DeclarationNode
};


// --- Concrete Expression Nodes ---
class IdentifierNode : public ExpressionNode {
public:
    std::string name;

    IdentifierNode(int l, int c, std::string n) : ExpressionNode(l, c), name(std::move(n)) {}
    void accept(ASTVisitor* visitor) override;
};

class IntegerLiteralNode : public ExpressionNode {
public:
    long long value; // Changed to long long for wider range, common in compilers

    IntegerLiteralNode(int l, int c, long long v) : ExpressionNode(l, c), value(v) {}
    void accept(ASTVisitor* visitor) override;
};

class StringLiteralNode : public ExpressionNode {
public:
    std::string value;

    StringLiteralNode(int l, int c, std::string v) : ExpressionNode(l, c), value(std::move(v)) {}
    void accept(ASTVisitor* visitor) override;
};

class BinaryOpNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> left;
    std::string op;
    std::unique_ptr<ExpressionNode> right;

    BinaryOpNode(int l, int c, std::unique_ptr<ExpressionNode> lhs, std::string oper, std::unique_ptr<ExpressionNode> rhs)
        : ExpressionNode(l, c), left(std::move(lhs)), op(std::move(oper)), right(std::move(rhs)) {}
    void accept(ASTVisitor* visitor) override;
};

class UnaryOpNode : public ExpressionNode {
public:
    std::string op; // e.g., "NOT", "-" (for unary minus)
    std::unique_ptr<ExpressionNode> operand;

    UnaryOpNode(int l, int c, std::string oper, std::unique_ptr<ExpressionNode> oper_node)
        : ExpressionNode(l, c), op(std::move(oper)), operand(std::move(oper_node)) {}
    void accept(ASTVisitor* visitor) override;
};

class ArrayAccessNode : public ExpressionNode {
public:
    std::unique_ptr<IdentifierNode> array_identifier; // The array variable itself
    std::unique_ptr<ExpressionNode> index_expression; // The expression within [ ... ]

    ArrayAccessNode(int l, int c, std::unique_ptr<IdentifierNode> ident, std::unique_ptr<ExpressionNode> index)
        : ExpressionNode(l, c), array_identifier(std::move(ident)), index_expression(std::move(index)) {}
    void accept(ASTVisitor* visitor) override;
};

class FunctionCallNode : public ExpressionNode {
public:
    std::unique_ptr<IdentifierNode> function_name;
    std::vector<std::unique_ptr<ExpressionNode>> arguments;

    FunctionCallNode(int l, int c, std::unique_ptr<IdentifierNode> name, std::vector<std::unique_ptr<ExpressionNode>> args)
        : ExpressionNode(l, c), function_name(std::move(name)), arguments(std::move(args)) {}
    void accept(ASTVisitor* visitor) override;
};

// New AST Nodes for Pointers and Memory Operations
class ReferenceNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> target_expr;
    ReferenceNode(int l, int c, std::unique_ptr<ExpressionNode> expr)
        : ExpressionNode(l, c), target_expr(std::move(expr)) {}
    void accept(ASTVisitor* visitor) override;
};

class DereferenceNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> pointer_expr;
    DereferenceNode(int l, int c, std::unique_ptr<ExpressionNode> expr)
        : ExpressionNode(l, c), pointer_expr(std::move(expr)) {}
    void accept(ASTVisitor* visitor) override;
};

class AllocateNode : public ExpressionNode { // Assuming allocate(size_expr)
public:
    std::unique_ptr<ExpressionNode> size_expr; // Could also be a type node if allocate(TYPE)
    AllocateNode(int l, int c, std::unique_ptr<ExpressionNode> expr)
        : ExpressionNode(l, c), size_expr(std::move(expr)) {}
    void accept(ASTVisitor* visitor) override;
};

class ReallocateNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> pointer_expr;
    std::unique_ptr<ExpressionNode> new_size_expr;
    ReallocateNode(int l, int c, std::unique_ptr<ExpressionNode> ptr_expr, std::unique_ptr<ExpressionNode> ns_expr)
        : ExpressionNode(l, c), pointer_expr(std::move(ptr_expr)), new_size_expr(std::move(ns_expr)) {}
    void accept(ASTVisitor* visitor) override;
};

class DeallocateNode : public ExpressionNode { // Or StatementNode if it doesn't return a value
public:
    std::unique_ptr<ExpressionNode> pointer_expr;
    DeallocateNode(int l, int c, std::unique_ptr<ExpressionNode> expr)
        : ExpressionNode(l, c), pointer_expr(std::move(expr)) {}
    void accept(ASTVisitor* visitor) override;
};

class NullLiteralNode : public ExpressionNode {
public:
    NullLiteralNode(int l, int c) : ExpressionNode(l, c) {}
    void accept(ASTVisitor* visitor) override;
};

class PointerMemberAccessNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> pointer_expr;
    std::unique_ptr<IdentifierNode> member_name;
    PointerMemberAccessNode(int l, int c, std::unique_ptr<ExpressionNode> ptr_expr, std::unique_ptr<IdentifierNode> member)
        : ExpressionNode(l, c), pointer_expr(std::move(ptr_expr)), member_name(std::move(member)) {}
    void accept(ASTVisitor* visitor) override;
};

// --- New Declaration Nodes ---
class EnumTypeNode : public DeclarationNode {
public:
    std::string name;
    std::vector<std::string> values;
    EnumTypeNode(int l, int c, std::string enum_name, std::vector<std::string> enum_values)
        : DeclarationNode(l, c), name(std::move(enum_name)), values(std::move(enum_values)) {}
    void accept(ASTVisitor* visitor) override;
};

class ConstantDeclarationNode : public DeclarationNode {
public:
    std::string name;
    std::unique_ptr<ExpressionNode> value; // Should be a literal expression
    ConstantDeclarationNode(int l, int c, std::string const_name, std::unique_ptr<ExpressionNode> val_expr)
        : DeclarationNode(l, c), name(std::move(const_name)), value(std::move(val_expr)) {}
    void accept(ASTVisitor* visitor) override;
};


// --- Concrete Statement Nodes ---
class AssignmentNode : public StatementNode {
public:
    std::unique_ptr<IdentifierNode> target; // LHS is typically an identifier
    std::unique_ptr<ExpressionNode> value;  // RHS is an expression

    AssignmentNode(int l, int c, std::unique_ptr<IdentifierNode> tgt, std::unique_ptr<ExpressionNode> val)
        : StatementNode(l, c), target(std::move(tgt)), value(std::move(val)) {}
    void accept(ASTVisitor* visitor) override;
};

class BlockNode : public StatementNode {
public:
    std::vector<std::unique_ptr<StatementNode>> statements;

    BlockNode(int l, int c) : StatementNode(l, c) {}
    void accept(ASTVisitor* visitor) override;
};

class IfNode : public StatementNode {
public:
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<BlockNode> then_block;
    std::unique_ptr<BlockNode> else_block; // Can be nullptr if no else

    IfNode(int l, int c, std::unique_ptr<ExpressionNode> cond, std::unique_ptr<BlockNode> then_b, std::unique_ptr<BlockNode> else_b = nullptr)
        : StatementNode(l, c), condition(std::move(cond)), then_block(std::move(then_b)), else_block(std::move(else_b)) {}
    void accept(ASTVisitor* visitor) override;
};

class OutputNode : public StatementNode { // For 'CETAK' or 'PRINT'
public:
    std::vector<std::unique_ptr<ExpressionNode>> expressions;
    bool omit_newline = false; // Added for INLINE keyword

    OutputNode(int l, int c, bool omit_nl = false) : StatementNode(l, c), omit_newline(omit_nl) {}
    void accept(ASTVisitor* visitor) override;
};

class WhileNode : public StatementNode {
public:
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<BlockNode> body;

    WhileNode(int l, int c, std::unique_ptr<ExpressionNode> cond, std::unique_ptr<BlockNode> b)
        : StatementNode(l, c), condition(std::move(cond)), body(std::move(b)) {}
    void accept(ASTVisitor* visitor) override;
};

class RepeatUntilNode : public StatementNode {
public:
    std::unique_ptr<BlockNode> body;
    std::unique_ptr<ExpressionNode> condition; // Condition is checked at the end

    RepeatUntilNode(int l, int c, std::unique_ptr<BlockNode> b, std::unique_ptr<ExpressionNode> cond)
        : StatementNode(l, c), body(std::move(b)), condition(std::move(cond)) {}
    void accept(ASTVisitor* visitor) override;
};

class ForNode : public StatementNode {
public:
    std::unique_ptr<IdentifierNode> loop_variable; // e.g., 'i'
    std::unique_ptr<ExpressionNode> start_value;
    std::unique_ptr<ExpressionNode> end_value;
    std::unique_ptr<BlockNode> body;
    // bool is_decrementing; // Optional: If 'downto' is supported

    ForNode(int l, int c, std::unique_ptr<IdentifierNode> var,
            std::unique_ptr<ExpressionNode> start, std::unique_ptr<ExpressionNode> end,
            std::unique_ptr<BlockNode> b)
        : StatementNode(l, c), loop_variable(std::move(var)), start_value(std::move(start)),
          end_value(std::move(end)), body(std::move(b)) {}
    void accept(ASTVisitor* visitor) override;
};

// CaseBranchNode: Helper for DependOnNode (switch-case structure)
class CaseBranchNode : public ASTNode { // Not a StatementNode itself, but part of DependOnNode logic
public:
    std::unique_ptr<ExpressionNode> value; // Null for 'otherwise' case (default case)
    std::unique_ptr<BlockNode> body;       // Block of statements for this case

    // Constructor for 'case <value>: ...'
    CaseBranchNode(int l, int c, std::unique_ptr<ExpressionNode> val, std::unique_ptr<BlockNode> b)
        : ASTNode(l, c), value(std::move(val)), body(std::move(b)) {}

    // Constructor for 'otherwise: ...' (value is nullptr)
    CaseBranchNode(int l, int c, std::unique_ptr<BlockNode> b)
        : ASTNode(l, c), value(nullptr), body(std::move(b)) {}

    bool is_otherwise() const { return value == nullptr; }
    void accept(ASTVisitor* visitor) override;
};

// DependOnNode: Represents a switch-case like structure 'depend on (...) case ... otherwise ... enddependon'
class DependOnNode : public StatementNode {
public:
    std::unique_ptr<ExpressionNode> control_variable;
    std::vector<std::unique_ptr<CaseBranchNode>> cases;
    // 'otherwise' is handled as a CaseBranchNode with value == nullptr within the 'cases' vector.

    DependOnNode(int l, int c, std::unique_ptr<ExpressionNode> ctrl_var)
        : StatementNode(l, c), control_variable(std::move(ctrl_var)) {}
    void accept(ASTVisitor* visitor) override;
};

class InputNode : public StatementNode {
public:
    // Stores the identifiers of variables to be read.
    std::vector<std::unique_ptr<IdentifierNode>> variables;

    InputNode(int l, int c) : StatementNode(l, c) {}
    void accept(ASTVisitor* visitor) override;
};


// --- Concrete Declaration Nodes ---
class VariableDeclarationNode : public DeclarationNode {
public:
    // For simplicity, one variable per declaration. Can be extended to a list of names for a single type.
    std::string var_name;
    std::string var_type; // Type as string for now, could be a TypeNode later

    VariableDeclarationNode(int l, int c, std::string name, std::string type)
        : DeclarationNode(l, c), var_name(std::move(name)), var_type(std::move(type)) {}
    void accept(ASTVisitor* visitor) override;
};

// Parameter for a function
class FunctionParameterNode : public ASTNode { // Or inherit DeclarationNode
public:
    std::string param_name;
    std::string param_type;
    ParameterMode mode;
    // bool is_reference; // Or some other way to denote pass-by-value/reference if needed

    FunctionParameterNode(int l, int c, std::string name, std::string type, ParameterMode p_mode = ParameterMode::IN)
        : ASTNode(l, c), param_name(std::move(name)), param_type(std::move(type)), mode(p_mode) {}
    void accept(ASTVisitor* visitor) override;
};

class FunctionPrototypeNode : public DeclarationNode {
public:
    std::string func_name;
    std::string return_type; // Type as string, or void if procedure
    std::vector<std::unique_ptr<FunctionParameterNode>> parameters;

    FunctionPrototypeNode(int l, int c, std::string name, std::string ret_type)
        : DeclarationNode(l, c), func_name(std::move(name)), return_type(std::move(ret_type)) {}
    void accept(ASTVisitor* visitor) override;
};

class SubprogramBodyNode : public ASTNode { // Not strictly a declaration, but groups prototype and body
public:
    std::unique_ptr<FunctionPrototypeNode> prototype;
    std::vector<std::unique_ptr<VariableDeclarationNode>> local_declarations; // KAMUS LOKAL
    std::unique_ptr<BlockNode> body;

    SubprogramBodyNode(int l, int c, std::unique_ptr<FunctionPrototypeNode> proto, std::unique_ptr<BlockNode> b)
        : ASTNode(l, c), prototype(std::move(proto)), body(std::move(b)) {}
    void accept(ASTVisitor* visitor) override;
};

// --- Root AST Node ---
class ProgramNode : public ASTNode {
public:
    std::string program_name;
    std::vector<std::unique_ptr<DeclarationNode>> global_declarations; // Global KAMUS
    std::unique_ptr<BlockNode> main_algoritma; // Main ALGORITMA block
    std::vector<std::unique_ptr<SubprogramBodyNode>> subprograms; // Functions/Procedures

    ProgramNode(int l, int c, std::string name) : ASTNode(l, c), program_name(std::move(name)) {}
    void accept(ASTVisitor* visitor) override;
};


// --- Parser Class ---
class Parser {
private:
    std::vector<Token> tokens;
    size_t current_token_idx; // Renamed from 'pos' for clarity
    Token current_token_cache; // Cache for current_token()

    // Helper methods to manage tokens
    void advance();
    bool match(TokenType type); // Check current token type and advance if match
    bool check(TokenType type); // Check current token type without advancing
    Token consume(TokenType type, const std::string& error_message); // Consume specific token or error
    Token previous_token(); // Get the token before the current one

    // Private parsing methods would return std::unique_ptr to new AST types
    // Example placeholder signatures:
    std::unique_ptr<ExpressionNode> parseExpression();
    std::unique_ptr<StatementNode> parseStatement();
    std::unique_ptr<BlockNode> parseBlock();
    std::unique_ptr<IdentifierNode> parseIdentifier();
    std::unique_ptr<VariableDeclarationNode> parseVariableDeclaration();
    std::unique_ptr<FunctionParameterNode> parseFunctionParameter();
    std::unique_ptr<FunctionPrototypeNode> parseFunctionPrototype();
    std::unique_ptr<SubprogramBodyNode> parseSubprogramBody();
    std::unique_ptr<ConstantDeclarationNode> parseConstantDeclaration(); // New
    std::unique_ptr<DeclarationNode> parseTypeDefinition();            // New (can return EnumTypeNode or RecordTypeNode)
    std::unique_ptr<EnumTypeNode> parseEnumTypeDefinitionBody(Token type_name_token); // New Helper
    // ... other parsing helpers for different node types
    // Helper function to get operator precedence - declaration
    int getOperatorPrecedence(TokenType type);
    // Helper function to get operator associativity - declaration
    enum class Associativity; // Forward declare enum for return type
    Associativity getOperatorAssociativity(TokenType type);

    std::unique_ptr<ExpressionNode> parseExpression(int min_precedence = 1); // Updated signature
    std::unique_ptr<ExpressionNode> parseUnaryExpression();


    // Symbol table to be populated during parsing
    std::unique_ptr<SymbolTable> symbol_table_ptr;

public:
    Parser() : current_token_idx(0) {} // Initialize if needed

    // Main parsing method
    ParseResult parse(const std::vector<Token>& input_tokens); // Updated return type
};

#endif // PARSER_HPP