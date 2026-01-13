#ifndef COMPILER_PROJECT_ASTNODE_H
#define COMPILER_PROJECT_ASTNODE_H

#include <memory>
#include <utility>
#include <vector>
#include "lex/Token.h"


class Scope;
class Statement;
class AstVisitor;

struct AstNode {
    explicit AstNode(TokenMetadata metadata)
        : metadata{std::move(metadata)} {
    }

    virtual ~AstNode() = default;

    virtual void accept(AstVisitor& visitor) = 0; // For visitor pattern

    TokenMetadata metadata;
};

// Program root
struct Program final : AstNode {
    std::vector<std::unique_ptr<AstNode> > statements;
    Scope*                                 scope = nullptr;

    explicit Program(const TokenMetadata& metadata)
        : AstNode(metadata) {
    }

    void accept(AstVisitor& visitor) override;

    void addStatement(std::unique_ptr<Statement> statement);
};

#endif //COMPILER_PROJECT_ASTNODE_H
