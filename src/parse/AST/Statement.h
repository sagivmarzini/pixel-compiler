#ifndef COMPILER_PROJECT_STATEMENT_H
#define COMPILER_PROJECT_STATEMENT_H

#include "Expression.h"

class Scope;
struct Symbol;

class Statement : public AstNode {
public:
    ~Statement() override = default;
};

// Variable declaration: int x = 5;
struct VariableDeclaration : Statement {
    bool                        isConst;
    Type                        specifiedType;
    std::string                 name;
    std::unique_ptr<Expression> value; // Can be null
    Symbol*                     symbol = nullptr;

    VariableDeclaration(bool isConst, const Type type, std::string name, std::unique_ptr<Expression> init = nullptr)
        : isConst(isConst), specifiedType(type), name(std::move(name)), value(std::move(init)) {
    }

    void accept(AstVisitor& visitor) override;
};

struct VariableAssignment : Statement {
    std::string                 varName;
    std::unique_ptr<Expression> assignedValue;

    VariableAssignment(std::string name, std::unique_ptr<Expression> value)
        : varName(std::move(name)), assignedValue(std::move(value)) {
    }

    void accept(AstVisitor& visitor) override;
};

// Return statement
struct ReturnStatement : Statement {
    std::unique_ptr<Expression> value; // Can be null for void returns

    ReturnStatement(std::unique_ptr<Expression> val = nullptr) : value(std::move(val)) {
    }

    void accept(AstVisitor& visitor) override;
};

// Block (compound statement)
struct Block : Statement {
    std::vector<std::unique_ptr<Statement> > statements;
    Scope*                                   scope;

    Block() = default;

    Block(std::vector<std::unique_ptr<Statement> > stmts) : statements(std::move(stmts)) {
    }

    void accept(AstVisitor& visitor) override;
};

// While loop
struct WhileLoop : Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement>  body;

    WhileLoop(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
        : condition(std::move(condition)), body(std::move(body)) {
    }

    void accept(AstVisitor& visitor) override;
};

struct ForLoop : Statement {
    std::string                      identifier;
    std::unique_ptr<RangeExpression> range;
    std::unique_ptr<Expression>      step;
    std::unique_ptr<Statement>       body;

    ForLoop(std::string                 identifier, std::unique_ptr<RangeExpression> range,
            std::unique_ptr<Expression> step, std::unique_ptr<Statement>             body)
        : identifier(std::move(identifier)), range(std::move(range)), step(std::move(step)), body(std::move(body)) {
    }

    void accept(AstVisitor& visitor) override;
};

// If statement
struct IfStatement : Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement>  thenBranch;
    std::unique_ptr<Statement>  elseBranch; // Can be null

    IfStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> thenBranch,
                std::unique_ptr<Statement>  elseBranch = nullptr)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {
    }

    void accept(AstVisitor& visitor) override;
};


// Function declaration
struct FunctionDeclaration : Statement {
    struct FunctionParameter {
        std::string name;
        Type        type;

        FunctionParameter(std::string name, const Type type)
            : name(std::move(name)), type(type) {
        }
    };

    std::string                    name;
    Type                           returnType;
    std::vector<FunctionParameter> parameters;
    std::unique_ptr<Block>         body;
    Symbol*                        symbol = nullptr;

    FunctionDeclaration(const Type                     returnType, std::string            name,
                        std::vector<FunctionParameter> parameters, std::unique_ptr<Block> body)
        : returnType(returnType), name(std::move(name)), parameters(std::move(parameters)), body(std::move(body)) {
    }

    void accept(AstVisitor& visitor) override;
};

struct ExpressionStatement : Statement {
    std::unique_ptr<Expression> expression;

    explicit ExpressionStatement(std::unique_ptr<Expression> expression) : expression(std::move(expression)) {
    }

    void accept(AstVisitor& visitor) override;
};


#endif //COMPILER_PROJECT_STATEMENT_H
