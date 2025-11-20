#ifndef COMPILER_PROJECT_EXPRESSION_H
#define COMPILER_PROJECT_EXPRESSION_H

#include <string>
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
    long value;

    IntegerLiteralNode(const long val) : value(val) {
    }

    void accept(Visitor &visitor) override;
};

struct StringLiteralNode : Expression {
    std::string value;

    StringLiteralNode(std::string val) : value(std::move(val)) {
    }

    void accept(Visitor &visitor) override;
};

// Binary operations (e.g., a + b, x * y)
struct BinaryExpression : Expression {
    std::unique_ptr<Expression> left;
    Operator op;
    std::unique_ptr<Expression> right;

    BinaryExpression(std::unique_ptr<Expression> l, Operator o, std::unique_ptr<Expression> r)
        : left(std::move(l)), op(o), right(std::move(r)) {
    }

    void accept(Visitor &visitor) override;
};

// Variable reference
struct IdentifierNode : Expression {
    std::string name;

    IdentifierNode(std::string n) : name(std::move(n)) {
    }

    void accept(Visitor &visitor) override;
};

// Function call
struct CallExpression : Expression {
    std::string functionName;
    std::vector<std::unique_ptr<Expression> > arguments;

    CallExpression(std::string name, std::vector<std::unique_ptr<Expression> > args)
        : functionName(std::move(name)), arguments(std::move(args)) {
    }

    void accept(Visitor &visitor) override;
};

#endif //COMPILER_PROJECT_EXPRESSION_H
