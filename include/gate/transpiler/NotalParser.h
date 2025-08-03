/**
 * @file NotalParser.h
 * @brief Recursive descent parser for the NOTAL language
 * 
 * This file defines the NotalParser class that implements a recursive descent
 * parser for the NOTAL language. The parser takes a sequence of tokens from
 * the lexer and constructs an Abstract Syntax Tree (AST) representing the
 * program structure according to NOTAL's grammar rules.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */

#ifndef GATE_TRANSPILER_NOTAL_PARSER_H
#define GATE_TRANSPILER_NOTAL_PARSER_H

#include "gate/core/Token.h"
#include "gate/ast/Expression.h"
#include "gate/ast/Statement.h"
#include "gate/diagnostics/DiagnosticEngine.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <map>

namespace gate::transpiler {

class PanicModeRecovery;
class PhraseLevelRecovery;

/**
 * @brief Recursive descent parser for NOTAL language
 * 
 * The NotalParser implements a recursive descent parser that converts a
 * sequence of tokens into an Abstract Syntax Tree (AST). It follows the
 * NOTAL language grammar rules and handles error recovery through panic
 * mode synchronization.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
class NotalParser {
public:
    /**
     * @brief Constructor for NotalParser
     * @param tokens Vector of tokens to parse
     * @param engine The diagnostic engine to use for error reporting
     */
    NotalParser(const std::vector<core::Token>& tokens, diagnostics::DiagnosticEngine& engine);

    /**
     * @brief Parse tokens into an AST
     * @return Root program statement of the parsed AST
     * @throws ParseError If parsing fails due to syntax errors
     */
    std::shared_ptr<ast::ProgramStmt> parse();

    // --- Public helpers for recovery classes ---
    bool isAtEnd();
    core::Token peek();
    core::Token peekNext();
    core::Token previous();
    core::Token advance();
    bool check(core::TokenType type);
    void reportWarning(const std::string& message, const core::Token& token);

public:
    /**
     * @brief Exception class for parsing errors
     *
     * This exception is thrown when the parser encounters syntax errors
     * that prevent successful parsing of the input token stream.
     */
    class ParseError : public std::runtime_error {
    public:
        /**
         * @brief Constructor for ParseError
         * @param token The token where the error occurred
         * @param message Error message describing the parsing failure
         */
        ParseError(const core::Token& token, const std::string& message)
            : std::runtime_error(message), token(token) {}
        /** @brief Token where the parsing error occurred */
        core::Token token;
    };

private:
    /** @brief Vector of tokens to parse */
    std::vector<core::Token> tokens_;
    /** @brief Reference to the diagnostic engine */
    diagnostics::DiagnosticEngine& diagnosticEngine_;
    /** @brief Current position in token stream */
    size_t current_ = 0;
    /** @brief Collection of subprogram declarations */
    std::vector<std::shared_ptr<ast::Statement>> subprogramDeclarations_;

    // --- Grammar Rule Methods ---
    /** @brief Parse program structure (PROGRAM ... KAMUS ... ALGORITMA) */
    std::shared_ptr<ast::ProgramStmt> program();
    /** @brief Parse KAMUS (dictionary/declarations) section */
    std::shared_ptr<ast::KamusStmt> kamus();
    /** @brief Parse ALGORITMA (algorithm/implementation) section */
    std::shared_ptr<ast::AlgoritmaStmt> algoritma();
    /** @brief Parse any declaration statement */
    std::shared_ptr<ast::Statement> declaration();
    /** @brief Parse variable declaration */
    std::shared_ptr<ast::Statement> varDeclaration();
    /** @brief Parse array declaration with given name */
    std::shared_ptr<ast::Statement> arrayDeclaration(core::Token name);
    /** @brief Parse constant declaration */
    std::shared_ptr<ast::Statement> constantDeclaration();
    /** @brief Parse type declaration */
    std::shared_ptr<ast::Statement> typeDeclaration();
    /** @brief Parse any statement */
    std::shared_ptr<ast::Statement> statement();
    /** @brief Parse allocate statement for dynamic memory */
    std::shared_ptr<ast::Statement> allocateStatement();
    /** @brief Parse deallocate statement for memory cleanup */
    std::shared_ptr<ast::Statement> deallocateStatement();
    /** @brief Parse input statement */
    std::shared_ptr<ast::Statement> inputStatement();
    /** @brief Parse if-then-else conditional statement */
    std::shared_ptr<ast::Statement> ifStatement();
    /** @brief Parse if statement body with indentation level */
    std::shared_ptr<ast::Statement> ifStatementBody(int parentIndentLevel);
    /** @brief Parse while loop statement */
    std::shared_ptr<ast::Statement> whileStatement();
    /** @brief Parse repeat-until loop statement */
    std::shared_ptr<ast::Statement> repeatUntilStatement();
    /** @brief Parse traversal loop statement */
    std::shared_ptr<ast::Statement> traversalStatement();
    /** @brief Parse iterate/stop control statement */
    std::shared_ptr<ast::Statement> iterateStopStatement();
    /** @brief Parse repeat N times loop statement */
    std::shared_ptr<ast::Statement> repeatNTimesStatement();
    /** @brief Parse depend-on switch statement */
    std::shared_ptr<ast::Statement> dependOnStatement();
    /** @brief Parse expression statement */
    std::shared_ptr<ast::Statement> expressionStatement();
    /** @brief Parse output statement */
    std::shared_ptr<ast::Statement> outputStatement();
    /** @brief Parse stop (break) statement */
    std::shared_ptr<ast::Statement> stopStatement();
    /** @brief Parse skip (continue) statement */
    std::shared_ptr<ast::Statement> skipStatement();
    std::vector<std::shared_ptr<ast::Statement>> block();
    std::vector<std::shared_ptr<ast::Statement>> parseBlockByIndentation(int expectedIndentLevel);
    std::shared_ptr<ast::Statement> subprogramDeclaration();
    std::shared_ptr<ast::Statement> procedureDeclaration();
    std::shared_ptr<ast::Statement> functionDeclaration();
    void subprogramImplementation(const core::Token& subprogramKeyword, const core::Token& subprogramName);
    std::vector<ast::Parameter> parameterList();
    std::shared_ptr<ast::Statement> returnStatement();


    // --- Expression Methods ---
    /** @brief Parse any expression (top-level expression parser) */
    std::shared_ptr<ast::Expression> expression();
    /** @brief Parse assignment expression (=, <-, etc.) */
    std::shared_ptr<ast::Expression> assignment();
    /** @brief Parse logical OR expression (or) */
    std::shared_ptr<ast::Expression> logic_or();
    /** @brief Parse logical AND expression (and) */
    std::shared_ptr<ast::Expression> logic_and();
    /** @brief Parse equality expression (=, <>) */
    std::shared_ptr<ast::Expression> equality();
    /** @brief Parse comparison expression (<, >, <=, >=) */
    std::shared_ptr<ast::Expression> comparison();
    /** @brief Parse term expression (+, -) */
    std::shared_ptr<ast::Expression> term();
    /** @brief Parse factor expression (*, /, div, mod) */
    std::shared_ptr<ast::Expression> factor();
    /** @brief Parse power expression (^) */
    std::shared_ptr<ast::Expression> power();
    /** @brief Parse unary expression (not, -, +) */
    std::shared_ptr<ast::Expression> unary();
    /** @brief Parse function call expression */
    std::shared_ptr<ast::Expression> call();
    /** @brief Complete function call parsing with arguments */
    std::shared_ptr<ast::Expression> finishCall(std::shared_ptr<ast::Expression> callee);
    /** @brief Parse array access expression with indices */
    std::shared_ptr<ast::Expression> arrayAccess(std::shared_ptr<ast::Expression> callee);
    /** @brief Parse primary expression (literals, identifiers, parentheses) */
    std::shared_ptr<ast::Expression> primary();


    // --- Helper Methods ---
    /** @brief Check if current token matches any of the given types */
    bool match(const std::vector<core::TokenType>& types);
    /** @brief Consume token of expected type or throw error */
    core::Token consume(core::TokenType type, const std::string& message);
    /** @brief Synchronize parser state after error */
    void synchronize();

    // --- Error Handling ---
    /** @brief Report parsing error and return ParseError exception */
    ParseError error(const core::Token& token, const std::string& message);
};

} // namespace gate::transpiler

#endif // GATE_TRANSPILER_NOTAL_PARSER_H
