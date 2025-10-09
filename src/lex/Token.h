#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <regex>
#include <unordered_map>


enum class TokenType
{
    Identifier,


    //types
    Integer,
    //float?


    //symbols
    Semicolon, //;
    LBrace,    // {
    RBrace,    // }

    LParen,    // (
    RParen,    // )

    EndOfFile,


    //Keywords
    Function,
    Return,
    Var,

    If,
    Else,
    
    While,
    For,


    //operators
    Assignment, // =

    Plus,  // +
    Minus, // -
    Star,  // *
    Slash, // /


    //boolian ops
    And,         // &&
    Or,          // ||
    Equal,       // ==
    NotEqual,    // !=
    Exclamation, // !

    Less,         // <
    LessEqual,    // <=
    Greater,      // >
    GreaterEqual, // >=
};

struct Token
{
    TokenType type;
    std::string value;
};

const std::unordered_map<std::string, TokenType> KEYWORDS = {
    {"function", TokenType::Function},
    {"return", TokenType::Return},
    {"var", TokenType::Var},

    {"if", TokenType::If},
    {"else", TokenType::Else},

    {"while", TokenType::While},
    {"for", TokenType::For},
};

void setKeyword(Token* token)
{
    auto iterator = KEYWORDS.find(token->value);
    if (iterator != KEYWORDS.end())
    {
        token->type = iterator->second;
    }
    else 
    {
        token->type = TokenType::Identifier;
    }
}

#endif //TOKEN_H