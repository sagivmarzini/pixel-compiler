//
// Created by sagivm on 11/12/2025.
//

#ifndef COMPILER_PROJECT_SYMBOLTABLE_H
#define COMPILER_PROJECT_SYMBOLTABLE_H
#include <memory>
#include <vector>

#include "lex/Token.h"
#include "Symbol.h"
#include "Scope.h"
#include "parse/AST/Statement.h"

class SymbolPool;

class SymbolTable {
public:
    explicit SymbolTable(SymbolPool& symbolPool);

    void enterScope();

    void exitScope();

    [[nodiscard]] Scope* getCurrentScope() const;

    void setCurrentScope(Scope* scope);

    [[nodiscard]] Symbol* lookup(const std::string& name) const;

    [[nodiscard]] Symbol* declare(const std::string& name, Symbol::SymbolKind kind, Type type,
                                  bool               isConst = false) const;

private:
    Scope*                               _currentScope = nullptr;
    SymbolPool&                          _pool;
    std::vector<std::unique_ptr<Scope> > _scopes;
};


#endif //COMPILER_PROJECT_SYMBOLTABLE_H
