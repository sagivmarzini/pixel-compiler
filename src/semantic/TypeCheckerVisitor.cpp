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

PrimitiveKind TypeCheckerVisitor::getPromotedType(PrimitiveKind t1, PrimitiveKind t2) {
    if (t1 == t2) return t1;

    if (isNumeric(t1) && isNumeric(t2)) {
        if (t1 == PrimitiveKind::Float || t2 == PrimitiveKind::Float) return PrimitiveKind::Float;
        return PrimitiveKind::Int; // Default numeric promotion
    }

    return PrimitiveKind::Error; // Mismatched types (e.g., Int and String)
}

PrimitiveKind TypeCheckerVisitor::kindOf(TypeNode* t) {
    if (!t) return PrimitiveKind::Unspecified;
    if (auto* s = dynamic_cast<ScalarTypeNode*>(t)) return s->scalar;
    if (dynamic_cast<ArrayTypeNode*>(t)) return PrimitiveKind::Error; // arrays aren't scalar
    return PrimitiveKind::Error;
}

bool TypeCheckerVisitor::isNumeric(PrimitiveKind type) {
    return type == PrimitiveKind::Int || type == PrimitiveKind::Float;
}

bool TypeCheckerVisitor::areComparableTypes(PrimitiveKind leftType, PrimitiveKind rightType) {
    return isNumeric(leftType) && isNumeric(rightType) || isString(leftType) && isString(rightType);
}

bool TypeCheckerVisitor::isString(PrimitiveKind type) {
    return type == PrimitiveKind::String;
}

bool TypeCheckerVisitor::isBoolean(PrimitiveKind type) {
    return type == PrimitiveKind::Bool;
}

bool TypeCheckerVisitor::isAssignableTo(PrimitiveKind assignedType, PrimitiveKind targetType) {
    return assignedType == targetType || (assignedType == PrimitiveKind::Int && targetType == PrimitiveKind::Float);
}

std::pair<PrimitiveKind, int> TypeCheckerVisitor::checkArrayLiteralType(const AST::ArrayLiteral& arr) {
    if (arr.elements.empty()) return {PrimitiveKind::Unspecified, 0};

    arr.elements[0]->accept(*this);

    PrimitiveKind common = kindOf(arr.elements[0]->type);
    const int     size   = arr.elements.size();
    for (size_t i = 1; i < size; ++i) {
        arr.elements[i]->accept(*this);
        common = getPromotedType(common, kindOf(arr.elements[i]->type));

        if (common == PrimitiveKind::Error) return {PrimitiveKind::Error, size};
    }

    return {common, size};
}

bool TypeCheckerVisitor::checkArgCount(const AST::FunctionCall& node, const Symbol& fn) {
    const bool isVariadic = node.functionName == "print";
    if (!isVariadic && node.arguments.size() != fn.params.size()) {
        logError(SemanticErrorType::ArgumentCountMismatch, node,
                 ParamMismatchData(node.functionName, fn.params.size(), node.arguments.size()));
        return false;
    }
    return true;
}

void TypeCheckerVisitor::checkArgument(AST::FunctionCall&                                 node,
                                       const AST::FunctionCall::FunctionArgument&         arg,
                                       const AST::FunctionDeclaration::FunctionParameter& param,
                                       const Symbol&                                      fn) {
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

PrimitiveKind TypeCheckerVisitor::checkBinaryOp(const AST::BinaryExpression& node, PrimitiveKind left,
                                                PrimitiveKind                right) {
    switch (node.op) {
        case Operator::Plus:
        case Operator::Minus:
        case Operator::Star:
        case Operator::Slash:
            return checkArithmetic(node, left, right);

        case Operator::Equal:
        case Operator::NotEqual:
            if (areComparableTypes(left, right) || (isBoolean(left) && isBoolean(right)))
                return PrimitiveKind::Bool;
            break;

        case Operator::LessThan:
        case Operator::GreaterThan:
        case Operator::LessEqual:
        case Operator::GreaterEqual:
            if (areComparableTypes(left, right))
                return PrimitiveKind::Bool;
            break;

        case Operator::LogicalAnd:
        case Operator::LogicalOr:
            if (isBoolean(left) && isBoolean(right))
                return PrimitiveKind::Bool;
            break;

        default:
            throw std::runtime_error(
                std::format("Internal error: Unhandled binary operator {}", operatorToString(node.op)));
    }

    logError(SemanticErrorType::TypeMismatch, node, OperatorData(node.op, left, right));
    return PrimitiveKind::Error;
}

PrimitiveKind
TypeCheckerVisitor::checkArithmetic(const AST::BinaryExpression& node, PrimitiveKind left, PrimitiveKind right) {
    if (isNumeric(left) && isNumeric(right)) {
        const PrimitiveKind promoted = getPromotedType(left, right);
        if (promoted == PrimitiveKind::Error)
            logError(SemanticErrorType::TypeMismatch, node, OperatorData(node.op, left, right));
        return promoted;
    }

    if (node.op == Operator::Plus && (isString(left) || isString(right))) {
        const PrimitiveKind other = isString(left) ? right : left;
        if (isString(other) || isNumeric(other))
            return PrimitiveKind::String;
    }

    logError(SemanticErrorType::TypeMismatch, node, OperatorData(node.op, left, right));
    return PrimitiveKind::Error;
}

void TypeCheckerVisitor::handleArrayLiteralInit(const AST::VariableDeclaration& node, const AST::ArrayLiteral* arrayLit,
                                                const ArrayTypeNode*            declaredArray) {
    const auto [elemKind, literalSize] = checkArrayLiteralType(*arrayLit);

    if (elemKind == PrimitiveKind::Error) {
        logError(SemanticErrorType::MultiTypeArray, node);
        return;
    }

    if (declaredArray) {
        const bool hasExplicitElemType = declaredArray->base != nullptr;

        if (hasExplicitElemType &&
            !isAssignableTo(elemKind, kindOf(declaredArray->base))) {
            logError(SemanticErrorType::IncompatibleAssignment, node,
                     TypeMismatchData(kindOf(declaredArray->base), elemKind));
            return;
        }

        const bool hasBoundedSize = declaredArray->size != -1;
        if (hasBoundedSize && literalSize > declaredArray->size) {
            logError(SemanticErrorType::ArrayLiteralTooLarge, node);
            return;
        }
    }

    TypeNode* resolvedType = _typeCtx.getArray(_typeCtx.get(elemKind), literalSize);
    node.initializer->type = resolvedType;
    node.symbol->type      = resolvedType;
}

void TypeCheckerVisitor::handleScalarFillInit(const AST::VariableDeclaration& node,
                                              const ArrayTypeNode*            declaredArray) {
    const PrimitiveKind initKind = kindOf(node.initializer->type);

    if (declaredArray->base) {
        // Element type was explicitly declared: `var nums[5]: int = 7;`
        if (!isAssignableTo(initKind, kindOf(declaredArray->base))) {
            logError(SemanticErrorType::IncompatibleAssignment, node,
                     TypeMismatchData(kindOf(declaredArray->base), initKind));
            return;
        }
        node.symbol->type = node.type;
    } else {
        // Element type inferred from scalar: `var nums[5] = 7;`
        node.symbol->type = _typeCtx.getArray(_typeCtx.get(initKind), declaredArray->size);
    }
}

void TypeCheckerVisitor::handleScalarInit(const AST::VariableDeclaration& node) {
    const PrimitiveKind initKind = kindOf(node.initializer->type);

    if (!node.type) {
        node.symbol->type = node.initializer->type; // type inference
        return;
    }

    if (!isAssignableTo(initKind, kindOf(node.type))) {
        logError(SemanticErrorType::IncompatibleAssignment, node,
                 TypeMismatchData(kindOf(node.type), initKind));
    }
}

void TypeCheckerVisitor::visit(AST::Program& program) {
    _symbolTable.setCurrentScope(program.scope);
    for (const auto& stmt: program.statements) {
        stmt->accept(*this);
    }
}

void TypeCheckerVisitor::visit(AST::FunctionDeclaration& node) {
    _currentFunctionReturnType = node.returnType;
    _foundReturn               = false;

    node.body->accept(*this);

    if (!_foundReturn && node.returnType != _typeCtx.get(PrimitiveKind::Void))
        logError(SemanticErrorType::MissingReturn, node);
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

void TypeCheckerVisitor::visit(AST::VariableDeclaration& node) {
    if (!node.initializer) {
        if (!node.type)
            logError(SemanticErrorType::CannotInferType, node);
        return;
    }

    node.initializer->accept(*this);

    auto* declaredArray = dynamic_cast<ArrayTypeNode*>(node.type);

    if (auto* arrayLit = dynamic_cast<AST::ArrayLiteral*>(node.initializer.get())) {
        if (node.symbol->scope->getParent()->getParent() == nullptr) {
            // Global scope
            if (dynamic_cast<ArrayTypeNode*>(node.symbol->type) ||
                dynamic_cast<AST::ArrayLiteral*>(node.initializer.get())) {
                logError(SemanticErrorType::GlobalArrayNotSupported, node);
                return;
            }
        }
        handleArrayLiteralInit(node, arrayLit, declaredArray);
    } else if (declaredArray) {
        handleScalarFillInit(node, declaredArray);
    } else {
        handleScalarInit(node);
    }
}

void TypeCheckerVisitor::visit(AST::VariableAssignment& node) {
    node.assignedValue->accept(*this);
    const PrimitiveKind assignedType = kindOf(node.assignedValue->type);

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

    if (const PrimitiveKind variableType = kindOf(symbol->type);
        variableType != assignedType && !isAssignableTo(assignedType, variableType)) {
        logError(SemanticErrorType::IncompatibleAssignment, node, TypeMismatchData(variableType, assignedType));
        return;
    }
}

void TypeCheckerVisitor::visit(AST::ArrayAssignment& node) {
    node.index->accept(*this);
    node.assignedValue->accept(*this);

    if (kindOf(node.index->type) != PrimitiveKind::Int) {
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
    auto* arrayType = dynamic_cast<ArrayTypeNode*>(symbol->type);
    if (!arrayType) {
        logError(SemanticErrorType::TypeMismatch, node); // assigning to non-array with index
        return;
    }

    const PrimitiveKind assignedKind = kindOf(node.assignedValue->type);
    if (!isAssignableTo(assignedKind, kindOf(arrayType->base))) {
        logError(SemanticErrorType::TypeMismatch, node,
                 TypeMismatchData(kindOf(arrayType->base), assignedKind));
        return;
    }

    // Bounds check on literal index
    if (auto* litIdx = dynamic_cast<AST::IntegerLiteralNode*>(node.index.get())) {
        if (litIdx->value >= arrayType->size) {
            logError(SemanticErrorType::OutOfBounds, node);
        }
    }
}

void TypeCheckerVisitor::visit(AST::IfStatement& node) {
    node.condition->accept(*this);
    if (node.condition->type != _typeCtx.get(PrimitiveKind::Bool)) {
        logError(SemanticErrorType::NonBooleanCondition, node);
    }

    // Check the 'then' branch
    bool originalStatus = _foundReturn;
    _foundReturn        = false;
    node.thenBranch->accept(*this);
    bool thenReturns = _foundReturn;

    // Check the 'else' branch
    bool elseReturns = false;
    _foundReturn     = false;
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
    if (node.condition->type != _typeCtx.get(PrimitiveKind::Bool)) {
        logError(SemanticErrorType::NonBooleanCondition, node);
    }

    node.body->accept(*this);
}

void TypeCheckerVisitor::visit(AST::ForLoop& node) {
    node.range->accept(*this);

    if (node.step) {
        node.step->accept(*this);
    }

    if (node.range->type != _typeCtx.get(PrimitiveKind::Int) || node.step->type != _typeCtx.get(PrimitiveKind::Int)) {
        logError(SemanticErrorType::NonIntForLoop, node);
    }

    node.body->accept(*this);
}

void TypeCheckerVisitor::visit(AST::RangeExpression& node) {
    node.start->accept(*this);
    node.end->accept(*this);

    const PrimitiveKind startKind = kindOf(node.start->type);
    const PrimitiveKind endKind   = kindOf(node.end->type);

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

void TypeCheckerVisitor::visit(AST::ReturnStatement& node) {
    _foundReturn = true;

    if (!node.value)
        return;

    node.value->accept(*this);

    const PrimitiveKind returnedType       = kindOf(node.value->type);
    const PrimitiveKind functionReturnType = kindOf(_currentFunctionReturnType);
    if (!isAssignableTo(functionReturnType, returnedType)) {
        logError(SemanticErrorType::TypeMismatch, node, TypeMismatchData(functionReturnType, returnedType));
    }
}

void TypeCheckerVisitor::visit(AST::BinaryExpression& node) {
    node.left->accept(*this);
    node.right->accept(*this);

    const PrimitiveKind left  = kindOf(node.left->type);
    const PrimitiveKind right = kindOf(node.right->type);

    if (left == PrimitiveKind::Error || right == PrimitiveKind::Error) return;

    const PrimitiveKind result = checkBinaryOp(node, left, right);
    node.type                  = _typeCtx.get(result);
}

void TypeCheckerVisitor::visit(AST::UnaryExpression& node) {
    node.operand->accept(*this);

    const PrimitiveKind operandType = kindOf(node.operand->type);

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

            node.type = _typeCtx.get(PrimitiveKind::Bool);

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

    const PrimitiveKind operandKind = kindOf(operand->type);

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

void TypeCheckerVisitor::visit(AST::ExpressionStatement& node) {
    node.expression->accept(*this);
}

void TypeCheckerVisitor::visit(AST::ArrayIndex& node) {
    const auto symbol = _symbolTable.lookup(node.variableName);
    if (!symbol) {
        logError(SemanticErrorType::UndefinedIdentifier, node);
        return;
    }
    if (symbol->isConst) {
        logError(SemanticErrorType::ReadOnlyAssignment, node);
        return;
    }
    node.symbol = symbol;

    node.index->accept(*this);
    if (kindOf(node.index->type) != PrimitiveKind::Int) {
        logError(SemanticErrorType::NonIntIndex, node);
        return;
    }

    // Symbol must be an array type
    auto* arrayType = dynamic_cast<ArrayTypeNode*>(symbol->type);
    if (!arrayType) {
        logError(SemanticErrorType::TypeMismatch, node); // assigning to non-array with index
        return;
    }

    // Bounds check on literal index
    if (auto* litIdx = dynamic_cast<AST::IntegerLiteralNode*>(node.index.get())) {
        if (litIdx->value >= arrayType->size) {
            logError(SemanticErrorType::OutOfBounds, node);
        }
    }

    node.type = arrayType->base;
}

void TypeCheckerVisitor::visit(AST::IntegerLiteralNode& node) {
    node.type = _typeCtx.get(PrimitiveKind::Int);
}

void TypeCheckerVisitor::visit(AST::FloatLiteralNode& node) {
    node.type = _typeCtx.get(PrimitiveKind::Float);
}

void TypeCheckerVisitor::visit(AST::StringLiteralNode& node) {
    node.type = _typeCtx.get(PrimitiveKind::String);
}

void TypeCheckerVisitor::visit(AST::BooleanLiteralNode& node) {
    node.type = _typeCtx.get(PrimitiveKind::Bool);
}

void TypeCheckerVisitor::visit(AST::VariableExpression& node) {
    if (!node.symbol) {
        // Only look up if we haven't already
        node.symbol = _symbolTable.lookup(node.name);
    }
    if (!node.symbol) {
        logError(SemanticErrorType::UndefinedIdentifier, node, node.name);
        node.type = _typeCtx.get(PrimitiveKind::Error);
        return;
    }

    node.type = node.symbol->type;
}

void TypeCheckerVisitor::visit(AST::ArrayLiteral& node) {
    const auto [elemKind, size] = checkArrayLiteralType(node);

    if (elemKind == PrimitiveKind::Error) {
        logError(SemanticErrorType::MultiTypeArray, node);
        node.type = _typeCtx.get(PrimitiveKind::Error);
        return;
    }

    if (elemKind == PrimitiveKind::Unspecified) {
        // Empty literal [] — type cannot be inferred standalone;
        // VariableDeclaration will resolve it from the declared type.
        node.type = nullptr;
        return;
    }

    node.type = _typeCtx.getArray(_typeCtx.get(elemKind), size);
}
