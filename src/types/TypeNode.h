//
// Created by sagiv on 23/03/2026.
//

#ifndef PXL_LANG_TYPENODE_H
#define PXL_LANG_TYPENODE_H
#include <llvm/IR/Type.h>

#include "lex/Token.h"

struct TypeNode {
    enum class Kind { Scalar, Array };


    virtual ~TypeNode() = default;

    virtual Kind kind() const = 0;

    virtual llvm::Type* toLLVMType(llvm::LLVMContext& ctx) const = 0;
};

struct ScalarTypeNode : TypeNode {
    ScalarKind scalar;

    explicit ScalarTypeNode(ScalarKind k) : scalar(k) {
    }

    Kind kind() const override { return Kind::Scalar; }

    llvm::Type* toLLVMType(llvm::LLVMContext& ctx) const override;
};

struct ArrayTypeNode : TypeNode {
    TypeNode* base; // non-owning - base type also lives in TypeContext
    int size;

    ArrayTypeNode(TypeNode* base, int size) : base(base), size(size) {
    }

    Kind kind() const override { return Kind::Array; }

    llvm::Type* toLLVMType(llvm::LLVMContext& ctx) const override;
};

#endif //PXL_LANG_TYPENODE_H
