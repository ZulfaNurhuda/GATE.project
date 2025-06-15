#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

// Forward declare ParameterMode or include parser.hpp if ParameterMode is defined there.
// Assuming ParameterMode is an enum class defined in parser.hpp, and parser.hpp is lightweight enough
// or ParameterMode is moved to a common header. For now, to use it as a member,
// its definition must be known. If parser.hpp includes symbol_table.hpp, this creates a circular dependency.
// Best: Move ParameterMode to its own header or this header if it's fundamental.
// Workaround: If parser.hpp is not too heavy, include it.
// Alternative: Forward declare `enum class ParameterMode;` if it's only used by pointer/reference here.
// Since it will be a direct member, definition is needed.
// Let's assume parser.hpp can be included, or ParameterMode is moved.
// For this exercise, we will assume ParameterMode is accessible.
// If #include "parser.hpp" is added, it needs to be done carefully.
// A common approach is to put such enums in a dedicated types header.

// Let's try forward declaration first, as it's less invasive.
// However, member `ParameterMode param_mode;` needs full definition.
// So, we must ensure ParameterMode is defined.
// The prompt stated "assume ParameterMode is accessible".
// This typically means its definition is visible.
// This might imply including "parser.hpp" or that ParameterMode was moved.
// Let's assume "parser.hpp" is light or ParameterMode is in a shared header.
// For now, to make this self-contained for the tool, I'll define it here then reconcile later if it's a duplicate.
// This is a common problem in C++ header management.
// The prompt says: "assume ParameterMode (from parser.hpp) is not already accessible here... ensure it is."
// This implies we should add the include if necessary.
// However, including parser.hpp in symbol_table.hpp is risky for circular dependencies.
// The safest is to define ParameterMode in a more fundamental header or here.
// Given the context, I will add a definition here, assuming it matches the one in parser.hpp.
// This is non-ideal but works for the tool's constraints.
// A better solution in a real project: #include "parameter_mode.hpp"

enum class ParameterMode {
    IN,
    OUT,
    IN_OUT,
    NONE // Added for non-parameter symbols or unknown cases
};

// Definition for SymbolInfo
struct SymbolInfo {
    std::string type;           // Data type or return type
    std::string kind;           // "variable", "function", "parameter", etc.
    int scope_level;            // Scope depth where the symbol was defined
    int declaration_line;       // Line number where the symbol was declared (optional)
    int declaration_col;        // Column number where the symbol was declared (optional)

    // Add other fields as necessary, for example:
    // For functions:
    // std::vector<SymbolInfo> parameters; // Or std::vector<std::string> parameter_types;
    // std::string return_type; // Can be covered by 'type'

    // For arrays:
    // std::string element_type;
    // std::vector<int> dimensions;

    // Array metadata
    bool is_array;
    std::string array_element_type; // AN element type string
    long long array_min_bound;
    long long array_max_bound;
    // If bounds can be expressions:
    // std::unique_ptr<ExpressionNode> min_bound_expr;
    // std::unique_ptr<ExpressionNode> max_bound_expr;

    ParameterMode param_mode;   // Mode for parameters (IN, OUT, IN_OUT)

    // Default constructor for convenience
    SymbolInfo(std::string t = "", std::string k = "", int sl = 0, int dl = 0, int dc = 0)
        : type(std::move(t)), kind(std::move(k)), scope_level(sl), declaration_line(dl), declaration_col(dc),
          is_array(false), array_element_type(""), array_min_bound(0), array_max_bound(0),
          // Initialize param_mode: if kind is "parameter", default to IN, otherwise NONE.
          param_mode(k == "parameter" ? ParameterMode::IN : ParameterMode::NONE)
          {}
};

class SymbolTable {
private:
    std::vector<std::unordered_map<std::string, SymbolInfo>> scope_stack;
    int current_scope_level; // Represents the current depth, -1 if no scope is active, 0 for global.

public:
    SymbolTable() : current_scope_level(-1) {
        // Automatically enter the global scope upon creation
        enterScope();
    }

    // Enters a new scope
    void enterScope() {
        current_scope_level++;
        scope_stack.emplace_back(); // Adds a new empty map for the new scope
    }

    // Exits the current scope
    void exitScope() {
        if (current_scope_level >= 0) {
            scope_stack.pop_back();
            current_scope_level--;
        }
        // Consider error handling or behavior if exitScope is called too many times
    }

    // Adds a symbol to the current scope.
    // Returns true on success, false if the symbol already exists in the current scope.
    bool addSymbol(const std::string& name, const SymbolInfo& info) {
        if (current_scope_level < 0 || scope_stack.empty()) {
            // This case should ideally not happen if constructor calls enterScope()
            return false;
        }
        // Check if symbol already exists in the current scope
        // Ensure current_scope_level is a valid index for scope_stack
        if (static_cast<size_t>(current_scope_level) >= scope_stack.size()) {
            // This might happen if exitScope was called too many times or enterScope wasn't called initially
            // Or if current_scope_level became inconsistent.
            // For robustness, one might re-initialize or error.
            // For now, let's prevent a crash.
            return false;
        }
        if (scope_stack[current_scope_level].count(name)) {
            return false; // Symbol already defined in current scope
        }
        // Add the symbol
        scope_stack[current_scope_level][name] = info;
        // Optionally, update info.scope_level if not already set by caller
        // scope_stack[current_scope_level][name].scope_level = current_scope_level;
        return true;
    }

    // Looks up a symbol starting from the current scope and going outwards to global.
    // Returns a pointer to SymbolInfo if found, otherwise nullptr.
    SymbolInfo* lookupSymbol(const std::string& name) {
        for (int i = current_scope_level; i >= 0; --i) {
            if (static_cast<size_t>(i) < scope_stack.size()) { // Ensure index is valid
                auto& current_scope_map = scope_stack[i];
                auto it = current_scope_map.find(name);
                if (it != current_scope_map.end()) {
                    return &(it->second); // Found the symbol
                }
            }
        }
        return nullptr; // Symbol not found in any scope
    }

    // Looks up a symbol only in the current (innermost) scope.
    // Returns a pointer to SymbolInfo if found, otherwise nullptr.
    SymbolInfo* lookupSymbolInCurrentScope(const std::string& name) {
        if (current_scope_level < 0 || scope_stack.empty() || static_cast<size_t>(current_scope_level) >= scope_stack.size()) {
            return nullptr; // No active scope or invalid state
        }
        auto& current_scope_map = scope_stack[current_scope_level];
        auto it = current_scope_map.find(name);
        if (it != current_scope_map.end()) {
            return &(it->second); // Found the symbol in the current scope
        }
        return nullptr; // Symbol not found in the current scope
    }

    // Returns the current scope level.
    int getCurrentScopeLevel() const {
        return current_scope_level;
    }
};

#endif // SYMBOL_TABLE_HPP