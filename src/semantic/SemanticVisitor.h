//
// Created by Sagiv Marzini on 04/12/2025.
//

#ifndef COMPILER_PROJECT_SEMANTICVISITOR_H
#define COMPILER_PROJECT_SEMANTICVISITOR_H
#include <memory>
#include <vector>

#include "parse/AST/AstVisitor.h"
#include "semantic/Scope.h"

class AstNode;
class SymbolPool;

class SemanticVisitor : public AstVisitor {
public:
    explicit SemanticVisitor(SymbolPool& symbolPool);

    virtual void run(AstNode& root) = 0;

protected:
    SymbolPool& _pool;
    Scope* _currentScope;
    std::vector<std::unique_ptr<Scope> > _scopes;


    void pushScope();

    void popScope();
};


#endif //COMPILER_PROJECT_SEMANTICVISITOR_H
