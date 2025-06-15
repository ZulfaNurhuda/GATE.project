#include "../include/lexer.hpp"
#include "../include/error_handler.hpp" // Assuming ErrorHandler can be used or replace with std::cerr
#include <cctype>
#include <unordered_map>
#include <algorithm> // For std::tolower

// Case-insensitive keyword map
static const std::unordered_map<std::string, TokenType> keywords = {
    {"program", TokenType::PROGRAM}, {"kamus", TokenType::KAMUS}, {"algoritma", TokenType::ALGORITMA},
    {"function", TokenType::FUNCTION}, {"procedure", TokenType::PROCEDURE},
    {"if", TokenType::IF}, {"then", TokenType::THEN}, {"else", TokenType::ELSE}, /* {"endif", TokenType::ENDIF}, */ // Removed
    {"for", TokenType::FOR}, /* {"to", TokenType::TO}, */ /* Replaced by TO_KW */
    {"while", TokenType::WHILE}, {"do", TokenType::DO}, /* {"endwhile", TokenType::ENDWHILE}, */ // Removed
    {"repeat", TokenType::REPEAT}, {"until", TokenType::UNTIL},
    {"depend", TokenType::DEPEND}, {"on", TokenType::ON}, {"case", TokenType::CASE},
    {"otherwise", TokenType::OTHERWISE}, /* {"enddependon", TokenType::ENDDEPENDON}, */ // Removed
    {"input", TokenType::INPUT}, {"output", TokenType::OUTPUT},
    // {"dispose", TokenType::DISPOSE}, // Removed, "dispose" maps to DEALLOCATE_KW
    {"true", TokenType::TRUE}, {"false", TokenType::FALSE},
    {"array", TokenType::ARRAY}, {"of", TokenType::OF},
    {"div", TokenType::DIV}, {"mod", TokenType::MOD},
    {"not", TokenType::NOT}, {"and", TokenType::AND}, {"or", TokenType::OR},
    {"integer", TokenType::TYPE_INTEGER}, {"real", TokenType::TYPE_REAL},
    {"boolean", TokenType::TYPE_BOOLEAN}, {"string", TokenType::TYPE_STRING},
    {"character", TokenType::TYPE_CHARACTER}, {"void", TokenType::TYPE_VOID},
    {"type", TokenType::TYPE}, // Generic "type" keyword if still used for user-defined types
    {"inline", TokenType::INLINE_KEYWORD},
    {"endprogram", TokenType::ENDPROGRAM}, {"end", TokenType::END},
    // New keywords for pointers and memory management
    {"pointer", TokenType::POINTER_KW},
    {"to", TokenType::TO_KW}, // "to" now maps to TO_KW
    {"reference", TokenType::REFERENCE_KW},
    {"dereference", TokenType::DEREFERENCE_KW},
    {"allocate", TokenType::ALLOCATE_KW},
    {"reallocate", TokenType::REALLOCATE_KW},
    {"deallocate", TokenType::DEALLOCATE_KW},
    {"dispose", TokenType::DEALLOCATE_KW}, // "dispose" also maps to DEALLOCATE_KW
    {"null", TokenType::NULL_KW},          // Added NULL keyword
    {"constant", TokenType::CONSTANT_KW}  // Added CONSTANT keyword
};

// Unused helper function calculate_leading_spaces removed.
// Unused helper function is_blank_or_comment_only removed.

std::vector<Token> Lexer::tokenize(const std::string &source_code) {
    std::vector<Token> tokens;
    size_t current_pos = 0;
    int current_line_num = 1;
    int current_col_num = 1; // Tracks column for tokens on the current line

    std::vector<int> indent_stack;
    indent_stack.push_back(0); // Initial indentation level

    size_t line_start_pos = 0;

    while (line_start_pos < source_code.size()) {
        size_t line_end_pos = source_code.find('\n', line_start_pos);
        if (line_end_pos == std::string::npos) {
            line_end_pos = source_code.size();
        }
        std::string line_content = source_code.substr(line_start_pos, line_end_pos - line_start_pos);

        // Calculate current indentation
        current_pos = line_start_pos; // Reset current_pos to start of line for token processing
        current_col_num = 1;    // Reset column for the new line

        int leading_spaces = 0;
        size_t first_char_pos = 0;
        while(first_char_pos < line_content.length() && line_content[first_char_pos] == ' ') {
            leading_spaces++;
            first_char_pos++;
        }
        // Check for tabs, ideally ErrorHandler should be used.
        if (first_char_pos < line_content.length() && line_content[first_char_pos] == '\t') {
             ErrorHandler::report(ErrorCode::LEXICAL_ERROR, current_line_num, first_char_pos + 1, "Tabs are not allowed for indentation. Please use spaces.");
             // Skip processing this line further or attempt recovery if desired
             line_start_pos = line_end_pos + 1;
             current_line_num++;
             continue;
        }


        // Skip blank lines or lines with only comments for indentation logic
        bool line_is_significant = false;
        size_t actual_code_start_idx_in_line = first_char_pos;
        while(actual_code_start_idx_in_line < line_content.length() && std::isspace(line_content[actual_code_start_idx_in_line])) {
            actual_code_start_idx_in_line++; // Skip any further spaces/tabs if mixed (though tabs are errors)
        }
        if (actual_code_start_idx_in_line < line_content.length() && line_content[actual_code_start_idx_in_line] != '{') {
             line_is_significant = true; // Line has non-comment code
        }


        if (line_is_significant) {
            if (leading_spaces > indent_stack.back()) {
                indent_stack.push_back(leading_spaces);
                tokens.push_back({TokenType::INDENT, "", current_line_num, 1}); // Indent token at start of line
            } else if (leading_spaces < indent_stack.back()) {
                while (leading_spaces < indent_stack.back()) {
                    indent_stack.pop_back();
                    tokens.push_back({TokenType::DEDENT, "", current_line_num, 1}); // Dedent token at start of line
                }
                if (leading_spaces != indent_stack.back()) {
                    // Error: Inconsistent dedentation
                    ErrorHandler::report(ErrorCode::LEXICAL_INDENTATION_ERROR, current_line_num, leading_spaces + 1, "Inconsistent dedentation level.");
                    // Potentially skip this line or try to recover
                }
            }
        }

        // Process tokens on the current line
        current_pos = line_start_pos + actual_code_start_idx_in_line; // Start tokenizing from the first non-space char
        current_col_num = actual_code_start_idx_in_line + 1;


        while (current_pos < line_end_pos) {
            char current_char = source_code[current_pos];
            int token_start_col = current_col_num;

            // Skip spaces within the line (already handled leading spaces for indentation)
            if (std::isspace(current_char)) {
                current_pos++;
                current_col_num++;
                continue;
            }

            // Block Comments { ... }
            if (current_char == '{') {
                // size_t comment_start_pos = current_pos; // Unused variable
                while (current_pos < source_code.size() && source_code[current_pos] != '}') {
                    if (source_code[current_pos] == '\n') {
                        // This would mean a multi-line comment; the outer loop handles line-by-line.
                        // For simplicity here, assume comments don't cross lines handled by this inner loop.
                        // Or, adjust to consume until '}' across lines.
                        // Current line-by-line processing means this handles comments on a single line
                        // or the start of a multi-line comment.
                        // If it's the start of a multi-line comment, the rest of the line might be consumed here.
                        break; // Let outer loop handle newline and next line's indentation
                    }
                    current_pos++;
                    current_col_num++;
                }
                if (current_pos < source_code.size() && source_code[current_pos] == '}') { // Found closing '}'
                    current_pos++; current_col_num++;
                } else {
                    // Unterminated comment on this line, or it spans multiple lines.
                    // If it spans, the outer loop will pick up next line.
                    // If it's unclosed at EOF, that's an error handled at the end or by parser.
                    // For now, consider content from '{' to end of line as part of comment.
                    // tokens.push_back({TokenType::UNKNOWN, line_content.substr(source_code[comment_start_pos - line_start_pos]), current_line_num, token_start_col});
                    // ErrorHandler::report("Unterminated block comment", current_line_num, token_start_col);
                    // Simplification: skip to end of line if '}' not on this line.
                    current_pos = line_end_pos;
                }
                continue; // Done with this comment block on this line
            }

            // Check for "input/output" keyword (case-insensitive)
            // This needs to be checked before general identifiers and before single '/' operator
            if ((current_char == 'i' || current_char == 'I') && current_pos + 11 <= line_end_pos) { // "input/output" is 12 chars
                std::string potential_kw = source_code.substr(current_pos, 12);
                std::string lower_kw = potential_kw;
                std::transform(lower_kw.begin(), lower_kw.end(), lower_kw.begin(), ::tolower);
                if (lower_kw == "input/output") {
                    tokens.push_back({TokenType::INPUTOUTPUT_KEYWORD, potential_kw, current_line_num, token_start_col});
                    current_pos += 12;
                    current_col_num += 12;
                    continue;
                }
            }

            // Identifiers and Keywords (including "input" and "output" separately)
            if (std::isalpha(current_char) || current_char == '_') {
                std::string word;
                while (current_pos < line_end_pos && (std::isalnum(source_code[current_pos]) || source_code[current_pos] == '_')) {
                    word += source_code[current_pos];
                    current_pos++;
                    current_col_num++;
                }
                std::string lower_word = word;
                std::transform(lower_word.begin(), lower_word.end(), lower_word.begin(), ::tolower);
                auto keyword_it = keywords.find(lower_word);
                if (keyword_it != keywords.end()) {
                    tokens.push_back({keyword_it->second, word, current_line_num, token_start_col});
                } else {
                    tokens.push_back({TokenType::IDENTIFIER, word, current_line_num, token_start_col});
                }
                continue; // Back to while(current_pos < line_end_pos)
            }

            // Integer Literals
            if (std::isdigit(current_char)) {
                std::string num_str;
                while (current_pos < line_end_pos && std::isdigit(source_code[current_pos])) {
                    num_str += source_code[current_pos];
                    current_pos++;
                    current_col_num++;
                }
                tokens.push_back({TokenType::INTEGER_LITERAL, num_str, current_line_num, token_start_col});
                continue;
            }

            // String Literals "..."
            if (current_char == '"') {
                std::string str_val;
                current_pos++; current_col_num++; // Consume opening quote
                while (current_pos < line_end_pos && source_code[current_pos] != '"') {
                    str_val += source_code[current_pos];
                    current_pos++; current_col_num++;
                }
                if (current_pos < line_end_pos && source_code[current_pos] == '"') { // Found closing quote
                    current_pos++; current_col_num++;
                    tokens.push_back({TokenType::STRING_LITERAL, str_val, current_line_num, token_start_col});
                } else { // Unterminated string on this line
                    tokens.push_back({TokenType::UNKNOWN, "\"", current_line_num, token_start_col});
                    ErrorHandler::report(ErrorCode::LEXICAL_ERROR, current_line_num, token_start_col, "Unterminated string literal on line.");
                }
                continue;
            }

            // Operators and Punctuation
            // Check for "^." first
            if (current_char == '^' && current_pos + 1 < line_end_pos && source_code[current_pos + 1] == '.') {
                tokens.push_back({TokenType::POINTER_ACCESS_OP, "^.", current_line_num, token_start_col});
                current_pos += 2;
                current_col_num += 2;
                continue;
            }

            // Try 2-character tokens first (other than ^.)
            if (current_pos + 1 < line_end_pos) {
                char next_char = source_code[current_pos + 1];
                std::string two_char_op = std::string(1, current_char) + next_char;
                if (two_char_op == "<-") { tokens.push_back({TokenType::ASSIGN_OP, "<-", current_line_num, token_start_col}); current_pos+=2; current_col_num+=2; continue;}
                else if (two_char_op == "<=") { tokens.push_back({TokenType::LESSEQUAL, "<=", current_line_num, token_start_col}); current_pos+=2; current_col_num+=2; continue;}
                else if (two_char_op == "<>") { tokens.push_back({TokenType::NOTEQUAL, "<>", current_line_num, token_start_col}); current_pos+=2; current_col_num+=2; continue;}
                else if (two_char_op == ">=") { tokens.push_back({TokenType::GREATEREQUAL, ">=", current_line_num, token_start_col}); current_pos+=2; current_col_num+=2; continue;}
                else if (two_char_op == "->") { tokens.push_back({TokenType::RIGHT_ARROW, "->", current_line_num, token_start_col}); current_pos+=2; current_col_num+=2; continue;}
                else if (two_char_op == "..") { tokens.push_back({TokenType::DOTDOT, "..", current_line_num, token_start_col}); current_pos+=2; current_col_num+=2; continue;}
            }

            // Try 1-character tokens
            switch (current_char) {
                case ':': tokens.push_back({TokenType::COLON, ":", current_line_num, token_start_col}); break;
                case ';': tokens.push_back({TokenType::SEMICOLON, ";", current_line_num, token_start_col}); break;
                case ',': tokens.push_back({TokenType::COMMA, ",", current_line_num, token_start_col}); break;
                case '.': tokens.push_back({TokenType::DOT, ".", current_line_num, token_start_col}); break;
                case '(': tokens.push_back({TokenType::LPAREN, "(", current_line_num, token_start_col}); break;
                case ')': tokens.push_back({TokenType::RPAREN, ")", current_line_num, token_start_col}); break;
                case '[': tokens.push_back({TokenType::LBRACKET, "[", current_line_num, token_start_col}); break;
                case ']': tokens.push_back({TokenType::RBRACKET, "]", current_line_num, token_start_col}); break;
                case '+': tokens.push_back({TokenType::PLUS, "+", current_line_num, token_start_col}); break;
                case '-': tokens.push_back({TokenType::MINUS, "-", current_line_num, token_start_col}); break;
                case '*': tokens.push_back({TokenType::STAR, "*", current_line_num, token_start_col}); break;
                case '/': tokens.push_back({TokenType::SLASH, "/", current_line_num, token_start_col}); break;
                case '=': tokens.push_back({TokenType::EQUAL, "=", current_line_num, token_start_col}); break;
                case '<': tokens.push_back({TokenType::LESS, "<", current_line_num, token_start_col}); break;
                case '>': tokens.push_back({TokenType::GREATER, ">", current_line_num, token_start_col}); break;
                case '^': tokens.push_back({TokenType::CARET, "^", current_line_num, token_start_col}); break;
                default:
                    tokens.push_back({TokenType::UNKNOWN, std::string(1, current_char), current_line_num, token_start_col});
                    ErrorHandler::report(ErrorCode::LEXICAL_ERROR, current_line_num, token_start_col, "Unknown character: " + std::string(1, current_char));
                    break;
            }
            current_pos++;
            current_col_num++;
        }
        line_start_pos = line_end_pos + 1; // Move to the start of the next line
        current_line_num++;
    }

    // Emit DEDENT tokens for any remaining indentation levels
    while (indent_stack.back() > 0) {
        indent_stack.pop_back();
        // DEDENTs at EOF should point to the last line, or last line + 1, with column 1
        tokens.push_back({TokenType::DEDENT, "", current_line_num, 1});
    }

    tokens.push_back({TokenType::EOF_TOKEN, "", current_line_num, 1}); // Use current_line_num for EOF
    return tokens;
}