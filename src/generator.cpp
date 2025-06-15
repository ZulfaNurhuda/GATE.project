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
    write(map_an_type_to_c(node->var_type) + " " + node->var_name);
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
    // Simple binary operation: (left op right). Parentheses for safety.
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

    node->function_name->accept(this); // Prints the function name
    write("(");
    for (size_t i = 0; i < node->arguments.size(); ++i) {
        if (node->arguments[i]) {
            node->arguments[i]->accept(this);
        } else {
            // This case should ideally be caught by the parser if arguments are mandatory.
            write("/* ERROR: Null argument in function call */");
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
    std::vector<ExpressionNode*> printf_args; // Store expressions that need to be arguments to printf

    for (size_t i = 0; i < node->expressions.size(); ++i) {
        ExpressionNode* expr = node->expressions[i].get();
        if (i > 0 && !format_string.empty() && format_string.back() != ' ') {
             // Add a space between format specifiers/literals unless previous was empty or ended with space
            format_string += " ";
        }

        if (auto* str_lit = dynamic_cast<StringLiteralNode*>(expr)) {
            // String literals are embedded directly into the format string
            // Basic sanitization for '%' in string literal to avoid printf format string vulnerability.
            std::string val = str_lit->value;
            size_t pos = 0;
            while ((pos = val.find('%', pos)) != std::string::npos) {
                val.replace(pos, 1, "%%");
                pos += 2; // Move past the "%%"
            }
            format_string += val;
        } else {
            // For other expression types, determine type and add format specifier
            std::string an_type = "integer"; // Default assumption

            if (auto* ident_node = dynamic_cast<IdentifierNode*>(expr)) {
                SymbolInfo* info = nullptr;
                if (symbol_table_ptr) { // Check if symbol_table_ptr is valid
                    info = symbol_table_ptr->lookupSymbol(ident_node->name);
                }
                if (info) {
                    an_type = info->type;
                    // writeln("/* Identifier: " + ident_node->name + ", Found Type: " + an_type + " */");
                } else {
                    // Fallback placeholder logic if symbol not found or table not available
                    // writeln("/* Identifier: " + ident_node->name + ", Type NOT FOUND, using placeholder. */");
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


            if (an_type == "real") format_string += "%f";
            else if (an_type == "character") format_string += "%c";
            else if (an_type == "boolean") format_string += "%d"; // Output bool as 0 or 1
            else if (an_type == "string") format_string += "%s";
            else format_string += "%d"; // Default for "integer" and others

            printf_args.push_back(expr);
        }
    }
    // Conditionally add newline character to the format string
    if (!node->omit_newline) {
        format_string += "\\n";
    }

    write("\"" + format_string + "\"");

    for (ExpressionNode* arg_expr : printf_args) {
        write(", ");
        arg_expr->accept(this);
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