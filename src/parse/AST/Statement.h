#ifndef COMPILER_PROJECT_STATEMENT_H
#define COMPILER_PROJECT_STATEMENT_H

#include "Expression.h"

#include <string>
#include <memory>

class Statement : public ASTNode {
public:
    ~Statement() override = default;
};

// Variable declaration: int x = 5;
struct VariableDeclaration : Statement {
    Type type;
    Identifier name;
    std::unique_ptr<Expression> initializer; // Can be null

    VariableDeclaration(const Type t, Identifier n, std::unique_ptr<Expression> init = nullptr)
        : type(t), name(std::move(n)), initializer(std::move(init)) {
    }

    void accept(Visitor &visitor) override;
};

struct VariableAssignment : Statement {
    Identifier name;
    std::unique_ptr<Expression> newValue;

    VariableAssignment(Identifier n, std::unique_ptr<Expression> value)
        : name(std::move(n)), newValue(std::move(value)) {
    }

    void accept(Visitor &visitor) override;
};

// Return statement
struct ReturnStatement : Statement {
    std::unique_ptr<Expression> value; // Can be null for void returns

    ReturnStatement(std::unique_ptr<Expression> val = nullptr) : value(std::move(val)) {
    }

    void accept(Visitor &visitor) override;
};

// Block (compound statement)
struct Block : Statement {
    std::vector<std::unique_ptr<Statement> > statements;

    Block(std::vector<std::unique_ptr<Statement> > stmts) : statements(std::move(stmts)) {
    }

    void accept(Visitor &visitor) override;
};

// While loop
struct WhileStatement : Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Block> body;

    WhileStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Block> b)
        : condition(std::move(cond)), body(std::move(b)) {
    }

    void accept(Visitor &visitor) override;
};

// If statement
struct IfStatement : Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Block> thenBranch;
    std::unique_ptr<Block> elseBranch; // Can be null

    IfStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Block> then,
                std::unique_ptr<Block> els = nullptr)
        : condition(std::move(condition)), thenBranch(std::move(then)), elseBranch(std::move(els)) {
    }

    void accept(Visitor &visitor) override;
};

// Function declaration
struct FunctionDeclaration : ASTNode {
    struct Parameter {
        Type type;
        Identifier name;

        Parameter(const Type t, Identifier n)
            : type(t), name(std::move(n)) {
        }
    };

    Type returnType;
    Identifier name;
    std::vector<Parameter> parameters;
    std::unique_ptr<ASTNode> body; // Usually a Block
    FunctionDeclaration(const Type ret, Identifier n,
                        std::vector<Parameter> params, std::unique_ptr<ASTNode> b)
        : returnType(ret), name(std::move(n)), parameters(std::move(params)), body(std::move(b)) {
    }

    void accept(Visitor &visitor) override;
};

// Program root
struct Program : ASTNode {
    std::vector<std::unique_ptr<ASTNode> > declarations;

    Program(std::vector<std::unique_ptr<ASTNode> > &decls) : declarations(std::move(decls)) {
    }

    void accept(Visitor &visitor) override;
};


#endif //COMPILER_PROJECT_STATEMENT_H
