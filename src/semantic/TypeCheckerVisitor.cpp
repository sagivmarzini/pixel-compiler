#include "TypeCheckerVisitor.h"

#include <set>


#include "CompilerException.h"
#include "parse/AST/Expression.h"
#include "parse/AST/Statement.h"

void TypeCheckerVisitor::run(AstNode& root) {
    root.accept(*this);

    if (!_errors.empty())
        throw CompilerException(_errors);
}

bool TypeCheckerVisitor::isNumeric(Type type) {
    return type == Type::Int || type == Type::Float;
}

bool TypeCheckerVisitor::areComparableTypes(Type leftType, Type rightType) {
    return isNumeric(leftType) && isNumeric(rightType) || isString(leftType) && isString(rightType);
}

bool TypeCheckerVisitor::isString(Type type) {
    return type == Type::String;
}

bool TypeCheckerVisitor::isBoolean(Type type) {
    return type == Type::Boolean;
}

bool TypeCheckerVisitor::isAssignableTo(Type assignedType, Type variableType) {
    return assignedType == variableType || (isNumeric(assignedType) && isNumeric(variableType));
}

Type TypeCheckerVisitor::getPromotedType(Type leftType, Type rightType) {
    if (leftType == rightType) return leftType;

    if (leftType == Type::Float || rightType == Type::Float) {
        return Type::Float;
    }

    return Type::Error;
}

void TypeCheckerVisitor::visit(Program& program) {
    _symbolTable.setCurrentScope(program.scope);
    for (const auto& stmt: program.statements) {
        stmt->accept(*this);
    }
}

void TypeCheckerVisitor::visit(FunctionDeclaration& node) {
    _currentFunctionReturnType = node.returnType;
    _foundReturn               = false;

    node.body->accept(*this);

    if (!_foundReturn && node.returnType != Type::Void) {
        logError(SemanticErrorType::MissingReturn, node);
    }
}

void TypeCheckerVisitor::visit(ExpressionStatement& node) {
    node.expression->accept(*this);
}

void TypeCheckerVisitor::visit(IfStatement& node) {
    node.condition->accept(*this);

    // Ensure condition returns a boolean
    if (node.condition->type != Type::Boolean) {
        logError(SemanticErrorType::NonBooleanCondition, node);
    }

    node.thenBranch->accept(*this);

    if (node.elseBranch)
        node.elseBranch->accept(*this);
}

void TypeCheckerVisitor::visit(WhileLoop& node) {
    node.condition->accept(*this);

    // Ensure condition returns a boolean
    if (node.condition->type != Type::Boolean) {
        logError(SemanticErrorType::NonBooleanCondition, node);
    }

    node.body->accept(*this);
}

void TypeCheckerVisitor::visit(ForLoop& node) {
    node.range->accept(*this);

    if (node.step) {
        node.step->accept(*this);

        if (!isNumeric(node.step->type)) {
            logError(SemanticErrorType::NonNumericStep, node);
        }
    }

    node.body->accept(*this);
}

void TypeCheckerVisitor::visit(RangeExpression& node) {
    node.start->accept(*this);
    node.end->accept(*this);

    const Type startType = node.start->type;
    const Type endType   = node.end->type;

    if (!isNumeric(startType) || !isNumeric(endType)) {
        logError(SemanticErrorType::NonNumericRange, node);
        return;
    }

    node.type = Type::Int;
}

void TypeCheckerVisitor::visit(Block& node) {
    _symbolTable.setCurrentScope(node.scope);
    for (const auto& stmt: node.statements) {
        stmt->accept(*this);
    }
}

void TypeCheckerVisitor::visit(VariableDeclaration& node) {
    if (node.value) {
        node.value->accept(*this);

        if (node.specifiedType != Type::Unspecified && node.value->type != node.specifiedType) {
            logError(SemanticErrorType::IncompatibleAssignment, node,
                     TypeMismatchData(node.specifiedType, node.value->type));
            return;
        }

        const auto symbol = node.symbol;
        if (!symbol) throw std::runtime_error("Internal error: undefined variable: " + node.name);

        if (symbol->type == Type::Unspecified)
            symbol->type = node.value->type;
    } else {
        // Parser already checked for typeless variable with no initial value,
        // checking here again just in case
        if (node.specifiedType == Type::Unspecified) {
            logError(SemanticErrorType::CannotInferType, node);
            return;
        }
    }
}


void TypeCheckerVisitor::visit(FunctionCall& node) {
    const auto calledFunction = _symbolTable.lookup(node.functionName);
    if (!calledFunction) {
        logError(SemanticErrorType::UndefinedFunction, node, node.functionName);
        return;
    }

    // Check if the number of arguments matches
    if (const auto expectedArgAmount = calledFunction->params.size(); node.arguments.size() != expectedArgAmount) {
        logError(SemanticErrorType::ArgumentCountMismatch, node,
                 ParamMismatchData(node.functionName, expectedArgAmount, node.arguments.size()));
        return;
    }

    node.type = calledFunction->type;
    std::set<std::string> seenParams;

    for (const auto& argument: node.arguments) {
        // Check for duplicate named arguments
        if (seenParams.contains(argument.name)) {
            logError(SemanticErrorType::DuplicateParameterName, node, argument.name);
            return;
        }
        seenParams.insert(argument.name);

        // Verify the parameter name exists in the function definition
        auto parameter = calledFunction->getParameterByName(argument.name);
        if (!parameter.has_value()) {
            logError(SemanticErrorType::UndefinedParameter, node, argument.name);
            return;
        }

        // Visit the argument expression to resolve its type
        argument.value->accept(*this);

        // Compare the expression type to the expected parameter type
        if (argument.value->type != parameter.value().type) {
            logError(SemanticErrorType::ArgumentTypeMismatch, node,
                     TypeMismatchData(parameter.value().type, argument.value->type));
            return;
        }
    }
}

void TypeCheckerVisitor::visit(BinaryExpression& node) {
    node.left->accept(*this);
    node.right->accept(*this);
    if (node.left->type == Type::Error || node.right->type == Type::Error) return;

    const Type leftType  = node.left->type;
    const Type rightType = node.right->type;

    switch (node.op) {
        // Arithmetic Operators (+, -, *, /)
        case Operator::Plus:
        case Operator::Minus:
        case Operator::Star:
        case Operator::Slash: {
            if (isNumeric(leftType) && isNumeric(rightType)) {
                // Determine result type based on type promotion (e.g., int + float = float)
                node.type = getPromotedType(leftType, rightType);
                if (node.type == Type::Error) {
                    logError(SemanticErrorType::TypeMismatch, node, OperatorData(node.op, leftType, rightType));
                    return;
                }
            } else if (node.op == Operator::Plus && (isString(leftType) || isString(rightType))) {
                // Check that the OTHER type is one that can be converted to a string (e.g., numeric, or another string).
                if (const Type otherType = isString(leftType) ? rightType : leftType;
                    isString(otherType) || isNumeric(otherType)) {
                    node.type = Type::String;
                } else {
                    logError(SemanticErrorType::TypeMismatch, node, OperatorData(node.op, Type::String, otherType));
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
        case Operator::NotEqual:
        case Operator::LessThan:
        case Operator::GreaterThan:
        case Operator::LessEqual:
        case Operator::GreaterEqual: {
            // Comparison is valid for numerics and strings (lexicographical)
            if (areComparableTypes(leftType, rightType)) {
                node.type = Type::Boolean; // Result of comparison is always boolean
            } else {
                logError(SemanticErrorType::TypeMismatch, node, OperatorData(node.op, leftType, rightType));
                return;
            }
            break;
        }

        // Logical Operators (&&, ||)
        case Operator::And:
        case Operator::Or: {
            // Logical operators only work on booleans
            if (isBoolean(leftType) && isBoolean(rightType)) {
                node.type = Type::Boolean; // Result is always boolean
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

void TypeCheckerVisitor::visit(UnaryExpression& node) {
    node.operand->accept(*this);

    const Type operandType = node.operand->type;

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

            node.type = Type::Boolean;

            break;
        }

        default:
            throw std::runtime_error(std::format("Internal error: Unhandled unary operator {}",
                                                 operatorToString(node.op)));
    }
}

void TypeCheckerVisitor::visit(IncDecExpression &node) {
    auto operand = _symbolTable.lookup(node.variableName);
    if (!operand) {
        logError(SemanticErrorType::UndefinedIdentifier, node);
        return;
    }

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

void TypeCheckerVisitor::visit(VariableAssignment& node) {
    node.assignedValue->accept(*this);
    const Type assignedType = node.assignedValue->type;

    const auto symbol = _symbolTable.lookup(node.varName);
    if (!symbol) {
        logError(SemanticErrorType::UndefinedIdentifier, node);
        return;
    }

    if (symbol->isConst) {
        logError(SemanticErrorType::ReadOnlyAssignment, node);
        return;
    }

    if (const Type variableType = symbol->type;
        variableType != assignedType && !isAssignableTo(assignedType, variableType)) {
        logError(SemanticErrorType::IncompatibleAssignment, node, TypeMismatchData(variableType, assignedType));
        return;
    }
}

void TypeCheckerVisitor::visit(ReturnStatement& node) {
    _foundReturn = true;

    node.value->accept(*this);

    if (const auto returnedType = node.value->type;
        !isAssignableTo(_currentFunctionReturnType, returnedType)) {
        logError(SemanticErrorType::TypeMismatch, node, TypeMismatchData(_currentFunctionReturnType, returnedType));
        return;
    }
}

void TypeCheckerVisitor::visit(IntegerLiteralNode& node) {
    node.type = Type::Int;
}

void TypeCheckerVisitor::visit(FloatLiteralNode& node) {
    node.type = Type::Float;
}

void TypeCheckerVisitor::visit(StringLiteralNode& node) {
    node.type = Type::String;
}

void TypeCheckerVisitor::visit(BooleanLiteralNode& node) {
    node.type = Type::Boolean;
}

void TypeCheckerVisitor::visit(IdentifierNode& node) {
    if (!node.symbol) {
        // Only look up if we haven't already
        node.symbol = _symbolTable.lookup(node.name);
    }
    if (!node.symbol) {
        logError(SemanticErrorType::UndefinedIdentifier, node, node.name);
        node.type = Type::Error;
        return;
    }

    node.type = node.symbol->type;
}
