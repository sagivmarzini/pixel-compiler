#include "SymbolPool.h"

Symbol& SymbolPool::createSymbol(const std::string& name, Symbol::SymbolKind kind, TypeNode* type, Scope* scope,
                                 bool isConst) {
    _symbols.push_back(std::make_unique<Symbol>(name, kind, type, scope, isConst));
    return *_symbols.back().get();
}
