#include "../include/lexer.hpp"
#include "../include/error_handler.hpp" // Assuming ErrorHandler can be used or replace with std::cerr
#include <cctype>
#include <unordered_map>
#include <algorithm> // For std::tolower

// Case-insensitive keyword map
static const std::unordered_map<std::string, TokenType> keywords = {
    {"program", TokenType::PROGRAM}, {"kamus", TokenType::KAMUS}, {"algoritma", TokenType::ALGORITMA},
    {"function", TokenType::FUNCTION}, {"procedure", TokenType::PROCEDURE},
    {"if", TokenType::IF}, {"then", TokenType::THEN}, {"else", TokenType::ELSE}, {"endif", TokenType::ENDIF},
    {"for", TokenType::FOR}, {"to", TokenType::TO}, {"endfor", TokenType::ENDFOR},
    {"while", TokenType::WHILE}, {"do", TokenType::DO}, {"endwhile", TokenType::ENDWHILE},
    {"repeat", TokenType::REPEAT}, {"until", TokenType::UNTIL},
    {"depend", TokenType::DEPEND}, {"on", TokenType::ON}, {"case", TokenType::CASE},
    {"otherwise", TokenType::OTHERWISE}, {"enddependon", TokenType::ENDDEPENDON},
    {"input", TokenType::INPUT}, {"output", TokenType::OUTPUT}, {"dispose", TokenType::DISPOSE},
    {"true", TokenType::TRUE}, {"false", TokenType::FALSE},
    {"array", TokenType::ARRAY}, {"of", TokenType::OF},
    {"div", TokenType::DIV}, {"mod", TokenType::MOD},
    {"not", TokenType::NOT}, {"and", TokenType::AND}, {"or", TokenType::OR},
    {"integer", TokenType::TYPE_INTEGER}, {"real", TokenType::TYPE_REAL},
    {"boolean", TokenType::TYPE_BOOLEAN}, {"string", TokenType::TYPE_STRING},
    {"character", TokenType::TYPE_CHARACTER}, {"void", TokenType::TYPE_VOID},
    {"type", TokenType::TYPE}, // Generic "type" keyword if still used for user-defined types
    {"endprogram", TokenType::ENDPROGRAM}, {"end", TokenType::END}
};

std::vector<Token> Lexer::tokenize(const std::string &source_code) {
    std::vector<Token> tokens;
    size_t pos = 0;
    int line = 1, col = 1; // Declare and initialize col

    while (pos < source_code.size()) {
        char current_char = source_code[pos];
        // current_col_start removed

        // 1. Whitespace
        if (std::isspace(current_char)) {
            if (current_char == '\n') {
                line++;
                col = 1;
            } else {
                col++;
            }
            pos++;
            continue;
        }

        // 2. Block Comments { ... }
        if (current_char == '{') {
            int token_col_lbrace = col; // Capture column for the '{'
            pos++; col++;
            while (pos < source_code.size() && source_code[pos] != '}') {
                if (source_code[pos] == '\n') {
                    line++; col = 1;
                } else {
                    col++;
                }
                pos++;
            }
            if (pos < source_code.size() && source_code[pos] == '}') { // Found closing '}'
                pos++; col++;
            } else {
                // Unterminated comment, report error or handle as per language spec
                tokens.push_back({TokenType::UNKNOWN, "{", line, token_col_lbrace}); // Use captured column for '{'
                // ErrorHandler::report("Unterminated block comment", line, token_col_lbrace);
            }
            continue;
        }

        // 3. Identifiers and Keywords
        if (std::isalpha(current_char) || current_char == '_') {
            std::string word;
            int token_line = line;
            int token_col = col;
            while (pos < source_code.size() && (std::isalnum(source_code[pos]) || source_code[pos] == '_')) {
                word += source_code[pos];
                pos++;
                col++;
            }
            std::string lower_word = word;
            std::transform(lower_word.begin(), lower_word.end(), lower_word.begin(), ::tolower);

            auto keyword_it = keywords.find(lower_word);
            if (keyword_it != keywords.end()) {
                tokens.push_back({keyword_it->second, word, token_line, token_col});
            } else {
                tokens.push_back({TokenType::IDENTIFIER, word, token_line, token_col});
            }
            continue;
        }

        // 4. Integer Literals
        if (std::isdigit(current_char)) {
            std::string num_str;
            int token_line = line;
            int token_col = col;
            while (pos < source_code.size() && std::isdigit(source_code[pos])) {
                num_str += source_code[pos];
                pos++;
                col++;
            }
            // Could add floating point support here if needed
            tokens.push_back({TokenType::INTEGER_LITERAL, num_str, token_line, token_col});
            continue;
        }

        // 5. String Literals "..."
        if (current_char == '"') {
            std::string str_val;
            int token_line = line;
            int token_col = col;
            pos++; col++; // Consume opening quote
            while (pos < source_code.size() && source_code[pos] != '"') {
                // Handle escape sequences if necessary, e.g., \" or a backslash
                str_val += source_code[pos];
                if (source_code[pos] == '\n') { // String literal containing newline - usually an error or needs special handling
                    line++; col = 1;
                } else {
                    col++;
                }
                pos++;
            }
            if (pos < source_code.size() && source_code[pos] == '"') { // Found closing quote
                pos++; col++;
                tokens.push_back({TokenType::STRING_LITERAL, str_val, token_line, token_col});
            } else { // Unterminated string
                tokens.push_back({TokenType::UNKNOWN, "\"", token_line, token_col}); // Tokenize the opening quote as unknown
                // ErrorHandler::report("Unterminated string literal", token_line, token_col);
            }
            continue;
        }

        // 6. Operators and Punctuation
        int token_line = line;
        int token_col = col;

        // Try 2-character tokens first
        if (pos + 1 < source_code.size()) {
            char next_char = source_code[pos+1];
            if (current_char == '<') {
                if (next_char == '-') { // Assignment: <-
                    tokens.push_back({TokenType::ASSIGN_OP, "<-", token_line, token_col});
                    pos += 2; col += 2; continue;
                } else if (next_char == '=') { // Less or Equal: <=
                    tokens.push_back({TokenType::LESSEQUAL, "<=", token_line, token_col});
                    pos += 2; col += 2; continue;
                } else if (next_char == '>') { // Not Equal: <>
                    tokens.push_back({TokenType::NOTEQUAL, "<>", token_line, token_col});
                    pos += 2; col += 2; continue;
                }
            } else if (current_char == '>') {
                if (next_char == '=') { // Greater or Equal: >=
                    tokens.push_back({TokenType::GREATEREQUAL, ">=", token_line, token_col});
                    pos += 2; col += 2; continue;
                }
            } else if (current_char == '-') {
                if (next_char == '>') { // Right Arrow: ->
                    tokens.push_back({TokenType::RIGHT_ARROW, "->", token_line, token_col});
                    pos += 2; col += 2; continue;
                }
            } else if (current_char == '.') {
                if (next_char == '.') { // Dot Dot: ..
                    tokens.push_back({TokenType::DOTDOT, "..", token_line, token_col});
                    pos += 2; col += 2; continue;
                }
            }
            // LEFT_ARROW ('←') is not handled here as it's a single Unicode char,
            // assuming assignment is '<-' as per ASSIGN_OP.
            // If '←' needs to be supported, it would be a single char case.
        }

        // Try 1-character tokens
        switch (current_char) {
            case ':': tokens.push_back({TokenType::COLON, ":", token_line, token_col}); pos++; col++; continue;
            case ';': tokens.push_back({TokenType::SEMICOLON, ";", token_line, token_col}); pos++; col++; continue;
            case ',': tokens.push_back({TokenType::COMMA, ",", token_line, token_col}); pos++; col++; continue;
            case '.': tokens.push_back({TokenType::DOT, ".", token_line, token_col}); pos++; col++; continue;
            case '(': tokens.push_back({TokenType::LPAREN, "(", token_line, token_col}); pos++; col++; continue;
            case ')': tokens.push_back({TokenType::RPAREN, ")", token_line, token_col}); pos++; col++; continue;
            case '[': tokens.push_back({TokenType::LBRACKET, "[", token_line, token_col}); pos++; col++; continue;
            case ']': tokens.push_back({TokenType::RBRACKET, "]", token_line, token_col}); pos++; col++; continue;
            case '+': tokens.push_back({TokenType::PLUS, "+", token_line, token_col}); pos++; col++; continue;
            case '-': tokens.push_back({TokenType::MINUS, "-", token_line, token_col}); pos++; col++; continue;
            case '*': tokens.push_back({TokenType::STAR, "*", token_line, token_col}); pos++; col++; continue;
            case '/': tokens.push_back({TokenType::SLASH, "/", token_line, token_col}); pos++; col++; continue;
            case '=': tokens.push_back({TokenType::EQUAL, "=", token_line, token_col}); pos++; col++; continue;
            case '<': tokens.push_back({TokenType::LESS, "<", token_line, token_col}); pos++; col++; continue;
            case '>': tokens.push_back({TokenType::GREATER, ">", token_line, token_col}); pos++; col++; continue;
            case '^': tokens.push_back({TokenType::CARET, "^", token_line, token_col}); pos++; col++; continue;
            // The Unicode '←' for LEFT_ARROW is not standard on keyboards.
            // If it must be supported:
            // case '←': tokens.push_back({TokenType::ASSIGN_OP /* or LEFT_ARROW if distinct */, "←", token_line, token_col}); pos++; col++; continue;
            default:
                // 7. Unknown token
                tokens.push_back({TokenType::UNKNOWN, std::string(1, current_char), token_line, token_col});
                // ErrorHandler::report("Unknown character: " + std::string(1, current_char), token_line, token_col);
                pos++;
                col++;
                continue;
        }
    }

    // 8. Add EOF token
    tokens.push_back({TokenType::EOF_TOKEN, "", line, col});
    return tokens;
}