#ifndef COMPILER_PROJECT_ASTNODE_H
#define COMPILER_PROJECT_ASTNODE_H

#include "lex/Token.h"


class Visitor;

class AstNode {
public:
    AstNode() = default;

    AstNode(const int line, const int col, std::string lexeme)
        : _metadata{line, col, std::move(lexeme)} {
    }

    virtual ~AstNode() = default;

    virtual void accept(const Visitor& visitor) = 0; // For visitor pattern

protected:
    TokenMetadata _metadata;
};

// Program root
struct Program final : AstNode {
    std::vector<std::unique_ptr<AstNode> > declarations;

    Program(std::vector<std::unique_ptr<AstNode> > declarations) : declarations(std::move(declarations)) {
    }

    void accept(const Visitor& visitor) override;
};

#endif //COMPILER_PROJECT_ASTNODE_H
