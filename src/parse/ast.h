//
// Created by Sagiv Marzini on 20/11/2025.
//

#ifndef COMPILER_PROJECT_AST_H
#define COMPILER_PROJECT_AST_H
#include <string>
#include <utility>
#include <vector>
#include <memory>

class ASTNode {
public:
    virtual ~ASTNode() = default;

    virtual void accept(class ASTVisitor &visitor) = 0; // For visitor pattern
};

// Base expression class
class Expression : public ASTNode {
public:
    ~Expression() override = default;
};

class IntegerLiteral : public Expression {
public:
    int value;

    IntegerLiteral(const int val) : value(val) {
    }

    void accept(ASTVisitor &visitor) override;
};

class StringLiteral : public Expression {
public:
    std::string value;

    StringLiteral(std::string val) : value(std::move(val)) {
    }

    void accept(ASTVisitor &visitor) override;
};

// Binary operations (e.g., a + b, x * y)
class BinaryExpression : public Expression {
public:
    enum Operator { Add, Subtract, Multiply, Divide, Equal, NotEqual, LessThan, /* etc */ };

    std::unique_ptr<ASTNode> left;
    Operator op;
    std::unique_ptr<ASTNode> right;

    BinaryExpression(std::unique_ptr<ASTNode> l, Operator o, std::unique_ptr<ASTNode> r)
        : left(std::move(l)), op(o), right(std::move(r)) {
    }

    void accept(ASTVisitor &visitor) override;
};

// Variable reference
class Identifier : public Expression {
public:
    std::string name;

    Identifier(std::string n) : name(std::move(n)) {
    }

    void accept(ASTVisitor &visitor) override;
};

// Function call
class CallExpression : public Expression {
public:
    std::string functionName;
    std::vector<std::unique_ptr<ASTNode> > arguments;

    CallExpression(std::string name, std::vector<std::unique_ptr<ASTNode> > args)
        : functionName(std::move(name)), arguments(std::move(args)) {
    }

    void accept(ASTVisitor &visitor) override;
};

class Statement : public ASTNode {
public:
    ~Statement() override = default;
};

// Variable declaration: int x = 5;
class VariableDeclaration : public Statement {
public:
    std::string type;
    std::string name;
    std::unique_ptr<ASTNode> initializer; // Can be null

    VariableDeclaration(std::string t, std::string n, std::unique_ptr<ASTNode> init = nullptr)
        : type(std::move(t)), name(std::move(n)), initializer(std::move(init)) {
    }

    void accept(ASTVisitor &visitor) override;
};

// If statement
class IfStatement : public Statement {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenBranch;
    std::unique_ptr<ASTNode> elseBranch; // Can be null

    IfStatement(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> then, std::unique_ptr<ASTNode> els = nullptr)
        : condition(std::move(cond)), thenBranch(std::move(then)), elseBranch(std::move(els)) {
    }

    void accept(ASTVisitor &visitor) override;
};

// While loop
class WhileStatement : public Statement {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;

    WhileStatement(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> b)
        : condition(std::move(cond)), body(std::move(b)) {
    }

    void accept(ASTVisitor &visitor) override;
};

// Return statement
class ReturnStatement : public Statement {
public:
    std::unique_ptr<ASTNode> value; // Can be null for void returns

    ReturnStatement(std::unique_ptr<ASTNode> val = nullptr) : value(std::move(val)) {
    }

    void accept(ASTVisitor &visitor) override;
};

// Block (compound statement)
class Block : public Statement {
public:
    std::vector<std::unique_ptr<ASTNode> > statements;

    Block(std::vector<std::unique_ptr<ASTNode> > stmts) : statements(std::move(stmts)) {
    }

    void accept(ASTVisitor &visitor) override;
};

// Function declaration
class FunctionDeclaration : public ASTNode {
public:
    struct Parameter {
        std::string type;
        std::string name;
    };

    std::string returnType;
    std::string name;
    std::vector<Parameter> parameters;
    ASTNodePtr body; // Usually a Block
    FunctionDeclaration(const std::string &ret, const std::string &n,
                        std::vector<Parameter> params, ASTNodePtr b)
        : returnType(ret), name(n), parameters(std::move(params)), body(std::move(b)) {
    }

    void accept(ASTVisitor &visitor) override;
};

// Program root
class Program : public ASTNode {
public:
    std::vector<ASTNodePtr> declarations;

    Program(std::vector<ASTNodePtr> decls) : declarations(std::move(decls)) {
    }

    void accept(ASTVisitor &visitor) override;
};

#endif // AST_H

#endif //COMPILER_PROJECT_AST_H
