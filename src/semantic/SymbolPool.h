//
// Created by Sagiv Marzini on 05/12/2025.
//

#ifndef COMPILER_PROJECT_SYMBOLTABLE_H
#define COMPILER_PROJECT_SYMBOLTABLE_H
#include <memory>
#include <vector>

#include "Symbol.h"

class Scope;
enum class Type;

class SymbolPool {
public:
    Symbol& createSymbol(const std::string& name, Symbol::SymbolKind kind, Type type, Scope* scope);

private:
    std::vector<std::unique_ptr<Symbol> > _pool;
};


#endif //COMPILER_PROJECT_SYMBOLTABLE_H
