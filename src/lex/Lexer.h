#ifndef COMPILER_PROJECT_LEXER_H
#define COMPILER_PROJECT_LEXER_H

#include <regex>
#include <unordered_map>

#include "Token.h"

using std::regex;

class Lexer
{
public:
    Lexer(std::string src);
    ~Lexer() = default;

    std::vector<Token> lex();
    const std::string printTokens(const std::vector<Token>& tokens);
private:
    std::string source;
    TokenType getKeyword(const std::string& value);

    const std::vector<std::pair<std::regex, TokenType>> rules = {

        // important! must put the longer stuff first so
        // == will parse as '==' and not '=' + '=' (equals instead of 2 asignments)

        // []- group stuff together: [a-zA-Z] is lowercase and upercase letters
        // + - one or more charecters: [a-z]+ will catch helo but not empty string
        // \\- the literal charecter: [a-z \\[ \\] ] will include a-z and '[', ']'
        // ^ - starts with: [1-3] will catch 321 from 54321, while [^1-3] will not

        {std::regex("(//[^\\n]*|/\\*([^*]|\\*+[^*/])*\\*+/)"), TokenType::Comment},


        {regex("^[a-zA-Z_][a-zA-Z0-9_]*"), TokenType::Identifier},

        {regex("[0-9]*?\\.[0-9]+"), TokenType::Literal_Number},
        {regex("^[0-9]+"), TokenType::Literal_Number},
        {regex(R"("[^"]*")"), TokenType::Literal_String},

        // --- symbols ---
        { regex("^;"), TokenType::Symbol_Semicolon },
        {regex("^:"), TokenType::Symbol_Colon},
        { regex("^\\{"), TokenType::Symbol_LBrace },
        { regex("^\\}"), TokenType::Symbol_RBrace },
        { regex("^\\("), TokenType::Symbol_LParen },
        { regex("^\\)"), TokenType::Symbol_RParen },
        {regex("^->"), TokenType::Symbol_Arrow},
        {regex("\\.\\."), TokenType::Symbol_TowDots},

        // opps (longer first)
        { regex("^=="), TokenType::Operator_Equal },
        { regex("^!="), TokenType::Operator_NotEqual },
        { regex("^<="), TokenType::Operator_LessEqual },
        { regex("^>="), TokenType::Operator_GreaterEqual },
        { regex("^&&"), TokenType::Operator_And },
        { regex("^\\|\\|"), TokenType::Operator_Or },

        { regex("^="),  TokenType::Operator_Assignment },
        { regex("^!"),  TokenType::Operator_Exclamation },
        { regex("^<"),  TokenType::Operator_Less },
        { regex("^>"),  TokenType::Operator_Greater },

        { regex("^\\+"), TokenType::Operator_Plus },
        { regex("^-"),  TokenType::Operator_Minus },
        { regex("^\\*"), TokenType::Operator_Star },
        { regex("^/"),  TokenType::Operator_Slash },
    };

    const std::unordered_map<std::string, TokenType> KEYWORDS = {
        {"true", TokenType::Literal_True},
        {"false", TokenType::Literal_False},

        {"int", TokenType::Type_Int},
        {"float", TokenType::Type_Float},
        {"bool", TokenType::Type_Bool},
        {"ptr", TokenType::Type_Ptr},
        {"string", TokenType::Type_String},
        {"color", TokenType::Type_Color},
        {"void", TokenType::Type_Void},


        {"func", TokenType::Keyword_Func},
        {"var", TokenType::Keyword_Var},
        {"return", TokenType::Keyword_Return},

        {"if", TokenType::Keyword_If},
        {"else", TokenType::Keyword_Else},
        {"while", TokenType::Keyword_While},
        {"for", TokenType::Keyword_For},
    };
};


#endif //COMPILER_PROJECT_LEXER_H