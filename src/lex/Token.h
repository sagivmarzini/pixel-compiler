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

// TODO: move this to TypeNode.h
enum class PrimitiveKind {
    Unspecified,

    Int,
    Float,
    Bool,
    Pointer,
    String,
    Color,
    Void,

    Error
};

std::ostream& operator<<(std::ostream& os, const PrimitiveKind& type);


// Token payload structs
struct Identifier {
    std::string name;
};

struct IntegerLiteral {
    int value = -1;
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

    Plus, // +
    PlusPlus, // ++
    Minus, // -
    MinusMinus, // --
    Star, // *
    Slash, // /

    LogicalAnd, // &&
    LogicalOr, // ||
    Equal, // ==
    NotEqual, // !=
    Exclamation, // !

    LessThan, // <
    LessEqual, // <=
    GreaterThan, // >
    GreaterEqual, // >=
};

std::ostream& operator<<(std::ostream& os, const Operator& op);

struct Semicolon {
};

struct Colon {
};

struct Comma {
};

struct LeftBrace {
};

struct RightBrace {
};

struct LeftParen {
};

struct RightParen {
};

struct LeftBracket {
};

struct RightBracket {
};

struct Arrow {
};

struct DoubleDot {
};

struct Underscore {
};

struct EndOfFile {
};


// Token variant

using TokenType = std::variant<
    // simple tokens
    Semicolon,
    Colon,
    Comma,
    LeftBrace,
    RightBrace,
    LeftParen,
    RightParen,
    LeftBracket,
    RightBracket,
    Arrow,
    DoubleDot,
    Underscore,

    // payload tokens
    Operator,

    Keyword,
    IntegerLiteral,
    FloatLiteral,
    StringLiteral,
    BooleanLiteral,
    PrimitiveKind,
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

    {"Int", PrimitiveKind::Int},
    {"Float", PrimitiveKind::Float},
    {"Bool", PrimitiveKind::Bool},
    {"String", PrimitiveKind::String},
    {"Color", PrimitiveKind::Color},
    {"Void", PrimitiveKind::Void},

    {"true", BooleanLiteral{true}},
    {"false", BooleanLiteral{false}},
};

struct TokenMetadata {
    int line;
    int col;
    std::string lexeme;
};

struct Token {
    TokenType type;
    TokenMetadata metadata{};

    Token() = default;

    Token(TokenType type, int line, int col, std::string lexeme);
};

std::ostream& operator<<(std::ostream& os, const Token& token);

std::string typeToString(PrimitiveKind type);

std::string operatorToString(Operator op);

std::string keywordToString(Keyword keyword);

std::string tokenTypeToString(const TokenType& tokenType);


#endif // COMPILER_PROJECT_TOKEN_H
