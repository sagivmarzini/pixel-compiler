//
// Created by Sagiv Marzini on 14/01/2026.
//

#ifndef COMPILER_PROJECT_IRGENERATORLLVM_H
#define COMPILER_PROJECT_IRGENERATORLLVM_H

#include <map>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include "parse/AST/Statement.h"
#include "semantic/Scope.h"

class SymbolTable;
struct ForLoop;
struct ExpressionStatement;
struct FloatLiteralNode;
struct RangeExpression;
struct IntegerLiteralNode;
struct StringLiteralNode;
struct BooleanLiteralNode;
struct BinaryExpression;
struct UnaryExpression;
struct VariableExpression;
struct FunctionCall;
struct VariableDeclaration;
struct VariableAssignment;
struct ReturnStatement;
struct Block;
struct WhileLoop;
struct IfStatement;
struct FunctionDeclaration;
struct Program;


class IRGeneratorLLVM {
public:
    explicit IRGeneratorLLVM();

    void print() const;

    llvm::Value* visit(const Program& program);

    llvm::Value* visit(const FunctionDeclaration& node);

    llvm::Value* visit(const FunctionCall& node);


    llvm::Value* visit(const IfStatement& node);

    llvm::Value* visit(const WhileLoop& node);

    llvm::Value* visit(const ForLoop& node);

    llvm::Value* visit(const RangeExpression& node);

    llvm::Value* visit(const Block& node);


    llvm::Value* visit(const VariableDeclaration& node);

    llvm::Value* visit(const VariableAssignment& node);


    llvm::Value* visit(const ExpressionStatement& node);

    llvm::Value* visit(const ReturnStatement& node);


    llvm::Value* visit(const BinaryExpression& node);

    llvm::Value* visit(const UnaryExpression& node);

    llvm::Value* visit(const IncDecExpression& node);


    llvm::Value* visit(const VariableExpression& node) const;

    llvm::Value* visit(const IntegerLiteralNode& node) const;

    llvm::Value* visit(const StringLiteralNode& node) const;

    llvm::Value* visit(const BooleanLiteralNode& node) const;

    llvm::Value* visit(const FloatLiteralNode& node) const;

private:
    // Owns the core LLVM data structures, such as the type and constant tables.
    std::unique_ptr<llvm::LLVMContext> _context;
    // A container for all functions and global variables in a single translation unit.
    std::unique_ptr<llvm::Module> _module;
    // A helper object that makes it easy to generate LLVM instructions (IR).
    std::unique_ptr<llvm::IRBuilder<> > _builder;
    // Keeps track of which values (like variables) are defined in the current scope.
    std::unordered_map<Symbol*, llvm::Value*> _namedValues;

    [[nodiscard]] llvm::Type* compilerTypeToLlvmType(const Type& type) const;

    [[nodiscard]] static Type llvmTypeToCompilerType(const llvm::Type& type);

    // Promotes two LLVM values to have matching types, if necessary.
    void promoteToMatch(llvm::Value*& lhs, llvm::Value*& rhs) const;

    llvm::Value* initLocalVariable(llvm::Type*  type, const std::string& name,
                                   llvm::Value* value) const;
};


#endif //COMPILER_PROJECT_IRGENERATORLLVM_H
