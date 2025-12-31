//
// Created by Sagiv Marzini on 04/12/2025.
//

#ifndef COMPILER_PROJECT_SEMANTICVISITOR_H
#define COMPILER_PROJECT_SEMANTICVISITOR_H

#include "parse/AST/AstVisitor.h"
#include "SymbolTable.h"

class AstNode;
class SymbolPool;

class SemanticVisitor : public AstVisitor {
public:
    explicit SemanticVisitor(SymbolTable& symbolTable);

    virtual void run(AstNode& root) = 0;

protected:
    SymbolTable& _symbolTable;

    void enterScope() const;

    void exitScope() const;
};


#endif //COMPILER_PROJECT_SEMANTICVISITOR_H
