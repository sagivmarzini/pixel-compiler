//
// Created by sagivm on 10/12/2025.
//

#ifndef COMPILER_PROJECT_TYPECHECKERVISITOR_H
#define COMPILER_PROJECT_TYPECHECKERVISITOR_H
#include "SemanticError.h"
#include "SemanticVisitor.h"
#include "lex/Token.h"


enum class SemanticErrorType;

class TypeCheckerVisitor : public SemanticVisitor {
public:
    using SemanticVisitor::SemanticVisitor; // inherit constructor

    void run(AstNode& root) override;

private:
    Type _currentFunctionReturnType = Type::Unspecified;
    bool _foundReturn               = false;

    // Returns true if the type is an `int` or `float`
    static bool isNumeric(Type type);

    // Returns true if the types are both either numeric or strings (compared lexicographically)
    static bool areComparableTypes(Type leftType, Type rightType);

    static bool isString(Type type);

    static bool isBoolean(Type type);

    static bool isAssignableTo(Type assignedType, Type variableType);

    // Returns the bigger type of the two (float > int, int > short, etc.)
    static Type getPromotedType(Type leftType, Type rightType);

    void visit(Program& program) override;

    void visit(FunctionDeclaration& node) override;

    void visit(ExpressionStatement& node) override;

    void visit(IfStatement& node) override;

    void visit(WhileLoop& node) override;

    void visit(ForLoop& node) override;

    void visit(RangeExpression& node) override;

    void visit(Block& node) override;

    void visit(VariableDeclaration& node) override;

    void visit(FunctionCall& node) override;

    void visit(BinaryExpression& node) override;

    void visit(UnaryExpression& node) override;

    void visit(VariableAssignment& node) override;

    void visit(ReturnStatement& node) override;

    void visit(IntegerLiteralNode& node) override;

    void visit(FloatLiteralNode& node) override;

    void visit(StringLiteralNode& node) override;

    void visit(BooleanLiteralNode& node) override;

    void visit(IdentifierNode& node) override;
};


#endif //COMPILER_PROJECT_TYPECHECKERVISITOR_H
