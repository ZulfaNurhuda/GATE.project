#include "../include/parser.hpp"
#include "../include/error_handler.hpp" // For error reporting
#include <stdexcept> // For std::runtime_error

// --- ASTNode accept method implementations ---
// These methods allow the Visitor to dispatch to the correct visit method.

void IdentifierNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void IntegerLiteralNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void StringLiteralNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void BinaryOpNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void AssignmentNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void BlockNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void IfNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void OutputNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void VariableDeclarationNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void FunctionParameterNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void FunctionPrototypeNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void SubprogramBodyNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void ProgramNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void WhileNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void RepeatUntilNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void ForNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void DependOnNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void CaseBranchNode::accept(ASTVisitor* visitor) { visitor->visit(this); }

// Base classes still need a definition for accept if they are not purely abstract
// with respect to accept, or if direct instantiation was possible (which it isn't here).
// However, since ExpressionNode, StatementNode, and DeclarationNode have pure virtual
// accept methods, they don't need a separate definition here.

// --- Parser Implementation ---

Parser::Parser() : current_token_idx(0) {
    // current_token_cache will be initialized by the first call to advance() in parse()
}

// --- Tokenizer Helper Methods ---

// Advance to the next token and update the cache
void Parser::advance() {
    if (current_token_idx < tokens.size()) {
        current_token_idx++;
    }
    if (current_token_idx < tokens.size()) {
        current_token_cache = tokens[current_token_idx];
    } else {
        // Create a synthetic EOF token if past the end
        // Line/col numbers should be from the last actual token if possible
        int line = tokens.empty() ? 0 : tokens.back().line;
        int col = tokens.empty() ? 0 : tokens.back().col + tokens.back().value.length();
        current_token_cache = Token{TokenType::EOF_TOKEN, "", line, col};
    }
}

// Check current token type without advancing
bool Parser::check(TokenType type) {
    return current_token_cache.type == type;
}

// Get the token before the current one (mostly for error reporting context)
Token Parser::previous_token() {
    if (current_token_idx > 0 && current_token_idx <= tokens.size()) {
        return tokens[current_token_idx - 1];
    }
    // Return a default or error token if out of bounds
    return Token{TokenType::UNKNOWN, "PREVIOUS_OOB", 0, 0};
}

// Consume a token of a specific type or report an error
Token Parser::consume(TokenType type, const std::string& error_message) {
    if (check(type)) {
        Token consumed_token = current_token_cache;
        advance();
        return consumed_token;
    }
    Token offending_token = current_token_cache;
    ErrorHandler::report(error_message, offending_token.line, offending_token.col);
    throw std::runtime_error("Parser error: " + error_message);
}

// If current token matches type, consume it and return true, otherwise false
bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

// --- Parsing Methods ---

// Entry point for parsing
std::unique_ptr<ProgramNode> Parser::parse(const std::vector<Token>& input_tokens) {
    if (input_tokens.empty()) {
        ErrorHandler::report("No tokens to parse.", 0, 0);
        throw std::runtime_error("Parser error: No tokens provided.");
    }
    tokens = input_tokens;
    current_token_idx = 0;
    current_token_cache = tokens[current_token_idx]; // Initialize cache

    return parseProgram();
}

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    Token program_kw = consume(TokenType::PROGRAM, "Expected 'PROGRAM' keyword.");
    Token program_name_tok = consume(TokenType::IDENTIFIER, "Expected program name.");
    consume(TokenType::SEMICOLON, "Expected ';' after program name.");

    auto program_node = std::make_unique<ProgramNode>(program_kw.line, program_kw.col, program_name_tok.value);

    // Parse Global KAMUS (Declarations)
    if (check(TokenType::KAMUS)) {
        consume(TokenType::KAMUS, "Expected 'KAMUS' keyword.");
        while (!check(TokenType::ALGORITMA) && !check(TokenType::EOF_TOKEN) &&
               (check(TokenType::FUNCTION) || check(TokenType::PROCEDURE) || check(TokenType::IDENTIFIER))) {
            // Here we'd distinguish between variable, function, and procedure declarations.
            // For now, let's assume only variable declarations in global KAMUS for simplicity.
            // And Function/Procedure prototypes might also be here.
            if (check(TokenType::IDENTIFIER)) { // Likely a variable declaration
                 program_node->global_declarations.push_back(parseVariableDeclaration());
            } else if (check(TokenType::FUNCTION) || check(TokenType::PROCEDURE)) {
                // Placeholder: parse function/procedure prototypes or full subprograms if not separated
                // For now, we'll assume subprograms are defined after main ALGORITMA
                // So this part might just be for forward declarations if your language supports it.
                // Let's skip parsing subprogram prototypes here for now.
                ErrorHandler::report("Function/Procedure prototypes in global KAMUS not yet supported.", current_token_cache.line, current_token_cache.col);
                advance(); // Skip the token to avoid infinite loop
            } else {
                ErrorHandler::report("Unexpected token in global KAMUS.", current_token_cache.line, current_token_cache.col);
                advance(); // Skip to prevent infinite loop
            }
        }
    }

    // Parse Main ALGORITMA
    consume(TokenType::ALGORITMA, "Expected 'ALGORITMA' keyword.");
    program_node->main_algoritma = parseBlock(); // Main algorithm is a block of statements

    // Parse Subprograms (Functions/Procedures) - Simplified
    while (check(TokenType::FUNCTION) || check(TokenType::PROCEDURE)) {
        program_node->subprograms.push_back(parseSubprogramBody());
    }

    // Optional: Expect a final token like "ENDPROGRAM." or just EOF
    if (check(TokenType::ENDPROGRAM)) { // Assuming ENDPROGRAM is a token
        consume(TokenType::ENDPROGRAM, "Expected 'ENDPROGRAM' keyword.");
        if(check(TokenType::DOT)) consume(TokenType::DOT, "Expected '.' after ENDPROGRAM.");
    } else if (check(TokenType::DOT) && previous_token().type == TokenType::END) { // e.g. END. for main block
         consume(TokenType::DOT, "Expected '.' at the end of the program main block.");
    }


    if (!check(TokenType::EOF_TOKEN)) {
         ErrorHandler::report("Expected EOF after program.", current_token_cache.line, current_token_cache.col);
    }

    return program_node;
}


std::unique_ptr<IdentifierNode> Parser::parseIdentifier() {
    Token id_tok = consume(TokenType::IDENTIFIER, "Expected an identifier.");
    return std::make_unique<IdentifierNode>(id_tok.line, id_tok.col, id_tok.value);
}

std::unique_ptr<IntegerLiteralNode> Parser::parseIntegerLiteral() {
    Token int_tok = consume(TokenType::INTEGER_LITERAL, "Expected an integer literal.");
    try {
        long long val = std::stoll(int_tok.value);
        return std::make_unique<IntegerLiteralNode>(int_tok.line, int_tok.col, val);
    } catch (const std::out_of_range& oor) {
        ErrorHandler::report("Integer literal out of range: " + int_tok.value, int_tok.line, int_tok.col);
        throw std::runtime_error("Parser error: Integer literal out of range.");
    }
}

std::unique_ptr<StringLiteralNode> Parser::parseStringLiteral() {
    Token str_tok = consume(TokenType::STRING_LITERAL, "Expected a string literal.");
    return std::make_unique<StringLiteralNode>(str_tok.line, str_tok.col, str_tok.value);
}


// Parses primary expressions (identifiers, literals)
std::unique_ptr<ExpressionNode> Parser::parsePrimaryExpression() {
    if (check(TokenType::IDENTIFIER)) {
        return parseIdentifier();
    } else if (check(TokenType::INTEGER_LITERAL)) {
        return parseIntegerLiteral();
    } else if (check(TokenType::STRING_LITERAL)) {
        return parseStringLiteral();
    } else if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after expression in parentheses.");
        return expr;
    }
    // Add other primary expressions like boolean literals, function calls, etc.
    Token offending_token = current_token_cache;
    ErrorHandler::report(ErrorCode::SYNTAX_INVALID_EXPRESSION, offending_token.line, offending_token.col, "Unexpected token in expression: " + offending_token.value);
    throw std::runtime_error("Parser error: Invalid expression component.");
}

// Parses binary operations (simplified: only one level of precedence for now)
std::unique_ptr<ExpressionNode> Parser::parseExpression() {
    auto left = parsePrimaryExpression(); // Start with a primary expression

    // Rudimentary binary operator parsing (no precedence or associativity yet)
    // This would need a proper Pratt parser or precedence climbing for full support.
    while (check(TokenType::PLUS) || check(TokenType::MINUS) || check(TokenType::MULTIPLY) || check(TokenType::DIVIDE) ||
           check(TokenType::LESS_THAN) || check(TokenType::GREATER_THAN) || check(TokenType::EQUAL) /* add more ops */ ) {
        Token op_token = current_token_cache;
        advance(); // Consume the operator
        auto right = parsePrimaryExpression(); // Parse the right-hand side

        // Create BinaryOpNode - line/col from operator for now
        left = std::make_unique<BinaryOpNode>(op_token.line, op_token.col, std::move(left), op_token.value, std::move(right));
    }
    return left;
}


std::unique_ptr<StatementNode> Parser::parseAssignmentStatement(std::unique_ptr<IdentifierNode> target) {
    Token assign_op = consume(TokenType::ASSIGN_OP, "Expected assignment operator '<-'."); // Assuming ASSIGN_OP is '<-'
    auto value = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after assignment statement.");
    return std::make_unique<AssignmentNode>(target->line, target->col, std::move(target), std::move(value));
}

std::unique_ptr<OutputNode> Parser::parseOutputStatement() {
    Token output_kw = consume(TokenType::OUTPUT, "Expected 'OUTPUT' keyword."); // Or CETAK, PRINT
    auto output_node = std::make_unique<OutputNode>(output_kw.line, output_kw.col);

    // Assuming output can take multiple comma-separated expressions, e.g., OUTPUT expr1, expr2
    // Or a single expression, or parenthesized list. For now, let's assume one or more.
    // Example: OUTPUT expression SEMICOLON
    // Example: OUTPUT LPAREN expression (COMMA expression)* RPAREN SEMICOLON

    bool expect_paren = check(TokenType::LPAREN);
    if(expect_paren) {
        consume(TokenType::LPAREN, "Expected '(' after OUTPUT if not directly followed by expression.");
    }

    output_node->expressions.push_back(parseExpression());
    while (match(TokenType::COMMA)) {
        output_node->expressions.push_back(parseExpression());
    }

    if(expect_paren) {
        consume(TokenType::RPAREN, "Expected ')' after OUTPUT expressions.");
    }
    consume(TokenType::SEMICOLON, "Expected ';' after output statement.");
    return output_node;
}

std::unique_ptr<IfNode> Parser::parseIfStatement() {
    Token if_tok = consume(TokenType::IF, "Expected 'IF' keyword.");
    auto condition = parseExpression();
    consume(TokenType::THEN, "Expected 'THEN' after IF condition.");

    auto then_block = parseBlock(); // then_block could be single statement or block

    std::unique_ptr<BlockNode> else_block = nullptr;
    if (match(TokenType::ELSE)) {
        else_block = parseBlock(); // else_block could also be single statement or block
    }
    // Optional: consume ENDIF if your language uses it
    // match(TokenType::ENDIF);
    // consume(TokenType::SEMICOLON, "Expected ';' after IF statement or ENDIF."); // Depending on language

    return std::make_unique<IfNode>(if_tok.line, if_tok.col, std::move(condition), std::move(then_block), std::move(else_block));
}


std::unique_ptr<StatementNode> Parser::parseStatement() {
    if (check(TokenType::IDENTIFIER)) {
        // Could be an assignment or a procedure call.
        // For now, assume it's the start of an assignment.
        // A more robust parser would use lookahead to differentiate.
        auto identifier = parseIdentifier();
        // If it's an assignment, next token should be ASSIGN_OP
        if (check(TokenType::ASSIGN_OP)) {
            return parseAssignmentStatement(std::move(identifier));
        }
        // Else, it could be a procedure call (not implemented yet in this simplified version)
        // For now, error if not assignment
        Token offending_token = current_token_cache;
    ErrorHandler::report(ErrorCode::SYNTAX_MISSING_EXPECTED_TOKEN, offending_token.line, offending_token.col, "Expected assignment operator after identifier in statement.");
        throw std::runtime_error("Parser error: Invalid statement (expected assignment).");

    } else if (check(TokenType::OUTPUT)) {
        return parseOutputStatement();
    } else if (check(TokenType::IF)) {
        return parseIfStatement();
    } else if (check(TokenType::WHILE)) {
        return parseWhileStatement();
    } else if (check(TokenType::REPEAT)) {
        return parseRepeatUntilStatement();
    } else if (check(TokenType::FOR)) {
        return parseForStatement();
    } else if (check(TokenType::DEPEND)) {
        return parseDependOnStatement();
    }
    // Add other statement types: READ, etc.

    Token offending_token = current_token_cache;
    ErrorHandler::report(ErrorCode::SYNTAX_UNEXPECTED_TOKEN, offending_token.line, offending_token.col, "Unexpected token, cannot parse statement: " + offending_token.value);
    throw std::runtime_error("Parser error: Invalid statement.");
}


std::unique_ptr<StatementNode> Parser::parseWhileStatement() {
    Token while_tok = consume(TokenType::WHILE, "Expected 'WHILE' keyword.");
    consume(TokenType::LPAREN, "Expected '(' after WHILE.");
    auto condition = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after WHILE condition.");
    consume(TokenType::DO, "Expected 'DO' after WHILE condition."); // Assuming AN grammar might use DO here
    auto body = parseBlock(); // Assumes parseBlock handles statements until ENDWHILE or similar
    consume(TokenType::ENDWHILE, "Expected 'ENDWHILE' to close WHILE loop.");
    return std::make_unique<WhileNode>(while_tok.line, while_tok.col, std::move(condition), std::move(body));
}

std::unique_ptr<StatementNode> Parser::parseRepeatUntilStatement() {
    Token repeat_tok = consume(TokenType::REPEAT, "Expected 'REPEAT' keyword.");

    auto body_block = std::make_unique<BlockNode>(repeat_tok.line, repeat_tok.col);
    while (!check(TokenType::UNTIL) && !check(TokenType::EOF_TOKEN)) {
        body_block->statements.push_back(parseStatement());
    }

    consume(TokenType::UNTIL, "Expected 'UNTIL' keyword after REPEAT block.");
    consume(TokenType::LPAREN, "Expected '(' after UNTIL.");
    auto condition = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after UNTIL condition.");
    consume(TokenType::SEMICOLON, "Expected ';' after REPEAT-UNTIL statement."); // Common, but check AN spec
    return std::make_unique<RepeatUntilNode>(repeat_tok.line, repeat_tok.col, std::move(body_block), std::move(condition));
}

std::unique_ptr<StatementNode> Parser::parseForStatement() {
    Token for_tok = consume(TokenType::FOR, "Expected 'FOR' keyword.");
    auto loop_var = parseIdentifier();
    consume(TokenType::ASSIGN_OP, "Expected '<-' assignment operator in FOR loop.");
    auto start_value = parseExpression();
    consume(TokenType::TO, "Expected 'TO' keyword in FOR loop.");
    auto end_value = parseExpression();
    consume(TokenType::DO, "Expected 'DO' keyword in FOR loop.");
    auto body = parseBlock(); // Assumes parseBlock handles statements until ENDFOR
    consume(TokenType::ENDFOR, "Expected 'ENDFOR' to close FOR loop.");
    return std::make_unique<ForNode>(for_tok.line, for_tok.col, std::move(loop_var), std::move(start_value), std::move(end_value), std::move(body));
}


std::unique_ptr<CaseBranchNode> Parser::parseCaseBranch() {
    Token case_branch_start_tok = current_token_cache; // For line/col info

    if (match(TokenType::CASE)) {
        auto case_value = parseExpression(); // Assuming case value is an expression
        consume(TokenType::COLON, "Expected ':' after CASE value.");
        auto body = parseBlock(); // Or parseStatementsUntil another CASE/OTHERWISE/ENDDEPENDON
        consume(TokenType::SEMICOLON, "Expected ';' after statements in CASE branch."); // Assuming typical AN statement termination
        return std::make_unique<CaseBranchNode>(case_branch_start_tok.line, case_branch_start_tok.col, std::move(case_value), std::move(body));
    } else if (match(TokenType::OTHERWISE)) {
        consume(TokenType::COLON, "Expected ':' after OTHERWISE.");
        auto body = parseBlock();
        consume(TokenType::SEMICOLON, "Expected ';' after statements in OTHERWISE branch."); // Assuming typical AN statement termination
        return std::make_unique<CaseBranchNode>(case_branch_start_tok.line, case_branch_start_tok.col, std::move(body));
    } else {
        ErrorHandler::report(ErrorCode::SYNTAX_UNEXPECTED_TOKEN, case_branch_start_tok.line, case_branch_start_tok.col, "Expected 'CASE' or 'OTHERWISE'.");
        throw std::runtime_error("Parser error: Invalid case branch.");
        // return nullptr; // Should not be reached due to throw
    }
}

std::unique_ptr<StatementNode> Parser::parseDependOnStatement() {
    Token depend_tok = consume(TokenType::DEPEND, "Expected 'DEPEND' keyword.");
    consume(TokenType::ON, "Expected 'ON' keyword after DEPEND.");
    consume(TokenType::LPAREN, "Expected '(' after DEPEND ON.");
    auto control_var = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after control variable in DEPEND ON.");

    auto depend_on_node = std::make_unique<DependOnNode>(depend_tok.line, depend_tok.col, std::move(control_var));

    bool found_case_or_otherwise = false;
    while (check(TokenType::CASE) || check(TokenType::OTHERWISE)) {
        depend_on_node->cases.push_back(parseCaseBranch());
        found_case_or_otherwise = true;
        if (previous_token().type == TokenType::OTHERWISE && check(TokenType::CASE)) { // Check if OTHERWISE was last parsed
             ErrorHandler::report(ErrorCode::SYNTAX_ERROR, current_token_cache.line, current_token_cache.col, "'CASE' cannot appear after 'OTHERWISE'.");
             throw std::runtime_error("Parser error: 'CASE' after 'OTHERWISE'.");
        }
         if (check(TokenType::OTHERWISE) && std::any_of(depend_on_node->cases.begin(), depend_on_node->cases.end(),
            [](const auto& cb){ return cb->is_otherwise(); })) {
            if(depend_on_node->cases.back()->is_otherwise()){ // If last added was otherwise
                 ErrorHandler::report(ErrorCode::SYNTAX_ERROR, current_token_cache.line, current_token_cache.col, "Multiple 'OTHERWISE' clauses are not allowed.");
                 throw std::runtime_error("Parser error: Multiple 'OTHERWISE' clauses.");
            }
        }
    }

    if (!found_case_or_otherwise) {
        ErrorHandler::report(ErrorCode::SYNTAX_MISSING_EXPECTED_TOKEN, current_token_cache.line, current_token_cache.col, "Expected at least one 'CASE' or 'OTHERWISE' in DEPEND ON statement.");
        throw std::runtime_error("Parser error: Empty DEPEND ON statement.");
    }

    consume(TokenType::ENDDEPENDON, "Expected 'ENDDEPENDON' to close DEPEND ON statement.");
    consume(TokenType::SEMICOLON, "Expected ';' after ENDDEPENDON."); // Assuming typical AN statement termination
    return depend_on_node;
}


std::unique_ptr<BlockNode> Parser::parseBlock() {
    // A block is a sequence of statements.
    // In some languages, blocks are delimited by BEGIN/END or {}.
    // For this simplified version, let's assume a block is parsed until
    // a token that cannot start a new statement is found, or a specific
    // end-of-block token (like END, ELSE, ENDIF, ENDWHILE, EOF for main block).
    // This will need refinement based on the specific language grammar.

    Token block_start_tok = current_token_cache; // For line/col info of the block itself
    auto block_node = std::make_unique<BlockNode>(block_start_tok.line, block_start_tok.col);

    // Example: A block ends if we see ELSE, ENDIF, ENDWHILE, FUNCTION, PROCEDURE, EOF, END (for main block)
    // This list needs to be context-dependent.
    while (!check(TokenType::END) &&         // General END keyword (e.g. for main block)
           !check(TokenType::ELSE) &&        // For IF statements
           !check(TokenType::ENDIF) &&       // If ENDIF is a keyword
           !check(TokenType::ENDWHILE) &&    // If ENDWHILE is a keyword
           !check(TokenType::ENDFOR) &&      // If ENDFOR is a keyword
           !check(TokenType::FUNCTION) &&    // Start of a new subprogram
           !check(TokenType::PROCEDURE) &&   // Start of a new subprogram
           !check(TokenType::EOF_TOKEN) &&
           !check(TokenType::ENDPROGRAM) &&  // End of the whole program
           !check(TokenType::UNTIL)          // For REPEAT..UNTIL loops
           /* other stoppers */ ) {

        // Check if the current token can start a statement
        // This is a simplified check. A real parser might need more lookahead or specific BEGIN/END for blocks.
        if (check(TokenType::IDENTIFIER) || check(TokenType::OUTPUT) || check(TokenType::IF) ||
            check(TokenType::WHILE) || check(TokenType::FOR) || check(TokenType::READ) || check(TokenType::RETURN)
            /* other statement starters */) {
            block_node->statements.push_back(parseStatement());
        } else {
            // If it's not a statement starter and not an end-of-block token, it might be an error
            // or the end of an implicit block. For now, we break.
            break;
        }
    }
    return block_node;
}

std::unique_ptr<VariableDeclarationNode> Parser::parseVariableDeclaration() {
    auto var_name_node = parseIdentifier();
    consume(TokenType::COLON, "Expected ':' after variable name in declaration.");
    // For type, it could be a basic type keyword (INTEGER, STRING) or another IDENTIFIER (for custom types)
    Token type_tok = current_token_cache;
    if (type_tok.type == TokenType::TYPE_INTEGER || type_tok.type == TokenType::TYPE_STRING ||
        type_tok.type == TokenType::TYPE_BOOLEAN || type_tok.type == TokenType::TYPE_CHARACTER || // Added TYPE_CHARACTER
        type_tok.type == TokenType::TYPE_REAL    || // Added TYPE_REAL
        type_tok.type == TokenType::IDENTIFIER) {
        advance(); // Consume the type token
    } else {
        ErrorHandler::report(ErrorCode::SYNTAX_INVALID_EXPRESSION, type_tok.line, type_tok.col, "Expected type identifier (e.g., integer, string) or custom type name.");
        throw std::runtime_error("Parser error: Invalid type in variable declaration.");
    }
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");
    return std::make_unique<VariableDeclarationNode>(var_name_node->line, var_name_node->col, var_name_node->name, type_tok.value);
}


std::unique_ptr<FunctionParameterNode> Parser::parseFunctionParameter() {
    // Example: `x : integer` or `INPUT x : integer` or `OUTPUT y : string`
    // This needs to be adapted to the specific language's parameter syntax.
    // For now, let's assume `var_name : type_name`
    Token param_name_tok = consume(TokenType::IDENTIFIER, "Expected parameter name.");
    consume(TokenType::COLON, "Expected ':' after parameter name.");
    Token param_type_tok = current_token_cache;
    if (param_type_tok.type == TokenType::TYPE_INTEGER || param_type_tok.type == TokenType::TYPE_STRING ||
        param_type_tok.type == TokenType::TYPE_BOOLEAN || param_type_tok.type == TokenType::TYPE_CHARACTER || // Added
        param_type_tok.type == TokenType::TYPE_REAL    || // Added
        param_type_tok.type == TokenType::IDENTIFIER) {
        advance(); // Consume type
    } else {
        ErrorHandler::report(ErrorCode::SYNTAX_INVALID_EXPRESSION, param_type_tok.line, param_type_tok.col, "Expected type for parameter " + param_name_tok.value);
        throw std::runtime_error("Parser error: Invalid type in parameter declaration.");
    }
    return std::make_unique<FunctionParameterNode>(param_name_tok.line, param_name_tok.col, param_name_tok.value, param_type_tok.value);
}


std::unique_ptr<FunctionPrototypeNode> Parser::parseFunctionPrototype() {
    Token func_proc_kw = current_token_cache; // FUNCTION or PROCEDURE
    advance(); // Consume FUNCTION or PROCEDURE

    Token func_name_tok = consume(TokenType::IDENTIFIER, "Expected function/procedure name.");

    auto proto_node = std::make_unique<FunctionPrototypeNode>(func_proc_kw.line, func_proc_kw.col, func_name_tok.value, "");

    // Parameters
    if (match(TokenType::LPAREN)) {
        if (!check(TokenType::RPAREN)) { // If there are parameters
            do {
                proto_node->parameters.push_back(parseFunctionParameter());
            } while (match(TokenType::COMMA));
        }
        consume(TokenType::RPAREN, "Expected ')' after function parameters.");
    }

    // Return type (only for FUNCTION)
    if (func_proc_kw.type == TokenType::FUNCTION) {
        consume(TokenType::COLON, "Expected ':' before function return type.");
        Token return_type_tok = current_token_cache;
        if (return_type_tok.type == TokenType::TYPE_INTEGER || return_type_tok.type == TokenType::TYPE_STRING ||
            return_type_tok.type == TokenType::TYPE_BOOLEAN || return_type_tok.type == TokenType::TYPE_CHARACTER || // Added
            return_type_tok.type == TokenType::TYPE_REAL    || // Added
            return_type_tok.type == TokenType::IDENTIFIER ||
            return_type_tok.type == TokenType::TYPE_VOID ) {
            advance(); // Consume return type
            proto_node->return_type = return_type_tok.value;
        } else {
            ErrorHandler::report(ErrorCode::SYNTAX_INVALID_EXPRESSION, return_type_tok.line, return_type_tok.col, "Expected return type for function " + func_name_tok.value);
            throw std::runtime_error("Parser error: Invalid return type.");
        }
    } else { // PROCEDURE
        proto_node->return_type = "void"; // Implicitly void
    }
    consume(TokenType::SEMICOLON, "Expected ';' after function/procedure prototype.");
    return proto_node;
}

std::unique_ptr<SubprogramBodyNode> Parser::parseSubprogramBody() {
    // This assumes the FUNCTION/PROCEDURE keyword and prototype is parsed first.
    // The structure is: Prototype KAMUS_LOKAL ALGORITMA_SUBPROGRAM

    auto prototype = parseFunctionPrototype(); // This will consume the initial FUNCTION/PROCEDURE keyword and semicolon

    // Line/col for SubprogramBodyNode can be from the prototype
    auto subprogram_node = std::make_unique<SubprogramBodyNode>(prototype->line, prototype->col, std::move(prototype), nullptr);

    // KAMUS LOKAL (Local Declarations)
    if (check(TokenType::KAMUS)) {
        consume(TokenType::KAMUS, "Expected 'KAMUS' for local declarations.");
        while (check(TokenType::IDENTIFIER) && !check(TokenType::ALGORITMA)) {
            // Assuming local declarations are similar to global ones (variable declarations)
            subprogram_node->local_declarations.push_back(parseVariableDeclaration());
        }
    }

    // ALGORITMA (Subprogram Body)
    consume(TokenType::ALGORITMA, "Expected 'ALGORITMA' for subprogram body.");
    subprogram_node->body = parseBlock();

    // Optional: Expect ENDFUNCTION or ENDPROCEDURE or a general END
    // consume(TokenType::END, "Expected 'END' after subprogram body."); // Or ENDFUNCTION/ENDPROCEDURE
    // consume(TokenType::SEMICOLON, "Expected ';' after END.");
    // This part depends heavily on the language's specific keywords for ending subprograms.
    // For now, the block parsing logic might handle the end implicitly.

    return subprogram_node;
}

// Placeholder for ASTVisitor visit methods (should be in a visitor implementation file)
// class ASTVisitor { public: ... };
// void ASTVisitor::visit(IdentifierNode* node) { /* ... */ }
// ... and so on for all node types.
// These are not part of the Parser itself but are called by node->accept(this).
// The actual ASTVisitor class and its methods would be defined elsewhere.
// For this file, we only need the `accept` methods in the nodes themselves.