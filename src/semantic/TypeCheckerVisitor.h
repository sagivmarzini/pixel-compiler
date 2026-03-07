#ifndef COMPILER_PROJECT_TYPECHECKERVISITOR_H
#define COMPILER_PROJECT_TYPECHECKERVISITOR_H
#include "SemanticError.h"
#include "SemanticVisitor.h"
#include "lex/Token.h"


enum class SemanticErrorType;

class TypeCheckerVisitor : public SemanticVisitor {
public:
    using SemanticVisitor::SemanticVisitor; // inherit constructor

    void run(AST::AstNode& root) override;

    // Returns the bigger type of the two (float > int, int > short, etc.)
    static Type getPromotedType(Type leftType, Type rightType);

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

    static bool isReturnTypeCompatible(Type functionType, Type returnedType);

    void visit(AST::Program& program) override;

    void visit(AST::FunctionDeclaration& node) override;

    void visit(AST::ExpressionStatement& node) override;

    void visit(AST::IfStatement& node) override;

    void visit(AST::WhileLoop& node) override;

    void visit(AST::ForLoop& node) override;

    void visit(AST::RangeExpression& node) override;

    void visit(AST::Block& node) override;

    void visit(AST::VariableDeclaration& node) override;

    void visit(AST::FunctionCall& node) override;

    void visit(AST::BinaryExpression& node) override;

    void visit(AST::UnaryExpression& node) override;

    void visit(AST::IncDecExpression& node) override;

    void visit(AST::VariableAssignment& node) override;


    void visit(AST::ReturnStatement& node) override;

    void visit(AST::IntegerLiteralNode& node) override;

    void visit(AST::FloatLiteralNode& node) override;

    void visit(AST::StringLiteralNode& node) override;

    void visit(AST::BooleanLiteralNode& node) override;

    void visit(AST::VariableExpression& node) override;
};


#endif //COMPILER_PROJECT_TYPECHECKERVISITOR_H
