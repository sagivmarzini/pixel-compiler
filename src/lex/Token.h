#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <regex>
#include <unordered_map>


enum class TokenType
{
    Identifier,
    Integer,
    EndOfFile,

    Keyword_Function,
    Keyword_Return,
    Keyword_Var,
    Keyword_If,
    Keyword_Else,
    Keyword_While,
    Keyword_For,

    Symbol_Semicolon,
    Symbol_LBrace,
    Symbol_RBrace,
    Symbol_LParen,
    Symbol_RParen,

    Operator_Assignment,

    Operator_Plus,
    Operator_Minus,
    Operator_Star,
    Operator_Slash,

    Operator_And,
    Operator_Or,
    Operator_Equal,
    Operator_NotEqual,
    Operator_Exclamation,

    Operator_Less,
    Operator_LessEqual,
    Operator_Greater,
    Operator_GreaterEqual,
};

struct Token
{
    TokenType type;
    std::string value;
};
#endif //TOKEN_H