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

    // Array metadata
    bool is_array;
    std::string array_element_type; // AN element type string
    long long array_min_bound;
    long long array_max_bound;
    // If bounds can be expressions:
    // std::unique_ptr<ExpressionNode> min_bound_expr;
    // std::unique_ptr<ExpressionNode> max_bound_expr;

    // Default constructor for convenience
    SymbolInfo(std::string t = "", std::string k = "", int sl = 0, int dl = 0, int dc = 0)
        : type(std::move(t)), kind(std::move(k)), scope_level(sl), declaration_line(dl), declaration_col(dc),
          is_array(false), array_element_type(""), array_min_bound(0), array_max_bound(0) {}
};

class SymbolTable {
private:
    std::vector<std::unordered_map<std::string, SymbolInfo>> scope_stack;
    int current_scope_level; // Represents the current depth, -1 if no scope is active, 0 for global.

public:
    SymbolTable(); // Declaration

    // Enters a new scope
    void enterScope(); // Declaration

    // Exits the current scope
    void exitScope(); // Declaration

    // Adds a symbol to the current scope.
    // Returns true on success, false if the symbol already exists in the current scope.
    bool addSymbol(const std::string& name, const SymbolInfo& info); // Declaration

    // Looks up a symbol starting from the current scope and going outwards to global.
    // Returns a pointer to SymbolInfo if found, otherwise nullptr.
    const SymbolInfo* lookupSymbol(const std::string& name) const; // Declaration

    // Looks up a symbol only in the current (innermost) scope.
    // Returns a pointer to SymbolInfo if found, otherwise nullptr.
    const SymbolInfo* lookupSymbolInCurrentScope(const std::string& name) const; // Declaration

    // Returns the current scope level.
    int getCurrentScopeLevel() const {
        return current_scope_level;
    }
};

#endif // SYMBOL_TABLE_HPP