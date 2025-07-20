#include "notal_transpiler/CodeGenerator.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <iomanip>

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

    // Separate different kinds of declarations
    std::vector<std::shared_ptr<ast::Stmt>> constDecls;
    std::vector<std::shared_ptr<ast::Stmt>> typeDecls;
    std::vector<std::shared_ptr<ast::Stmt>> varDecls;
    std::vector<std::shared_ptr<ast::Stmt>> constrainedVarDecls;

    for (const auto& decl : stmt->declarations) {
        if (std::dynamic_pointer_cast<ast::ConstDeclStmt>(decl)) {
            constDecls.push_back(decl);
        } else if (std::dynamic_pointer_cast<ast::RecordTypeDeclStmt>(decl) || 
                   std::dynamic_pointer_cast<ast::EnumTypeDeclStmt>(decl)) {
            typeDecls.push_back(decl);
        } else if (std::dynamic_pointer_cast<ast::ConstrainedVarDeclStmt>(decl)) {
            constrainedVarDecls.push_back(decl);
        } else {
            varDecls.push_back(decl);
        }
    }

    // Output type declarations first
    if (!typeDecls.empty()) {
        out << "type\n";
        indentLevel++;
        for (const auto& decl : typeDecls) {
            execute(decl);
        }
        indentLevel--;
        out << "\n";
    }

    // Output const declarations
    if (!constDecls.empty()) {
        out << "const\n";
        indentLevel++;
        for (const auto& decl : constDecls) {
            execute(decl);
        }
        indentLevel--;
        out << "\n";
    }

    // Output var declarations and constrained vars
    if (!varDecls.empty() || !constrainedVarDecls.empty()) {
        out << "var\n";
        indentLevel++;
        
        // Regular variables first
        for (const auto& decl : varDecls) {
            indent();
            execute(decl);
            out << ";\n";
        }
        
        // Constrained variables
        for (const auto& decl : constrainedVarDecls) {
            indent();
            execute(decl);
            out << ";\n";
        }
        
        indentLevel--;
        out << "\n";
    }

    // Generate setter procedures for constrained variables
    if (!constrainedVarDecls.empty()) {
        for (const auto& decl : constrainedVarDecls) {
            auto constrainedVar = std::dynamic_pointer_cast<ast::ConstrainedVarDeclStmt>(decl);
            if (constrainedVar) {
                // Generate setter procedure
                out << "procedure Set" << constrainedVar->name.lexeme << "(var " << constrainedVar->name.lexeme << ": " << pascalType(constrainedVar->type) << "; value: " << pascalType(constrainedVar->type) << ");\n";
                out << "begin\n";
                indentLevel++;
                indent();
                out << "Assert((" << evaluate(constrainedVar->constraint) << "), 'Error: " << constrainedVar->name.lexeme << " constraint violation!');\n";
                indent();
                out << constrainedVar->name.lexeme << " := value;\n";
                indentLevel--;
                out << "end;\n\n";
            }
        }
    }

    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::VarDeclStmt> stmt) {
    out << stmt->name.lexeme << ": " << pascalType(stmt->type);
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::ConstDeclStmt> stmt) {
    auto literal = std::dynamic_pointer_cast<ast::Literal>(stmt->initializer);
    if (literal) {
        constants[stmt->name.lexeme] = literal;
    }
    indent();
    out << stmt->name.lexeme << " = " << evaluate(stmt->initializer) << ";" << std::endl;
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::InputStmt> stmt) {
    out << "readln(" << stmt->variable->name.lexeme << ");" << std::endl;
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
        if (auto varExpr = std::dynamic_pointer_cast<ast::Variable>(stmt->expressions[i])) {
            auto it = constants.find(varExpr->name.lexeme);
            if (it != constants.end()) {
                out << varExpr->name.lexeme; // Output constant name
            } else {
                out << evaluate(stmt->expressions[i]); // Evaluate as usual
            }
        } else {
            out << evaluate(stmt->expressions[i]); // Evaluate as usual
        }
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
        std::string s = std::to_string(std::any_cast<double>(expr->value));
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (s.back() == '.') {
            s.pop_back();
        }
        return s;
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