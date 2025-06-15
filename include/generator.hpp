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
class FunctionCallNode;
class ReferenceNode;
class DereferenceNode;
class AllocateNode;
class ReallocateNode;
class DeallocateNode;
class NullLiteralNode; // Added
class EnumTypeNode;
class ConstantDeclarationNode;
class PointerMemberAccessNode;
class RecordTypeNode; // Added
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
    virtual void visit(DependOnNode* node) = 0;
    virtual void visit(CaseBranchNode* node) = 0;
    virtual void visit(UnaryOpNode* node) = 0;
    virtual void visit(InputNode* node) = 0;
    virtual void visit(ArrayAccessNode* node) = 0;
    virtual void visit(FunctionCallNode* node) = 0;
    virtual void visit(ReferenceNode* node) = 0;
    virtual void visit(DereferenceNode* node) = 0;
    virtual void visit(AllocateNode* node) = 0;
    virtual void visit(ReallocateNode* node) = 0;
    virtual void visit(DeallocateNode* node) = 0;
    virtual void visit(NullLiteralNode* node) = 0; // Added
    virtual void visit(EnumTypeNode* node) = 0;
    virtual void visit(ConstantDeclarationNode* node) = 0;
    virtual void visit(PointerMemberAccessNode* node) = 0;
    virtual void visit(RecordTypeNode* node) = 0; // Added
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
    void visit(DependOnNode* node) override;
    void visit(CaseBranchNode* node) override;
    void visit(UnaryOpNode* node) override;
    void visit(InputNode* node) override;
    void visit(ArrayAccessNode* node) override;
    void visit(FunctionCallNode* node) override;
    void visit(ReferenceNode* node) override;
    void visit(DereferenceNode* node) override;
    void visit(AllocateNode* node) override;
    void visit(ReallocateNode* node) override;
    void visit(DeallocateNode* node) override;
    void visit(NullLiteralNode* node) override; // Added
    void visit(EnumTypeNode* node) override;
    void visit(ConstantDeclarationNode* node) override;
    void visit(PointerMemberAccessNode* node) override;
    void visit(RecordTypeNode* node) override; // Added
    // Implement visit for any other concrete ASTNode types
};

#endif // GENERATOR_HPP