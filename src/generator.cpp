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

void CodeGenerator::visit(ProgramNode* node) {
    writeln("#include <stdio.h>");
    writeln("#include <stdbool.h>"); // For bool type
    writeln("#include <string.h>");  // For string operations if needed (e.g. strcpy for char*)
    writeln("#include <stdlib.h>");  // For malloc/free if using dynamic char* for strings
    writeln("#include <math.h>");    // Added for pow() function

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
    if (symbol_table_ptr) {
        SymbolInfo* info = symbol_table_ptr->lookupSymbol(node->var_name);
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

            // If it's an array of strings (char*), malloc might be needed for each element,
            // but that's more complex than typical AN array declarations handled this way.
            // Usually, char* arrays are arrays of pointers, each needing malloc.
            // For now, this declares `char* arr_name[size];` which is an array of pointers.
            // If AN implies `array of string` means `array of char[MAX_STR_LEN]`, C gen changes.
            // The current `map_an_type_to_c("string")` returns "char*", so this is array of char pointers.
            return;
        }
    }
    // Fallback to existing simple type declaration logic or if symbol_info not found
    indent_spaces();
    // node->var_type from VariableDeclarationNode might be "array of integer" or just "integer".
    // If it's "array of ...", map_an_type_to_c might not handle it well.
    // map_an_type_to_c expects basic AN types.
    // The SymbolInfo lookup is the more reliable way to get base C type for arrays.
    // If we reach here for an array, it means SymbolInfo lookup failed or it wasn't marked as array.
    // For non-array types, node->var_type should be the simple AN type.
    // This is now refined to handle pointer types explicitly.
    SymbolInfo* info = nullptr; // Re-lookup, as the one in array block is out of scope
    if (symbol_table_ptr) {
        info = symbol_table_ptr->lookupSymbol(node->var_name);
    }

    if (info && info->is_pointer_type) {
        std::string c_base_type = map_an_type_to_c(info->pointed_type);
        indent_spaces();
        if (info->pointed_type == "string") { // NOTAL: pointer to string -> C: char**
            write("char* *" + node->var_name + "; /* pointer to string */");
        } else {
            write(c_base_type + "* " + node->var_name + "; /* pointer to " + info->pointed_type + " */");
        }
        writeln();
    } else if (!(info && info->is_array)) { // If not a pointer and not an array (array handled above)
        indent_spaces();
        write(map_an_type_to_c(node->var_type) + " " + node->var_name);
        writeln(";");
    }
    // Array case is already handled and returns earlier in the function.
}

void CodeGenerator::visit(AssignmentNode* node) {
    indent_spaces();

    if (auto* target_id_node = dynamic_cast<IdentifierNode*>(node->target.get())) {
        SymbolInfo* target_sym_info = nullptr;
        if (symbol_table_ptr) {
            target_sym_info = symbol_table_ptr->lookupSymbol(target_id_node->name);
        }

        if (target_sym_info && target_sym_info->kind == "parameter" &&
            (target_sym_info->param_mode == ParameterMode::OUT || target_sym_info->param_mode == ParameterMode::IN_OUT)) {

            if (target_sym_info->type == "string" && target_sym_info->param_mode == ParameterMode::OUT) {
                // Special handling for: output string param (char**)
                // if (*param_name) free(*param_name); *param_name = strdup(value_expr);
                write("if (*");
                target_id_node->accept(this); // Writes param_name
                write(") free(*");
                target_id_node->accept(this);
                writeln(");");
                indent_spaces(); // New line for the assignment part
                write("*");
                target_id_node->accept(this);
                write(" = strdup(");
                node->value->accept(this); // Generate RHS (string expression)
                write(")");
            } else {
                // Other pointer types (int*, double*, char* for IN_OUT string)
                write("*");
                target_id_node->accept(this); // Writes *param_name
                write(" = ");
                node->value->accept(this); // Generate RHS
            }
        } else {
            // Not an OUT/IN_OUT parameter, or symbol info not found
            target_id_node->accept(this); // Writes param_name
            write(" = ");
            node->value->accept(this); // Generate RHS
        }
    } else {
        // Target is not a simple identifier (e.g., array access)
        node->target->accept(this);
        write(" = ");
        node->value->accept(this);
    }
    writeln(";");
}

void CodeGenerator::visit(IdentifierNode* node) {
    // Default behavior: just write the name.
    // Contextual dereferencing will be handled by parent nodes like BinaryOpNode, OutputNode, AssignmentNode (for RHS).
    write(node->name);
}

void CodeGenerator::visit(IntegerLiteralNode* node) {
    write(std::to_string(node->value));
}

void CodeGenerator::visit(StringLiteralNode* node) {
    // Basic C string literal. Need to handle escapes within node->value if complex.
    write("\"" + node->value + "\"");
}

// Helper function for expression generation with potential dereference
void CodeGenerator::generate_expression_with_dereference_if_needed(ExpressionNode* expr_node) {
    if (auto* id_node = dynamic_cast<IdentifierNode*>(expr_node)) {
        SymbolInfo* sym_info = nullptr;
        if (symbol_table_ptr) {
            sym_info = symbol_table_ptr->lookupSymbol(id_node->name);
        }
        if (sym_info && sym_info->kind == "parameter" &&
            (sym_info->param_mode == ParameterMode::OUT || sym_info->param_mode == ParameterMode::IN_OUT)) {

            if (sym_info->type == "string" && sym_info->param_mode == ParameterMode::OUT) {
                write("(*"); // e.g. (*str_param_out)
                id_node->accept(this);
                write(")");
            } else {
                 write("(*"); // e.g. (*int_param)
                id_node->accept(this);
                write(")");
            }
            return;
        }
    }
    // Default: visit the node as is
    expr_node->accept(this);
}


void CodeGenerator::visit(BinaryOpNode* node) {
    // Simple binary operation: (left op right). Parentheses for safety.
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
        // Mapping AN operators to C operators
        if (node->op == "=") c_operator = " == ";
        else if (node->op == "<>") c_operator = " != ";
        else if (node->op == "DIV") c_operator = " / ";    // Integer division
        else if (node->op == "MOD") c_operator = " % ";    // Modulo
        else if (node->op == "AND") c_operator = " && ";   // Logical AND
        else if (node->op == "OR") c_operator = " || ";    // Logical OR
        // NOT is a unary operator, would need a UnaryOpNode or special handling if represented in BinaryOpNode
        // For direct C operators like +, -, *, /, <, >, <=, >=, they can be used if node->op stores them directly.
        // Assuming the parser stores these common ones directly as C-compatible strings in node->op.
        else c_operator = " " + node->op + " ";

        write(c_operator);
        node->right->accept(this);
    }
    write(")");
}

void CodeGenerator::visit(UnaryOpNode* node) {
    if (node->op == "NOT") {
        write("!("); // C logical NOT operator
        if (node->operand) {
            node->operand->accept(this);
        } else {
            write("/* ERROR: Missing operand for NOT */");
        }
        write(")");
    }
    // Example for unary minus (if parser creates UnaryOpNode with "-" op)
    // else if (node->op == "-") {
    //     write("-(");
    //     if (node->operand) {
    //         node->operand->accept(this);
    //     } else {
    //         write("/* ERROR: Missing operand for unary - */");
    //     }
    //     write(")");
    // }
    else {
        // Default or error for unknown unary operator
        // Write the operator as is, then operand in parentheses for some safety
        write(node->op + "(");
        if (node->operand) {
            node->operand->accept(this);
        } else {
            write("/* ERROR: Missing operand for unary op " + node->op + " */");
        }
        write(")");
    }
}

void CodeGenerator::visit(ArrayAccessNode* node) {
    if (!node || !node->array_identifier || !node->index_expression) {
        write("/* ERROR: Incomplete ArrayAccessNode */");
        return;
    }

    SymbolInfo* info = nullptr;
    if (symbol_table_ptr) {
        info = symbol_table_ptr->lookupSymbol(node->array_identifier->name);
    }

    node->array_identifier->accept(this); // Generates the array name
    write("[");
    node->index_expression->accept(this); // Generates the AN index expression

    if (info && info->is_array && info->array_min_bound != 0) {
        // Apply index mapping to 0-based C array if AN min_bound is not 0
        // E.g., if AN is array[1..10], accessing AN index 1 means C index (1 - 1) = 0
        // E.g., if AN is array[5..10], accessing AN index 5 means C index (5 - 5) = 0
        write(" - " + std::to_string(info->array_min_bound));
    } else if (!info || !info->is_array) {
        // If symbol info not found or it's not an array, generate a comment.
        // This is likely a semantic error that should be caught earlier.
        write(" /* WARN: Array '");
        node->array_identifier->accept(this); // write name again for comment
        write("' not found or not an array in symbol table. Indexing as is. */");
    }
    // If info && info->is_array && info->array_min_bound == 0, no adjustment needed.

    write("]");
}

void CodeGenerator::visit(FunctionCallNode* node) {
    if (!node || !node->function_name) {
        write("/* ERROR: Invalid function call node */");
        return;
    }

    SymbolInfo* func_sym_info = nullptr;
    if (symbol_table_ptr) {
        func_sym_info = symbol_table_ptr->lookupSymbol(node->function_name->name);
    }

    // TODO: This is where we *should* get parameter modes from func_sym_info->parameters
    // As a workaround, we'll check if the argument itself is an OUT/IN_OUT param in the *current* scope.
    // This is NOT fully correct but a placeholder for the complex inter-procedural analysis.

    node->function_name->accept(this);
    write("(");
    for (size_t i = 0; i < node->arguments.size(); ++i) {
        ExpressionNode* arg_expr_node = node->arguments[i].get();
        if (!arg_expr_node) {
            write("/* ERROR: Null argument */");
        } else {
            bool pass_by_address = false;
            // Simplified/heuristic check: if argument is an identifier that is an OUT/IN_OUT param
            // of the *calling* function, pass its address. This is not using the *callee's* signature.
            if (auto* id_arg = dynamic_cast<IdentifierNode*>(arg_expr_node)) {
                SymbolInfo* arg_sym_info = nullptr;
                if (symbol_table_ptr) {
                    arg_sym_info = symbol_table_ptr->lookupSymbol(id_arg->name);
                }
                if (arg_sym_info && arg_sym_info->kind == "parameter" &&
                    (arg_sym_info->param_mode == ParameterMode::OUT || arg_sym_info->param_mode == ParameterMode::IN_OUT)) {

                    // This heuristic is problematic: if `P(output x)` calls `Q(y)`, and `y` is also `output`,
                    // we'd pass `&(*y)` which is not what we want.
                    // The correct logic relies on the CALLEE's parameter mode.
                    // For now, let's assume if it's an IDENTIFIER, we check its mode.
                    // This part needs the callee's signature for full correctness.
                    // Let's assume for this subtask, we are primarily concerned with `&` for non-string OUT/IN_OUT.
                    // If callee expects `int*` and we pass an `int` variable, we use `&var`.
                    // If callee expects `char**` for `output string`, and we pass `char* s`, we use `&s`.
                    // This is hard to do without callee info.

                    // Tentative: if the argument is an IdentifierNode, and it's an OUT/IN_OUT parameter
                    // of the *current* function, it's already a pointer. If the callee expects a pointer
                    // to this (e.g. int** for int*), this simple '&' is not enough.
                    // Given the constraints, this will be very simplified:
                    // We'll rely on the argument types matching what the callee expects *after* potential '&'.
                    // The proper solution for `FunctionCallNode` needs the callee's parameter modes.
                    // For now, we'll assume the parser/user ensures type compatibility.
                    // We will pass address for Identifiers that are not strings and are marked as OUT/IN_OUT
                    // in the current scope (this is still not fully correct).
                    // Or, more simply, always pass address if the callee's formal param (if known) is OUT/IN_OUT.
                    // Since we don't have callee's formal param modes easily:
                    // This is a placeholder for a more robust solution.
                    // For now, we'll assume the subtask focuses on the *signature generation*
                    // and basic use cases for assignment. Calls are harder.
                    // Let's assume a simplified rule: if we pass an identifier that is not a string,
                    // and we *assume* it's for an OUT/IN_OUT parameter of the callee, we'd pass `&`.
                    // This is too speculative.
                    // Sticking to the prompt: "If formal_param_sym_info->param_mode == OUT or IN_OUT: Prepend &"
                    // This requires getting formal_param_sym_info. This is the part that's hard.

                    // Fallback: No special handling for arguments in FunctionCallNode in this pass due to SymbolTable limitations for callee.
                    // The user would have to write `P(&x)` in AN if `x` needs to be passed by address and `P` takes `int*`.
                    // This is not ideal.
                    // Let's try the prompt's request as best as possible, assuming we *could* get formal_param_sym_info.
                    // Since we can't, the `&` will be applied based on the argument's own properties if it's an OUT/IN_OUT param.

                    // If arg is an identifier that is an OUT/IN_OUT param of the current function,
                    // it's already a pointer type (e.g. int*). If passed to another function
                    // that expects int*, it's passed by value. If that function expects int**, then & is needed.
                    // This is where the `SymbolInfo` for the *callee* is essential.
                    // For now, let's try a simplified approach for call sites as requested by the prompt,
                    // *pretending* we have formal_param_sym_info.
                    // This part of the code will be non-functional without the actual callee's parameter modes.
                    // I will make it so it only adds '&' if the argument is a plain variable, not already a pointer.

                    // Simplified logic: If the argument is an identifier, and we assume it's being passed to an OUT/IN_OUT parameter
                    // (and it's not a string being passed to char** OUT), then take its address.
                    // This is still a guess. The most direct interpretation of the prompt is to assume we have formal_param_mode.
                    // Lacking that, I will only add '&' if the argument is an IdentifierNode.
                    // This is a known simplification due to SymbolTable structure.
                    if (dynamic_cast<IdentifierNode*>(arg_expr_node) /* && formal_param_mode is OUT/IN_OUT for non-string */ ) {
                         // This part cannot be fully implemented correctly without callee's parameter modes.
                         // For now, we'll assume if it's an identifier, and the (hypothetical) formal param is by ref, we add &.
                         // This is a placeholder for where the logic would go.
                         // write("&"); // Tentatively, always pass address if it's an identifier to a procedure.
                                      // This is a very broad assumption.
                                      // Awaiting clarification or SymbolTable enhancement.
                         // For now, let's proceed with the general structure and refine if FunctionCallNode is too complex.
                         // The prompt implies we should try to implement it.
                         // Let's assume we need to pass the address of identifiers for OUT/IN_OUT.
                         // This is a common C pattern: procedure(&var);
                         // We'll assume this for non-string types. Strings are already pointers.
                        SymbolInfo* arg_info = symbol_table_ptr->lookupSymbol(static_cast<IdentifierNode*>(arg_expr_node)->name);
                        if (arg_info && arg_info->type != "string") { // Simplistic: pass address of non-string identifiers for now
                             // This is a placeholder for the logic that would use the *callee's* parameter mode.
                             // As per prompt: "If formal_param_sym_info->param_mode == OUT or IN_OUT: Prepend &"
                             // Since we cannot get formal_param_sym_info accurately, this is a HACK/SIMPLIFICATION.
                             // Let's assume for now: if the argument is an identifier and the callee is a procedure,
                             // and the type is not string, pass by address.
                             if (func_sym_info && func_sym_info->type == "void") { // Heuristic: it's a procedure
                                write("&");
                             }
                        }
                    }
                }
            }
            // Default generation for the argument
            arg_expr_node->accept(this);
        }
        if (i < node->arguments.size() - 1) {
            write(", ");
        }
    }
    write(")");
}

void CodeGenerator::visit(OutputNode* node) {
    indent_spaces();
    write("printf(");

    std::string format_string = "";
    std::vector<ExpressionNode*> printf_args;

    for (size_t i = 0; i < node->expressions.size(); ++i) {
        ExpressionNode* expr = node->expressions[i].get();
        if (i > 0 && !format_string.empty() && format_string.back() != ' ') {
            format_string += " ";
        }

        if (auto* str_lit = dynamic_cast<StringLiteralNode*>(expr)) {
            std::string val = str_lit->value;
            size_t pos = 0;
            while ((pos = val.find('%', pos)) != std::string::npos) {
                val.replace(pos, 1, "%%");
                pos += 2; // Move past the "%%"
            }
            format_string += val;
        } else {
            std::string val = str_lit->value;
            size_t pos = 0;
            while ((pos = val.find('%', pos)) != std::string::npos) {
                val.replace(pos, 1, "%%");
                pos += 2;
            }
            format_string += val;
        } else {
            std::string an_type = "integer";
            bool is_dereferenced_param = false;

            if (auto* ident_node = dynamic_cast<IdentifierNode*>(expr)) {
                SymbolInfo* info = nullptr;
                if (symbol_table_ptr) {
                    info = symbol_table_ptr->lookupSymbol(ident_node->name);
                }
                if (info) {
                    an_type = info->type;
                    if (info->kind == "parameter" && (info->param_mode == ParameterMode::OUT || info->param_mode == ParameterMode::IN_OUT)) {
                        is_dereferenced_param = true;
                        if (info->type == "string" && info->param_mode == ParameterMode::OUT) {
                            // This is char**, printf needs char*
                            // format_string += "%s"; // Already handled by an_type = "string" below
                        }
                    }
                } else {
                    if (ident_node->name.rfind("r_", 0) == 0) an_type = "real";
                    else if (ident_node->name.rfind("c_", 0) == 0) an_type = "character";
                    else if (ident_node->name.rfind("b_", 0) == 0) an_type = "boolean";
                    else if (ident_node->name.rfind("s_", 0) == 0) an_type = "string";
                    else an_type = "integer"; // Default for other identifiers for now
                }
            } else if (dynamic_cast<IntegerLiteralNode*>(expr)) {
                an_type = "integer";
            }
            // Add RealLiteralNode, CharLiteralNode, BoolLiteralNode if they get created by parser
            // else if (dynamic_cast<RealLiteralNode*>(expr)) an_type = "real";
            // else if (dynamic_cast<CharLiteralNode*>(expr)) an_type = "character";
            // else if (dynamic_cast<BooleanLiteralNode*>(expr)) an_type = "boolean";
            else if (auto* bin_op_node = dynamic_cast<BinaryOpNode*>(expr)) {
                // TODO: Type propagation for binary operations
                // For now, make simple assumptions:
                if (bin_op_node->op == "AND" || bin_op_node->op == "OR" ||
                    bin_op_node->op == "=" || bin_op_node->op == "<>" ||
                    bin_op_node->op == "<" || bin_op_node->op == "<=" ||
                    bin_op_node->op == ">" || bin_op_node->op == ">=") {
                    an_type = "boolean";
                } else if (bin_op_node->op == "/" ) { // Real division
                     an_type = "real"; // Assuming '/' is float division
                }
                 else { // Default to integer for other ops like +, -, *, DIV, MOD, ^
                    an_type = "integer";
                }
                // writeln("/* BinaryOp: " + bin_op_node->op + ", Assumed AN Type: " + an_type + " */");
            } else if (auto* unary_op_node = dynamic_cast<UnaryOpNode*>(expr)) {
                // TODO: Type propagation for unary operations
                if (unary_op_node->op == "NOT") {
                    an_type = "boolean";
                } else {
                    an_type = "integer"; // Assuming unary minus results in integer/real
                }
                // writeln("/* UnaryOp: " + unary_op_node->op + ", Assumed AN Type: " + an_type + " */");
            }


            if (an_type == "real") format_string += "%lf"; // Use %lf for double precision for printf
            else if (an_type == "character") format_string += "%c";
            else if (an_type == "boolean") format_string += "%d";
            else if (an_type == "string") format_string += "%s";
            else format_string += "%d"; // Default for "integer"

            printf_args.push_back(expr); // Store original expression node
        }
    }

    if (!node->omit_newline) {
        format_string += "\\n";
    }

    write("\"" + format_string + "\"");

    for (ExpressionNode* arg_expr : printf_args) {
        write(", ");
        // Use the helper for arguments to handle potential dereferences
        generate_expression_with_dereference_if_needed(arg_expr);
    }
    writeln(");");
}

void CodeGenerator::visit(InputNode* node) {
    indent_spaces();
    // For "input(var1, var2, ...)", we construct a single scanf call.
    // scanf requires knowing the type of each variable to use the correct format specifier.

    if (!node || node->variables.empty()) {
        // Should not happen if parser validates, but good to be safe.
        // writeln("/* INPUT node has no variables. */");
        return;
    }

    std::string format_string = "";
    std::string scanf_args_string = "";

    for (size_t i = 0; i < node->variables.size(); ++i) {
        const auto& var_ident_node = node->variables[i];
        if (!var_ident_node) continue; // Should not happen

        std::string var_name = var_ident_node->name;
        std::string an_type = "integer"; // Default if type not found or error
        bool type_found = false;

        if (symbol_table_ptr) {
            SymbolInfo* info = symbol_table_ptr->lookupSymbol(var_name);
            if (info) {
                an_type = info->type;
                type_found = true;
            }
        }

        if (!type_found) {
            // Handle undeclared identifier for input - this is a semantic error.
            // Generator might output a comment and assume a default or skip.
            // For now, let's assume string input as a fallback and add a comment.
            format_string += "%s /* UD_ID: " + var_name + " */";
            scanf_args_string += ", " + var_name; // No & for string with %s
            // Consider adding an error message to C output or via ErrorHandler if available
            // write("/* Error: Undeclared identifier '" + var_name + "' used in INPUT. */ ");
            continue;
        }

        // Add space between format specifiers if not the first one.
        // scanf is generally robust with whitespace, but explicit spaces for multiple items are fine.
        if (i > 0) {
            format_string += " ";
        }

        if (an_type == "integer") {
            format_string += "%d";
            scanf_args_string += ", &" + var_name;
        } else if (an_type == "real") {
            format_string += "%lf"; // %lf for double with scanf
            scanf_args_string += ", &" + var_name;
        } else if (an_type == "character") {
            format_string += " %c"; // Note the leading space to consume potential leftover newline
            scanf_args_string += ", &" + var_name;
        } else if (an_type == "string") {
            // For reading strings with scanf, ensure buffer is large enough.
            // Assuming char* variables are declared as arrays or malloc'd.
            // Using %99s (or similar) is a common way to limit input to prevent overflow if buffer is fixed.
            // This requires the C variable to be char var_name[100]; or similar.
            format_string += "%99s"; // TODO: Add comment about pre-allocated buffer for string
            scanf_args_string += ", " + var_name; // No & for char array/pointer with %s
        } else if (an_type == "boolean") {
            format_string += "%d"; // Read boolean as an integer (0 or 1)
            scanf_args_string += ", &" + var_name; // TODO: Add comment assuming bool var handles int input
        } else {
            // Fallback for unknown AN types - treat as string input for robustness
            format_string += "%s /* UnknownType: " + an_type + " for " + var_name + " */";
            scanf_args_string += ", " + var_name;
        }
    }

    write("scanf(\"" + format_string + "\"" + scanf_args_string + ");");
    writeln(""); // End the scanf line
    // It's good practice to check scanf's return value in C to ensure all items were read.
    // writeln("int items_read = scanf(...);");
    // writeln("if (items_read != " + std::to_string(node->variables.size()) + ") { /* Handle input error */ }");
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

// New visit methods for pointer access, null, enum, constant
void CodeGenerator::visit(PointerMemberAccessNode* node) {
    write("(");
    if (node->pointer_expr) {
        node->pointer_expr->accept(this);
    } else {
        write("/* ERROR: NULL pointer_expr in PointerMemberAccessNode */");
    }
    write(")->");
    if (node->member_name) {
        node->member_name->accept(this); // This will just write the name
    } else {
        write("/* ERROR: NULL member_name in PointerMemberAccessNode */");
    }
    write(")");
}

void CodeGenerator::visit(NullLiteralNode* node) {
    // The NullLiteralNode itself doesn't carry a type, but the context might.
    // For general C, NULL is often ((void*)0).
    write("NULL");
}

void CodeGenerator::visit(EnumTypeNode* node) {
    indent_spaces();
    write("typedef enum { ");
    for (size_t i = 0; i < node->values.size(); ++i) {
        // It's good practice to prefix enum values in C to avoid name clashes,
        // e.g., MyEnum_VAL1. For now, direct translation.
        write(node->values[i]);
        if (i < node->values.size() - 1) {
            write(", ");
        }
    }
    write(" } " + node->name + ";");
    writeln();
    writeln(); // Extra blank line for readability after type definition
}

void CodeGenerator::visit(ConstantDeclarationNode* node) {
    SymbolInfo* info = nullptr;
    if (symbol_table_ptr) {
        info = symbol_table_ptr->lookupSymbol(node->name);
    }

    if (!info) {
        indent_spaces();
        // Fallback if symbol info not found, though parser should ensure this.
        // Try to infer from AST node value if possible as a last resort, or default to "int".
        std::string c_type = "int"; // Default fallback
        if (dynamic_cast<StringLiteralNode*>(node->value.get())) {
            c_type = "char*";
        } else if (dynamic_cast<NullLiteralNode*>(node->value.get())) {
            c_type = "void*"; // For NULL
        }
        // No good way to infer float/double without RealLiteralNode
        writeln("/* WARNING: SymbolInfo not found for constant: " + node->name + ". Using guessed type " + c_type + ". */");
        indent_spaces();
        write("const " + c_type + " " + node->name + " = ");
    } else {
        std::string c_type;
        if (info->is_pointer_type) {
            if (!info->pointed_type.empty()) {
                 if (info->pointed_type == "string") {
                    // const pointer to string literal: const char* NAME = "value";
                    // const pointer to char (if it's a pointer to a single char): const char* NAME = &char_var;
                    // pointer to pointer to char: const char** NAME = ...;
                    // If the constant itself is NULL and type is pointer to string: const char** NAME = NULL;
                    // If the constant value is a string literal and type is pointer to string: (this is unusual for constants)
                    // For `constant MY_PTR_STR : pointer to string = NULL` -> `const char** MY_PTR_STR = NULL;`
                    // For `constant MY_STR_LIT_PTR : pointer to string = reference("some_global_string_literal_var")` -> `const char** ...`
                    // The SymbolInfo.type for a constant NULL was set to "pointer" by parser.
                    // If info.type is "pointer" and info.pointed_type is "string" -> char**
                     c_type = map_an_type_to_c(info->pointed_type) + "*";
                 } else { // pointer to non-string (e.g. integer, real)
                    c_type = map_an_type_to_c(info->pointed_type) + "*";
                 }
            } else { // Generic pointer, e.g. from `MY_NULL = NULL` where type wasn't `pointer to X`
                c_type = "void*"; // `const void* NAME = NULL;` is safe
            }
        } else if (info->type == "pointer") { // Case for `constant X = NULL` where type was inferred as "pointer"
            c_type = "void*";
        }
         else { // Not a pointer type
            c_type = map_an_type_to_c(info->type);
            // If it's a string constant like `constant GREET = "Hello"`, c_type becomes "char*"
            // So `const char* GREET = ...` is correct.
        }
        indent_spaces();
        write("const " + c_type + " " + node->name + " = ");
    }

    if (node->value) {
        node->value->accept(this);
    } else {
        // This case should ideally be prevented by the parser.
        write("/* ERROR: NULL value_expr in ConstantDeclarationNode */");
    }
    write(";");
    writeln();
}

// Visit methods for new AST Nodes (Pointers and Memory)
void CodeGenerator::visit(ReferenceNode* node) {
    write("&(");
    if (node->target_expr) {
        node->target_expr->accept(this);
    } else {
        write("/* ERROR: NULL target_expr in ReferenceNode */");
    }
    write(")");
}

void CodeGenerator::visit(DereferenceNode* node) {
    write("(*(");
    if (node->pointer_expr) {
        node->pointer_expr->accept(this);
    } else {
        write("/* ERROR: NULL pointer_expr in DereferenceNode */");
    }
    write("))");
}

void CodeGenerator::visit(AllocateNode* node) {
    // Assuming result needs a cast in C, e.g. (char*)malloc(size)
    // For simplicity, omitting cast as C allows void* to any pointer.
    // Type information for cast would ideally come from context or type inference.
    write("malloc(");
    if (node->size_expr) {
        node->size_expr->accept(this);
    } else {
        write("/* ERROR: NULL size_expr in AllocateNode */");
    }
    write(")");
}

void CodeGenerator::visit(ReallocateNode* node) {
    write("realloc(");
    if (node->pointer_expr) {
        node->pointer_expr->accept(this);
    } else {
        write("/* ERROR: NULL pointer_expr in ReallocateNode */");
    }
    write(", ");
    if (node->new_size_expr) {
        node->new_size_expr->accept(this);
    } else {
        write("/* ERROR: NULL new_size_expr in ReallocateNode */");
    }
    write(")");
}

void CodeGenerator::visit(DeallocateNode* node) {
    write("free(");
    if (node->pointer_expr) {
        node->pointer_expr->accept(this);
    } else {
        write("/* ERROR: NULL pointer_expr in DeallocateNode */");
    }
    write(")");
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
    // SymbolInfo* var_info = symbol_table_ptr->lookupSymbol(node->loop_variable->name); // Changed
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
    // This visitor is for the AST node. Symbol table should be primary source for type/mode.
    SymbolInfo* sym_info = nullptr;
    if (symbol_table_ptr) {
        // Parameters are tricky for lookup here as scopes change.
        // We rely on the mode stored in the FunctionParameterNode itself, set during parsing.
        // sym_info = symbol_table_ptr->lookupSymbol(node->param_name); // Might not find it in current_scope if called for prototype
    }

    ParameterMode mode = node->mode; // Use mode from AST node
    std::string an_type = node->param_type;
    std::string c_type_str = map_an_type_to_c(an_type);

    if (mode == ParameterMode::OUT || mode == ParameterMode::IN_OUT) {
        if (an_type == "string") {
            if (mode == ParameterMode::OUT) { // output string param_name -> char** param_name
                write("char** " + node->param_name);
            } else { // input/output string param_name -> char* param_name (managed by caller)
                write("char* " + node->param_name);
            }
        } else { // int*, double*, bool*, char*
            write(c_type_str + "* " + node->param_name);
        }
    } else { // ParameterMode::IN
        write(c_type_str + " " + node->param_name);
    }
}

void CodeGenerator::visit(FunctionPrototypeNode* node) {
    indent_spaces();
    write(map_an_type_to_c(node->return_type) + " " + node->func_name + "(");
    for (size_t i = 0; i < node->parameters.size(); ++i) {
        if (i > 0) write(", ");
        // The FunctionParameterNode's accept method is now responsible for generating its C representation
        // including pointer syntax based on its mode.
        node->parameters[i]->accept(this);
    }
    write(");");
    writeln();
}

void CodeGenerator::visit(SubprogramBodyNode* node) {
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
                else if (ident_node->name.rfind("c_", 0) == 0) an_type = "character";
                else if (ident_node->name.rfind("b_", 0) == 0) an_type = "boolean";
                else if (ident_node->name.rfind("s_", 0) == 0) an_type = "string";
                else an_type = "integer"; // Default for other identifiers for now
                // writeln("/* Identifier: " + ident_node->name + ", Assumed AN Type: " + an_type + " */");
            } else if (dynamic_cast<IntegerLiteralNode*>(expr)) {
                an_type = "integer";
            }
            // Add RealLiteralNode, CharLiteralNode, BoolLiteralNode if they get created by parser
            // else if (dynamic_cast<RealLiteralNode*>(expr)) an_type = "real";
            // else if (dynamic_cast<CharLiteralNode*>(expr)) an_type = "character";
            // else if (dynamic_cast<BooleanLiteralNode*>(expr)) an_type = "boolean";
            else if (auto* bin_op_node = dynamic_cast<BinaryOpNode*>(expr)) {
                // TODO: Type propagation for binary operations
                // For now, make simple assumptions:
                if (bin_op_node->op == "AND" || bin_op_node->op == "OR" ||
                    bin_op_node->op == "=" || bin_op_node->op == "<>" ||
                    bin_op_node->op == "<" || bin_op_node->op == "<=" ||
                    bin_op_node->op == ">" || bin_op_node->op == ">=") {
                    an_type = "boolean";
                } else if (bin_op_node->op == "/" ) { // Real division
                     an_type = "real"; // Assuming '/' is float division
                }
                 else { // Default to integer for other ops like +, -, *, DIV, MOD, ^
                    an_type = "integer";
                }
                // writeln("/* BinaryOp: " + bin_op_node->op + ", Assumed AN Type: " + an_type + " */");
            } else if (auto* unary_op_node = dynamic_cast<UnaryOpNode*>(expr)) {
                // TODO: Type propagation for unary operations
                if (unary_op_node->op == "NOT") {
                    an_type = "boolean";
                } else {
                    an_type = "integer"; // Assuming unary minus results in integer/real
                }
                // writeln("/* UnaryOp: " + unary_op_node->op + ", Assumed AN Type: " + an_type + " */");
            }


            if (an_type == "real") format_string += "%f";
            else if (an_type == "character") format_string += "%c";
            else if (an_type == "boolean") format_string += "%d"; // Output bool as 0 or 1
            else if (an_type == "string") format_string += "%s";
            else format_string += "%d"; // Default for "integer" and others

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