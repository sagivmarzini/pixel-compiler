#ifndef COMPILER_PROJECT_EXPRESSION_H
#define COMPILER_PROJECT_EXPRESSION_H

#include <memory>
#include <vector>

#include "AstNode.h"
#include "../../lex/Token.h"
#include "types/TypeNode.h"

struct Symbol;
class Visitor;

namespace AST {
    struct Expression : AstNode {
        explicit Expression(const TokenMetadata& metadata)
            : AstNode(metadata) {
        }

        ~Expression() override = default;

        TypeNode* type = nullptr;
    };

    struct IntegerLiteralNode final : Expression {
        int value;

        IntegerLiteralNode(const TokenMetadata& metadata, const int value)
            : Expression(metadata), value(value) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };

    struct FloatLiteralNode final : Expression {
        float value;

        FloatLiteralNode(const TokenMetadata& metadata, const float value) : Expression(metadata), value(value) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };

    struct StringLiteralNode final : Expression {
        std::string value;

        StringLiteralNode(const TokenMetadata& metadata, std::string value) : Expression(metadata),
                                                                              value(std::move(value)) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };

    struct BooleanLiteralNode final : Expression {
        bool value;

        BooleanLiteralNode(const TokenMetadata& metadata, const bool value) : Expression(metadata), value(value) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };

    // Binary operations (e.g., a + b, x * y)
    struct BinaryExpression final : Expression {
        std::unique_ptr<Expression> left;
        Operator op;
        std::unique_ptr<Expression> right;

        BinaryExpression(const TokenMetadata& metadata, std::unique_ptr<Expression> left, const Operator op,
                         std::unique_ptr<Expression> right)
            : Expression(metadata), left(std::move(left)), op(op), right(std::move(right)) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };

    struct UnaryExpression final : Expression {
        std::unique_ptr<Expression> operand;
        Operator op;

        UnaryExpression(const TokenMetadata& metadata, std::unique_ptr<Expression> operand, const Operator op)
            : Expression(metadata), operand(std::move(operand)), op(op) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };

    // increment and decrement expressions ++ and --
    struct IncDecExpression final : Expression {
        std::string variableName;
        Operator op;

        enum Fix {
            Prefix,
            Postfix
        } fix;

        Symbol* symbol = nullptr;

        IncDecExpression(const TokenMetadata& metadata, const std::string& varName, const Operator op, const Fix fixPos)
            : Expression(metadata), variableName(varName), op(op), fix(fixPos) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };

    // Variable reference
    struct VariableExpression final : Expression {
        std::string name;
        Symbol* symbol = nullptr;

        VariableExpression(const TokenMetadata& metadata, std::string name) : Expression(metadata),
                                                                              name(std::move(name)) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };

    struct ArrayIndex final : Expression {
        std::string variableName; // The name of the array
        std::unique_ptr<Expression> index;
        Symbol* symbol = nullptr;

        ArrayIndex(const TokenMetadata& metadata, std::string name, std::unique_ptr<Expression> idx)
            : Expression(metadata), variableName(std::move(name)), index(std::move(idx)) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override; // Returns the loaded value at index
    };

    struct ArrayLiteral final : Expression {
        std::vector<std::unique_ptr<Expression> > elements;

        ArrayLiteral(const TokenMetadata& metadata, std::vector<std::unique_ptr<Expression> > elems)
            : Expression(metadata), elements(std::move(elems)) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };

    // Function call
    struct FunctionCall final : Expression {
        struct FunctionArgument {
            std::unique_ptr<Expression> value;
            std::optional<std::string> name;

            FunctionArgument(std::unique_ptr<Expression> value, std::optional<std::string> name)
                : value(std::move(value)), name(std::move(name)) {
            }
        };

        std::string functionName;
        std::vector<FunctionArgument> arguments;

        FunctionCall(const TokenMetadata& metadata, std::string name, std::vector<FunctionArgument> arguments)
            : Expression(metadata), functionName(std::move(name)), arguments(std::move(arguments)) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };

    struct RangeExpression final : Expression {
        std::unique_ptr<Expression> start;
        std::unique_ptr<Expression> end;

        RangeExpression(const TokenMetadata& metadata, std::unique_ptr<Expression> start,
                        std::unique_ptr<Expression> end)
            : Expression(metadata), start(std::move(start)), end(std::move(end)) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };
}

#endif //COMPILER_PROJECT_EXPRESSION_H
