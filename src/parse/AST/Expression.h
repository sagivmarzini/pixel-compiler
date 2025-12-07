#ifndef COMPILER_PROJECT_EXPRESSION_H
#define COMPILER_PROJECT_EXPRESSION_H

#include <memory>
#include <vector>

#include "AstNode.h"
#include "../../lex/Token.h"

class Visitor;

// Base expression class
class Expression : public ASTNode {
public:
    ~Expression() override = default;
};

struct IntegerLiteralNode : Expression {
    int value;

    IntegerLiteralNode(const int value) : value(value) {
    }

    void accept(const Visitor& visitor) override;
};

struct StringLiteralNode : Expression {
    std::string value;

    StringLiteralNode(std::string value) : value(std::move(value)) {
    }

    void accept(const Visitor& visitor) override;
};

struct BooleanLiteralNode : Expression {
    bool value;

    BooleanLiteralNode(const bool value) : value(value) {
    }

    void accept(const Visitor& visitor) override;
};

// Binary operations (e.g., a + b, x * y)
struct BinaryExpression : Expression {
    std::unique_ptr<Expression> left;
    Operator op;
    std::unique_ptr<Expression> right;

    BinaryExpression(std::unique_ptr<Expression> left, const Operator op, std::unique_ptr<Expression> right)
        : left(std::move(left)), op(op), right(std::move(right)) {
    }

    void accept(const Visitor& visitor) override;
};

struct UnaryExpression : Expression {
    std::unique_ptr<Expression> operand;
    Operator op;

    UnaryExpression(std::unique_ptr<Expression> operand, const Operator op)
        : operand(std::move(operand)), op(op) {
    }

    void accept(const Visitor& visitor) override;
};

// Variable reference
struct IdentifierNode : Expression {
    std::string name;

    IdentifierNode(std::string name) : name(std::move(name)) {
    }

    void accept(const Visitor& visitor) override;
};

struct FunctionArgument {
    std::string name;
    std::unique_ptr<Expression> value;

    FunctionArgument(std::string name, std::unique_ptr<Expression> value)
        : name(std::move(name)), value(std::move(value)) {
    }
};

// Function call
struct FunctionCall : Expression {
    std::string functionName;
    std::vector<FunctionArgument> arguments;

    FunctionCall(std::string name, std::vector<FunctionArgument> arguments)
        : functionName(std::move(name)), arguments(std::move(arguments)) {
    }

    void accept(const Visitor& visitor) override;
};

#endif //COMPILER_PROJECT_EXPRESSION_H
