//
// Created by Sagiv Marzini on 04/12/2025.
//

#include "SemanticVisitor.h"
#include "SymbolTable.h"

SemanticVisitor::SemanticVisitor(SymbolTable& symbolTable)
    : _symbolTable(symbolTable) {
}

void SemanticVisitor::enterScope() const {
    _symbolTable.enterScope();
}

void SemanticVisitor::exitScope() const {
    _symbolTable.exitScope();
}
