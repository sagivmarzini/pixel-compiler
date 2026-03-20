#include "SymbolTable.h"

#include "SymbolPool.h"
#include "Scope.h"
#include "functions/FunctionInfo.h"
#include "globals/GlobalEntry.h"

SymbolTable::SymbolTable(SymbolPool& symbolPool) : _currentScope(nullptr), _pool(symbolPool) {
    enterScope();
}

void SymbolTable::enterScope() {
    auto scope = std::make_unique<Scope>(_currentScope);
    _scopes.push_back(std::move(scope));

    _currentScope = _scopes.back().get();
}

void SymbolTable::exitScope() {
    _currentScope = _currentScope->getParent();
}

Scope* SymbolTable::getCurrentScope() const {
    return _currentScope;
}

void SymbolTable::setCurrentScope(Scope* scope) {
    _currentScope = scope;
}

Symbol* SymbolTable::lookup(const std::string& name) const {
    if (_currentScope) return _currentScope->findSymbol(name);

    return nullptr;
}

Symbol* SymbolTable::declare(const std::string& name, Symbol::SymbolKind kind, Type type, bool isConst) const {
    if (_currentScope->localSymbolExists(name)) return nullptr;
    Symbol& sym = _pool.createSymbol(name, kind, type, _currentScope, isConst);
    return _currentScope->addSymbol(sym);
}

// Declare the language's built-in functions in the global scope
void SymbolTable::declareBuiltinFunctions(
    const std::unordered_map<std::string, FunctionInfo>& declarations)
const {
    for (auto& [name, signature]: declarations) {
        auto& symbol = _pool.createSymbol(name, Symbol::SymbolKind::Function, signature.returnType, _currentScope,
                                          false);
        symbol.params = signature.params;
        _currentScope->addSymbol(symbol);
    }
}

void SymbolTable::declareBuiltinGlobals(const std::unordered_map<std::string, GlobalEntry>& pairs) const {
    for (const auto& [name, global]: pairs) {
        auto& symbol = _pool.createSymbol(name, Symbol::SymbolKind::Constant, global.type, _currentScope, true);
        _currentScope->addSymbol(symbol);
    }
}
