//
// Created by Sagiv Marzini on 05/12/2025.
//

#include "SymbolPool.h"

Symbol& SymbolPool::createSymbol(const std::string& name, Symbol::SymbolKind kind, Type type, Scope* scope) {
    _pool.push_back(std::make_unique<Symbol>(name, kind, type, scope));
    return *_pool.back().get();
}
