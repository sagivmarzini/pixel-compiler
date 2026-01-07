#ifndef COMPILER_PROJECT_ASTNODE_H
#define COMPILER_PROJECT_ASTNODE_H

#include "lex/Token.h"


class Statement;
class AstVisitor;

class AstNode {
public:
    AstNode() = default;

    AstNode(const int line, const int col, std::string lexeme)
        : _metadata{line, col, std::move(lexeme)} {
    }

    virtual ~AstNode() = default;

    virtual void accept(AstVisitor& visitor) = 0; // For visitor pattern

protected:
    TokenMetadata _metadata;
};

// Program root
struct Program final : AstNode {
    std::vector<std::unique_ptr<AstNode> > statements;

    Program() = default;

    void accept(AstVisitor& visitor) override;

    void addStatement(std::unique_ptr<Statement> statement);
};

#endif //COMPILER_PROJECT_ASTNODE_H
