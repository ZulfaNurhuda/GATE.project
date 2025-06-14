#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <iostream> // For std::ostream
#include <string>
#include <vector>
#include <memory>   // For std::unique_ptr if needed in some visitor logic (though not directly here)
#include "../include/parser.hpp"       // For ASTNode definitions
#include "../include/symbol_table.hpp" // For SymbolTable

// ASTVisitor Interface
// Forward declare all concrete AST node types that the visitor will handle.
// These should match the concrete classes derived from ASTNode in parser.hpp.
class ProgramNode;
class IdentifierNode;
class IntegerLiteralNode;
class StringLiteralNode;
class BinaryOpNode;
class AssignmentNode;
class BlockNode;
class IfNode;
class OutputNode;
class VariableDeclarationNode;
class FunctionParameterNode;
class FunctionPrototypeNode;
class SubprogramBodyNode;
class WhileNode;
class RepeatUntilNode;
class ForNode;
class DependOnNode;
class CaseBranchNode;
class UnaryOpNode;
class InputNode;
class ArrayAccessNode;
class FunctionCallNode; // Added Forward Declaration
// ExpressionNode, StatementNode, DeclarationNode are abstract, so no visit for them directly.

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;

    virtual void visit(const ProgramNode* node) = 0;
    virtual void visit(const IdentifierNode* node) = 0;
    virtual void visit(const IntegerLiteralNode* node) = 0;
    virtual void visit(const StringLiteralNode* node) = 0;
    virtual void visit(const BinaryOpNode* node) = 0;
    virtual void visit(const AssignmentNode* node) = 0;
    virtual void visit(const BlockNode* node) = 0;
    virtual void visit(const IfNode* node) = 0;
    virtual void visit(const OutputNode* node) = 0;
    virtual void visit(const VariableDeclarationNode* node) = 0;
    virtual void visit(const FunctionParameterNode* node) = 0;
    virtual void visit(const FunctionPrototypeNode* node) = 0;
    virtual void visit(const SubprogramBodyNode* node) = 0;
    virtual void visit(const WhileNode* node) = 0;
    virtual void visit(const RepeatUntilNode* node) = 0;
    virtual void visit(const ForNode* node) = 0;
    virtual void visit(const DependOnNode* node) = 0;
    virtual void visit(const CaseBranchNode* node) = 0;
    virtual void visit(const UnaryOpNode* node) = 0;
    virtual void visit(const InputNode* node) = 0;
    virtual void visit(const ArrayAccessNode* node) = 0;
    virtual void visit(const FunctionCallNode* node) = 0;  // Added visit method
    // Add any other concrete ASTNode types here
};

// CodeGenerator Class implementing ASTVisitor
class CodeGenerator : public ASTVisitor {
private:
    std::ostream* out_stream_ptr; // Pointer to the output stream
    const SymbolTable* symbol_table_ptr; // Pointer to the externally populated symbol table
    int current_indent_level;     // For pretty printing C code

    // Private helper methods
    void indent_spaces(); // Renamed to avoid conflict with CodeGenerator::indent member if any
    void write(const std::string& str);
    void writeln(const std::string& str = "");
    std::string map_an_type_to_c(const std::string& an_type); // Maps AN internal types to C types

public:
    CodeGenerator(); // Constructor updated to initialize symbol_table_ptr to nullptr

    // Main method to initiate code generation
    void generate_c_code(const ProgramNode* program_root, const SymbolTable& populated_symbol_table, std::ostream& output_stream_ref);

    // Overridden visit methods from ASTVisitor
    void visit(const ProgramNode* node) override;
    void visit(const IdentifierNode* node) override;
    void visit(const IntegerLiteralNode* node) override;
    void visit(const StringLiteralNode* node) override;
    void visit(const BinaryOpNode* node) override;
    void visit(const AssignmentNode* node) override;
    void visit(const BlockNode* node) override;
    void visit(const IfNode* node) override;
    void visit(const OutputNode* node) override;
    void visit(const VariableDeclarationNode* node) override;
    void visit(const FunctionParameterNode* node) override;
    void visit(const FunctionPrototypeNode* node) override;
    void visit(const SubprogramBodyNode* node) override;
    void visit(const WhileNode* node) override;
    void visit(const RepeatUntilNode* node) override;
    void visit(const ForNode* node) override;
    void visit(const DependOnNode* node) override;
    void visit(const CaseBranchNode* node) override;
    void visit(const UnaryOpNode* node) override;
    void visit(const InputNode* node) override;
    void visit(const ArrayAccessNode* node) override;
    void visit(const FunctionCallNode* node) override;  // Added override
    // Implement visit for any other concrete ASTNode types
};

#endif // GENERATOR_HPP