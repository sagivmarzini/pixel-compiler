#ifndef COMPILER_PROJECT_TYPECHECKERVISITOR_H
#define COMPILER_PROJECT_TYPECHECKERVISITOR_H
#include "SemanticVisitor.h"
#include "lex/Token.h"


class TypeContext;
enum class SemanticErrorType;

class TypeCheckerVisitor : public SemanticVisitor {
public:
    using SemanticVisitor::SemanticVisitor; // inherit constructor

    void run(AST::AstNode& root) override;

    // Returns the bigger type of the two (float > int, int > short, etc.)
    static ScalarKind getPromotedType(ScalarKind t1, ScalarKind t2);

private:
    TypeNode* _currentFunctionReturnType = nullptr;
    bool _foundReturn = false;

    static ScalarKind kindOf(TypeNode* t);

    // Returns true if the type is an `int` or `float`
    static bool isNumeric(ScalarKind type);

    // Returns true if the types are both either numeric or strings (compared lexicographically)
    static bool areComparableTypes(ScalarKind leftType, ScalarKind rightType);

    static bool isString(ScalarKind type);

    static bool isBoolean(ScalarKind type);

    static bool isAssignableTo(ScalarKind assignedType, ScalarKind targetType);

    std::pair<ScalarKind, int> checkArrayLiteralType(const AST::ArrayLiteral& arr);

    bool checkArgCount(AST::FunctionCall& node, const Symbol& fn);

    void checkArgument(AST::FunctionCall& node,
                       const AST::FunctionCall::FunctionArgument& arg,
                       const AST::FunctionDeclaration::FunctionParameter& param,
                       const Symbol& fn);

    bool checkArgumentLabel(AST::FunctionCall& node,
                            const std::string& argName,
                            const AST::FunctionDeclaration::FunctionParameter& param,
                            const Symbol& calledFunction);

    ScalarKind checkBinaryOp(AST::BinaryExpression& node, ScalarKind left, ScalarKind right);

    ScalarKind checkArithmetic(AST::BinaryExpression& node, ScalarKind left, ScalarKind right);

    void visit(AST::Program& program) override;


    void visit(AST::FunctionDeclaration& node) override;

    void visit(AST::FunctionCall& node) override;


    void visit(AST::VariableDeclaration& node) override;

    void visit(AST::VariableAssignment& node) override;

    void visit(AST::ArrayAssignment& node) override;


    void visit(AST::IfStatement& node) override;

    void visit(AST::WhileLoop& node) override;

    void visit(AST::ForLoop& node) override;

    void visit(AST::RangeExpression& node) override;

    void visit(AST::Block& node) override;

    void visit(AST::ReturnStatement& node) override;


    void visit(AST::BinaryExpression& node) override;

    void visit(AST::UnaryExpression& node) override;

    void visit(AST::IncDecExpression& node) override;

    void visit(AST::ExpressionStatement& node) override;

    void visit(AST::ArrayIndex& node) override;


    void visit(AST::IntegerLiteralNode& node) override;

    void visit(AST::FloatLiteralNode& node) override;

    void visit(AST::StringLiteralNode& node) override;

    void visit(AST::BooleanLiteralNode& node) override;

    void visit(AST::VariableExpression& node) override;

    void visit(AST::ArrayLiteral& node) override;
};


#endif //COMPILER_PROJECT_TYPECHECKERVISITOR_H
