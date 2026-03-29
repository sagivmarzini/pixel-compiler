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

    virtual Kind kind() const = 0;

    virtual llvm::Type* toLLVMType(llvm::LLVMContext& ctx) const = 0;
};

struct ScalarTypeNode : TypeNode {
    PrimitiveKind scalar;

    explicit ScalarTypeNode(PrimitiveKind k) : scalar(k) {
    }

    Kind kind() const override { return Kind::Scalar; }

    llvm::Type* toLLVMType(llvm::LLVMContext& ctx) const override {
        switch (scalar) {
            case PrimitiveKind::Int:
                return llvm::Type::getInt32Ty(ctx);
            case PrimitiveKind::Float:
                return llvm::Type::getFloatTy(ctx);
            case PrimitiveKind::Bool:
                return llvm::Type::getInt1Ty(ctx);
            case PrimitiveKind::Void:
                return llvm::Type::getVoidTy(ctx);
            case PrimitiveKind::Pointer:
            case PrimitiveKind::String:
                return llvm::PointerType::get(ctx, 0);

            default:
                throw std::runtime_error("Internal error: unsupported scalar type: " + typeToString(scalar));
        }
    }
};

struct ArrayTypeNode : TypeNode {
    TypeNode* base; // non-owning - base type also lives in TypeContext
    int       size;

    ArrayTypeNode(TypeNode* base, int size) : base(base), size(size) {
    }

    Kind kind() const override { return Kind::Array; }

    llvm::Type* toLLVMType(llvm::LLVMContext& ctx) const override {
        return llvm::ArrayType::get(base->toLLVMType(ctx), size);
    }
};

#endif //PXL_LANG_TYPENODE_H
