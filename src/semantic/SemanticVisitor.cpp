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

void SemanticVisitor::logError(SemanticErrorType type, const AstNode& node) {
    _errors.push_back(SemanticError(type, node, std::monostate{}));
}
