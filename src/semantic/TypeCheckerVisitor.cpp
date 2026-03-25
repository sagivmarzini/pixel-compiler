#include "TypeCheckerVisitor.h"

#include <format>
#include <set>


#include "CompilerException.h"
#include "parse/AST/Expression.h"
#include "parse/AST/Statement.h"
#include "types/TypeContext.h"


void TypeCheckerVisitor::run(AST::AstNode& root) {
    root.accept(*this);

    if (!_errors.empty())
        throw CompilerException(_errors);
}

ScalarKind TypeCheckerVisitor::getPromotedType(ScalarKind t1, ScalarKind t2) {
    if (t1 == t2) return t1;

    if (isNumeric(t1) && isNumeric(t2)) {
        if (t1 == ScalarKind::Float || t2 == ScalarKind::Float) return ScalarKind::Float;
        return ScalarKind::Int; // Default numeric promotion
    }

    return ScalarKind::Error; // Mismatched types (e.g., Int and String)
}

ScalarKind TypeCheckerVisitor::kindOf(TypeNode* t) {
    if (!t) return ScalarKind::Unspecified;
    if (auto* s = dynamic_cast<ScalarTypeNode *>(t)) return s->scalar;
    if (dynamic_cast<ArrayTypeNode *>(t)) return ScalarKind::Error; // arrays aren't scalar
    return ScalarKind::Error;
}

bool TypeCheckerVisitor::isNumeric(ScalarKind type) {
    return type == ScalarKind::Int || type == ScalarKind::Float;
}

bool TypeCheckerVisitor::areComparableTypes(ScalarKind leftType, ScalarKind rightType) {
    return isNumeric(leftType) && isNumeric(rightType) || isString(leftType) && isString(rightType);
}

bool TypeCheckerVisitor::isString(ScalarKind type) {
    return type == ScalarKind::String;
}

bool TypeCheckerVisitor::isBoolean(ScalarKind type) {
    return type == ScalarKind::Bool;
}

bool TypeCheckerVisitor::isAssignableTo(ScalarKind assignedType, ScalarKind targetType) {
    return assignedType == targetType || (assignedType == ScalarKind::Int && targetType == ScalarKind::Float);
}

std::pair<ScalarKind, int> TypeCheckerVisitor::checkArrayLiteralType(const AST::ArrayLiteral& arr) {
    if (arr.elements.empty()) return {ScalarKind::Unspecified, 0};

    arr.elements[0]->accept(*this);

    ScalarKind common = kindOf(arr.elements[0]->type);
    const int size = arr.elements.size();
    for (size_t i = 1; i < size; ++i) {
        arr.elements[i]->accept(*this);
        common = getPromotedType(common, kindOf(arr.elements[i]->type));

        if (common == ScalarKind::Error) return {ScalarKind::Error, size};
    }

    return {common, size};
}

bool TypeCheckerVisitor::checkArgCount(AST::FunctionCall& node, const Symbol& fn) {
    const bool isVariadic = node.functionName == "print";
    if (!isVariadic && node.arguments.size() != fn.params.size()) {
        logError(SemanticErrorType::ArgumentCountMismatch, node,
                 ParamMismatchData(node.functionName, fn.params.size(), node.arguments.size()));
        return false;
    }
    return true;
}

void TypeCheckerVisitor::checkArgument(AST::FunctionCall& node,
                                       const AST::FunctionCall::FunctionArgument& arg,
                                       const AST::FunctionDeclaration::FunctionParameter& param,
                                       const Symbol& fn) {
    if (arg.name.has_value()) {
        if (!checkArgumentLabel(node, arg.name.value(), param, fn)) return;
    } else if (!param.isImplicit) {
        logError(SemanticErrorType::MissingArgumentLabel, node, param.name);
        return;
    }

    arg.value->accept(*this);
    if (!isAssignableTo(kindOf(arg.value->type), kindOf(param.type))) {
        logError(SemanticErrorType::ArgumentTypeMismatch, node,
                 TypeMismatchData(kindOf(param.type), kindOf(arg.value->type)));
    }
}

bool TypeCheckerVisitor::checkArgumentLabel(AST::FunctionCall& node, const std::string& argName,
                                            const AST::FunctionDeclaration::FunctionParameter& param,
                                            const Symbol& calledFunction) {
    if (param.name == argName) return true;

    if (calledFunction.getParameterByName(argName))
        logError(SemanticErrorType::InvalidArgumentPosition, node,
                 ArgumentPositionData(node.arguments, calledFunction.params));
    else
        logError(SemanticErrorType::UndefinedArgument, node, argName);

    return false;
}

ScalarKind TypeCheckerVisitor::checkBinaryOp(AST::BinaryExpression& node, ScalarKind left, ScalarKind right) {
    switch (node.op) {
        case Operator::Plus:
        case Operator::Minus:
        case Operator::Star:
        case Operator::Slash:
            return checkArithmetic(node, left, right);

        case Operator::Equal:
        case Operator::NotEqual:
            if (areComparableTypes(left, right) || (isBoolean(left) && isBoolean(right)))
                return ScalarKind::Bool;
            break;

        case Operator::LessThan:
        case Operator::GreaterThan:
        case Operator::LessEqual:
        case Operator::GreaterEqual:
            if (areComparableTypes(left, right))
                return ScalarKind::Bool;
            break;

        case Operator::LogicalAnd:
        case Operator::LogicalOr:
            if (isBoolean(left) && isBoolean(right))
                return ScalarKind::Bool;
            break;

        default:
            throw std::runtime_error(
                std::format("Internal error: Unhandled binary operator {}", operatorToString(node.op)));
    }

    logError(SemanticErrorType::TypeMismatch, node, OperatorData(node.op, left, right));
    return ScalarKind::Error;
}

ScalarKind TypeCheckerVisitor::checkArithmetic(AST::BinaryExpression& node, ScalarKind left, ScalarKind right) {
    if (isNumeric(left) && isNumeric(right)) {
        const ScalarKind promoted = getPromotedType(left, right);
        if (promoted == ScalarKind::Error)
            logError(SemanticErrorType::TypeMismatch, node, OperatorData(node.op, left, right));
        return promoted;
    }

    if (node.op == Operator::Plus && (isString(left) || isString(right))) {
        const ScalarKind other = isString(left) ? right : left;
        if (isString(other) || isNumeric(other))
            return ScalarKind::String;
    }

    logError(SemanticErrorType::TypeMismatch, node, OperatorData(node.op, left, right));
    return ScalarKind::Error;
}

void TypeCheckerVisitor::visit(AST::Program& program) {
    _symbolTable.setCurrentScope(program.scope);
    for (const auto& stmt: program.statements) {
        stmt->accept(*this);
    }
}

void TypeCheckerVisitor::visit(AST::FunctionDeclaration& node) {
    _currentFunctionReturnType = node.returnType;
    _foundReturn = false;

    node.body->accept(*this);
    if (!_foundReturn && node.returnType != _typeCtx.get(ScalarKind::Void)) {
        logError(SemanticErrorType::MissingReturn, node);
    }
}

void TypeCheckerVisitor::visit(AST::ExpressionStatement& node) {
    node.expression->accept(*this);
}

void TypeCheckerVisitor::visit(AST::ArrayIndex& node) {
}

void TypeCheckerVisitor::visit(AST::IfStatement& node) {
    node.condition->accept(*this);
    if (node.condition->type != _typeCtx.get(ScalarKind::Bool)) {
        logError(SemanticErrorType::NonBooleanCondition, node);
    }

    // Check the 'then' branch
    bool originalStatus = _foundReturn;
    _foundReturn = false;
    node.thenBranch->accept(*this);
    bool thenReturns = _foundReturn;

    // Check the 'else' branch
    bool elseReturns = false;
    _foundReturn = false;
    if (node.elseBranch) {
        node.elseBranch->accept(*this);
        elseReturns = _foundReturn;
    }

    // Logic: The whole IF is terminated ONLY if both paths return
    // If there is no else branch, elseReturns is false, so foundReturn becomes false.
    _foundReturn = originalStatus || (thenReturns && elseReturns);
}

void TypeCheckerVisitor::visit(AST::WhileLoop& node) {
    node.condition->accept(*this);

    // Ensure condition returns a boolean
    if (node.condition->type != _typeCtx.get(ScalarKind::Bool)) {
        logError(SemanticErrorType::NonBooleanCondition, node);
    }

    node.body->accept(*this);
}

void TypeCheckerVisitor::visit(AST::ForLoop& node) {
    node.range->accept(*this);

    if (node.step) {
        node.step->accept(*this);
    }

    if (node.range->type != _typeCtx.get(ScalarKind::Int) || node.step->type != _typeCtx.get(ScalarKind::Int)) {
        logError(SemanticErrorType::NonIntForLoop, node);
    }

    node.body->accept(*this);
}

void TypeCheckerVisitor::visit(AST::RangeExpression& node) {
    node.start->accept(*this);
    node.end->accept(*this);

    const ScalarKind startKind = kindOf(node.start->type);
    const ScalarKind endKind = kindOf(node.end->type);

    if (!isNumeric(startKind) || !isNumeric(endKind)) {
        logError(SemanticErrorType::NonNumericRange, node);
        return;
    }

    node.type = _typeCtx.get(getPromotedType(startKind, endKind));
}

void TypeCheckerVisitor::visit(AST::Block& node) {
    _symbolTable.setCurrentScope(node.scope);
    for (const auto& stmt: node.statements) {
        if (_foundReturn) {
            // Found a statement after a return!
            logError(SemanticErrorType::UnreachableCode, *stmt);
            break;
        }

        stmt->accept(*this);
    }
}

void TypeCheckerVisitor::visit(AST::VariableDeclaration& node) {
    if (node.initializer) {
        node.initializer->accept(*this);

        if (auto* arrayPtr = dynamic_cast<AST::ArrayLiteral *>(node.initializer.get())) {
            const auto [baseKind, size] = checkArrayLiteralType(*arrayPtr);
            if (baseKind == ScalarKind::Error) {
                logError(SemanticErrorType::MultiTypeArray, node);
                return;
            }

            // Build the proper ArrayTypeNode and assign it to both the expression and symbol
            TypeNode* baseType = _typeCtx.get(baseKind);
            TypeNode* arrayType = _typeCtx.getArray(baseType, size);
            node.initializer->type = arrayType;
            node.symbol->type = arrayType;
            return;
        }

        // Scalar assignment - check compatibility
        const ScalarKind initKind = kindOf(node.initializer->type);
        if (node.type) {
            // declared type exists
            if (!isAssignableTo(initKind, kindOf(node.type))) {
                logError(SemanticErrorType::IncompatibleAssignment, node,
                         TypeMismatchData(kindOf(node.type), initKind));
                return;
            }
        } else {
            // Type inference - symbol gets the inferred type
            node.symbol->type = node.initializer->type;
        }
    } else {
        if (!node.type) {
            logError(SemanticErrorType::CannotInferType, node);
        }
    }
}

void TypeCheckerVisitor::visit(AST::FunctionCall& node) {
    if (node.functionName == "main") node.functionName = "__main";

    const auto calledFunction = _symbolTable.lookup(node.functionName);
    if (!calledFunction) {
        logError(SemanticErrorType::UndefinedFunction, node, node.functionName);
        return;
    }

    if (!checkArgCount(node, *calledFunction)) return;

    node.type = calledFunction->type;

    for (int i = 0; i < node.arguments.size(); ++i) {
        if (i < calledFunction->params.size())
            checkArgument(node, node.arguments[i], calledFunction->params[i], *calledFunction);
        else
            node.arguments[i].value->accept(*this); // variadic overflow — resolve type only
    }
}

void TypeCheckerVisitor::visit(AST::BinaryExpression& node) {
    node.left->accept(*this);
    node.right->accept(*this);

    const ScalarKind left = kindOf(node.left->type);
    const ScalarKind right = kindOf(node.right->type);

    if (left == ScalarKind::Error || right == ScalarKind::Error) return;

    const ScalarKind result = checkBinaryOp(node, left, right);
    node.type = _typeCtx.get(result);
}

void TypeCheckerVisitor::visit(AST::UnaryExpression& node) {
    node.operand->accept(*this);

    const ScalarKind operandType = kindOf(node.operand->type);

    switch (node.op) {
        case Operator::Plus:
        case Operator::PlusPlus:
        case Operator::Minus:
        case Operator::MinusMinus: {
            if (!isNumeric(operandType)) {
                logError(SemanticErrorType::TypeMismatch, node, UnaryOperatorData(node.op, operandType));
                return;
            }

            node.type = _typeCtx.get(operandType);

            break;
        }

        case Operator::Exclamation: {
            if (!isBoolean(operandType)) {
                logError(SemanticErrorType::TypeMismatch, node, UnaryOperatorData(node.op, operandType));
                return;
            }

            node.type = _typeCtx.get(ScalarKind::Bool);

            break;
        }

        default:
            throw std::runtime_error(std::format("Internal error: Unhandled unary operator {}",
                                                 operatorToString(node.op)));
    }
}

void TypeCheckerVisitor::visit(AST::IncDecExpression& node) {
    auto operand = _symbolTable.lookup(node.variableName);
    if (!operand) {
        logError(SemanticErrorType::UndefinedIdentifier, node);
        return;
    }
    node.symbol = operand;

    const ScalarKind operandKind = kindOf(operand->type);

    switch (node.op) {
        case Operator::PlusPlus:
        case Operator::MinusMinus: {
            if (!isNumeric(operandKind)) {
                logError(SemanticErrorType::TypeMismatch, node, UnaryOperatorData(node.op, operandKind));
            }

            if (operand->isConst) {
                logError(SemanticErrorType::ReadOnlyAssignment, node);
            }
            node.type = operand->type;
            break;
        }

        default:
            throw std::runtime_error("Internal error: Unhandled postfix operator " + operatorToString(node.op));
    }
}

void TypeCheckerVisitor::visit(AST::VariableAssignment& node) {
    node.assignedValue->accept(*this);
    const ScalarKind assignedType = kindOf(node.assignedValue->type);

    const auto symbol = _symbolTable.lookup(node.varName);
    if (!symbol) {
        logError(SemanticErrorType::UndefinedIdentifier, node);
        return;
    }
    node.symbol = symbol;

    if (symbol->isConst) {
        logError(SemanticErrorType::ReadOnlyAssignment, node);
        return;
    }

    if (const ScalarKind variableType = kindOf(symbol->type);
        variableType != assignedType && !isAssignableTo(assignedType, variableType)) {
        logError(SemanticErrorType::IncompatibleAssignment, node, TypeMismatchData(variableType, assignedType));
        return;
    }
}

void TypeCheckerVisitor::visit(AST::ArrayAssignment& node) {
    node.index->accept(*this);
    node.assignedValue->accept(*this);

    if (kindOf(node.index->type) != ScalarKind::Int) {
        logError(SemanticErrorType::NonIntIndex, node);
        return;
    }

    const auto symbol = _symbolTable.lookup(node.varName);
    if (!symbol) {
        logError(SemanticErrorType::UndefinedIdentifier, node);
        return;
    }
    node.symbol = symbol;

    if (symbol->isConst) {
        logError(SemanticErrorType::ReadOnlyAssignment, node);
        return;
    }

    // Symbol must be an array type
    auto* arrayType = dynamic_cast<ArrayTypeNode *>(symbol->type);
    if (!arrayType) {
        logError(SemanticErrorType::TypeMismatch, node); // assigning to non-array with index
        return;
    }

    const ScalarKind assignedKind = kindOf(node.assignedValue->type);
    if (!isAssignableTo(assignedKind, kindOf(arrayType->base))) {
        logError(SemanticErrorType::TypeMismatch, node,
                 TypeMismatchData(kindOf(arrayType->base), assignedKind));
        return;
    }

    // Bounds check on literal index
    if (auto* litIdx = dynamic_cast<AST::IntegerLiteralNode *>(node.index.get())) {
        if (litIdx->value >= arrayType->size) {
            logError(SemanticErrorType::OutOfBounds, node);
        }
    }
}

void TypeCheckerVisitor::visit(AST::ReturnStatement& node) {
    _foundReturn = true;

    if (!node.value)
        return;

    node.value->accept(*this);

    const ScalarKind returnedType = kindOf(node.value->type);
    const ScalarKind functionReturnType = kindOf(_currentFunctionReturnType);
    if (!isAssignableTo(functionReturnType, returnedType)) {
        logError(SemanticErrorType::TypeMismatch, node, TypeMismatchData(functionReturnType, returnedType));
    }
}

void TypeCheckerVisitor::visit(AST::IntegerLiteralNode& node) {
    node.type = _typeCtx.get(ScalarKind::Int);
}

void TypeCheckerVisitor::visit(AST::FloatLiteralNode& node) {
    node.type = _typeCtx.get(ScalarKind::Float);
}

void TypeCheckerVisitor::visit(AST::StringLiteralNode& node) {
    node.type = _typeCtx.get(ScalarKind::String);
}

void TypeCheckerVisitor::visit(AST::BooleanLiteralNode& node) {
    node.type = _typeCtx.get(ScalarKind::Bool);
}

void TypeCheckerVisitor::visit(AST::VariableExpression& node) {
    if (!node.symbol) {
        // Only look up if we haven't already
        node.symbol = _symbolTable.lookup(node.name);
    }
    if (!node.symbol) {
        logError(SemanticErrorType::UndefinedIdentifier, node, node.name);
        node.type = _typeCtx.get(ScalarKind::Error);
        return;
    }

    node.type = node.symbol->type;
}

void TypeCheckerVisitor::visit(AST::ArrayLiteral& node) {
}
