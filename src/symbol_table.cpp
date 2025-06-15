#include "../include/symbol_table.hpp"

// Constructor
// Initializes current_scope_level to -1 (no scope active initially)
// and then enters the global scope (level 0).
SymbolTable::SymbolTable() : current_scope_level(-1) {
    enterScope(); // Initialize global scope, current_scope_level becomes 0
}

// Enters a new scope
void SymbolTable::enterScope() {
    current_scope_level++;
    scope_stack.emplace_back(); // Add a new empty unordered_map for the new scope
}

// Exits the current scope
void SymbolTable::exitScope() {
    if (current_scope_level >= 0) { // Check if there is an active scope
        if (!scope_stack.empty()) { // Additional safety check
             scope_stack.pop_back();
        }
        current_scope_level--;
    }
    // If current_scope_level < 0, it means exitScope was called too many times
    // or without a corresponding enterScope. No explicit error handling here
    // as per the general guidance, but could be added (e.g., throw exception or log).
}

// Adds a symbol to the current scope.
// Assumes info.scope_level is correctly set by the caller.
// Returns true on success, false if the symbol already exists in the current scope
// or if there's no active scope (which shouldn't happen with proper object init).
bool SymbolTable::addSymbol(const std::string& name, const SymbolInfo& info) {
    if (current_scope_level < 0 || static_cast<size_t>(current_scope_level) >= scope_stack.size()) {
        // This indicates an invalid state, e.g., no scopes or current_scope_level out of sync.
        // Should ideally not be reached if constructor and scope management are correct.
        return false;
    }

    std::unordered_map<std::string, SymbolInfo>& current_map = scope_stack[current_scope_level];

    if (current_map.count(name)) {
        return false; // Symbol already exists in the current scope
    }

    // Add the symbol. The 'info' object is copied.
    // If 'info.scope_level' needed to be set here:
    // SymbolInfo new_info = info;
    // new_info.scope_level = current_scope_level;
    // current_map[name] = new_info;
    // However, following the problem description's preference, assuming 'info' is complete.
    current_map[name] = info;
    return true;
}

// Looks up a symbol starting from the current scope and going outwards to global.
// Returns a pointer to SymbolInfo if found, otherwise nullptr.
SymbolInfo* SymbolTable::lookupSymbol(const std::string& name) const { // Added const
    for (int i = current_scope_level; i >= 0; --i) {
        // Ensure 'i' is a valid index for scope_stack
        if (static_cast<size_t>(i) < scope_stack.size()) {
            const std::unordered_map<std::string, SymbolInfo>& scope = scope_stack[i]; // Use const&
            auto it = scope.find(name); // find is a const method
            if (it != scope.end()) {
                // Returning a non-const pointer from a const method to a member is unsafe.
                // Using const_cast to match the original non-const return type.
                // Ideally, return type should be const SymbolInfo*.
                return const_cast<SymbolInfo*>(&(it->second));
            }
        }
    }
    return nullptr; // Symbol not found in any accessible scope
}

// Looks up a symbol only in the current (innermost) scope.
// Returns a pointer to SymbolInfo if found, otherwise nullptr.
SymbolInfo* SymbolTable::lookupSymbolInCurrentScope(const std::string& name) const { // Added const
    if (current_scope_level < 0 || static_cast<size_t>(current_scope_level) >= scope_stack.size()) {
        // Invalid state (no active scope or current_scope_level out of sync)
        return nullptr;
    }

    const std::unordered_map<std::string, SymbolInfo>& current_map = scope_stack[current_scope_level]; // Use const&
    auto it = current_map.find(name);
    if (it != current_map.end()) {
        // Returning a non-const pointer from a const method to a member is unsafe.
        // Using const_cast to match the original non-const return type.
        return const_cast<SymbolInfo*>(&(it->second));
    }

    return nullptr; // Symbol not found in the current scope
}

// Returns the current scope level.
int SymbolTable::getCurrentScopeLevel() const {
    return current_scope_level;
}