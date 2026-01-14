#include "Token.h"

#include <stdexcept>

std::ostream& operator<<(std::ostream& os, const Keyword& keyword) {
    return os << keywordToString(keyword);
}

std::ostream& operator<<(std::ostream& os, const Type& type) {
    return os << typeToString(type);
}

std::ostream& operator<<(std::ostream& os, const Operator& op) {
    return os << operatorToString(op);
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << tokenTypeToString(token.type);
    return os;
}

std::string typeToString(const Type type) {
    switch (type) {
        case Type::Int: return "int";
        case Type::Float: return "float";
        case Type::Boolean: return "boolean";
        case Type::Ptr: return "ptr";
        case Type::String: return "string";
        case Type::Color: return "color";
        case Type::Void: return "void";
        case Type::Unspecified: return "Type"; // for printing 'expected a 'type' token'
        case Type::Error: return "Error";
        default: return "unknown type";
    }
}

std::string operatorToString(Operator op) {
    switch (op) {
        case Operator::Assignment: return "=";
        case Operator::Plus: return "+";
        case Operator::PlusPlus: return "++";
        case Operator::Minus: return "-";
        case Operator::MinusMinus: return "--";
        case Operator::Star: return "*";
        case Operator::Slash: return "/";
        case Operator::And: return "&&";
        case Operator::Or: return "||";
        case Operator::Equal: return "==";
        case Operator::NotEqual: return "!=";
        case Operator::Exclamation: return "!";
        case Operator::LessThan: return "<";
        case Operator::LessEqual: return "<=";
        case Operator::GreaterThan: return ">";
        case Operator::GreaterEqual: return ">=";
        default: return "unknown operator";
    }
}

std::string keywordToString(Keyword keyword) {
    switch (keyword) {
        case Keyword::Func: return "Func";
        case Keyword::Var: return "Var";
        case Keyword::Const: return "Const";
        case Keyword::Return: return "Return";
        case Keyword::If: return "If";
        case Keyword::Else: return "Else";
        case Keyword::While: return "While";
        case Keyword::For: return "For";
        case Keyword::In: return "In";
        case Keyword::Step: return "Step";
        default: throw std::runtime_error("Print function not implemented for this keyword");
    }
}

std::string tokenTypeToString(const TokenType& tokenType) {
    return std::visit([]<typename U>(U&& arg) -> std::string {
        using T = std::decay_t<U>;

        if constexpr (std::is_same_v<T, Semicolon>) return ";";
        else if constexpr (std::is_same_v<T, Comma>) return ",";
        else if constexpr (std::is_same_v<T, Colon>) return ":";
        else if constexpr (std::is_same_v<T, LBrace>) return "{";
        else if constexpr (std::is_same_v<T, RBrace>) return "}";
        else if constexpr (std::is_same_v<T, LParen>) return "(";
        else if constexpr (std::is_same_v<T, RParen>) return ")";
        else if constexpr (std::is_same_v<T, Arrow>) return "->";
        else if constexpr (std::is_same_v<T, DoubleDot>) return "..";
        else if constexpr (std::is_same_v<T, EndOfFile>) return "EndOfFile";
        else if constexpr (std::is_same_v<T, Operator>) {
            return operatorToString(arg);
        } else if constexpr (std::is_same_v<T, Keyword>) {
            return keywordToString(arg);
        } else if constexpr (std::is_same_v<T, Type>) {
            return typeToString(arg);
        } else if constexpr (std::is_same_v<T, Identifier>) {
            return "Identifier(" + arg.name + ")";
        } else if constexpr (std::is_same_v<T, IntegerLiteral>) {
            return "IntegerLiteral(" + std::to_string(arg.value) + ")";
        } else if constexpr (std::is_same_v<T, FloatLiteral>) {
            return "FloatLiteral(" + std::to_string(arg.value) + ")";
        } else if constexpr (std::is_same_v<T, StringLiteral>) {
            return "StringLiteral(\"" + arg.value + "\")";
        } else if constexpr (std::is_same_v<T, BooleanLiteral>) {
            return arg.value ? "BooleanLiteral(true)" : "BooleanLiteral(false)";
        }

        throw std::runtime_error("Print not implemented for this token");
    }, tokenType);
}


