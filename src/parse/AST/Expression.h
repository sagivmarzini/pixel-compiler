#ifndef COMPILER_PROJECT_EXPRESSION_H
#define COMPILER_PROJECT_EXPRESSION_H

#include <memory>
#include <vector>

#include "AstNode.h"
#include "../../lex/Token.h"

struct Symbol;
class Visitor;

struct Expression : AstNode {
    explicit Expression(const TokenMetadata& metadata)
        : AstNode(metadata) {
    }

    ~Expression() override = default;

    Type type = Type::Unspecified;
};

struct IntegerLiteralNode final : Expression {
    int value;

    IntegerLiteralNode(const TokenMetadata& metadata, const int value)
        : Expression(metadata), value(value) {
    }

    void accept(AstVisitor& visitor) override;
};

struct FloatLiteralNode final : Expression {
    float value;

    FloatLiteralNode(const TokenMetadata& metadata, const float value) : Expression(metadata), value(value) {
    }

    void accept(AstVisitor& visitor) override;
};

struct StringLiteralNode final : Expression {
    std::string value;

    StringLiteralNode(const TokenMetadata& metadata, std::string value) : Expression(metadata),
                                                                          value(std::move(value)) {
    }

    void accept(AstVisitor& visitor) override;
};

struct BooleanLiteralNode final : Expression {
    bool value;

    BooleanLiteralNode(const TokenMetadata& metadata, const bool value) : Expression(metadata), value(value) {
    }

    void accept(AstVisitor& visitor) override;
};

// Binary operations (e.g., a + b, x * y)
struct BinaryExpression final : Expression {
    std::unique_ptr<Expression> left;
    Operator                    op;
    std::unique_ptr<Expression> right;

    BinaryExpression(const TokenMetadata&        metadata, std::unique_ptr<Expression> left, const Operator op,
                     std::unique_ptr<Expression> right)
        : Expression(metadata), left(std::move(left)), op(op), right(std::move(right)) {
    }

    void accept(AstVisitor& visitor) override;
};

struct UnaryExpression final : Expression {
    std::unique_ptr<Expression> operand;
    Operator                    op;

    UnaryExpression(const TokenMetadata& metadata, std::unique_ptr<Expression> operand, const Operator op)
        : Expression(metadata), operand(std::move(operand)), op(op) {
    }

    void accept(AstVisitor& visitor) override;
};

// Variable reference
struct IdentifierNode final : Expression {
    std::string name;
    Symbol*     symbol = nullptr;

    IdentifierNode(const TokenMetadata& metadata, std::string name) : Expression(metadata), name(std::move(name)) {
    }

    void accept(AstVisitor& visitor) override;
};

// Function call
struct FunctionCall final : Expression {
    struct FunctionArgument {
        std::string                 name;
        std::unique_ptr<Expression> value;

        FunctionArgument(std::string name, std::unique_ptr<Expression> value)
            : name(std::move(name)), value(std::move(value)) {
        }
    };

    std::string                   functionName;
    std::vector<FunctionArgument> arguments;

    FunctionCall(const TokenMetadata& metadata, std::string name, std::vector<FunctionArgument> arguments)
        : Expression(metadata), functionName(std::move(name)), arguments(std::move(arguments)) {
    }

    void accept(AstVisitor& visitor) override;
};

struct RangeExpression final : Expression {
    std::unique_ptr<Expression> start;
    std::unique_ptr<Expression> end;

    RangeExpression(const TokenMetadata& metadata, std::unique_ptr<Expression> start, std::unique_ptr<Expression> end)
        : Expression(metadata), start(std::move(start)), end(std::move(end)) {
    }

    void accept(AstVisitor& visitor) override;
};

#endif //COMPILER_PROJECT_EXPRESSION_H
