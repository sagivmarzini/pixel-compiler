#ifndef COMPILER_PROJECT_SYMBOLPOOL_H
#define COMPILER_PROJECT_SYMBOLPOOL_H
#include <memory>
#include <vector>

#include "Symbol.h"

class Scope;
enum class ScalarKind;

class SymbolPool {
public:
    Symbol &createSymbol(const std::string &name, Symbol::SymbolKind kind, ScalarKind type, Scope *scope, bool isConst);

private:
    std::vector<std::unique_ptr<Symbol> > _symbols;
};


#endif //COMPILER_PROJECT_SYMBOLPOOL_H
