#include "../include/symbol_table.hpp"

// Constructor
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
}

// Adds a symbol to the current scope.
bool SymbolTable::addSymbol(const std::string& name, const SymbolInfo& info) {
    if (current_scope_level < 0 || static_cast<size_t>(current_scope_level) >= scope_stack.size()) {
        return false;
    }
    std::unordered_map<std::string, SymbolInfo>& current_map = scope_stack[current_scope_level];
    if (current_map.count(name)) {
        return false; // Symbol already exists in the current scope
    }
    current_map[name] = info;
    return true;
}

// Looks up a symbol starting from the current scope and going outwards to global.
const SymbolInfo* SymbolTable::lookupSymbol(const std::string& name) const {
    for (int i = current_scope_level; i >= 0; --i) {
        if (static_cast<size_t>(i) < scope_stack.size()) {
            const auto& scope = scope_stack[i]; // scope_stack[i] is const& due to this method being const
            auto it = scope.find(name);
            if (it != scope.end()) {
                return &(it->second); // it->second is const SymbolInfo&, so address is const SymbolInfo*
            }
        }
    }
    return nullptr;
}

// Looks up a symbol only in the current (innermost) scope.
const SymbolInfo* SymbolTable::lookupSymbolInCurrentScope(const std::string& name) const {
    if (current_scope_level < 0 || static_cast<size_t>(current_scope_level) >= scope_stack.size()) {
        return nullptr;
    }
    const auto& current_map = scope_stack[current_scope_level]; // current_map is const&
    auto it = current_map.find(name);
    if (it != current_map.end()) {
        return &(it->second); // it->second is const SymbolInfo&
    }
    return nullptr;
}

// getCurrentScopeLevel() is defined inline in the header.
// If it were not, its definition would be:
// int SymbolTable::getCurrentScopeLevel() const {
//     return current_scope_level;
// }
