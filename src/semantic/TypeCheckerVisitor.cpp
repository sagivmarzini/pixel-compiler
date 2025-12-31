//
// Created by sagivm on 10/12/2025.
//

#include "TypeCheckerVisitor.h"

#include <set>


#include "parse/AST/Expression.h"
#include "parse/AST/Statement.h"

void TypeCheckerVisitor::run(AstNode& root) {
    root.accept(*this);
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

    throw std::runtime_error(std::format("Cannot promote types: {} and {}", typeToString(leftType),
                                         typeToString(rightType)));
}

void TypeCheckerVisitor::visit(Program& program) {
    for (const auto& stmt: program.statements) {
        stmt->accept(*this);
    }
}

void TypeCheckerVisitor::visit(FunctionDeclaration& node) {
    _currentFunctionReturnType = node.returnType;
    _foundReturn               = false;

    node.body->accept(*this);

    if (!_foundReturn && node.returnType != Type::Void) {
        throw std::runtime_error("Missing return statement in non-void function " + node.name);
    }
}

void TypeCheckerVisitor::visit(ExpressionStatement& node) {
    node.expression->accept(*this);
}

void TypeCheckerVisitor::visit(IfStatement& node) {
    node.condition->accept(*this);

    // Ensure condition returns a boolean
    if (node.condition->type != Type::Boolean) {
        throw std::runtime_error("If condition must be boolean");
    }

    node.thenBranch->accept(*this);

    if (node.elseBranch)
        node.elseBranch->accept(*this);
}

void TypeCheckerVisitor::visit(WhileLoop& node) {
    node.condition->accept(*this);

    // Ensure condition returns a boolean
    if (node.condition->type != Type::Boolean) {
        throw std::runtime_error("While condition must be boolean");
    }

    node.body->accept(*this);
}

void TypeCheckerVisitor::visit(ForLoop& node) {
    node.range->accept(*this);

    if (node.step) {
        node.step->accept(*this);

        if (!isNumeric(node.step->type)) {
            throw std::runtime_error("Loop step must be a numeric type.");
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
        throw std::runtime_error(
            std::format("Range expression requires numeric bounds (start: {}, end: {}).",
                        typeToString(startType),
                        typeToString(endType))
        );
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

        if (node.specifiedType != Type::Unspecified && node.value->type != node.specifiedType)
            throw std::runtime_error("Value doesn't match specified type of variable: " + node.name);

        const auto symbol = node.symbol;
        if (!symbol) throw std::runtime_error("Internal error: undefined variable: " + node.name);

        if (symbol->type == Type::Unspecified)
            symbol->type = node.value->type;
    } else {
        // Parser already checked for typeless variable with no initial value,
        // checking here again just in case
        if (node.specifiedType == Type::Unspecified)
            throw std::runtime_error("Cannot infer variable '" + node.name + "' with no initial value");
    }
}


void TypeCheckerVisitor::visit(FunctionCall& node) {
    const auto calledFunction = _symbolTable.lookup(node.functionName);
    if (!calledFunction)
        throw std::runtime_error("Attempting to call undefined function: " + node.functionName);

    // Check if the number of arguments matches
    if (node.arguments.size() != calledFunction->params.size()) {
        throw std::runtime_error(std::format(
            "Function '{}' expects {} arguments, but {} were provided",
            node.functionName,
            calledFunction->params.size(),
            node.arguments.size()
        ));
    }

    node.type = calledFunction->type;
    std::set<std::string> seenParams;

    for (const auto& argument: node.arguments) {
        // Check for duplicate named arguments
        if (seenParams.contains(argument.name)) {
            throw std::runtime_error(std::format(
                "Duplicate argument '{}' provided in call to function '{}'",
                argument.name,
                node.functionName
            ));
        }
        seenParams.insert(argument.name);

        // Verify the parameter name exists in the function definition
        auto parameter = calledFunction->getParameterByName(argument.name);
        if (!parameter.has_value()) {
            throw std::runtime_error(std::format(
                "Function '{}' has no parameter named '{}'",
                node.functionName,
                argument.name
            ));
        }

        // Visit the argument expression to resolve its type
        argument.value->accept(*this);

        // Compare the expression type to the expected parameter type
        if (argument.value->type != parameter.value().type) {
            throw std::runtime_error(std::format(
                "Type mismatch for argument '{}' in call to '{}': expected '{}', but got '{}'",
                argument.name,
                node.functionName,
                typeToString(parameter.value().type),
                typeToString(argument.value->type)
            ));
        }
    }
}

void TypeCheckerVisitor::visit(BinaryExpression& node) {
    node.left->accept(*this);
    node.right->accept(*this);

    Type leftType  = node.left->type;
    Type rightType = node.right->type;

    switch (node.op) {
        // Arithmetic Operators (+, -, *, /)
        case Operator::Plus:
        case Operator::Minus:
        case Operator::Star:
        case Operator::Slash: {
            if (isNumeric(leftType) && isNumeric(rightType)) {
                // Determine result type based on type promotion (e.g., int + float = float)
                node.type = getPromotedType(leftType, rightType);
            } else if (node.op == Operator::Plus && (isString(leftType) || isString(rightType))) {
                // Check that the OTHER type is one that can be converted to a string (e.g., numeric, or another string).
                if (const Type otherType = isString(leftType) ? rightType : leftType;
                    isString(otherType) || isNumeric(otherType)) {
                    node.type = Type::String;
                } else {
                    throw std::runtime_error(
                        std::format("Cannot concatenate string with type {}", typeToString(otherType))
                    );
                }
            } else {
                throw std::runtime_error(
                    std::format("Undefined {} operation between non-numeric types {} and {}",
                                operatorToString(node.op), typeToString(leftType),
                                typeToString(rightType))
                );
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
                throw std::runtime_error(
                    std::format("Cannot compare types {} and {} using {}",
                                typeToString(leftType), typeToString(rightType),
                                operatorToString(node.op))
                );
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
                throw std::runtime_error(
                    std::format("Logical operator '{}' requires boolean operands, but got {} and {}",
                                operatorToString(node.op), typeToString(leftType),
                                typeToString(rightType))
                );
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
            if (!isNumeric(operandType))
                throw std::runtime_error(std::format("Cannot perform operator {} on type {}", operatorToString(node.op),
                                                     typeToString(operandType)));

            node.type = operandType;

            break;
        }

        case Operator::Exclamation: {
            if (!isBoolean(operandType))
                throw std::runtime_error(std::format("Cannot perform operator {} on non-boolean type {}",
                                                     operatorToString(node.op),
                                                     typeToString(operandType)));
            node.type = Type::Boolean;

            break;
        }

        default:
            throw std::runtime_error(std::format("Internal error: Unhandled unary operator {}",
                                                 operatorToString(node.op)));
    }
}

void TypeCheckerVisitor::visit(VariableAssignment& node) {
    node.assignedValue->accept(*this);
    const Type assignedType = node.assignedValue->type;

    const auto symbol = _symbolTable.lookup(node.varName);
    if (!symbol)
        throw std::runtime_error("Invalid assignment: variable '" + node.varName + "' is undefined");

    if (symbol->isConst)
        throw std::runtime_error("Cannot assign to const variable: " + node.varName);

    if (const Type variableType = symbol->type;
        variableType != assignedType && !isAssignableTo(assignedType, variableType))
        throw std::runtime_error(std::format(
            "Type mismatch in assignment to variable '{}'. Cannot assign value of type {} to variable of type {}",
            node.varName,
            typeToString(assignedType),
            typeToString(variableType)));
}

void TypeCheckerVisitor::visit(ReturnStatement& node) {
    _foundReturn = true;

    node.value->accept(*this);

    if (const auto returnedType = node.value->type;
        !isAssignableTo(_currentFunctionReturnType, returnedType))
        throw std::runtime_error(std::format(
            "Return Type Mismatch: Function expects to return {}, but received {} instead",
            typeToString(_currentFunctionReturnType),
            typeToString(returnedType)));
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
    if (!node.symbol) throw std::runtime_error("Undefined " + node.name);

    node.type = node.symbol->type;
}
