// --- BEGINNING OF src/generator.cpp CONTENT ---
#include "../include/generator.hpp"
#include "../include/parser.hpp"      // For AST node concrete type details
#include "../include/symbol_table.hpp" // For SymbolTable and SymbolInfo
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <string>
#include <vector>
#include <set> // Added for known_adt_modules if used locally
#include <sstream> // Added for std::ostringstream

// --- CodeGenerator Constructor ---
CodeGenerator::CodeGenerator() : out_stream_ptr(nullptr), symbol_table_ptr(nullptr), current_indent_level(0) {}

// --- Main Generation Method ---
void CodeGenerator::generate_c_code(const ProgramNode* program_root, const SymbolTable& populated_symbol_table, std::ostream& output_stream_ref) {
    if (!program_root) {
        std::cerr << "Error: Program root is null." << std::endl;
        return;
    }
    this->out_stream_ptr = &output_stream_ref;
    this->symbol_table_ptr = &populated_symbol_table;
    this->current_indent_level = 0;
    program_root->accept(this); // This call is now valid due to const accept and const ProgramNode*
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
    if (lower_type == "real") return "double";
    if (lower_type == "boolean") return "bool";
    if (lower_type == "string") return "char*";
    if (lower_type == "character") return "char";
    if (lower_type == "void") return "void";

    // Check if it's a known user-defined type (record/enum)
    if (symbol_table_ptr) { // Ensure symbol_table_ptr is valid
        const SymbolInfo* type_info = symbol_table_ptr->lookupSymbol(an_type); // lookupSymbol is const
        if (type_info && (type_info->is_record_type || type_info->is_enum_type)) {
            return an_type; // Return the type name itself (it's a typedef in C)
        }
    }
    return an_type; // Fallback for unknown types, or if it's already a C type
}

void CodeGenerator::generate_expression_with_dereference_if_needed(const ExpressionNode* expr_node) {
    if (!expr_node) {
        write("/* NULL_EXPR */");
        return;
    }
    const IdentifierNode* id_node = dynamic_cast<const IdentifierNode*>(expr_node);
    if (id_node && symbol_table_ptr) {
        const SymbolInfo* info = symbol_table_ptr->lookupSymbol(id_node->name);
        if (info && info->kind == "parameter" && (info->param_mode == ParameterMode::OUT || info->param_mode == ParameterMode::IN_OUT)) {
            // For string output params (char**), dereferencing once gives char*
            // For other pointers (int*, double*, char* for IN_OUT string), dereferencing gives value type
            write("(*");
            // id_node->accept(this); // No, this would just print name again. We need the name.
            write(id_node->name);
            write(")");
            return;
        }
    }
    expr_node->accept(this);
}

// --- Visit Method Implementations ---

void CodeGenerator::visit(const ProgramNode* node) {
    writeln("#include <stdio.h>");
    writeln("#include <stdbool.h>");
    writeln("#include <string.h>");
    writeln("#include <stdlib.h>");
    writeln("#include <math.h>");
    writeln("");

    // Generate RecordTypeNode definitions first (structs)
    for (const auto& decl : node->global_declarations) {
        if (dynamic_cast<const RecordTypeNode*>(decl.get())) {
            decl->accept(this);
        }
    }
    // Generate EnumTypeNode definitions next (enums)
    for (const auto& decl : node->global_declarations) {
        if (dynamic_cast<const EnumTypeNode*>(decl.get())) {
            decl->accept(this);
        }
    }
    // Generate ConstantDeclarations next
    for (const auto& decl : node->global_declarations) {
        if (dynamic_cast<const ConstantDeclarationNode*>(decl.get())) {
            decl->accept(this);
        }
    }
    // Forward declarations for functions/procedures
    for (const auto& decl : node->global_declarations) {
        if (dynamic_cast<const FunctionPrototypeNode*>(decl.get())) {
            decl->accept(this);
        }
    }
    for (const auto& subprogram : node->subprograms) {
        if (subprogram && subprogram->prototype) {
            subprogram->prototype->accept(this);
        }
    }
    writeln("");

    writeln("int main(void) {"); // Changed to (void)
    current_indent_level++;

    bool has_main_kamus = false;
    for (const auto& decl : node->global_declarations) {
        if (dynamic_cast<const VariableDeclarationNode*>(decl.get())) {
            has_main_kamus = true;
            decl->accept(this);
        }
    }
    if(has_main_kamus) writeln("");

    if (node->main_algoritma) {
        node->main_algoritma->accept(this);
    }

    indent_spaces();
    writeln("return 0;");
    current_indent_level--;
    writeln("}");
    writeln("");

    for (const auto& subprogram : node->subprograms) {
        subprogram->accept(this);
        writeln("");
    }
}

void CodeGenerator::visit(const BlockNode* node) {
    for (const auto& stmt : node->statements) {
        stmt->accept(this);
    }
}

void CodeGenerator::visit(const VariableDeclarationNode* node) {
    if (!symbol_table_ptr) { write("/* SYMBOL TABLE NOT AVAILABLE */\n"); return; }
    const SymbolInfo* info = symbol_table_ptr->lookupSymbol(node->var_name);
    if (!info) { writeln("/* SymbolInfo not found for " + node->var_name + " */"); return; }

    indent_spaces();
    std::string c_type_str;
    if (info->is_pointer_type) {
        std::string base_type_name = info->pointed_type;
        if (base_type_name == "string") {
            c_type_str = "char**";
        } else {
            c_type_str = map_an_type_to_c(base_type_name) + "*";
        }
        write(c_type_str + " " + node->var_name + " = NULL;");
    } else if (info->is_array) {
        std::string c_element_type = map_an_type_to_c(info->array_element_type);
        long long size = info->array_max_bound - info->array_min_bound + 1;
        if (size <= 0) { size = 1; }
        write(c_element_type + " " + node->var_name + "[" + std::to_string(size) + "];");
        if (!info->array_element_type.empty()) {
            write(" /* AN: array [" + std::to_string(info->array_min_bound) + ".." + std::to_string(info->array_max_bound) + "] of " + info->array_element_type + " */");
        }
    } else if (info->is_record_type || info->is_enum_type) {
        c_type_str = info->type;
        write(c_type_str + " " + node->var_name + ";");
    } else {
        c_type_str = map_an_type_to_c(info->type);
        write(c_type_str + " " + node->var_name);
        if (info->type == "string") {
             write(" = NULL");
        }
        write(";");
    }
     if (info->is_pointer_type || info->is_array) writeln(); // only newline if it was a complex type not ending in ;
     else writeln(); // Ensure simple types also get a newline if not already done by ;
}


void CodeGenerator::visit(const AssignmentNode* node) {
    indent_spaces();
    const IdentifierNode* target_id_node = dynamic_cast<const IdentifierNode*>(node->target.get());
    const DereferenceNode* target_deref_node = dynamic_cast<const DereferenceNode*>(node->target.get()); // For dereference(p) <- value

    if (target_id_node && symbol_table_ptr) {
        const SymbolInfo* info = symbol_table_ptr->lookupSymbol(target_id_node->name);
        if (info && info->kind == "parameter" &&
            (info->param_mode == ParameterMode::OUT || info->param_mode == ParameterMode::IN_OUT) &&
            info->type == "string") { // Special handling for string OUT or IN_OUT parameters (char**)

            // Free existing string if any, then strdup new one
            write("if (*" + target_id_node->name + ") free(*" + target_id_node->name + "); // Free old string if any\n");
            indent_spaces();
            write("*" + target_id_node->name + " = strdup(");
            generate_expression_with_dereference_if_needed(node->value.get());
            write(");");
            writeln();
            return;
        }
    }

    if (target_deref_node && symbol_table_ptr) {
        const IdentifierNode* underlying_pointer_id_node = nullptr;
        // Try to get the identifier of the pointer being dereferenced.
        // This logic might need to be more robust for complex pointer expressions.
        if (const IdentifierNode* id_in_deref = dynamic_cast<const IdentifierNode*>(target_deref_node->pointer_expr.get())) {
            underlying_pointer_id_node = id_in_deref;
        }

        if (underlying_pointer_id_node) {
            const SymbolInfo* ptr_info = symbol_table_ptr->lookupSymbol(underlying_pointer_id_node->name);
            // If assigning to dereferenced 'pointer to string' (char** pStr; *pStr = "value")
            if (ptr_info && ptr_info->is_pointer_type && ptr_info->pointed_type == "string") {
                write("if (");
                target_deref_node->accept(this); // Generates (*pStr)
                write(") free(");
                target_deref_node->accept(this); // Generates (*pStr)
                write(");\n");
                indent_spaces();
                target_deref_node->accept(this); // Generates (*pStr)
                write(" = strdup(");
                generate_expression_with_dereference_if_needed(node->value.get()); // Value to assign
                write(");");
                writeln();
                return;
            }
        }
    }

    // Default assignment for other cases (simple variables, non-string OUT/IN_OUT params, array elements, etc.)
    node->target->accept(this);
    write(" = ");
    generate_expression_with_dereference_if_needed(node->value.get());
    writeln(";");
}

void CodeGenerator::visit(const IdentifierNode* node) {
    write(node->name);
}
void CodeGenerator::visit(const IntegerLiteralNode* node) { write(std::to_string(node->value));}
void CodeGenerator::visit(const StringLiteralNode* node) {
    std::string escaped_val = "";
    for (char c : node->value) {
        if (c == '"') escaped_val += "\\\""; // Escape double quotes
        else if (c == '\\') escaped_val += "\\\\"; // Escape backslashes
        else escaped_val += c;
    }
    write("\"" + escaped_val + "\"");
}
void CodeGenerator::visit(const NullLiteralNode* node) { (void)node; write("NULL"); }

void CodeGenerator::visit(const BinaryOpNode* node) {
    write("(");
    generate_expression_with_dereference_if_needed(node->left.get());
    std::string c_operator;
    if (node->op == "^") { // Special case for power, map to pow()
        write(", ");
        generate_expression_with_dereference_if_needed(node->right.get());
        // Need to adjust structure for pow: write("pow(left, right)")
        // Re-arranging for pow:
        std::string temp_out_str = "";
        std::ostream* original_out_stream_ptr = out_stream_ptr;
        std::ostringstream oss;
        out_stream_ptr = &oss; // Capture left and right operands

        write("pow(");
        generate_expression_with_dereference_if_needed(node->left.get());
        write(", ");
        generate_expression_with_dereference_if_needed(node->right.get());
        write(")");

        out_stream_ptr = original_out_stream_ptr; // Restore original stream
        write(oss.str()); // Write captured pow expression

    } else {
        if (node->op == "=") c_operator = " == ";
        else if (node->op == "<>") c_operator = " != ";
        else if (node->op == "DIV") c_operator = " / ";
        else if (node->op == "MOD") c_operator = " % ";
        else if (node->op == "AND") c_operator = " && ";
        else if (node->op == "OR") c_operator = " || ";
        else c_operator = " " + node->op + " ";
        write(c_operator);
        generate_expression_with_dereference_if_needed(node->right.get());
    }
    write(")");
}

void CodeGenerator::visit(const UnaryOpNode* node) {
    if (node->op == "NOT") {
        write("!(");
        if (node->operand) generate_expression_with_dereference_if_needed(node->operand.get());
        write(")");
    } else {
        write(node->op + "("); // For potential future unary ops like '-'
        if (node->operand) generate_expression_with_dereference_if_needed(node->operand.get());
        write(")");
    }
}

void CodeGenerator::visit(const ArrayAccessNode* node) {
    if (!node || !node->array_identifier || !node->index_expression) { write("/* ERROR: Incomplete ArrayAccessNode */"); return; }
    const SymbolInfo* info = nullptr;
    if (symbol_table_ptr) info = symbol_table_ptr->lookupSymbol(node->array_identifier->name);

    node->array_identifier->accept(this);
    write("[");
    generate_expression_with_dereference_if_needed(node->index_expression.get());
    if (info && info->is_array && info->array_min_bound != 0) {
        write(" - (" + std::to_string(info->array_min_bound) + ")"); // Parenthesize for safety
    }
    write("]");
}

void CodeGenerator::visit(const FunctionCallNode* node) {
    if (!node || !node->function_name) { write("/* ERROR: Invalid function call node */"); return; }
    const SymbolInfo* func_sym_info = nullptr;
    if(symbol_table_ptr) func_sym_info = symbol_table_ptr->lookupSymbol(node->function_name->name);

    node->function_name->accept(this);
    write("(");

    if (node->arguments.empty()) {
        if (func_sym_info && func_sym_info->kind != "variable" && func_sym_info->param_modes_for_call_site.empty()) {
             write("void"); // Explicitly pass void if function is known to take no arguments
        }
    } else {
        for (size_t i = 0; i < node->arguments.size(); ++i) {
            bool passed_by_address = false;
            if (func_sym_info && i < func_sym_info->param_modes_for_call_site.size()) {
                 ParameterMode mode = func_sym_info->param_modes_for_call_site[i];
                 if (mode == ParameterMode::OUT || mode == ParameterMode::IN_OUT) {
                     if (dynamic_cast<const IdentifierNode*>(node->arguments[i].get()) ||
                         dynamic_cast<const ArrayAccessNode*>(node->arguments[i].get()) ||
                         dynamic_cast<const DereferenceNode*>(node->arguments[i].get()) ||
                         dynamic_cast<const PointerMemberAccessNode*>(node->arguments[i].get())) {
                         write("&");
                         passed_by_address = true;
                     } else {
                         write("/* ERROR: Cannot pass non-addressable expr for OUT/IN_OUT param */ ");
                     }
                 }
            }
            if (!passed_by_address) {
                 generate_expression_with_dereference_if_needed(node->arguments[i].get());
            } else {
                 node->arguments[i]->accept(this);
            }
            if (i < node->arguments.size() - 1) write(", ");
        }
    }
    write(")");
}

void CodeGenerator::visit(const OutputNode* node) {
    indent_spaces();
    write("printf(\"");
    std::string format_string_content = "";
    std::vector<const ExpressionNode*> printf_args_expressions;

    for (size_t i = 0; i < node->expressions.size(); ++i) {
        const ExpressionNode* current_expr = node->expressions[i].get();
        if (i > 0 && !format_string_content.empty() && format_string_content.back() != ' ') {
            format_string_content += " ";
        }

        if (const StringLiteralNode* sl = dynamic_cast<const StringLiteralNode*>(current_expr)) {
            std::string val = sl->value;
            for (char c : val) { // More robustly escape %
                if (c == '%') format_string_content += "%%";
                else format_string_content += c;
            }
        } else {
            std::string an_type_for_format = "integer";
            const SymbolInfo* expr_type_info = nullptr;

            if (const IdentifierNode* id_node = dynamic_cast<const IdentifierNode*>(current_expr)) {
                if (symbol_table_ptr) expr_type_info = symbol_table_ptr->lookupSymbol(id_node->name);
            }

            if (expr_type_info) {
                if (expr_type_info->kind == "parameter" && (expr_type_info->param_mode == ParameterMode::OUT || expr_type_info->param_mode == ParameterMode::IN_OUT)) {
                    an_type_for_format = expr_type_info->type; // The base type of the parameter
                } else if (expr_type_info->is_pointer_type) {
                     an_type_for_format = expr_type_info->pointed_type.empty() ? "pointer_address" : expr_type_info->pointed_type;
                } else if (expr_type_info->is_enum_type) {
                    an_type_for_format = "integer";
                } else {
                    an_type_for_format = expr_type_info->type;
                }
            } else if (dynamic_cast<const IntegerLiteralNode*>(current_expr)) {
                an_type_for_format = "integer";
            } else if (dynamic_cast<const NullLiteralNode*>(current_expr)) {
                an_type_for_format = "pointer_address";
            }
            // TODO: Add more types: real, boolean, char literals etc. for non-identifier expressions
            // For complex expressions (BinaryOpNode, etc.), type inference is needed.
            // This simplified logic might default to %d for complex expressions.

            std::string lower_an_type = an_type_for_format;
            std::transform(lower_an_type.begin(), lower_an_type.end(), lower_an_type.begin(), ::tolower);

            if (lower_an_type == "real") format_string_content += "%f"; // %lf for scanf, %f for printf often works for double
            else if (lower_an_type == "character") format_string_content += "%c";
            else if (lower_an_type == "boolean") format_string_content += "%d";
            else if (lower_an_type == "string") format_string_content += "%s";
            else if (lower_an_type == "pointer_address") format_string_content += "%p";
            else format_string_content += "%d";

            printf_args_expressions.push_back(current_expr);
        }
    }

    if (!node->omit_newline) {
        format_string_content += "\\n";
    }
    write(format_string_content);
    write("\"");

    for (const ExpressionNode* arg_expr : printf_args_expressions) {
        write(", ");
        generate_expression_with_dereference_if_needed(arg_expr);
    }
    writeln(");");
}

void CodeGenerator::visit(const InputNode* node) {
    indent_spaces();
    if (!node || node->variables.empty()) { return; }
    std::string format_string = "";
    std::string scanf_args_string = "";
    for (size_t i = 0; i < node->variables.size(); ++i) {
        const auto& var_ident_node_ptr = node->variables[i];
        if (!var_ident_node_ptr) continue;
        const IdentifierNode& var_ident_node = *var_ident_node_ptr;
        std::string var_name = var_ident_node.name;
        std::string an_type_for_scanf = "integer";
        const SymbolInfo* info = nullptr;
        if (symbol_table_ptr) info = symbol_table_ptr->lookupSymbol(var_name);

        std::string address_prefix = "&";
        if (info) {
            if (info->is_pointer_type) {
                an_type_for_scanf = info->pointed_type;
                address_prefix = ""; // Read into *ptr, so pass ptr to scanf
            } else {
                an_type_for_scanf = info->type;
            }
            if (an_type_for_scanf == "string" && !info->is_pointer_type) { // s : string (char*), pass s
                 address_prefix = "";
            }
        } else {
            writeln("/* Warning: SymbolInfo not found for input var " + var_name + ". Assuming int*. */");
        }

        if (i > 0) format_string += " ";
        std::string lower_type = an_type_for_scanf;
        std::transform(lower_type.begin(), lower_type.end(), lower_type.begin(), ::tolower);

        if (lower_type == "integer") { format_string += "%d"; scanf_args_string += ", " + address_prefix + var_name; }
        else if (lower_type == "real") { format_string += "%lf"; scanf_args_string += ", " + address_prefix + var_name; }
        else if (lower_type == "character") { format_string += " %c"; scanf_args_string += ", " + address_prefix + var_name; }
        else if (lower_type == "string") {
            format_string += "%99s";
            // If original variable was `pointer to string` (char**), then reading into `*var_name` (a char*) is problematic
            // as `*var_name` itself needs to be allocated. This requires `*var_name = malloc(...)` before scanf.
            // This is a complex case for `input()`. For now, assume simple `char var[100]` or `char* var = malloc(100)`.
            if (info && info->is_pointer_type && info->pointed_type == "string") { // p : pointer to string (char**)
                 // This case is complex: input(p) implies *p should be a buffer.
                 // For simplicity, assuming *p is already allocated, or this is an error.
                 // write("/* Ensure *(" + var_name + ") is allocated before scanf */ ");
                 scanf_args_string += ", *" + var_name; // scanf into the char* that the char** points to
            } else { // s : string (char*)
                 scanf_args_string += ", " + var_name;
            }
        } else if (lower_type == "boolean") {
            format_string += "%d";
            scanf_args_string += ", " + address_prefix + var_name;
        } else {
            format_string += "/* UNSUPPORTED_SCANF_TYPE(" + an_type_for_scanf + ") */";
        }
    }
    write("scanf(\"" + format_string + "\"" + scanf_args_string + ");");
    writeln("");
}

void CodeGenerator::visit(const IfNode* node) {
    indent_spaces();
    write("if (");
    generate_expression_with_dereference_if_needed(node->condition.get());
    writeln(") {");
    current_indent_level++;
    if (node->then_block) node->then_block->accept(this);
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
    generate_expression_with_dereference_if_needed(node->condition.get());
    writeln(") {");
    current_indent_level++;
    if (node->body) node->body->accept(this);
    current_indent_level--;
    indent_spaces();
    writeln("}");
}

void CodeGenerator::visit(const RepeatUntilNode* node) {
    indent_spaces();
    writeln("do {");
    current_indent_level++;
    if (node->body) node->body->accept(this);
    current_indent_level--;
    indent_spaces();
    write("} while (!(");
    generate_expression_with_dereference_if_needed(node->condition.get());
    writeln("));");
}

void CodeGenerator::visit(const ForNode* node) {
    indent_spaces();
    write("for (");
    node->loop_variable->accept(this);
    write(" = ");
    generate_expression_with_dereference_if_needed(node->start_value.get());
    write("; ");
    node->loop_variable->accept(this);
    write(" <= ");
    generate_expression_with_dereference_if_needed(node->end_value.get());
    write("; ");
    node->loop_variable->accept(this);
    write("++) {");
    writeln("");
    current_indent_level++;
    if (node->body) node->body->accept(this);
    current_indent_level--;
    indent_spaces();
    writeln("}");
}

void CodeGenerator::visit(const DependOnNode* node) {
    indent_spaces();
    write("switch (");
    if (node->control_variable) generate_expression_with_dereference_if_needed(node->control_variable.get());
    writeln(") {");
    current_indent_level++;
    for (const auto& case_branch : node->cases) {
        if (case_branch) case_branch->accept(this);
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
            node->value->accept(this); // Case values are literals, no deref needed.
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
    std::string c_type_str;
    if (node->mode == ParameterMode::OUT || node->mode == ParameterMode::IN_OUT) {
        if (node->param_type == "string") {
            c_type_str = "char**";
        } else {
            c_type_str = map_an_type_to_c(node->param_type) + "*";
        }
    } else {
        c_type_str = map_an_type_to_c(node->param_type);
    }
    write(c_type_str + " " + node->param_name);
}

void CodeGenerator::visit(const FunctionPrototypeNode* node) {
    indent_spaces();
    write(map_an_type_to_c(node->return_type) + " " + node->func_name + "(");
    if (node->parameters.empty()) {
        write("void");
    } else {
        for (size_t i = 0; i < node->parameters.size(); ++i) {
            if (i > 0) write(", ");
            if (node->parameters[i]) node->parameters[i]->accept(this);
        }
    }
    writeln(");");
}

void CodeGenerator::visit(const SubprogramBodyNode* node) {
    if (!node || !node->prototype) return;
    indent_spaces();
    write(map_an_type_to_c(node->prototype->return_type) + " " + node->prototype->func_name + "(");
    if (node->prototype->parameters.empty()) {
        write("void");
    } else {
        for (size_t i = 0; i < node->prototype->parameters.size(); ++i) {
            if (i > 0) write(", ");
            if (node->prototype->parameters[i]) node->prototype->parameters[i]->accept(this);
        }
    }
    writeln(") {");
    current_indent_level++;
    for (const auto& decl : node->local_declarations) {
        decl->accept(this);
    }
    if (!node->local_declarations.empty()) writeln("");
    if (node->body) {
        node->body->accept(this);
    }
    current_indent_level--;
    indent_spaces();
    writeln("}");
}

void CodeGenerator::visit(const ReferenceNode* node) {
    write("&(");
    if(node->target_expr) node->target_expr->accept(this); // Target of & might not need deref helper
    write(")");
}

void CodeGenerator::visit(const DereferenceNode* node) {
    write("(*");
    if(node->pointer_expr) node->pointer_expr->accept(this);
    write(")");
}

void CodeGenerator::visit(const AllocateNode* node) {
    write("malloc(");
    if(node->size_expr) generate_expression_with_dereference_if_needed(node->size_expr.get());
    write(")");
}

void CodeGenerator::visit(const ReallocateNode* node) {
    write("realloc(");
    if(node->pointer_expr) generate_expression_with_dereference_if_needed(node->pointer_expr.get());
    write(", ");
    if(node->new_size_expr) generate_expression_with_dereference_if_needed(node->new_size_expr.get());
    write(")");
}

void CodeGenerator::visit(const DeallocateNode* node) {
    write("free(");
    if(node->pointer_expr) generate_expression_with_dereference_if_needed(node->pointer_expr.get());
    write(")");
}

void CodeGenerator::visit(const EnumTypeNode* node) {
    indent_spaces();
    write("typedef enum { ");
    for (size_t i = 0; i < node->values.size(); ++i) {
        write(node->values[i]);
        if (i < node->values.size() - 1) {
            write(", ");
        }
    }
    write(" } " + node->name + ";");
    writeln(); writeln();
}

void CodeGenerator::visit(const ConstantDeclarationNode* node) {
    if (!symbol_table_ptr || !node || !node->value) return;
    const SymbolInfo* info = symbol_table_ptr->lookupSymbol(node->name);
    if (!info) { writeln("/* Error: Symbol info not found for constant: " + node->name + " */"); return; }

    std::string c_type;
    bool is_str_literal = dynamic_cast<const StringLiteralNode*>(node->value.get()) != nullptr;

    if (info->is_pointer_type) {
        if (dynamic_cast<const NullLiteralNode*>(node->value.get())) { // const X : pointer to Y = NULL
            c_type = map_an_type_to_c(info->pointed_type) + "*";
        } else { // e.g. const P : pointer to integer = reference(some_int_var) -> this is complex for const
            c_type = map_an_type_to_c(info->pointed_type) + "*"; // Placeholder, actual init needs care
        }
    } else if (info->type == "string" && is_str_literal) {
        c_type = "const char*"; // const MY_STR = "hello" -> const char* MY_STR = "hello"
    } else {
        c_type = map_an_type_to_c(info->type);
    }

    indent_spaces();
    if (c_type != "const char*") write("const "); // Avoid "const const char*"
    write(c_type + " " + node->name + " = ");
    node->value->accept(this);
    writeln(";");
}

void CodeGenerator::visit(const RecordTypeNode* node) {
    if (!symbol_table_ptr) { writeln("/* Error: Symbol table unavailable for RecordTypeNode */"); return; }
    const SymbolInfo* record_sym_info = symbol_table_ptr->lookupSymbol(node->name);
    if (!record_sym_info || !record_sym_info->is_record_type) {
        writeln("/* Error: Symbol info not found or not a record type for: " + node->name + " */");
        return;
    }

    indent_spaces();
    writeln("typedef struct " + node->name + " " + node->name + ";");
    indent_spaces();
    writeln("struct " + node->name + " {");
    current_indent_level++;
    for (const auto& field_detail : record_sym_info->record_fields) {
        indent_spaces();
        std::string c_field_type_str;
        const std::string pointer_prefix = "pointer to ";
        if (field_detail.type_str.rfind(pointer_prefix, 0) == 0) {
            std::string base_type = field_detail.type_str.substr(pointer_prefix.length());
            if (base_type == node->name) {
                c_field_type_str = node->name + "*";
            } else if (base_type == "string") {
                c_field_type_str = "char**";
            } else {
                c_field_type_str = map_an_type_to_c(base_type) + "*";
            }
        } else {
            c_field_type_str = map_an_type_to_c(field_detail.type_str);
        }
        write(c_field_type_str + " " + field_detail.name + ";");
        writeln();
    }
    current_indent_level--;
    indent_spaces();
    writeln("};");
    writeln();
}

void CodeGenerator::visit(const PointerMemberAccessNode* node) {
    write("(");
    if (node->pointer_expr) generate_expression_with_dereference_if_needed(node->pointer_expr.get());
    write(")->");
    if (node->member_name) node->member_name->accept(this);
    // Closing parenthesis removed as it's part of the expression context where this is used.
    // E.g. ( (ptr)->member ) + 1 . The outer parentheses are handled by BinaryOpNode visitor.
}

void CodeGenerator::visit(const FieldNode* node) {
    (void)node; // Fields are typically handled by RecordTypeNode visitor.
}
// --- END OF src/generator.cpp CONTENT ---
