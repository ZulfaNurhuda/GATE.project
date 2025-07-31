#include "notal_transpiler/CodeGenerator.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cctype>

namespace notal {

std::string CodeGenerator::generate(std::shared_ptr<ast::ProgramStmt> program) {
    if (!program) return "";
    preScan(program->algoritma);
    execute(program);
    return out.str();
}

void CodeGenerator::preScan(std::shared_ptr<ast::Stmt> stmt) {
    if (!stmt) return;

    if (auto algoritma = std::dynamic_pointer_cast<ast::AlgoritmaStmt>(stmt)) {
        preScan(algoritma->body);
    } else if (auto block = std::dynamic_pointer_cast<ast::BlockStmt>(stmt)) {
        for (const auto& s : block->statements) {
            preScan(s);
        }
    } else if (auto repeat = std::dynamic_pointer_cast<ast::RepeatNTimesStmt>(stmt)) {
        loopVariables.push_back("_loop_iterator_" + std::to_string(loopVariables.size()));
    } else if (auto ifStmt = std::dynamic_pointer_cast<ast::IfStmt>(stmt)) {
        preScan(ifStmt->thenBranch);
        preScan(ifStmt->elseBranch);
    } else if (auto whileStmt = std::dynamic_pointer_cast<ast::WhileStmt>(stmt)) {
        preScan(whileStmt->body);
    } else if (auto traversalStmt = std::dynamic_pointer_cast<ast::TraversalStmt>(stmt)) {
        preScan(traversalStmt->body);
    } else if (auto iterateStmt = std::dynamic_pointer_cast<ast::IterateStopStmt>(stmt)) {
        preScan(iterateStmt->body);
    } else if (auto dependOnStmt = std::dynamic_pointer_cast<ast::DependOnStmt>(stmt)) {
        for (const auto& c : dependOnStmt->cases) {
            preScan(c.body);
        }
        preScan(dependOnStmt->otherwiseBranch);
    }
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
        case TokenType::POINTER: return "^"; // This will be combined with the type
        case TokenType::IDENTIFIER: return token.lexeme; // Custom types use their name directly
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
    if (!stmt->subprograms.empty()) {
        forwardDeclare = true;
        for (const auto& sub : stmt->subprograms) {
            execute(sub);
        }
        forwardDeclare = false;
        out << "\n";
    }
    for (const auto& sub : stmt->subprograms) {
        execute(sub);
        out << "\n";
    }
    execute(stmt->algoritma);
    out << ".\n";
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::KamusStmt> stmt) {
    std::vector<std::shared_ptr<ast::Stmt>> constDecls, typeDecls, varDecls, constrainedVarDecls;
    for (const auto& decl : stmt->declarations) {
        if (std::dynamic_pointer_cast<ast::ProcedureStmt>(decl) || std::dynamic_pointer_cast<ast::FunctionStmt>(decl)) continue;
        if (std::dynamic_pointer_cast<ast::ConstDeclStmt>(decl)) constDecls.push_back(decl);
        else if (std::dynamic_pointer_cast<ast::RecordTypeDeclStmt>(decl) || std::dynamic_pointer_cast<ast::EnumTypeDeclStmt>(decl)) typeDecls.push_back(decl);
        else if (std::dynamic_pointer_cast<ast::ConstrainedVarDeclStmt>(decl)) constrainedVarDecls.push_back(decl);
        else varDecls.push_back(decl);
    }

    if (!typeDecls.empty()) {
        out << "type\n";
        indentLevel++;
        for (const auto& decl : typeDecls) { execute(decl); }
        indentLevel--;
        out << "\n";
    }

    if (!constDecls.empty()) {
        out << "const\n";
        indentLevel++;
        for (const auto& decl : constDecls) { execute(decl); }
        indentLevel--;
        out << "\n";
    }

    if (!varDecls.empty() || !constrainedVarDecls.empty() || !loopVariables.empty()) {
        out << "var\n";
        indentLevel++;
        for (const auto& decl : varDecls) { indent(); execute(decl); out << ";\n"; }
        for (const auto& decl : constrainedVarDecls) {
            auto constrainedVar = std::dynamic_pointer_cast<ast::ConstrainedVarDeclStmt>(decl);
            if (constrainedVar) constrainedVars[constrainedVar->name.lexeme] = constrainedVar;
            indent(); execute(decl); out << ";\n";
        }
        for (const auto& var : loopVariables) { indent(); out << var << ": integer;\n"; }
        indentLevel--;
        out << "\n";
    }

    if (!constrainedVarDecls.empty()) {
        for (const auto& decl : constrainedVarDecls) {
            auto constrainedVar = std::dynamic_pointer_cast<ast::ConstrainedVarDeclStmt>(decl);
            if (constrainedVar) {
                out << "procedure Set" << constrainedVar->name.lexeme << "(var " << constrainedVar->name.lexeme << ": " << pascalType(constrainedVar->type) << "; value: " << pascalType(constrainedVar->type) << ");\n";
                out << "begin\n";
                indentLevel++;
                indent();
                out << "Assert(" << generateConstraintCheck(constrainedVar) << ", 'Error: " << constrainedVar->name.lexeme << " constraint violation!');\n";
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
    out << stmt->name.lexeme << ": ";
    if (stmt->type.type == TokenType::POINTER) {
        out << "^" << pascalType(stmt->pointedToType);
    } else {
        out << pascalType(stmt->type);
    }
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::StaticArrayDeclStmt> stmt) {
    out << stmt->name.lexeme << ": array[";
    for (size_t i = 0; i < stmt->dimensions.size(); ++i) {
        out << evaluate(stmt->dimensions[i].start) << ".." << evaluate(stmt->dimensions[i].end);
        if (i < stmt->dimensions.size() - 1) out << ", ";
    }
    out << "] of " << pascalType(stmt->elementType);
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::DynamicArrayDeclStmt> stmt) {
    dynamicArrayDimensions[stmt->name.lexeme] = stmt->dimensions;
    out << stmt->name.lexeme << ": ";
    for (int i = 0; i < stmt->dimensions; ++i) {
        out << "array of ";
    }
    out << pascalType(stmt->elementType);
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::AllocateStmt> stmt) {
    if (stmt->sizes.empty()) { // Pointer allocation
        out << "New(" << evaluate(stmt->callee) << ");\n";
    } else { // Array allocation
        out << "SetLength(" << evaluate(stmt->callee);
        for (const auto& size : stmt->sizes) {
            out << ", " << evaluate(size);
        }
        out << ");\n";
    }
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::DeallocateStmt> stmt) {
    std::string varName = evaluate(stmt->callee);
    if (stmt->dimension == -1) { // Pointer deallocation
        out << "Dispose(" << varName << ");\n";
    } else { // Array deallocation
        if (dynamicArrayDimensions.find(varName) == dynamicArrayDimensions.end()) {
            throw std::runtime_error("Deallocating an undeclared dynamic array: " + varName);
        }
        int declaredDim = dynamicArrayDimensions[varName];
        if (declaredDim != stmt->dimension) {
             throw std::runtime_error("Deallocation dimension mismatch for '" + varName + "'. Declared: " + std::to_string(declaredDim) + ", Used: " + std::to_string(stmt->dimension));
        }
        out << "SetLength(" << varName;
        for (int i = 0; i < stmt->dimension; ++i) {
            out << ", 0";
        }
        out << ");\n";
    }
    return {};
}


std::any CodeGenerator::visit(std::shared_ptr<ast::ConstDeclStmt> stmt) {
    auto literal = std::dynamic_pointer_cast<ast::Literal>(stmt->initializer);
    if (literal) constants[stmt->name.lexeme] = literal;
    indent();
    out << stmt->name.lexeme << " = " << evaluate(stmt->initializer) << ";\n";
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::InputStmt> stmt) {
    out << "readln(" << stmt->variable->name.lexeme << ");\n";
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::AlgoritmaStmt> stmt) {
    out << "begin\n";
    indentLevel++;
    execute(stmt->body);
    indentLevel--;
    indent();
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
                out << varExpr->name.lexeme;
            } else {
                out << evaluate(stmt->expressions[i]);
            }
        } else {
            out << evaluate(stmt->expressions[i]);
        }
        if (i < stmt->expressions.size() - 1) {
            out << ", ";
        }
    }
    out << ");\n";
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::Assign> expr) {
    std::string targetStr = evaluate(expr->target);

    // Check for constrained variables only if the target is a simple variable
    if (auto var = std::dynamic_pointer_cast<ast::Variable>(expr->target)) {
        if (constrainedVars.find(var->name.lexeme) != constrainedVars.end()) {
            return "Set" + var->name.lexeme + "(" + var->name.lexeme + ", " + evaluate(expr->value) + ")";
        }
    }

    return targetStr + " := " + evaluate(expr->value);
}

std::any CodeGenerator::visit(std::shared_ptr<ast::Binary> expr) {
    if (expr->op.type == TokenType::POWER) {
        std::string base = evaluate(expr->left);
        std::string exponent = evaluate(expr->right);
        return "trunc(exp(" + exponent + " * ln(" + base + ")))";
    }

    std::string op = expr->op.lexeme;
    if (expr->op.type == TokenType::AMPERSAND) op = "+";
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
    if (expr->value.type() == typeid(int)) return std::to_string(std::any_cast<int>(expr->value));
    if (expr->value.type() == typeid(double)) {
        std::string s = std::to_string(std::any_cast<double>(expr->value));
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (s.back() == '.') s.pop_back();
        return s;
    }
    if (expr->value.type() == typeid(bool)) return std::any_cast<bool>(expr->value) ? "true" : "false";
    if (expr->value.type() == typeid(std::string)) return "'" + std::any_cast<std::string>(expr->value) + "'";
    return "null";
}

std::any CodeGenerator::visit(std::shared_ptr<ast::ArrayAccess> expr) {
    std::string result = evaluate(expr->callee) + "[";
    for (size_t i = 0; i < expr->indices.size(); ++i) {
        result += evaluate(expr->indices[i]);
        if (i < expr->indices.size() - 1) {
            result += ", ";
        }
    }
    result += "]";
    return result;
}


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
        out << " else ";
        if (std::dynamic_pointer_cast<ast::IfStmt>(stmt->elseBranch)) {
            execute(stmt->elseBranch);
        } else {
            out << "\n";
            indent();
            out << "begin\n";
            indentLevel++;
            execute(stmt->elseBranch);
            indentLevel--;
            indent();
            out << "end";
        }
    }

    bool isPartOfElif = false;
    if (auto parent = stmt->parent.lock()) {
        if (auto parentIf = std::dynamic_pointer_cast<ast::IfStmt>(parent)) {
            if (parentIf->elseBranch.get() == stmt.get()) isPartOfElif = true;
        }
    }
    if (!isPartOfElif) out << ";\n";
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::WhileStmt> stmt) {
    out << "while " << evaluate(stmt->condition) << " do\n";
    indentLevel++;
    indent();
    out << "begin\n";
    indentLevel++;
    execute(stmt->body);
    indentLevel--;
    indent();
    out << "end;\n";
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
        out << "case " << evaluate(stmt->expression) << " of\n";
        indentLevel++;
        for (const auto& caseItem : stmt->cases) {
            indent();
            for (size_t i = 0; i < caseItem.conditions.size(); ++i) {
                out << evaluate(caseItem.conditions[i]);
                if (i < caseItem.conditions.size() - 1) out << ", ";
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
        for (size_t i = 0; i < stmt->cases.size(); ++i) {
            if (i > 0) out << "else ";
            out << "if (";
            for (size_t j = 0; j < stmt->cases[i].conditions.size(); ++j) {
                out << evaluate(stmt->expression) << " = " << evaluate(stmt->cases[i].conditions[j]);
                if (j < stmt->cases[i].conditions.size() - 1) out << " or ";
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

std::any CodeGenerator::visit(std::shared_ptr<ast::TraversalStmt> stmt) {
    std::string iterator = stmt->iterator.lexeme;
    std::string start = evaluate(stmt->start);
    std::string end = evaluate(stmt->end);
    std::string step = "1";
    if (stmt->step) step = evaluate(stmt->step);

    indent();
    out << iterator << " := " << start << ";\n";
    indent();
    out << "while (" << iterator << " <= " << end << ") do\n";
    indent();
    out << "begin\n";
    indentLevel++;
    execute(stmt->body);
    indent();
    out << "Inc(" << iterator << ", " << step << ");\n";
    indentLevel--;
    indent();
    out << "end;\n";
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::IterateStopStmt> stmt) {
    indent();
    out << "while true do\n";
    indent();
    out << "begin\n";
    indentLevel++;
    execute(stmt->body);
    indent();
    out << "if " << evaluate(stmt->condition) << " then break;\n";
    indentLevel--;
    indent();
    out << "end;\n";
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::RepeatNTimesStmt> stmt) {
    std::string iterator = loopVariables[loopCounter++];
    indent();
    out << "for " << iterator << " := 1 to " << evaluate(stmt->times) << " do\n";
    indent();
    out << "begin\n";
    indentLevel++;
    execute(stmt->body);
    indentLevel--;
    indent();
    out << "end;\n";
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::StopStmt> stmt) {
    (void)stmt;
    out << "break;\n";
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::SkipStmt> stmt) {
    (void)stmt;
    out << "continue;\n";
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::Call> expr) {
    std::string callee = evaluate(expr->callee);
    std::string args;
    for (size_t i = 0; i < expr->arguments.size(); ++i) {
        args += evaluate(expr->arguments[i]);
        if (i < expr->arguments.size() - 1) args += ", ";
    }
    return callee + "(" + args + ")";
}

std::any CodeGenerator::visit(std::shared_ptr<ast::FieldAccess> expr) {
    return evaluate(expr->object) + "." + expr->name.lexeme;
}

std::any CodeGenerator::visit(std::shared_ptr<ast::FieldAssign> expr) {
    return evaluate(expr->target) + " := " + evaluate(expr->value);
}

std::any CodeGenerator::visit(std::shared_ptr<ast::RecordTypeDeclStmt> stmt) {
    indent();
    out << stmt->typeName.lexeme << " = record\n";
    indentLevel++;
    for (const auto& field : stmt->fields) {
        indent();
        out << field.name.lexeme << ": " << pascalType(field.type) << ";\n";
    }
    indentLevel--;
    indent();
    out << "end;\n\n";
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::EnumTypeDeclStmt> stmt) {
    indent();
    out << stmt->typeName.lexeme << " = (";
    for (size_t i = 0; i < stmt->values.size(); ++i) {
        out << stmt->values[i].lexeme;
        if (i < stmt->values.size() - 1) out << ", ";
    }
    out << ");\n";
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::ConstrainedVarDeclStmt> stmt) {
    out << stmt->name.lexeme << ": " << pascalType(stmt->type);
    return {};
}

void CodeGenerator::generateParameterList(const std::vector<ast::Parameter>& params) {
    out << "(";
    for (size_t i = 0; i < params.size(); ++i) {
        const auto& p = params[i];
        switch (p.mode) {
            case ast::ParameterMode::INPUT: break;
            case ast::ParameterMode::OUTPUT:
            case ast::ParameterMode::INPUT_OUTPUT: out << "var "; break;
        }
        out << p.name.lexeme << ": " << pascalType(p.type);
        if (i < params.size() - 1) out << "; ";
    }
    out << ")";
}

std::any CodeGenerator::visit(std::shared_ptr<ast::ProcedureStmt> stmt) {
    if (forwardDeclare) {
        indent();
        out << "procedure " << stmt->name.lexeme;
        generateParameterList(stmt->params);
        out << "; forward;\n";
    } else {
        indent();
        out << "procedure " << stmt->name.lexeme;
        generateParameterList(stmt->params);
        out << ";\n";
        if (stmt->kamus) execute(stmt->kamus);
        execute(stmt->body);
        out << ";\n";
    }
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::FunctionStmt> stmt) {
    if (forwardDeclare) {
        indent();
        out << "function " << stmt->name.lexeme;
        generateParameterList(stmt->params);
        out << ": " << pascalType(stmt->returnType) << "; forward;\n";
    } else {
        indent();
        out << "function " << stmt->name.lexeme;
        generateParameterList(stmt->params);
        out << ": " << pascalType(stmt->returnType) << ";\n";
        if (stmt->kamus) execute(stmt->kamus);
        currentFunctionName = stmt->name.lexeme;
        execute(stmt->body);
        currentFunctionName = "";
        out << ";\n";
    }
    return {};
}

std::any CodeGenerator::visit(std::shared_ptr<ast::ReturnStmt> stmt) {
    if (currentFunctionName.empty()) throw std::runtime_error("Return statement used outside of a function.");
    out << currentFunctionName << " := " << evaluate(stmt->value) << ";\n";
    return {};
}

std::string CodeGenerator::generateConstraintCheck(std::shared_ptr<ast::ConstrainedVarDeclStmt> constrainedVar) {
    std::string constraintExpr = evaluate(constrainedVar->constraint);
    std::string result = constraintExpr;
    size_t pos = 0;
    std::string varName = constrainedVar->name.lexeme;
    while ((pos = result.find(varName, pos)) != std::string::npos) {
        bool isWholeWord = true;
        if (pos > 0 && (std::isalnum(result[pos-1]) || result[pos-1] == '_')) isWholeWord = false;
        if (pos + varName.length() < result.length() && (std::isalnum(result[pos + varName.length()]) || result[pos + varName.length()] == '_')) isWholeWord = false;
        if (isWholeWord) {
            result.replace(pos, varName.length(), "value");
            pos += 5;
        } else {
            pos += varName.length();
        }
    }
    return "(" + result + ")";
}

} // namespace notal