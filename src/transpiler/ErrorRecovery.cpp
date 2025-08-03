#include "gate/transpiler/ErrorRecovery.h"
#include "gate/transpiler/NotalParser.h"
#include "gate/diagnostics/DiagnosticEngine.h"
#include "gate/core/Token.h"
#include <set>
#include <map>
#include <vector>

namespace gate::transpiler {

// --- PanicModeRecovery Implementation ---

const std::set<gate::core::TokenType> PanicModeRecovery::SYNCHRONIZATION_TOKENS = {
    gate::core::TokenType::PROGRAM,
    gate::core::TokenType::KAMUS,
    gate::core::TokenType::ALGORITMA,
    gate::core::TokenType::IF,
    gate::core::TokenType::WHILE,
    gate::core::TokenType::REPEAT,
    gate::core::TokenType::PROCEDURE,
    gate::core::TokenType::FUNCTION,
    gate::core::TokenType::TYPE,
    gate::core::TokenType::CONSTANT
};

void PanicModeRecovery::recover(NotalParser* parser) {
    while (!parser->isAtEnd()) {
        /*
         The user's brief mentioned checking for SEMICOLON, but this token
         does not exist in the language definition (Token.h).
         This check is removed.
        */

        if (SYNCHRONIZATION_TOKENS.count(parser->peek().type) > 0) {
            return;
        }

        parser->advance();
    }
}


// --- PhraseLevelRecovery Implementation ---

bool PhraseLevelRecovery::tryRecover(NotalParser* parser, gate::core::TokenType expectedType) {
    static const std::map<gate::core::TokenType, std::vector<gate::core::TokenType>> SUBSTITUTIONS = {
        {gate::core::TokenType::THEN, {gate::core::TokenType::DO}},
        {gate::core::TokenType::DO, {gate::core::TokenType::THEN}},
        {gate::core::TokenType::COLON, {gate::core::TokenType::EQUAL}},
    };

    auto it = SUBSTITUTIONS.find(expectedType);
    if (it != SUBSTITUTIONS.end()) {
        for (auto substitute : it->second) {
            if (parser->check(substitute)) {
                std::string message = "Found '" + tokenTypeToString(substitute) +
                                      "', but expected '" + tokenTypeToString(expectedType) + "'.";
                parser->reportWarning(message, parser->peek());
                parser->advance();
                return true;
            }
        }
    }

    return false;
}

} // namespace gate::transpiler
