//
// Created by Sagiv Marzini on 04/12/2025.
//

#ifndef COMPILER_PROJECT_SCOPE_H
#define COMPILER_PROJECT_SCOPE_H

#include <unordered_map>
#include <string>

class Symbol;

class Scope {
public:
    explicit Scope(Scope* parent = nullptr);

    Symbol* addSymbol(Symbol& symbol);

    Symbol* findSymbol(const std::string& name);

    [[nodiscard]] Scope* getParent() const;

private:
    Scope*                                   _parent;
    std::unordered_map<std::string, Symbol*> symbols;
};


#endif //COMPILER_PROJECT_SCOPE_H
