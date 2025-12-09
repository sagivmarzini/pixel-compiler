//
// Created by Sagiv Marzini on 04/12/2025.
//

#ifndef COMPILER_PROJECT_SCOPE_H
#define COMPILER_PROJECT_SCOPE_H
#include <unordered_map>

#include "Symbol.h"


class Scope {
public:
    explicit Scope(Scope* parent = nullptr);


    bool declare(Symbol& symbol);

    Symbol* lookup(const std::string& name);

    [[nodiscard]] Scope* getParent() const;

private:
    Scope* _parent;
    std::unordered_map<std::string, Symbol*> symbols;
};


#endif //COMPILER_PROJECT_SCOPE_H
