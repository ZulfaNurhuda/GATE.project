#include "notal_transpiler/Parser.h"
#include <iostream>
#include <memory>
#include <vector>

namespace notal {

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

// --- Public Parse Method ---

std::shared_ptr<ast::ProgramStmt> Parser::parse() {
    try {
        return program();
    } catch (const ParseError& error) {
        std::cerr << "[Line " << error.token.line << "] Error";
        if (error.token.type == TokenType::END_OF_FILE) {
            std::cerr << " at end";
        } else {
            std::cerr << " at '" << error.token.lexeme << "'";
        }
        std::cerr << ": " << error.what() << std::endl;
        return nullptr;
    }
}

// --- Grammar Rule Methods ---

// program -> "PROGRAM" IDENTIFIER kamus algoritma
std::shared_ptr<ast::ProgramStmt> Parser::program() {
    consume(TokenType::PROGRAM, "Expect 'PROGRAM'.");
    Token name = consume(TokenType::IDENTIFIER, "Expect program name.");
    
    // The kamus() method will now populate the `subprogramDeclarations` map
    std::shared_ptr<ast::KamusStmt> kamusBlock = kamus();

    std::shared_ptr<ast::AlgoritmaStmt> algoritmaBlock = algoritma();

    std::vector<std::shared_ptr<ast::Stmt>> subprograms;
    while (!isAtEnd()) {
        Token subprogramKeyword = peek();
        if (subprogramKeyword.type != TokenType::PROCEDURE && subprogramKeyword.type != TokenType::FUNCTION) {
            throw error(peek(), "Expect procedure or function implementation after main algorithm.");
        }
        advance(); // consume PROCEDURE or FUNCTION

        Token subprogramName = consume(TokenType::IDENTIFIER, "Expect procedure or function name for implementation.");

        // Find declaration and parse the implementation
        subprogramImplementation(subprogramKeyword, subprogramName);
    }

    // Move completed subprograms from the map to the vector
    for (auto const& [key, val] : subprogramDeclarations) {
        subprograms.push_back(val);
    }

    // TODO: Check if any declarations were not implemented.

    return std::make_shared<ast::ProgramStmt>(name, kamusBlock, algoritmaBlock, subprograms);
}

// kamus -> "KAMUS" declaration*
std::shared_ptr<ast::KamusStmt> Parser::kamus() {
    consume(TokenType::KAMUS, "Expect 'KAMUS'.");
    std::vector<std::shared_ptr<ast::Stmt>> declarations;
    while (!check(TokenType::ALGORITMA) && !isAtEnd()) {
        declarations.push_back(declaration());
    }
    return std::make_shared<ast::KamusStmt>(declarations);
}

// algoritma -> "ALGORITMA" block
std::shared_ptr<ast::AlgoritmaStmt> Parser::algoritma() {
    consume(TokenType::ALGORITMA, "Expect 'ALGORITMA'.");
    std::vector<std::shared_ptr<ast::Stmt>> statements = block();
    auto body = std::make_shared<ast::BlockStmt>(statements);
    return std::make_shared<ast::AlgoritmaStmt>(body);
}

// declaration -> constantDeclaration | typeDeclaration | varDeclaration
std::shared_ptr<ast::Stmt> Parser::declaration() {
    if (check(TokenType::PROCEDURE) || check(TokenType::FUNCTION)) {
        return subprogramDeclaration();
    }
    if (match({TokenType::CONSTANT})) {
        return constantDeclaration();
    }
    if (match({TokenType::TYPE})) {
        return typeDeclaration();
    }
    return varDeclaration();
}

// constantDeclaration -> "constant" IDENTIFIER ":" type "=" expression
std::shared_ptr<ast::Stmt> Parser::constantDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect constant name.");
    consume(TokenType::COLON, "Expect ':' after constant name.");

    Token type = advance();
    if (type.type != TokenType::INTEGER && type.type != TokenType::REAL &&
        type.type != TokenType::STRING && type.type != TokenType::BOOLEAN &&
        type.type != TokenType::CHARACTER) {
            throw error(type, "Expect a type name.");
    }

    consume(TokenType::EQUAL, "Expect '=' after type.");

    std::shared_ptr<ast::Expr> initializer = expression();

    return std::make_shared<ast::ConstDeclStmt>(name, type, initializer);
}

// typeDeclaration -> "type" IDENTIFIER ":" recordType | enumType
std::shared_ptr<ast::Stmt> Parser::typeDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect type name.");
    consume(TokenType::COLON, "Expect ':' after type name.");
    
    if (check(TokenType::LESS)) {
        // Record type: type Student: < name: string, age: integer >
        advance(); // consume <
        
        std::vector<ast::RecordTypeDeclStmt::Field> fields;
        
        if (!check(TokenType::GREATER)) {
            do {
                Token fieldName = consume(TokenType::IDENTIFIER, "Expect field name.");
                consume(TokenType::COLON, "Expect ':' after field name.");
                Token fieldType = advance();
                
                if (fieldType.type != TokenType::INTEGER && fieldType.type != TokenType::REAL &&
                    fieldType.type != TokenType::STRING && fieldType.type != TokenType::BOOLEAN &&
                    fieldType.type != TokenType::CHARACTER && fieldType.type != TokenType::IDENTIFIER) {
                    throw error(fieldType, "Expect a basic type name or custom type.");
                }
                
                fields.emplace_back(fieldName, fieldType);
            } while (match({TokenType::COMMA}));
        }
        
        consume(TokenType::GREATER, "Expect '>' after record fields.");
        return std::make_shared<ast::RecordTypeDeclStmt>(name, fields);
        
    } else if (check(TokenType::LPAREN)) {
        // Enum type: type Day: (monday, tuesday, wednesday)
        advance(); // consume (
        
        std::vector<Token> values;
        
        if (!check(TokenType::RPAREN)) {
            do {
                Token value = consume(TokenType::IDENTIFIER, "Expect enum value name.");
                values.push_back(value);
            } while (match({TokenType::COMMA}));
        }
        
        consume(TokenType::RPAREN, "Expect ')' after enum values.");
        return std::make_shared<ast::EnumTypeDeclStmt>(name, values);
    } else {
        throw error(peek(), "Expect '<' for record type or '(' for enum type.");
    }
}

// varDeclaration -> IDENTIFIER ":" type [ "|" constraint ]
std::shared_ptr<ast::Stmt> Parser::varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    consume(TokenType::COLON, "Expect ':' after variable name.");
    
    Token type = advance();
    if (type.type != TokenType::INTEGER && type.type != TokenType::REAL &&
        type.type != TokenType::STRING && type.type != TokenType::BOOLEAN &&
        type.type != TokenType::CHARACTER && type.type != TokenType::IDENTIFIER) {
            throw error(type, "Expect a type name.");
        }

    // Check for constraint: age: integer | age >= 0 and age <= 150
    if (match({TokenType::PIPE})) {
        std::shared_ptr<ast::Expr> constraint = expression();
        return std::make_shared<ast::ConstrainedVarDeclStmt>(name, type, constraint);
    }

    return std::make_shared<ast::VarDeclStmt>(name, type);
}

// block -> (statement)*
std::vector<std::shared_ptr<notal::ast::Stmt>> Parser::block() {
    // This block is for the top-level ALGORITMA, which starts at column 0 (or whatever the initial indentation is)
    // We'll use the column of the first statement to determine the expected indentation for subsequent statements.
    // If the ALGORITMA block is empty, peek().column might be irrelevant, so handle that.
    int initialIndent = 0;
    if (!isAtEnd()) {
        initialIndent = peek().column;
    }
    return parseBlockByIndentation(initialIndent);
}

std::vector<std::shared_ptr<notal::ast::Stmt>> Parser::parseBlockByIndentation(int expectedIndentLevel) {
    std::vector<std::shared_ptr<notal::ast::Stmt>> statements;
    // std::cout << "--> Parsing indented block, expected indent >= " << expectedIndentLevel << "\n";
    while (!isAtEnd() && peek().column >= expectedIndentLevel) {
        // std::cout << "--> In block loop, current token: " << peek().lexeme << ", column: " << peek().column << "\n";
        if (peek().column == expectedIndentLevel &&
            (peek().type == TokenType::PROCEDURE || peek().type == TokenType::FUNCTION)) {
            break;
        }
        if (peek().column < expectedIndentLevel) {
            break;
        }
        statements.push_back(statement());
    }
    // std::cout << "--> Exiting indented block\n";
    return statements;
}

// statement -> ifStatement | whileStatement | repeatUntilStatement | outputStatement | inputStatement | expressionStatement
std::shared_ptr<notal::ast::Stmt> Parser::statement() {
    if (check(TokenType::IF)) {
        return ifStatement();
    }
    if (check(TokenType::WHILE)) {
        return whileStatement();
    }
    if (match({TokenType::REPEAT})) {
        if (peek().type == TokenType::INTEGER_LITERAL) {
            return repeatNTimesStatement();
        }
        return repeatUntilStatement();
    }
    if (check(TokenType::DEPEND)) {
        return dependOnStatement();
    }
    if (check(TokenType::OUTPUT)) {
        return outputStatement();
    }
    if (check(TokenType::INPUT)) {
        return inputStatement();
    }
    if (peek().type == TokenType::IDENTIFIER && peekNext().type == TokenType::TRAVERSAL) {
        return traversalStatement();
    }
    if (check(TokenType::ITERATE)) {
        return iterateStopStatement();
    }
    if (match({TokenType::STOP})) {
        return std::make_shared<ast::StopStmt>();
    }
    if (match({TokenType::SKIP})) {
        return std::make_shared<ast::SkipStmt>();
    }
    if (check(TokenType::ARROW)) {
        return returnStatement();
    }
    return expressionStatement();
}

// inputStatement -> "input" "(" IDENTIFIER ")"
std::shared_ptr<ast::Stmt> Parser::inputStatement() {
    consume(TokenType::INPUT, "Expect 'input'.");
    consume(TokenType::LPAREN, "Expect '(' after 'input'.");

    Token variable_token = consume(TokenType::IDENTIFIER, "Expect variable name.");
    auto variable = std::make_shared<ast::Variable>(variable_token);

    consume(TokenType::RPAREN, "Expect ')' after variable name.");
    return std::make_shared<ast::InputStmt>(variable);
}

std::shared_ptr<notal::ast::Stmt> Parser::ifStatement() {
    Token ifToken = consume(TokenType::IF, "Expect 'if'."); // Capture the IF token
    return ifStatementBody(ifToken.column); // Pass the column of 'if'
}

std::shared_ptr<notal::ast::Stmt> Parser::ifStatementBody(int parentIndentLevel) {
    auto condition = expression();
    consume(TokenType::THEN, "Expect 'then' after condition.");
    
    int thenBranchIndent = 0;
    if (!isAtEnd()) {
        thenBranchIndent = peek().column;
    }

    std::shared_ptr<notal::ast::BlockStmt> thenBranch;
    if (thenBranchIndent > parentIndentLevel) {
        thenBranch = std::make_shared<ast::BlockStmt>(parseBlockByIndentation(thenBranchIndent));
    } else {
        thenBranch = std::make_shared<ast::BlockStmt>(std::vector<std::shared_ptr<ast::Stmt>>{});
    }
    
    std::shared_ptr<notal::ast::Stmt> elseBranch = nullptr;

    if (match({TokenType::ELIF})) {
        if (previous().column != parentIndentLevel) {
            throw error(previous(), "'elif' must be at the same indentation level as 'if'.");
        }
        elseBranch = ifStatementBody(parentIndentLevel);
    } else if (match({TokenType::ELSE})) {
        if (previous().column != parentIndentLevel) {
            throw error(previous(), "'else' must be at the same indentation level as 'if'.");
        }
        int elseBranchIndent = 0;
        if (!isAtEnd()) {
            elseBranchIndent = peek().column;
        }

        if (elseBranchIndent > parentIndentLevel) {
            elseBranch = std::make_shared<ast::BlockStmt>(parseBlockByIndentation(elseBranchIndent));
        } else {
            elseBranch = std::make_shared<ast::BlockStmt>(std::vector<std::shared_ptr<ast::Stmt>>{});
        }
    }

    auto ifStmt = std::make_shared<notal::ast::IfStmt>(condition, thenBranch, elseBranch);
    if (thenBranch) thenBranch->parent = ifStmt;
    if (elseBranch) elseBranch->parent = ifStmt;

    return ifStmt;
}

std::shared_ptr<notal::ast::Stmt> Parser::whileStatement() {
    Token whileToken = consume(TokenType::WHILE, "Expect 'while'."); // Capture the WHILE token
    std::shared_ptr<notal::ast::Expr> condition = expression();
    consume(TokenType::DO, "Expect 'do' after while condition.");
    
    int whileBodyIndent = 0;
    if (!isAtEnd()) {
        whileBodyIndent = peek().column;
    }
    
    // The body must be more indented than the while statement itself.
    if (whileBodyIndent <= whileToken.column) {
        // This could be an empty loop body.
        auto bodyBlock = std::make_shared<ast::BlockStmt>(std::vector<std::shared_ptr<ast::Stmt>>{});
        return std::make_shared<ast::WhileStmt>(condition, bodyBlock);
    }

    std::shared_ptr<notal::ast::BlockStmt> bodyBlock = std::make_shared<ast::BlockStmt>(parseBlockByIndentation(whileBodyIndent));

    return std::make_shared<notal::ast::WhileStmt>(condition, bodyBlock);
}

std::shared_ptr<ast::Stmt> Parser::repeatUntilStatement() {
    Token repeatToken = previous(); // 'repeat' was already matched

    int bodyIndent = 0;
    if (!isAtEnd()) {
        bodyIndent = peek().column;
    }

    if (bodyIndent <= repeatToken.column) {
        throw error(peek(), "The body of a repeat-until loop must be indented.");
    }

    auto body = std::make_shared<ast::BlockStmt>(parseBlockByIndentation(bodyIndent));

    consume(TokenType::UNTIL, "Expect 'until' after repeat block.");

    std::shared_ptr<ast::Expr> condition = expression();

    return std::make_shared<ast::RepeatUntilStmt>(body, condition);
}

// i traversal [1..10 step 2]
std::shared_ptr<ast::Stmt> Parser::traversalStatement() {
    Token iterator = consume(TokenType::IDENTIFIER, "Expect iterator name.");
    consume(TokenType::TRAVERSAL, "Expect 'traversal'.");
    consume(TokenType::LBRACKET, "Expect '[' after 'traversal'.");
    
    std::shared_ptr<ast::Expr> start = primary();
    consume(TokenType::DOT, "Expect '..' between start and end values.");
    consume(TokenType::DOT, "Expect '..' between start and end values.");
    std::shared_ptr<ast::Expr> end = primary();
    
    std::shared_ptr<ast::Expr> step = nullptr;
    if (match({TokenType::STEP})) {
        step = primary();
    }
    
    consume(TokenType::RBRACKET, "Expect ']' after range.");

    int bodyIndent = 0;
    if (!isAtEnd()) {
        bodyIndent = peek().column;
    }

    if (bodyIndent <= iterator.column) {
        throw error(peek(), "The body of a traversal loop must be indented.");
    }

    auto body = std::make_shared<ast::BlockStmt>(parseBlockByIndentation(bodyIndent));
    
    return std::make_shared<ast::TraversalStmt>(iterator, start, end, step, body);
}

// iterate ... stop (condition)
std::shared_ptr<ast::Stmt> Parser::iterateStopStatement() {
    Token iterateToken = consume(TokenType::ITERATE, "Expect 'iterate'.");

    int bodyIndent = 0;
    if (!isAtEnd()) {
        bodyIndent = peek().column;
    }

    if (bodyIndent <= iterateToken.column) {
        throw error(peek(), "The body of an iterate-stop loop must be indented.");
    }

    auto body = std::make_shared<ast::BlockStmt>(parseBlockByIndentation(bodyIndent));

    consume(TokenType::STOP, "Expect 'stop' after iterate block.");
    consume(TokenType::LPAREN, "Expect '(' after 'stop'.");
    std::shared_ptr<ast::Expr> condition = expression();
    consume(TokenType::RPAREN, "Expect ')' after stop condition.");

    return std::make_shared<ast::IterateStopStmt>(body, condition);
}

// repeat 5 times
std::shared_ptr<ast::Stmt> Parser::repeatNTimesStatement() {
    // 'repeat' token was matched in statement(). It's the previous token.
    Token repeatToken = previous();

    std::shared_ptr<ast::Expr> times = primary();
    consume(TokenType::TIMES, "Expect 'times' after number.");

    int bodyIndent = 0;
    if (!isAtEnd()) {
        bodyIndent = peek().column;
    }

    if (bodyIndent <= repeatToken.column) {
        throw error(peek(), "The body of a repeat N times loop must be indented.");
    }

    auto body = std::make_shared<ast::BlockStmt>(parseBlockByIndentation(bodyIndent));

    return std::make_shared<ast::RepeatNTimesStmt>(times, body);
}


std::shared_ptr<ast::Stmt> Parser::dependOnStatement() {
    Token dependToken = consume(TokenType::DEPEND, "Expect 'depend'.");
    consume(TokenType::ON, "Expect 'on' after 'depend'.");
    consume(TokenType::LPAREN, "Expect '(' after 'on'.");
    std::shared_ptr<ast::Expr> expr = expression();
    consume(TokenType::RPAREN, "Expect ')' after depend on expression.");

    std::vector<ast::DependOnStmt::Case> cases;
    std::shared_ptr<ast::Stmt> otherwiseBranch = nullptr;

    int caseIndent = 0;
    if (!isAtEnd()) {
        caseIndent = peek().column;
    }

    if (caseIndent <= dependToken.column) {
        throw error(peek(), "Cases for 'depend on' must be indented.");
    }

    while (!isAtEnd() && peek().column == caseIndent && !check(TokenType::OTHERWISE)) {
        std::vector<std::shared_ptr<ast::Expr>> conditions;
        do {
            conditions.push_back(expression());
        } while (match({TokenType::COMMA}));

        consume(TokenType::COLON, "Expect ':' after case conditions.");

        int bodyIndent = 0;
        if (!isAtEnd()) {
            bodyIndent = peek().column;
        }

        if (bodyIndent <= caseIndent) {
            throw error(peek(), "The body of a case must be indented.");
        }

        auto body = std::make_shared<ast::BlockStmt>(parseBlockByIndentation(bodyIndent));
        cases.emplace_back(conditions, body);
    }

    if (match({TokenType::OTHERWISE})) {
        consume(TokenType::COLON, "Expect ':' after 'otherwise'.");
        int otherwiseIndent = 0;
        if (!isAtEnd()) {
            otherwiseIndent = peek().column;
        }
        if (otherwiseIndent <= dependToken.column) {
            throw error(peek(), "The body of 'otherwise' must be indented.");
        }
        otherwiseBranch = std::make_shared<ast::BlockStmt>(parseBlockByIndentation(otherwiseIndent));
    }

    return std::make_shared<ast::DependOnStmt>(expr, cases, otherwiseBranch);
}

std::shared_ptr<ast::Stmt> Parser::outputStatement() {
    consume(TokenType::OUTPUT, "Expect 'output'.");
    consume(TokenType::LPAREN, "Expect '(' after 'output'.");
    
    std::vector<std::shared_ptr<ast::Expr>> expressions;
    if (!check(TokenType::RPAREN)) {
        do {
            expressions.push_back(expression());
        } while (match({TokenType::COMMA}));
    }

    consume(TokenType::RPAREN, "Expect ')' after output arguments.");
    return std::make_shared<ast::OutputStmt>(expressions);
}

std::shared_ptr<ast::Stmt> Parser::expressionStatement() {
    std::shared_ptr<ast::Expr> expr = expression();
    return std::make_shared<ast::ExpressionStmt>(expr);
}


// --- Subprogram Parsing ---

std::shared_ptr<ast::Stmt> Parser::subprogramDeclaration() {
    if (check(TokenType::PROCEDURE)) {
        return procedureDeclaration();
    }
    if (check(TokenType::FUNCTION)) {
        return functionDeclaration();
    }
    // Should not be reached if called correctly
    throw error(peek(), "Expect 'procedure' or 'function'.");
}

std::shared_ptr<ast::Stmt> Parser::procedureDeclaration() {
    consume(TokenType::PROCEDURE, "Expect 'procedure'.");
    Token name = consume(TokenType::IDENTIFIER, "Expect procedure name.");

    std::vector<ast::Parameter> params = parameterList();

    if (subprogramDeclarations.count(name.lexeme)) {
        throw error(name, "Procedure with this name already declared.");
    }

    // Create a procedure statement with an empty body for now.
    // The body will be filled in when the implementation is parsed.
    auto procStmt = std::make_shared<ast::ProcedureStmt>(name, params, nullptr, nullptr);
    subprogramDeclarations[name.lexeme] = procStmt;

    return procStmt;
}

std::shared_ptr<ast::Stmt> Parser::functionDeclaration() {
    consume(TokenType::FUNCTION, "Expect 'function'.");
    Token name = consume(TokenType::IDENTIFIER, "Expect function name.");

    std::vector<ast::Parameter> params = parameterList();

    consume(TokenType::ARROW, "Expect '->' for function return type.");
    Token returnType = advance();
    if (returnType.type != TokenType::INTEGER && returnType.type != TokenType::REAL &&
        returnType.type != TokenType::STRING && returnType.type != TokenType::BOOLEAN &&
        returnType.type != TokenType::CHARACTER && returnType.type != TokenType::IDENTIFIER) {
            throw error(returnType, "Expect a valid return type name.");
    }

    if (subprogramDeclarations.count(name.lexeme)) {
        throw error(name, "Function with this name already declared.");
    }

    auto funcStmt = std::make_shared<ast::FunctionStmt>(name, params, returnType, nullptr, nullptr);
    subprogramDeclarations[name.lexeme] = funcStmt;

    return funcStmt;
}

std::vector<ast::Parameter> Parser::parameterList() {
    consume(TokenType::LPAREN, "Expect '(' after subprogram name.");
    std::vector<ast::Parameter> params;
    if (!check(TokenType::RPAREN)) {
        do {
            ast::ParameterMode mode = ast::ParameterMode::INPUT; // Default mode

            if (peek().type == TokenType::INPUT) {
                advance(); // consume 'input'
                if (match({TokenType::DIVIDE})) {
                    consume(TokenType::OUTPUT, "Expect 'output' after '/' for 'input/output' parameter.");
                    mode = ast::ParameterMode::INPUT_OUTPUT;
                } else {
                    mode = ast::ParameterMode::INPUT;
                }
            } else if (peek().type == TokenType::OUTPUT) {
                advance(); // consume 'output'
                mode = ast::ParameterMode::OUTPUT;
            }

            Token name = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            consume(TokenType::COLON, "Expect ':' after parameter name.");
            Token type = advance();
             if (type.type != TokenType::INTEGER && type.type != TokenType::REAL &&
                type.type != TokenType::STRING && type.type != TokenType::BOOLEAN &&
                type.type != TokenType::CHARACTER && type.type != TokenType::IDENTIFIER) {
                    throw error(type, "Expect a valid type name for parameter.");
            }
            params.emplace_back(mode, name, type);
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RPAREN, "Expect ')' after parameter list.");
    return params;
}

void Parser::subprogramImplementation(const Token& subprogramKeyword, const Token& subprogramName) {
    auto it = subprogramDeclarations.find(subprogramName.lexeme);
    if (it == subprogramDeclarations.end()) {
        throw error(subprogramName, "Implementation provided for an undeclared subprogram.");
    }

    // Parse the parameter list again to consume the tokens correctly.
    // The result is discarded, but this keeps the parser state valid.
    // TODO: A better implementation would verify this signature against the declaration.
    parameterList();

    if (subprogramKeyword.type == TokenType::FUNCTION) {
        consume(TokenType::ARROW, "Expect '->' for function implementation signature.");
        // Also discard the return type token
        advance();
    }


    std::shared_ptr<ast::KamusStmt> kamus = nullptr;
    if (check(TokenType::KAMUS)) {
        kamus = this->kamus();
    }

    std::shared_ptr<ast::AlgoritmaStmt> algoritma = this->algoritma();

    if (auto proc = std::dynamic_pointer_cast<ast::ProcedureStmt>(it->second)) {
        proc->kamus = kamus;
        proc->body = algoritma;
    } else if (auto func = std::dynamic_pointer_cast<ast::FunctionStmt>(it->second)) {
        func->kamus = kamus;
        func->body = algoritma;
    }
}

std::shared_ptr<ast::Stmt> Parser::returnStatement() {
    Token keyword = consume(TokenType::ARROW, "Expect '->'.");
    std::shared_ptr<ast::Expr> value = expression();
    return std::make_shared<ast::ReturnStmt>(keyword, value);
}


// --- Expression Parsing ---

std::shared_ptr<ast::Expr> Parser::expression() {
    return assignment();
}

std::shared_ptr<ast::Expr> Parser::assignment() {
    std::shared_ptr<ast::Expr> expr = logic_or();

    if (match({TokenType::ASSIGN})) {
        Token equals = previous();
        std::shared_ptr<ast::Expr> value = assignment();

        if (auto var = std::dynamic_pointer_cast<ast::Variable>(expr)) {
            Token name = var->name;
            return std::make_shared<ast::Assign>(name, value);
        } else if (auto fieldAccess = std::dynamic_pointer_cast<ast::FieldAccess>(expr)) {
            return std::make_shared<ast::FieldAssign>(fieldAccess, value);
        }

        throw error(equals, "Invalid assignment target.");
    }

    return expr;
}

std::shared_ptr<ast::Expr> Parser::logic_or() {
    std::shared_ptr<ast::Expr> expr = logic_and();
    while (match({TokenType::OR})) {
        Token op = previous();
        std::shared_ptr<ast::Expr> right = logic_and();
        expr = std::make_shared<ast::Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<ast::Expr> Parser::logic_and() {
    std::shared_ptr<ast::Expr> expr = equality();
    while (match({TokenType::AND})) {
        Token op = previous();
        std::shared_ptr<ast::Expr> right = equality();
        expr = std::make_shared<ast::Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<ast::Expr> Parser::equality() {
    std::shared_ptr<ast::Expr> expr = comparison();
    while (match({TokenType::NOT_EQUAL, TokenType::EQUAL})) {
        Token op = previous();
        std::shared_ptr<ast::Expr> right = comparison();
        expr = std::make_shared<ast::Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<ast::Expr> Parser::comparison() {
    std::shared_ptr<ast::Expr> expr = term();
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        std::shared_ptr<ast::Expr> right = term();
        expr = std::make_shared<ast::Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<ast::Expr> Parser::term() {
    std::shared_ptr<ast::Expr> expr = factor();
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        std::shared_ptr<ast::Expr> right = factor();
        expr = std::make_shared<ast::Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<ast::Expr> Parser::factor() {
    std::shared_ptr<ast::Expr> expr = unary();
    while (match({TokenType::DIVIDE, TokenType::MULTIPLY, TokenType::MOD, TokenType::DIV})) {
        Token op = previous();
        std::shared_ptr<ast::Expr> right = unary();
        expr = std::make_shared<ast::Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<ast::Expr> Parser::unary() {
    if (match({TokenType::NOT, TokenType::MINUS})) {
        Token op = previous();
        std::shared_ptr<ast::Expr> right = unary();
        return std::make_shared<ast::Unary>(op, right);
    }
    return call();
}

std::shared_ptr<ast::Expr> Parser::call() {
    std::shared_ptr<ast::Expr> expr = primary();
    
    while (true) {
        if (match({TokenType::LPAREN})) {
            expr = finishCall(expr);
        } else if (match({TokenType::DOT})) {
            Token name = consume(TokenType::IDENTIFIER, "Expect field name after '.'.");
            expr = std::make_shared<ast::FieldAccess>(expr, name);
        } else {
            break;
        }
    }
    
    return expr;
}

std::shared_ptr<ast::Expr> Parser::finishCall(std::shared_ptr<ast::Expr> callee) {
    std::vector<std::shared_ptr<ast::Expr>> arguments;
    if (!check(TokenType::RPAREN)) {
        do {
            // NOTAL doesn't have a hard limit, but good practice to prevent infinite loops.
            if (arguments.size() >= 255) {
                error(peek(), "Cannot have more than 255 arguments.");
            }
            arguments.push_back(expression());
        } while (match({TokenType::COMMA}));
    }

    Token paren = consume(TokenType::RPAREN, "Expect ')' after arguments.");

    return std::make_shared<ast::Call>(callee, paren, arguments);
}

std::shared_ptr<ast::Expr> Parser::primary() {
    if (match({TokenType::BOOLEAN_LITERAL})) return std::make_shared<ast::Literal>(previous().lexeme == "true");
    if (match({TokenType::INTEGER_LITERAL})) return std::make_shared<ast::Literal>(std::stoi(previous().lexeme));
    if (match({TokenType::REAL_LITERAL})) return std::make_shared<ast::Literal>(std::stod(previous().lexeme));
    if (match({TokenType::STRING_LITERAL})) return std::make_shared<ast::Literal>(previous().lexeme);
    if (match({TokenType::IDENTIFIER})) return std::make_shared<ast::Variable>(previous());
    if (match({TokenType::LPAREN})) {
        std::shared_ptr<ast::Expr> expr = expression();
        consume(TokenType::RPAREN, "Expect ')' after expression.");
        return std::make_shared<ast::Grouping>(expr);
    }
    throw error(peek(), "Expect expression.");
}

// --- Helper Methods ---

bool Parser::isAtEnd() { return peek().type == TokenType::END_OF_FILE; }
Token Parser::peek() { return tokens[current]; }
Token Parser::peekNext() { if (isAtEnd() || current + 1 >= tokens.size()) return peek(); return tokens[current + 1]; }
Token Parser::previous() { return tokens[current - 1]; }
Token Parser::advance() { if (!isAtEnd()) current++; return previous(); }
bool Parser::check(TokenType type) { if (isAtEnd()) return false; return peek().type == type; }

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

Parser::ParseError Parser::error(const Token& token, const std::string& message) {
    return ParseError(token, message);
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        switch (peek().type) {
            case TokenType::PROGRAM:
            case TokenType::KAMUS:
            case TokenType::ALGORITMA:
            case TokenType::PROCEDURE:
            case TokenType::FUNCTION:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::REPEAT:
            case TokenType::OUTPUT:
                return;
            default:
                break;
        }
        advance();
    }
}

} // namespace notal
