#include "../include/generator.hpp"
#include "../include/parser.hpp"      // For AST node concrete type details
#include "../include/symbol_table.hpp" // Potentially for type lookups
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm> // For std::transform or other utilities if needed
#include <string>    // For std::to_string

// --- CodeGenerator Constructor ---
CodeGenerator::CodeGenerator() : out_stream_ptr(nullptr), symbol_table_ptr(nullptr), current_indent_level(0) {}

// --- Main Generation Method ---
void CodeGenerator::generate_c_code(const ProgramNode* program_root, const SymbolTable& populated_symbol_table, std::ostream& output_stream_ref) {
    if (!program_root) {
        // Or handle error appropriately
        std::cerr << "Error: Program root is null." << std::endl;
        return;
    }
    this->out_stream_ptr = &output_stream_ref;
    this->symbol_table_ptr = &populated_symbol_table; // Use the passed-in symbol table
    this->current_indent_level = 0;
    // No longer creating a new SymbolTable here: this->symbol_table = SymbolTable();

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

void CodeGenerator::visit(const ProgramNode* node) {
    writeln("#include <stdio.h>");
    writeln("#include <stdbool.h>"); // For bool type
    writeln("#include <string.h>");  // For string operations if needed (e.g. strcpy for char*)
    writeln("#include <stdlib.h>");  // For malloc/free if using dynamic char* for strings
    writeln("#include <math.h>");    // Added for pow() function

    // Forward declarations for functions (if any are in global_declarations or subprograms)
    // This is a simplified approach. A more robust way is two passes or hoisting.
    for (const auto& decl : node->global_declarations) {
        if (dynamic_cast<const FunctionPrototypeNode*>(decl.get())) { // Check if it's a function prototype
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
        if (dynamic_cast<const VariableDeclarationNode*>(decl.get())) {
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

void CodeGenerator::visit(const BlockNode* node) {
    // For blocks not directly part of main or a function's immediate body,
    // C requires them to be part of a statement (e.g. if, else, while blocks).
    // The {} are typically handled by the visit methods for those statements.
    // This visit(BlockNode*) is for sequences of statements.
    for (const auto& stmt : node->statements) {
        stmt->accept(this);
    }
}

void CodeGenerator::visit(const VariableDeclarationNode* node) {
    if (symbol_table_ptr) {
        const SymbolInfo* info = symbol_table_ptr->lookupSymbol(node->var_name);
        if (info && info->is_array) {
            std::string c_element_type = map_an_type_to_c(info->array_element_type);
            long long size = info->array_max_bound - info->array_min_bound + 1;
            if (size <= 0) {
                // writeln("/* ERROR: Array '" + node->var_name + "' has invalid size (" + std::to_string(size) + "). Defaulting to size 1. */");
                size = 1; // Fallback to avoid C error, though it's a semantic issue.
            }
            indent_spaces();
            write(c_element_type + " " + node->var_name + "[" + std::to_string(size) + "];");
            writeln(" /* AN: array [" + std::to_string(info->array_min_bound) + ".." + std::to_string(info->array_max_bound) + "] of " + info->array_element_type + " */");
            return;
        }
    }
    indent_spaces();
    write(map_an_type_to_c(node->var_type) + " " + node->var_name);
    writeln(";");
}

void CodeGenerator::visit(const AssignmentNode* node) {
    indent_spaces();
    node->target->accept(this); // Should produce the variable name
    write(" = ");
    node->value->accept(this);  // Should produce the expression
    writeln(";");
}

void CodeGenerator::visit(const IdentifierNode* node) {
    write(node->name);
}

void CodeGenerator::visit(const IntegerLiteralNode* node) {
    write(std::to_string(node->value));
}

void CodeGenerator::visit(const StringLiteralNode* node) {
    write("\"" + node->value + "\"");
}

void CodeGenerator::visit(const BinaryOpNode* node) {
    write("(");
    if (node->op == "^") {
        write("pow(");
        node->left->accept(this);
        write(", ");
        node->right->accept(this);
        write(")");
    } else {
        node->left->accept(this);
        std::string c_operator;
        if (node->op == "=") c_operator = " == ";
        else if (node->op == "<>") c_operator = " != ";
        else if (node->op == "DIV") c_operator = " / ";
        else if (node->op == "MOD") c_operator = " % ";
        else if (node->op == "AND") c_operator = " && ";
        else if (node->op == "OR") c_operator = " || ";
        else c_operator = " " + node->op + " ";
        write(c_operator);
        node->right->accept(this);
    }
    write(")");
}

void CodeGenerator::visit(const UnaryOpNode* node) {
    if (node->op == "NOT") {
        write("!(");
        if (node->operand) {
            node->operand->accept(this);
        } else {
            write("/* ERROR: Missing operand for NOT */");
        }
        write(")");
    }
    else {
        write(node->op + "(");
        if (node->operand) {
            node->operand->accept(this);
        } else {
            write("/* ERROR: Missing operand for unary op " + node->op + " */");
        }
        write(")");
    }
}

void CodeGenerator::visit(const ArrayAccessNode* node) {
    if (!node || !node->array_identifier || !node->index_expression) {
        write("/* ERROR: Incomplete ArrayAccessNode */");
        return;
    }
    const SymbolInfo* info = nullptr;
    if (symbol_table_ptr) {
        info = symbol_table_ptr->lookupSymbol(node->array_identifier->name);
    }
    node->array_identifier->accept(this);
    write("[");
    node->index_expression->accept(this);
    if (info && info->is_array && info->array_min_bound != 0) {
        write(" - " + std::to_string(info->array_min_bound));
    } else if (!info || !info->is_array) {
        write(" /* WARN: Array '");
        node->array_identifier->accept(this);
        write("' not found or not an array in symbol table. Indexing as is. */");
    }
    write("]");
}

void CodeGenerator::visit(const FunctionCallNode* node) {
    if (!node || !node->function_name) {
        write("/* ERROR: Invalid function call node */");
        return;
    }
    node->function_name->accept(this);
    write("(");
    for (size_t i = 0; i < node->arguments.size(); ++i) {
        if (node->arguments[i]) {
            node->arguments[i]->accept(this);
        } else {
            write("/* ERROR: Null argument in function call */");
        }
        if (i < node->arguments.size() - 1) {
            write(", ");
        }
    }
    write(")");
}

void CodeGenerator::visit(const OutputNode* node) {
    indent_spaces();
    write("printf(");
    std::string format_string = "";
    std::vector<const ExpressionNode*> printf_args; // Changed to const ExpressionNode*
    for (size_t i = 0; i < node->expressions.size(); ++i) {
        const ExpressionNode* expr = node->expressions[i].get(); // expr is now const*
        if (i > 0 && !format_string.empty() && format_string.back() != ' ') {
            format_string += " ";
        }
        if (auto* str_lit = dynamic_cast<const StringLiteralNode*>(expr)) {
            std::string val = str_lit->value;
            size_t pos = 0;
            while ((pos = val.find('%', pos)) != std::string::npos) {
                val.replace(pos, 1, "%%");
                pos += 2;
            }
            format_string += val;
        } else {
            std::string an_type = "integer";
            if (auto* ident_node = dynamic_cast<const IdentifierNode*>(expr)) {
                const SymbolInfo* info = nullptr; // Changed to const
                if (symbol_table_ptr) {
                    info = symbol_table_ptr->lookupSymbol(ident_node->name);
                }
                if (info) {
                    an_type = info->type;
                } else {
                    if (ident_node->name.rfind("r_", 0) == 0) an_type = "real";
                    else if (ident_node->name.rfind("c_", 0) == 0) an_type = "character";
                    else if (ident_node->name.rfind("b_", 0) == 0) an_type = "boolean";
                    else if (ident_node->name.rfind("s_", 0) == 0) an_type = "string";
                    else an_type = "integer";
                }
            } else if (dynamic_cast<const IntegerLiteralNode*>(expr)) {
                an_type = "integer";
            }
            else if (auto* bin_op_node = dynamic_cast<const BinaryOpNode*>(expr)) {
                if (bin_op_node->op == "AND" || bin_op_node->op == "OR" ||
                    bin_op_node->op == "=" || bin_op_node->op == "<>" ||
                    bin_op_node->op == "<" || bin_op_node->op == "<=" ||
                    bin_op_node->op == ">" || bin_op_node->op == ">=") {
                    an_type = "boolean";
                } else if (bin_op_node->op == "/" ) {
                     an_type = "real";
                }
                 else {
                    an_type = "integer";
                }
            } else if (auto* unary_op_node = dynamic_cast<const UnaryOpNode*>(expr)) {
                if (unary_op_node->op == "NOT") {
                    an_type = "boolean";
                } else {
                    an_type = "integer";
                }
            }
            if (an_type == "real") format_string += "%f";
            else if (an_type == "character") format_string += "%c";
            else if (an_type == "boolean") format_string += "%d";
            else if (an_type == "string") format_string += "%s";
            else format_string += "%d";
            printf_args.push_back(expr);
        }
    }
    format_string += "\\n";
    write("\"" + format_string + "\"");
    for (const ExpressionNode* arg_expr : printf_args) { // arg_expr is now const*
        write(", ");
        arg_expr->accept(this);
    }
    writeln(");");
}

void CodeGenerator::visit(const InputNode* node) {
    indent_spaces();
    if (!node || node->variables.empty()) {
        return;
    }
    std::string format_string = "";
    std::string scanf_args_string = "";
    for (size_t i = 0; i < node->variables.size(); ++i) {
        const auto& var_ident_node = node->variables[i];
        if (!var_ident_node) continue;
        std::string var_name = var_ident_node->name;
        std::string an_type = "integer";
        bool type_found = false;
        if (symbol_table_ptr) {
            const SymbolInfo* info = symbol_table_ptr->lookupSymbol(var_name);
            if (info) {
                an_type = info->type;
                type_found = true;
            }
        }
        if (!type_found) {
            format_string += "%s /* UD_ID: " + var_name + " */";
            scanf_args_string += ", " + var_name;
            continue;
        }
        if (i > 0) {
            format_string += " ";
        }
        if (an_type == "integer") {
            format_string += "%d";
            scanf_args_string += ", &" + var_name;
        } else if (an_type == "real") {
            format_string += "%lf";
            scanf_args_string += ", &" + var_name;
        } else if (an_type == "character") {
            format_string += " %c";
            scanf_args_string += ", &" + var_name;
        } else if (an_type == "string") {
            format_string += "%99s";
            scanf_args_string += ", " + var_name;
        } else if (an_type == "boolean") {
            format_string += "%d";
            scanf_args_string += ", &" + var_name;
        } else {
            format_string += "%s /* UnknownType: " + an_type + " for " + var_name + " */";
            scanf_args_string += ", " + var_name;
        }
    }
    write("scanf(\"" + format_string + "\"" + scanf_args_string + ");");
    writeln("");
}

void CodeGenerator::visit(const IfNode* node) {
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

void CodeGenerator::visit(const WhileNode* node) {
    indent_spaces();
    write("while (");
    node->condition->accept(this);
    writeln(") {");
    current_indent_level++;
    if (node->body) {
        node->body->accept(this);
    }
    current_indent_level--;
    indent_spaces();
    writeln("}");
}

void CodeGenerator::visit(const RepeatUntilNode* node) {
    indent_spaces();
    writeln("do {");
    current_indent_level++;
    if (node->body) {
        node->body->accept(this);
    }
    current_indent_level--;
    indent_spaces();
    write("} while (!(");
    node->condition->accept(this);
    writeln("));");
}

void CodeGenerator::visit(const ForNode* node) {
    indent_spaces();
    write("for (");
    node->loop_variable->accept(this);
    write(" = ");
    node->start_value->accept(this);
    write("; ");
    node->loop_variable->accept(this);
    write(" <= ");
    node->end_value->accept(this);
    write("; ");
    node->loop_variable->accept(this);
    write("++) {");
    writeln("");
    current_indent_level++;
    if (node->body) {
        node->body->accept(this);
    }
    current_indent_level--;
    indent_spaces();
    writeln("}");
}

void CodeGenerator::visit(const DependOnNode* node) {
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

void CodeGenerator::visit(const CaseBranchNode* node) {
    if (node->is_otherwise()) {
        indent_spaces();
        writeln("default:");
    } else {
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
    indent_spaces();
    writeln("break;");
    current_indent_level--;
}

void CodeGenerator::visit(const FunctionParameterNode* node) {
    write(map_an_type_to_c(node->param_type) + " " + node->param_name);
}

void CodeGenerator::visit(const FunctionPrototypeNode* node) {
    indent_spaces();
    write(map_an_type_to_c(node->return_type) + " " + node->func_name + "(");
    for (size_t i = 0; i < node->parameters.size(); ++i) {
        if (i > 0) {
            write(", ");
        }
        node->parameters[i]->accept(this);
    }
    write(");");
    writeln();
}

void CodeGenerator::visit(const SubprogramBodyNode* node) {
    indent_spaces();
    write(map_an_type_to_c(node->prototype->return_type) + " " + node->prototype->func_name + "(");
    for (size_t i = 0; i < node->prototype->parameters.size(); ++i) {
        if (i > 0) {
            write(", ");
        }
        node->prototype->parameters[i]->accept(this);
    }
    writeln(") {");
    current_indent_level++;
    for (const auto& decl : node->local_declarations) {
        decl->accept(this);
    }
    if (!node->local_declarations.empty()) {
        writeln("");
    }
    if (node->body) {
        node->body->accept(this);
    }
    current_indent_level--;
    indent_spaces();
    writeln("}");
}
// All duplicated code after this point is removed.