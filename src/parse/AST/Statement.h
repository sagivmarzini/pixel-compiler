#ifndef COMPILER_PROJECT_STATEMENT_H
#define COMPILER_PROJECT_STATEMENT_H

#include "Expression.h"

struct TypeNode;
class Scope;
struct Symbol;

namespace AST {
    class Statement : public AST::AstNode {
    public:
        explicit Statement(const TokenMetadata& metadata)
            : AstNode(metadata) {
        }

        ~Statement() override = default;
    };

    // Variable declaration: int x = 5;
    struct VariableDeclaration : Statement {
        bool                        isConst;
        TypeNode*                   type;
        std::string                 name;
        std::unique_ptr<Expression> initializer; // Can be null

        Symbol* symbol = nullptr;

        VariableDeclaration(const TokenMetadata&        metadata, bool isConst, TypeNode* type, std::string name,
                            std::unique_ptr<Expression> init = nullptr)
            : Statement(metadata), isConst(isConst), type(type), name(std::move(name)),
              initializer(std::move(init)) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };

    struct VariableAssignment : Statement {
        std::string                 varName;
        std::unique_ptr<Expression> assignedValue;
        Symbol*                     symbol = nullptr;

        VariableAssignment(const TokenMetadata& metadata, std::string name, std::unique_ptr<Expression> value)
            : Statement(metadata), varName(std::move(name)), assignedValue(std::move(value)) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };

    struct ArrayAssignment : Statement {
        std::string                 varName;
        std::unique_ptr<Expression> index;
        std::unique_ptr<Expression> assignedValue;
        Symbol*                     symbol = nullptr;

        ArrayAssignment(const TokenMetadata&        metadata, std::string name, std::unique_ptr<Expression> index,
                        std::unique_ptr<Expression> value)
            : Statement(metadata), varName(std::move(name)),
              index(std::move(index)), assignedValue(std::move(value)) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };

    // Return statement
    struct ReturnStatement : Statement {
        std::unique_ptr<Expression> value; // Can be null for void returns

        explicit ReturnStatement(const TokenMetadata& metadata, std::unique_ptr<Expression> val = nullptr)
            : Statement(metadata), value(std::move(val)) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };

    // Block (compound statement)
    struct Block : Statement {
        std::vector<std::unique_ptr<Statement> > statements;
        Scope*                                   scope;

        explicit Block(const TokenMetadata& metadata, std::vector<std::unique_ptr<Statement> > stmts = {})
            : Statement(metadata), statements(std::move(stmts)), scope(nullptr) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };

    // While loop
    struct WhileLoop : Statement {
        std::unique_ptr<Expression> condition;
        std::unique_ptr<Statement>  body;

        WhileLoop(const TokenMetadata& metadata, std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
            : Statement(metadata), condition(std::move(condition)), body(std::move(body)) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };

    struct ForLoop : Statement {
        std::string                      identifier;
        Symbol*                          symbol = nullptr;
        std::unique_ptr<RangeExpression> range;
        std::unique_ptr<Expression>      step;
        std::unique_ptr<Statement>       body;

        ForLoop(const TokenMetadata&        metadata, std::string identifier, std::unique_ptr<RangeExpression> range,
                std::unique_ptr<Expression> step, std::unique_ptr<Statement> body)
            : Statement(metadata), identifier(std::move(identifier)), range(std::move(range)), step(std::move(step)),
              body(std::move(body)) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };

    // If statement
    struct IfStatement : Statement {
        std::unique_ptr<Expression> condition;
        std::unique_ptr<Statement>  thenBranch;
        std::unique_ptr<Statement>  elseBranch; // Can be null

        IfStatement(const TokenMetadata&       metadata, std::unique_ptr<Expression> condition,
                    std::unique_ptr<Statement> thenBranch,
                    std::unique_ptr<Statement> elseBranch = nullptr)
            : Statement(metadata), condition(std::move(condition)), thenBranch(std::move(thenBranch)),
              elseBranch(std::move(elseBranch)) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };


    // Function declaration
    struct FunctionDeclaration : Statement {
        struct FunctionParameter {
            std::string name;
            TypeNode*   type;
            bool        isImplicit;
            Symbol*     symbol = nullptr;

            FunctionParameter(std::string name, TypeNode* type, bool isImplicit = false)
                : name(std::move(name)), type(type), isImplicit(isImplicit) {
            }
        };

        std::string                    name;
        TypeNode*                      returnType;
        std::vector<FunctionParameter> parameters;
        std::unique_ptr<Block>         body;
        Symbol*                        symbol = nullptr;

        FunctionDeclaration(const TokenMetadata&           metadata, TypeNode* returnType, std::string name,
                            std::vector<FunctionParameter> parameters, std::unique_ptr<Block> body)
            : Statement(metadata), name(std::move(name)), returnType(returnType), parameters(std::move(parameters)),
              body(std::move(body)) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };

    struct ExpressionStatement : Statement {
        std::unique_ptr<Expression> expression;

        ExpressionStatement(const TokenMetadata& metadata, std::unique_ptr<Expression> expression)
            : Statement(metadata), expression(std::move(expression)) {
        }

        void accept(AstVisitor& visitor) override;

        llvm::Value* acceptIR(IRGeneratorLLVM& visitor) override;
    };
}


#endif //COMPILER_PROJECT_STATEMENT_H
