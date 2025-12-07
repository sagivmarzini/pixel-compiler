#ifndef COMPILER_PROJECT_LEXER_H
#define COMPILER_PROJECT_LEXER_H

#include "Token.h"

#include <regex>
#include <unordered_map>

#include "LexerError.h"

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
    std::vector<LexerError> _errors;

    [[nodiscard]] char peek() const;

    [[nodiscard]] char peekNext() const;

    char eat();

    Token parseNumber();

    Token parseIdentifierOrKeyword();

    void skipSingleLineComment();

    void skipMultiLineComment();

    Token parseStringLiteral();
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
