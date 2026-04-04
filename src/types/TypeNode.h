//
// Created by sagiv on 23/03/2026.
//

#ifndef PXL_LANG_TYPENODE_H
#define PXL_LANG_TYPENODE_H
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>

#include "lex/Token.h"

struct TypeNode {
    enum class Kind { Scalar, Array };


    virtual ~TypeNode() = default;

    [[nodiscard]] virtual Kind kind() const = 0;

    virtual llvm::Type* toLLVMType(llvm::LLVMContext& ctx) const = 0;

    virtual std::string toString() const = 0;
};

struct ScalarTypeNode : TypeNode {
    PrimitiveKind scalar;

    explicit ScalarTypeNode(PrimitiveKind k);

    [[nodiscard]] Kind kind() const override;

    llvm::Type* toLLVMType(llvm::LLVMContext& ctx) const override;

    std::string toString() const override;
};

struct ArrayTypeNode : TypeNode {
    TypeNode* base; // non-owning - base type also lives in TypeContext
    int       size;

    ArrayTypeNode(TypeNode* base, int size);

    [[nodiscard]] Kind kind() const override;

    llvm::Type* toLLVMType(llvm::LLVMContext& ctx) const override;

    std::string toString() const override;
};

#endif //PXL_LANG_TYPENODE_H
