#include "Scope.h"
#include "Symbol.h"

Scope::Scope(Scope* parent)
    : _parent(parent) {
}

Symbol* Scope::addSymbol(Symbol& symbol) {
    auto [it, inserted] = _symbols.try_emplace(symbol.name, &symbol);
    return it->second;
}

Symbol* Scope::findSymbol(const std::string& name) {
    for (Scope* scope = this; scope; scope = scope->_parent) {
        if (auto it = scope->_symbols.find(name); it != scope->_symbols.end())
            return it->second;
    }

    return nullptr;
}

bool Scope::localSymbolExists(const std::string& name) const {
    return _symbols.contains(name);
}

Scope* Scope::getParent() const {
    return _parent;
}
