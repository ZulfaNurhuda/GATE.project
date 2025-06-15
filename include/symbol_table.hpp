#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

// Definition of ParameterMode, used by SymbolInfo and parser.hpp (which includes this file)
enum class ParameterMode {
    IN,
    OUT,
    IN_OUT,
    NONE // For non-parameter symbols or uninitialized state
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

    // Pointer type information
    bool is_pointer_type;
    std::string pointed_type;   // Base type if is_pointer_type is true

    // Constant information
    bool is_constant;
    // std::string literal_value_for_constant; // Optional: Store actual value if simple

    // Enum type information
    bool is_enum_type;
    std::vector<std::string> enum_values_list; // For enum type, lists its values

    // Enum value information
    bool is_enum_value;
    std::string enum_parent_type_name; // For enum value, stores its enum type name

    // Default constructor for convenience
    SymbolInfo(std::string t = "", std::string k = "", int sl = 0, int dl = 0, int dc = 0)
        : type(std::move(t)), kind(std::move(k)), scope_level(sl), declaration_line(dl), declaration_col(dc),
          is_array(false), array_element_type(""), array_min_bound(0), array_max_bound(0),
          param_mode(k == "parameter" ? ParameterMode::IN : ParameterMode::NONE),
          is_pointer_type(false), pointed_type(""),
          is_constant(false),
          is_enum_type(false), /* enum_values_list is default-constructed to empty */
          is_enum_value(false), enum_parent_type_name("")
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