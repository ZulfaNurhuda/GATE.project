#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <optional> // Not strictly needed for pointer return, but good to keep for future reference

// Definition for SymbolInfo
struct SymbolInfo {
    std::string type;           // Data type (e.g., "integer", "boolean", "MyStruct") or return type for functions
    std::string kind;           // Kind of symbol (e.g., "variable", "function", "type_definition", "parameter")
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

    // Default constructor for convenience
    SymbolInfo(std::string t = "", std::string k = "", int sl = 0, int dl = 0, int dc = 0)
        : type(std::move(t)), kind(std::move(k)), scope_level(sl), declaration_line(dl), declaration_col(dc) {}
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