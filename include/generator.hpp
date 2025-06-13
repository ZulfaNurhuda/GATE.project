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
class DependOnNode; // Added Forward Declaration
class CaseBranchNode; // Added Forward Declaration
// ExpressionNode, StatementNode, DeclarationNode are abstract, so no visit for them directly.

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;

    virtual void visit(ProgramNode* node) = 0;
    virtual void visit(IdentifierNode* node) = 0;
    virtual void visit(IntegerLiteralNode* node) = 0;
    virtual void visit(StringLiteralNode* node) = 0;
    virtual void visit(BinaryOpNode* node) = 0;
    virtual void visit(AssignmentNode* node) = 0;
    virtual void visit(BlockNode* node) = 0;
    virtual void visit(IfNode* node) = 0;
    virtual void visit(OutputNode* node) = 0;
    virtual void visit(VariableDeclarationNode* node) = 0;
    virtual void visit(FunctionParameterNode* node) = 0;
    virtual void visit(FunctionPrototypeNode* node) = 0;
    virtual void visit(SubprogramBodyNode* node) = 0;
    virtual void visit(WhileNode* node) = 0;
    virtual void visit(RepeatUntilNode* node) = 0;
    virtual void visit(ForNode* node) = 0;
    virtual void visit(DependOnNode* node) = 0;      // Added visit method
    virtual void visit(CaseBranchNode* node) = 0;    // Added visit method
    // Add any other concrete ASTNode types here
};

// CodeGenerator Class implementing ASTVisitor
class CodeGenerator : public ASTVisitor {
private:
    std::ostream* out_stream_ptr; // Pointer to the output stream
    SymbolTable symbol_table;     // Symbol table for type checking, etc. (owned for now)
    int current_indent_level;     // For pretty printing C code

    // Private helper methods
    void indent_spaces(); // Renamed to avoid conflict with CodeGenerator::indent member if any
    void write(const std::string& str);
    void writeln(const std::string& str = "");
    std::string map_an_type_to_c(const std::string& an_type); // Maps AN internal types to C types

public:
    CodeGenerator();

    // Main method to initiate code generation
    void generate_c_code(const ProgramNode* program_root, std::ostream& output_stream_ref);

    // Overridden visit methods from ASTVisitor
    void visit(ProgramNode* node) override;
    void visit(IdentifierNode* node) override;
    void visit(IntegerLiteralNode* node) override;
    void visit(StringLiteralNode* node) override;
    void visit(BinaryOpNode* node) override;
    void visit(AssignmentNode* node) override;
    void visit(BlockNode* node) override;
    void visit(IfNode* node) override;
    void visit(OutputNode* node) override;
    void visit(VariableDeclarationNode* node) override;
    void visit(FunctionParameterNode* node) override;
    void visit(FunctionPrototypeNode* node) override;
    void visit(SubprogramBodyNode* node) override;
    void visit(WhileNode* node) override;
    void visit(RepeatUntilNode* node) override;
    void visit(ForNode* node) override;
    void visit(DependOnNode* node) override;      // Added override
    void visit(CaseBranchNode* node) override;    // Added override
    // Implement visit for any other concrete ASTNode types
};

#endif // GENERATOR_HPP