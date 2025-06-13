#include "../include/generator.hpp"
#include "../include/parser.hpp"      // For AST node concrete type details
#include "../include/symbol_table.hpp" // Potentially for type lookups
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm> // For std::transform or other utilities if needed
#include <string>    // For std::to_string

// --- CodeGenerator Constructor ---
CodeGenerator::CodeGenerator() : out_stream_ptr(nullptr), current_indent_level(0) {}

// --- Main Generation Method ---
void CodeGenerator::generate_c_code(const ProgramNode* program_root, std::ostream& output_stream_ref) {
    if (!program_root) {
        // Or handle error appropriately
        std::cerr << "Error: Program root is null." << std::endl;
        return;
    }
    this->out_stream_ptr = &output_stream_ref;
    this->current_indent_level = 0;
    this->symbol_table = SymbolTable(); // Re-initialize symbol table for fresh generation

    program_root->accept(this);
}

// --- Helper Methods ---
void CodeGenerator::indent_spaces() {
    if (out_stream_ptr) {
        for (int i = 0; i < current_indent_level * 4; ++i) {
            *out_stream_ptr << " ";
        }
    }
}

void CodeGenerator::write(const std::string& str) {
    if (out_stream_ptr) {
        *out_stream_ptr << str;
    }
}

void CodeGenerator::writeln(const std::string& str) {
    if (out_stream_ptr) {
        *out_stream_ptr << str << "\n";
    }
}

std::string CodeGenerator::map_an_type_to_c(const std::string& an_type) {
    std::string lower_type = an_type;
    std::transform(lower_type.begin(), lower_type.end(), lower_type.begin(), ::tolower);

    if (lower_type == "integer") return "int";
    if (lower_type == "real") return "double"; // Assuming 'real' maps to 'double'
    if (lower_type == "boolean") return "bool";
    if (lower_type == "string") return "char*"; // Simplified C-style string
    if (lower_type == "character") return "char";
    if (lower_type == "void") return "void"; // For procedure return types

    // Default: return original type (could be a custom type name)
    // In a more complex system, this might involve checking if an_type is a known struct/typedef
    return an_type;
}

// --- Visit Method Implementations ---

void CodeGenerator::visit(ProgramNode* node) {
    writeln("#include <stdio.h>");
    writeln("#include <stdbool.h>"); // For bool type
    writeln("#include <string.h>");  // For string operations if needed (e.g. strcpy for char*)
    writeln("#include <stdlib.h>");  // For malloc/free if using dynamic char* for strings
    // writeln("#include <math.h>"); // If ^ or other math ops are used

    // Forward declarations for functions (if any are in global_declarations or subprograms)
    // This is a simplified approach. A more robust way is two passes or hoisting.
    for (const auto& decl : node->global_declarations) {
        if (dynamic_cast<FunctionPrototypeNode*>(decl.get())) { // Check if it's a function prototype
            decl->accept(this); // This should generate the prototype
        }
    }
     for (const auto& subprogram : node->subprograms) {
        subprogram->prototype->accept(this); // Generate prototype
    }
    writeln("");


    writeln("int main() {");
    current_indent_level++;

    // Process global_declarations that are variables (KAMUS section for main)
    // This assumes global_declarations might mix var decls and func prototypes.
    // A clearer AST structure might separate these.
    bool has_main_kamus = false;
    for (const auto& decl : node->global_declarations) {
        if (dynamic_cast<VariableDeclarationNode*>(decl.get())) {
            has_main_kamus = true;
            decl->accept(this);
        }
    }
    if(has_main_kamus) writeln(""); // Add a newline after declarations if any

    // Process main ALGORITMA block
    if (node->main_algoritma) {
        node->main_algoritma->accept(this);
    }

    indent_spaces();
    writeln("return 0;");
    current_indent_level--;
    writeln("}");
    writeln("");

    // Process Subprogram implementations
    for (const auto& subprogram : node->subprograms) {
        subprogram->accept(this);
        writeln(""); // Add a newline between subprograms
    }
}

void CodeGenerator::visit(BlockNode* node) {
    // For blocks not directly part of main or a function's immediate body,
    // C requires them to be part of a statement (e.g. if, else, while blocks).
    // The {} are typically handled by the visit methods for those statements.
    // This visit(BlockNode*) is for sequences of statements.
    for (const auto& stmt : node->statements) {
        stmt->accept(this);
    }
}

void CodeGenerator::visit(VariableDeclarationNode* node) {
    indent_spaces();
    // Assuming var_type in Node is the AN type.
    write(map_an_type_to_c(node->var_type) + " " + node->var_name);
    // Basic initialization for pointers to avoid uninitialized reads if language implies default init.
    // This is a simplification; C doesn't auto-init local vars.
    // if (map_an_type_to_c(node->var_type) == "char*") {
    //     write(" = NULL");
    // }
    writeln(";");
}

void CodeGenerator::visit(AssignmentNode* node) {
    indent_spaces();
    node->target->accept(this); // Should produce the variable name
    write(" = ");
    node->value->accept(this);  // Should produce the expression
    writeln(";");
}

void CodeGenerator::visit(IdentifierNode* node) {
    write(node->name);
}

void CodeGenerator::visit(IntegerLiteralNode* node) {
    write(std::to_string(node->value));
}

void CodeGenerator::visit(StringLiteralNode* node) {
    // Basic C string literal. Need to handle escapes within node->value if complex.
    write("\"" + node->value + "\"");
}

void CodeGenerator::visit(BinaryOpNode* node) {
    // Basic C string literal. Need to handle escapes within node->value if complex.
    // Simple binary operation: (left op right). Parentheses for safety for now.
    write("(");
    node->left->accept(this);
    write(" " + node->op + " "); // Assuming op is already C-compatible (+, -, *, /, ==, !=, <, >, <=, >=, &&, ||)
                                 // Special handling for AN ops like 'MOD', 'DIV', '^' would be needed here.
    node->right->accept(this);
    write(")");
}

void CodeGenerator::visit(OutputNode* node) {
    indent_spaces();
    write("printf(");

    std::string format_string = "";
    std::vector<ExpressionNode*> printf_args;

    for (size_t i = 0; i < node->expressions.size(); ++i) {
        ExpressionNode* expr = node->expressions[i].get();
        if (i > 0) {
            format_string += " "; // Space between printed items
        }

        if (auto* str_lit = dynamic_cast<StringLiteralNode*>(expr)) {
            // TODO: Sanitize str_lit->value for C format string (e.g. escape % chars)
            format_string += str_lit->value;
        } else if (dynamic_cast<IntegerLiteralNode*>(expr) ||
                   (dynamic_cast<IdentifierNode*>(expr) /* && type is int from symbol_table */ )) {
            format_string += "%d";
            printf_args.push_back(expr);
        } else if (dynamic_cast<IdentifierNode*>(expr) /* && type is string from symbol_table */) {
             format_string += "%s";
             printf_args.push_back(expr);
        }
        // Add more types: %f for double/real, %c for char
        // Default or unknown type: try to treat as string if it's an identifier.
        // This part needs symbol table integration to determine identifier types.
        // For now, a simplification:
        else if (dynamic_cast<IdentifierNode*>(expr)) { // Fallback for identifiers
            format_string += "%s"; // Assuming it might be a string, or relying on C's flexibility (dangerous)
            printf_args.push_back(expr);
        } else if (dynamic_cast<BinaryOpNode*>(expr)) { // Assume binary ops result in int for now
             format_string += "%d";
             printf_args.push_back(expr);
        }
    }
    format_string += "\\n"; // Newline at the end of output

    write("\"" + format_string + "\"");

    for (ExpressionNode* arg_expr : printf_args) {
        write(", ");
        arg_expr->accept(this);
    }
    writeln(");");
}


void CodeGenerator::visit(IfNode* node) {
    indent_spaces();
    write("if (");
    node->condition->accept(this);
    writeln(") {");

    current_indent_level++;
    if (node->then_block) {
        node->then_block->accept(this);
    }
    current_indent_level--;
    indent_spaces();
    writeln("}");

    if (node->else_block) {
        indent_spaces();
        writeln("else {");
        current_indent_level++;
        node->else_block->accept(this);
        current_indent_level--;
        indent_spaces();
        writeln("}");
    }
}

void CodeGenerator::visit(WhileNode* node) {
    indent_spaces();
    write("while (");
    node->condition->accept(this);
    writeln(") {");
    current_indent_level++;
    if (node->body) { // Ensure body is not null
        node->body->accept(this);
    }
    current_indent_level--;
    indent_spaces();
    writeln("}");
}

void CodeGenerator::visit(RepeatUntilNode* node) {
    indent_spaces();
    writeln("do {");
    current_indent_level++;
    if (node->body) { // Ensure body is not null
        node->body->accept(this);
    }
    current_indent_level--;
    indent_spaces();
    write("} while (!("); // AN 'until (condition)' translates to C 'while (!(condition))'
    node->condition->accept(this);
    writeln("));");
}

void CodeGenerator::visit(ForNode* node) {
    indent_spaces();
    write("for (");
    // Assuming loop_variable is an IdentifierNode and its type is known (e.g., int)
    // For C, the variable might need to be declared if not already.
    // This basic version assumes it's an int and declared or usable in this scope.
    // A full implementation would use symbol table to get type or declare it.

    // Initializer part: e.g., i = start_value
    // We need to ensure the type of the loop variable is correctly handled.
    // For now, we don't explicitly declare it here, assuming it's declared in KAMUS.
    // If the language requires declaration in for loop, this needs change.
    // SymbolInfo* var_info = symbol_table.lookupSymbol(node->loop_variable->name);
    // if (var_info) { write(map_an_type_to_c(var_info->type) + " "); } else { /* default to int or error */ write("int ");}

    node->loop_variable->accept(this);
    write(" = ");
    node->start_value->accept(this);
    write("; ");

    // Condition part: e.g., i <= end_value
    node->loop_variable->accept(this);
    write(" <= "); // Assuming 'TO' means inclusive end
    node->end_value->accept(this);
    write("; ");

    // Increment part: e.g., i++
    node->loop_variable->accept(this);
    write("++) {");
    writeln(""); // Newline after opening brace

    current_indent_level++;
    if (node->body) { // Ensure body is not null
        node->body->accept(this);
    }
    current_indent_level--;
    indent_spaces();
    writeln("}");
}

void CodeGenerator::visit(DependOnNode* node) {
    indent_spaces();
    write("switch (");
    if (node->control_variable) {
        node->control_variable->accept(this);
    } else {
        write("/* ERROR: Missing control variable for DEPEND ON */");
    }
    writeln(") {");
    current_indent_level++;

    for (const auto& case_branch : node->cases) {
        if (case_branch) {
            case_branch->accept(this);
        }
    }

    current_indent_level--;
    indent_spaces();
    writeln("}");
}

void CodeGenerator::visit(CaseBranchNode* node) {
    if (node->is_otherwise()) { // This is the 'otherwise' (default) case
        indent_spaces();
        writeln("default:");
    } else { // This is a 'case value:'
        indent_spaces();
        write("case ");
        if (node->value) {
            node->value->accept(this);
        } else {
            write("/* ERROR: Missing value for CASE */");
        }
        writeln(":");
    }

    current_indent_level++;
    if (node->body) {
        node->body->accept(this);
    }
    // Add break statement, crucial for C switch-case behavior to mimic AN depend on
    indent_spaces();
    writeln("break;");
    current_indent_level--;
}

void CodeGenerator::visit(FunctionParameterNode* node) {
    // Generates "type name" for parameter lists
    write(map_an_type_to_c(node->param_type) + " " + node->param_name);
}

void CodeGenerator::visit(FunctionPrototypeNode* node) {
    // Generates a function prototype (declaration)
    indent_spaces(); // Usually prototypes are at global scope or in class defs, so indent might be 0
    write(map_an_type_to_c(node->return_type) + " " + node->func_name + "(");
    for (size_t i = 0; i < node->parameters.size(); ++i) {
        if (i > 0) {
            write(", ");
        }
        node->parameters[i]->accept(this);
    }
    write(");");
    writeln(); // Newline after prototype
}

void CodeGenerator::visit(SubprogramBodyNode* node) {
    // Generates a full function definition
    indent_spaces(); // Function definitions are typically at global scope.
    write(map_an_type_to_c(node->prototype->return_type) + " " + node->prototype->func_name + "(");
    for (size_t i = 0; i < node->prototype->parameters.size(); ++i) {
        if (i > 0) {
            write(", ");
        }
        node->prototype->parameters[i]->accept(this);
    }
    writeln(") {");
    current_indent_level++;

    // Local declarations (KAMUS LOKAL)
    for (const auto& decl : node->local_declarations) {
        decl->accept(this);
    }
    if (!node->local_declarations.empty()) {
        writeln(""); // Newline after local declarations
    }

    // Function body (ALGORITMA)
    if (node->body) {
        node->body->accept(this);
    }

    // Return statement for non-void functions if not explicitly handled by user code.
    // This is a simplification. Real compilers do flow analysis to check for missing returns.
    // if (map_an_type_to_c(node->prototype->return_type) != "void") {
    //     // Could add a default return if no return statement was found in the block.
    //     // indent_spaces(); writeln("return /* default value or error */;");
    // }

    current_indent_level--;
    indent_spaces();
    writeln("}");
}