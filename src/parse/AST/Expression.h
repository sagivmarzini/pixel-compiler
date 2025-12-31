#ifndef COMPILER_PROJECT_EXPRESSION_H
#define COMPILER_PROJECT_EXPRESSION_H

#include <memory>
#include <vector>

#include "AstNode.h"
#include "../../lex/Token.h"

struct Symbol;
class Visitor;

struct Expression : AstNode {
    ~Expression() override = default;

    Type type = Type::Unspecified;
};

struct IntegerLiteralNode final : Expression {
    int value;

    IntegerLiteralNode(const int value) : value(value) {
    }

    void accept(AstVisitor& visitor) override;
};

struct FloatLiteralNode final : Expression {
    float value;

    FloatLiteralNode(const float value) : value(value) {
    }

    void accept(AstVisitor& visitor) override;
};

struct StringLiteralNode final : Expression {
    std::string value;

    StringLiteralNode(std::string value) : value(std::move(value)) {
    }

    void accept(AstVisitor& visitor) override;
};

struct BooleanLiteralNode final : Expression {
    bool value;

    BooleanLiteralNode(const bool value) : value(value) {
    }

    void accept(AstVisitor& visitor) override;
};

// Binary operations (e.g., a + b, x * y)
struct BinaryExpression final : Expression {
    std::unique_ptr<Expression> left;
    Operator                    op;
    std::unique_ptr<Expression> right;

    BinaryExpression(std::unique_ptr<Expression> left, const Operator op, std::unique_ptr<Expression> right)
        : left(std::move(left)), op(op), right(std::move(right)) {
    }

    void accept(AstVisitor& visitor) override;
};

struct UnaryExpression final : Expression {
    std::unique_ptr<Expression> operand;
    Operator                    op;

    UnaryExpression(std::unique_ptr<Expression> operand, const Operator op)
        : operand(std::move(operand)), op(op) {
    }

    void accept(AstVisitor& visitor) override;
};

// Variable reference
struct IdentifierNode final : Expression {
    std::string name;
    Symbol*     symbol = nullptr;

    IdentifierNode(std::string name) : name(std::move(name)) {
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

    FunctionCall(std::string name, std::vector<FunctionArgument> arguments)
        : functionName(std::move(name)), arguments(std::move(arguments)) {
    }

    void accept(AstVisitor& visitor) override;
};

struct RangeExpression final : Expression {
    std::unique_ptr<Expression> start;
    std::unique_ptr<Expression> end;

    RangeExpression(std::unique_ptr<Expression> start, std::unique_ptr<Expression> end)
        : start(std::move(start)), end(std::move(end)) {
    }

    void accept(AstVisitor& visitor) override;
};

#endif //COMPILER_PROJECT_EXPRESSION_H
