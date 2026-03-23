#include "TypeCheckerVisitor.h"

#include <format>
#include <set>


#include "CompilerException.h"
#include "parse/AST/Expression.h"
#include "parse/AST/Statement.h"
#include "types/TypeContext.h"

TypeCheckerVisitor::TypeCheckerVisitor(SymbolTable& symbolTable, TypeContext& typeCtx)
    : SemanticVisitor(symbolTable), _typeCtx(typeCtx) {
}

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

TypeNode* TypeCheckerVisitor::nodeFor(ScalarKind k) const {
    switch (k) {
        case ScalarKind::Int: return _typeCtx.getInt();
        case ScalarKind::Float: return _typeCtx.getFloat();
        case ScalarKind::Bool: return _typeCtx.getBool();
        case ScalarKind::String: return _typeCtx.getString();
        case ScalarKind::Void: return _typeCtx.getVoid();
        default: return nullptr;
    }
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

ScalarKind TypeCheckerVisitor::checkArrayLiteralType(const AST::ArrayLiteral& arrayLiteral) {
    if (arrayLiteral.elements.empty()) return {ScalarKind::Unspecified, 0};

    arrayLiteral.elements[0]->accept(*this);
    ScalarKind commonType = arrayLiteral.elements[0]->type;
    const int arraySize = arrayLiteral.elements.size();

    for (size_t i = 1; i < arraySize; ++i) {
        arrayLiteral.elements[i]->accept(*this);

        commonType = getPromotedType(commonType, arrayLiteral.elements[i]->type);

        if (commonType == ScalarKind::Error) {
            return {ScalarKind::Error, arraySize};
        }
    }

    return {commonType, arraySize};
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
    if (!_foundReturn && node.returnType != ScalarKind::Void) {
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
    if (node.condition->type != ScalarKind::Bool) {
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
    if (node.condition->type != ScalarKind::Bool) {
        logError(SemanticErrorType::NonBooleanCondition, node);
    }

    node.body->accept(*this);
}

void TypeCheckerVisitor::visit(AST::ForLoop& node) {
    node.range->accept(*this);

    if (node.step) {
        node.step->accept(*this);
    }

    if (node.range->type != ScalarKind::Int || node.step->type != ScalarKind::Int) {
        logError(SemanticErrorType::NonIntForLoop, node);
    }

    node.body->accept(*this);
}

void TypeCheckerVisitor::visit(AST::RangeExpression& node) {
    node.start->accept(*this);
    node.end->accept(*this);

    const ScalarKind startType = node.start->type;
    const ScalarKind endType = node.end->type;

    if (!isNumeric(startType) || !isNumeric(endType)) {
        logError(SemanticErrorType::NonNumericRange, node);
        return;
    }

    node.type = getPromotedType(startType, endType);
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
            const auto arrayLiteralType = checkArrayLiteralType(*arrayPtr);
            if (arrayLiteralType.baseType == ScalarKind::Error) logError(SemanticErrorType::MultiTypeArray, node);

            node.initializer->type = arrayLiteralType.baseType;
            node.symbol->arrayType = arrayLiteralType;
        }

        if (node.specifiedType != ScalarKind::Unspecified &&
            !isAssignableTo(node.initializer->type, node.specifiedType)) {
            logError(SemanticErrorType::IncompatibleAssignment, node,
                     TypeMismatchData(node.specifiedType, node.initializer->type));
            return;
        }

        const auto symbol = node.symbol;
        if (!symbol) throw std::runtime_error("Internal error: undefined variable: " + node.name);

        if (symbol->type == ScalarKind::Unspecified)
            symbol->type = node.initializer->type;
    } else {
        // Parser already checked for typeless variable with no initial value,
        // checking here again just in case
        if (node.specifiedType == ScalarKind::Unspecified) {
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

    // Check if the number of arguments matches
    if (const auto expectedArgAmount = calledFunction->params.size();
        node.arguments.size() != expectedArgAmount &&
        // printf has a variable number of arguments
        node.functionName != "print") {
        logError(SemanticErrorType::ArgumentCountMismatch, node,
                 ParamMismatchData(node.functionName, expectedArgAmount, node.arguments.size()));
        return;
    }

    node.type = calledFunction->type;
    std::set<std::string> seenParams;
    int index = 0;

    for (const auto& argument: node.arguments) {
        if (index < calledFunction->params.size()) {
            auto& parameter = calledFunction->params.at(index);

            if (argument.name.has_value()) {
                // named argument
                auto argName = argument.name.value();

                if (parameter.name != argName) {
                    if (calledFunction->getParameterByName(argName)) {
                        // invalid pos
                        logError(SemanticErrorType::InvalidArgumentPosition, node,
                                 ArgumentPositionData(node.arguments, calledFunction->params));
                        return;
                    }

                    logError(SemanticErrorType::UndefinedArgument, node, argName);
                    return;
                }
            } else if (!parameter.isImplicit) {
                logError(SemanticErrorType::MissingArgumentLabel, node, parameter.name);
            }

            // Validate type against the formal parameter
            argument.value->accept(*this);
            if (!isAssignableTo(argument.value->type, parameter.type)) {
                logError(SemanticErrorType::ArgumentTypeMismatch, node,
                         TypeMismatchData(parameter.type, argument.value->type));
                return;
            }
        } else {
            // 2. Extra arguments for printf path
            // Just resolve the type of the extra expression so the compiler doesn't crash
            argument.value->accept(*this);

            // Optional: In a real variadic system, you'd check if these are "basic" types
            // (int, float, ptr) since printf can't handle complex structs.
        }

        index++;
    }
}

void TypeCheckerVisitor::visit(AST::BinaryExpression& node) {
    node.left->accept(*this);
    node.right->accept(*this);
    if (node.left->type == ScalarKind::Error || node.right->type == ScalarKind::Error) return;

    const ScalarKind leftType = node.left->type;
    const ScalarKind rightType = node.right->type;

    switch (node.op) {
        // Arithmetic Operators (+, -, *, /)
        case Operator::Plus:
        case Operator::Minus:
        case Operator::Star:
        case Operator::Slash: {
            if (isNumeric(leftType) && isNumeric(rightType)) {
                // Determine result type based on type promotion (e.g., int + float = float)
                node.type = getPromotedType(leftType, rightType);
                if (node.type == ScalarKind::Error) {
                    logError(SemanticErrorType::TypeMismatch, node, OperatorData(node.op, leftType, rightType));
                    return;
                }
            } else if (node.op == Operator::Plus && (isString(leftType) || isString(rightType))) {
                // Check that the OTHER type is one that can be converted to a string (e.g., numeric, or another string).
                if (const ScalarKind otherType = isString(leftType) ? rightType : leftType;
                    isString(otherType) || isNumeric(otherType)) {
                    node.type = ScalarKind::String;
                } else {
                    logError(SemanticErrorType::TypeMismatch, node,
                             OperatorData(node.op, ScalarKind::String, otherType));
                    return;
                }
            } else {
                logError(SemanticErrorType::TypeMismatch, node, OperatorData(node.op, leftType, rightType));
                return;
            }
            break;
        }

        // Comparison and Equality Operators (=, !=, <, >, <=, >=)
        case Operator::Equal:
        case Operator::NotEqual: {
            // Comparison is valid for numerics and strings (lexicographical)
            if (areComparableTypes(leftType, rightType) || isBoolean(leftType) && isBoolean(rightType)) {
                node.type = ScalarKind::Bool; // The result of comparison is always boolean
            } else {
                logError(SemanticErrorType::TypeMismatch, node, OperatorData(node.op, leftType, rightType));
                return;
            }
            break;
        }
        case Operator::LessThan:
        case Operator::GreaterThan:
        case Operator::LessEqual:
        case Operator::GreaterEqual: {
            // Comparison is valid for numerics and strings (lexicographical)
            if (areComparableTypes(leftType, rightType)) {
                node.type = ScalarKind::Bool; // The result of comparison is always boolean
            } else {
                logError(SemanticErrorType::TypeMismatch, node, OperatorData(node.op, leftType, rightType));
                return;
            }
            break;
        }

        // Logical Operators (&&, ||)
        case Operator::LogicalAnd:
        case Operator::LogicalOr: {
            // Logical operators only work on booleans
            if (isBoolean(leftType) && isBoolean(rightType)) {
                node.type = ScalarKind::Bool; // Result is always boolean
            } else {
                logError(SemanticErrorType::TypeMismatch, node, OperatorData(node.op, leftType, rightType));
                return;
            }
            break;
        }

        default:
            throw std::runtime_error(
                std::format("Internal error: Unhandled binary operator {}", operatorToString(node.op))
            );
    }
}

void TypeCheckerVisitor::visit(AST::UnaryExpression& node) {
    node.operand->accept(*this);

    const ScalarKind operandType = node.operand->type;

    switch (node.op) {
        case Operator::Plus:
        case Operator::PlusPlus:
        case Operator::Minus:
        case Operator::MinusMinus: {
            if (!isNumeric(operandType)) {
                logError(SemanticErrorType::TypeMismatch, node, UnaryOperatorData(node.op, operandType));
                return;
            }

            node.type = operandType;

            break;
        }

        case Operator::Exclamation: {
            if (!isBoolean(operandType)) {
                logError(SemanticErrorType::TypeMismatch, node, UnaryOperatorData(node.op, operandType));
                return;
            }

            node.type = ScalarKind::Bool;

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

    switch (node.op) {
        case Operator::PlusPlus:
        case Operator::MinusMinus: {
            if (!isNumeric(operand->type)) {
                logError(SemanticErrorType::TypeMismatch, node, UnaryOperatorData(node.op, operand->type));
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
    const ScalarKind assignedType = node.assignedValue->type;

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

    if (const ScalarKind variableType = symbol->type;
        variableType != assignedType && !isAssignableTo(assignedType, variableType)) {
        logError(SemanticErrorType::IncompatibleAssignment, node, TypeMismatchData(variableType, assignedType));
    }
}

void TypeCheckerVisitor::visit(AST::ArrayAssignment& node) {
    node.index->accept(*this);
    node.assignedValue->accept(*this);
    const ScalarKind indexType = node.index->type;
    const ScalarKind assignedType = node.assignedValue->type;
    if (indexType != ScalarKind::Int) {
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

    if (assignedType != node.symbol->arrayType->baseType) {
        logError(SemanticErrorType::TypeMismatch, node,
                 TypeMismatchData(node.symbol->arrayType->baseType, assignedType));
        return;
    }
    // Catch out of bounds with literal int if possible
    if (const auto& index = dynamic_cast<AST::IntegerLiteralNode *>(node.index.get());
        index->value >= node.symbol->arrayType->size) {
        logError(SemanticErrorType::OutOfBounds, node);
    }
}

void TypeCheckerVisitor::visit(AST::ReturnStatement& node) {
    _foundReturn = true;

    if (!node.value)
        return;

    node.value->accept(*this);

    if (const auto returnedType = node.value->type;
        !isAssignableTo(_currentFunctionReturnType, returnedType)) {
        logError(SemanticErrorType::TypeMismatch, node, TypeMismatchData(_currentFunctionReturnType, returnedType));
    }
}

void TypeCheckerVisitor::visit(AST::IntegerLiteralNode& node) {
    node.type = ScalarKind::Int;
}

void TypeCheckerVisitor::visit(AST::FloatLiteralNode& node) {
    node.type = ScalarKind::Float;
}

void TypeCheckerVisitor::visit(AST::StringLiteralNode& node) {
    node.type = ScalarKind::String;
}

void TypeCheckerVisitor::visit(AST::BooleanLiteralNode& node) {
    node.type = ScalarKind::Bool;
}

void TypeCheckerVisitor::visit(AST::VariableExpression& node) {
    if (!node.symbol) {
        // Only look up if we haven't already
        node.symbol = _symbolTable.lookup(node.name);
    }
    if (!node.symbol) {
        logError(SemanticErrorType::UndefinedIdentifier, node, node.name);
        node.type = ScalarKind::Error;
        return;
    }

    node.type = node.symbol->type;
}

void TypeCheckerVisitor::visit(AST::ArrayLiteral& node) {
}
