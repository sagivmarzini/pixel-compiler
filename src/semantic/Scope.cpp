//
// Created by Sagiv Marzini on 04/12/2025.
//

#include "Scope.h"
#include "Symbol.h"

Scope::Scope(Scope* parent)
    : _parent(parent) {
}

Symbol* Scope::addSymbol(Symbol& symbol) {
    auto [it, inserted] = symbols.try_emplace(symbol.name, &symbol);
    return it->second;
}

Symbol* Scope::findSymbol(const std::string& name) {
    for (Scope* scope = this; scope; scope = scope->_parent) {
        if (auto it = scope->symbols.find(name); it != scope->symbols.end())
            return it->second;
    }

    return nullptr;
}

Scope* Scope::getParent() const {
    return _parent;
}
