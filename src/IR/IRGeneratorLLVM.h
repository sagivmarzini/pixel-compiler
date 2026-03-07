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

struct FunctionSignature;
class SymbolTable;

class IRGeneratorLLVM {
public:
    explicit IRGeneratorLLVM();

    explicit IRGeneratorLLVM(
        std::unordered_map<std::string, FunctionSignature> builtInFunctions);

    void print() const;

    void createExecutable(const std::string& outputPath) const;

    llvm::Value* visit(const AST::Program& program);

    llvm::Value* visit(const AST::FunctionDeclaration& node);

    llvm::Value* visit(const AST::FunctionCall& node);


    llvm::Value* visit(const AST::IfStatement& node);

    llvm::Value* visit(const AST::WhileLoop& node);

    llvm::Value* visit(const AST::ForLoop& node);

    llvm::Value* visit(const AST::RangeExpression& node);

    llvm::Value* visit(const AST::Block& node);


    llvm::Value* visit(const AST::VariableDeclaration& node);

    llvm::Value* visit(const AST::VariableAssignment& node);


    llvm::Value* visit(const AST::ExpressionStatement& node);

    llvm::Value* visit(const AST::ReturnStatement& node);


    llvm::Value* visit(const AST::BinaryExpression& node);

    llvm::Value* visit(const AST::UnaryExpression& node);

    llvm::Value* visit(const AST::IncDecExpression& node);


    llvm::Value* visit(const AST::VariableExpression& node) const;

    llvm::Value* visit(const AST::IntegerLiteralNode& node) const;

    llvm::Value* visit(const AST::StringLiteralNode& node) const;

    llvm::Value* visit(const AST::BooleanLiteralNode& node) const;

    llvm::Value* visit(const AST::FloatLiteralNode& node) const;

private:
    // Owns the core LLVM data structures, such as the type and constant tables.
    std::unique_ptr<llvm::LLVMContext> _context;
    // A container for all functions and global variables in a single translation unit.
    std::unique_ptr<llvm::Module> _module;
    // A helper object that makes it easy to generate LLVM instructions (IR).
    std::unique_ptr<llvm::IRBuilder<> > _builder;
    // Keeps track of which values (like variables) are defined in the current scope.
    std::unordered_map<Symbol*, llvm::Value*> _namedValues;

    std::unordered_map<std::string, FunctionSignature> _builtinFunctions{};

    [[nodiscard]] llvm::Type* compilerTypeToLlvmType(const Type& type) const;

    [[nodiscard]] static Type llvmTypeToCompilerType(const llvm::Type& type);

    // Promotes two LLVM values to have matching types, if necessary.
    void promoteToMatch(llvm::Value*& lhs, llvm::Value*& rhs) const;

    [[nodiscard]] llvm::Value* castToExpectedType(llvm::Value* value, const llvm::Type* expectedType) const;

    llvm::Value* initLocalVariable(llvm::Type*  type, const std::string& name,
                                   llvm::Value* value) const;

    llvm::Function* getOrDeclareBuiltin(const std::string& name);
};


#endif //COMPILER_PROJECT_IRGENERATORLLVM_H
