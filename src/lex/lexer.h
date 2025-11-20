#ifndef COMPILER_PROJECT_LEXER_H
#define COMPILER_PROJECT_LEXER_H

#include "token.h"

#include <regex>
#include <unordered_map>

class lexer {
public:
    explicit lexer(std::string sourceCode);

    ~lexer() = default;


    [[nodiscard]] std::vector<token> lex();

private:
    std::string _sourceCode;
    size_t _position;

    [[nodiscard]] char peek() const;

    char eat();

    token parseNumber();

    token parseIdentifierOrKeyword();

    static token makeToken(const TokenType &type);
};

const std::unordered_map<std::string, Keyword> Keywords = {
    {"func", Keyword::Func},
    {"var", Keyword::Var},
    {"return", Keyword::Return},
    {"if", Keyword::If},
    {"else", Keyword::Else},
    {"while", Keyword::While},
    {"for", Keyword::For}
};

const std::unordered_map<std::string, Type> Types = {
    {"int", Type::Int},
    {"float", Type::Float},
    {"bool", Type::Bool},
    {"ptr", Type::Ptr},
    {"string", Type::String},
    {"color", Type::Color},
    {"void", Type::Void}
};

const std::unordered_map<std::string, BooleanLiteral> Booleans = {
    {"true", BooleanLiteral{true}},
    {"false", BooleanLiteral{false}}
};


#endif //COMPILER_PROJECT_LEXER_H
