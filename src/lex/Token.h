#ifndef COMPILER_PROJECT_TOKEN_H
#define COMPILER_PROJECT_TOKEN_H

#include <string>
#include <utility>
#include <variant>
#include <unordered_map>
#include <iostream>

enum class Keyword {
    Func,
    Var, Const,
    Return,
    If, Else,
    While,
    For, In, Step
};

std::ostream& operator<<(std::ostream& os, const Keyword& keyword);


enum class Type {
    Unspecified,

    Int,
    Float,
    Boolean,
    Ptr,
    String,
    Color,
    Void,

    Error
};

std::ostream& operator<<(std::ostream& os, const Type& type);


// Token payload structs
struct Identifier {
    std::string name;
};

struct IntegerLiteral {
    int value;
};

struct FloatLiteral {
    float value;
};

struct StringLiteral {
    std::string value;
};

struct BooleanLiteral {
    bool value;
};

enum class Operator {
    Assignment, // =

    Plus,       // +
    PlusPlus,   // ++
    Minus,      // -
    MinusMinus, // --
    Star,       // *
    Slash,      // /

    And,         // &&
    Or,          // ||
    Equal,       // ==
    NotEqual,    // !=
    Exclamation, // !

    LessThan,     // <
    LessEqual,    // <=
    GreaterThan,  // >
    GreaterEqual, // >=
};

std::ostream& operator<<(std::ostream& os, const Operator& op);

struct Semicolon {
};

struct Colon {
};

struct Comma {
};

struct LBrace {
};

struct RBrace {
};

struct LParen {
};

struct RParen {
};

struct Arrow {
};

struct DoubleDot {
};

struct EndOfFile {
};

// Token variant
using TokenType = std::variant<
    // simple tokens
    Semicolon,
    Colon,
    Comma,
    LBrace,
    RBrace,
    LParen,
    RParen,
    Arrow,
    DoubleDot,

    // payload tokens
    Operator,

    Keyword,
    IntegerLiteral,
    FloatLiteral,
    StringLiteral,
    BooleanLiteral,
    Type,
    Identifier,

    // EOF
    EndOfFile
>;

static const std::unordered_map<std::string, TokenType> keywords = {
    {"func", Keyword::Func},
    {"var", Keyword::Var},
    {"const", Keyword::Const},
    {"return", Keyword::Return},
    {"if", Keyword::If},
    {"else", Keyword::Else},
    {"while", Keyword::While},
    {"for", Keyword::For},
    {"in", Keyword::In},
    {"step", Keyword::Step},

    {"int", Type::Int},
    {"float", Type::Float},
    {"bool", Type::Boolean},
    {"ptr", Type::Ptr},
    {"string", Type::String},
    {"Color", Type::Color},
    {"void", Type::Void},

    {"true", BooleanLiteral{true}},
    {"false", BooleanLiteral{false}},
};

struct TokenMetadata {
    int         line;
    int         col;
    std::string lexeme;
};

struct Token {
    TokenType     type;
    TokenMetadata metadata{};

    Token() = default;

    Token(TokenType type, int line, int col, std::string lexeme)
        : type(std::move(type)), metadata(line, col, std::move(lexeme)) {
    }
};

std::ostream& operator<<(std::ostream& os, const Token& token);

std::string typeToString(Type type);

std::string operatorToString(Operator op);

std::string keywordToString(Keyword keyword);

std::string tokenTypeToString(const TokenType& tokenType);


#endif // COMPILER_PROJECT_TOKEN_H
