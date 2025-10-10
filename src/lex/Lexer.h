#ifndef LEXER_H
#define LEXER_H

#include "Token.h"


class Lexer
{
public:
    Lexer(std::string src);
    ~Lexer();

    std::vector<Token> lex();
    const std::string printTokens(const std::vector<Token>& tokens);
private:
    std::string source;
    TokenType getKeyword(const std::string& value);

    const std::vector<std::pair<std::regex, TokenType>> rules = {
        
        // important! must put the longer stuff first so
        // == will parse as '==' and not '=' + '=' (equals instead of 2 asignments)
        
        // []- group stuff together: a-zA-Z is lowercase and upercase letters
        // + - one or more charecters: [a-z]+ will catch helo but not empty string
        // \\- the literal charecter: [a-z \\[ \\] ] will include a-z and []

        {std::regex("^[a-zA-Z_][a-zA-Z0-9_]*"), TokenType::Identifier},
        { std::regex("^[0-9]+"), TokenType::Integer },

        // --- symbols ---
        { std::regex("^;"), TokenType::Symbol_Semicolon },
        { std::regex("^\\{"), TokenType::Symbol_LBrace },
        { std::regex("^\\}"), TokenType::Symbol_RBrace },
        { std::regex("^\\("), TokenType::Symbol_LParen },
        { std::regex("^\\)"), TokenType::Symbol_RParen },
        
        // opps (longer first)
        { std::regex("^=="), TokenType::Operator_Equal },
        { std::regex("^!="), TokenType::Operator_NotEqual },
        { std::regex("^<="), TokenType::Operator_LessEqual },
        { std::regex("^>="), TokenType::Operator_GreaterEqual },
        { std::regex("^&&"), TokenType::Operator_And },
        { std::regex("^\\|\\|"), TokenType::Operator_Or },

        { std::regex("^="),  TokenType::Operator_Assignment },
        { std::regex("^!"),  TokenType::Operator_Exclamation },
        { std::regex("^<"),  TokenType::Operator_Less },
        { std::regex("^>"),  TokenType::Operator_Greater },
        
        { std::regex("^\\+"), TokenType::Operator_Plus },
        { std::regex("^-"),  TokenType::Operator_Minus },
        { std::regex("^\\*"), TokenType::Operator_Star },
        { std::regex("^/"),  TokenType::Operator_Slash },
    };

    const std::unordered_map<std::string, TokenType> KEYWORDS = {
        {"function", TokenType::Keyword_Function},
        {"return", TokenType::Keyword_Return},
        {"var", TokenType::Keyword_Var},

        {"if", TokenType::Keyword_If},
        {"else", TokenType::Keyword_Else},

        {"while", TokenType::Keyword_While},
        {"for", TokenType::Keyword_For},
    };
};


#endif //LEXER_H