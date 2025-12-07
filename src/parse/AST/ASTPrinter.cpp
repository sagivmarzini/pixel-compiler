#include "ASTPrinter.h"

#include <iostream>

#include "Statement.h"


void ASTPrinter::visit(const Program& program) const {
    std::cout << "program:\n";
    for (auto& node: program.declarations) {
        node->accept(*this);
    }
}

void ASTPrinter::visit(const IntegerLiteralNode& node) const {
    std::cout << node.value;
}

void ASTPrinter::visit(const StringLiteralNode& node) const {
    std::cout << '\"' + node.value + '\"';
}

void ASTPrinter::visit(const BooleanLiteralNode& node) const {
    std::cout << (node.value ? "true" : "false");
}

void ASTPrinter::visit(const BinaryExpression& node) const {
    std::cout << ' ';
    node.left->accept(*this);
    std::cout << ' ' << operatorToString(node.op);
    node.right->accept(*this);
}

void ASTPrinter::visit(const UnaryExpression& node) const {
    std::cout << ' ' << operatorToString(node.op) << ' ';
    node.operand->accept(*this);
}

void ASTPrinter::visit(const IdentifierNode& node) const {
    std::cout << node.name;
}

void ASTPrinter::visit(const FunctionCall& node) const {
    std::cout << node.functionName << '(';
    for (auto& arg: node.arguments) {
        std::cout << arg.name << ": ";
        arg.value->accept(*this);
        std::cout << ", ";
    }
    std::cout << ')';
}

void ASTPrinter::visit(const VariableDeclaration& node) const {
    std::cout << node.name << " : " << typeToString(node.type) << " = ";
    if (node.initializer) {
        node.initializer->accept(*this);
    }
    std::cout << '\n';
}

void ASTPrinter::visit(const VariableAssignment& node) const {
    std::cout << node.name << " = ";
    node.newValue->accept(*this);
    std::cout << '\n';
}

void ASTPrinter::visit(const ReturnStatement& node) const {
    std::cout << "return ";
    node.value->accept(*this);
    std::cout << '\n';
}

void ASTPrinter::visit(const Block& node) const {
    std::cout << "{\n";
    for (auto& statement: node.statements) {
        statement->accept(*this);
    }
    std::cout << "}\n";
}

void ASTPrinter::visit(const WhileLoop& node) const {
    std::cout << "while (";
    node.condition->accept(*this);
    std::cout << ")\n";
    node.body->accept(*this);
}

void ASTPrinter::visit(const IfStatement& node) const {
    std::cout << "if (";
    node.condition->accept(*this);
    std::cout << ")\n";
    node.thenBranch->accept(*this);
    if (node.elseBranch) {
        std::cout << "else\n";
        node.elseBranch->accept(*this);
    }
}

void ASTPrinter::visit(const FunctionDeclaration& node) const {
    std::cout << node.name << '(';
    for (auto& param: node.parameters) {
        std::cout << param.name << ": " << typeToString(param.type) << ", ";
    }
    std::cout << ") -> " << typeToString(node.returnType) << '\n';
    node.body->accept(*this);
}

void ASTPrinter::visit(const ForLoop& node) const {
    std::cout << "it's a for loop...";
}

void ASTPrinter::visit(const ExpressionStatement& node) const {
    node.expression->accept(*this);
}
