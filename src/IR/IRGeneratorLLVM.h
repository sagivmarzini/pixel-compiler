//
// Created by Sagiv Marzini on 14/01/2026.
//

#ifndef COMPILER_PROJECT_IRGENERATORLLVM_H
#define COMPILER_PROJECT_IRGENERATORLLVM_H

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include "parse/AST/Statement.h"
#include "semantic/Scope.h"
#include "semantic/functions/FunctionRegistry.h"

class GlobalRegistry;
class SymbolTable;

class IRGeneratorLLVM {
public:
    explicit IRGeneratorLLVM(const FunctionRegistry& registry, const GlobalRegistry& globalRegistry);

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

    llvm::Value* visit(const AST::ArrayAssignment& node);


    llvm::Value* visit(const AST::ExpressionStatement& node);

    llvm::Value* visit(const AST::ReturnStatement& node);


    llvm::Value* visit(const AST::BinaryExpression& node);

    llvm::Value* visit(const AST::UnaryExpression& node);

    llvm::Value* visit(const AST::IncDecExpression& node);

    llvm::Value* visit(const AST::ArrayIndex& node);


    llvm::Value* visit(const AST::VariableExpression& node) const;

    llvm::Value* visit(const AST::IntegerLiteralNode& node) const;

    llvm::Value* visit(const AST::StringLiteralNode& node);

    llvm::Value* visit(const AST::BooleanLiteralNode& node) const;

    llvm::Value* visit(const AST::FloatLiteralNode& node) const;

    llvm::Value* visit(const AST::ArrayLiteral& node);

private:
    // Owns the core LLVM data structures, such as the type and constant tables.
    std::unique_ptr<llvm::LLVMContext> _context;
    // A container for all functions and global variables in a single translation unit.
    std::unique_ptr<llvm::Module> _module;
    // A helper object that makes it easy to generate LLVM instructions (IR).
    std::unique_ptr<llvm::IRBuilder<> > _builder;
    // Keeps track of which values (like variables) are defined and their alloca address
    std::unordered_map<Symbol *, llvm::Value *> _namedValues;

    const FunctionRegistry& _functionRegistry;
    const GlobalRegistry& _globalRegistry;
    std::unordered_map<Operator, std::string> _stringOperatorLowering = {
        {Operator::Plus, "pxl_concat_string"},
        {Operator::Equal, "pxl_string_equals"},
        {Operator::NotEqual, "pxl_string_not_equals"},
        {Operator::GreaterThan, "pxl_string_greater"},
        {Operator::LessThan, "pxl_string_smaller"},
        {Operator::GreaterEqual, "pxl_string_greater_equals"},
        {Operator::LessEqual, "pxl_string_smaller_equals"},

        {Operator::Assignment, "pxl_copy"}
    };

    [[nodiscard]] llvm::Type* getLlvmType(const PrimitiveKind& type) const;

    [[nodiscard]] static PrimitiveKind llvmTypeToCompilerType(const llvm::Type& type);

    // Promotes two LLVM values to have matching types, if necessary.
    void promoteToMatch(llvm::Value*& lhs, llvm::Value*& rhs) const;

    [[nodiscard]] llvm::Value* castToType(llvm::Value* value, const llvm::Type* expectedType) const;

    llvm::Value* initLocalVariable(llvm::Type* type, const std::string& name,
                                   llvm::Value* value) const;

    llvm::Function* getOrDeclareBuiltinFunction(const std::string& name) const;

    llvm::Value* createPxlStringFromLiteral(const std::string& value) const;

    void createMainFunction() const;
};


#endif //COMPILER_PROJECT_IRGENERATORLLVM_H
