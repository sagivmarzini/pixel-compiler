//
// Created by Sagiv Marzini on 04/12/2025.
//

#include "Scope.h"

Scope::Scope(Scope* parent)
    : _parent(parent) {
}

bool Scope::declare(Symbol& symbol) {
    auto [it, inserted] = symbols.try_emplace(symbol.name, &symbol);
    return inserted;
}

Symbol* Scope::lookup(const std::string& name) {
    for (Scope* scope = this; scope; scope = scope->_parent) {
        if (auto it = scope->symbols.find(name); it != scope->symbols.end())
            return it->second;
    }

    return nullptr;
}

Scope* Scope::getParent() const {
    return _parent;
}
