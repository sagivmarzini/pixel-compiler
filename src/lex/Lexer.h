#ifndef COMPILER_PROJECT_LEXER_H
#define COMPILER_PROJECT_LEXER_H

#include "Token.h"

#include <regex>
#include <unordered_map>

class Lexer {
public:
    explicit Lexer(std::string sourceCode);

    ~Lexer() = default;


    [[nodiscard]] std::vector<Token> lex();

private:
    std::string _sourceCode;
    size_t _position;
    int _line;
    int _col;

    [[nodiscard]] char peek() const;

    char eat();

    Token parseNumber();

    Token parseIdentifierOrKeyword();

    static Token makeToken(const TokenType &type, int line, int col, const std::string &lexeme);
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
