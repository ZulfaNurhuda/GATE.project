#include "gate/transpiler/PascalCodeGenerator.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <set>
#include <fstream>
#include <vector>

namespace gate::transpiler {

// Using directives for brevity
using gate::core::Token;
using gate::core::TokenType;
using namespace gate::ast;

const std::set<std::string> BUILTIN_CASTING_FUNCTIONS = {
    "BooleanToChar", "BooleanToInteger", "BooleanToReal", "BooleanToString",
    "CharToBoolean", "CharToInteger", "CharToReal", "CharToString",
    "IntegerToBoolean", "IntegerToChar", "IntegerToHexString", "IntegerToReal", "IntegerToString",
    "RealToBoolean", "RealToChar", "RealToInteger", "RealToString",
    "StringHexToInteger", "StringToBoolean", "StringToChar", "StringToInteger", "StringToReal"
};

std::string PascalCodeGenerator::generate(std::shared_ptr<ProgramStmt> program) {
    if (!program) return "";
    preScan(program->algoritma);
    execute(program);
    return out_.str();
}

void PascalCodeGenerator::preScan(std::shared_ptr<Statement> stmt) {
    if (!stmt) return;

    if (auto algoritma = std::dynamic_pointer_cast<AlgoritmaStmt>(stmt)) {
        preScan(algoritma->body);
    } else if (auto block = std::dynamic_pointer_cast<BlockStmt>(stmt)) {
        for (const auto& s : block->statements) {
            preScan(s);
        }
    } else if (auto repeat = std::dynamic_pointer_cast<RepeatNTimesStmt>(stmt)) {
        loopVariables_.push_back("_loop_iterator_" + std::to_string(loopVariables_.size()));
    } else if (auto ifStmt = std::dynamic_pointer_cast<IfStmt>(stmt)) {
        preScan(ifStmt->thenBranch);
        preScan(ifStmt->elseBranch);
    } else if (auto whileStmt = std::dynamic_pointer_cast<WhileStmt>(stmt)) {
        preScan(whileStmt->body);
    } else if (auto traversalStmt = std::dynamic_pointer_cast<TraversalStmt>(stmt)) {
        preScan(traversalStmt->body);
    } else if (auto iterateStmt = std::dynamic_pointer_cast<IterateStopStmt>(stmt)) {
        preScan(iterateStmt->body);
    } else if (auto dependOnStmt = std::dynamic_pointer_cast<DependOnStmt>(stmt)) {
        for (const auto& c : dependOnStmt->cases) {
            preScan(c.body);
        }
        preScan(dependOnStmt->otherwiseBranch);
    }
}

void PascalCodeGenerator::indent() {
    for (int i = 0; i < indentLevel_; ++i) {
        out_ << "  ";
    }
}

std::string PascalCodeGenerator::pascalType(const Token& token) {
    switch (token.type) {
        case TokenType::INTEGER: return "integer";
        case TokenType::REAL: return "real";
        case TokenType::STRING: return "string";
        case TokenType::BOOLEAN: return "boolean";
        case TokenType::CHARACTER: return "char";
        case TokenType::POINTER: return "^";
        case TokenType::IDENTIFIER: return token.lexeme;
        default: throw std::runtime_error("Unknown type for code generation: " + token.lexeme);
    }
}

std::string PascalCodeGenerator::evaluate(std::shared_ptr<Expression> expr) {
    auto result = expr->accept(*this);
    try {
        return std::any_cast<std::string>(result);
    } catch (const std::bad_any_cast& e) {
        return ""; // Return empty string for statements
    }
}

void PascalCodeGenerator::execute(std::shared_ptr<Statement> stmt) {
    if (stmt) {
        stmt->accept(*this);
    }
}

// --- Statement Visitors ---

std::any PascalCodeGenerator::visit(std::shared_ptr<ProgramStmt> stmt) {
    for (const auto& sub : stmt->subprograms) {
        scanForCastingFunctions(sub);
    }
    scanForCastingFunctions(stmt->algoritma);

    out_ << "program " << stmt->name.lexeme << ";\n\n";

    if (!usedCastingFunctions_.empty()) {
        out_ << "uses SysUtils;\n\n";
    }

    execute(stmt->kamus);

    // Generate forward declarations from the original declaration order
    if (stmt->kamus) {
        forwardDeclare_ = true;
        for (const auto& decl : stmt->kamus->declarations) {
            if (std::dynamic_pointer_cast<ProcedureStmt>(decl) || std::dynamic_pointer_cast<FunctionStmt>(decl)) {
                execute(decl);
            }
        }
        forwardDeclare_ = false;
    }

    if (!usedCastingFunctions_.empty()) {
        generateCastingForwardDecls();
    }
    out_ << "\n";

    // Generate implementations from the implementation order
    for (const auto& sub : stmt->subprograms) {
        execute(sub);
        out_ << "\n";
    }

    if (!usedCastingFunctions_.empty()) {
        generateCastingImplementations();
    }

    execute(stmt->algoritma);
    out_ << ".\n";
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<KamusStmt> stmt) {
    std::vector<std::shared_ptr<Statement>> constDecls, typeDecls, varDecls, constrainedVarDecls;
    for (const auto& decl : stmt->declarations) {
        if (std::dynamic_pointer_cast<ProcedureStmt>(decl) || std::dynamic_pointer_cast<FunctionStmt>(decl)) continue;
        if (std::dynamic_pointer_cast<ConstDeclStmt>(decl)) constDecls.push_back(decl);
        else if (std::dynamic_pointer_cast<RecordTypeDeclStmt>(decl) || std::dynamic_pointer_cast<EnumTypeDeclStmt>(decl)) typeDecls.push_back(decl);
        else if (std::dynamic_pointer_cast<ConstrainedVarDeclStmt>(decl)) constrainedVarDecls.push_back(decl);
        else varDecls.push_back(decl);
    }

    if (!typeDecls.empty()) {
        out_ << "type\n";
        indentLevel_++;
        for (const auto& decl : typeDecls) { execute(decl); }
        indentLevel_--;
        out_ << "\n";
    }

    if (!constDecls.empty()) {
        out_ << "const\n";
        indentLevel_++;
        for (const auto& decl : constDecls) { execute(decl); }
        indentLevel_--;
        out_ << "\n";
    }

    if (!varDecls.empty() || !constrainedVarDecls.empty() || !loopVariables_.empty()) {
        out_ << "var\n";
        indentLevel_++;
        for (const auto& decl : varDecls) { indent(); execute(decl); out_ << ";\n"; }
        for (const auto& decl : constrainedVarDecls) {
            auto constrainedVar = std::dynamic_pointer_cast<ConstrainedVarDeclStmt>(decl);
            if (constrainedVar) constrainedVars_[constrainedVar->name.lexeme] = constrainedVar;
            indent(); execute(decl); out_ << ";\n";
        }
        for (const auto& var : loopVariables_) { indent(); out_ << var << ": integer;\n"; }
        indentLevel_--;
        out_ << "\n";
    }

    if (!constrainedVarDecls.empty()) {
        for (const auto& decl : constrainedVarDecls) {
            auto constrainedVar = std::dynamic_pointer_cast<ConstrainedVarDeclStmt>(decl);
            if (constrainedVar) {
                out_ << "procedure Set" << constrainedVar->name.lexeme << "(var " << constrainedVar->name.lexeme << ": " << pascalType(constrainedVar->type) << "; value: " << pascalType(constrainedVar->type) << ");\n";
                out_ << "begin\n";
                indentLevel_++;
                indent();
                out_ << "Assert(" << generateConstraintCheck(constrainedVar) << ", 'Error: " << constrainedVar->name.lexeme << " constraint violation!');\n";
                indent();
                out_ << constrainedVar->name.lexeme << " := value;\n";
                indentLevel_--;
                out_ << "end;\n\n";
            }
        }
    }
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<VarDeclStmt> stmt) {
    out_ << stmt->name.lexeme << ": ";
    if (stmt->type.type == TokenType::POINTER) {
        out_ << "^" << pascalType(stmt->pointedToType);
    } else {
        out_ << pascalType(stmt->type);
    }
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<StaticArrayDeclStmt> stmt) {
    out_ << stmt->name.lexeme << ": array[";
    for (size_t i = 0; i < stmt->dimensions.size(); ++i) {
        out_ << evaluate(stmt->dimensions[i].start) << ".." << evaluate(stmt->dimensions[i].end);
        if (i < stmt->dimensions.size() - 1) out_ << ", ";
    }
    out_ << "] of " << pascalType(stmt->elementType);
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<DynamicArrayDeclStmt> stmt) {
    dynamicArrayDimensions_[stmt->name.lexeme] = stmt->dimensions;
    out_ << stmt->name.lexeme << ": ";
    for (int i = 0; i < stmt->dimensions; ++i) {
        out_ << "array of ";
    }
    out_ << pascalType(stmt->elementType);
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<AllocateStmt> stmt) {
    if (stmt->sizes.empty()) {
        out_ << "New(" << evaluate(stmt->callee) << ");\n";
    } else {
        out_ << "SetLength(" << evaluate(stmt->callee);
        for (const auto& size : stmt->sizes) {
            out_ << ", " << evaluate(size);
        }
        out_ << ");\n";
    }
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<DeallocateStmt> stmt) {
    std::string varName = evaluate(stmt->callee);
    if (stmt->dimension == -1) {
        out_ << "Dispose(" << varName << ");\n";
    } else {
        if (dynamicArrayDimensions_.find(varName) == dynamicArrayDimensions_.end()) {
            throw std::runtime_error("Deallocating an undeclared dynamic array: " + varName);
        }
        int declaredDim = dynamicArrayDimensions_[varName];
        if (declaredDim != stmt->dimension) {
             throw std::runtime_error("Deallocation dimension mismatch for '" + varName + "'. Declared: " + std::to_string(declaredDim) + ", Used: " + std::to_string(stmt->dimension));
        }
        out_ << "SetLength(" << varName;
        for (int i = 0; i < stmt->dimension; ++i) {
            out_ << ", 0";
        }
        out_ << ");\n";
    }
    return {};
}


std::any PascalCodeGenerator::visit(std::shared_ptr<ConstDeclStmt> stmt) {
    auto literal = std::dynamic_pointer_cast<Literal>(stmt->initializer);
    if (literal) constants_[stmt->name.lexeme] = literal;
    indent();
    out_ << stmt->name.lexeme << " = " << evaluate(stmt->initializer) << ";\n";
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<InputStmt> stmt) {
    out_ << "readln(" << stmt->variable->name.lexeme << ");\n";
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<AlgoritmaStmt> stmt) {
    out_ << "begin\n";
    indentLevel_++;
    execute(stmt->body);
    indentLevel_--;
    indent();
    out_ << "end";
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<BlockStmt> stmt) {
    for (const auto& statement : stmt->statements) {
        indent();
        execute(statement);
    }
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<ExpressionStmt> stmt) {
    out_ << evaluate(stmt->expression) << ";\n";
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<OutputStmt> stmt) {
    out_ << "writeln(";
    for (size_t i = 0; i < stmt->expressions.size(); ++i) {
        if (auto varExpr = std::dynamic_pointer_cast<Variable>(stmt->expressions[i])) {
            auto it = constants_.find(varExpr->name.lexeme);
            if (it != constants_.end()) {
                out_ << varExpr->name.lexeme;
            } else {
                out_ << evaluate(stmt->expressions[i]);
            }
        } else {
            out_ << evaluate(stmt->expressions[i]);
        }
        if (i < stmt->expressions.size() - 1) {
            out_ << ", ";
        }
    }
    out_ << ");\n";
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<Assign> expr) {
    std::string targetStr = evaluate(expr->target);

    if (auto var = std::dynamic_pointer_cast<Variable>(expr->target)) {
        if (constrainedVars_.find(var->name.lexeme) != constrainedVars_.end()) {
            return "Set" + var->name.lexeme + "(" + var->name.lexeme + ", " + evaluate(expr->value) + ")";
        }
    }

    return targetStr + " := " + evaluate(expr->value);
}

std::any PascalCodeGenerator::visit(std::shared_ptr<Binary> expr) {
    if (expr->op.type == TokenType::POWER) {
        std::string base = evaluate(expr->left);
        std::string exponent = evaluate(expr->right);
        return "trunc(exp(" + exponent + " * ln(" + base + ")))";
    }

    std::string op = expr->op.lexeme;
    if (expr->op.type == TokenType::AMPERSAND) op = "+";
    return "(" + evaluate(expr->left) + " " + op + " " + evaluate(expr->right) + ")";
}

std::any PascalCodeGenerator::visit(std::shared_ptr<Unary> expr) {
    if (expr->op.type == TokenType::POWER) {
        return "(" + evaluate(expr->right) + expr->op.lexeme + ")";
    }
    if (expr->op.type == TokenType::AT) {
        return expr->op.lexeme + "(" + evaluate(expr->right) + ")";
    }
    return "(" + expr->op.lexeme + " " + evaluate(expr->right) + ")";
}

std::any PascalCodeGenerator::visit(std::shared_ptr<Grouping> expr) {
    return evaluate(expr->expression);
}

std::any PascalCodeGenerator::visit(std::shared_ptr<Variable> expr) {
    return std::string(expr->name.lexeme);
}

std::any PascalCodeGenerator::visit(std::shared_ptr<Literal> expr) {
    if (expr->value.type() == typeid(int)) {
        return std::to_string(std::any_cast<int>(expr->value));
    }
    if (expr->value.type() == typeid(double)) {
        std::string s = std::to_string(std::any_cast<double>(expr->value));
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (s.back() == '.') s.pop_back();
        return s;
    }
    if (expr->value.type() == typeid(bool)) {
        return std::string(std::any_cast<bool>(expr->value) ? "true" : "false");
    }
    if (expr->value.type() == typeid(std::string)) {
        return std::string("'") + std::any_cast<std::string>(expr->value) + "'";
    }
    return std::string("null");
}

std::any PascalCodeGenerator::visit(std::shared_ptr<ArrayAccess> expr) {
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

std::any PascalCodeGenerator::visit(std::shared_ptr<IfStmt> stmt) {
    out_ << "if " << evaluate(stmt->condition) << " then\n";
    indent();
    out_ << "begin\n";
    indentLevel_++;
    execute(stmt->thenBranch);
    indentLevel_--;
    indent();
    out_ << "end";

    if (stmt->elseBranch != nullptr) {
        out_ << " else ";
        if (std::dynamic_pointer_cast<IfStmt>(stmt->elseBranch)) {
            execute(stmt->elseBranch);
        } else {
            out_ << "\n";
            indent();
            out_ << "begin\n";
            indentLevel_++;
            execute(stmt->elseBranch);
            indentLevel_--;
            indent();
            out_ << "end";
        }
    }

    bool isPartOfElif = false;
    if (auto parent = stmt->parent.lock()) {
        if (auto parentIf = std::dynamic_pointer_cast<IfStmt>(parent)) {
            if (parentIf->elseBranch.get() == stmt.get()) isPartOfElif = true;
        }
    }
    if (!isPartOfElif) out_ << ";\n";
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<WhileStmt> stmt) {
    out_ << "while " << evaluate(stmt->condition) << " do\n";
    indentLevel_++;
    indent();
    out_ << "begin\n";
    indentLevel_++;
    execute(stmt->body);
    indentLevel_--;
    indent();
    out_ << "end;\n";
    indentLevel_--;
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<RepeatUntilStmt> stmt) {
    out_ << "repeat\n";
    indentLevel_++;
    execute(stmt->body);
    indentLevel_--;
    indent();
    out_ << "until " << evaluate(stmt->condition) << ";\n";
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<DependOnStmt> stmt) {
    bool allLiterals = true;
    for (const auto& caseItem : stmt->cases) {
        for (const auto& cond : caseItem.conditions) {
            if (std::dynamic_pointer_cast<Literal>(cond) == nullptr) {
                allLiterals = false;
                break;
            }
        }
        if (!allLiterals) break;
    }

    if (allLiterals && stmt->expressions.size() == 1) {
        out_ << "case " << evaluate(stmt->expressions[0]) << " of\n";
        indentLevel_++;
        for (const auto& caseItem : stmt->cases) {
            indent();
            for (size_t i = 0; i < caseItem.conditions.size(); ++i) {
                out_ << evaluate(caseItem.conditions[i]);
                if (i < caseItem.conditions.size() - 1) out_ << ", ";
            }
            out_ << ":\n";
            indentLevel_++;
            indent();
            out_ << "begin\n";
            indentLevel_++;
            execute(caseItem.body);
            indentLevel_--;
            indent();
            out_ << "end;\n";
            indentLevel_--;
        }
        if (stmt->otherwiseBranch) {
            indent();
            out_ << "else\n";
            indentLevel_++;
            indent();
            out_ << "begin\n";
            indentLevel_++;
            execute(stmt->otherwiseBranch);
            indentLevel_--;
            indent();
            out_ << "end;\n";
            indentLevel_--;
        }
        indentLevel_--;
        indent();
        out_ << "end;\n";
    } else {
        for (size_t i = 0; i < stmt->cases.size(); ++i) {
            if (i > 0) {
                indent();
                out_ << "else ";
            }
            out_ << "if " << evaluate(stmt->cases[i].conditions[0]) << " then\n";
            indent();
            out_ << "begin\n";
            indentLevel_++;
            execute(stmt->cases[i].body);
            indentLevel_--;
            indent();
            out_ << "end";

            if (i == stmt->cases.size() - 1 && !stmt->otherwiseBranch) {
                 out_ << ";\n";
            } else {
                 out_ << "\n";
            }
        }

        if (stmt->otherwiseBranch) {
            indent();
            out_ << "else\n";
            indent();
            out_ << "begin\n";
            indentLevel_++;
            execute(stmt->otherwiseBranch);
            indentLevel_--;
            indent();
            out_ << "end;\n";
        }
    }
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<TraversalStmt> stmt) {
    std::string iterator = stmt->iterator.lexeme;
    std::string start = evaluate(stmt->start);
    std::string end = evaluate(stmt->end);
    std::string step = "1";
    if (stmt->step) step = evaluate(stmt->step);

    indent();
    out_ << iterator << " := " << start << ";\n";
    indent();
    out_ << "while (" << iterator << " <= " << end << ") do\n";
    indent();
    out_ << "begin\n";
    indentLevel_++;
    execute(stmt->body);
    indent();
    out_ << "Inc(" << iterator << ", " << step << ");\n";
    indentLevel_--;
    indent();
    out_ << "end;\n";
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<IterateStopStmt> stmt) {
    indent();
    out_ << "while true do\n";
    indent();
    out_ << "begin\n";
    indentLevel_++;
    execute(stmt->body);
    indent();
    out_ << "if " << evaluate(stmt->condition) << " then break;\n";
    indentLevel_--;
    indent();
    out_ << "end;\n";
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<RepeatNTimesStmt> stmt) {
    std::string iterator = loopVariables_[loopCounter_++];
    indent();
    out_ << "for " << iterator << " := 1 to " << evaluate(stmt->times) << " do\n";
    indent();
    out_ << "begin\n";
    indentLevel_++;
    execute(stmt->body);
    indentLevel_--;
    indent();
    out_ << "end;\n";
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<StopStmt> stmt) {
    (void)stmt;
    out_ << "break;\n";
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<SkipStmt> stmt) {
    (void)stmt;
    out_ << "continue;\n";
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<Call> expr) {
    std::string callee = evaluate(expr->callee);
    std::string args;
    for (size_t i = 0; i < expr->arguments.size(); ++i) {
        args += evaluate(expr->arguments[i]);
        if (i < expr->arguments.size() - 1) args += ", ";
    }
    return callee + "(" + args + ")";
}

std::any PascalCodeGenerator::visit(std::shared_ptr<FieldAccess> expr) {
    return evaluate(expr->object) + "." + expr->name.lexeme;
}

std::any PascalCodeGenerator::visit(std::shared_ptr<FieldAssign> expr) {
    return evaluate(expr->target) + " := " + evaluate(expr->value);
}

std::any PascalCodeGenerator::visit(std::shared_ptr<RecordTypeDeclStmt> stmt) {
    indent();
    out_ << stmt->typeName.lexeme << " = record\n";
    indentLevel_++;
    for (const auto& field : stmt->fields) {
        indent();
        out_ << field.name.lexeme << ": " << pascalType(field.type) << ";\n";
    }
    indentLevel_--;
    indent();
    out_ << "end;\n\n";
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<EnumTypeDeclStmt> stmt) {
    indent();
    out_ << stmt->typeName.lexeme << " = (";
    for (size_t i = 0; i < stmt->values.size(); ++i) {
        out_ << stmt->values[i].lexeme;
        if (i < stmt->values.size() - 1) out_ << ", ";
    }
    out_ << ");\n";
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<ConstrainedVarDeclStmt> stmt) {
    out_ << stmt->name.lexeme << ": " << pascalType(stmt->type);
    return {};
}

void PascalCodeGenerator::generateParameterList(const std::vector<Parameter>& params) {
    out_ << "(";
    for (size_t i = 0; i < params.size(); ++i) {
        const auto& p = params[i];
        switch (p.mode) {
            case ParameterMode::INPUT: break;
            case ParameterMode::OUTPUT:
            case ParameterMode::INPUT_OUTPUT: out_ << "var "; break;
        }
        out_ << p.name.lexeme << ": " << pascalType(p.type);
        if (i < params.size() - 1) out_ << "; ";
    }
    out_ << ")";
}

std::any PascalCodeGenerator::visit(std::shared_ptr<ProcedureStmt> stmt) {
    if (forwardDeclare_) {
        indent();
        out_ << "procedure " << stmt->name.lexeme;
        generateParameterList(stmt->params);
        out_ << "; forward;\n";
    } else {
        indent();
        out_ << "procedure " << stmt->name.lexeme;
        generateParameterList(stmt->params);
        out_ << ";\n";
        if (stmt->kamus) execute(stmt->kamus);
        execute(stmt->body);
        out_ << ";\n";
    }
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<FunctionStmt> stmt) {
    if (forwardDeclare_) {
        indent();
        out_ << "function " << stmt->name.lexeme;
        generateParameterList(stmt->params);
        out_ << ": " << pascalType(stmt->returnType) << "; forward;\n";
    } else {
        indent();
        out_ << "function " << stmt->name.lexeme;
        generateParameterList(stmt->params);
        out_ << ": " << pascalType(stmt->returnType) << ";\n";
        if (stmt->kamus) execute(stmt->kamus);
        currentFunctionName_ = stmt->name.lexeme;
        execute(stmt->body);
        currentFunctionName_ = "";
        out_ << ";\n";
    }
    return {};
}

std::any PascalCodeGenerator::visit(std::shared_ptr<ReturnStmt> stmt) {
    if (currentFunctionName_.empty()) throw std::runtime_error("Return statement used outside of a function.");
    out_ << currentFunctionName_ << " := " << evaluate(stmt->value) << ";\n";
    return {};
}

std::string PascalCodeGenerator::generateConstraintCheck(std::shared_ptr<ConstrainedVarDeclStmt> constrainedVar) {
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

// --- Casting Function Helpers ---

void PascalCodeGenerator::scanForCastingFunctions(std::shared_ptr<Statement> stmt) {
    if (!stmt) return;

    if (auto algoritma = std::dynamic_pointer_cast<AlgoritmaStmt>(stmt)) {
        scanForCastingFunctions(algoritma->body);
    } else if (auto block = std::dynamic_pointer_cast<BlockStmt>(stmt)) {
        for (const auto& s : block->statements) scanForCastingFunctions(s);
    } else if (auto exprStmt = std::dynamic_pointer_cast<ExpressionStmt>(stmt)) {
        scanExpression(exprStmt->expression);
    } else if (auto ifStmt = std::dynamic_pointer_cast<IfStmt>(stmt)) {
        scanExpression(ifStmt->condition);
        scanForCastingFunctions(ifStmt->thenBranch);
        scanForCastingFunctions(ifStmt->elseBranch);
    } else if (auto whileStmt = std::dynamic_pointer_cast<WhileStmt>(stmt)) {
        scanExpression(whileStmt->condition);
        scanForCastingFunctions(whileStmt->body);
    } else if (auto repeatStmt = std::dynamic_pointer_cast<RepeatUntilStmt>(stmt)) {
        scanForCastingFunctions(repeatStmt->body);
        scanExpression(repeatStmt->condition);
    } else if (auto dependOnStmt = std::dynamic_pointer_cast<DependOnStmt>(stmt)) {
        for (const auto& expr : dependOnStmt->expressions) scanExpression(expr);
        for (const auto& c : dependOnStmt->cases) {
            for (const auto& cond : c.conditions) scanExpression(cond);
            scanForCastingFunctions(c.body);
        }
        if (dependOnStmt->otherwiseBranch) scanForCastingFunctions(dependOnStmt->otherwiseBranch);
    } else if (auto traversalStmt = std::dynamic_pointer_cast<TraversalStmt>(stmt)) {
        scanExpression(traversalStmt->start);
        scanExpression(traversalStmt->end);
        if (traversalStmt->step) scanExpression(traversalStmt->step);
        scanForCastingFunctions(traversalStmt->body);
    } else if (auto iterateStmt = std::dynamic_pointer_cast<IterateStopStmt>(stmt)) {
        scanForCastingFunctions(iterateStmt->body);
        scanExpression(iterateStmt->condition);
    } else if (auto outputStmt = std::dynamic_pointer_cast<OutputStmt>(stmt)) {
        for (const auto& expr : outputStmt->expressions) scanExpression(expr);
    } else if (auto procStmt = std::dynamic_pointer_cast<ProcedureStmt>(stmt)) {
        if (procStmt->body) scanForCastingFunctions(procStmt->body);
    } else if (auto funcStmt = std::dynamic_pointer_cast<FunctionStmt>(stmt)) {
        if (funcStmt->body) scanForCastingFunctions(funcStmt->body);
    } else if (auto returnStmt = std::dynamic_pointer_cast<ReturnStmt>(stmt)) {
        scanExpression(returnStmt->value);
    }
}

void PascalCodeGenerator::scanExpression(std::shared_ptr<Expression> expr) {
    if (!expr) return;

    if (auto assign = std::dynamic_pointer_cast<Assign>(expr)) {
        scanExpression(assign->target);
        scanExpression(assign->value);
    } else if (auto binary = std::dynamic_pointer_cast<Binary>(expr)) {
        scanExpression(binary->left);
        scanExpression(binary->right);
    } else if (auto unary = std::dynamic_pointer_cast<Unary>(expr)) {
        scanExpression(unary->right);
    } else if (auto grouping = std::dynamic_pointer_cast<Grouping>(expr)) {
        scanExpression(grouping->expression);
    } else if (auto call = std::dynamic_pointer_cast<Call>(expr)) {
        scanExpression(call->callee);
        for (const auto& arg : call->arguments) scanExpression(arg);
        if (auto var = std::dynamic_pointer_cast<Variable>(call->callee)) {
            if (BUILTIN_CASTING_FUNCTIONS.count(var->name.lexeme)) {
                usedCastingFunctions_.insert(var->name.lexeme);
            }
        }
    } else if (auto fieldAccess = std::dynamic_pointer_cast<FieldAccess>(expr)) {
        scanExpression(fieldAccess->object);
    } else if (auto arrayAccess = std::dynamic_pointer_cast<ArrayAccess>(expr)) {
        scanExpression(arrayAccess->callee);
        for (const auto& index : arrayAccess->indices) scanExpression(index);
    }
}


void PascalCodeGenerator::generateCastingForwardDecls() {
    for (const auto& funcName : usedCastingFunctions_) {
        std::string filePath = "src/casting/" + funcName + ".casting.txt";
        std::ifstream file(filePath);
        if (!file) {
            throw std::runtime_error("Could not open casting file: " + filePath);
        }
        std::string line;
        while (std::getline(file, line)) {
            if (line.rfind("function", 0) == 0 || line.rfind("procedure", 0) == 0) {
                if (!line.empty() && line.back() == ';') {
                    line.pop_back();
                }
                out_ << line << "; forward;\n";
            }
        }
    }
}


void PascalCodeGenerator::generateCastingImplementations() {
    for (const auto& funcName : usedCastingFunctions_) {
        std::string filePath = "src/casting/" + funcName + ".casting.txt";
        std::ifstream file(filePath);
        if (!file) {
            throw std::runtime_error("Could not open casting file: " + filePath);
        }
        out_ << file.rdbuf() << "\n\n";
    }
}

} // namespace gate::transpiler
