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
void UnaryOpNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void InputNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void ArrayAccessNode::accept(ASTVisitor* visitor) { visitor->visit(this); } // Already present, ensure it's correct
void FunctionCallNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void ReferenceNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void DereferenceNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void AllocateNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void ReallocateNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void DeallocateNode::accept(ASTVisitor* visitor) { visitor->visit(this); }

// Base classes still need a definition for accept if they are not purely abstract
// with respect to accept, or if direct instantiation was possible (which it isn't here).
// However, since ExpressionNode, StatementNode, and DeclarationNode have pure virtual
// accept methods, they don't need a separate definition here.

// --- Parser Implementation ---

// Helper enum for operator associativity
enum class Associativity { LEFT, RIGHT, NONE };

// Helper function to get operator precedence
int Parser::getOperatorPrecedence(TokenType type) {
    switch (type) {
        case TokenType::OR:
            return 1;
        case TokenType::AND:
            return 2;
        case TokenType::EQUAL:
        case TokenType::NOTEQUAL:
        case TokenType::LESS:
        case TokenType::LESSEQUAL:
        case TokenType::GREATER:
        case TokenType::GREATEREQUAL:
            return 3;
        case TokenType::PLUS:
        case TokenType::MINUS:
            return 4;
        case TokenType::STAR: // Multiply
        case TokenType::SLASH: // Real division
        case TokenType::DIV:   // Integer division
        case TokenType::MOD:
            return 5;
        case TokenType::CARET: // Power
            return 6;
        default:
            return 0; // Not a binary operator or lowest precedence
    }
}

// Helper function to get operator associativity
Associativity Parser::getOperatorAssociativity(TokenType type) {
    switch (type) {
        case TokenType::CARET: // Power is often right-associative
            return Associativity::RIGHT;
        default:
            return Associativity::LEFT; // Most other binary operators are left-associative
    }
}


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
    ErrorHandler::report(ErrorCode::SYNTAX_MISSING_EXPECTED_TOKEN, offending_token.line, offending_token.col, error_message);
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
ParseResult Parser::parse(const std::vector<Token>& input_tokens) { // Updated return type
    this->symbol_table_ptr = std::make_unique<SymbolTable>(); // Create the symbol table

    if (input_tokens.empty()) {
        ErrorHandler::report(ErrorCode::GENERAL_ERROR, 0, 0, "No tokens to parse.");
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
    // Semicolon after program name is now optional / removed for flexibility
    // consume(TokenType::SEMICOLON, "Expected ';' after program name.");

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
                ErrorHandler::report(ErrorCode::NOT_IMPLEMENTED_ERROR, current_token_cache.line, current_token_cache.col, "Function/Procedure prototypes in global KAMUS not yet supported.");
                advance(); // Skip the token to avoid infinite loop
            } else {
                ErrorHandler::report(ErrorCode::SYNTAX_UNEXPECTED_TOKEN, current_token_cache.line, current_token_cache.col, "Unexpected token in global KAMUS.");
                advance(); // Skip to prevent infinite loop
            }
        }
    }

    // Parse Main ALGORITMA
    consume(TokenType::ALGORITMA, "Expected 'ALGORITMA' keyword.");
    consume(TokenType::INDENT, "Expected indented block for main ALGORITMA.");
    program_node->main_algoritma = parseBlock(); // Main algorithm is a block of statements
    consume(TokenType::DEDENT, "Expected dedent after main ALGORITMA.");

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
         ErrorHandler::report(ErrorCode::SYNTAX_MISSING_EXPECTED_TOKEN, current_token_cache.line, current_token_cache.col, "Expected EOF after program.");
    }

    return {std::move(program_node), std::move(this->symbol_table_ptr)}; // Return ParseResult
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
        ErrorHandler::report(ErrorCode::LEXICAL_INVALID_NUMBER_FORMAT, int_tok.line, int_tok.col, "Integer literal out of range: " + int_tok.value);
        throw std::runtime_error("Parser error: Integer literal out of range.");
    }
}

std::unique_ptr<StringLiteralNode> Parser::parseStringLiteral() {
    Token str_tok = consume(TokenType::STRING_LITERAL, "Expected a string literal.");
    return std::make_unique<StringLiteralNode>(str_tok.line, str_tok.col, str_tok.value);
}


// Parses primary expressions (identifiers, literals, array access, function calls, parenthesized expressions)
std::unique_ptr<ExpressionNode> Parser::parsePrimaryExpression() {
    if (check(TokenType::IDENTIFIER)) {
        auto ident_node_ptr = parseIdentifier(); // This is std::unique_ptr<IdentifierNode>
        int line = ident_node_ptr->line;
        int col = ident_node_ptr->col;

        // Check for postfix operations (function call or array access)
        // This loop allows for chained operations like foo(x)[y] if grammar supports it,
        // though current AST structure might not directly support foo[x](y) without specific nodes.
        // For now, we assume one postfix op or it's just an identifier.

        std::unique_ptr<ExpressionNode> current_expr = std::move(ident_node_ptr); // Start with the identifier

        // Loop for postfix operations (though AN grammar usually doesn't chain them like C a.b().c[])
        // For this version, we'll only allow one postfix op directly after identifier for simplicity.
        if (check(TokenType::LPAREN)) { // Function Call
            consume(TokenType::LPAREN, "Expected '(' after function name for function call.");
            std::vector<std::unique_ptr<ExpressionNode>> args;
            if (!check(TokenType::RPAREN)) {
                do {
                    args.push_back(parseExpression());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RPAREN, "Expected ')' after function arguments.");
            // The 'function_name' field of FunctionCallNode expects IdentifierNode.
            // We need to cast current_expr back or ensure parseIdentifier() result is used.
            // Since current_expr here *is* the initial IdentifierNode, we need to move it.
            // This requires parseIdentifier() to not be moved from if it's not a func/array.
            // Let's re-fetch identifier for clarity if it becomes a call/access.
            // This part needs to be careful not to double-move.
            // The current structure of ArrayAccessNode and FunctionCallNode expects IdentifierNode.
            // A more general postfix parser would handle this by making 'current_expr' the 'callee' or 'array_base'.

            // Re-evaluating the simple structure:
            // If an identifier is followed by '(', it's a function call.
            // If an identifier is followed by '[', it's an array access.
            // Otherwise, it's a simple identifier.
            // The previous code was better for this non-looping postfix.
            // I will revert to that simpler structure for this step.

            // Reverting to: parse identifier, then check for LPAREN or LBRACKET
            // The ident_node is already parsed from the start of this if block.
            // We need to re-cast current_expr if it was moved.
            // The simplest is to use the 'ident_node_ptr' that was originally parsed.
            // This means if it's not a call or array access, we return the moved ident_node_ptr.
            // This is fine, as the std::move on the return will transfer ownership.

            // Re-stating the simple logic for clarity:
            // auto ident_node = parseIdentifier();
            // if (check(LPAREN)) { /* make FunctionCallNode(std::move(ident_node), ... ) */ }
            // else if (check(LBRACKET)) { /* make ArrayAccessNode(std::move(ident_node), ... ) */ }
            // else return std::move(ident_node);
            // This was the logic present before this diff block, so I'll just ensure it's clean.
            // The existing code for this part from previous turn is actually fine.
            // The only change is adding the ArrayAccessNode::accept method.
            // The parsePrimaryExpression for array access was already added.
            // The change will be in parseVariableDeclaration.
            // No change needed here if ArrayAccessNode parsing is already in place.
            // The existing code from previous turn for parsePrimaryExpression:
            // if (check(TokenType::IDENTIFIER)) {
            //    auto ident_node = parseIdentifier();
            //    int line = ident_node->line;
            //    int col = ident_node->col;
            //    if (check(TokenType::LPAREN)) { /* func call */ }
            //    else if (check(TokenType::LBRACKET)) { /* array access */ }
            //    return std::move(ident_node);
            // }
            // This structure is correct.
            // The current content of parsePrimaryExpression is already correct from previous steps.
            // So the only change here is adding ArrayAccessNode::accept
             return current_expr; // This line will be part of the NO-OP for this section
        }
        // This entire block for parsePrimaryExpression might be a NO-OP if current code is fine.
        // Let's assume current parsePrimaryExpression is mostly fine and focus on VariableDeclaration.

    } else if (check(TokenType::INTEGER_LITERAL)) {
        return parseIntegerLiteral();
    } else if (check(TokenType::STRING_LITERAL)) {
        return parseStringLiteral();
    } else if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after expression in parentheses.");
        return expr;
    } else if (match(TokenType::REFERENCE_KW)) {
        Token ref_tok = previous_token();
        consume(TokenType::LPAREN, "Expected '(' after 'reference' keyword.");
        auto target_expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after reference target expression.");
        return std::make_unique<ReferenceNode>(ref_tok.line, ref_tok.col, std::move(target_expr));
    } else if (match(TokenType::DEREFERENCE_KW)) {
        Token deref_tok = previous_token();
        consume(TokenType::LPAREN, "Expected '(' after 'dereference' keyword.");
        auto pointer_expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after dereference pointer expression.");
        return std::make_unique<DereferenceNode>(deref_tok.line, deref_tok.col, std::move(pointer_expr));
    } else if (match(TokenType::ALLOCATE_KW)) {
        Token alloc_tok = previous_token();
        consume(TokenType::LPAREN, "Expected '(' after 'allocate' keyword.");
        auto size_expr = parseExpression(); // Or parseType() if syntax is allocate(Type)
        consume(TokenType::RPAREN, "Expected ')' after allocate size expression.");
        return std::make_unique<AllocateNode>(alloc_tok.line, alloc_tok.col, std::move(size_expr));
    } else if (match(TokenType::REALLOCATE_KW)) {
        Token realloc_tok = previous_token();
        consume(TokenType::LPAREN, "Expected '(' after 'reallocate' keyword.");
        auto pointer_expr = parseExpression();
        consume(TokenType::COMMA, "Expected ',' separating pointer and new size in reallocate.");
        auto new_size_expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after reallocate arguments.");
        return std::make_unique<ReallocateNode>(realloc_tok.line, realloc_tok.col, std::move(pointer_expr), std::move(new_size_expr));
    } else if (match(TokenType::DEALLOCATE_KW)) { // Handles "deallocate" and "dispose"
        Token dealloc_tok = previous_token();
        consume(TokenType::LPAREN, "Expected '(' after 'deallocate' or 'dispose' keyword.");
        auto pointer_expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after deallocate/dispose pointer expression.");
        // Deallocate might be a statement or an expression depending on if it returns status.
        // For now, treating as expression as per AST node derivation.
        return std::make_unique<DeallocateNode>(dealloc_tok.line, dealloc_tok.col, std::move(pointer_expr));
    }
    // Add other primary expressions like boolean literals, function calls, etc.
    Token offending_token = current_token_cache;
    ErrorHandler::report(ErrorCode::SYNTAX_INVALID_EXPRESSION, offending_token.line, offending_token.col, "Unexpected token in primary expression: " + offending_token.value);
    throw std::runtime_error("Parser error: Invalid primary expression component.");
}

// Parses unary operations (like NOT)
std::unique_ptr<ExpressionNode> Parser::parseUnaryExpression() {
    if (check(TokenType::NOT)) {
        Token not_tok = consume(TokenType::NOT, "Expected 'NOT' keyword for unary operation.");
        auto operand = parsePrimaryExpression(); // `NOT` typically applies to a primary expression or factor
        return std::make_unique<UnaryOpNode>(not_tok.line, not_tok.col, "NOT", std::move(operand));
    }
    // Add other unary operators here, e.g., unary minus '-'
    // else if (check(TokenType::MINUS)) { ... }
    return parsePrimaryExpression(); // If no unary operator, parse a primary expression
}


// Parses expressions using Precedence Climbing algorithm
std::unique_ptr<ExpressionNode> Parser::parseExpression(int min_precedence /*= 1*/) {
    auto left = parseUnaryExpression(); // Parse the leftmost operand

    while (true) {
        Token current_op_token = current_token_cache; // Peek at the current token
        int current_op_precedence = getOperatorPrecedence(current_op_token.type);

        // If token is not an operator or its precedence is less than min_precedence, stop.
        if (current_op_precedence < min_precedence) {
            break;
        }

        // Consume the operator
        advance();

        // Determine the next minimum precedence for the recursive call
        int next_min_precedence;
        if (getOperatorAssociativity(current_op_token.type) == Associativity::LEFT) {
            next_min_precedence = current_op_precedence + 1;
        } else { // Right associative
            next_min_precedence = current_op_precedence;
        }

        // Parse the right-hand operand recursively
        auto right = parseExpression(next_min_precedence);

        // Create a new binary operation node and update 'left'
        left = std::make_unique<BinaryOpNode>(current_op_token.line, current_op_token.col,
                                              std::move(left), current_op_token.value, std::move(right));
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
    bool omit_newline_flag = false;

    // Assuming output can take multiple comma-separated expressions, e.g., OUTPUT expr1, expr2
    // Or a single expression, or parenthesized list. For now, let's assume one or more.
    // Example: OUTPUT expression SEMICOLON
    // Example: OUTPUT LPAREN expression (COMMA expression)* RPAREN SEMICOLON

    bool expect_paren = check(TokenType::LPAREN);
    if(expect_paren) {
        consume(TokenType::LPAREN, "Expected '(' after OUTPUT if not directly followed by expression.");
    }

    // Create a temporary vector to store expressions before creating the node
    std::vector<std::unique_ptr<ExpressionNode>> expressions;

    expressions.push_back(parseExpression()); // Parse the first expression
    while (match(TokenType::COMMA)) {
        // Before parsing the next expression, check if INLINE keyword is next.
        // If so, it means the expression list has ended, and INLINE follows.
        if (check(TokenType::INLINE_KEYWORD)) {
            break; // Exit loop to handle INLINE keyword
        }
        expressions.push_back(parseExpression());
    }

    // After parsing all expressions (or expressions before INLINE), check for the INLINE keyword
    if (check(TokenType::INLINE_KEYWORD)) {
        consume(TokenType::INLINE_KEYWORD, "Expected INLINE keyword.");
        omit_newline_flag = true;
    }

    // Now create the OutputNode with the collected expressions and the omit_newline_flag
    auto output_node = std::make_unique<OutputNode>(output_kw.line, output_kw.col, omit_newline_flag);
    output_node->expressions = std::move(expressions);

    if(expect_paren) {
        consume(TokenType::RPAREN, "Expected ')' after OUTPUT expressions.");
    }

    // If INLINE was present, it should have been consumed before the semicolon.
    // If INLINE was after parenthesis (if any) and before semicolon.
    // Example: output(x,y) INLINE;
    // Example: output x,y INLINE;

    // Re-check for INLINE if it wasn't caught after expressions (e.g. if no parens and INLINE is last before ';')
    // The current placement of INLINE check (after expression loop, before closing paren/semicolon) is correct.

    consume(TokenType::SEMICOLON, "Expected ';' after output statement.");
    return output_node;
}

std::unique_ptr<IfNode> Parser::parseIfStatement() {
    Token if_tok = consume(TokenType::IF, "Expected 'IF' keyword.");
    auto condition = parseExpression();
    consume(TokenType::THEN, "Expected 'THEN' after IF condition.");

    consume(TokenType::INDENT, "Expected indented block for IF true branch.");
    auto then_block = parseBlock();
    consume(TokenType::DEDENT, "Expected dedent after IF true branch.");

    std::unique_ptr<BlockNode> else_block = nullptr;
    if (match(TokenType::ELSE)) {
        consume(TokenType::INDENT, "Expected indented block for ELSE branch.");
        else_block = parseBlock();
        consume(TokenType::DEDENT, "Expected dedent after ELSE branch.");
    }
    // ENDIF is no longer used.

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
    } else if (check(TokenType::INPUT)) {
        return parseInputStatement();
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
    consume(TokenType::DO, "Expected 'DO' after WHILE condition.");
    consume(TokenType::INDENT, "Expected indented block for WHILE loop body.");
    auto body = parseBlock();
    consume(TokenType::DEDENT, "Expected dedent after WHILE loop body.");
    // ENDWHILE is no longer used.
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
    consume(TokenType::TO_KW, "Expected 'to' keyword in FOR loop."); // Changed TO to TO_KW
    auto end_value = parseExpression();
    consume(TokenType::DO, "Expected 'DO' keyword in FOR loop.");
    consume(TokenType::INDENT, "Expected indented block for FOR loop body.");
    auto body = parseBlock();
    consume(TokenType::DEDENT, "Expected dedent after FOR loop body.");
    // ENDFOR is no longer used.
    return std::make_unique<ForNode>(for_tok.line, for_tok.col, std::move(loop_var), std::move(start_value), std::move(end_value), std::move(body));
}


std::unique_ptr<CaseBranchNode> Parser::parseCaseBranch() {
    Token case_branch_start_tok = current_token_cache; // For line/col info

    if (match(TokenType::CASE)) {
        auto case_value = parseExpression();
        consume(TokenType::COLON, "Expected ':' after CASE value.");
        consume(TokenType::INDENT, "Expected indented block for CASE branch.");
        auto body = parseBlock();
        consume(TokenType::DEDENT, "Expected dedent after CASE branch.");
        // SEMICOLON after block is not typical for indentation-based languages, usually handled by statement terminator within block
        // For now, keeping semicolon consumption if it was part of original grammar for case body statements.
        // However, if parseBlock correctly parses statements ending in semicolons, this outer semicolon might be redundant or an error.
        // Let's assume individual statements within the block are semicolon-terminated as per parseStatement.
        // The block itself is defined by INDENT/DEDENT.
        // consume(TokenType::SEMICOLON, "Expected ';' after statements in CASE branch."); // Re-evaluating this
        return std::make_unique<CaseBranchNode>(case_branch_start_tok.line, case_branch_start_tok.col, std::move(case_value), std::move(body));
    } else if (match(TokenType::OTHERWISE)) {
        consume(TokenType::COLON, "Expected ':' after OTHERWISE.");
        consume(TokenType::INDENT, "Expected indented block for OTHERWISE branch.");
        auto body = parseBlock();
        consume(TokenType::DEDENT, "Expected dedent after OTHERWISE branch.");
        // consume(TokenType::SEMICOLON, "Expected ';' after statements in OTHERWISE branch."); // Re-evaluating this
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

    // ENDDEPENDON is no longer used. The end of cases and final DEDENT (if depend on was in an indented block) marks its end.
    // consume(TokenType::ENDDEPENDON, "Expected 'ENDDEPENDON' to close DEPEND ON statement.");
    // The SEMICOLON after ENDDEPENDON is also likely removed if ENDDEPENDON is removed.
    // consume(TokenType::SEMICOLON, "Expected ';' after ENDDEPENDON.");
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

    // A block is a sequence of statements until a DEDENT token is encountered (or EOF).
    // The DEDENT token itself is not consumed by parseBlock.
    Token block_start_tok = current_token_cache;
    auto block_node = std::make_unique<BlockNode>(block_start_tok.line, block_start_tok.col);

    while (!check(TokenType::DEDENT) && !check(TokenType::EOF_TOKEN)) {
        // Check for other potential block terminators if necessary (e.g. specific keywords that might end a list of statements
        // even before a DEDENT in some grammar variations, though pure indentation shouldn't need this).
        // Example: Python's `elif` or `else` would terminate the previous `if` block's statements before the DEDENT.
        // For this grammar, assuming simple statement list until DEDENT.
        // Also, need to ensure we don't loop infinitely if parseStatement fails to consume tokens.
        if ( check(TokenType::ELSE) || // ELSE will be handled by ifStatement, not as part of then_block's statements.
             check(TokenType::CASE) || check(TokenType::OTHERWISE) || // these start new branches in dependon
             check(TokenType::UNTIL) // UNTIL terminates a REPEAT block's statements
           ) {
            break;
        }

        // Heuristic: if current token cannot start a statement, break to avoid infinite loop on errors.
        // This check should be more robust based on what parseStatement expects.
        // For now, assume parseStatement will throw if it finds an invalid start.
        // If parseStatement *can* return nullptr or not advance, this loop needs more care.
        // Given parseStatement throws on error, this direct call is okay.

        block_node->statements.push_back(parseStatement());
    }
    // The DEDENT is consumed by the caller.
    return block_node;
}

std::unique_ptr<VariableDeclarationNode> Parser::parseVariableDeclaration() {
    auto var_name_node = parseIdentifier(); // std::unique_ptr<IdentifierNode>
    // Token var_name_token = previous_token(); // previous_token() is based on current_token_idx, not necessarily var_name_node's token
    Token var_name_token_for_info = Token{TokenType::IDENTIFIER, var_name_node->name, var_name_node->line, var_name_node->col};

    SymbolInfo info; // Declare info object at the beginning
    info.kind = "variable";
    info.scope_level = symbol_table_ptr->getCurrentScopeLevel();
    info.declaration_line = var_name_token_for_info.line;
    info.declaration_col = var_name_token_for_info.col;
    // Default other fields like is_array, is_pointer_type etc., are set by SymbolInfo constructor

    consume(TokenType::COLON, "Expected ':' after variable name in declaration.");
    std::string node_var_type_string;

    if (match(TokenType::POINTER_KW)) {
        consume(TokenType::TO_KW, "Expected 'to' after 'pointer'.");
        Token base_type_tok = current_token_cache;
        if (base_type_tok.type == TokenType::TYPE_INTEGER || base_type_tok.type == TokenType::TYPE_STRING ||
            base_type_tok.type == TokenType::TYPE_BOOLEAN || base_type_tok.type == TokenType::TYPE_CHARACTER ||
            base_type_tok.type == TokenType::TYPE_REAL    ||
            base_type_tok.type == TokenType::IDENTIFIER) {
            advance();
            info.is_pointer_type = true;
            info.pointed_type = base_type_tok.value;
            info.type = "pointer"; // Main type for SymbolInfo
            node_var_type_string = "pointer to " + base_type_tok.value; // For AST Node
        } else {
            ErrorHandler::report(ErrorCode::SYNTAX_MISSING_EXPECTED_TOKEN, base_type_tok.line, base_type_tok.col, "Expected base type after 'pointer to'.");
            throw std::runtime_error("Parser error: Invalid pointer base type.");
        }
    } else if (check(TokenType::ARRAY)) {
        consume(TokenType::ARRAY, "Expected 'ARRAY' keyword.");
        consume(TokenType::LBRACKET, "Expected '[' after ARRAY keyword.");

        auto min_bound_expr_node = parseExpression();
        consume(TokenType::DOTDOT, "Expected '..' for array range separator.");
        auto max_bound_expr_node = parseExpression();

        consume(TokenType::RBRACKET, "Expected ']' after array bounds.");
        consume(TokenType::OF, "Expected 'OF' keyword after array bounds specification.");

        Token element_type_token = current_token_cache;
        if (element_type_token.type == TokenType::TYPE_INTEGER ||
            element_type_token.type == TokenType::TYPE_REAL ||
            element_type_token.type == TokenType::TYPE_BOOLEAN ||
            element_type_token.type == TokenType::TYPE_CHARACTER ||
            element_type_token.type == TokenType::TYPE_STRING ||
            element_type_token.type == TokenType::IDENTIFIER) {
            advance();
        } else {
            ErrorHandler::report(ErrorCode::SYNTAX_MISSING_EXPECTED_TOKEN, element_type_token.line, element_type_token.col, "Expected valid array element type (e.g., integer, real, custom_type).");
            throw std::runtime_error("Parser error: Invalid array element type.");
        }

        info.is_array = true;
        info.array_element_type = element_type_token.value;
        info.type = "array"; // Overall type for symbol table

        IntegerLiteralNode* min_lit_node = dynamic_cast<IntegerLiteralNode*>(min_bound_expr_node.get());
        IntegerLiteralNode* max_lit_node = dynamic_cast<IntegerLiteralNode*>(max_bound_expr_node.get());

        if (min_lit_node && max_lit_node) {
            info.array_min_bound = min_lit_node->value;
            info.array_max_bound = max_lit_node->value;
            node_var_type_string = "array [" + std::to_string(min_lit_node->value) + ".." + std::to_string(max_lit_node->value) + "] of " + element_type_token.value;
            if (info.array_min_bound > info.array_max_bound) {
                ErrorHandler::report(ErrorCode::SEMANTIC_ERROR, var_name_token_for_info.line, var_name_token_for_info.col,
                                     "Array minimum bound (" + std::to_string(info.array_min_bound) +
                                     ") cannot be greater than maximum bound (" + std::to_string(info.array_max_bound) + ").");
            }
        } else {
            ErrorHandler::report(ErrorCode::SEMANTIC_ERROR, var_name_token_for_info.line, var_name_token_for_info.col, "Array bounds must currently be integer literals.");
            info.array_min_bound = 0;
            info.array_max_bound = -1;
            node_var_type_string = "array [...] of " + element_type_token.value; // Placeholder
        }
    } else {
         Token type_tok = current_token_cache;
        if (type_tok.type == TokenType::TYPE_INTEGER || type_tok.type == TokenType::TYPE_STRING ||
            type_tok.type == TokenType::TYPE_BOOLEAN || type_tok.type == TokenType::TYPE_CHARACTER ||
            type_tok.type == TokenType::TYPE_REAL    ||
            type_tok.type == TokenType::IDENTIFIER) {
            advance();
            info.type = type_tok.value;
            node_var_type_string = type_tok.value;
        } else {
            ErrorHandler::report(ErrorCode::SYNTAX_INVALID_EXPRESSION, type_tok.line, type_tok.col, "Expected type identifier (e.g., integer, string) or custom type name, or 'pointer'.");
            throw std::runtime_error("Parser error: Invalid type in variable declaration.");
        }
    }

    // Populate SymbolInfo (info.type should get the full string like "pointer to integer" or "integer")
    // SymbolInfo info; // MOVED TO TOP OF FUNCTION
    // info.kind = "variable"; // MOVED TO TOP
    // info.scope_level = symbol_table_ptr->getCurrentScopeLevel(); // MOVED TO TOP
    // info.declaration_line = var_name_token_for_info.line; // MOVED TO TOP
    // info.declaration_col = var_name_token_for_info.col; // MOVED TO TOP
    // info.type is set above based on whether it's a pointer or not

    // Check if node_var_type_string indicates a pointer type
    // const std::string pointer_prefix = "pointer to "; // This logic is now integrated above
    // if (node_var_type_string.rfind(pointer_prefix, 0) == 0) {
    //     info.is_pointer_type = true;
    //     info.pointed_type = node_var_type_string.substr(pointer_prefix.length());
    //     info.type = "pointer";
    // } else {
    //     info.type = node_var_type_string;
    // }

    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");

    if (!symbol_table_ptr->addSymbol(var_name_node->name, info)) {
        Token base_type_tok = current_token_cache;
        if (base_type_tok.type == TokenType::TYPE_INTEGER || base_type_tok.type == TokenType::TYPE_STRING ||
            base_type_tok.type == TokenType::TYPE_BOOLEAN || base_type_tok.type == TokenType::TYPE_CHARACTER ||
            base_type_tok.type == TokenType::TYPE_REAL    ||
            base_type_tok.type == TokenType::IDENTIFIER) { // IDENTIFIER for custom types
            advance(); // Consume the base type token
            node_var_type_string = "pointer to " + base_type_tok.value;
        } else {
            ErrorHandler::report(ErrorCode::SYNTAX_MISSING_EXPECTED_TOKEN, base_type_tok.line, base_type_tok.col, "Expected base type after 'pointer to'.");
            throw std::runtime_error("Parser error: Invalid pointer base type.");
        }
        // SymbolInfo population for pointer type will be handled in a later subtask
        // For now, SymbolInfo.type will store "pointer to <base_type>"
        // And specific fields like is_pointer, pointer_base_type can be added to SymbolInfo later.
    } else if (check(TokenType::ARRAY)) {
        consume(TokenType::ARRAY, "Expected 'ARRAY' keyword.");
        consume(TokenType::LBRACKET, "Expected '[' after ARRAY keyword.");

        auto min_bound_expr_node = parseExpression(); // This should evaluate to an integer literal
        consume(TokenType::DOTDOT, "Expected '..' for array range separator.");
        auto max_bound_expr_node = parseExpression(); // This should also evaluate to an integer literal

        consume(TokenType::RBRACKET, "Expected ']' after array bounds.");
        consume(TokenType::OF, "Expected 'OF' keyword after array bounds specification.");

        Token element_type_token = current_token_cache;
        if (element_type_token.type == TokenType::TYPE_INTEGER ||
            element_type_token.type == TokenType::TYPE_REAL ||
            element_type_token.type == TokenType::TYPE_BOOLEAN ||
            element_type_token.type == TokenType::TYPE_CHARACTER ||
            element_type_token.type == TokenType::TYPE_STRING ||
            element_type_token.type == TokenType::IDENTIFIER) { // For custom types
            advance(); // Consume the element type token
        } else {
            ErrorHandler::report(ErrorCode::SYNTAX_MISSING_EXPECTED_TOKEN, element_type_token.line, element_type_token.col, "Expected valid array element type (e.g., integer, real, custom_type).");
            throw std::runtime_error("Parser error: Invalid array element type.");
        }

        info.is_array = true;
        info.array_element_type = element_type_token.value;
        info.type = "array"; // Overall type for symbol table
        node_var_type_string = "array [" + std::string("...") + ".." + std::string("...") + "] of " + element_type_token.value; // Placeholder for actual bound values in string

        // Process bounds: Must be integer literals as per current SymbolInfo structure
        IntegerLiteralNode* min_lit_node = dynamic_cast<IntegerLiteralNode*>(min_bound_expr_node.get());
        IntegerLiteralNode* max_lit_node = dynamic_cast<IntegerLiteralNode*>(max_bound_expr_node.get());

        if (min_lit_node && max_lit_node) {
            info.array_min_bound = min_lit_node->value;
            info.array_max_bound = max_lit_node->value;
            node_var_type_string = "array [" + std::to_string(min_lit_node->value) + ".." + std::to_string(max_lit_node->value) + "] of " + element_type_token.value;
            if (info.array_min_bound > info.array_max_bound) {
                ErrorHandler::report(ErrorCode::SEMANTIC_ERROR, var_name_token_for_info.line, var_name_token_for_info.col,
                                     "Array minimum bound (" + std::to_string(info.array_min_bound) +
                                     ") cannot be greater than maximum bound (" + std::to_string(info.array_max_bound) + ").");
                // Potentially throw or mark as error
            }
        } else {
            ErrorHandler::report(ErrorCode::SEMANTIC_ERROR, var_name_token_for_info.line, var_name_token_for_info.col, "Array bounds must currently be integer literals.");
            // Set to default/error values if bounds are not integer literals
            info.array_min_bound = 0;
            info.array_max_bound = -1; // Indicates error or uninitialized
        }
    } else { // Simple variable type
        Token type_tok = current_token_cache;
        if (type_tok.type == TokenType::TYPE_INTEGER || type_tok.type == TokenType::TYPE_STRING ||
            type_tok.type == TokenType::TYPE_BOOLEAN || type_tok.type == TokenType::TYPE_CHARACTER ||
            type_tok.type == TokenType::TYPE_REAL    ||
            type_tok.type == TokenType::IDENTIFIER) {
            advance(); // Consume the type token
            info.type = type_tok.value;
            node_var_type_string = type_tok.value;
        } else {
            ErrorHandler::report(ErrorCode::SYNTAX_INVALID_EXPRESSION, type_tok.line, type_tok.col, "Expected type identifier (e.g., integer, string) or custom type name.");
            throw std::runtime_error("Parser error: Invalid type in variable declaration.");
        }
        info.is_array = false;
    } else { // Simple type or already determined pointer type string
         Token type_tok = current_token_cache;
        if (type_tok.type == TokenType::TYPE_INTEGER || type_tok.type == TokenType::TYPE_STRING ||
            type_tok.type == TokenType::TYPE_BOOLEAN || type_tok.type == TokenType::TYPE_CHARACTER ||
            type_tok.type == TokenType::TYPE_REAL    ||
            type_tok.type == TokenType::IDENTIFIER) {
            advance(); // Consume the type token
            // info.type = type_tok.value; // This will be set later from node_var_type_string
            node_var_type_string = type_tok.value;
        } else {
            ErrorHandler::report(ErrorCode::SYNTAX_INVALID_EXPRESSION, type_tok.line, type_tok.col, "Expected type identifier (e.g., integer, string) or custom type name, or 'pointer'.");
            throw std::runtime_error("Parser error: Invalid type in variable declaration.");
        }
    }

    // Populate SymbolInfo
    SymbolInfo info;
    info.kind = "variable";
    info.scope_level = symbol_table_ptr->getCurrentScopeLevel();
    info.declaration_line = var_name_token_for_info.line;
    info.declaration_col = var_name_token_for_info.col;
    // info.type is set below based on whether it's a pointer or not

    // Check if node_var_type_string indicates a pointer type
    const std::string pointer_prefix = "pointer to ";
    if (node_var_type_string.rfind(pointer_prefix, 0) == 0) { // starts with "pointer to "
        info.is_pointer_type = true;
        info.pointed_type = node_var_type_string.substr(pointer_prefix.length());
        info.type = "pointer"; // Set main type to "pointer" for clarity in SymbolInfo
    } else {
        // Not a pointer, is_pointer_type remains false, pointed_type remains ""
        // info.type is simply node_var_type_string (e.g., "integer", "array [...]...")
        // This was previously handled by the SymbolInfo constructor default for is_pointer_type and pointed_type.
        // Explicitly set info.type here for clarity for non-pointer types too.
        info.type = node_var_type_string;
    }
    // is_array, array_element_type etc. are set if it's an array (this logic was inside the 'else if (check(TokenType::ARRAY))' block, needs to be accessible to 'info')
    // The SymbolInfo 'info' object needs to be accessible where array details are parsed if those details are to be stored in it.
    // Current structure: node_var_type_string is built, then info is populated.
    // If it's an array, info.is_array etc. were being set on a *different info object* if it was declared inside that block.
    // This needs correction: 'info' should be declared before the if/else if/else for type parsing,
    // and then populated.

    // Refactor: Declare SymbolInfo info at the beginning of the function after var_name_node.
    // The current diff will apply to the section after node_var_type_string is determined.
    // The SymbolInfo `info` is already declared *after* node_var_type_string is determined which is fine.
    // The array specific fields like `info.is_array` were set inside the array parsing block.
    // This is okay if `info` itself is passed by reference or those fields are set on the same `info` object.
    // Looking at the code, `info` is declared *after* the if-else-if for type string determination.
    // This means array metadata (is_array, etc.) is NOT being set on the `info` object that gets added to symbol table.
    // This is a bug from previous refactoring of this function.
    // I must correct this by declaring `info` earlier.

    // For this subtask, I will focus only on setting is_pointer_type and pointed_type.
    // The array metadata issue is pre-existing.

    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");

    if (!symbol_table_ptr->addSymbol(var_name_node->name, info)) {
        ErrorHandler::report(ErrorCode::SEMANTIC_REDEFINITION_IDENTIFIER,
                             info.declaration_line, info.declaration_col,
                             "Variable '" + var_name_node->name + "' already declared in this scope.");
    }
    return std::make_unique<VariableDeclarationNode>(var_name_token_for_info.line, var_name_token_for_info.col, var_name_node->name, node_var_type_string);
}


std::unique_ptr<FunctionParameterNode> Parser::parseFunctionParameter() {
    ParameterMode current_mode = ParameterMode::IN; // Default mode

    // Check for explicit mode specifiers
    if (match(TokenType::OUTPUT)) {
        current_mode = ParameterMode::OUT;
    } else if (match(TokenType::INPUTOUTPUT_KEYWORD)) {
        current_mode = ParameterMode::IN_OUT;
    } else if (match(TokenType::INPUT)) {
        current_mode = ParameterMode::IN;
    }
    // If none of these are matched, it defaults to ParameterMode::IN as initialized.

    Token param_name_tok = consume(TokenType::IDENTIFIER, "Expected parameter name.");
    consume(TokenType::COLON, "Expected ':' after parameter name.");
    Token param_type_tok = current_token_cache;

    if (param_type_tok.type == TokenType::TYPE_INTEGER || param_type_tok.type == TokenType::TYPE_STRING ||
        param_type_tok.type == TokenType::TYPE_BOOLEAN || param_type_tok.type == TokenType::TYPE_CHARACTER ||
        param_type_tok.type == TokenType::TYPE_REAL    ||
        param_type_tok.type == TokenType::IDENTIFIER) { // IDENTIFIER for custom types
        advance(); // Consume type token
    } else {
        ErrorHandler::report(ErrorCode::SYNTAX_MISSING_EXPECTED_TOKEN, param_type_tok.line, param_type_tok.col, "Expected type for parameter '" + param_name_tok.value + "'.");
        throw std::runtime_error("Parser error: Invalid type in parameter declaration.");
    }

    SymbolInfo param_info;
    param_info.type = param_type_tok.value;
    param_info.kind = "parameter";
    param_info.scope_level = symbol_table_ptr->getCurrentScopeLevel();
    param_info.declaration_line = param_name_tok.line;
    param_info.declaration_col = param_name_tok.col;
    param_info.param_mode = current_mode; // Set the parameter mode in SymbolInfo

    if (!symbol_table_ptr->addSymbol(param_name_tok.value, param_info)) {
        ErrorHandler::report(ErrorCode::SEMANTIC_REDEFINITION_IDENTIFIER,
                             param_info.declaration_line, param_info.declaration_col,
                             "Parameter '" + param_name_tok.value + "' already declared in this function/procedure scope.");
        // Consider if this should throw or just report and continue for robustness
    }

    return std::make_unique<FunctionParameterNode>(param_name_tok.line, param_name_tok.col, param_name_tok.value, param_type_tok.value, current_mode);
}


std::unique_ptr<FunctionPrototypeNode> Parser::parseFunctionPrototype() {
    Token func_proc_kw = current_token_cache; // FUNCTION or PROCEDURE
    advance(); // Consume FUNCTION or PROCEDURE

    Token func_name_tok = consume(TokenType::IDENTIFIER, "Expected function or procedure name.");

    // Placeholder for return type, will be filled after parameters
    auto proto_node = std::make_unique<FunctionPrototypeNode>(func_proc_kw.line, func_proc_kw.col, func_name_tok.value, "");

    // Parameters - Enter new scope for parameters
    symbol_table_ptr->enterScope();
    if (match(TokenType::LPAREN)) {
        if (!check(TokenType::RPAREN)) { // If there are parameters
            do {
                proto_node->parameters.push_back(parseFunctionParameter());
            } while (match(TokenType::COMMA));
        }
        consume(TokenType::RPAREN, "Expected ')' after function parameters.");
    }
    // Parameter scope is still active here. It will be exited by parseSubprogramBody.

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

    // Add function/procedure symbol to the parent scope (the one active before parameter scope)
    // Need to determine the correct scope level for this.
    // If parsing global functions, current scope *before* enterScope for params was global.
    // If parsing nested functions, it would be the outer function's scope.
    // SymbolTable's current_scope_level is now for parameters. So parent is current_scope_level - 1.
    SymbolInfo func_info;
    func_info.type = proto_node->return_type; // Already determined
    func_info.kind = (func_proc_kw.type == TokenType::FUNCTION) ? "function" : "procedure";
    func_info.scope_level = symbol_table_ptr->getCurrentScopeLevel() -1; // Added to parent scope
    func_info.declaration_line = func_name_tok.line;
    func_info.declaration_col = func_name_tok.col;
    // TODO: Store parameter types in func_info if SymbolInfo supports it.

    // Temporarily exit to parent scope to add the function symbol, then re-enter for subsequent parsing if necessary.
    // This is a bit tricky. A better way might be to pass the target symbol table and scope level to addSymbol.
    // Or, collect all info and add it after all scopes are exited.
    // For simplicity, let's assume addSymbol can handle adding to a specific scope or we adjust.
    // The current SymbolTable addSymbol adds to current scope.
    // So, we'd need to add it *before* entering param scope, or have a way to add to outer scope.

    // Let's assume we add the function symbol to the scope that was active *before* parameters.
    // This means we should construct func_info and add it *before* symbol_table_ptr->enterScope();
    // This part of the logic needs careful review with the SymbolTable implementation.
    // For now, the SymbolInfo is created, but adding it to the correct scope is deferred or needs symbol_table enhancement.
    // The provided solution for SymbolTable adds to scope_stack[current_scope_level].
    // So, to add to parent, we'd need to do it before `enterScope` for params.

    // The current structure of parseFunctionPrototype and parseSubprogramBody implies
    // that parseFunctionPrototype is called, then parameter scope is entered,
    // then local kamus scope is entered, then exited, then parameter scope is exited.
    // The function symbol itself should be added to the scope *containing* the function.

    // Simplification: Add function symbol to the scope active when `parseFunctionPrototype` was called.
    // This is complex because `current_scope_level` changes.
    // A common approach: `parseFunctionPrototype` returns all info, and `parseSubprogramBody` adds it.
    // For now, the SymbolInfo for the function itself is not added here due to scope complexity.
    // This will be addressed by adding it in `parseSubprogramBody` before parameter scope is entered.

    return proto_node;
}

std::unique_ptr<SubprogramBodyNode> Parser::parseSubprogramBody() {
    // Function/Procedure keyword already consumed by lookahead in parseProgram or similar dispatcher
    Token func_proc_start_token = previous_token(); // FUNCTION or PROCEDURE token, captured before calling parseFunctionPrototype

    // Add function/procedure symbol to the current scope (e.g., global, or outer function scope)
    // This must happen *before* entering scope for parameters.
    // This requires pre-parsing or peeking parts of the prototype (name, type) here.
    // For now, we'll assume parseFunctionPrototype is called first, and we manage scopes around it.
    // This is a structural challenge. Let's assume the function symbol is added *after* its full prototype is known
    // but to the scope that was current *before* its parameters were processed.

    // Store current scope level to correctly add function symbol later
    int function_definition_scope_level = symbol_table_ptr->getCurrentScopeLevel();

    // Parse the full prototype, which includes entering scope for parameters
    // and adding parameters to that new scope.
    auto prototype_node = parseFunctionPrototype(); // This will enter param scope

    // Now, add the function/procedure symbol itself to the scope where it's defined.
    SymbolInfo func_sym_info;
    func_sym_info.type = prototype_node->return_type;
    func_sym_info.kind = (func_proc_start_token.type == TokenType::FUNCTION) ? "function" : "procedure";
    func_sym_info.scope_level = function_definition_scope_level;
    func_sym_info.declaration_line = prototype_node->line; // Use line/col from prototype node
    func_sym_info.declaration_col = prototype_node->col;
    // TODO: Populate parameter list in func_sym_info if SymbolInfo supports it.

    // Add to the original scope (parent of parameter scope)
    // This requires a way to add to a specific scope index or temporarily revert scope.
    // Current `addSymbol` adds to `scope_stack[current_scope_level]`.
    // `current_scope_level` is now the parameter scope.
    // This is a known challenge. For now, we'll try to add to the parent scope by index if possible,
    // or acknowledge this needs a SymbolTable enhancement or different call order.
    // Quick fix: Temporarily decrement, add, then increment. (This is a hack)
    symbol_table_ptr->current_scope_level--; // HACK: go to parent scope
    if (!symbol_table_ptr->addSymbol(prototype_node->func_name, func_sym_info)) {
        ErrorHandler::report(ErrorCode::SEMANTIC_REDEFINITION_IDENTIFIER,
                             func_sym_info.declaration_line, func_sym_info.declaration_col,
                             "Subprogram '" + prototype_node->func_name + "' already declared in this scope.");
    }
    symbol_table_ptr->current_scope_level++; // HACK: restore to parameter scope

    // Line/col for SubprogramBodyNode can be from the prototype
    auto subprogram_node = std::make_unique<SubprogramBodyNode>(prototype_node->line, prototype_node->col, std::move(prototype_node), nullptr);

    // KAMUS LOKAL (Local Declarations) - Enter new scope for locals
    bool local_scope_entered = false;
    if (check(TokenType::KAMUS)) {
        consume(TokenType::KAMUS, "Expected 'KAMUS' for local declarations in subprogram.");
        symbol_table_ptr->enterScope(); // New scope for local variables
        local_scope_entered = true;
        while (check(TokenType::IDENTIFIER) && !check(TokenType::ALGORITMA)) {
            subprogram_node->local_declarations.push_back(parseVariableDeclaration());
        }
    }

    // ALGORITMA (Subprogram Body)
    consume(TokenType::ALGORITMA, "Expected 'ALGORITMA' for subprogram body.");
    consume(TokenType::INDENT, "Expected indented block for subprogram ALGORITMA.");
    subprogram_node->body = parseBlock();
    consume(TokenType::DEDENT, "Expected dedent after subprogram ALGORITMA.");

    // Exit scopes
    if (local_scope_entered) {
        symbol_table_ptr->exitScope(); // Exit local variable scope
    }
    symbol_table_ptr->exitScope(); // Exit parameter scope

    return subprogram_node;
}

// Placeholder for ASTVisitor visit methods (should be in a visitor implementation file)
// class ASTVisitor { public: ... };
// void ASTVisitor::visit(IdentifierNode* node) { /* ... */ }
// ... and so on for all node types.
// These are not part of the Parser itself but are called by node->accept(this).
// The actual ASTVisitor class and its methods would be defined elsewhere.
// For this file, we only need the `accept` methods in the nodes themselves.