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

    bool localSymbolExists(const std::string& name) const;

    [[nodiscard]] Scope* getParent() const;

private:
    Scope*                                   _parent;
    std::unordered_map<std::string, Symbol*> _symbols;
};


#endif //COMPILER_PROJECT_SCOPE_H
