//
// Created by sagivm on 11/12/2025.
//

#include "SymbolTable.h"

#include "SymbolPool.h"
#include "Scope.h"

SymbolTable::SymbolTable(SymbolPool &symbolPool) : _pool(symbolPool) {
}

void SymbolTable::enterScope() {
    auto scope = std::make_unique<Scope>(_currentScope);
    _scopes.push_back(std::move(scope));

    _currentScope = _scopes.back().get();
}

void SymbolTable::exitScope() {
    _currentScope = _currentScope->getParent();
}

Scope *SymbolTable::getCurrentScope() const {
    return _currentScope;
}

void SymbolTable::setCurrentScope(Scope *scope) {
    _currentScope = scope;
}

Symbol *SymbolTable::lookup(const std::string &name) const {
    if (_currentScope) return _currentScope->findSymbol(name);

    return nullptr;
}

Symbol *SymbolTable::declare(const std::string &name, Symbol::SymbolKind kind, Type type, bool isConst) const {
    if (lookup(name)) return nullptr;
    Symbol &sym = _pool.createSymbol(name, kind, type, _currentScope, isConst);
    return _currentScope->addSymbol(sym);
}
