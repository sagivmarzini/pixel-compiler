//
// Created by sagiv on 23/03/2026.
//

#ifndef PXL_LANG_TYPECONTEXT_H
#define PXL_LANG_TYPECONTEXT_H
#include <map>
#include <memory>
#include <utility>

#include "TypeNode.h"


struct TypeNode;

class TypeContext {
public:
    TypeNode* getInt() const { return _intType.get(); }
    TypeNode* getFloat() const { return _floatType.get(); }
    TypeNode* getBool() const { return _boolType.get(); }
    TypeNode* getVoid() const { return _voidType.get(); }
    TypeNode* getString() const { return _stringType.get(); }
    TypeNode* getPointer() const { return _pointerType.get(); }

    TypeNode* getArray(TypeNode* base, int size);

    TypeNode* get(ScalarKind kind) const;

private:
    std::unique_ptr<ScalarTypeNode> _intType;
    std::unique_ptr<ScalarTypeNode> _floatType;
    std::unique_ptr<ScalarTypeNode> _boolType;
    std::unique_ptr<ScalarTypeNode> _voidType;
    std::unique_ptr<ScalarTypeNode> _stringType;
    std::unique_ptr<ScalarTypeNode> _pointerType;

    // Array types are deduplicated by (base, size)
    std::map<std::pair<TypeNode *, int>, std::unique_ptr<ArrayTypeNode> > _arrayTypes;
};


#endif //PXL_LANG_TYPECONTEXT_H
