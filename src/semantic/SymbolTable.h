#ifndef COMPILER_PROJECT_SYMBOLTABLE_H
#define COMPILER_PROJECT_SYMBOLTABLE_H
#include <memory>
#include <vector>

#include "lex/Token.h"
#include "Symbol.h"
#include "Scope.h"

struct GlobalEntry;
struct FunctionInfo;
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
                                  bool isConst = false) const;

    void declareBuiltinFunctions(
        const std::unordered_map<std::string, FunctionInfo>& declarations)
    const;

    void declareBuiltinGlobals(const std::unordered_map<std::string, GlobalEntry>& pairs) const;

private:
    Scope* _currentScope;
    SymbolPool& _pool;
    std::vector<std::unique_ptr<Scope> > _scopes;
};


#endif //COMPILER_PROJECT_SYMBOLTABLE_H
