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
    // Check symbol table for user-defined types (records, enums)
    if (symbol_table_ptr) {
        SymbolInfo* type_info = symbol_table_ptr->lookupSymbol(an_type);
        if (type_info && (type_info->is_record_type || type_info->is_enum_type)) {
            return an_type; // Return the name itself, as it will be a typedef'd name in C
        }
    }
    // Fallback for unrecognized types (should ideally not happen for valid AN code after parsing)
    return an_type;
}

// --- Visit Method Implementations ---

void CodeGenerator::visit(const ProgramNode* node) {
    writeln("#include <stdio.h>");
    writeln("#include <stdbool.h>");
    writeln("#include <string.h>");
    writeln("#include <stdlib.h>");
    writeln("#include <math.h>");

    for (const auto& decl : node->global_declarations) {
        if (dynamic_cast<const FunctionPrototypeNode*>(decl.get())) { // const dynamic_cast
            decl->accept(this);
        }
    }
     for (const auto& subprogram : node->subprograms) {
        subprogram->prototype->accept(this);
    }
    writeln("");

    writeln("int main() {");
    current_indent_level++;

    bool has_main_kamus = false;
    for (const auto& decl : node->global_declarations) {
        if (dynamic_cast<const VariableDeclarationNode*>(decl.get())) { // const dynamic_cast
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
    if (symbol_table_ptr) {
        const SymbolInfo* info = symbol_table_ptr->lookupSymbol(node->var_name); // const
        if (info && info->is_array) {
            std::string c_element_type = map_an_type_to_c(info->array_element_type);
            long long size = info->array_max_bound - info->array_min_bound + 1;
            if (size <= 0) {
                size = 1;
            }
            indent_spaces();
            write(c_element_type + " " + node->var_name + "[" + std::to_string(size) + "];");
            writeln(" /* AN: array [" + std::to_string(info->array_min_bound) + ".." + std::to_string(info->array_max_bound) + "] of " + info->array_element_type + " */");
            return;
        }
    }
    indent_spaces();
    const SymbolInfo* info = nullptr; // const
    if (symbol_table_ptr) {
        info = symbol_table_ptr->lookupSymbol(node->var_name); // const
    }

    if (info && info->is_pointer_type) {
        std::string c_base_type = map_an_type_to_c(info->pointed_type);
        indent_spaces();
        if (info->pointed_type == "string") {
            write("char* *" + node->var_name + "; /* pointer to string */");
        } else {
            write(c_base_type + "* " + node->var_name + "; /* pointer to " + info->pointed_type + " */");
        }
        writeln();
    } else if (!(info && info->is_array)) {
        indent_spaces();
        write(map_an_type_to_c(node->var_type) + " " + node->var_name); // node->var_type is fine
        writeln(";");
    }
}

void CodeGenerator::visit(const AssignmentNode* node) {
    indent_spaces();

    if (auto* target_id_node = dynamic_cast<const IdentifierNode*>(node->target.get())) { // const
        const SymbolInfo* target_sym_info = nullptr; // const
        if (symbol_table_ptr) {
            target_sym_info = symbol_table_ptr->lookupSymbol(target_id_node->name);
        }

        if (target_sym_info && target_sym_info->kind == "parameter" &&
            (target_sym_info->param_mode == ParameterMode::OUT || target_sym_info->param_mode == ParameterMode::IN_OUT)) {

            if (target_sym_info->type == "string" && target_sym_info->param_mode == ParameterMode::OUT) {
                write("if (*");
                target_id_node->accept(this);
                write(") free(*");
                target_id_node->accept(this);
                writeln(");");
                indent_spaces();
                write("*");
                target_id_node->accept(this);
                write(" = strdup(");
                node->value->accept(this);
                write(")");
            } else {
                write("*");
                target_id_node->accept(this);
                write(" = ");
                node->value->accept(this);
            }
        } else {
            target_id_node->accept(this);
            write(" = ");
            node->value->accept(this);
        }
    } else {
        node->target->accept(this);
        write(" = ");
        node->value->accept(this);
    }
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

// Helper function for expression generation with potential dereference
void CodeGenerator::generate_expression_with_dereference_if_needed(const ExpressionNode* expr_node) { // const
    if (auto* id_node = dynamic_cast<const IdentifierNode*>(expr_node)) { // const
        const SymbolInfo* sym_info = nullptr; // const
        if (symbol_table_ptr) {
            sym_info = symbol_table_ptr->lookupSymbol(id_node->name);
        }
        if (sym_info && sym_info->kind == "parameter" &&
            (sym_info->param_mode == ParameterMode::OUT || sym_info->param_mode == ParameterMode::IN_OUT)) {

            if (sym_info->type == "string" && sym_info->param_mode == ParameterMode::OUT) {
                write("(*");
                id_node->accept(this);
                write(")");
            } else {
                 write("(*");
                id_node->accept(this);
                write(")");
            }
            return;
        }
    }
    expr_node->accept(this);
}

void CodeGenerator::visit(const BinaryOpNode* node) {
    write("(");
    if (node->op == "^") {
        write("pow(");
        generate_expression_with_dereference_if_needed(node->left.get());
        write(", ");
        generate_expression_with_dereference_if_needed(node->right.get());
        write(")");
    } else {
        generate_expression_with_dereference_if_needed(node->left.get());
        std::string c_operator;
        if (node->op == "=") c_operator = " == ";
        else if (node->op == "<>") c_operator = " != ";
        else if (node->op == "DIV") c_operator = " / ";
        else if (node->op == "MOD") c_operator = " % ";
        else if (node->op == "AND") c_operator = " && ";
        else if (node->op == "OR") c_operator = " || ";
        else c_operator = " " + node->op + " ";

        write(c_operator);
        generate_expression_with_dereference_if_needed(node->right.get()); // Apply to RHS too
    }
    write(")");
}

void CodeGenerator::visit(const UnaryOpNode* node) {
    if (node->op == "NOT") {
        write("!(");
        if (node->operand) {
            generate_expression_with_dereference_if_needed(node->operand.get()); // Apply helper here too
        } else {
            write("/* ERROR: Missing operand for NOT */");
        }
        write(")");
    }
    else {
        write(node->op + "(");
        if (node->operand) {
            generate_expression_with_dereference_if_needed(node->operand.get()); // And here
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

    const SymbolInfo* info = nullptr; // const
    if (symbol_table_ptr) {
        info = symbol_table_ptr->lookupSymbol(node->array_identifier->name);
    }

    node->array_identifier->accept(this);
    write("[");
    // Index expression itself might need dereferencing if it's a parameter
    generate_expression_with_dereference_if_needed(node->index_expression.get());

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

    const SymbolInfo* func_sym_info = nullptr; // const
    if (symbol_table_ptr) {
        func_sym_info = symbol_table_ptr->lookupSymbol(node->function_name->name);
    }

    node->function_name->accept(this);
    write("(");
    for (size_t i = 0; i < node->arguments.size(); ++i) {
        const ExpressionNode* arg_expr_node = node->arguments[i].get(); // const
        if (!arg_expr_node) {
            write("/* ERROR: Null argument */");
        } else {
            bool pass_by_address = false; // This variable is unused, warning was correct.
            if (auto* id_arg = dynamic_cast<const IdentifierNode*>(arg_expr_node)) { // const
                const SymbolInfo* arg_info = nullptr; // const
                if (symbol_table_ptr) {
                     arg_info = symbol_table_ptr->lookupSymbol(id_arg->name);
                }
                // Heuristic for passing address (still needs callee's parameter mode for full correctness)
                if (arg_info && arg_info->type != "string") {
                    if (func_sym_info && func_sym_info->type == "void") { // If callee is a procedure
                        // This logic is highly speculative and needs to be based on callee's parameter list.
                        // For now, assume we'd pass address if it's an identifier to a procedure,
                        // and the formal parameter type (if we knew it) was a pointer.
                        // write("&"); // Placeholder - this needs proper logic
                    }
                }
            }
            // Arguments to functions might themselves be expressions needing dereference
            generate_expression_with_dereference_if_needed(arg_expr_node);
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
    std::vector<const ExpressionNode*> printf_args; // const

    for (size_t i = 0; i < node->expressions.size(); ++i) {
        const ExpressionNode* expr = node->expressions[i].get(); // const
        if (i > 0 && !format_string.empty() && format_string.back() != ' ') {
            format_string += " ";
        }

        if (auto* str_lit = dynamic_cast<const StringLiteralNode*>(expr)) { // const
            std::string val = str_lit->value;
            size_t pos = 0;
            while ((pos = val.find('%', pos)) != std::string::npos) {
                val.replace(pos, 1, "%%");
                pos += 2;
            }
            format_string += val;
        } else {
            std::string an_type = "integer";

            if (auto* ident_node = dynamic_cast<const IdentifierNode*>(expr)) { // const
                const SymbolInfo* info = nullptr; // const
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
                }
            } else if (dynamic_cast<const IntegerLiteralNode*>(expr)) { // const
                an_type = "integer";
            }
            else if (auto* bin_op_node = dynamic_cast<const BinaryOpNode*>(expr)) { // const
                if (bin_op_node->op == "AND" || bin_op_node->op == "OR" ||
                    bin_op_node->op == "=" || bin_op_node->op == "<>" ||
                    bin_op_node->op == "<" || bin_op_node->op == "<=" ||
                    bin_op_node->op == ">" || bin_op_node->op == ">=") {
                    an_type = "boolean";
                } else if (bin_op_node->op == "/" ) {
                     an_type = "real";
                } else {
                    an_type = "integer";
                }
            } else if (auto* unary_op_node = dynamic_cast<const UnaryOpNode*>(expr)) { // const
                if (unary_op_node->op == "NOT") {
                    an_type = "boolean";
                } else {
                    an_type = "integer";
                }
            }

            if (an_type == "real") format_string += "%lf";
            else if (an_type == "character") format_string += "%c";
            else if (an_type == "boolean") format_string += "%d";
            else if (an_type == "string") format_string += "%s";
            else format_string += "%d";

            printf_args.push_back(expr);
        }
    }

    if (!node->omit_newline) {
        format_string += "\\n";
    }

    write("\"" + format_string + "\"");

    for (const ExpressionNode* arg_expr : printf_args) { // const
        write(", ");
        generate_expression_with_dereference_if_needed(arg_expr);
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
        const auto& var_ident_node = node->variables[i]; // This is unique_ptr<IdentifierNode>, not const IdentifierNode*
        if (!var_ident_node) continue;

        std::string var_name = var_ident_node->name;
        std::string an_type = "integer";
        bool type_found = false;

        if (symbol_table_ptr) {
            const SymbolInfo* info = symbol_table_ptr->lookupSymbol(var_name); // const
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

void CodeGenerator::visit(const RecordTypeNode* node) {
    const SymbolInfo* record_sym_info = nullptr; // const
    if (symbol_table_ptr) {
        record_sym_info = symbol_table_ptr->lookupSymbol(node->name);
    }

    if (!record_sym_info || !record_sym_info->is_record_type) {
        writeln("/* ERROR: SymbolInfo for record type '" + node->name + "' not found or not a record type. Cannot generate struct. */");
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
    if (node->pointer_expr) {
        generate_expression_with_dereference_if_needed(node->pointer_expr.get()); // Apply helper
    } else {
        write("/* ERROR: NULL pointer_expr in PointerMemberAccessNode */");
    }
    write(")->");
    if (node->member_name) {
        node->member_name->accept(this);
    } else {
        write("/* ERROR: NULL member_name in PointerMemberAccessNode */");
    }
    // No closing parenthesis here, it's part of the expression structure
}

void CodeGenerator::visit(const NullLiteralNode* node) {
    // Parameter 'node' is unused, but signature must match.
    write("NULL");
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
    writeln();
    writeln();
}

void CodeGenerator::visit(const ConstantDeclarationNode* node) {
    const SymbolInfo* info = nullptr; // const
    if (symbol_table_ptr) {
        info = symbol_table_ptr->lookupSymbol(node->name);
    }

    std::string c_type;
    if (!info) {
        indent_spaces();
        std::string temp_type = "int";
        if (dynamic_cast<const StringLiteralNode*>(node->value.get())) { // const
            temp_type = "char*";
        } else if (dynamic_cast<const NullLiteralNode*>(node->value.get())) { // const
            temp_type = "void*";
        }
        writeln("/* WARNING: SymbolInfo not found for constant: " + node->name + ". Using guessed type " + temp_type + ". */");
        c_type = temp_type;
    } else {
        if (info->is_pointer_type) {
            if (!info->pointed_type.empty()) {
                 if (info->pointed_type == "string") {
                     c_type = map_an_type_to_c(info->pointed_type) + "*";
                 } else {
                    c_type = map_an_type_to_c(info->pointed_type) + "*";
                 }
            } else {
                c_type = "void*";
            }
        } else if (info->type == "pointer") {
            c_type = "void*";
        } else {
            c_type = map_an_type_to_c(info->type);
        }
    }

    indent_spaces();
    write("const " + c_type + " " + node->name + " = ");
    if (node->value) {
        node->value->accept(this);
    } else {
        write("/* ERROR: NULL value_expr in ConstantDeclarationNode */");
    }
    write(";");
    writeln();
}

// Visit methods for new AST Nodes (Pointers and Memory)
void CodeGenerator::visit(const ReferenceNode* node) {
    write("&(");
    if (node->target_expr) {
        // Target of reference might itself be an expression needing deref if it's an OUT param
        generate_expression_with_dereference_if_needed(node->target_expr.get());
    } else {
        write("/* ERROR: NULL target_expr in ReferenceNode */");
    }
    write(")");
}

void CodeGenerator::visit(const DereferenceNode* node) {
    write("(*"); // Dereference operator itself
    if (node->pointer_expr) {
        // The pointer_expr usually shouldn't need further auto-deref by the helper,
        // as it's expected to be a pointer already.
        node->pointer_expr->accept(this);
    } else {
        write("/* ERROR: NULL pointer_expr in DereferenceNode */");
    }
    write(")");
}

void CodeGenerator::visit(const AllocateNode* node) {
    write("malloc(");
    if (node->size_expr) {
        generate_expression_with_dereference_if_needed(node->size_expr.get());
    } else {
        write("/* ERROR: NULL size_expr in AllocateNode */");
    }
    write(")");
}

void CodeGenerator::visit(const ReallocateNode* node) {
    write("realloc(");
    if (node->pointer_expr) {
        generate_expression_with_dereference_if_needed(node->pointer_expr.get());
    } else {
        write("/* ERROR: NULL pointer_expr in ReallocateNode */");
    }
    write(", ");
    if (node->new_size_expr) {
        generate_expression_with_dereference_if_needed(node->new_size_expr.get());
    } else {
        write("/* ERROR: NULL new_size_expr in ReallocateNode */");
    }
    write(")");
}

void CodeGenerator::visit(const DeallocateNode* node) {
    write("free(");
    if (node->pointer_expr) {
        generate_expression_with_dereference_if_needed(node->pointer_expr.get());
    } else {
        write("/* ERROR: NULL pointer_expr in DeallocateNode */");
    }
    write(")");
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
        generate_expression_with_dereference_if_needed(node->control_variable.get());
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
            generate_expression_with_dereference_if_needed(node->value.get());
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
    const SymbolInfo* sym_info = nullptr; // const, unused variable warning was correct
    // ParameterMode mode = node->mode; // This local 'mode' is not needed if using node->mode directly
    std::string an_type = node->param_type;
    std::string c_type_str = map_an_type_to_c(an_type);

    if (node->mode == ParameterMode::OUT || node->mode == ParameterMode::IN_OUT) {
        if (an_type == "string") {
            if (node->mode == ParameterMode::OUT) {
                write("char** " + node->param_name);
            } else {
                write("char* " + node->param_name); // For input/output string, pass char* (caller manages buffer)
            }
        } else {
            write(c_type_str + "* " + node->param_name);
        }
    } else {
        write(c_type_str + " " + node->param_name);
    }
}

void CodeGenerator::visit(const FunctionPrototypeNode* node) {
    indent_spaces();
    write(map_an_type_to_c(node->return_type) + " " + node->func_name + "(");
    if (node->parameters.empty() && node->return_type != "void") { // For functions, if no params, use void
         write("void");
    } else {
        for (size_t i = 0; i < node->parameters.size(); ++i) {
            if (i > 0) write(", ");
            node->parameters[i]->accept(this);
        }
         if (node->parameters.empty() && node->return_type == "void") { // For procedures with no params
            write("void");
        }
    }
    write(");");
    writeln();
}

void CodeGenerator::visit(const SubprogramBodyNode* node) {
    indent_spaces();
    write(map_an_type_to_c(node->prototype->return_type) + " " + node->prototype->func_name + "(");
    for (size_t i = 0; i < node->prototype->parameters.size(); ++i) {
        if (i > 0) write(", ");
        node->prototype->parameters[i]->accept(this); // Relies on FunctionParameterNode::accept
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