//
// Created by Sagiv Marzini on 04/12/2025.
//

#include "SemanticVisitor.h"

SemanticVisitor::SemanticVisitor(SymbolPool& symbolPool)
    : _pool(symbolPool), _currentScope(nullptr) {
}

void SemanticVisitor::pushScope() {
    auto scope = std::make_unique<Scope>(_currentScope);
    _scopes.push_back(std::move(scope));

    _currentScope = _scopes.back().get();
}

void SemanticVisitor::popScope() {
    _currentScope = _currentScope->getParent();
}
