#ifndef GATE_TRANSPILER_ERROR_RECOVERY_H
#define GATE_TRANSPILER_ERROR_RECOVERY_H

#include "core/NotalParser.h"
#include "core/Token.h"
#include <set>
#include <map>
#include <vector>

namespace gate::transpiler {

class PanicModeRecovery {
public:
    static void recover(NotalParser* parser);
private:
    static const std::set<core::TokenType> SYNCHRONIZATION_TOKENS;
};

class PhraseLevelRecovery {
public:
    static bool tryRecover(NotalParser* parser, core::TokenType expectedType);
};

} // namespace gate::transpiler

#endif // GATE_TRANSPILER_ERROR_RECOVERY_H
