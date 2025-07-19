#include "notal_transpiler/CodeGenerator.h"
#include <stdexcept>
#include <iostream>

namespace notal {

std::string CodeGenerator::generate(std::shared_ptr<ast::ProgramStmt> program) {
    if (!program) return "";
    execute(program);
    return out.str();
}

void CodeGenerator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        out << "  ";
    }
}

std::string CodeGenerator::pascalType(const Token& token) {
    switch (token.type) {
        case TokenType::INTEGER: return "integer";
        case TokenType::REAL: return "real";
        case TokenType::STRING: return "string";
        case TokenType::BOOLEAN: return "boolean";
        case TokenType::CHARACTER: return "char";
        default: throw std::runtime_error("Unknown type for code generation: " + token.lexeme);
    }
}

std::string CodeGenerator::evaluate(std::shared_ptr<ast::Expr> expr) {
    auto result = expr->accept(*this);
    return std::any_cast<std::string>(result);
}

void CodeGenerator::execute(std::shared_ptr<ast::Stmt> stmt) {
    stmt->accept(*this);
}

// --- Statement Visitors ---

std::any CodeGenerator::visit(std::shared_ptr<ast::ProgramStmt> stmt) {
    out << "program " << stmt->name.lexeme << ";\n\n";
    execute(stmt->kamus);
    execute(stmt->algoritma);
    out << ".\n";
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::KamusStmt> stmt) {
    if (stmt->declarations.empty()) return {};
    
    out << "var\n";
    indentLevel++;
    for (const auto& decl : stmt->declarations) {
        indent();
        execute(decl);
        out << ";\n";
    }
    indentLevel--;
    out << "\n";
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::VarDeclStmt> stmt) {
    out << stmt->name.lexeme << ": " << pascalType(stmt->type);
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::AlgoritmaStmt> stmt) {
    out << "begin\n";
    indentLevel++;
    execute(stmt->body);
    indentLevel--;
    out << "end";
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::BlockStmt> stmt) {
    for (const auto& statement : stmt->statements) {
        indent();
        execute(statement);
    }
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::ExpressionStmt> stmt) {
    out << evaluate(stmt->expression) << ";\n";
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::OutputStmt> stmt) {
    out << "writeln(";
    for (size_t i = 0; i < stmt->expressions.size(); ++i) {
        out << evaluate(stmt->expressions[i]);
        if (i < stmt->expressions.size() - 1) {
            out << ", ";
        }
    }
    out << ");\n";
    return {};
}


// --- Expression Visitors ---

std::any CodeGenerator::visit(std::shared_ptr<ast::Assign> expr) {
    return expr->name.lexeme + " := " + evaluate(expr->value);
}

std::any CodeGenerator::visit(std::shared_ptr<ast::Binary> expr) {
    std::string op = expr->op.lexeme;
    if (expr->op.type == TokenType::AMPERSAND) {
        op = "+"; // String concatenation
    }
    return "(" + evaluate(expr->left) + " " + op + " " + evaluate(expr->right) + ")";
}

std::any CodeGenerator::visit(std::shared_ptr<ast::Unary> expr) {
    return "(" + expr->op.lexeme + evaluate(expr->right) + ")";
}

std::any CodeGenerator::visit(std::shared_ptr<ast::Grouping> expr) {
    return evaluate(expr->expression);
}

std::any CodeGenerator::visit(std::shared_ptr<ast::Variable> expr) {
    return std::string(expr->name.lexeme);
}

std::any CodeGenerator::visit(std::shared_ptr<ast::Literal> expr) {
    if (expr->value.type() == typeid(int)) {
        return std::to_string(std::any_cast<int>(expr->value));
    }
    if (expr->value.type() == typeid(double)) {
        return std::to_string(std::any_cast<double>(expr->value));
    }
    if (expr->value.type() == typeid(bool)) {
        return std::any_cast<bool>(expr->value) ? std::string("true") : std::string("false");
    }
    if (expr->value.type() == typeid(std::string)) {
        return "'" + std::any_cast<std::string>(expr->value) + "'";
    }
    return std::string("null");
}

// --- Unimplemented Visitors (for now) ---

std::any CodeGenerator::visit(std::shared_ptr<ast::IfStmt> stmt) {
    out << "if " << evaluate(stmt->condition) << " then\n";
    indent();
    out << "begin\n";
    indentLevel++;
    execute(stmt->thenBranch);
    indentLevel--;
    indent();
    out << "end";

    if (stmt->elseBranch != nullptr) {
        out << "\n";
        indent();
        
        if (auto elseIf = std::dynamic_pointer_cast<ast::IfStmt>(stmt->elseBranch)) {
            out << "else ";
            execute(elseIf);
        } else {
            out << "else\n";
            indent();
            out << "begin\n";
            indentLevel++;
            execute(stmt->elseBranch);
            indentLevel--;
            indent();
            out << "end;\n";
        }
    } else {
        out << ";\n";
    }
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::WhileStmt> stmt) {
    out << "while " << evaluate(stmt->condition) << " do\n";
    indentLevel++;
    indent();
    out << "begin\n";
    indentLevel++;
    execute(stmt->body); // BlockStmt handles its own indentation and newlines
    indentLevel--;
    indent();
    out << "end;\n"; // Add semicolon and newline after the block
    indentLevel--;
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::RepeatUntilStmt> stmt) {
    out << "repeat\n";
    indentLevel++;
    execute(stmt->body);
    indentLevel--;
    indent();
    out << "until " << evaluate(stmt->condition) << ";\n";
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::DependOnStmt> stmt) {
    // Check if all cases are literals to decide between 'case' and 'if-elif'
    bool allLiterals = true;
    for (const auto& caseItem : stmt->cases) {
        for (const auto& cond : caseItem.conditions) {
            if (std::dynamic_pointer_cast<ast::Literal>(cond) == nullptr) {
                allLiterals = false;
                break;
            }
        }
        if (!allLiterals) break;
    }

    if (allLiterals) {
        // Generate a 'case' statement
        out << "case " << evaluate(stmt->expression) << " of\n";
        indentLevel++;
        for (const auto& caseItem : stmt->cases) {
            indent();
            for (size_t i = 0; i < caseItem.conditions.size(); ++i) {
                out << evaluate(caseItem.conditions[i]);
                if (i < caseItem.conditions.size() - 1) {
                    out << ", ";
                }
            }
            out << ":\n";
            indentLevel++;
            indent();
            out << "begin\n";
            indentLevel++;
            execute(caseItem.body);
            indentLevel--;
            indent();
            out << "end;\n";
            indentLevel--;
        }
        if (stmt->otherwiseBranch) {
            indent();
            out << "else\n";
            indentLevel++;
            indent();
            out << "begin\n";
            indentLevel++;
            execute(stmt->otherwiseBranch);
            indentLevel--;
            indent();
            out << "end;\n";
            indentLevel--;
        }
        indentLevel--;
        indent();
        out << "end;\n";
    } else {
        // Generate an 'if-elif-else' chain
        for (size_t i = 0; i < stmt->cases.size(); ++i) {
            if (i > 0) {
                out << "else ";
            }
            out << "if (";
            for (size_t j = 0; j < stmt->cases[i].conditions.size(); ++j) {
                out << evaluate(stmt->expression) << " = " << evaluate(stmt->cases[i].conditions[j]);
                if (j < stmt->cases[i].conditions.size() - 1) {
                    out << " or ";
                }
            }
            out << ") then\n";
            indent();
            out << "begin\n";
            indentLevel++;
            execute(stmt->cases[i].body);
            indentLevel--;
            indent();
            out << "end";
            if (i < stmt->cases.size() - 1 || stmt->otherwiseBranch) {
                out << "\n";
                indent();
            } else {
                out << ";\n";
            }
        }

        if (stmt->otherwiseBranch) {
            out << "else\n";
            indent();
            out << "begin\n";
            indentLevel++;
            execute(stmt->otherwiseBranch);
            indentLevel--;
            indent();
            out << "end;\n";
        }
    }

    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::Call> expr) {
    (void)expr; // Suppress unused parameter warning
    return std::string("{ call expression not implemented }");
}

} // namespace notal