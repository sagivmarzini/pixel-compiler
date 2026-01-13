//
// Created by Sagiv Marzini on 04/12/2025.
//

#ifndef COMPILER_PROJECT_SEMANTICVISITOR_H
#define COMPILER_PROJECT_SEMANTICVISITOR_H

#include "CompilerError.h"
#include "SemanticError.h"
#include "parse/AST/AstVisitor.h"
#include "SymbolTable.h"

class AstNode;
class SymbolPool;

class SemanticVisitor : public AstVisitor {
public:
    explicit SemanticVisitor(SymbolTable& symbolTable);

    virtual void run(AstNode& root) = 0;

protected:
    SymbolTable&               _symbolTable;
    std::vector<CompilerError> _errors;

    void enterScope() const;

    void exitScope() const;

    template<typename T>
    void logError(SemanticErrorType type, const AstNode& node, T&& contextData) {
        _errors.push_back(SemanticError(type, node, ErrorContext(std::forward<T>(contextData))));
    }

    // Overload for errors with no extra data
    void logError(SemanticErrorType type, const AstNode& node);
};


#endif //COMPILER_PROJECT_SEMANTICVISITOR_H
