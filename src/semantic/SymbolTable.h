#ifndef COMPILER_PROJECT_SYMBOLTABLE_H
#define COMPILER_PROJECT_SYMBOLTABLE_H
#include <memory>
#include <vector>

#include "lex/Token.h"
#include "Symbol.h"
#include "Scope.h"

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
    void declareBuiltins();

private:
    Scope*                               _currentScope;
    SymbolPool&                          _pool;
    std::vector<std::unique_ptr<Scope> > _scopes;
    std::unordered_map<std::string, std::vector<FunctionDeclaration::FunctionParameter>> _builtinFunctions = {
        {"print", {{"str", Type::String, true}}},

        {"setCanvas" , {{"width", Type::Int},
                                {"height", Type::Int}}},

        {"background", {{"r", Type::Int},
                            {"g", Type::Int},
                            {"b", Type::Int}}},

        {"setColor", {{"r", Type::Int},
                            {"g", Type::Int},
                            {"b", Type::Int}}},

        {"rect", {{"x", Type::Int},
                            {"y", Type::Int},
                            {"width", Type::Int},
                            {"height", Type::Int}}},

        {"circle", {{"x", Type::Int},
                            {"y", Type::Int},
                            {"radius", Type::Int}}}
    };
};


#endif //COMPILER_PROJECT_SYMBOLTABLE_H
