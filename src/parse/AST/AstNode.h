#ifndef COMPILER_PROJECT_ASTNODE_H
#define COMPILER_PROJECT_ASTNODE_H

#include "lex/Token.h"


class Visitor;

class ASTNode {
public:
    virtual ~ASTNode() = default;

    virtual void accept(const Visitor& visitor) = 0; // For visitor pattern

protected:
    TokenMetadata _metadata{};
};

// Program root
struct Program final : ASTNode {
    std::vector<std::unique_ptr<ASTNode> > declarations;

    Program(std::vector<std::unique_ptr<ASTNode> > declarations) : declarations(std::move(declarations)) {
    }

    void accept(const Visitor& visitor) override;
};

#endif //COMPILER_PROJECT_ASTNODE_H
