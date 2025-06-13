#include "../include/lexer.hpp"
#include "../include/error_handler.hpp"
#include <cctype>
#include <unordered_map>

static const std::unordered_map<std::string, TokenType> keywords = {
    {"Program", TokenType::PROGRAM}, {"KAMUS", TokenType::KAMUS}, {"ALGORITMA", TokenType::ALGORITMA}, {"function", TokenType::FUNCTION}, {"type", TokenType::TYPE}, {"if", TokenType::IF}, {"then", TokenType::THEN}, {"else", TokenType::ELSE}, {"endif", TokenType::ENDIF}, {"for", TokenType::FOR}, {"to", TokenType::TO}, {"endfor", TokenType::ENDFOR}, {"input", TokenType::INPUT}, {"output", TokenType::OUTPUT}, {"dispose", TokenType::DISPOSE}, {"true", TokenType::TRUE}, {"false", TokenType::FALSE}};

std::vector<Token> Lexer::tokenize(const std::string &source_code)
{
    std::vector<Token> tokens;
    size_t pos = 0;
    int line = 1, col = 1;

    while (pos < source_code.size())
    {
        char c = source_code[pos];
        if (std::isspace(c))
        {
            if (c == '\n')
            {
                line++;
                col = 1;
            }
            else
            {
                col++;
            }
            pos++;
            continue;
        }
        if (c == '{')
        {
            while (pos < source_code.size() && source_code[pos] != '}')
            {
                if (source_code[pos] == '\n')
                {
                    line++;
                    col = 1;
                }
                else
                {
                    col++;
                }
                pos++;
            }
            pos++;
            col++;
            continue;
        }

        if (std::isalpha(c))
        {
            std::string word;
            while (pos < source_code.size() && (std::isalnum(source_code[pos]) || source_code[pos] == '_'))
            {
                word += source_code[pos++];
                col++;
            }
            if (keywords.count(word))
            {
                tokens.push_back({keywords.at(word), word, line, col - word.size()});
            }
            else
            {
                tokens.push_back({TokenType::IDENTIFIER, word, line, col - word.size()});
            }
            continue;
        }

        if (std::isdigit(c))
        {
            std::string num;
            while (pos < source_code.size() && std::isdigit(source_code[pos]))
            {
                num += source_code[pos++];
                col++;
            }
            tokens.push_back({TokenType::INTEGER_LITERAL, num, line, col - num.size()});
            continue;
        }

        if (c == '"')
        {
            std::string str;
            pos++;
            col++;
            while (pos < source_code.size() && source_code[pos] != '"')
            {
                str += source_code[pos++];
                col++;
            }
            pos++;
            col++;
            tokens.push_back({TokenType::STRING_LITERAL, str, line, col - str.size() - 2});
            continue;
        }

        switch (c)
        {
        case '←':
            tokens.push_back({TokenType::LEFT_ARROW, "←", line, col++});
            pos++;
            break;
        case ':':
            tokens.push_back({TokenType::COLON, ":", line, col++});
            pos++;
            break;
        case ';':
            tokens.push_back({TokenType::SEMICOLON, ";", line, col++});
            pos++;
            break;
        case ',':
            tokens.push_back({TokenType::COMMA, ",", line, col++});
            pos++;
            break;
        case '.':
            tokens.push_back({TokenType::DOT, ".", line, col++});
            pos++;
            break;
        case '(':
            tokens.push_back({TokenType::LPAREN, "(", line, col++});
            pos++;
            break;
        case ')':
            tokens.push_back({TokenType::RPAREN, ")", line, col++});
            pos++;
            break;
        case '+':
            tokens.push_back({TokenType::PLUS, "+", line, col++});
            pos++;
            break;
        case '-':
            pos++;
            col++;
            if (pos < source_code.size() && source_code[pos] == '>')
            {
                tokens.push_back({TokenType::LEFT_ARROW, "->", line, col - 1});
                pos++;
                col++;
            }
            else
            {
                tokens.push_back({TokenType::MINUS, "-", line, col - 1});
            }
            break;
        case '*':
            tokens.push_back({TokenType::STAR, "*", line, col++});
            pos++;
            break;
        case '/':
            tokens.push_back({TokenType::SLASH, "/", line, col++});
            pos++;
            break;
        case '=':
            tokens.push_back({TokenType::EQUAL, "=", line, col++});
            pos++;
            break;
        case '>':
            pos++;
            col++;
            if (pos < source_code.size() && source_code[pos] == '=')
            {
                tokens.push_back({TokenType::GREATEREQUAL, ">=", line, col - 1});
                pos++;
                col++;
            }
            else
            {
                tokens.push_back({TokenType::GREATER, ">", line, col - 1});
            }
            break;
        default:
            ErrorHandler::report("Unknown character", line, col);
            pos++;
            col++;
        }
    }
    return tokens;
}