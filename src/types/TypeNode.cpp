//
// Created by Sagiv Marzini on 04/04/2026.
//

#include "TypeNode.h"

ScalarTypeNode::ScalarTypeNode(PrimitiveKind k) : scalar(k) {
}

TypeNode::Kind ScalarTypeNode::kind() const {
    return Kind::Scalar;
}

llvm::Type* ScalarTypeNode::toLLVMType(llvm::LLVMContext& ctx) const {
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

std::string ScalarTypeNode::toString() const {
    return typeToString(scalar);
}

ArrayTypeNode::ArrayTypeNode(TypeNode* base, int size) : base(base), size(size) {
}

TypeNode::Kind ArrayTypeNode::kind() const {
    return Kind::Array;
}

llvm::Type* ArrayTypeNode::toLLVMType(llvm::LLVMContext& ctx) const {
    return llvm::ArrayType::get(base->toLLVMType(ctx), size);
}

std::string ArrayTypeNode::toString() const {
    return (base ? base->toString() : "unknown") + "[" + std::to_string(size) + "]";
}
